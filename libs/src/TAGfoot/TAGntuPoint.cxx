
#include "TString.h"
#include "TClonesArray.h"
#include "TAGntuPoint.hxx"


ClassImp(TAGpoint) // Description of Single Detector TAGpoint

//______________________________________________________________________________
//  default constructor
TAGpoint::TAGpoint()
 : TAGobject(),
   fPosition(0,0,0),
   fTime(0.),
   fChargeZ(-99),
   fChargeProbaZ(-1)
{
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TVector3 pos, Double_t time, Double_t chargeZ, Double_t probaZ)
 : TAGobject(),
   fPosition(pos),
   fTime(time),
   fChargeZ(chargeZ),
   fChargeProbaZ(probaZ)
{
}

//______________________________________________________________________________
// Clear
void TAGpoint::Clear(Option_t*)
{
}


//##############################################################################
ClassImp(TAGntuPoint);


TString TAGntuPoint::fgkBranchName   = "glbpoint.";

//------------------------------------------+-----------------------------------
//! 
TAGntuPoint::TAGntuPoint() 
: TAGdata(),
  fListOfPoints(0x0)
{
	SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGntuPoint::~TAGntuPoint()
{
	delete fListOfPoints;
}

//______________________________________________________________________________
//  standard 
TAGpoint* TAGntuPoint::NewPoint(TVector3 pos, Double_t time, Double_t chargeZ, Double_t probaZ)
{
	TClonesArray &pixelArray = *fListOfPoints;
	TAGpoint* pixel = new(pixelArray[pixelArray.GetEntriesFast()]) TAGpoint(pos, time, chargeZ, probaZ);

	return pixel;
}

//------------------------------------------+-----------------------------------
Int_t TAGntuPoint::GetPointsN()
{
	return fListOfPoints->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TAGpoint* TAGntuPoint::GetPoint(Int_t iPoint) {

	if ( iPoint < 0  || iPoint >= GetPointsN() ) {
      Error("GetPoint", "Index %d outside the limit %d", iPoint, GetPointsN() -1);
	}
	return (TAGpoint*)fListOfPoints->At(iPoint);
}

//------------------------------------------+-----------------------------------
//! Setup clones. Crate and initialise the list of pixels
void TAGntuPoint::SetupClones()
{
   if (fListOfPoints) return;
   fListOfPoints = new TClonesArray("TAGpoint", 500);
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAGntuPoint::Clear(Option_t*)
{
	fListOfPoints->Clear("C");
}





/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAGntuPoint::ToStream(ostream& os, Option_t* option) const
{
   // for (Int_t i = 0; i < m_vtxGeo->GetSensorsN(); ++i) {
	  
	  // os << "TAGntuPoint " << GetName()
	  // << Form("  nPixels=%3d", GetPixelsN(i))
	  // << endl;
	  
	  // //TODO properly
	  // //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
	  // for (Int_t j = 0; j < GetPixelsN(i); j++) {  // all by default
		 // const TAGpoint*  pixel = GetPixel(i,j, "all");
		 // if (pixel)
			// os << Form("%4d", pixel->GetPixelIndex());
		 // os << endl;
	  // }
   // }
}
