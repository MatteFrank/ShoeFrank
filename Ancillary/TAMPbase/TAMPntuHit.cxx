/*!
 \file TAMPntuHit.cxx
 \brief   Container class for ITR raw hit
 */

#include "TString.h"
#include "TClonesArray.h"

#include "TAGnameManager.hxx"

#include "TAMPntuHit.hxx"

/*!
 \class TAMPntuHit
 \brief Container class for ITR raw hit
 */

////////////////////////////////////////////////////////////
// Class Description of TAMPhit                        //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////


#include "TAGroot.hxx"
#include "TAMPparGeo.hxx"


//##############################################################################
//! Class Imp
ClassImp(TAMPntuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMPntuHit::TAMPntuHit()
: TAGdata(),
  fListOfPixels(0x0),
  fpGeoMap(0x0)
{
   fpGeoMap = (TAMPparGeo*) gTAGroot->FindParaDsc("mpGeo", "TAMPparGeo")->Object();
   if (!fpGeoMap) {
      Error("TAMPntuHit()", "Para desciptor vtGeo does not exist");
      exit(0);
   }

   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMPntuHit::~TAMPntuHit()
{
   delete fListOfPixels;
}

//------------------------------------------+-----------------------------------
//! return number of pixels for a given sensor.
//!
//! \param[in] iSensor sensor index
Int_t TAMPntuHit::GetPixelsN(Int_t iSensor) const
{
   if (iSensor >= 0  && iSensor < fpGeoMap->GetSensorsN()) {
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
TClonesArray* TAMPntuHit::GetListOfPixels(Int_t iSensor)
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
TClonesArray* TAMPntuHit::GetListOfPixels(Int_t iSensor) const
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
TAMPhit* TAMPntuHit::GetPixel(Int_t iSensor, Int_t iPixel)
{
   if (iPixel >=0 && iPixel < GetPixelsN(iSensor)) {
      TClonesArray* list = GetListOfPixels(iSensor);
      return (TAMPhit*)list->At(iPixel);
   } else {
      Error("GetPixel()", "Wrong sensor (%d) or pixel (%d)\n", iSensor, iPixel);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor and pixel index (const)
//!
//! \param[in] iSensor sensor index
//! \param[in] iPixel pixel index
const TAMPhit* TAMPntuHit::GetPixel(Int_t iSensor, Int_t iPixel) const
{
   if (iPixel >=0 && iPixel < GetPixelsN(iSensor)) {
      TClonesArray* list = GetListOfPixels(iSensor);
      return (TAMPhit*)list->At(iPixel);
   } else {
      Error("GetPixel()", "Wrong sensor (%d) or pixel (%d)\n", iSensor, iPixel);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAMPntuHit::SetupClones()
{
   if (fListOfPixels) return;
   fListOfPixels = new TObjArray(fpGeoMap->GetSensorsN());
   
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      TClonesArray* arr = new TClonesArray("TAMPhit", 500);
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
void TAMPntuHit::Clear(Option_t*)
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
TAMPhit* TAMPntuHit::NewPixel(Int_t iSensor, Double_t value, Int_t aLine, Int_t aColumn)
{
   if (iSensor >= 0 && iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray &pixelArray = *GetListOfPixels(iSensor);
      std::pair<int, int> idx(aLine, aColumn);
      
      // check if pixel already exists
      if ( fMap[idx] == iSensor+1) {
         TAMPhit* pixel = new TAMPhit(iSensor, value, aLine, aColumn);
         TAMPhit* curPixel = (TAMPhit*)pixelArray.FindObject(pixel);
         delete pixel;
         return curPixel;
         
      } else {
         fMap[idx] = iSensor+1;
         TAMPhit* pixel = new(pixelArray[pixelArray.GetEntriesFast()]) TAMPhit(iSensor, value, aLine, aColumn);
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
void TAMPntuHit::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      
      os << "TAMPntuHit " << GetName()
      << Form("  nPixels=%3d", GetPixelsN(i))
      << endl;
      
      //TODO properly
      //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
      for (Int_t j = 0; j < GetPixelsN(i); j++) {
         const TAMPhit*  pixel = GetPixel(i,j);
         if (pixel)
            os << Form("%4d", pixel->GetPixelIndex());
         os << endl;
      }
   }
}




























