/*!
 \file TAGFtrackUtilities.hxx
 \brief  Header of GenFit track wrapper for utilities functions
 \author R. Zarrella
*/

#ifndef TAGFtrackUtilities_H
#define TAGFtrackUtilities_H

#include "TMatrixD.h"
#include "TVectorD.h"

#include "AbsMeasurement.h"
#include "PlanarMeasurement.h"
#include "KalmanFittedStateOnPlane.h"
#include "KalmanFitterInfo.h"
#include "Track.h"

#include "TAGroot.hxx"
#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TATWntuPoint.hxx"

#include "TAGFdetectorMap.hxx"
#include "TAGFgeometryConstructor.hxx"
#include "UpdatePDG.hxx"

using namespace std;
using namespace genfit;

class TAGFtrackUtilities
{
protected:
	TAGFgeometryConstructor* fGFgeometry;
	TAGFdetectorMap* fGFdetMap;

public:
	TAGFtrackUtilities(TAGFdetectorMap* GFdetMap, TAGFgeometryConstructor* GFgeometry);
	~TAGFtrackUtilities();

	TVector3	ExtrapolateToOuterTracker(Track* trackToFit, int whichPlane, TVector3& mom, bool backExtrap = false, int repId = -1);
	TVector3	ExtrapolateToOuterTracker(Track* trackToFit, int whichPlane, bool backExtrap = false, int repId = -1);

	Int_t		GetChargeFromTW(Track* trackToCheck);
	Double_t	ComputeTrackDefaultBeta(Track* track);
	void		ComputeMainIsotopeMomentum(Track* track, TVector3& mom);
	void 		GetRecoTrackInfo ( int i, Track* track, TVector3* KalmanPos, TVector3* KalmanMom, TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov );
	void 		GetMeasInfo( int detID, int hitID, int* iSensor, int* iClus, TVector3* pos, TVector3* posErr );
	void 		MatrixToZero( TMatrixD& matrix );

	TAGFdetectorMap* GetGFdetMap()		{ return fGFdetMap; }
};

#endif