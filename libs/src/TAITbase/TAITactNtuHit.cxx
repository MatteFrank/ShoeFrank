/*!
 \file TAITactNtuHit.cxx
 \brief   Ntuplizer for ITR raw data
 */

#include "TH2F.h"

#include "DEITREvent.hh"
#include "DAQMarkers.hh"

#include "TAGrecoManager.hxx"
#include "TAGdaqEvent.hxx"
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"

#include "TAITactNtuHit.hxx"

/*!
 \class TAITactNtuHit
 \brief Ntuplizer for ITR raw data
 */

//! Class Imp
ClassImp(TAITactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[out] pNtuRaw hit container descriptor
//! \param[in] pDatDaq daq event container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pParMap mapping parameter descriptor
TAITactNtuHit::TAITactNtuHit(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pDatDaq, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAITactBaseRaw(name, pNtuRaw, pGeoMap, pConfig, pParMap),
  fpDatDaq(pDatDaq)
{
   AddDataIn(pDatDaq, "TAGdaqEvent");
   AddDataOut(pNtuRaw, "TAITntuHit");
   AddPara(pGeoMap, "TAITparGeo");
   AddPara(pConfig, "TAITparConf");
   
   TAITparGeo* parGeo = (TAITparGeo*) fpGeoMap->Object();
   fNSensors = parGeo->GetSensorsN();
   
   for (Int_t i = 0; i < fNSensors; ++i) {
      fPrevEventNumber[i]   = 0;
      fPrevTriggerNumber[i] = 0;
      fPrevTimeStamp[i]     = 0;
   }
   
   Int_t size = parGeo->GetSensorsN()*sizeof(MI26_FrameRaw)*4;
   fData.resize(size);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactNtuHit::~TAITactNtuHit()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAITactNtuHit::Action()
{
   
   TAGdaqEvent* datDaq = (TAGdaqEvent*)  fpDatDaq->Object();
   
   Int_t nFragments = datDaq->GetFragmentsN();
   
   for (Int_t i = 0; i < nFragments; ++i) {
      
       TString type = datDaq->GetClassType(i);
       if (type.Contains("DEITREvent")) {
          const DEITREvent* evt = static_cast<const DEITREvent*> (datDaq->GetFragment(i));
          fData      = evt->values;
          fEventSize = evt->evtSize;
          fDataLink  = evt->channelID - (dataVTX | 0x30);
          if (fEventSize == 0) continue;
          DecodeEvent();
       }
   }
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}

// --------------------------------------------------------------------------------------
//! Find vertex data
Bool_t TAITactNtuHit::DecodeEvent()
{
   MI26_FrameRaw* data = new MI26_FrameRaw;
   TAITparMap* pParMap = (TAITparMap*)  fpParMap->Object();
   TAITparGeo* pGeoMap = (TAITparGeo*)  fpGeoMap->Object();
   fIndex = 0;

   // Vertex header
   if (!GetVtxHeader()) return false;
   
   // loop over boards
   for (Int_t i = 0; i < pGeoMap->GetSensPerDataLink(); ++i) {
      
      if (!GetSensorHeader(i)) return false;
      fFirstFrame = true;
      // loop over frame (3 max)
      while (GetFrame(i, data)) {
         DecodeFrame(i, data);
      }
      
      Int_t planeId = pParMap->GetPlaneId(i, fDataLink);

      fPrevEventNumber[planeId]   = fEventNumber;
      fPrevTriggerNumber[planeId] = fTriggerNumber;
      fPrevTimeStamp[planeId]     = fTimeStamp;
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

   return true;
}


// private method
// --------------------------------------------------------------------------------------
//! Find vertex header
Bool_t TAITactNtuHit::GetVtxHeader()
{
   do {
      if (fData[fIndex] == DEITREvent::GetItrHeader()) {
         return true;
      }
   } while (fIndex++ < fEventSize);
   
   return false;
}

// --------------------------------------------------------------------------------------
//! Find sensor header
//!
//! \param[in] iSensor sensor index
Bool_t TAITactNtuHit::GetSensorHeader(Int_t iSensor)
{
   do {
      if (fData[fIndex] == GetKeyHeader(iSensor)) {
         fEventNumber   = fData[++fIndex];
         fTriggerNumber = fData[++fIndex];
         fTimeStamp     = fData[++fIndex];
         
         if (ValidHistogram()) {
            TAITparMap* pParMap = (TAITparMap*) fpParMap->Object();
            Int_t planeId = pParMap->GetPlaneId(iSensor, fDataLink);
            FillHistoEvt(planeId);
         }
         return true;
      }
   } while (fIndex++ < fEventSize);
   

   return false;
}

// --------------------------------------------------------------------------------------
//! Get Frame structure
//!
//! \param[in] iSensor sensor index
//! \param[in] data Mimosa sensor data structure
Bool_t TAITactNtuHit::GetFrame(Int_t iSensor, MI26_FrameRaw* data)
{
   // check frame header
   if ((fData[++fIndex] & 0xFFF) ==  (GetFrameHeader() & 0xFFF)) { // protection against wrong header !!!
      memcpy(data, &fData[fIndex], sizeof(MI26_FrameRaw));
      
      if (ValidHistogram()) {
         TAITparMap* pParMap = (TAITparMap*) fpParMap->Object();
         Int_t planeId = pParMap->GetPlaneId(iSensor, fDataLink);
         FillHistoFrame(planeId, data);
      }
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

