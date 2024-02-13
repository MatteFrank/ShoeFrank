/*!
 \file TAGFselectorBase.cxx
 \brief Base class for track finding/selection in GenFit Global Reconstruction
 \author R. Zarrella and M. Franchini
*/

#include "TAGnameManager.hxx"
#include "TAGFselectorBase.hxx"

/*!
 \class TAGFselectorBase
 \brief Base class for track finding/selection in GenFit Global Reconstruction

 There are already different algorithms implemented for track finding:
 - "TrueParticle": MC truth particle tracking
 - "Standard": Data-like base algorithm, no MC used
 - "Linear": Data-like algorithm for track finding w/out magnetic field -> linear track extrapolation btw detectors
 - "Backtracking": Data-like base algorithm starting from TW and going back to VT
*/


//----------------------------------------------------------------------------------------------------

//! \brief Default constructor
TAGFselectorBase::TAGFselectorBase() :
m_chargeVect(0x0),
m_allHitMeas(0x0),
m_SensorIDMap(0x0),
m_trackCategoryMap(0x0),
m_measParticleMC_collection(0x0),
m_McNtuEve(0x0),
m_singleVertexCounter(0x0),
m_noVTtrackletEvents(0x0),
m_noTWpointEvents(0x0),
m_systemsON(""),
m_VTtolerance(.5),
m_ITtolerance(.5),
m_MSDtolerance(.5),
m_TWtolerance(4.)
{
	m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

	if(TAGrecoManager::GetPar()->IncludeVT())
		m_VT_geo = (TAVTparGeo*) gTAGroot->FindParaDsc(FootParaDscName("TAVTparGeo"))->Object();

	if(TAGrecoManager::GetPar()->IncludeIT())
		m_IT_geo = (TAITparGeo*) gTAGroot->FindParaDsc(FootParaDscName("TAITparGeo"))->Object();

	if(TAGrecoManager::GetPar()->IncludeMSD())
		m_MSD_geo = (TAMSDparGeo*) gTAGroot->FindParaDsc(FootParaDscName("TAMSDparGeo"))->Object();

	if(TAGrecoManager::GetPar()->IncludeTW())
		m_TW_geo = (TATWparGeo*) gTAGroot->FindParaDsc(FootParaDscName("TATWparGeo"))->Object();

	m_BeamEnergy = ( (TAGparGeo*) gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"))->Object() )->GetBeamPar().Energy;

	if( FootDebugLevel(2) )	cout << "Beam Energy::" << m_BeamEnergy << endl;

	m_eventType = 0;
}


//! \brief Default destructor
TAGFselectorBase::~TAGFselectorBase()
{
	//Clear TrackTempMap
	for(auto it = m_trackTempMap.begin(); it != m_trackTempMap.end(); ++it)
		delete it->second;

	m_trackTempMap.clear();

	m_chargeVect->clear();

	for(auto it = m_allHitMeas->begin(); it != m_allHitMeas->end(); ++it)
	{
		for(auto itvec : it->second)
			delete itvec;
		it->second.clear();
	}
	m_allHitMeas->clear();

	for(auto it : m_trackRepVec)
		delete it;
	m_trackRepVec.clear();

	m_trackSlopeMap.clear();

	// if(m_measParticleMC_collection)
	// {
	// 	for(auto it = m_measParticleMC_collection->begin(); it != m_measParticleMC_collection->end(); ++it)
	// 		it->second.clear();
	// 	m_measParticleMC_collection->clear();
	// }
}


//! \brief Set all the needed variables for track selection
//!
//! \param[in] allHitMeas Pointer to the map containing all the measurements in GenFit format; the key is the GenFit FitPlane index
//! \param[in] GFsystemsOn String containing all systems included in the track selection algorithm
//! \param[in] chargeVect Pointer to vector of possible charges in the event (either measured from TW or MC truth)
//! \param[in] SensorIDmap Pointer to TAGFdetectorMap of the campaign
//! \param[in,out] trackCategoryMap Pointer to the map of selected tracks that have to be sent to the KFitter action
//! \param[in] measParticleMC_collection Pointer to the map containing the MC particles found in the event for each measurement; the key is the global index of the measurement
//! \param[in] IsMC boolean flag indicating if the input file is from MC or real data
//! \param[in] singleVertexCounter Pointer to variable counting events with exactly one vertex
//! \param[in] noVTtrackletEvents Pointer to variable counting events with no valid VT tracklet for track extrapolation
//! \param[in] noVTtrackletEvents Pointer to variable counting events with no valid TW point for track extrapolation
void TAGFselectorBase::SetVariables(map<int, vector<AbsMeasurement *>> *allHitMeas, TString GFsystemsOn, vector<int> *chargeVect, TAGFdetectorMap *SensorIDmap, map<TString, Track *> *trackCategoryMap, map<int, vector<int>> *measParticleMC_collection, bool IsMC, uint *singleVertexCounter, uint *noVTtrackletEvents, uint* noTWpointEvents)
{
	m_allHitMeas = allHitMeas;
	m_chargeVect = chargeVect;
	m_SensorIDMap = SensorIDmap;
	m_trackCategoryMap = trackCategoryMap;
	m_measParticleMC_collection = measParticleMC_collection;
	m_IsMC = IsMC;
	m_singleVertexCounter = singleVertexCounter;
	m_noVTtrackletEvents = noVTtrackletEvents;
	m_noTWpointEvents = noTWpointEvents;

	m_systemsON = GFsystemsOn;
	m_detectors = TAGparTools::Tokenize( m_systemsON.Data() , " " );

	if (m_IsMC)
      m_McNtuEve = (TAMCntuPart *)gTAGroot->FindDataDsc(FootActionDscName("TAMCntuPart"))->Object();
}

//----------------------------------------------------------------------------------------------------

//! \brief Base function for track finding/selection/categorization
//!
//! This function gets called from the TAGactKFitter class
//! \return 0 if there were no errors
int TAGFselectorBase::FindTrackCandidates()
{
	CheckPlaneOccupancy();

	if (FillTrackRepVector() != 0)
		return -1;

	// fill m_mapTrack
	Categorize();

	if( TAGrecoManager::GetPar()->EnableEventDisplay() )
		CreateDummyTrack();

	return 0;
}



//----------------------------------------------------------------------------------------------------

//! \brief Check the charges seen by TW and fill the track representation vector
//!
//! \return 0 if track representation vector was filled w/out errors
int TAGFselectorBase::FillTrackRepVector()
{
	m_trackRepVec.clear();
	m_trackTempMap.clear();

	if(m_chargeVect->size() < 1)
	{
		// Error("FillTrackRepVector()", "TW charge vector has non positive length! Vec size::%ld", m_chargeVect->size());
		(*m_noTWpointEvents)++;
		return -1;
	}
	for(int i = 0; i < m_chargeVect->size(); ++i)
	{
		if ( FootDebugLevel(1) ) cout << "TAGFselectorBase::FillTrackRepVector() -- charge: " << m_chargeVect->at(i) << "\n";

		AbsTrackRep* rep = new RKTrackRep( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( m_chargeVect->at(i) ) );
		m_trackRepVec.push_back( rep );
	}
	return 0;
}



//! \brief Count the particles generated in the MC and visible in the acceptance of FOOT
//!
//! This function if used for Selection Efficiency evaluation
//! \return Map containing the name of MC particles seen in the event ("H", "He", "Li"...) and their number
map<string, int> TAGFselectorBase::CountParticleGeneratedAndVisible()
{

	if(FootDebugLevel(1)) 
		cout << "TAGFselector::CountParticleGeneratedAndVisible --  Cycle on planes\t"  << m_SensorIDMap->GetFitPlanesN() << "\n";

	map<string, int> genCount_vector;
	// m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc(FootActionDscName("TAMCntuPart"), "TAMCntuPart")->Object();

	for ( int iPart = 0; iPart < m_McNtuEve->GetTracksN(); iPart++ ) {

		TAMCpart* particle = m_McNtuEve->GetTrack(iPart);

		int particleCh = particle->GetCharge();
		float mass = particle->GetMass();


		if ( particle->GetCharge() < 1 || particle->GetCharge() > ( (TAGparGeo*) gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"))->Object() )->GetBeamPar().AtomicNumber)	continue;

		string outName, pdgName;
		outName = GetParticleNameFromCharge(particleCh);

		//CAREFUL HERE!! Think about the possibility of throwing an error -> skip particle for the moment
		if( outName == "fail" ) {continue;}

		pdgName = outName + int(round(mass/m_AMU));

		//CAREFUL HERE!!!!!!!!! FOOT TAGrecoManager file does not have Hydrogen and Helium isotopes!!!! Also think about throwing an error here...
		if ( !TAGrecoManager::GetPar()->Find_MCParticle( pdgName ) ) 	{
			if(FootDebugLevel(1))  cout << "Found Particle not in MC list: " << pdgName << " num=" << iPart << "\n";
			continue;

		}

		if ( genCount_vector.size() < 1 || genCount_vector.find( outName ) == genCount_vector.end() ) 
			genCount_vector[ outName ] = 0;

		int foundHit = 0;
		int foundHitTW = 0;

		for(int iPlane = 0; iPlane < m_SensorIDMap->GetFitPlanesN(); ++iPlane)    {
		
			//Skip plane if no hit was found
			if(m_allHitMeas->find(iPlane) == m_allHitMeas->end()) 
				continue;
		
			//Cycle on all measurements/clusters of plane
			for(vector<AbsMeasurement*>::iterator itHit = m_allHitMeas->at(iPlane).begin(); itHit != m_allHitMeas->at(iPlane).end(); ++itHit)		{
			
				int MeasGlobId = (*itHit)->getHitId();
				int match = 0;
				for( vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasGlobId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasGlobId).end(); ++itTrackMC )	{
					
					int tmpID = ( *(itTrackMC) );
					if ( tmpID == iPart ) 
					{
						match = 1;
						if( iPlane == m_SensorIDMap->GetFitPlaneTW() )	foundHitTW++;
					}
				}

				if (match == 0)		continue;

				foundHit++;
			}
		}

		if ( foundHit < TAGrecoManager::GetPar()->MeasureN() && !foundHitTW ) 
			continue;

		genCount_vector.at( outName ) += 1;

	}

	return genCount_vector;
}



//--------------------------------------------------------------------------------------------
//! \brief Check the occupancy of all the FitPlanes in GenFit geometry and categorize the events to identify out of target fragmentations
void TAGFselectorBase::CheckPlaneOccupancy()
{

	for (auto itDet = m_detectors.begin(); itDet != m_detectors.end(); ++itDet)
	{
		// RZ: Uncomment when we have an idea or the IT
		//  m_PlaneOccupancy[*itDet];
		//  if( *itDet == "IT" )
		//  	m_PlaneOccupancy[*itDet].resize(m_SensorIDMap->GetPossibleITzLocal()->size());
		//  else
		//  	m_PlaneOccupancy[*itDet].resize(m_SensorIDMap->GetFitPlanesN(*itDet));
		if(*itDet == "IT")
			continue;

		m_PlaneOccupancy[*itDet];
		m_PlaneOccupancy[*itDet].resize(m_SensorIDMap->GetFitPlanesN(*itDet));

		std::fill(m_PlaneOccupancy[*itDet].begin(), m_PlaneOccupancy[*itDet].end(), 0);
	}

	//Cycle on FitPlanes
	for(int iPlane = 0; iPlane < m_SensorIDMap->GetFitPlanesN(); ++iPlane)
	{
		//Skip plane if no hit was found
		if(m_allHitMeas->find(iPlane) == m_allHitMeas->end()){continue;}

		string det = m_SensorIDMap->GetDetNameFromFitPlaneId(iPlane);
		if( det == "IT" )
		{
			continue; //RZ: IT to be studied better!
			// vector<int>* planesAtZLocal;
			// int id=0;
			// for (auto itZ : *(m_SensorIDMap->GetPossibleITzLocal()) )
			// {
			// 	planesAtZLocal = m_SensorIDMap->GetPlanesAtZLocal(itZ);
			// 	if( std::find(planesAtZLocal->begin(), planesAtZLocal->end(), iPlane) != planesAtZLocal->end() )
			// 	{
			// 		m_PlaneOccupancy[det][id] += m_allHitMeas->at(iPlane).size();
			// 		break;
			// 	}
			// 	id++;
			// }
		}
		else
		{
			int sensorId;
			if( m_SensorIDMap->GetSensorID(iPlane, &sensorId) )
				m_PlaneOccupancy[det][sensorId] = m_allHitMeas->at(iPlane).size();
		}

	} //End of loop on sensors

	//Classify the event
	if( m_PlaneOccupancy.find("TW") == m_PlaneOccupancy.end() || m_PlaneOccupancy["TW"][0] < 1 )
		m_eventType = 5; // No TW point in the event -> bad event
	else
	{
		bool needsCheck = false;
		bool needsCheckIT = false;
		bool foundIncrease = false;
		bool foundIncreaseBtwDets = false;
		map<string, bool> detCountsChange;
		map<string, bool> detCountsIncrease;
		map<string, bool> detCountsIncreaseBtwDets;
		int prev_counts, counts;

		//Modify when IT is included!!!
		if( m_detectors[0] == "IT" && m_detectors.size() > 1 )
			prev_counts = m_PlaneOccupancy[m_detectors[1]][0];
		else if( m_detectors[0] != "IT" )
			prev_counts = m_PlaneOccupancy[m_detectors[0]][0];
		else
		{
			Error("CheckPlaneOccupancy()", "Error in handling plane occupancy maps! Exiting...");
			exit(-1);
		}

		for(auto itDet = m_detectors.begin(); itDet != m_detectors.end(); ++itDet)
		{

			if(*itDet != "IT")
			{
				detCountsChange[*itDet] = false;
				detCountsIncrease[*itDet] = false;
				if( itDet != m_detectors.begin() )
					detCountsIncreaseBtwDets[*itDet] = false;

				counts = m_PlaneOccupancy[*itDet][0];
				// Set flag for increase btw detectors
				if( itDet != m_detectors.begin() && prev_counts != counts )
				{
					needsCheck = true;
					if( prev_counts < counts)	detCountsIncreaseBtwDets[*itDet] = true;
				}

				for( int i = 1; i < m_PlaneOccupancy[*itDet].size(); ++i)
				{
					if( m_PlaneOccupancy[*itDet][i] != counts )
					{
						detCountsChange[*itDet] = true;

						//Set flag for increase if it happens at ANY stage
						if( m_PlaneOccupancy[*itDet][i] > counts )	detCountsIncrease[*itDet] = true;
						counts = m_PlaneOccupancy[*itDet][i];
					}
				}
				prev_counts = counts;
			}
			else
			{
				continue;
				// int counts1 = 0, counts2 = 0;
				// for( int i = 0; i < m_PlaneOccupancy[*itDet].size()/2; ++i )
				// {
				// 	// if( m_PlaneOccupancy[*itDet][2*i] != m_PlaneOccupancy[*itDet][2*i + 1] )

				// 	counts1 += m_PlaneOccupancy[*itDet][2*i];
				// 	counts2 += m_PlaneOccupancy[*itDet][2*i + 1];
				// }
				// if(counts1 != counts2) needsCheckIT = true;
			}

			foundIncrease = foundIncrease || detCountsIncrease[*itDet];
			foundIncreaseBtwDets = foundIncreaseBtwDets || detCountsIncreaseBtwDets[*itDet];
			needsCheck = needsCheck || needsCheckIT || detCountsChange[*itDet] || foundIncrease || foundIncreaseBtwDets;
		}


		if( !needsCheck )
			m_eventType = 1; //Clean event, likely all fragments reconstructable
		else
		{
			if( foundIncrease )
				m_eventType = 3; //Likely fragmentation in a detector plane
			else if ( foundIncreaseBtwDets )
				m_eventType = 4; //Likely fragmentation in air
			else
				m_eventType = 2; //Good event, likely some tracks exiting the angular acceptance
		}
	}

	//Print in debug mode
	if( FootDebugLevel(2))// || (m_eventType != 1 && m_eventType != 5))
	{
		cout << "EVENT::" << gTAGroot->CurrentEventId().EventNumber() << "\tTYPE::" << m_eventType << endl;
		for(auto itDet = m_detectors.begin(); itDet != m_detectors.end(); ++itDet)
		{
			for( int i=0; i < m_PlaneOccupancy[*itDet].size(); ++i)
				cout << *itDet << "\tId::" << i << "\tNmeas::" << m_PlaneOccupancy[*itDet][i] << endl;
		}
	}
}

//! \brief Get the event type seen by the Genfit selector
int TAGFselectorBase::GetEventType() { return m_eventType; }


//! \brief Fill plane occupancy histogram
void TAGFselectorBase::FillPlaneOccupancy(TH2I** h_PlaneOccupancy)
{
	int count = 0;
	for( auto it = m_detectors.begin(); it != m_detectors.end(); ++it)
	{
		if( m_PlaneOccupancy.find(*it) == m_PlaneOccupancy.end() )
			continue;
		else
		{
			for(int i=0; i < m_PlaneOccupancy[*it].size(); ++i)
			{
				h_PlaneOccupancy[0]->Fill( count, m_PlaneOccupancy[*it][i] );
				h_PlaneOccupancy[m_eventType]->Fill( count, m_PlaneOccupancy[*it][i] );
				++count;
			}
		}
	}
}


//--------------------------------------------------------------------------------------------


//! \brief Fill the final track map to process in TAGactKFitter w/ the selected tracks
void TAGFselectorBase::FillTrackCategoryMap()
{

	for(map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack)
	{
		// Handle the dummy track showing all the TW points -> present only when the genfit event display is on
		if(itTrack->first == -7)
		{
			(*m_trackCategoryMap)["dummy"] = new Track(*(itTrack->second));
			continue;
		}
		TString outName;
		int MeasId = itTrack->second->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId();
		if( TAGrecoManager::GetPar()->PreselectStrategy() != "TrueParticle" && m_SensorIDMap->GetFitPlaneIDFromMeasID(MeasId) != m_SensorIDMap->GetFitPlaneTW())
		{
			if(FootDebugLevel(1))
				Info("FillTrackCategoryMap()", "Track candidate %d no TW point!", itTrack->first);
		}

		int measCharge = itTrack->second->getCardinalRep()->getPDGCharge();
		int measMass;
		if( TAGrecoManager::GetPar()->PreselectStrategy() == "Linear" )
			measMass = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(measCharge) );
		else
			measMass = round( itTrack->second->getCardinalRep()->getMass( (itTrack->second)->getFittedState(-1) )/m_AMU );


		if ( FootDebugLevel(2) )	Info("FillTrackCategoryMap()", "Track with measured charge %d and mass %d!!", measCharge, measMass);

		outName = GetParticleNameFromCharge(measCharge);

		if( outName == "fail" )
		{
			Info("FillTrackCategoryMap()", "Skipped Track with measured charge %d and mass %d!!", measCharge, measMass);
			continue;
		}

		outName += Form("_%d_%d", measMass, itTrack->first);
		(*m_trackCategoryMap)[outName] = new Track(*(itTrack->second));
	}
}


//! \brief Create a dummy track to display all the TW points of the event
//! 
//! Only called when Genfit event display is switched on
void TAGFselectorBase::CreateDummyTrack()
{
	int planeTW = m_SensorIDMap->GetFitPlaneTW();
	if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() )	return;

	//If the genfit event display is switched on, create a dummy track to show all the TW points
	if( TAGrecoManager::GetPar()->EnableEventDisplay() )
	{
		Track* dummy = new Track();
		for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( planeTW ).begin(); it != m_allHitMeas->at( planeTW ).end(); ++it){
			AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (*it))->clone();
				dummy->insertMeasurement( hitToAdd );
		}
		if(dummy->getNumPointsWithMeasurement()>0) m_trackTempMap[-7] = new Track(*dummy);
		delete dummy;
	}
}


//----------------------------------------------------------------------------------------------------

//! \brief Get the possible charge of a selected track from the TW measurement
//!
//! Currently uses MC information (returns charge of most frequent particle along the track) OR the charge of the TWpoint associated to the track
//! \param[in] trackToCheck Pointer to GenFit track
//! \return Charge measured from the TW
int TAGFselectorBase::GetChargeFromTW(Track *trackToCheck)
{
	int charge = -1;

	TATWpoint* twpoint = 0x0;
	if( trackToCheck->getNumPointsWithMeasurement() >= 0 )
	{
		for (int jTracking = trackToCheck->getNumPointsWithMeasurement() - 1; jTracking >= 0; --jTracking){
			if ( static_cast<genfit::PlanarMeasurement*>(trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId() != m_SensorIDMap->GetFitPlaneTW() ) continue;

			int MeasId = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getHitId();

			twpoint = ( (TATWntuPoint*) gTAGroot->FindDataDsc(FootActionDscName("TATWntuPoint"))->Object() )->GetPoint( m_SensorIDMap->GetHitIDFromMeasID(MeasId) ); //Find TW point associated to the track

			charge = twpoint->GetChargeZ();
			break;
		}
	}

	return charge;
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
TVector3 TAGFselectorBase::ExtrapolateToOuterTracker(Track* trackToFit, int whichPlane, TVector3& mom, bool backExtrap, int repId)
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
	if (tp == nullptr) {
		throw genfit::Exception("Track has no TrackPoint with fitterInfo", __LINE__, __FILE__);
	}

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
	trackRep->extrapolateToPlane(kfTest, m_SensorIDMap->GetFitPlane(whichPlane), false, false); //RZ: Local reference frame of "whichPlane"!!!

	TVector3 posi((kfTest.getState()[3]),(kfTest.getState()[4]), m_SensorIDMap->GetFitPlane(whichPlane)->getO().Z());
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
TVector3 TAGFselectorBase::ExtrapolateToOuterTracker(Track* trackToFit, int whichPlane, bool backExtrap, int repId)
{
	TVector3 mom;
	return ExtrapolateToOuterTracker(trackToFit, whichPlane, mom, backExtrap, repId);
}


//! \brief Get the MC true particle information from its MC track index
//!
//! \param[in] trackId Index of the MC track
//! \param[in,out] flukaID Pointer where to save the FLUKA ID
//! \param[in,out] charge Pointer where to save the true charge of the particle
//! \param[in,out] mass Pointer where to save the true mass of the particle
//! \param[in,out] posV Pointer where to save the initial position of the particle
//! \param[in,out] momV Pointer where to save the initial momentum of the particle
void TAGFselectorBase::GetTrueParticleType(int trackid, int* flukaID, int* charge, double* mass, TVector3* posV, TVector3* momV)
{

	TAMCpart* particle = m_McNtuEve->GetTrack(trackid);

	*flukaID = particle->GetFlukaID();
	*charge  = particle->GetCharge();
	*mass    = particle->GetMass();

	*posV = particle->GetInitPos();
	*momV = particle->GetInitP();
}

//! \brief Get the name of a track
//!
//! \param[in] tr Pointer to the GenFit track
//! \return Name of the track
TString TAGFselectorBase::GetRecoTrackName(Track* tr)
{
	for(map<TString, Track*>::iterator it = m_trackCategoryMap->begin(); it != m_trackCategoryMap->end(); ++it)
	{
		if(it->second == tr)
			return it->first;
	}
	Error("GetRecoTrackName()", "Track not found in Category Map!!"), exit(42);
}


//! \brief Get the name of a particle from its charge
//! \param[in] ch Charge of the particle
//! \return Name of the particle
string TAGFselectorBase::GetParticleNameFromCharge(int ch)
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


void TAGFselectorBase::PrintCurrentTracksMC()
{
	for( auto itTrack : m_trackTempMap )
	{
		cout << "Track::" << itTrack.first << "\tMC Ids::";
		for( int i = 0; i < (itTrack.second)->getNumPointsWithMeasurement(); ++i )
			cout << ListMCparticlesOfHit( (itTrack.second)->getPointWithMeasurement(i)->getRawMeasurement()->getHitId() ) << " ";
		cout << "\n";
	}
	cout << "\n";
}



//! \brief List the MC particle Ids of a hit (function used for debug purposes!)
//!
//! \param[in] HitId Global Id of the hit
//! \return MC particle Ids of the hit as a string
string TAGFselectorBase::ListMCparticlesOfHit(int HitId)
{
	TString output = "";
	if( !m_measParticleMC_collection || m_measParticleMC_collection->size() < 1 || m_measParticleMC_collection->find(HitId) == m_measParticleMC_collection->end() )
	{
		Info("ListMCparticlesOfHit()", "Problem in function for hit %d", HitId);
		return output.Data();
	}
	
	vector <int> mcIds = m_measParticleMC_collection->at(HitId);
	output += "[";
	for (auto id : mcIds)
		output += Form("%d,",id);
	output.Remove(TString::kTrailing, ',');
	output+="]";
	return output.Data();
}