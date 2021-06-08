
#include "TString.h"
#include "TClonesArray.h"
#include "TAGshoeTrackPoint.hxx"


// ClassImp(TAGshoeTrackPoint) // Description of Single Detector TAGshoeTrackPoint

//______________________________________________________________________________
//  default constructor
TAGshoeTrackPoint::TAGshoeTrackPoint()
 : TAGcluster(),
   fDevName(""),
   fPosition(0,0,0),
   fPosError(0,0,0),
   fMomentum(0,0,0),
   fMomError(0,0,0),
   fChargeZ(-99)
{
}

// //______________________________________________________________________________
// //  build a point
// TAGshoeTrackPoint::TAGshoeTrackPoint(TVector3 pos, TVector3 posErr)
// : TAGcluster(),
//   fDevName(""),
//   fPosition(pos),
//   fPosError(posErr),
//   fMomentum(0,0,0),
//   fMomError(0,0,0)
// {
// }

// //______________________________________________________________________________
// //  build a point
// TAGshoeTrackPoint::TAGshoeTrackPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ)
//  : TAGcluster(),
//    fDevName(""),
//    fPosition(pos),
//    fPosError(posErr),
//    fMomentum(mom),
//    fMomError(momErr),
//    fChargeZ(chargeZ)
// {
// }

// //______________________________________________________________________________
// //  build a point
// TAGshoeTrackPoint::TAGshoeTrackPoint(TString name, TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ)
// : TAGcluster(),
//   fDevName(name),
//   fPosition(pos),
//   fPosError(posErr),
//   fMomentum(mom),
//   fMomError(momErr),
//   fChargeZ(chargeZ)
// {
// }

//______________________________________________________________________________
//  build a point
TAGshoeTrackPoint::TAGshoeTrackPoint( int trackDetID, int iPlane, int iClus, vector<int> iPart, TVector3* measPos )
                  : TAGcluster()
{
	fDevName = "VT";       // Device name (VT,IT, MSD, TW, CA)
	fPosition = TVector3(0.,1,2);      // position in FOOT framework
	fPosError = TVector3(0.,1,2);      // position error in FOOT framework
	fMomentum = TVector3(0.,1,2);      // momentum in FOOT framework
	fMomError = TVector3(0.,1,2);      // momentum error in FOOT framework
	fChargeZ = 7;       // Charge Z
}



void TAGshoeTrackPoint::SetRecoInfo( TVector3* recoPos, TVector3* recoMom, TMatrixD* recoPos_cov, TMatrixD* recoMom_cov ) {
  fPosition = *recoPos;
  fMomentum = *recoMom;

}



//______________________________________________________________________________
// Clear
void TAGshoeTrackPoint::Clear(Option_t*)
{
}


