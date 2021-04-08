
#include "TString.h"
#include "TClonesArray.h"
#include "TAGntuPoint.hxx"


ClassImp(TAGpoint) // Description of Single Detector TAGpoint

//______________________________________________________________________________
//  default constructor
TAGpoint::TAGpoint()
 : TAGcluster(),
   fDevName(""),
   fPosition(0,0,0),
   fPosError(0,0,0),
   fMomentum(0,0,0),
   fMomError(0,0,0),
   fChargeZ(-99)
{
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TVector3 pos, TVector3 posErr)
: TAGcluster(),
  fDevName(""),
  fPosition(pos),
  fPosError(posErr),
  fMomentum(0,0,0),
  fMomError(0,0,0)
{
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ)
 : TAGcluster(),
   fDevName(""),
   fPosition(pos),
   fPosError(posErr),
   fMomentum(mom),
   fMomError(momErr),
   fChargeZ(chargeZ)
{
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TString name, TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ)
: TAGcluster(),
  fDevName(name),
  fPosition(pos),
  fPosError(posErr),
  fMomentum(mom),
  fMomError(momErr),
  fChargeZ(chargeZ)
{
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TString name, TVector3 pos, TVector3 posErr)
: TAGcluster(),
  fDevName(name),
  fPosition(pos),
  fPosError(posErr),
  fMomentum(0,0,0),
  fMomError(0,0,0),
  fChargeZ(-99)
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
TAGpoint* TAGntuPoint::NewPoint(TVector3 pos, TVector3 posErr)
{
   TClonesArray &pixelArray = *fListOfPoints;
   TAGpoint* pixel = new(pixelArray[pixelArray.GetEntriesFast()]) TAGpoint(pos, posErr);
   
   return pixel;
}

//______________________________________________________________________________
//  standard + momentum
TAGpoint* TAGntuPoint::NewPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ)
{
	TClonesArray &pixelArray = *fListOfPoints;
	TAGpoint* pixel = new(pixelArray[pixelArray.GetEntriesFast()]) TAGpoint(pos, posErr, mom, momErr, chargeZ);

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
