////////////////////////////////////////////////////////////
// Class Description of TAMSDrawHit                        //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////
#include "TClonesArray.h"

#include "TAMSDntuRaw.hxx"

ClassImp(TAMSDrawHit) 

//______________________________________________________________________________
//
TAMSDrawHit::TAMSDrawHit()
: TObject(),
   fSensorId(0),
   fCharge(0),
   fIndex(0),
   fView(0),
   fStrip(0)
{
   
}

//______________________________________________________________________________
//
TAMSDrawHit::TAMSDrawHit(Int_t id, Int_t view, Int_t strip, UInt_t charge)
: TObject(),
   fSensorId(id),
   fCharge(charge),
   fIndex(0),
   fView(view),
   fStrip(strip)
{

}

//_______________________________________
//!Compare return 1 if grater 0 if equal -1 if lower
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

TString TAMSDntuRaw::fgkBranchName   = "msdrd.";

ClassImp(TAMSDntuRaw)

//______________________________________________________________________________
//
TAMSDntuRaw::TAMSDntuRaw() 
: TAGdata(),
  fListOfStrips(0x0)
{
   fpGeoMap = (TAMSDparGeo*) gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object();
   if (!fpGeoMap) {
      Error("TAMSDntuRaw()", "Para desciptor %s does not exist", TAMSDparGeo::GetDefParaName());
      exit(0);
   }
   SetupClones();
}

//______________________________________________________________________________
//  
TAMSDntuRaw::~TAMSDntuRaw()
{
  // TAMSDntuRaw default destructor
   delete fListOfStrips;
}

//------------------------------------------+-----------------------------------
//! return number of pixels for a given sensor.
Int_t TAMSDntuRaw::GetStripsN(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN())
	  return GetStrips(iSensor)->GetEntries();
   else {
	  Error("GetStripsN()","Wrong sensor number %d\n", iSensor);
	  return -1;
   }
}

//______________________________________________________________________________
//
TClonesArray* TAMSDntuRaw::GetStrips(Int_t iSensor) const
{ 
   if (iSensor >= 0  || iSensor < fpGeoMap->GetSensorsN())
	  return (TClonesArray*)fListOfStrips->At(iSensor);
   else {
	  Error("GetStrips()","Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
   
}
   
//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TAMSDrawHit* TAMSDntuRaw::GetStrip(Int_t iSensor, Int_t iStrip) const
{
   if (iStrip >=0 || iStrip < GetStripsN(iSensor))
	  return (TAMSDrawHit*)GetStrips(iSensor)->At(iStrip);
   else {
	  Error("GetStrips()","Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }
}
 
//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TAMSDrawHit* TAMSDntuRaw::GetStrip(Int_t iSensor, Int_t iStrip)
{
   if (iStrip >=0 || iStrip < GetStripsN(iSensor))
	  return (TAMSDrawHit*)GetStrips(iSensor)->At(iStrip);
   else {
	  Error("GetStrip()","Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }
}
   
//______________________________________________________________________________
//  
void TAMSDntuRaw::AddStrip(Int_t sensor, Int_t view, Int_t strip, UInt_t value)
{
   if (sensor >= 0  || sensor < fpGeoMap->GetSensorsN()) {
	  TClonesArray &pixelArray = *GetStrips(sensor);
	  new(pixelArray[pixelArray.GetEntriesFast()]) TAMSDrawHit(sensor, strip, view, value);
   } else {
	  Error("AddStrip()","Wrong sensor number %d\n", sensor);
   }
}

//______________________________________________________________________________
//  
void TAMSDntuRaw::SetupClones()
{
   if (fListOfStrips) return;
   fListOfStrips = new TObjArray();
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
	  TClonesArray* arr = new TClonesArray("TAMSDrawHit");
	  arr->SetOwner(true); 
	  fListOfStrips->AddAt(arr,i);
   }
   fListOfStrips->SetOwner(true);    
}

//______________________________________________________________________________
//  
void TAMSDntuRaw::Clear(Option_t* /*opt*/)
{
   TAGdata::Clear();
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i)
	  GetStrips(i)->Delete();
}

//______________________________________________________________________________
//  
void TAMSDntuRaw::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fpGeoMap->GetSensorsN(); ++i) {
	  os << "TAMSDntuRaw " << GetName()
	     << Form("  nhit=%3d", GetStripsN(i))
	     << endl;
	  
	  for (Int_t k = 0; k < GetStripsN(i); k++) {
		 const TAMSDrawHit* hit = GetStrip(i,k);
		 os << Form("%4d\n", hit->GetSensorId());
		 os << Form("%4d\n", hit->GetCharge());
		 os << Form("%4d\n", hit->GetView());
		 os << Form("%4d\n", hit->GetStrip());
		 os << Form("%4d\n", hit->GetIndex());
		 os << endl;
	  }
   }
}
