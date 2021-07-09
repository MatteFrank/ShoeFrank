
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
   fChargeZ(-99),
   fIsXon(true),
   fIsYon(true),
   fEnergyLoss(-1.)
{
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TVector3 pos, TVector3 posErr)
: TAGcluster(),
  fDevName(""),
  fMomentum(0,0,0),
  fMomError(0,0,0),
  fIsXon(true),
  fIsYon(true),
  fEnergyLoss(-1.)
{
   SetPosition(pos);
   SetPosError(posErr);
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ)
 : TAGcluster(),
   fDevName(""),
   fMomentum(mom),
   fMomError(momErr),
   fChargeZ(chargeZ),
   fIsXon(true),
   fIsYon(true),
   fEnergyLoss(-1.)
{
   SetPosition(pos);
   SetPosError(posErr);
}

//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TString name, TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ)
: TAGcluster(),
  fDevName(name),
  fMomentum(mom),
  fMomError(momErr),
  fChargeZ(chargeZ),
  fIsXon(true),
  fIsYon(true),
  fEnergyLoss(-1.)
{
   SetPosition(pos);
   SetPosError(posErr);
}



//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint(TString name, TVector3 pos, TVector3 posErr)
: TAGcluster(),
  fDevName(name),
  fMomentum(0,0,0),
  fMomError(0,0,0),
  fChargeZ(-99),
  fIsXon(true),
  fIsYon(true),
  fEnergyLoss(-1.)
{
   SetPosition(pos);
   SetPosError(posErr);
}






//______________________________________________________________________________
//  build a point
TAGpoint::TAGpoint( string trackDetID, int iPlane, int iClus, vector<int> iPart, TVector3* measPos )
                  : TAGcluster()
{
	fDevName = trackDetID;       // Device name (VT,IT, MSD, TW, CA)
	m_planeID = iPlane;
	m_clusterID = iClus;
	for(int i=0; i < iPart.size(); ++i){
		m_iPart.push_back( iPart.at(i) );
	}
	m_measPos = *measPos;
}




void TAGpoint::SetRecoInfo( TVector3* recoPos, TVector3* recoMom, TMatrixD* recoPos_cov, TMatrixD* recoMom_cov ) {
	fPosition = *recoPos;
	fMomentum = *recoMom;
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

	fPosError = EvalError( *recoPos_cov );
	fMomError = EvalError( *recoMom_cov );

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
