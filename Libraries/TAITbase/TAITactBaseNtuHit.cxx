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
#include "TAITntuHit.hxx"
#include "TAIThit.hxx"

#include "TAITactBaseNtuHit.hxx"

/*!
 \class TAITactBaseNtuHit
 \brief Base class to decode raw data from single file
 */

//! Class imp
ClassImp(TAITactBaseNtuHit);

const UInt_t TAITactBaseNtuHit::fgkDataLinkKey[]  = {0xfafa0110, 0xfafa0111, 0xfafa0112, 0xfafa0113, 0xfafa0114, 0xfafa0115, 0xfafa0116, 0xfafa0117};
const UInt_t TAITactBaseNtuHit::fgkDataLinkTail[] = {0xabcd0110, 0xabcd0111, 0xabcd0112, 0xabcd0113, 0xabcd0114, 0xabcd0115, 0xabcd0116, 0xabcd0117};


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
   
   for (Int_t i = 0; i < parGeo->GetSensorsN(); ++i) {
      fPrevEventNumber[i]   = 0;
      fPrevTriggerNumber[i] = 0;
      fPrevTimeStamp[i]     = 0;
   }
   
   Int_t size = (((sizeof(MI26_FrameRaw)/4)*3 + 4)*parGeo->GetSensorsN())+7; // 3 frame per event and 7 header/trailer words for each sensor/board
   fData.reserve(size);
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
   
  do {
      // IT board
      Int_t l = GetDataLinkHeader();
      
     if (l == -1) return false;
     
      // loop over sensors
     for (Int_t i = 0; i < pParMap->GetSensorsN(l); ++i) {

        if (!GetSensorHeader(i, l)) continue;
        Int_t planeId = pParMap->GetPlaneId(i, l);
        if (planeId == -1) continue;

        ResetFrames();
        
        Int_t framenumber = 0;
        // loop over frame (3 max)
        while (GetFrame(i, l, data)) {
           DecodeFrame(planeId, data, framenumber);
           framenumber++;
        }
        
        fPrevEventNumber[planeId]   = fEventNumber;
        fPrevTriggerNumber[planeId] = fTriggerNumber;
        fPrevTimeStamp[planeId]     = fTimeStamp;
     }
  }  while (fIndex++ < fEventSize);

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
//! \param[in] iDataLink board index
Int_t TAITactBaseNtuHit::GetDataLinkHeader()
{
   TAITparMap*  pParMap = (TAITparMap*)  fpParMap->Object();

   do {
      Int_t dataLink = -1;
      
      for (Int_t i = 0; i < pParMap->GetDataLinksN(); ++i) {
         if (fData[fIndex] == GetDataLinkKey(i)) {
            dataLink = i;
            break;
         }
      }
      
      if (fData[fIndex] == GetDataLinkKey(dataLink)) {
         fIndex++;
         fDataLinkTrigger = fData[++fIndex];
         
         if(FootDebugLevel(3))
            printf("DataLink %d (0x%x): trig#: %d\n", dataLink, fData[fIndex-2], fDataLinkTrigger);
         
         return dataLink;
      }
   } while (fIndex++ < fEventSize);
   
   return -1;
}

// --------------------------------------------------------------------------------------
//! Find sensor header
//!
//! \param[in] iSensor sensor index
//! \param[in] datalink board index
Bool_t TAITactBaseNtuHit::GetSensorHeader(Int_t sensor, Int_t datalink)
{
   TAITparMap*  pParMap = (TAITparMap*)  fpParMap->Object();

   do {
      if (fData[fIndex] == GetSensorKey(sensor)) {
         fEventNumber   = fData[++fIndex];
         fTriggerNumber = fData[++fIndex];
         fTimeStamp     = fData[++fIndex];
         
         if(FootDebugLevel(3))
            printf("datalink: %d sensor: %d trig#: %d evt#: %d\n", datalink, sensor, fTriggerNumber, fEventNumber);
         
         if(ValidHistogram()) {
            Int_t planeId = pParMap->GetPlaneId(sensor, datalink);
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
//! \param[in] datalink board index
//! \param[in] data Mimosa sensor data structure
Bool_t TAITactBaseNtuHit::GetFrame(Int_t iSensor, Int_t datalink, MI26_FrameRaw* data)
{
   TAITparMap*  pParMap = (TAITparMap*)  fpParMap->Object();

   Int_t startIdx = fIndex;
   // check frame header
   if (fData[++fIndex] ==  GetFrameHeader()) {

      memcpy(data, &fData[fIndex], sizeof(MI26_FrameRaw));
      if (ValidHistogram()) {
         Int_t planeId = pParMap->GetPlaneId(iSensor, datalink);
         FillHistoFrame(planeId, data);
      }
      
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
      printf("Sensor %d datalink %d\n", iSensor, datalink);
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

// --------------------------------------------------------------------------------------
//! Add pixel to container
//!
//! \param[in] iSensor sensor index
//! \param[in] value pixel value
//! \param[in] aLine line id
//! \param[in] aColumn column id
void TAITactBaseNtuHit::AddPixel( Int_t iSensor, Int_t value, Int_t aLine, Int_t aColumn, Int_t frameNumber)
{
   // Add a pixel to the vector of pixels
   // require the following info
   // - input = number of the sensors
   // - value = analog value of this pixel
   // - line & column = position of the pixel in the matrix
   
   TAITntuHit*  pNtuRaw = (TAITntuHit*)  fpNtuRaw->Object();
   TAITparGeo*  pGeoMap = (TAITparGeo*)  fpGeoMap->Object();
   TAITparConf* pConfig = (TAITparConf*) fpConfig->Object();
   
   if (pConfig->IsDeadPixel(iSensor, aLine, aColumn)) return;
   
   TAIThit* pixel   = (TAIThit*)pNtuRaw->NewPixel(iSensor, value, aLine, aColumn, frameNumber);
   
   double v = pGeoMap->GetPositionV(aLine);
   double u = pGeoMap->GetPositionU(aColumn);
   TVector3 pos(u,v,0);
   pixel->SetPosition(pos);
   pixel->SetValidFrames(fFrameOk);
   
   if (ValidHistogram())
      FillHistoPixel(iSensor, aLine, aColumn, value);
}
