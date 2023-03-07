/*!
 \file TAVTntuHit.cxx
 \brief   Container class for VTX ntu hit
 */

#include "TString.h"
#include "TClonesArray.h"

#include "TAVTntuHit.hxx"

/*!
 \class TAVTntuHit 
 \brief Container class for VTX ntu hit
 */

#include "TAGroot.hxx"
#include "TAVTparGeo.hxx"


//##############################################################################
//! Class Imp
ClassImp(TAVTntuHit);

TString TAVTntuHit::fgkBranchName   = "vtrh.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTntuHit::TAVTntuHit()
: TAGdata(),
  fListOfPixels(0x0),
  fpGeoMap(0x0),
  fValid(true)
{
   fpGeoMap = (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object();
   if (!fpGeoMap) {
      Error("TAVTntuHit()", "Para desciptor vtGeo does not exist");
      exit(0);
   }

   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTntuHit::~TAVTntuHit()
{
   delete fListOfPixels;
}

//------------------------------------------+-----------------------------------
//! return number of pixels for a given sensor.
//!
//! \param[in] iSensor sensor index
Int_t TAVTntuHit::GetPixelsN(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray*list = GetListOfPixels(iSensor);
      return list->GetEntriesFast();
   } else  {
      Error("GetPixelsN()", "Wrong sensor number %d\n", iSensor);
      return -1;
   }
}

//------------------------------------------+-----------------------------------
//! return list of pixels for a given sensor.
//!
//! \param[in] iSensor sensor index
TClonesArray* TAVTntuHit::GetListOfPixels(Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray* list = (TClonesArray*)fListOfPixels->At(iSensor);
      return list;
   } else {
      Error("GetListOfPixels()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return list of pixels for a given sensor (const)
//!
//! \param[in] iSensor sensor index
TClonesArray* TAVTntuHit::GetListOfPixels(Int_t iSensor) const
{
   if (iSensor >= 0  && iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray* list = (TClonesArray*)fListOfPixels->At(iSensor);
      return list;
   } else {
      Error("GetListOfPixels()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor and pixel index
//!
//! \param[in] iSensor sensor index
//! \param[in] iPixel pixel index
TAVThit* TAVTntuHit::GetPixel(Int_t iSensor, Int_t iPixel)
{
   if (iPixel >=0 && iPixel < GetPixelsN(iSensor)) {
      TClonesArray* list = GetListOfPixels(iSensor);
      return (TAVThit*)list->At(iPixel);
   } else {
      Error("GetPixel()", "Wrong sensor %d or pixel %d\n", iSensor, iPixel);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor and pixel index (const)
//!
//! \param[in] iSensor sensor index
//! \param[in] iPixel pixel index
const TAVThit* TAVTntuHit::GetPixel(Int_t iSensor, Int_t iPixel) const
{
   if (iPixel >=0 && iPixel < GetPixelsN(iSensor)) {
      TClonesArray* list = GetListOfPixels(iSensor);
      return (TAVThit*)list->At(iPixel);
   } else {
      Error("GetPixel()", "Wrong sensor %d or pixel %d\n", iSensor, iPixel);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAVTntuHit::SetupClones()
{
   if (fListOfPixels) return;
   fListOfPixels = new TObjArray();
   
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      TClonesArray* arr = new TClonesArray("TAVThit", 500);
      arr->SetOwner(true);
      fListOfPixels->AddAt(arr, i);
   }
   fListOfPixels->SetOwner(true);
   fMap.clear();
}

//------------------------------------------+-----------------------------------
//! Clear event.
//!
//! \param[in] opt option for clearing (not used)
void TAVTntuHit::Clear(Option_t*)
{
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      TClonesArray* list = GetListOfPixels(i);
      list->Clear("C");
   }
   fMap.clear();
}

//______________________________________________________________________________
//! Create new pixel from a given sensor, line and column
//!
//! \param[in] iSensor sensor index
//! \param[in] value pixel value
//! \param[in] aLine line number
//! \param[in] aColumn column number
TAVThit* TAVTntuHit::NewPixel(Int_t iSensor, Double_t value, Int_t aLine, Int_t aColumn, Int_t iFrame)
{
   if (iSensor >= 0  && iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray &pixelArray = *GetListOfPixels(iSensor);
      std::pair<int, int> idx(aLine, aColumn);
      
      // check if pixel already exists
      if ( fMap[idx] == iSensor+1) {
         TAVThit* pixel = new TAVThit(iSensor, value, aLine, aColumn);
         TAVThit* curPixel = (TAVThit*)pixelArray.FindObject(pixel);
         curPixel->AddFrameOn(iFrame);
         delete pixel;
         return curPixel;
         
      } else {
         fMap[idx] = iSensor+1;
         TAVThit* pixel = new(pixelArray[pixelArray.GetEntriesFast()]) TAVThit(iSensor, value, aLine, aColumn);
         pixel->AddFrameOn(iFrame);
         return pixel;
      }
   } else {
      Error("NewPixel()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAVTntuHit::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      
      os << "TAVTntuHit " << GetName()
      << Form("  nPixels=%3d", GetPixelsN(i))
      << endl;
      
      //TODO properly
      //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
      for (Int_t j = 0; j < GetPixelsN(i); j++) {
         const TAVThit*  pixel = GetPixel(i,j);
         if (pixel)
            os << Form("%4d", pixel->GetPixelIndex());
         os << endl;
      }
   }
}
