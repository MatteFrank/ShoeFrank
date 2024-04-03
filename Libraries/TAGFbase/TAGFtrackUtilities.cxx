/*!
 \file TAGFtrackUtilities.cxx
 \brief  Implementation of utility class used for handling a GF track
 \author R.Zarrella
*/

#include "TAGFtrackUtilities.hxx"

/*!
 \class TAGFtrackUtilities
 \brief Utility class used for most functions handling a GF track
*/

TAGFtrackUtilities::TAGFtrackUtilities(TAGFdetectorMap* GFdetMap, TAGFgeometryConstructor* GFgeometry)
{
	fGFdetMap = GFdetMap;
	fGFgeometry = GFgeometry;
}

TAGFtrackUtilities::~TAGFtrackUtilities()
{
}


//----------------------------------------------------------------------------------------------------

//! \brief Extrapolate a track to a GenFit FitPlane
//!
//! \param[in] trackToFit Pointer to track to extrapolate
//! \param[in] whichPlane Index of the FitPlane where to extrapolate the track
//! \param[in,out] mom reference to vector for storing particle momentum at extrapolated state
//! \param[in] backExtrap Flag that signals if the extrapolation has to be performed in the backward direction (default = false)
//! \param[in] repId Index of the track representation to use for the extrapolation (default = -1, i.e. cardinal representation)
//! \return Extrapolated position vector in the FitPlane local reference frame
TVector3 TAGFtrackUtilities::ExtrapolateToOuterTracker(Track* trackToFit, int whichPlane, TVector3& mom, bool backExtrap, int repId)
{

	//+++++++++++++++++++++++++++++++++++++++
	AbsTrackRep* trackRep;
	if(repId == -1)
		trackRep = trackToFit->getCardinalRep();
	else
		trackRep = trackToFit->getTrackRep(repId);
	
	// Get first point if extrapolation goes backward or last point if forward
	int pointId = backExtrap ? 1 : -1;
	TrackPoint* tp = trackToFit->getPointWithMeasurementAndFitterInfo(pointId, trackRep);
	if (tp == nullptr)
		throw genfit::Exception("Track has no TrackPoint with fitterInfo", __LINE__, __FILE__);

	//Get fitter info
	KalmanFitterInfo* fitInfo = static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(trackRep));

	// Check if point has backward/forward update
	bool hasUpdate = backExtrap ? fitInfo->hasBackwardUpdate() : fitInfo->hasForwardUpdate();
	if ( !hasUpdate ) {
		string dir = backExtrap ? "backward" : "forward";
		Error("ExtrapolateToOuterTracker()", "TrackPoint has no %s update", dir.c_str());
		exit(42);
	}

	//RZ: Get update of fitted state and extrpolate to the plane
	KalmanFittedStateOnPlane* stateToCopy = backExtrap ? fitInfo->getBackwardUpdate() : fitInfo->getForwardUpdate();
	KalmanFittedStateOnPlane kfTest = *stateToCopy;
	trackRep->extrapolateToPlane(kfTest, fGFdetMap->GetFitPlane(whichPlane), false, false); //RZ: Local reference frame of "whichPlane"!!!

	TVector3 posi((kfTest.getState()[3]),(kfTest.getState()[4]), fGFdetMap->GetFitPlane(whichPlane)->getO().Z());
	mom = kfTest.getMom();

	return posi;
}


//! \brief Extrapolate a track to a GenFit FitPlane
//!
//! Function overload for when particle momentum at extrapolated state is not needed in output
//! \param[in] trackToFit Pointer to track to extrapolate
//! \param[in] whichPlane Index of the FitPlane where to extrapolate the track
//! \param[in] backExtrap Flag that signals if the extrapolation has to be performed in the backward direction (default = false)
//! \param[in] repId Index of the track representation to use for the extrapolation
//! \return Extrapolated position vector in the FitPlane local reference frame
TVector3 TAGFtrackUtilities::ExtrapolateToOuterTracker(Track* trackToFit, int whichPlane, bool backExtrap, int repId)
{
	TVector3 mom;
	return ExtrapolateToOuterTracker(trackToFit, whichPlane, mom, backExtrap, repId);
}


//----------------------------------------------------------------------------------------------------

//! \brief Get the name of a particle from its charge
//! \param[in] ch Charge of the particle
//! \return Name of the particle
string TAGFtrackUtilities::GetParticleNameFromCharge(int ch)
{
	std::string name;
	switch(ch)
	{
		case 1:	name = "H";	break;
		case 2:	name = "He";	break;
		case 3:	name = "Li";	break;
		case 4:	name = "Be";	break;
		case 5:	name = "B";	break;
		case 6:	name = "C";	break;
		case 7:	name = "N";	break;
		case 8:	name = "O";	break;
		default:
			name = "fail";	break;
	}

	return name;
}


//! \brief Get the possible charge of a selected track from the TW measurement
//!
//! \param[in] trackToCheck Pointer to GenFit track
//! \return Charge measured from the TW
Int_t TAGFtrackUtilities::GetChargeFromTW(Track *trackToCheck)
{
	TATWpoint* twpoint = 0x0;
	if( trackToCheck->getNumPointsWithMeasurement() >= 0 )
	{
		for (int jTracking = trackToCheck->getNumPointsWithMeasurement() - 1; jTracking >= 0; --jTracking){
			if ( static_cast<genfit::PlanarMeasurement*>(trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId() != fGFdetMap->GetFitPlaneTW() ) continue;

			int MeasId = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getHitId();

			twpoint = ( (TATWntuPoint*) gTAGroot->FindDataDsc(FootActionDscName("TATWntuPoint"))->Object() )->GetPoint( fGFdetMap->GetHitIDFromMeasID(MeasId) ); //Find TW point associated to the track

			return twpoint->GetChargeZ();
		}
	}

	return -1;
}


//! \brief Compute the "default" momentum value for a track, i.e. using measured TOF + nominal TG-TW distance and a mass hypothesis with the main isotope for the TW charge (Z)
//! 
//! Mainly useful for TAGFselector
//! \param[in] track Ptr to genfit track
//! \param[out] mom Reference to momentum vector
void TAGFtrackUtilities::ComputeMainIsotopeMomentum(Track* track, TVector3& mom)
{
	double chargeFromTW = GetChargeFromTW(track);
	double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(chargeFromTW) );
	double beta = ComputeTrackDefaultBeta(track);
	mom.SetMag(mass_Hypo*beta/TMath::Sqrt(1 - pow(beta,2)));
}


//! \brief Compute the "default" track beta from reco quantities, i.e. using measured TOF + nominal TG-TW distance
//! 
//! Mainly useful for TAGFselector
//! \param[in] track Ptr to genfit track
Double_t TAGFtrackUtilities::ComputeTrackDefaultBeta(Track* track)
{
	int pointID = fGFdetMap->GetHitIDFromMeasID(track->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId());
	float TOF = ( (TATWntuPoint*) gTAGroot->FindDataDsc(FootActionDscName("TATWntuPoint"))->Object() )->GetPoint( pointID )->GetMeanTof();
	float var = fGFgeometry->GetGparGeo()->GetBeamPar().Energy/TAGgeoTrafo::AmuToGev();
	float beam_speed = TAGgeoTrafo::GetLightVelocity()*TMath::Sqrt(var*(var + 2))/(var + 1);
	TOF -= (fGFgeometry->GetGgeoTrafo()->GetTGCenter().Z()-fGFgeometry->GetGgeoTrafo()->GetSTCenter().Z())/beam_speed;
	Double_t beta = (fGFgeometry->GetGgeoTrafo()->GetTWCenter().Z() - fGFgeometry->GetGgeoTrafo()->GetTGCenter().Z())/(TOF*TAGgeoTrafo::GetLightVelocity());

	return beta;
}


//! \brief Get the reconstructed information for a Track Point
//!
//! \param[in] i Index of the Track Point
//! \param[in] track Pointer to track under study
//! \param[out] KalmanPos Pointer to vector used to store the fitted position
//! \param[out] KalmanMom Pointer to vector used to store the fitted momentum
//! \param[out] KalmanPos_cov Pointer to matrix used to store the covariance of fitted position
//! \param[out] KalmanMom_cov Pointer to matrix used to store the covariance of fitted momentum
void TAGFtrackUtilities::GetRecoTrackInfo ( int i, Track* track, TVector3* KalmanPos, TVector3* KalmanMom, TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov )
{
	TVectorD state6 = (track->getFittedState(i).get6DState());
	TMatrixD cov6 = (track->getFittedState(i).get6DCov());

	// Get reco track kinematics and errors
	*KalmanPos = TVector3( state6[0], state6[1], state6[2] );
	*KalmanMom = TVector3( state6[3], state6[4], state6[5] );

	MatrixToZero(*KalmanPos_cov);
	MatrixToZero(*KalmanMom_cov);
	for ( int j=0; j<3; j++ ) {
		for ( int k=0; k<3; k++ ) {
			(*KalmanMom_cov)(j,k) = cov6[j+3][k+3];
			(*KalmanPos_cov)(j,k) = cov6[j][k];
		}
	}

}


//! \brief Get information on a measurement along a global track
//!
//! \param[in] detID Id of the detector
//! \param[in] hitID Global measurement Id
//! \param[out] iSensor Pointer to local sensor Id
//! \param[out] iClus Pointer to local cluster Id
//! \param[out] iPart Pointer to vector of MC particles of the cluster
//! \param[out] pos Pointer to vector where*e to store the position error
void TAGFtrackUtilities::GetMeasInfo( int detID, int hitID, int* iSensor, int* iClus, TVector3* pos, TVector3* posErr )
{
	// check
	if ( detID != fGFdetMap->GetDetIDFromMeasID( hitID ) )
	{
		Error("GetMeasInfo()", "Detector ID not matching between GENFIT (%d) and SensorIDmap (%d)", detID, fGFdetMap->GetDetIDFromMeasID( hitID ));
		exit(42);
	}

	string det = fGFdetMap->GetDetNameFromMeasID( hitID );
	*iSensor = fGFdetMap->GetSensorIDFromMeasID( hitID );
	*iClus = fGFdetMap->GetHitIDFromMeasID( hitID );

	//Get information on the hit position
	if ( det == "VT" ) {
		TAVTcluster* clus = ( (TAVTntuCluster*) gTAGroot->FindDataDsc(FootActionDscName("TAVTntuCluster"))->Object() )->GetCluster( *iSensor, *iClus );
		*pos = fGFgeometry->GetGgeoTrafo()->FromVTLocalToGlobal( clus->GetPositionG() );
		*posErr = clus->GetPosError();
	}
	else if ( det == "IT" ) {
		TAITcluster* clus = ( (TAITntuCluster*) gTAGroot->FindDataDsc(FootActionDscName("TAITntuCluster"))->Object() )->GetCluster( *iSensor, *iClus );
		*pos = fGFgeometry->GetGgeoTrafo()->FromITLocalToGlobal( clus->GetPositionG() );
		*posErr = clus->GetPosError();
	}
	else if ( det == "MSD" ) {
		TAMSDcluster* clus = ( (TAMSDntuCluster*) gTAGroot->FindDataDsc(FootActionDscName("TAMSDntuCluster"))->Object() )->GetCluster( *iSensor, *iClus );
		*pos = fGFgeometry->GetGgeoTrafo()->FromMSDLocalToGlobal( clus->GetPositionG() );
		*posErr = clus->GetPosError();
	}
	else if ( det == "TW" ) {
		TATWpoint* clus = ( (TATWntuPoint*) gTAGroot->FindDataDsc(FootActionDscName("TATWntuPoint"))->Object() )->GetPoint( *iClus );
		*pos = fGFgeometry->GetGgeoTrafo()->FromTWLocalToGlobal( clus->GetPositionG() );
		*posErr = clus->GetPosErrorG();
	}
	else 
		cout << "ERROR -- TAGactKFitter::GetMeasTrackInfo -- No correct detector name found: "<< det << endl, exit(42);
}


//! \brief Reset matrix values to zero
void TAGFtrackUtilities::MatrixToZero( TMatrixD &matrix )
{
	for ( int j=0; j<matrix.GetNrows(); j++ ) {
		for ( int k=0; k<matrix.GetNcols(); k++ ) {
			matrix(j,k) = 0;
		}
	}
}
