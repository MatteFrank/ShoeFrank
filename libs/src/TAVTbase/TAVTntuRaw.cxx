/*!
 \file
 \version $Id: TAVTntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
 \brief   Implementation of TAVTntuRaw.
 */

#include "TString.h"
#include "TClonesArray.h"

#include "TAVTntuRaw.hxx"

/*!
 \class TAVTntuRaw TAVTntuRaw.hxx "TAVTntuRaw.hxx"
 \brief Container class for VTX ntu hit **
 */

////////////////////////////////////////////////////////////
// Class Description of TAVTntuHit                        //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////


#include "TAGroot.hxx"
#include "TAVTparGeo.hxx"


//##############################################################################

ClassImp(TAVTntuRaw);

TString TAVTntuRaw::fgkBranchName   = "vtrh.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTntuRaw::TAVTntuRaw()
: TAGdata(),
  fListOfPixels(0x0),
  fpGeoMap(0x0)
{
   fpGeoMap = (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object();
   if (!fpGeoMap) {
      Error("TAVTntuRaw()", "Para desciptor vtGeo does not exist");
      exit(0);
   }

   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTntuRaw::~TAVTntuRaw()
{
   delete fListOfPixels;
}

//------------------------------------------+-----------------------------------
//! return number of pixels for a given sensor.
Int_t TAVTntuRaw::GetPixelsN(Int_t iSensor) const
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
TClonesArray* TAVTntuRaw::GetListOfPixels(Int_t iSensor)
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
TClonesArray* TAVTntuRaw::GetListOfPixels(Int_t iSensor) const
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
TAVTntuHit* TAVTntuRaw::GetPixel(Int_t iSensor, Int_t iPixel)
{
   if (iPixel >=0 || iPixel < GetPixelsN(iSensor)) {
      TClonesArray* list = GetListOfPixels(iSensor);
      return (TAVTntuHit*)list->At(iPixel);
   } else {
      Error("GetPixel()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TAVTntuHit* TAVTntuRaw::GetPixel(Int_t iSensor, Int_t iPixel) const
{
   if (iPixel >=0 || iPixel < GetPixelsN(iSensor)) {
      TClonesArray* list = GetListOfPixels(iSensor);
      return (TAVTntuHit*)list->At(iPixel);
   } else {
      Error("GetPixel()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAVTntuRaw::SetupClones()
{
   if (fListOfPixels) return;
   fListOfPixels = new TObjArray();
   
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      TClonesArray* arr = new TClonesArray("TAVTntuHit", 500);
      arr->SetOwner(true);
      fListOfPixels->AddAt(arr, i);
   }
   fListOfPixels->SetOwner(true);
   fMap.clear();
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAVTntuRaw::Clear(Option_t*)
{
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      TClonesArray* list = GetListOfPixels(i);
      list->Clear("C");
   }
   fMap.clear();
}

//______________________________________________________________________________
//
TAVTntuHit* TAVTntuRaw::NewPixel(Int_t iSensor, Double_t value, Int_t aLine, Int_t aColumn)
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray &pixelArray = *GetListOfPixels(iSensor);
      std::pair<int, int> idx(aLine, aColumn);
      
      // check if pixel already exists
      if ( fMap[idx] == iSensor+1) {
         TAVTntuHit* pixel = new TAVTntuHit(iSensor, value, aLine, aColumn);
         TAVTntuHit* curPixel = (TAVTntuHit*)pixelArray.FindObject(pixel);
         delete pixel;
         return curPixel;
         
      } else {
         fMap[idx] = iSensor+1;
         TAVTntuHit* pixel = new(pixelArray[pixelArray.GetEntriesFast()]) TAVTntuHit(iSensor, value, aLine, aColumn);
         return pixel;
      }
   } else {
      Error("NewPixel()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}



/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAVTntuRaw::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      
      os << "TAVTntuRaw " << GetName()
      << Form("  nPixels=%3d", GetPixelsN(i))
      << endl;
      
      //TODO properly
      //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
      for (Int_t j = 0; j < GetPixelsN(i); j++) {
         const TAVTntuHit*  pixel = GetPixel(i,j);
         if (pixel)
            os << Form("%4d", pixel->GetPixelIndex());
         os << endl;
      }
   }
}




























