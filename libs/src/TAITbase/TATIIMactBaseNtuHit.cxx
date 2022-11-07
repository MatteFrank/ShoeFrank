/*!
 \file TATIIMactBaseNtuHit.cxx
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

#include "TATIIMactBaseNtuHit.hxx"

/*!
 \class TATIIMactBaseNtuHit
 \brief Base class to decode raw data from single file
 */

//! Class imp
ClassImp(TATIIMactBaseNtuHit);

const UInt_t TATIIMactBaseNtuHit::fgkBoardKey[]  = {0xfafa0110, 0xfafa0111, 0xfafa0112, 0xfafa0113, 0xfafa0114, 0xfafa0115, 0xfafa0116, 0xfafa0117};
const UInt_t TATIIMactBaseNtuHit::fgkBoardTail[] = {0xabcd0110, 0xabcd0111, 0xabcd0112, 0xabcd0113, 0xabcd0114, 0xabcd0115, 0xabcd0116, 0xabcd0117};
const UInt_t TATIIMactBaseNtuHit::fgkEventKey    =  0x80088007;
const UInt_t TATIIMactBaseNtuHit::fgkEventTail   =  0xaaa8aaa7;


//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pParMap mapping parameter descriptor
TATIIMactBaseNtuHit::TATIIMactBaseNtuHit(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
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
   
   Int_t size = parGeo->GetPixelsNx()*parGeo->GetPixelsNy() + 6;
   fData.reserve(size);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TATIIMactBaseNtuHit::~TATIIMactBaseNtuHit()
{   
}

// --------------------------------------------------------------------------------------
//! Find vertex data
Bool_t TATIIMactBaseNtuHit::DecodeEvent()
{
   fIndex     = 0;
   TAITparMap*  pParMap = (TAITparMap*)  fpParMap->Object();
   
   // event header
   if (!GetEvtHeader()) return false;
   
   for (Int_t l = 0; l < pParMap->GetDataLinksN(); ++l) {
      
      if(FootDebugLevel(1))
         printf("\nlink# %d  sensors %d\n", l, pParMap->GetSensorsN(l));
      
      // loop over sensors
      for (Int_t i = 0; i < pParMap->GetSensorsN(l); ++i) {
         Int_t planeId = pParMap->GetPlaneId(i, l);

         if (!GetSensorHeader(i, l)) continue;

         ResetFrames();

         // loop over frame (3 max)
         while (GetFrame(i,l)) {
            DecodeFrame(planeId);
         }
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
   }
   
   
   
   return true;
}


// private method
// --------------------------------------------------------------------------------------
//! Find vertex header
Bool_t TATIIMactBaseNtuHit::GetEvtHeader()
{
   do {
      if (fData[fIndex] == fgkEventKey) {
         fTriggerNumber = fData[++fIndex];
         fTimeStamp     = fData[++fIndex];
         
         if(FootDebugLevel(3))
            printf("Trig#: %d evt#: %d\n", fTriggerNumber, fEventNumber);
         
         return true;
      }
   } while (fIndex++ < fEventSize);
   
   

   return false;
}

// --------------------------------------------------------------------------------------
//! Find sensor header
//!
//! \param[in] iSensor sensor index
//! \param[in] datalink board index
Bool_t TATIIMactBaseNtuHit::GetSensorHeader(Int_t iSensor, Int_t datalink)
{
   TAITparMap*  pParMap = (TAITparMap*)  fpParMap->Object();
   do {
      if (fData[fIndex] == GetSensorKey(iSensor)) {

         if(ValidHistogram()) {
            Int_t planeId = pParMap->GetPlaneId(iSensor, datalink);
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
Bool_t TATIIMactBaseNtuHit::GetFrame(Int_t iSensor, Int_t datalink, MI26_FrameRaw* )
{
   TAITparGeo* parGeo = (TAITparGeo*) fpGeoMap->Object();
   
   
   Int_t size = parGeo->GetPixelsNx()*parGeo->GetPixelsNy() + 6;

   Int_t startIdx = fIndex;
   
   for (Int_t i = 0; i <  parGeo->GetPixelsNx(); ++i) {
      for (Int_t j = 0; j < parGeo->GetPixelsNy() ; ++j) {
         fIndex = startIdx + i* parGeo->GetPixelsNx() + j;
         if (fIndex >= fEventSize) break;
         Int_t value = fData[fIndex];
         if (value > 0)
            AddPixel(iSensor, value, i, j);
      }
   }
   
 
   // go to sensor trailer
   do {
      if (fData[fIndex] == GetSensorHeader(iSensor, datalink) ) {
         break;
      }
      
   } while (fIndex++ < fEventSize);
   
   fDataSize = fIndex - startIdx;
   
   return true;
}

// --------------------------------------------------------------------------------------
//! Add pixel to container
//!
//! \param[in] iSensor sensor index
//! \param[in] value pixel value
//! \param[in] aLine line id
//! \param[in] aColumn column id
void TATIIMactBaseNtuHit::AddPixel( Int_t iSensor, Int_t value, Int_t aLine, Int_t aColumn)
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
   
   TAIThit* pixel   = (TAIThit*)pNtuRaw->NewPixel(iSensor, value, aLine, aColumn);
   
   double v = pGeoMap->GetPositionV(aLine);
   double u = pGeoMap->GetPositionU(aColumn);
   TVector3 pos(u,v,0);
   pixel->SetPosition(pos);
   pixel->SetValidFrames(fFrameOk);
   
   if (ValidHistogram())
      FillHistoPixel(iSensor, aLine, aColumn);
}
