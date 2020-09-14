////////////////////////////////////////////////////////////
// Class Description of TAMSDrawHit                        //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////
#include "TClonesArray.h"

#include "TAMSDdatRaw.hxx"

ClassImp(TAMSDrawHit) 

//______________________________________________________________________________
//
TAMSDrawHit::TAMSDrawHit()
: TObject(),
   fInput(0), 
   fValue(0),
   fIndex(0),
   fView(0),
   fStrip(0)
{
   
}

//______________________________________________________________________________
//
TAMSDrawHit::TAMSDrawHit( Int_t input, Int_t value, Int_t view, Int_t strip)
: TObject(),
   fInput(input), 
   fValue(value),
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
  // Class Description of TAMSDdatRaw                        //
  //                                                        //
  //                                                        //
  ////////////////////////////////////////////////////////////

#include "TAMSDparMap.hxx"

ClassImp(TAMSDdatRaw) 

TString TAMSDdatRaw::fgkBranchName   = "msdrd.";

//______________________________________________________________________________
//
TAMSDdatRaw::TAMSDdatRaw() 
: TAGdata(),
  fListOfPixels(0x0)
{
   SetupClones();
}

//______________________________________________________________________________
//  
TAMSDdatRaw::~TAMSDdatRaw()
{
  // TAMSDdatRaw default destructor
   delete fListOfPixels; 
}

//------------------------------------------+-----------------------------------
//! return number of pixels for a given sensor.
Int_t TAMSDdatRaw::GetPixelsN(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < TAMSDparMap::GetSensorsN())
	  return GetPixels(iSensor)->GetEntries();
   else {
	  Error("GetPixelsN()","Wrong sensor number %d\n", iSensor);
	  return -1;
   }
}

//______________________________________________________________________________
//
TClonesArray* TAMSDdatRaw::GetPixels(Int_t iSensor) const      
{ 
   if (iSensor >= 0  || iSensor < TAMSDparMap::GetSensorsN()) 	  
	  return (TClonesArray*)fListOfPixels->At(iSensor);
   else {
	  Error("GetPixels()","Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
   
}
   
//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TAMSDrawHit* TAMSDdatRaw::GetPixel(Int_t iSensor, Int_t iPixel) const
{
   if (iPixel >=0 || iPixel < GetPixelsN(iSensor))
	  return (TAMSDrawHit*)GetPixels(iSensor)->At(iPixel);
   else {
	  Error("GetPixels()","Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }
}
 
//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TAMSDrawHit* TAMSDdatRaw::GetPixel(Int_t iSensor, Int_t iPixel)
{
   if (iPixel >=0 || iPixel < GetPixelsN(iSensor))
	  return (TAMSDrawHit*)GetPixels(iSensor)->At(iPixel);
   else {
	  Error("GetPixel()","Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }
}
   
//______________________________________________________________________________
//  
void TAMSDdatRaw::AddPixel(Int_t sensor, Int_t value, Int_t aLine, Int_t aColumn)
{
   if (sensor >= 0  || sensor < TAMSDparMap::GetSensorsN()) {	  
	  TClonesArray &pixelArray = *GetPixels(sensor);
	  new(pixelArray[pixelArray.GetEntriesFast()]) TAMSDrawHit(sensor, value, aLine, aColumn);
   } else {
	  Error("AddPixel()","Wrong sensor number %d\n", sensor);
   }
}

//______________________________________________________________________________
//  
void TAMSDdatRaw::SetupClones()
{
   if (fListOfPixels) return;
   fListOfPixels = new TObjArray();
   for (Int_t i = 0; i < TAMSDparMap::GetSensorsN(); ++i) {
	  TClonesArray* arr = new TClonesArray("TAMSDrawHit");
	  arr->SetOwner(true); 
	  fListOfPixels->AddAt(arr,i);
   }
   fListOfPixels->SetOwner(true);    
}

//______________________________________________________________________________
//  
void TAMSDdatRaw::Clear(Option_t* /*opt*/)
{
   TAGdata::Clear();
   for (Int_t i = 0; i < TAMSDparMap::GetSensorsN(); ++i) 
	  GetPixels(i)->Delete();
}

//______________________________________________________________________________
//  
void TAMSDdatRaw::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < TAMSDparMap::GetSensorsN(); ++i) {
	  os << "TAMSDdatRaw " << GetName()
	     << Form("  nhit=%3d", GetPixelsN(i))
	     << endl;
	  
	  for (Int_t k = 0; k < GetPixelsN(i); k++) {
		 const TAMSDrawHit* hit = GetPixel(i,k);
		 os << Form("%4d\n", hit->GetInput());
		 os << Form("%4d\n", hit->GetValue());
		 os << Form("%4d\n", hit->GetView());
		 os << Form("%4d\n", hit->GetStrip());
		 os << Form("%4d\n", hit->GetIndex());
		 os << endl;
	  }
   }
}
