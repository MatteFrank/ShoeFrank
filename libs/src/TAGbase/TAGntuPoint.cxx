
#include "TString.h"
#include "TClonesArray.h"
#include "TAGntuPoint.hxx"
#include <array>

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
//  build a point
TAGpoint::TAGpoint( string trackDetID, int iPlane, int iClus, vector<int>* iPart, TVector3* measPos, TVector3* measPosErr )
                  : TAGcluster(),
   fDevName(trackDetID),
   fEnergyLoss(-1.)
{
   fClusterIdx = iClus;
   fSensorIdx = iPlane;
	for(int i=0; i < iPart->size(); ++i){
      fMcTrackIdx.Set(fMcTrackIdx.GetSize() + 1);
		fMcTrackIdx[fMcTrackIdx.GetSize() - 1] = iPart->at(i);
	}
	SetMeasPosition(*measPos);
	SetMeasPosError(*measPosErr);
}




void TAGpoint::SetRecoInfo( TVector3* recoPos, TVector3* recoMom, TMatrixD* recoPos_cov, TMatrixD* recoMom_cov ) {
	SetFitPosition(*recoPos);
   TVector3 temp = EvalError( *recoPos_cov );
	SetFitPosError(temp);

	SetMomentum(*recoMom);
   temp = EvalError( *recoMom_cov );
   SetMomError(temp);
	// TMatrixD m(3,3);
	// m_recoPos_cov = m;
	// m_recoMom_cov = TMatrixD(3,3);

	// m_recoPos_cov = *recoPos_cov;
	// m_recoMom_cov = *recoMom_cov;
	// MatrixToZero(m_recoPos_cov);
	// MatrixToZero(m_recoMom_cov);

	// for ( int j=0; j<3; j++ ) {
	// 	for ( int k=0; k<3; k++ ) {
	// 		m_recoPos_cov(j,k) = (*recoPos_cov)(j,k);
	// 		m_recoMom_cov(j,k) = (*recoMom_cov)(j,k);
	// 	}
	// }
}




//----------------------------------------------------------------------------------------------------
TVector3 TAGpoint::EvalError( TMatrixD cov ) {

	TVector3 vec(0,0,0);
  

	vec = TVector3( cov(0,0)*cov(0,0), cov(1,1)*cov(1,1), cov(2,2)*cov(2,2) ); // * diagFactor;

  
  return vec;
}





//----------------------------------------------------------------------------------------------------
//  measure the Kalman uncertainty INCLUDING the cross terms in the covariance matrix. CORRELATION considered!!!
double TAGpoint::EvalError( TVector3 mom, TMatrixD cov ) {

  // if ( cov.GetNcols() != 3 || cov.GetNrows() != 3 )
  //   cout << "ERROR :: TAGtrackRepoKalman::EvalError  >>  covariance dimension (should be 6) is wrong " << cov.GetNcols() << " x " << cov.GetNrows() << endl, exit(0);

  array<double,3> partialDer = { mom.x()/sqrt(mom.Mag()), mom.y()/sqrt(mom.Mag()), mom.z()/sqrt(mom.Mag()) };


  double err = 0;
  for ( int j=0; j<cov.GetNrows(); j++ ) {
    for ( int k=0; k<cov.GetNcols(); k++ ) {

      err += partialDer[j] * partialDer[k] * cov(j,k); // * diagFactor;

    }
  }

  double dp = sqrt(err);

  return dp;
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
