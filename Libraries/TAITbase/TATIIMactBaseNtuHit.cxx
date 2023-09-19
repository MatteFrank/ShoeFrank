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
const UInt_t TATIIMactBaseNtuHit::fgkEventTail   =  0xdeadbeaf;


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
   UInt_t aCol  = 0;
   UInt_t aLine = 0;
   UInt_t value = 0;
   UInt_t tmp   = 0;

   TAITparMap*  pParMap = (TAITparMap*)  fpParMap->Object();
   
   // loop over sensors
   for (Int_t i = 0; i < pParMap->GetSensorsN(); ++i) {
      fIndex = 0;

      do {
         if (fData[fIndex] == fgkEventKey) {
            break;
         }
      } while (fIndex++ < fEventSize);
      
      fIndex++;
      
      do {
         tmp  = fData[fIndex++];
         tmp  = fData[fIndex++];
         fTriggerNumber  = fData[fIndex++];
         aLine  = fData[fIndex++];
         aCol = fData[fIndex++];
         value = fData[fIndex++];
         AddPixel(i, value, aLine, aCol);
         
        if(FootDebugLevel(3))
            printf("trg:# %u line: %u col: %u value: %u\n", fTriggerNumber, aLine, aCol, value);
      } while (fIndex < fEventSize);
      
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
void TATIIMactBaseNtuHit::AddPixel( Int_t iSensor, Int_t value, Int_t aLine, Int_t aColumn, Int_t /*frameNumber*/)
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
      FillHistoPixel(iSensor, aLine, aColumn, value);
}
