
#include "TString.h"
#include "TClonesArray.h"
#include "TAGntuPoint.hxx"


ClassImp(TAGpoint) // Description of Single Detector TAGpoint

//______________________________________________________________________________
//  default constructor
TAGpoint::TAGpoint()
 : TAGcluster(),
   fDevName(""),
   fMomentum(0,0,0),
   fMomError(0,0,0),
   fEnergyLoss(-1.)
{
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TString name, TVector3 measPos, TVector3 measPosErr)
: TAGcluster(),
   fDevName(name),
   fMomentum(0,0,0),
   fMomError(0,0,0),
   fEnergyLoss(-1.)
{
   SetMeasPosition(measPos);
   SetMeasPosError(measPosErr);
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr)
: TAGcluster(),
  fDevName(""),
  fMomentum(0,0,0),
  fMomError(0,0,0),
  fEnergyLoss(-1.)
{
   SetMeasPosition(measPos);
   SetMeasPosError(measPosErr);
   SetFitPosition(fitPos);
   SetFitPosError(fitPosErr);
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr)
 : TAGcluster(),
   fDevName(""),
   fMomentum(mom),
   fMomError(momErr),
   fEnergyLoss(-1.)
{
   SetMeasPosition(measPos);
   SetMeasPosError(measPosErr);
   SetFitPosition(fitPos);
   SetFitPosError(fitPosErr);
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TString name,TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr)
: TAGcluster(),
   fDevName(name),
   fMomentum(0,0,0),
   fMomError(0,0,0),
   fEnergyLoss(-1.)
{
   SetMeasPosition(measPos);
   SetMeasPosError(measPosErr);
   SetFitPosition(fitPos);
   SetFitPosError(fitPosErr);
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TString name,TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr)
: TAGcluster(),
   fDevName(name),
   fMomentum(mom),
   fMomError(momErr),
   fEnergyLoss(-1.)
{
   SetMeasPosition(measPos);
   SetMeasPosError(measPosErr);
   SetFitPosition(fitPos);
   SetFitPosError(fitPosErr);
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
TAGpoint* TAGntuPoint::NewPoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr)
{
   TClonesArray &pixelArray = *fListOfPoints;
   TAGpoint* pixel = new(pixelArray[pixelArray.GetEntriesFast()]) TAGpoint(measPos, measPosErr, fitPos, fitPosErr);
   
   return pixel;
}

//______________________________________________________________________________
//  standard + momentum
TAGpoint* TAGntuPoint::NewPoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr)
{
	TClonesArray &pixelArray = *fListOfPoints;
	TAGpoint* pixel = new(pixelArray[pixelArray.GetEntriesFast()]) TAGpoint(measPos, measPosErr, fitPos, fitPosErr, mom, momErr);

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
