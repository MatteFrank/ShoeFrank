/*!
 \file
 \version $Id: TAITactNtuRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
 \brief   Implementation of TAITactNtuRaw.
 */

#include "TH2F.h"

#include "GlobalPar.hxx"
#include "DECardEvent.hh"
#include "TAGdaqEvent.hxx"
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"

#include "TAITactNtuRaw.hxx"

/*!
 \class TAITactNtuRaw TAITactNtuRaw.hxx "TAITactNtuRaw.hxx"
 \brief Get vertex raw data from MBS. **
 */

//------------------------------------------+-----------------------------------
//! Default constructor.

TAITactNtuRaw::TAITactNtuRaw(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pDatDaq, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAITactBaseRaw(name, pNtuRaw, pGeoMap, pConfig, pParMap),
  fpDatDaq(pDatDaq)
{
   AddDataIn(pDatDaq, "TAGdaqEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactNtuRaw::~TAITactNtuRaw()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAITactNtuRaw::Action()
{
   
   TAGdaqEvent* datDaq = (TAGdaqEvent*)  fpDatDaq->Object();
   
   Int_t nFragments = datDaq->GetFragmentsN();
   
   for (Int_t i = 0; i < nFragments; ++i) {
      
       TString type = datDaq->GetClassType(i);
       if (type.Contains("DECardEvent")) {
         const DECardEvent* evt = static_cast<const DECardEvent*> (datDaq->GetFragment(i));
          DecodeEvent(evt);
       }
   }
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}

// --------------------------------------------------------------------------------------
Bool_t TAITactNtuRaw::DecodeEvent(const DECardEvent* evt)
{
   fData      = evt->values;
   fEventSize = evt->evtSize;
   
   if (fEventSize == 0) return true;

   fIndex     = 0;
   MI26_FrameRaw* data = new MI26_FrameRaw;

   
   TAITparGeo*  pGeoMap = (TAITparGeo*)  fpGeoMap->Object();
   
   // Vertex header
   if (!GetVtxHeader()) return false;
   
   // loop over boards
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      
      if (!GetSensorHeader(i)) return false;
      fFirstFrame = true;
      // loop over frame (3 max)
      while (GetFrame(i, data)) {
         DecodeFrame(i, data);
      }
   }
   
  if(FootDebugLevel(3)) {
      printf("%08x ", fEventSize);
      for (Int_t i = 0; i < (fEventSize)/2; ++i) {
         if (i == 9) {
            printf("\n");
         } else {
            if ((i+1) % 10 == 0) printf("\n");
         }
         printf("%08x ", fData[i]);
      }
      printf("\n");
  }
   
   delete data;

   fPrevEventNumber   = fEventNumber;
   fPrevTriggerNumber = fTriggerNumber;
   fPrevTimeStamp     = fTimeStamp;

   return true;
}


// private method
// --------------------------------------------------------------------------------------
Bool_t TAITactNtuRaw::GetVtxHeader()
{
   do {
      if (fData[fIndex] == DECardEvent::GetVertexHeader()) {
         return true;
      }
   } while (fIndex++ < fEventSize);
   
   return false;
}

// --------------------------------------------------------------------------------------
Bool_t TAITactNtuRaw::GetSensorHeader(Int_t iSensor)
{
   
   do {
      if (fData[fIndex] == GetKeyHeader(iSensor)) {
         fEventNumber   = fData[++fIndex];
         fTriggerNumber = fData[++fIndex];
         fTimeStamp     = fData[++fIndex];
         
         if (ValidHistogram())
            FillHistoEvt(iSensor);

         return true;
      }
   } while (fIndex++ < fEventSize);
   

   return false;
}

// --------------------------------------------------------------------------------------
Bool_t TAITactNtuRaw::GetFrame(Int_t iSensor, MI26_FrameRaw* data)
{
   // check frame header
   if ((fData[++fIndex] & 0xFFF) ==  (GetFrameHeader() & 0xFFF)) { // protection against wrong header !!!
      memcpy(data, &fData[fIndex], sizeof(MI26_FrameRaw));
      if (ValidHistogram())
         FillHistoFrame(iSensor, data);
      
   } else
      return false;
 
   // go to frame trailer
   do {
      if ((fData[fIndex] & 0xFFF) == (GetFrameTail() & 0xFFF)) {
         data->Trailer = fData[fIndex];
         break;
      }
   } while (fIndex++ < fEventSize);
   
   if(FootDebugLevel(3)) {
      printf("%08x\n", data->Header);
      printf("%08x\n", data->TriggerCnt);
      printf("%08x\n", data->TimeStamp);
      printf("%08x\n", data->FrameCnt);
      printf("%08x\n", data->DataLength);
      Int_t dataLength    = ((data->DataLength & 0xFFFF0000)>>16);
      for (Int_t i = 0; i < dataLength; ++i)
         printf("%08x\n", data->ADataW16[i]);
      printf("%08x\n", data->Trailer);
   }
   
   return true;
}

