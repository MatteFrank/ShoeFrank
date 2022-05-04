/*!
 \file TAGFselector.cxx
 \brief  Class for track finding/selection in GenFit Global Reconstruction
 \author R. Zarrella and M. Franchini
*/

#include "TAGFselector.hxx"

/*!
 \class TAGFselector
 \brief Class for track finding/selection in GenFit Global Reconstruction

 There are already different algorithms implemented for track finding:
 - "TrueParticle": MC truth particle tracking
 - "Sept2020": Data-like base algorithm, no MC used
 - "Linear": First implementation of track finding w/out magnetic field
*/


//----------------------------------------------------------------------------------------------------

//! \brief Default constructor
//!
//! \param[in] allHitMeas Pointer to the map containing all the measurements in GenFit format; the key is the GenFit FitPlane index
//! \param[in] chargeVect Pointer to vector of possible charges in the event (either measured from TW or MC truth)
//! \param[in] SensorIDmap Pointer to TAGFdetectorMap of the campaign
//! \param[in,out] trackCategoryMap Pointer to the map of selected tracks that have to be sent to the KFitter action
//! \param[in] measParticleMC_collection Pointer to the map containing the MC particles found in the event for each measurement; the key is the global index of the measurement
TAGFselector::TAGFselector( map< int, vector<AbsMeasurement*> >* allHitMeas, vector<int>* chargeVect, 
								TAGFdetectorMap* SensorIDmap , map<TString, Track*>* trackCategoryMap, 
								map< int, vector<int> >* measParticleMC_collection, bool IsMC) {

	m_allHitMeas = allHitMeas;
	m_chargeVect = chargeVect;
	m_SensorIDMap = SensorIDmap;
	m_trackCategoryMap = trackCategoryMap;
	m_measParticleMC_collection = measParticleMC_collection;
	m_IsMC = IsMC;

	m_debug = TAGrecoManager::GetPar()->Debug();

	m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

	if(TAGrecoManager::GetPar()->IncludeVT()) 
		m_VT_geo = (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object();
	if(TAGrecoManager::GetPar()->IncludeIT())
		m_IT_geo = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetDefParaName(), "TAITparGeo")->Object();
	if(TAGrecoManager::GetPar()->IncludeMSD())
		m_MSD_geo = (TAMSDparGeo*) gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object();
	if(TAGrecoManager::GetPar()->IncludeTW())
		m_TW_geo = (TATWparGeo*) gTAGroot->FindParaDsc(TATWparGeo::GetDefParaName(), "TATWparGeo")->Object();

	m_BeamEnergy = ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().Energy;

	if( m_debug > 1 )	cout << "Beam Energy::" << m_BeamEnergy << endl;

	if ( m_IsMC )	m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();

}


//! \brief Default destructor
//!
//! NEED TO CHECK IF SOMETHING IS MISSING HERE
TAGFselector::~TAGFselector()
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

	for(auto it = m_measParticleMC_collection->begin(); it != m_measParticleMC_collection->end(); ++it)
		it->second.clear();
}




//----------------------------------------------------------------------------------------------------

//! \brief Base function for track finding/selection/categorization
//!
//! This function gets called from the TAGactKFitter class
//! \return 0 if there were no errors
int TAGFselector::Categorize( ) {

	if ( FillTrackRepVector() != 0) 
		return -1;


	// fill m_mapTrack
	if ( TAGrecoManager::GetPar()->PreselectStrategy() == "TrueParticle" ){
		if ( !m_IsMC )
			Error("TAGFselector::Categorize()", "Asked TrueParticle tracking but running not on MC." ), exit(0); 
		if(m_debug > 0) cout << "TAGFselector::Categorize_TruthMC START\n";
		
		Categorize_TruthMC();

		if(m_debug > 0) cout << "TAGFselector::Categorize_TruthMC END\n";
	}
	else if ( TAGrecoManager::GetPar()->PreselectStrategy() == "Sept2020" )
		Categorize_dataLike();
	else if ( TAGrecoManager::GetPar()->PreselectStrategy() == "Linear" )
		Categorize_Linear();
	else if ( TAGrecoManager::GetPar()->PreselectStrategy() == "Backtracking")
		Categorize_Backtracking();
	else
		cout <<"ERROR :: TAGFselector::Categorize  -->	 TAGrecoManager::GetPar()->PreselectStrategy() not defined" << endl, exit(0);

	return 0;
}





//----------------------------------------------------------------------------------------------------

//! \brief Check the charges seen by TW and fill the track representation vector
//!
//! \return 0 if track representation vector was filled w/out errors
int TAGFselector::FillTrackRepVector()	{

	m_trackRepVec.clear();
	m_trackTempMap.clear();

	if(m_chargeVect->size() < 1)
	{
		Error("FillTrackRepVector()", "TW charge vector has non positive length! Vec size::%ld", m_chargeVect->size());
		return -1;
	}
	for(int i=0; i< m_chargeVect->size(); ++i)
	{
		if(m_debug > 0)
			cout << "TAGFselector::FillTrackRepVector() -- charge: "<< m_chargeVect->at(i) << "\n";
		
		AbsTrackRep* rep = new RKTrackRep( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( m_chargeVect->at(i) ) );
		m_trackRepVec.push_back( rep );
	}
	return 0;
}



//! \brief Count the particles generated in the MC and visible in the acceptance of FOOT
//!
//! This function if used for Selection Efficiency evaluation
//! \return Map containing the name of MC particles seen in the event ("H", "He", "Li"...) and their number
map<string, int> TAGFselector::CountParticleGenaratedAndVisible() { 

	if(m_debug > 0) cout << "TAGFselector::CountParticleGenaratedAndVisible --  Cycle on planes\t"  << m_SensorIDMap->GetFitPlanesN() << "\n";

	int particleCh;
	float mass;
	map<string, int> genCount_vector;
	TAMCntuPart* m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();
	
	for ( int iPart = 0; iPart < m_McNtuEve->GetTracksN(); iPart++ ) {

		
		TAMCpart* particle = m_McNtuEve->GetTrack(iPart);		
		
		particleCh = particle->GetCharge();
		mass = particle->GetMass();


		if ( particle->GetCharge() < 1 || particle->GetCharge() > ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber)
		 	continue;

		string outName, pdgName;

		switch(particleCh)	{

			case 1:	outName = "H";	break;
			case 2:	outName = "He";	break;
			case 3:	outName = "Li";	break;
			case 4:	outName = "Be";	break;
			case 5:	outName = "B";	break;
			case 6:	outName = "C";	break;
			case 7:	outName = "N";	break;
			case 8:	outName = "O";	break;
			default:
				outName = "fail";	break;
		}

		//CAREFUL HERE!! Think about the possibility of throwing an error -> skip particle for the moment
		if( outName == "fail" ) {continue;}

		pdgName = outName + int(round(mass/m_AMU));

		//CAREFUL HERE!!!!!!!!! FOOT TAGrecoManager file does not have Hydrogen and Helium isotopes!!!! Also think about throwing an error here...
		if ( !TAGrecoManager::GetPar()->Find_MCParticle( pdgName ) ) 	{
			if(m_debug > 0)  cout << "Found Particle not in MC list: " << pdgName << " num=" << iPart << "\n";
			continue;

		}

		if ( genCount_vector.find( outName ) == genCount_vector.end() ) 
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





//----------------------------------------------------------------------------------------------------

//! \brief Main function of MC track selection algorithm
//!
//! \return 0 if no errors occured during track selection
int TAGFselector::Categorize_TruthMC( )
{
	//Categorize events using MC truth
	int flukaID, charge;
	double mass;

	if(m_debug > 0)
	{
		cout << "MC particles:\n\n";
		for(auto it = m_measParticleMC_collection->begin(); it != m_measParticleMC_collection->end(); ++it)
		{
			for(int i=0; i < (it->second).size(); ++i)
			{
				cout << "GlobId::" << it->first << "\tId::" << i << "\tTrackId::" << it->second.at(i) << "\n";
			}
		}
	}

	//Cycle on FitPlanes
	if(m_debug > 0) cout << "Cycle on planes\t"  << m_SensorIDMap->GetFitPlanesN() << "\n";
	for(int iPlane = 0; iPlane < m_SensorIDMap->GetFitPlanesN(); ++iPlane)
	{

		if(m_debug > 0) cout << "Plane::" << iPlane << "\n";
		
		//Skip plane if no hit was found
		if(m_allHitMeas->find(iPlane) == m_allHitMeas->end()){continue;}

		if(m_debug > 0) cout << "Cycle on hits\n";

		int foundHit = 0;
	
		//Cycle on all measurements/clusters of plane
		for(vector<AbsMeasurement*>::iterator itHit = m_allHitMeas->at(iPlane).begin(); itHit != m_allHitMeas->at(iPlane).end(); ++itHit)
		{
			int MeasGlobId = (*itHit)->getHitId();

			//Cycle on all the MC particles that created that cluster/measurement
			if(m_debug > 0) cout << "Cycle on MC particles with GlobId::" << MeasGlobId << "\n";


			TVector3 posV;		//global coord [cm]
			TVector3 momV;	//GeV //considering that fragments have same velocity of beam this should be changed accordingly
			posV = TVector3(0,0,0);
			for( vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasGlobId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasGlobId).end(); ++itTrackMC )
			{
				GetTrueParticleType(*itTrackMC, &flukaID, &charge, &mass, &posV, &momV );

				TString outName, pdgName;

				switch(charge)
				{
					case 1:	outName = "H";	break;
					case 2:	outName = "He";	break;
					case 3:	outName = "Li";	break;
					case 4:	outName = "Be";	break;
					case 5:	outName = "B";	break;
					case 6:	outName = "C";	break;
					case 7:	outName = "N";	break;
					case 8:	outName = "O";	break;
					default:
						outName = "fail";	break;
				}

				//CAREFUL HERE!! Think about the possibility of throwing an error -> skip particle for the moment
				if( outName == "fail" ) {continue;}

				pdgName = outName + int(round(mass/m_AMU));

				//CAREFUL HERE!!!!!!!!! FOOT TAGrecoManager file does not have Hydrogen and Helium isotopes!!!! Also think about throwing an error here...
				if ( !TAGrecoManager::GetPar()->Find_MCParticle( pdgName.Data() ) ) 
				{
					if(m_debug > 0) cout << "Found Particle not in MC list: " << pdgName << "\n";
					continue;
				}

				//Set a unique ID for the particle that will be used in the map of Genfit tracks to fit
				outName += Form("_%d_%d", int(round(mass/m_AMU)), *itTrackMC);

				if ( m_debug > 0 ) cout << "\tSelected Category: " << outName << "  flukaID=" << flukaID << "  partID="<< *itTrackMC << "  charge="<<charge << "  mass="<<mass<< "\n";

				if(m_trackCategoryMap->find(outName) == m_trackCategoryMap->end())
				{
					if(m_debug > 0) {
						cout << "Found new particle!! " << outName << "\n";
						posV.Print();
						momV.Print();
					}
					(*m_trackCategoryMap)[outName] = new Track();
					posV.SetMag(posV.Mag()*0.99);
					m_trackCategoryMap->at(outName)->setStateSeed(posV, momV);

					m_trackCategoryMap->at(outName)->addTrackRep( new RKTrackRep( UpdatePDG::GetPDG()->GetPdgCode( pdgName.Data() ) ) );
				}

				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> ( *itHit ) )->clone() ;

				m_trackCategoryMap->at(outName)->insertMeasurement( hitToAdd );

				foundHit++;
			
			} //End of loop on MC particles per cluster/measurement

		} //End of loop on measurements per sensor
	
	} //End of loop on sensors

	return 0;
}



//----------------------------------------------------------------------------------------------------
/********************************** FORWARD TRACKING ************************************************/
//----------------------------------------------------------------------------------------------------
//! \brief Categorize tracks and representations using a Data-Like approach
//!
//! This algorithm selects the track points avoiding MC truth info; Currently depending on VT presence
//! \return 0 if no errors occured during track finding
int TAGFselector::Categorize_dataLike( ) {

	if( m_debug > 1 ) cout << "******* START OF VT CYCLE *********\n";

	if(!TAGrecoManager::GetPar()->IncludeVT())
	{
		Error("Categorize_dataLike()", "Vertex is needed for data-like selection!");
		throw -1;
	}
	else
		CategorizeVT();

	if( m_debug > 1 ) cout << "******** END OF VT CYCLE **********\n";

	if( m_debug > 1 ) cout << "******* START OF IT CYCLE *********\n";
	
	if(TAGrecoManager::GetPar()->IncludeIT())
		CategorizeIT();
	
	if( m_debug > 1 ) cout << "******** END OF IT CYCLE **********\n";

	if( m_debug > 1 ) cout << "******* START OF MSD CYCLE *********\n";
	
	if(TAGrecoManager::GetPar()->IncludeMSD())
		CategorizeMSD();
	
	if( m_debug > 1 ) cout << "******** END OF MSD CYCLE **********\n";

	if( m_debug > 1 ) cout << "******* START OF TW CYCLE *********\n";
	
	if(TAGrecoManager::GetPar()->IncludeTW())
		CategorizeTW();
	
	if( m_debug > 1 ) cout << "******** END OF TW CYCLE **********\n";

	FillTrackCategoryMap();

	return 0;
}


//! \brief Function used to handle linear tracks when no magnetic field is used
//!
//! \return 0 if no errors occured during track finding
int TAGFselector::Categorize_Linear()
{
	if( m_debug > 1 ) cout << "******* START OF VT CYCLE *********\n";

	if(!TAGrecoManager::GetPar()->IncludeVT())
	{
		Error("Categorize_dataLike()", "Vertex is needed for linear selection!");
		throw -1;
	}
	else
		CategorizeVT();

	if( m_debug > 1 ) cout << "******** END OF VT CYCLE **********\n";

	if( m_debug > 1 ) cout << "******* START OF IT CYCLE *********\n";
	
	if(TAGrecoManager::GetPar()->IncludeIT())
		CategorizeIT();
	
	if( m_debug > 1 ) cout << "******** END OF IT CYCLE **********\n";

	if( m_debug > 1 ) cout << "******* START OF MSD CYCLE *********\n";
	
	if(TAGrecoManager::GetPar()->IncludeMSD())
		CategorizeMSD_Linear();
	
	if( m_debug > 1 ) cout << "******** END OF MSD CYCLE **********\n";

	if( m_debug > 1 ) cout << "******* START OF TW CYCLE *********\n";
	
	if(TAGrecoManager::GetPar()->IncludeTW())
		CategorizeTW_Linear();
	
	if( m_debug > 1 ) cout << "******** END OF TW CYCLE **********\n";

	double dPVal = 1.E-3; // convergence criterion
	KalmanFitter* preFitter = new KalmanFitter(1, dPVal);

	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack)
	{
		int Z_Hypo = GetChargeFromTW(itTrack->second);
		if( Z_Hypo == -1 )
		{
			Z_Hypo = ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber;
			itTrack->second->addTrackRep(new RKTrackRep(UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( Z_Hypo )));
		}
		else
		{
			for ( int nRep=0; nRep < m_trackRepVec.size(); nRep++)
				if( m_trackRepVec.at(nRep)->getPDGCharge() == Z_Hypo )	(itTrack->second)->addTrackRep( m_trackRepVec.at( nRep )->clone() );
		}

		//Get first track measurement and use it to set the track seed
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		int SensorId = m_SensorIDMap->GetSensorIDFromMeasID( firstTrackMeas->getHitId() );
		TVector3 pos = TVector3(firstTrackMeas->getRawHitCoords()(0), firstTrackMeas->getRawHitCoords()(1), 0);
		pos = m_GeoTrafo->FromVTLocalToGlobal( m_VT_geo->Sensor2Detector(SensorId, pos ) ); //Move position to global coords

		pos = pos - m_trackSlopeMap[itTrack->first]*pos.Z();

		TVector3 mom = m_trackSlopeMap[itTrack->first];
		double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(Z_Hypo) );
		int A_Hypo = round(mass_Hypo/m_AMU);

		mom.SetMag(TMath::Sqrt( pow(m_BeamEnergy*A_Hypo,2) + 2*mass_Hypo*m_BeamEnergy*A_Hypo ));
		itTrack->second->setStateSeed(pos, mom);

		// preFitter->processTrackWithRep( itTrack->second, itTrack->second->getCardinalRep() );
	}
	delete preFitter;

	FillTrackCategoryMap();

	return 0;
}



//! \brief Track selection in the VT for forward tracking
//!
//! The algorithm currently starts from VT tracklets and checks the number of points in them
void TAGFselector::CategorizeVT()
{
	TAVTntuVertex* vertexContainer = (TAVTntuVertex*) gTAGroot->FindDataDsc("vtVtx", "TAVTntuVertex")->Object();
		//cluster test
	TAVTntuCluster* vtntuclus = (TAVTntuCluster*) gTAGroot->FindDataDsc("vtClus","TAVTntuCluster")->Object(); //To find the riht clus Index -> TO BE CHANGED!

	int vertexNumber = vertexContainer->GetVertexN();
	TAVTvertex* vtxPD   = 0x0; //NEW

	TVector3 pos_(0, 0, 0);		//global coord [cm]
    TVector3 mom_(0, 0, 2.);	//GeV //considering that fragments have same velocity of beam this should be changed accordingly

    if ( m_debug > 1 )		cout << "TAGFselector::CategorizeVT()  --  " << vertexNumber << "\n";

	//loop over all vertices
	for (Int_t iVtx = 0; iVtx < vertexNumber; ++iVtx) {
		vtxPD = vertexContainer->GetVertex(iVtx);
		if (vtxPD == 0x0){
			cout << "Vertex number " << iVtx << " seems to be empty\n";
			continue;
		}
		else if( !m_IsMC && !vtxPD->IsBmMatched() )
		{
			if(m_debug > 0)
			{
				Info("CategorizeVT()", "In event %d: vertex %d found but not matched with BM tracks; Skipping...",gTAGroot->CurrentEventId().EventNumber(), iVtx);
			}
			continue;
		}

		if ( m_debug > 0 )		cout  << "vertex number " << iVtx << " has this nr of tracks " << vtxPD->GetTracksN() <<"\n";

		//loop over tracks for each Vertex
		for (int iTrack = 0; iTrack < vtxPD->GetTracksN(); iTrack++) {

			TAVTtrack* tracklet = vtxPD->GetTrack( iTrack );

			// N clusters per tracklet
			int ncluster = tracklet->GetClustersN();
			if( m_debug > 0 )
				cout << "tracklet " << iTrack << " has " << ncluster << " clusters" << "\n";

			if(ncluster < 3){ continue; }

			Track*  fitTrack_ = new Track();  // container of the tracking objects
			fitTrack_->setStateSeed(pos_, mom_);

			// loop over clusters in the tracklet --> inverse numbering
			for (int iCluster = ncluster - 1; iCluster >= 0; iCluster--) {

				TAVTcluster* clus = (TAVTcluster*) tracklet->GetCluster( iCluster );
				if (!clus->IsValid()) continue;

				int sensor = clus->GetSensorIdx();
				int clusIdPerSensor = clus->GetClusterIdx();
				int plane = m_SensorIDMap->GetFitPlaneID("VT", sensor);

				int index=0;
				while( clusIdPerSensor != vtntuclus->GetCluster(sensor, index)->GetClusterIdx() )
					index++;

				// if ( m_allHitMeas->find( plane ) == m_allHitMeas->end() )	continue;
				// if ( m_allHitMeas->at(plane).find( index ) == m_allHitMeas->at(plane).end() )	continue;


				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (  m_allHitMeas->at(plane).at(index) ))->clone();
				fitTrack_->insertMeasurement( hitToAdd );

				if( m_debug > 1) 
				{
					cout << "VTX::SENSOR::" << sensor << "\n";
					cout << "VTX::FITPLANE::" << plane << "\n";
					cout << "VTX::CLUS_IDx::" << clusIdPerSensor << "\n";
					cout << "VTX::CLUS_ID::" << index << "\n";
					cout << "VTX::CLUS_POS::"; 
					hitToAdd->getRawHitCoords().Print();
				}


				if ( m_debug > 1 && m_IsMC ) {
					vector<int> iPart = m_measParticleMC_collection->at( hitToAdd->getHitId() );
					cout << "\t-- Truth particles of the measurement:\n";
					for (int k=0; k< iPart.size(); k++) {
						TAMCpart* particle = m_McNtuEve->GetTrack( iPart.at(k) );
						TVector3 mcMom = particle->GetInitP();
						cout << "\t\t-charge: " << particle->GetCharge() << "   mom:"; mcMom.Print();
					}
				}

			}	// end cluster loop

			if (fitTrack_->getNumPointsWithMeasurement() > 4 || fitTrack_->getNumPointsWithMeasurement() < 3){
				Warning("Categorize_dataLike()", "Track with %d measurements found in VTX => rejected!", fitTrack_->getNumPointsWithMeasurement());
				continue;
			}

			for ( int nRep=0; nRep < m_trackRepVec.size(); nRep++) {
				fitTrack_->addTrackRep( m_trackRepVec.at( nRep )->clone() );
				if ( m_debug > 1 ) {
					cout << "TAGFselector::CategorizeVT() -- rep charge = " << m_trackRepVec.at( nRep )->getPDGCharge() << "\n";
				}
			}

			int IdTrack = iVtx*1000 + iTrack;

			m_trackTempMap[IdTrack] = new Track(*fitTrack_);
			m_trackSlopeMap[IdTrack] = tracklet->GetSlopeZ();
		
			delete fitTrack_;
		}	// end track loop

	} //end loop on vertices

	if(m_trackTempMap.size() == 0)
		Warning("CategorizeVT()","No VT tracklet found in the event!");
}


//! \brief Track selection at IT level
//!
//! Currently this step is performed w/ a linear extrapolation
void TAGFselector::CategorizeIT()	{

	// ExtrapFromVTXtoIT 
	TVector3 tmpExtrap, tmpVTX;

	// same index if VTX_tracklets (for one vertex..)
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end();) {
		int addedMeas = 0;
		
		//Get last VT measurement for extrapolation
		PlanarMeasurement* lastVTXMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(-1)->getRawMeasurement());
		int VTsensorId = m_SensorIDMap->GetSensorIDFromMeasID( lastVTXMeas->getHitId() );
		tmpVTX = TVector3(lastVTXMeas->getRawHitCoords()(0), lastVTXMeas->getRawHitCoords()(1), 0);
		tmpVTX = m_GeoTrafo->FromVTLocalToGlobal( m_VT_geo->Sensor2Detector(VTsensorId, tmpVTX) ); //move to global coords

		//Get some parameters for IT FitPlanes
		int maxITdetPlane = m_SensorIDMap->GetMaxFitPlane("IT");
		int minITdetPlane = m_SensorIDMap->GetMinFitPlane("IT");
		double tmpITz;
		vector<float>* allZinIT = m_SensorIDMap->GetPossibleITz();


		for ( int iz = 0; iz < allZinIT->size(); iz++ ) {
			
			tmpITz = allZinIT->at(iz);
			tmpExtrap = tmpVTX + m_trackSlopeMap[itTrack->first]*( tmpITz - tmpVTX.Z() );

			vector<int>* planesAtZ  = m_SensorIDMap->GetPlanesAtZ( tmpITz );

			// select a matching plane -> CHECK!!!!!!!!!!!!!!!
			// RZ: there is a potentially bad issue here with the bending plane!!! the intersection might be in another sensor since it is done with a linear extrapolation. Would it be better to only check the y? how much do we risk of f-ing this up?

			Int_t sensorId;
			for ( vector<int>::iterator iPlane = planesAtZ->begin(); iPlane != planesAtZ->end(); iPlane++ ) {
				// cout << "Found plane::" << *iPlane << " at z::" << tmpITz << "\n";
				if( !m_SensorIDMap->GetSensorID(*iPlane, &sensorId) )
				{
					Error("CategorizeIT()", "Sensor not found for Genfit plane %d!", *iPlane);
					throw -1;
				}
				TVector3 guessOnPlaneIT = m_GeoTrafo->FromGlobalToITLocal( tmpExtrap ); //RZ: IsInActive controls local or global variables????
				guessOnPlaneIT = m_IT_geo->Detector2Sensor(sensorId, guessOnPlaneIT); //Move to local coords
				if ( !m_SensorIDMap->GetFitPlane( *iPlane )->isInActiveY( guessOnPlaneIT.Y() ) )
				{
					if(m_debug > 1) cout << "Extrapolation to IT not in active region of sensor " << sensorId << endl;
					continue;
				}

				if(m_debug > 1)	cout << "Extrapolation to IT is in active area of sensor " << sensorId << endl;

				int sensorMatch = (*iPlane);

				int indexOfMinY = -1.;
				double distanceInY = .1, distanceInX;

				if ( m_allHitMeas->find( sensorMatch ) == m_allHitMeas->end() )   {
					if( m_debug > 1)	cout << "TAGFselector::CategorizeIT() -- WARNING extapolated plane empty!\n";
					continue;
				}

				do {
					int count = 0;
					for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( sensorMatch ).begin(); it != m_allHitMeas->at( sensorMatch ).end(); ++it){
					
						// check -> RZ: why do we need this check???? is it for debug only??
						if ( m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != sensorMatch )	cout << "TAGFselector::Categorize_dataLike() --> ERROR IT" <<endl, exit(0);

						// find hit at minimum distance
						if ( fabs( guessOnPlaneIT.Y() - (*it)->getRawHitCoords()(1) ) < distanceInY ){
							distanceInY = fabs(guessOnPlaneIT.Y() - (*it)->getRawHitCoords()(1));
							distanceInX = fabs(guessOnPlaneIT.X() - (*it)->getRawHitCoords()(0));
							indexOfMinY = count;
						}
						count++;
					}
					distanceInY += .05; 

				}while (indexOfMinY == -1 && distanceInY < .3);
				
				//Insert measurement in GF track if found!
				if (indexOfMinY != -1 && distanceInX < 1.){
					if(m_debug > 0) cout << "ITcheck\tTrack::" << itTrack->first << "\tdistanceInY::" << distanceInY << "\tdistanceinX::" << distanceInX << "\n";
					AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> ( m_allHitMeas->at(sensorMatch).at(indexOfMinY) ))->clone();
					(itTrack->second)->insertMeasurement( hitToAdd );
					addedMeas++;
					
				}
			}	// end loop on IT planes
		} // end loop over z

		++itTrack;

	}	// end loop on GF Track candidates
}





//! \brief Track selection at the MSD for data-like "Sept2020" selection
//!
//! This step uses a Kalman Filter extrapolation to find the MSD measurements of the track
void TAGFselector::CategorizeMSD()	{

	//RZ: CHECK!!! ADDED TO AVOID ERRORS
	int findMSD;

	KalmanFitter* m_fitter_extrapolation = new KalmanFitter(1);

	// Extrapolate to MSD
	// same index if VTX_tracklets (for one vertex..)
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end();) {

		findMSD=0;

		int maxMSDdetPlane = m_SensorIDMap->GetMaxFitPlane("MSD");
		int minMSDdetPlane = m_SensorIDMap->GetMinFitPlane("MSD");

		//RZ: SET STATE SEED
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		int VTsensorId = m_SensorIDMap->GetSensorIDFromMeasID( firstTrackMeas->getHitId() );
		TVector3 pos = TVector3( firstTrackMeas->getRawHitCoords()(0), firstTrackMeas->getRawHitCoords()(1), 0);
		pos = m_GeoTrafo->FromVTLocalToGlobal( m_VT_geo->Sensor2Detector(VTsensorId, pos) );
		
		if(m_debug > 0)
		{
			cout << "***POS SEED***\nVTX: "; pos.Print();
		}

		pos = pos - m_trackSlopeMap[itTrack->first]*pos.Z();

		if(m_debug > 0)
		{
			cout << "TGT: "; pos.Print();
		}

		//Set mom seed for extrapolation: use track slope and then scale for particle mass hypo with beta of the primary
		TVector3 mom = m_trackSlopeMap[itTrack->first];
		
		if(m_debug > 0)
		{
			cout << "\n***MOM SEED***\nDIR: "; mom.Print();
		}

		m_fitter_extrapolation->setMaxIterations(1);
		float chi2 = 10000;
		int idCardRep = -1;
		if(m_debug > 0)	cout << "\nSelectorKalmanGF::CategorizeMSD()  --  number of Reps = "<< itTrack->second->getNumReps() <<"\n";

		for(int repId = 0; repId < itTrack->second->getNumReps(); ++repId)
		{
			Track* testTrack = new Track(* itTrack->second );
			int Z_Hypo = testTrack->getTrackRep(repId)->getPDGCharge();
			double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(Z_Hypo) );
			int A_Hypo = round(mass_Hypo/m_AMU);

			if(m_debug > 0)	cout << "Z_Hypo::" << Z_Hypo << "\tA_Hypo::" << A_Hypo << "\n";

			mom.SetMag(TMath::Sqrt( pow(m_BeamEnergy*A_Hypo,2) + 2*mass_Hypo*m_BeamEnergy*A_Hypo ));

			if(m_debug > 0)
			{
				cout << "MOM: "; mom.Print();
			}

			testTrack->setStateSeed(pos, mom);
			try
			{
				m_fitter_extrapolation->processTrackWithRep( testTrack, testTrack->getTrackRep(repId) );
				
				if(m_debug > 0)
				{
					cout << "Processed\n";
					TVector3 guessOnMSD = ExtrapolateToOuterTracker( testTrack, m_SensorIDMap->GetMinFitPlane("MSD"), repId); //RZ: In local reference frame of FitPlane!!
					cout << "GuessOnMSD: ";
					guessOnMSD.Print();
					cout << "\t\t charge = " << Z_Hypo << "  chi2 = " << m_fitter_extrapolation->getRedChiSqu(testTrack, testTrack->getTrackRep(repId) ) << "\n";
				}

				if(chi2 > m_fitter_extrapolation->getRedChiSqu(testTrack, testTrack->getTrackRep(repId) ))
				{
					chi2 = m_fitter_extrapolation->getRedChiSqu(testTrack, testTrack->getTrackRep(repId) );
					idCardRep = repId;
				}
			}
			catch (genfit::Exception& e)
			{
				std::cerr << e.what();
				std::cerr << "MSD extrapolation: Exception, next rep\n";
				continue;
			}
			delete testTrack;

		}

		//Set cardinal rep as the one with the best chi2
		itTrack->second->setCardinalRep( idCardRep );
		
		double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( itTrack->second->getCardinalRep()->getPDGCharge() ) );
		int A_Hypo = round(mass_Hypo/m_AMU);

		mom.SetMag( TMath::Sqrt( pow(m_BeamEnergy*A_Hypo,2) + 2*mass_Hypo*m_BeamEnergy*A_Hypo ));

		itTrack->second->setStateSeed(pos, mom);
		m_fitter_extrapolation->processTrackWithRep( itTrack->second, itTrack->second->getCardinalRep() );

		if(m_debug > 0)
		{
			itTrack->second->getCardinalRep()->Print();
			cout << "CardRep charge::" << itTrack->second->getCardinalRep()->getPDGCharge() << "\n";
		}

		//------------------------------------------------------------------------------------------

		
		for ( int MSDnPlane = minMSDdetPlane; MSDnPlane <= maxMSDdetPlane; MSDnPlane++ ) {

			TVector3 guessOnMSD = ExtrapolateToOuterTracker( itTrack->second, MSDnPlane ); //RZ: NOW LOCAL COORDS!!
			if( !m_SensorIDMap->GetFitPlane(MSDnPlane)->isInActive( guessOnMSD.x(), guessOnMSD.y() ) )
				continue;

			int indexOfMinY = -1;
			int count = 0;
			double distanceInY = 1;
			int sensorMatch = MSDnPlane;

			if ( m_allHitMeas->find( MSDnPlane ) == m_allHitMeas->end() ) {
				if(m_debug > 0) cout << "TAGFselector::CategorizeMSD() -- no measurement found in MSDnPlane "<< MSDnPlane<<"\n";
				continue;
			}

			for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( MSDnPlane ).begin(); it != m_allHitMeas->at( MSDnPlane ).end(); ++it){
			// cout << "TAGFselector::CategorizeMSD()     MSDcheck4\n";
				if ( m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != sensorMatch )	cout << "TAGFselector::Categorize_dataLike() --> ERROR MSD" <<endl, exit(0);

				//RZ: CHECK -> AVOID ERRORS
				double distanceFromHit;
				string strip;

				if ( ! static_cast<PlanarMeasurement*>(*it)->getStripV() )
				{
					distanceFromHit = fabs(guessOnMSD.X() - (*it)->getRawHitCoords()(0));
					strip = "X";
				}
				else
				{
					distanceFromHit = fabs(guessOnMSD.Y() - (*it)->getRawHitCoords()(0));
					strip = "Y";
				}

				// find hit at minimum distance
				if ( distanceFromHit < distanceInY ){
					if(m_debug > 0) cout << "MSDcheck\tPlane::" << sensorMatch << "\tTrack::" << itTrack->first << "\tdistanceFromHit::" << distanceFromHit << "\tStrip::" << strip << "\n";
					distanceInY = distanceFromHit;
					indexOfMinY = count;
				}
				count++;
			}

			// insert measurement in GF Track
			if (indexOfMinY != -1){

				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(sensorMatch).at(indexOfMinY)))->clone();
				(itTrack->second)->insertMeasurement( hitToAdd );
				findMSD++;
			}

		} // end loop MSD planes
		
		++itTrack;

	}// end loop on GF Track candidates
	delete m_fitter_extrapolation;

}


//! \brief Track selection at the MSD when no magnetic field is present ("Linear")
//!
//! This step is performed through a linear extrapolation at the MSD
void TAGFselector::CategorizeMSD_Linear()
{
	int findMSD;

	// Extrapolate to MSD
	// same index if VTX_tracklets (for one vertex..)
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end();) {

		findMSD=0;

		int maxMSDdetPlane = m_SensorIDMap->GetMaxFitPlane("MSD");
		int minMSDdetPlane = m_SensorIDMap->GetMinFitPlane("MSD");

		//RZ: SET STATE SEED
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		int VTsensorId = m_SensorIDMap->GetSensorIDFromMeasID( firstTrackMeas->getHitId() );
		TVector3 pos = TVector3( firstTrackMeas->getRawHitCoords()(0), firstTrackMeas->getRawHitCoords()(1), 0);
		pos = m_GeoTrafo->FromVTLocalToGlobal( m_VT_geo->Sensor2Detector(VTsensorId, pos) );
		
		if(m_debug > 0)
		{
			cout << "***POS SEED***\nVTX: "; pos.Print();
		}

		for ( int MSDnPlane = minMSDdetPlane; MSDnPlane <= maxMSDdetPlane; MSDnPlane++ ) {

			int sensorId;
			if( !m_SensorIDMap->GetSensorID(MSDnPlane, &sensorId) )
			{
				Error("CategorizeIT()", "Sensor not found for Genfit plane %d!", MSDnPlane);
				throw -1;
			}
			TVector3 guessOnMSD = m_GeoTrafo->FromGlobalToMSDLocal(pos + m_trackSlopeMap[itTrack->first]*(m_SensorIDMap->GetFitPlane(MSDnPlane)->getO().Z() - pos.Z()));
			guessOnMSD = m_MSD_geo->Detector2Sensor( sensorId,  guessOnMSD );
			
			if( !m_SensorIDMap->GetFitPlane(MSDnPlane)->isInActive( guessOnMSD.x(), guessOnMSD.y() ) )
				continue;

			int indexOfMinY = -1;
			int count = 0;
			double distanceInY = 1;
			int sensorMatch = MSDnPlane;

			if ( m_allHitMeas->find( MSDnPlane ) == m_allHitMeas->end() ) {
				if(m_debug > 0) cout << "TAGFselector::CategorizeMSD() -- no measurement found in MSDnPlane "<< MSDnPlane<<"\n";
				continue;
			}

			for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( MSDnPlane ).begin(); it != m_allHitMeas->at( MSDnPlane ).end(); ++it){

				if ( m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != sensorMatch )	cout << "TAGFselector::Categorize_dataLike() --> ERROR MSD" <<endl, exit(0);

				//RZ: CHECK -> AVOID ERRORS
				double distanceFromHit;
				string strip;

				if ( ! static_cast<PlanarMeasurement*>(*it)->getStripV() )
				{
					distanceFromHit = fabs(guessOnMSD.X() - (*it)->getRawHitCoords()(0));
					strip = "X";
				}
				else
				{
					distanceFromHit = fabs(guessOnMSD.Y() - (*it)->getRawHitCoords()(0));
					strip = "Y";
				}

				// find hit at minimum distance
				if ( distanceFromHit < distanceInY ){
					if(m_debug > 0) cout << "MSDcheck\tPlane::" << sensorMatch << "\tTrack::" << itTrack->first << "\tdistanceFromHit::" << distanceFromHit << "\tStrip::" << strip << "\n";
					distanceInY = distanceFromHit;
					indexOfMinY = count;
				}
				count++;
			}

			// insert measurementi in GF Track
			if (indexOfMinY != -1){

				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(sensorMatch).at(indexOfMinY)))->clone();
				(itTrack->second)->insertMeasurement( hitToAdd );
				findMSD++;
			}

		} // end loop MSD planes

		++itTrack;

	}// end loop on GF Track candidates

	return;
}



//! \brief TW track selection for data-like "Sept2020" categorization
//!
//! This step uses a Kalman Filter extrapolation
void TAGFselector::CategorizeTW()
{
	int planeTW = m_SensorIDMap->GetFitPlaneTW();
	if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() ) {
		if(m_debug > 0) cout << "TAGFselector::CategorizeTW() -- no measurement found in TW layer\n";
		return;
	}

	// Extrapolate to TW
	KalmanFitter* m_fitter_extrapolation = new KalmanFitter(1);
	m_fitter_extrapolation->setMaxIterations(1);
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); itTrack++) 
	{
		m_fitter_extrapolation->processTrackWithRep(itTrack->second, itTrack->second->getCardinalRep() );

		TVector3 guessOnTW;
		try
		{
			guessOnTW = ExtrapolateToOuterTracker( itTrack->second, planeTW); //RZ: Local coords!
		}
		catch(genfit::Exception& ex)
		{
			std::cerr << ex.what();
			std::cerr << "Exception, skip track candidate\n";
			continue;
		}

		if( m_debug > 0) cout << "guessOnTW " << guessOnTW.X() << "  " << guessOnTW.Y() << "\n";

		//calculate distance TW point
		double TWdistance = 4.;
		int indexOfMin = -1;
		int count = 0;

		for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( planeTW ).begin(); it != m_allHitMeas->at( planeTW ).end(); ++it){

			if (  m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != planeTW )
				cout << "TAGFselector::Categorize_dataLike() --> ERROR TW" <<endl, exit(0);

			double distanceFromHit = sqrt( ( guessOnTW.X() - (*it)->getRawHitCoords()(0) )*( guessOnTW.X() - (*it)->getRawHitCoords()(0) ) +
					( guessOnTW.Y() - (*it)->getRawHitCoords()(1) )*( guessOnTW.Y() - (*it)->getRawHitCoords()(1) ) );
			
			if( m_debug > 0) cout << "measurement: " << (*it)->getRawHitCoords()(0) << "   " << (*it)->getRawHitCoords()(1)<< "\n";

			if ( distanceFromHit < TWdistance )	{
				TWdistance = distanceFromHit;
				indexOfMin = count;
			}

			count++;
		}	// end of TW hits

		if (indexOfMin != -1)	{
			AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(planeTW).at(indexOfMin)))->clone();
			(itTrack->second)->insertMeasurement( hitToAdd );
		}
	}
	delete m_fitter_extrapolation;
	
}



//! \brief TW track selection when no magnetic field is present ("Linear")
//!
//! This step uses a linear extrapolation at the TW
void TAGFselector::CategorizeTW_Linear()
{
	// Extrapolate to TW
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); itTrack++) 
	{
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		int VTsensorId = m_SensorIDMap->GetSensorIDFromMeasID( firstTrackMeas->getHitId() );
		TVector3 pos = TVector3( firstTrackMeas->getRawHitCoords()(0), firstTrackMeas->getRawHitCoords()(1), 0);
		pos = m_GeoTrafo->FromVTLocalToGlobal( m_VT_geo->Sensor2Detector(VTsensorId, pos) );

		int planeTW = m_SensorIDMap->GetFitPlaneTW();
		TVector3 guessOnTW =  m_GeoTrafo->FromGlobalToTWLocal( pos + m_trackSlopeMap[itTrack->first]*(m_SensorIDMap->GetFitPlane(planeTW)->getO().Z() - pos.Z()) );
		// guessOnTW = m_TW_geo->Detector2Sensor( 0, guessOnTW );

		if( m_debug > 1) cout << "guessOnTW " << guessOnTW.X() << "  " << guessOnTW.Y() << "\n";

		//calculate distance TW point
		double TWdistance = 4.;
		int indexOfMin = -1;
		int count = 0;

		//RZ -> See if this check can be done outside this cycle... it seems a much more general skip
		if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() ) {
			if(m_debug > 0) cout << "TAGFselector::CategorizeTW() -- no measurement found in TW layer\n";
			continue;
		}
		double distInX, distInY;

		for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( planeTW ).begin(); it != m_allHitMeas->at( planeTW ).end(); ++it){

			if (  m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != planeTW )
				cout << "TAGFselector::Categorize_dataLike() --> ERROR TW" <<endl, exit(0);

			double distanceFromHit = sqrt( ( guessOnTW.X() - (*it)->getRawHitCoords()(0) )*( guessOnTW.X() - (*it)->getRawHitCoords()(0) ) +
					( guessOnTW.Y() - (*it)->getRawHitCoords()(1) )*( guessOnTW.Y() - (*it)->getRawHitCoords()(1) ) );
			
			if( m_debug > 0) cout << "measurement: " << (*it)->getRawHitCoords()(0) << "   " << (*it)->getRawHitCoords()(1)<< "\n";

			if ( distanceFromHit < TWdistance )	{
				distInX = guessOnTW.X() - (*it)->getRawHitCoords()(0);
				distInY = guessOnTW.Y() - (*it)->getRawHitCoords()(1);
				TWdistance = distanceFromHit;
				indexOfMin = count;
			}

			count++;
		}	// end of TW hits

		if (indexOfMin != -1)	{
			AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(planeTW).at(indexOfMin)))->clone();
			(itTrack->second)->insertMeasurement( hitToAdd );
		}
	}


	return;
}



//--------------------------------------------------------------------------------------------
/**********************************BACKWARD TRACKING*****************************************/
//--------------------------------------------------------------------------------------------

int TAGFselector::Categorize_Backtracking()
{
	if( !TAGrecoManager::GetPar()->IncludeTW() || !TAGrecoManager::GetPar()->IncludeMSD() )
	{
		Error("Categorize_Backtracking()", "TW and MSD are needed for backtracking!");
		exit(0);
	}

	if( m_debug > 1)	Info("Categorize_Backtracking()", "Backtracking START!!");

	BackTracklets();

	if( m_debug > 1)	Info("Categorize_Backtracking()", "BackTracklets created!");

	if( TAGrecoManager::GetPar()->IncludeIT() )
	{
		if( m_debug > 1)	Info("Categorize_Backtracking()", "Start of IT cycle!");
		CategorizeIT_back();
		if( m_debug > 1)	Info("Categorize_Backtracking()", "End of IT cycle!");
	}

	if( TAGrecoManager::GetPar()->IncludeVT() )
	{
		if( m_debug > 1)	Info("Categorize_Backtracking()", "Start of VT cycle!");
		CategorizeVT_back();
		if( m_debug > 1)	Info("Categorize_Backtracking()", "End of VT cycle!");
	}

	FillTrackCategoryMap();

	return 0;
}


void TAGFselector::BackTracklets()
{
	int planeTW = m_SensorIDMap->GetFitPlaneTW();
	if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() ) {
		if(m_debug > 0) cout << "TAGFselector::BackTracklets() -- no measurement found in TW layer\n";
		return;
	}

	KalmanFitter* m_fitter_extrapolation = new KalmanFitter(1);
	m_fitter_extrapolation->setMaxIterations(1);
	int TrackCounter=0;
	// Cycle on TW points
	for (vector<AbsMeasurement*>::iterator itTW = m_allHitMeas->at( planeTW ).begin(); itTW != m_allHitMeas->at( planeTW ).end(); ++itTW) 
	{
		//LOCAL COORDS!!!!
		float xTW = (*itTW)->getRawHitCoords()(0);
		float yTW = (*itTW)->getRawHitCoords()(1);
		float zTW = 0;// m_SensorIDMap->GetFitPlane(m_SensorIDMap->GetFitPlaneTW())->getO().Z();
		TVector3 globPosTW = m_GeoTrafo->FromTWLocalToGlobal(TVector3(xTW, yTW, zTW));
		int MSDPlane1 = m_SensorIDMap->GetMinFitPlane("MSD");
		int MSDPlane2 = MSDPlane1+1;
		bool isMSD1y, isMSD2y;
		float xMSD = -100., yMSD = -100.;
		float zMSD = m_SensorIDMap->GetFitPlane(MSDPlane1)->getO().Z();

		//Cycle on the first 2 planes of MSD
		for(vector<AbsMeasurement*>::iterator itMSD1 = m_allHitMeas->at(MSDPlane1).begin(); itMSD1 != m_allHitMeas->at(MSDPlane1).end(); ++itMSD1)
		{
			isMSD1y = static_cast<PlanarMeasurement*>(*itMSD1)->getStripV();

			if( isMSD1y )
				yMSD = (*itMSD1)->getRawHitCoords()(0);
				// yMSD = m_GeoTrafo->FromMSDLocalToGlobal(TVector3(0, (*itMSD1)->getRawHitCoords()(0), 0)).Y(); //RZ: Check for local coordinates!!!!
			else
				xMSD = (*itMSD1)->getRawHitCoords()(0);
				// xMSD = m_GeoTrafo->FromMSDLocalToGlobal(TVector3((*itMSD1)->getRawHitCoords()(0), 0, 0)).X();

			for(vector<AbsMeasurement*>::iterator itMSD2 = m_allHitMeas->at(MSDPlane2).begin(); itMSD2 != m_allHitMeas->at(MSDPlane2).end(); ++itMSD2)
			{
				isMSD2y = static_cast<PlanarMeasurement*>(*itMSD2)->getStripV();

				if( (isMSD1y && isMSD2y) || (!isMSD1y && !isMSD2y))
					Warning("BackTracklets()", "MSD clusters have the same strip direction!!");


				if( isMSD2y )
					yMSD = (*itMSD2)->getRawHitCoords()(0);
				else
					xMSD = (*itMSD2)->getRawHitCoords()(0);
				
				Track* testTrack = new Track();
				testTrack->insertMeasurement( static_cast<genfit::PlanarMeasurement*>(*itMSD1)->clone() );
				testTrack->insertMeasurement( static_cast<genfit::PlanarMeasurement*>(*itMSD2)->clone() );
				testTrack->insertMeasurement( static_cast<genfit::PlanarMeasurement*>(*itTW)->clone() );

				int Z_Hypo = GetChargeFromTW(testTrack);
				double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(Z_Hypo) );
				int A_Hypo = round(mass_Hypo/m_AMU);
				testTrack->addTrackRep(new RKTrackRep(UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( Z_Hypo )));
				
				TVector3 pos(xMSD, yMSD, zMSD*0.98);
				TVector3 mom((globPosTW.x() - xMSD)/(globPosTW.z() - zMSD), (globPosTW.y() - yMSD)/(globPosTW.z() -zMSD) ,1);
				// if(m_debug > 1)
				// {
					cout << "BACKTRACKLET CANDIDATE::" << (*itTW)->getHitId() << "\t" << (*itMSD1)->getHitId() << "\t" << (*itMSD2)->getHitId() << endl;
					cout << Z_Hypo << "\t" << A_Hypo << endl;
					cout << "Pos::"; pos.Print();
					cout << "Mom::"; mom.Print();
				// }

				// if(mom.Theta() > angularCoverage)
				// if(mom.Phi() !compatible w/ x,y coordinates quadrant)


				for ( int MSDnPlane = MSDPlane2+1; MSDnPlane <= m_SensorIDMap->GetMaxFitPlane("MSD"); MSDnPlane++ )
				{
					TVector3 guessOnMSD = m_GeoTrafo->FromGlobalToMSDLocal( pos + mom*(m_SensorIDMap->GetFitPlane(MSDnPlane)->getO().Z() - pos.Z()));
					
					if( !m_SensorIDMap->GetFitPlane(MSDnPlane)->isInActive( guessOnMSD.x(), guessOnMSD.y() ) ) //RZ: should be ok since X,Y local coordinates of MSD are currently in the detector fram
						continue;

					int indexOfMinY = -1;
					int count = 0;
					double distanceInY = 1.;
					int sensorMatch = MSDnPlane;
					//RZ: TO BE CHECKED!! ADDED TO AVOID ERRORS
					Bool_t areLightFragments = false;
					if (areLightFragments) distanceInY = 2;
					// loop all absMeas in the found IT plane

					if ( m_allHitMeas->find( MSDnPlane ) == m_allHitMeas->end() ) {
						if(m_debug > 0) cout << "TAGFselector::CategorizeMSD() -- no measurement found in MSDnPlane "<< MSDnPlane<<"\n";
						continue;
					}

					for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( MSDnPlane ).begin(); it != m_allHitMeas->at( MSDnPlane ).end(); ++it){

						if ( m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != sensorMatch )	cout << "TAGFselector::Categorize_dataLike() --> ERROR MSD" <<endl, exit(0);

						//RZ: CHECK -> AVOID ERRORS
						double distanceFromHit;
						string strip;

						if ( ! static_cast<PlanarMeasurement*>(*it)->getStripV() )
							distanceFromHit = fabs(guessOnMSD.X() - (*it)->getRawHitCoords()(0));
						else
							distanceFromHit = fabs(guessOnMSD.Y() - (*it)->getRawHitCoords()(0));

						// find hit at minimum distance
						if ( distanceFromHit < distanceInY ){
							distanceInY = distanceFromHit;
							indexOfMinY = count;
						}
						count++;
					}

					// insert measurementi in GF Track
					if (indexOfMinY != -1){

						AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(sensorMatch).at(indexOfMinY)))->clone();
						testTrack->insertMeasurement( hitToAdd, testTrack->getNumPointsWithMeasurement()-1 );
					}

				} // end loop MSD planes

				cout << "Found testTrack with " << testTrack->getNumPointsWithMeasurement() << " points" << endl;

				if( testTrack->getNumPointsWithMeasurement() < 5 )
				{
					delete testTrack;
					continue;
				}

				try
				{
					mom.SetMag(TMath::Sqrt( pow(m_BeamEnergy*A_Hypo,2) + 2*mass_Hypo*m_BeamEnergy*A_Hypo )*0.95);
					testTrack->setStateSeed(pos, mom);

					m_fitter_extrapolation->processTrackWithRep(testTrack, testTrack->getCardinalRep() );

					StateOnPlane tempState = testTrack->getFittedState(-1);

					cout << "TW::"; tempState.getPos().Print();

					TVector2 TWcoords(xTW, yTW);

					cout << "dist::" << (tempState.getPos().XYvector() - TWcoords).Mod() << endl;

					if( (tempState.getPos().XYvector() - TWcoords).Mod() > 3 )
					{
						Info("""BackTracklets()", "Found wrong MSD-TW point association! Removing track...");
						delete testTrack;
						continue;
					}

					// tempState = testTrack->getFittedState(0);

					// testTrack->getCardinalRep()->extrapolateToPoint( tempState, TVector3(0,0,0));
					
					// //Check if 
					// cout << "TGT::"; tempState.getPos().Print();
					m_trackTempMap[TrackCounter] = testTrack;
					TrackCounter++;
				}
				catch(genfit::Exception& e)
				{
					std::cerr << e.what() << '\n';
					std::cerr << "Exception for backward tracklet fitting! Skipping..." << std::endl;
					continue;
				}
				
			}
		}

	}
	delete m_fitter_extrapolation;

	return;
}

void TAGFselector::CategorizeIT_back()
{
	cout << "IT back" << endl;
	return;
}

void TAGFselector::CategorizeVT_back()
{
	cout << "VT back" << endl;
	return;
}





//--------------------------------------------------------------------------------------------



//! \brief Fill the final track map to process in TAGactKFitter w/ the selected tracks
void TAGFselector::FillTrackCategoryMap()  {

	for(map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack)
	{
		TString outName;
		int MeasId = itTrack->second->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId();
		if( TAGrecoManager::GetPar()->PreselectStrategy() != "TrueParticle" && m_SensorIDMap->GetFitPlaneIDFromMeasID(MeasId) != m_SensorIDMap->GetFitPlaneTW())
		{
			if(m_debug > 0)
				Info("FillTrackCategoryMap()", "Found track candidate (%d) with no TW point! Skipping...", itTrack->first);
			continue;
		}

		int measCharge = itTrack->second->getCardinalRep()->getPDGCharge();
		int measMass;
		if( TAGrecoManager::GetPar()->PreselectStrategy() == "Linear" )
			measMass = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(measCharge) );
		else
			measMass = round( itTrack->second->getCardinalRep()->getMass( (itTrack->second)->getFittedState(-1) )/m_AMU );
		

		switch(measCharge)
		{
			case 1:	outName = "H";	break;
			case 2:	outName = "He";	break;
			case 3:	outName = "Li";	break;
			case 4:	outName = "Be";	break;
			case 5:	outName = "B";	break;
			case 6:	outName = "C";	break;
			case 7:	outName = "N";	break;
			case 8:	outName = "O";	break;
			default:
				outName = "fail";	break;
		}

	if ( m_debug > 1 ) 		Info("FillTrackCategoryMap()", "Track with measured charge %d and mass %d!!", measCharge, measMass);
		
		if( outName == "fail" )
		{
			Info("FillTrackCategoryMap()", "Skipped Track with measured charge %d and mass %d!!", measCharge, measMass);
			continue;
		}

		outName += Form("_%d_%d", measMass, itTrack->first);
		(*m_trackCategoryMap)[outName] = new Track(*(itTrack->second));

	}
}



//----------------------------------------------------------------------------------------------------

//! \brief Get the possible charge of a selected track from the TW measurement
//!
//! Currently uses MC information (returns charge of most frequent particle along the track) OR the charge of the TWpoint associated to the track
//! \param[in] trackToCheck Pointer to GenFit track
//! \return Charge measured from the TW
int TAGFselector::GetChargeFromTW(Track* trackToCheck){

	int charge = -1;

	// if( TAGrecoManager::GetPar()->PreselectStrategy() != "TrueParticle" ) //do not use MC!
	// {
	TATWpoint* twpoint = 0x0;
	for (unsigned int jTracking = trackToCheck->getNumPointsWithMeasurement() - 1; jTracking >= 0; --jTracking){

		if ( static_cast<genfit::PlanarMeasurement*>(trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId() != m_SensorIDMap->GetFitPlaneTW() ) continue;
		
		int MeasId = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getHitId();

		twpoint = ( (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint","TATWntuPoint")->Object() )->GetPoint( m_SensorIDMap->GetHitIDFromMeasID(MeasId) ); //Find TW point associated to the track

		charge = twpoint->GetChargeZ();
		break;
	}
	// }	//end of charge calculation from data

	// else	//use MC!
	// {	
	// 	int MeasId = trackToCheck->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId();
	// 	if(m_SensorIDMap->GetFitPlaneIDFromMeasID(MeasId) != m_SensorIDMap->GetFitPlaneTW())
	// 		return -1;
		
	// 	if(m_measParticleMC_collection->at(MeasId).size() == 1)
	// 	{
	// 		return m_McNtuEve->GetTrack( m_measParticleMC_collection->at(MeasId).at(0) )->GetCharge();
	// 	}
	// 	else
	// 	{
	// 		map<int,int> ChargeOccMap;

	// 		for(vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasId).end(); ++itTrackMC)
	// 		{
	// 			if( m_McNtuEve->GetTrack( *itTrackMC )->GetCharge() < 1 ||  m_McNtuEve->GetTrack( *itTrackMC )->GetCharge() > ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber)
	// 				continue;
	// 			ChargeOccMap[ m_McNtuEve->GetTrack( *itTrackMC )->GetCharge() ] = 1;
	// 		}

	// 		for(int i = 0; i < trackToCheck->getNumPointsWithMeasurement() - 1; ++i)
	// 		{
	// 			MeasId = trackToCheck->getPointWithMeasurement(i)->getRawMeasurement()->getHitId();
	// 			for(vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasId).end(); ++itTrackMC)
	// 			{
	// 				charge = m_McNtuEve->GetTrack( *itTrackMC )->GetCharge();
	// 				if(ChargeOccMap.find(charge) == ChargeOccMap.end()) 
	// 					continue;
				
	// 				ChargeOccMap[ charge ]++;
	// 			}
	// 		}
			
	// 		int occ = 0;

	// 		for(map<int,int>::iterator itMap = ChargeOccMap.begin(); itMap != ChargeOccMap.end(); ++itMap)
	// 		{
	// 			if(itMap->second > occ)
	// 			{
	// 				occ = itMap->second;
	// 				charge = itMap->first;
	// 			}
	// 		}
	// 	}
	// } //end MC charge calcualtion


	return charge;
}



//----------------------------------------------------------------------------------------------------

//! \brief Extrapolate a track to a GenFit FitPlane
//!
//! \param[in] trackToFit Pointer to track to extrapolate
//! \param[in] whichPlane Index of the FitPlane where to extrapolate the track
//! \param[in] repId Index of the track representation to use for the extrapolation
//! \return Extrapolated position vector in the FitPlane local reference frame
TVector3 TAGFselector::ExtrapolateToOuterTracker( Track* trackToFit, int whichPlane, int repId){

	//+++++++++++++++++++++++++++++++++++++++
	if(repId == -1)
		repId = trackToFit->getCardinalRepId();
	TrackPoint* tp = trackToFit->getPointWithMeasurementAndFitterInfo(-1, trackToFit->getTrackRep(repId));
	if (tp == nullptr) {
		Error("ExtrapolateToOuterTracker()", "Track has no TrackPoint with fitterInfo");
		exit(0);
	}

	if ( (static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(trackToFit->getTrackRep(repId)))->hasForwardUpdate() )  == false) {
		Error("ExtrapolateToOuterTracker()", "TrackPoint has no forward update");
		exit(0);
	}

	//RZ: Test with last fitted state!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	KalmanFittedStateOnPlane kfTest;

	kfTest = *(static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(trackToFit->getTrackRep( repId )))->getForwardUpdate());
	trackToFit->getTrackRep(repId)->extrapolateToPlane(kfTest, m_SensorIDMap->GetFitPlane(whichPlane), false, false); //RZ: Local reference frame of "whichPlane"!!!


	TVector3 posi;
	posi.SetXYZ((kfTest.getState()[3]),(kfTest.getState()[4]), m_SensorIDMap->GetFitPlane(whichPlane)->getO().Z());

	return posi;

}







//----------------------------------------------------------------------------------------------------

//! \brief pre-fit requirements to be applied to EACH of the hitCollections
//!
//! FUNCTION CURRENTLY NOT USED!
bool TAGFselector::PrefitRequirements( map< string, vector<AbsMeasurement*> >::iterator element ) {

	if ( m_debug > 0 )		cout << "SelctorKalmanGF::PrefitRequirements()  -  Category = " << (*element).first << " " << m_systemsON << "\n";


	int testHitNumberLimit = 0;
	int testHit_VT = 0;
	int testHit_IT = 0;
	int testHit_MSD = 0;
	int testHit_TW = 0;

	// define the number of hits per each detector to consider to satisfy the pre-fit requirements
	if ( m_systemsON == "all" ) {
		testHit_VT = m_SensorIDMap->GetFitPlanesN("VT");
		testHit_IT = m_SensorIDMap->GetFitPlanesN("IT")/16;
		testHit_MSD = m_SensorIDMap->GetFitPlanesN("MSD")/2;
	}

  	else {
	    if ( m_systemsON.find( "VT" ) != string::npos )			testHit_VT = m_SensorIDMap->GetFitPlanesN("VT");
	    if ( m_systemsON.find( "IT" ) != string::npos )			testHit_IT = m_SensorIDMap->GetFitPlanesN("IT")/16;
	    if ( m_systemsON.find( "MSD" ) != string::npos )		testHit_MSD = m_SensorIDMap->GetFitPlanesN("MSD");
	    if ( m_systemsON.find( "TW" ) != string::npos )			testHit_TW = 1;
  	}

	// num of total hits
	testHitNumberLimit = testHit_VT + testHit_IT + testHit_MSD + testHit_TW;
	if ( testHitNumberLimit == 0 ) 		
		cout << "ERROR --> TAGFselector::PrefitRequirements :: m_systemsON mode is wrong!!!" << endl, exit(0);

	// // test the total number of hits ->  speed up the test
	// if ( (int)((*element).second.size()) != testHitNumberLimit ) {
	// 	if ( m_debug > 0 )		cout << "WARNING :: TAGFselector::PrefitRequirements  -->  number of elements different wrt the expected ones : Nel=" << (int)((*element).second.size()) << "   Nexp= " << testHitNumberLimit << "\n";
	// 	return false;
	// }

	int nHitVT = 0;
	int nHitIT = 0;
	int nHitMSD = 0;
	int nHitTW = 0;

	// count the hits per each detector
		for ( vector<AbsMeasurement*>::iterator it=(*element).second.begin(); it != (*element).second.end(); it++ ) {
			int planeId = (*it)->getDetId();
			if ( m_SensorIDMap->IsFitPlaneInDet(planeId, "VT") )	nHitVT++;
			if ( m_SensorIDMap->IsFitPlaneInDet(planeId, "IT") )	nHitIT++;
			if ( m_SensorIDMap->IsFitPlaneInDet(planeId, "MSD") )	nHitMSD++;
			if ( planeId == m_SensorIDMap->GetFitPlaneTW() )	nHitTW++;
	}

	if ( m_debug > 0 )	cout << "nHitVT  " <<nHitVT<< " nHitIT " <<nHitIT<< " nHitMSD "<<nHitMSD<< " nHitTW "<<nHitTW<<"\n";

	// test the num of hits per each detector
	// if ( nHitVT != testHit_VT || nHitIT != testHit_IT || nHitMSD != testHit_MSD ) {

	if ( nHitVT != testHit_VT || nHitIT != testHit_IT || nHitMSD < 4 ){
	    if ( m_debug > 0 ) {
		    cout << "WARNING :: TAGFselector::PrefitRequirements  -->  number of elements different wrt the expected ones : " <<
				    "\n\n\t nVTX = " << nHitVT << "  Nexp = " << testHit_VT <<
				    "\n\n\t nITR = " << nHitIT << "  Nexp = " << testHit_IT <<
				    "\n\n\t nMSD = " << nHitMSD << "  Nexp = " << testHit_MSD <<
				    "\n\n\t nTW = " << nHitTW << "  Nexp = " << testHit_TW << "\n";
	    }
    	return false;
  	}

 	return true;
 }





//! \brief Get the MC true particle information from its MC track index
//!
//! \param[in] trackId Index of the MC track
//! \param[in,out] flukaID Pointer where to save the FLUKA ID
//! \param[in,out] charge Pointer where to save the true charge of the particle
//! \param[in,out] mass Pointer where to save the true mass of the particle
//! \param[in,out] posV Pointer where to save the initial position of the particle
//! \param[in,out] momV Pointer where to save the initial momentum of the particle
void TAGFselector::GetTrueParticleType(int trackid, int* flukaID, int* charge, double* mass, TVector3* posV, TVector3* momV ) {

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
TString TAGFselector::GetRecoTrackName(Track* tr)
{
	for(map<TString, Track*>::iterator it = m_trackCategoryMap->begin(); it != m_trackCategoryMap->end(); ++it)
	{
		if(it->second == tr)
			return it->first;
	}
	Error("GetRecoTrackName()", "Track not found in Category Map!!");
	throw -1;
}


