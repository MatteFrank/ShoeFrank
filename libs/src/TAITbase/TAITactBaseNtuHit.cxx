/*!
 \file TAITactBaseNtuHit.cxx
 \brief Base class to decode raw data from single file
 */

#include "DEITREvent.hh"

#include "TAGrecoManager.hxx"
#include "TAGdaqEvent.hxx"
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"

#include "TAITactBaseNtuHit.hxx"

/*!
 \class TAITactBaseNtuHit
 \brief Base class to decode raw data from single file
 */

//! Class imp
ClassImp(TAITactBaseNtuHit);

const UInt_t TAITactBaseNtuHit::fgkBoardKey[]  = {0xfafa0110, 0xfafa0111, 0xfafa0112, 0xfafa0113, 0xfafa0114, 0xfafa0115, 0xfafa0116, 0xfafa0117};
const UInt_t TAITactBaseNtuHit::fgkBoardTail[] = {0xabcd0110, 0xabcd0111, 0xabcd0112, 0xabcd0113, 0xabcd0114, 0xabcd0115, 0xabcd0116, 0xabcd0117};


//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pParMap mapping parameter descriptor
TAITactBaseNtuHit::TAITactBaseNtuHit(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAVTactBaseRaw(name, pNtuRaw, pGeoMap, pConfig, pParMap)
{
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
TAITactBaseNtuHit::~TAITactBaseNtuHit()
{   
}

// --------------------------------------------------------------------------------------
//! Find vertex data
Bool_t TAITactBaseNtuHit::DecodeEvent()
{
   fIndex     = 0;
   MI26_FrameRaw* data = new MI26_FrameRaw;

   TAITparMap*  pParMap = (TAITparMap*)  fpParMap->Object();
   
   // IT header
   if (!GetItrHeader()) return false;
   
   for (Int_t l = 0; l < pParMap->GetDataLinksN(); ++l) {
      
      // IT header
      if (!GetBoardHeader(l)) return false;
      
      // loop over sensors
      for (Int_t i = 0; i < pParMap->GetSensorsN(l); ++i) {
         
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
   }
   
   delete data;
   
   
   return true;
}


// private method
// --------------------------------------------------------------------------------------
//! Find vertex header
Bool_t TAITactBaseNtuHit::GetItrHeader()
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
//! \param[in] iBoard board index
Bool_t TAITactBaseNtuHit::GetBoardHeader(Int_t iBoard)
{
   do {
      if (fData[fIndex] == GetBoardKey(iBoard)) {
         fBoardSize  = fData[++fIndex];
         fTimeStamp  = fData[++fIndex];
         
         if(FootDebugLevel(3))
            printf("Board %d: size: %d\n", iBoard, fBoardSize);
         
//         if(ValidHistogram())
//            FillHistoEvt(iSensor);
         
         return true;
      }
   } while (fIndex++ < fEventSize);
   
   
   return false;
}

// --------------------------------------------------------------------------------------
//! Find sensor header
//!
//! \param[in] iSensor sensor index
Bool_t TAITactBaseNtuHit::GetSensorHeader(Int_t iSensor)
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
Bool_t TAITactBaseNtuHit::GetFrame(Int_t iSensor, MI26_FrameRaw* data)
{
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

