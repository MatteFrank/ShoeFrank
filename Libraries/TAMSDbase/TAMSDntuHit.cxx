/*!
 \file TAMSDntuHit.cxx
 \brief   Implementation of TAMSDntuHit.
 */

#include "TString.h"
#include "TClonesArray.h"

#include "TAMSDntuHit.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TAMSDntuHit 
 \brief Container class for VTX ntu hit **
 */

////////////////////////////////////////////////////////////
// Class Description of TAMSDhit                        //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAGnameManager.hxx"
#include "TAGroot.hxx"
#include "TAVTparGeo.hxx"


//##############################################################################

//! Class Imp
ClassImp(TAMSDntuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMSDntuHit::TAMSDntuHit(Int_t sensorsN)
 : TAGdata(),
   fSensorsN(sensorsN),
   fListOfStrips(0x0)
{
   if (sensorsN == 0) {
      Warning("TAMSDntuHit()", "Size of hit array not set, set to %d\n", TAMSDparGeo::GetDefSensorsN());
      fSensorsN = TAMSDparGeo::GetDefSensorsN();
   }

   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDntuHit::~TAMSDntuHit()
{
   delete fListOfStrips;
}

//------------------------------------------+-----------------------------------
//! return number of strips for a given sensor.
//!
//! \param[in] iSensor sensor id
Int_t TAMSDntuHit::GetStripsN(Int_t iSensor) const
{
   if (iSensor >= 0  && iSensor < fSensorsN) {
      TClonesArray* list = GetListOfStrips(iSensor);
      return list->GetEntriesFast();
   } else  {
      Error("GetStripsN()", "Wrong sensor number %d\n", iSensor);
      return -1;
   }
}

//------------------------------------------+-----------------------------------
//! Get list of strips
//!
//! \param[in] iSensor sensor id
TClonesArray* TAMSDntuHit::GetListOfStrips(Int_t iSensor)
{
   if (iSensor >= 0 && iSensor < fSensorsN) {
      TClonesArray* list = (TClonesArray*)fListOfStrips->At(iSensor);
      return list;
   } else {
      Error("GetListOfStrips()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! Get list of strips (const)
//!
//! \param[in] iSensor sensor id
TClonesArray* TAMSDntuHit::GetListOfStrips(Int_t iSensor) const
{
   if (iSensor >= 0 && iSensor < fSensorsN) {
      TClonesArray* list = (TClonesArray*)fListOfStrips->At(iSensor);
      return list;
   } else {
      Error("GetListOfStrips()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}


//------------------------------------------+-----------------------------------
//! return a strip for a given sensor
//!
//! \param[in] iSensor sensor id
//! \param[in] iStrip strip id
TAMSDhit* TAMSDntuHit::GetStrip(Int_t iSensor, Int_t iStrip)
{
   if (iStrip >=0 && iStrip < GetStripsN(iSensor)) {
      TClonesArray* list = GetListOfStrips(iSensor);
      return (TAMSDhit*)list->At(iStrip);
   } else {
      Error("GetStrip()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! return a strip for a given sensor (const)
//!
//! \param[in] iSensor sensor id
//! \param[in] iStrip strip id
const TAMSDhit* TAMSDntuHit::GetStrip(Int_t iSensor, Int_t iStrip) const
{
   if (iStrip >=0 && iStrip < GetStripsN(iSensor)) {
      TClonesArray* list = GetListOfStrips(iSensor);
      return (TAMSDhit*)list->At(iStrip);
   } else {
      Error("GetStrip()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}


//------------------------------------------+-----------------------------------
//! Setup clones.
void TAMSDntuHit::SetupClones()
{
   if (fListOfStrips) return;
   fListOfStrips = new TObjArray(fSensorsN);
   
   for (Int_t i = 0; i < fSensorsN; ++i) {
      TClonesArray* arr = new TClonesArray("TAMSDhit", 500);
      arr->SetOwner(true);
      fListOfStrips->AddAt(arr, i);
   }
   fListOfStrips->SetOwner(true);

   fMap.clear();
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAMSDntuHit::Clear(Option_t*)
{
   for (Int_t i = 0; i < fSensorsN; ++i) {
      TClonesArray* list = GetListOfStrips(i);
      list->Clear("C");
   }
   fMap.clear();
}

//______________________________________________________________________________
//! New cluster
//!
//! \param[in] iSensor sensor id
//! \param[in] value charge value
//! \param[in] aView plane view
//! \param[in] aStrip strip id
TAMSDhit* TAMSDntuHit::NewStrip(Int_t iSensor, Double_t value, Int_t aView, Int_t aStrip)
{
   if (iSensor >= 0  && iSensor < fSensorsN) {
      TClonesArray &stripArray = *GetListOfStrips(iSensor);
      std::pair<int, int> idx(aView, aStrip);
      
      // check if strip already exists
      if ( fMap[idx] == iSensor+1) {
         TAMSDhit* strip = new TAMSDhit(iSensor, value, aView, aStrip);
         TAMSDhit* curStrip = (TAMSDhit*)stripArray.FindObject(strip);
         delete strip;
         return curStrip;
         
      } else {
         fMap[idx] = iSensor+1;
         TAMSDhit* strip = new(stripArray[stripArray.GetEntriesFast()]) TAMSDhit(iSensor, value, aView, aStrip);
         return strip;
      }
   } else {
      Error("NewStrip()", "Wrong sensor number %d\n", iSensor);
      return 0x0;
   }
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAMSDntuHit::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fSensorsN; ++i) {
      
      os << "TAMSDntuHit " << GetName()
      << Form("  nStrips=%3d", GetStripsN(i))
      << endl;
      
      //TODO properly
      //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
      for (Int_t j = 0; j < GetStripsN(i); j++) {
         const TAMSDhit*  strip = GetStrip(i,j);
         if (strip)
            os << Form("%4d", strip->GetStrip());
         os << endl;
      }
   }
}

