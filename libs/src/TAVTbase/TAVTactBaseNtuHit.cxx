/*!
 \file TAVTactBaseNtuHit.cxx
 \brief Base class to decode raw data from single file
 */

#include "DECardEvent.hh"

#include "TAGrecoManager.hxx"
#include "TAGdaqEvent.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparMap.hxx"

#include "TAVTactBaseNtuHit.hxx"

/*!
 \class TAVTactBaseNtuHit
 \brief Base class to decode raw data from single file
 */

//! Class imp
ClassImp(TAVTactBaseNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pParMap mapping parameter descriptor
TAVTactBaseNtuHit::TAVTactBaseNtuHit(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAVTactBaseRaw(name, pNtuRaw, pGeoMap, pConfig, pParMap)
{
   AddDataOut(pNtuRaw, "TAVTntuHit");
   AddPara(pGeoMap, "TAVTparGeo");
   AddPara(pConfig, "TAVTparConf");
   
   TAVTparGeo* parGeo = (TAVTparGeo*) fpGeoMap->Object();
   fSensorsN = parGeo->GetSensorsN();
   
   for (Int_t i = 0; i < fSensorsN; ++i) {
      fPrevEventNumber[i]   = 0;
      fPrevTriggerNumber[i] = 0;
      fPrevTimeStamp[i]     = 0;
   }
   
   Int_t size = parGeo->GetSensorsN()*(sizeof(MI26_FrameRaw)+4);
   fData.resize(size);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseNtuHit::~TAVTactBaseNtuHit()
{   
}

// --------------------------------------------------------------------------------------
//! Find vertex data
Bool_t TAVTactBaseNtuHit::DecodeEvent()
{
   fIndex     = 0;
   MI26_FrameRaw* data = new MI26_FrameRaw;

   TAVTparMap*  pParMap = (TAVTparMap*)  fpParMap->Object();

   // Vertex header
   if (!GetVtxHeader()) return false;
   
   // loop over boards
   for (Int_t i = 0; i < pParMap->GetSensorsN(); ++i) {
      
      Int_t idx     = pParMap->GetSensorId(i);
      Int_t planeId = pParMap->GetPlaneId(idx);

      if (!GetSensorHeader(idx)) return false;
      
      ResetFrames();
      
      // loop over frame (3 max)
      while (GetFrame(idx, data)) {
         DecodeFrame(planeId, data);
      }
      
      fPrevEventNumber[idx]   = fEventNumber;
      fPrevTriggerNumber[idx] = fTriggerNumber;
      fPrevTimeStamp[idx]     = fTimeStamp;
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
Bool_t TAVTactBaseNtuHit::GetVtxHeader()
{
   do {
      if (fData[fIndex] == DECardEvent::GetVertexHeader()) {
         return true;
      }
   } while (fIndex++ < fEventSize);
   
   return false;
}

// --------------------------------------------------------------------------------------
//! Find sensor header
//!
//! \param[in] iSensor sensor index
Bool_t TAVTactBaseNtuHit::GetSensorHeader(Int_t iSensor)
{
   do {
      if (fData[fIndex] == GetSensorKey(iSensor)) {
         fEventNumber   = fData[++fIndex];
         fTriggerNumber = fData[++fIndex];
         fTimeStamp     = fData[++fIndex];
         
         if(FootDebugLevel(3))
            printf("sensor %d: trig: %d evt: %d\n", iSensor, fTriggerNumber, fEventNumber);
         
         if(ValidHistogram())
            FillHistoEvt(iSensor);

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
Bool_t TAVTactBaseNtuHit::GetFrame(Int_t iSensor, MI26_FrameRaw* data)
{
   Int_t startIdx = fIndex;

   // check frame header
   if (fData[++fIndex] ==  GetFrameHeader()) {
      memcpy(data, &fData[fIndex], sizeof(MI26_FrameRaw));
      if (ValidHistogram())
         FillHistoFrame(iSensor, data);
      
   } else
      return false;
 
   // go to frame trailer
   do {
      if (((fData[fIndex] & 0xFFF) == (GetFrameTail() & 0xFFF)) || ((fData[fIndex] >> 16 & 0xFFF) == (GetFrameTail() >>16  & 0xFFF)) ) {
         data->Trailer = fData[fIndex];
         break;
      }
      
      if (fData[fIndex] == GetSensorTail(iSensor)) {
         fIndex--;
         break;
      }
      
   } while (fIndex++ < fEventSize);
   
   fDataSize = fIndex - fgkFrameHeaderSize - startIdx;

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

