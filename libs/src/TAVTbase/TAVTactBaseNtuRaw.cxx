/*!
 \file
 \brief   Implementation of TAVTactBaseNtuRaw.
 */

#include "DECardEvent.hh"

#include "GlobalPar.hxx"
#include "TAGdaqEvent.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"

#include "TAVTactBaseNtuRaw.hxx"

/*!
 \class TAVTactBaseNtuRaw TAVTactBaseNtuRaw.hxx "TAVTactBaseNtuRaw.hxx"
 \brief Get vertex raw data from DAQ. **
 */

ClassImp(TAVTactBaseNtuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAVTactBaseNtuRaw::TAVTactBaseNtuRaw(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAVTactBaseRaw(name, pNtuRaw, pGeoMap, pConfig, pParMap)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseNtuRaw::~TAVTactBaseNtuRaw()
{   
}

// --------------------------------------------------------------------------------------
Bool_t TAVTactBaseNtuRaw::DecodeEvent()
{
   fIndex     = 0;
   MI26_FrameRaw* data = new MI26_FrameRaw;

   
   TAVTparGeo*  pGeoMap = (TAVTparGeo*)  fpGeoMap->Object();
   
   // Vertex header
   if (!GetVtxHeader()) return false;
   
   // loop over boards
   for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
      
      if (!GetSensorHeader(i)) return false;
      
      ResetFrames();
      
      // loop over frame (3 max)
      while (GetFrame(i, data)) {
         DecodeFrame(i, data);
      }
      
      fPrevEventNumber[i]   = fEventNumber;
      fPrevTriggerNumber[i] = fTriggerNumber;
      fPrevTimeStamp[i]     = fTimeStamp;
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
Bool_t TAVTactBaseNtuRaw::GetVtxHeader()
{
   do {
      if (fData[fIndex] == DECardEvent::GetVertexHeader()) {
         return true;
      }
   } while (fIndex++ < fEventSize);
   
   return false;
}

// --------------------------------------------------------------------------------------
Bool_t TAVTactBaseNtuRaw::GetSensorHeader(Int_t iSensor)
{
   
   do {
      if (fData[fIndex] == GetKeyHeader(iSensor)) {
         fEventNumber   = fData[++fIndex];
         fTriggerNumber = fData[++fIndex];
         fTimeStamp     = fData[++fIndex];
         
         if(FootDebugLevel(3))
            printf("sensor %d: %d %d\n", iSensor, fTriggerNumber, fEventNumber);
         
         FillHistoEvt(iSensor);

         return true;
      }
   } while (fIndex++ < fEventSize);
   

   return false;
}

// --------------------------------------------------------------------------------------
Bool_t TAVTactBaseNtuRaw::GetFrame(Int_t iSensor, MI26_FrameRaw* data)
{
   // check frame header
   if (fData[++fIndex] ==  GetFrameHeader()) {
      memcpy(data, &fData[fIndex], sizeof(MI26_FrameRaw));
      FillHistoFrame(iSensor, data);
      
   } else
      return false;
 
   // go to frame trailer
   do {
      if (((fData[fIndex] & 0xFFF) == (GetFrameTail() & 0xFFF)) || ((fData[fIndex] >> 16 & 0xFFF) == (GetFrameTail() >>16  & 0xFFF)) ) {
         data->Trailer = fData[fIndex];
         break;
      }
      
      if (fData[fIndex] == GetKeyTail(iSensor)) {
         fIndex--;
         break;
      }
      
   } while (fIndex++ < fEventSize);
   
   fDataSize = fIndex - fgkFrameHeaderSize;

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

