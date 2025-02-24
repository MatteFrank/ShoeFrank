/*!
 \file TAMSDntuRaw.cxx
 \brief   Implementation of TAMSDntuRaw.
 */


#include "TClonesArray.h"

#include "TAMSDntuRaw.hxx"

/*!
 \class TAMSDrawHit
 \brief Raw hit for MSD detectors. **
 */

//! Class imp
ClassImp(TAMSDrawHit) 

//______________________________________________________________________________
// Constructor
TAMSDrawHit::TAMSDrawHit()
: TObject(),
   fSensorId(0),
   fCharge(0),
   fIndex(0),
   fView(0),
   fStrip(0),
   fIsSeed(false),
   fIsNoisy(false)
{
}

//______________________________________________________________________________
//!  build raw hit
//!
//! \param[in] id plane id
//! \param[in] view plane view id
//! \param[in] strip strip id
//! \param[in] charge raw measured charge
TAMSDrawHit::TAMSDrawHit(Int_t id, Int_t view, Int_t strip, Double_t charge)
: TObject(),
   fSensorId(id),
   fCharge(charge),
   fIndex(0),
   fView(view),
   fStrip(strip),
   fIsSeed(false),
   fIsNoisy(false)
{
}

//______________________________________________________________________________
//!  Compare to a hit
//!
//! \param[in] obj hit to compare
Int_t TAMSDrawHit::Compare(const TObject* obj) const
{
   Int_t aLine = fView;
   Int_t nLine = ((TAMSDrawHit *)obj)->GetView();
   Int_t aCol = fStrip;
   Int_t nCol = ((TAMSDrawHit *)obj)->GetStrip();
   
   if (aLine == nLine) { //Order ok then order for column
      if(aCol<nCol)
		 return -1;
      else
		 return 1;
   }else if(aLine > nLine)
      return 1;
   else
      return -1;
   
}

//####################################################################################

  ////////////////////////////////////////////////////////////
  // Class Description of TAMSDntuRaw                        //
  //                                                        //
  //                                                        //
  ////////////////////////////////////////////////////////////

#include "TAGroot.hxx"
#include "TAGparaDsc.hxx"
#include "TAMSDparGeo.hxx"
#include "TAMSDparMap.hxx"
#include "TAGnameManager.hxx"

/*!
 \class TAMSDntuRaw
 \brief Raw hit container for MSD detectors. **
 */

//! Class imp
ClassImp(TAMSDntuRaw)

//______________________________________________________________________________
//! Detector
TAMSDntuRaw::TAMSDntuRaw(Int_t sensorsN)
 : TAGdata(),
   fSensorsN(sensorsN),
   fListOfStrips(0x0)
{
   if (sensorsN == 0) {
      Warning("TAMSDntuRaw()", "Size of hit array not set, set to %d\n", TAMSDparGeo::GetDefSensorsN());
      fSensorsN = TAMSDparGeo::GetDefSensorsN();
   }
   
   SetupClones();
}

//______________________________________________________________________________
//! Destructor
TAMSDntuRaw::~TAMSDntuRaw()
{
  // TAMSDntuRaw default destructor
   delete fListOfStrips;
}

//------------------------------------------+-----------------------------------
//! return number of strips for a given sensor (const)
//!
//! \param[in] iSensor sensor id
Int_t TAMSDntuRaw::GetStripsN(Int_t iSensor) const
{
   if (iSensor >= 0  && iSensor < fSensorsN)
	  return GetStrips(iSensor)->GetEntriesFast();
   else {
	  Error("GetStripsN()","Wrong sensor number %d\n", iSensor);
	  return -1;
   }
}

//------------------------------------------+-----------------------------------
//! return number of strips for a given sensor
//!
//! \param[in] iSensor sensor id
TClonesArray* TAMSDntuRaw::GetStrips(Int_t iSensor) const
{ 
   if (iSensor >= 0  && iSensor < fSensorsN)
	  return (TClonesArray*)fListOfStrips->At(iSensor);
   else {
	  Error("GetStrips()","Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
   
}

//------------------------------------------+-----------------------------------
//! Get strip (const)
//!
//! \param[in] iSensor sensor id
//! \param[in] iStrip strip index
const TAMSDrawHit* TAMSDntuRaw::GetStrip(Int_t iSensor, Int_t iStrip) const
{
   if (iStrip >=0 && iStrip < GetStripsN(iSensor))
	  return (TAMSDrawHit*)GetStrips(iSensor)->At(iStrip);
   else {
	  Error("GetStrip()","Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }
}
 
//------------------------------------------+-----------------------------------
//! Get strip
//!
//! \param[in] iSensor sensor id
//! \param[in] iStrip strip index
TAMSDrawHit* TAMSDntuRaw::GetStrip(Int_t iSensor, Int_t iStrip)
{
   if (iStrip >=0 && iStrip < GetStripsN(iSensor))
	  return (TAMSDrawHit*)GetStrips(iSensor)->At(iStrip);
   else {
	  Error("GetStrip()","Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }
}

//______________________________________________________________________________
//!  Add raw hit
//!
//! \param[in] sensor sensor id
//! \param[in] view plane view id
//! \param[in] strip strip id
//! \param[in] value raw measured charge
TAMSDrawHit* TAMSDntuRaw::AddStrip(Int_t sensor, Int_t view, Int_t strip, Double_t value)
{
   if (sensor >= 0  && sensor < fSensorsN) {
	  TClonesArray &stripArray = *GetStrips(sensor);
	  return new(stripArray[stripArray.GetEntriesFast()]) TAMSDrawHit(sensor, view, strip, value);
   } else {
	  Error("AddStrip()","Wrong sensor number %d\n", sensor);
      return 0x0;
   }
}

//______________________________________________________________________________
//! Setup clones
void TAMSDntuRaw::SetupClones()
{
   if (fListOfStrips) return;
   fListOfStrips = new TObjArray(fSensorsN);
   for (Int_t i = 0; i < fSensorsN; ++i) {
	  TClonesArray* arr = new TClonesArray("TAMSDrawHit");
	  arr->SetOwner(true); 
	  fListOfStrips->AddAt(arr,i);
   }
   fListOfStrips->SetOwner(true);

}

//______________________________________________________________________________
//! Clear
void TAMSDntuRaw::Clear(Option_t* /*opt*/)
{
   TAGdata::Clear();
   
   for (Int_t i = 0; i < fSensorsN; ++i){
      TClonesArray* list = GetStrips(i);
      list->Clear("C");
   }
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAMSDntuRaw::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fSensorsN; ++i) {
	  os << "TAMSDntuRaw " << GetName()
	     << Form("  nhit=%3d", GetStripsN(i))
	     << endl;
	  
	  for (Int_t k = 0; k < GetStripsN(i); k++) {
		 const TAMSDrawHit* hit = GetStrip(i,k);
		 os << Form("%4d\n", hit->GetSensorId());
		 os << Form("%4f\n", hit->GetCharge());
		 os << Form("%4d\n", hit->GetView());
		 os << Form("%4d\n", hit->GetStrip());
		 os << Form("%4d\n", hit->GetIndex());
		 os << endl;
	  }
   }
}
