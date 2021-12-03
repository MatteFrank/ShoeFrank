/*!
 \file
 \version $Id: TAITntuHit.cxx
 \brief   Container class for ITR raw hit
 */

#include "TString.h"
#include "TClonesArray.h"

#include "TAITntuHit.hxx"

/*!
 \class TAITntuHit
 \brief Container class for ITR raw hit
 */

////////////////////////////////////////////////////////////
// Class Description of TAIThit                        //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////


#include "TAGroot.hxx"
#include "TAITparGeo.hxx"


//##############################################################################

ClassImp(TAITntuHit);

TString TAITntuHit::fgkBranchName   = "itrh.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAITntuHit::TAITntuHit()
: TAGdata(),
  fListOfPixels(0x0),
  fpGeoMap(0x0)
{
   fpGeoMap = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetDefParaName(), "TAITparGeo")->Object();
   if (!fpGeoMap) {
      Error("TAITntuHit()", "Para desciptor vtGeo does not exist");
      exit(0);
   }

   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITntuHit::~TAITntuHit()
{
   delete fListOfPixels;
}

//------------------------------------------+-----------------------------------
//! return number of pixels for a given sensor.
Int_t TAITntuHit::GetPixelsN(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray*list = GetListOfPixels(iSensor);
      return list->GetEntries();
   } else  {
      Error("GetPixelsN()", "Wrong sensor number %d\n", iSensor);
      return -1;
   }
}

//------------------------------------------+-----------------------------------
TClonesArray* TAITntuHit::GetListOfPixels(Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray* list = (TClonesArray*)fListOfPixels->At(iSensor);
      return list;
   } else {
      Error("GetListOfPixels()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
TClonesArray* TAITntuHit::GetListOfPixels(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray* list = (TClonesArray*)fListOfPixels->At(iSensor);
      return list;
   } else {
      Error("GetListOfPixels()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TAIThit* TAITntuHit::GetPixel(Int_t iSensor, Int_t iPixel)
{
   if (iPixel >=0 || iPixel < GetPixelsN(iSensor)) {
      TClonesArray* list = GetListOfPixels(iSensor);
      return (TAIThit*)list->At(iPixel);
   } else {
      Error("GetPixel()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TAIThit* TAITntuHit::GetPixel(Int_t iSensor, Int_t iPixel) const
{
   if (iPixel >=0 || iPixel < GetPixelsN(iSensor)) {
      TClonesArray* list = GetListOfPixels(iSensor);
      return (TAIThit*)list->At(iPixel);
   } else {
      Error("GetPixel()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAITntuHit::SetupClones()
{
   if (fListOfPixels) return;
   fListOfPixels = new TObjArray(fpGeoMap->GetSensorsN());
   
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      TClonesArray* arr = new TClonesArray("TAIThit", 500);
      arr->SetOwner(true);
      fListOfPixels->AddAt(arr, i);
   }
   fListOfPixels->SetOwner(true);
   fMap.clear();
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAITntuHit::Clear(Option_t*)
{
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      TClonesArray* list = GetListOfPixels(i);
      list->Clear("C");
   }
   fMap.clear();
}

//______________________________________________________________________________
//
TAIThit* TAITntuHit::NewPixel(Int_t iSensor, Double_t value, Int_t aLine, Int_t aColumn)
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray &pixelArray = *GetListOfPixels(iSensor);
      std::pair<int, int> idx(aLine, aColumn);
      
      // check if pixel already exists
      if ( fMap[idx] == iSensor+1) {
         TAIThit* pixel = new TAIThit(iSensor, value, aLine, aColumn);
         TAIThit* curPixel = (TAIThit*)pixelArray.FindObject(pixel);
         delete pixel;
         return curPixel;
         
      } else {
         fMap[idx] = iSensor+1;
         TAIThit* pixel = new(pixelArray[pixelArray.GetEntriesFast()]) TAIThit(iSensor, value, aLine, aColumn);
         return pixel;
      }
   } else {
      Error("NewPixel()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}



/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAITntuHit::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      
      os << "TAITntuHit " << GetName()
      << Form("  nPixels=%3d", GetPixelsN(i))
      << endl;
      
      //TODO properly
      //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
      for (Int_t j = 0; j < GetPixelsN(i); j++) {
         const TAIThit*  pixel = GetPixel(i,j);
         if (pixel)
            os << Form("%4d", pixel->GetPixelIndex());
         os << endl;
      }
   }
}




























