/*!
 \file
 \version $Id: TAMSDntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
 \brief   Implementation of TAMSDntuRaw.
 */

#include "TString.h"
#include "TClonesArray.h"

#include "TAMSDntuRaw.hxx"

/*!
 \class TAMSDntuRaw TAMSDntuRaw.hxx "TAMSDntuRaw.hxx"
 \brief Container class for VTX ntu hit **
 */

////////////////////////////////////////////////////////////
// Class Description of TAMSDntuHit                        //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////


#include "TAGroot.hxx"
#include "TAVTparGeo.hxx"


//##############################################################################

ClassImp(TAMSDntuRaw);

TString TAMSDntuRaw::fgkBranchName   = "msdrh.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMSDntuRaw::TAMSDntuRaw()
: TAGdata(),
  fListOfStrips(0x0),
  fpGeoMap(0x0)
{
   fpGeoMap = (TAMSDparGeo*) gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object();
   if (!fpGeoMap) {
      Error("TAMSDntuRaw()", "Para desciptor %s does not exist", TAMSDparGeo::GetDefParaName());
      exit(0);
   }

   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDntuRaw::~TAMSDntuRaw()
{
   delete fListOfStrips;
}

//------------------------------------------+-----------------------------------
//! return number of strips for a given sensor.
Int_t TAMSDntuRaw::GetStripsN(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray* list = GetListOfStrips(iSensor);
      return list->GetEntries();
   } else  {
      Error("GetStripsN()", "Wrong sensor number %d\n", iSensor);
      return -1;
   }
}

//------------------------------------------+-----------------------------------
TClonesArray* TAMSDntuRaw::GetListOfStrips(Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray* list = (TClonesArray*)fListOfStrips->At(iSensor);
      return list;
   } else {
      Error("GetListOfStrips()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
TClonesArray* TAMSDntuRaw::GetListOfStrips(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray* list = (TClonesArray*)fListOfStrips->At(iSensor);
      return list;
   } else {
      Error("GetListOfStrips()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a strip for a given sensor
TAMSDntuHit* TAMSDntuRaw::GetStrip(Int_t iSensor, Int_t iStrip)
{
   if (iStrip >=0 || iStrip < GetStripsN(iSensor)) {
      TClonesArray* list = GetListOfStrips(iSensor);
      return (TAMSDntuHit*)list->At(iStrip);
   } else {
      Error("GetStrip()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a strip for a given sensor
const TAMSDntuHit* TAMSDntuRaw::GetStrip(Int_t iSensor, Int_t iStrip) const
{
   if (iStrip >=0 || iStrip < GetStripsN(iSensor)) {
      TClonesArray* list = GetListOfStrips(iSensor);
      return (TAMSDntuHit*)list->At(iStrip);
   } else {
      Error("GetStrip()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAMSDntuRaw::SetupClones()
{
   if (fListOfStrips) return;
   fListOfStrips = new TObjArray();
   
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      TClonesArray* arr = new TClonesArray("TAMSDntuHit", 500);
      arr->SetOwner(true);
      fListOfStrips->AddAt(arr, i);
   }
   fListOfStrips->SetOwner(true);
   fMap.clear();
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAMSDntuRaw::Clear(Option_t*)
{
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      TClonesArray* list = GetListOfStrips(i);
      list->Clear("C");
   }
   fMap.clear();
}

//______________________________________________________________________________
//
TAMSDntuHit* TAMSDntuRaw::NewStrip(Int_t iSensor,  Int_t aView, Int_t aStrip)
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN()) {
      TClonesArray &stripArray = *GetListOfStrips(iSensor);
      std::pair<int, int> idx(aView, aStrip);
      
      // check if strip already exists
      if ( fMap[idx] == iSensor+1) {
         TAMSDntuHit* strip = new TAMSDntuHit(iSensor, aView, aStrip);
         TAMSDntuHit* curStrip = (TAMSDntuHit*)stripArray.FindObject(strip);
         delete strip;
         return curStrip;
         
      } else {
         fMap[idx] = iSensor+1;
         TAMSDntuHit* strip = new(stripArray[stripArray.GetEntriesFast()]) TAMSDntuHit(iSensor, aView, aStrip);
         return strip;
      }
   } else {
      Error("NewStrip()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAMSDntuRaw::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
      
      os << "TAMSDntuRaw " << GetName()
      << Form("  nPixels=%3d", GetStripsN(i))
      << endl;
      
      //TODO properly
      //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
      for (Int_t j = 0; j < GetStripsN(i); j++) {
         const TAMSDntuHit*  strip = GetStrip(i,j);
         if (strip)
            os << Form("%4d", strip->GetStrip());
         os << endl;
      }
   }
}

