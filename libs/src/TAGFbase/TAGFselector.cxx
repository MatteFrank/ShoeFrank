
#include "TAGFselector.hxx"



//----------------------------------------------------------------------------------------------------
//
TAGFselector::TAGFselector( map< int, vector<AbsMeasurement*> >* allHitMeas, vector<int>* chargeVect, 
								TAGFdetectorMap* SensorIDmap , map<TString, Track*>* trackCategoryMap, 
								map< int, vector<int> >* measParticleMC_collection) {

	m_allHitMeas = allHitMeas;
	m_chargeVect = chargeVect;
	m_SensorIDMap = SensorIDmap;
	m_trackCategoryMap = trackCategoryMap;
	m_measParticleMC_collection = measParticleMC_collection;

	m_debug = TAGrecoManager::GetPar()->Debug();

	m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());


	m_BeamEnergy = ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().Energy;

	if( m_debug > 1 )	cout << "Beam Energy::" << m_BeamEnergy << endl;


	if ( TAGrecoManager::GetPar()->IsMC() )
		m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();
}






//----------------------------------------------------------------------------------------------------
//
int TAGFselector::Categorize( ) {

	if ( FillTrackRepVector() != 0) 
		return -1;


	// fill m_mapTrack
	if ( TAGrecoManager::GetPar()->PreselectStrategy() == "TrueParticle" ){
	  //		if ( !TAGrecoManager::GetPar()->IsMC() )		Error("TAGactKFitter::Action()", "Asked TrueParticle tracking but running not on MC." ), exit(0); 
		if(m_debug > 0) cout << "TAGFselector::Categorize_TruthMC START\n";
		
		Categorize_TruthMC();

		if(m_debug > 0) cout << "TAGFselector::Categorize_TruthMC END\n";
	}
	else if ( TAGrecoManager::GetPar()->PreselectStrategy() == "Sept2020" )
		Categorize_dataLike();
	else if ( TAGrecoManager::GetPar()->PreselectStrategy() == "Linear" )
		Categorize_Linear();
	else
		cout <<"ERROR :: TAGFselector::MakeFit  -->	 TAGrecoManager::GetPar()->PreselectStrategy() not defined" << endl, exit(0);

	return 0;
}





//----------------------------------------------------------------------------------------------------
//Check the charges seen by TW and fill the track representation vector 
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


		if ( particle->GetCharge() < 1 || particle->GetCharge() > 8)
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
						match = 1;
				}


				if (match == 0)		continue;

				foundHit++;
			}
		}

		//		if ( foundHit < TAGrecoManager::GetPar()->MeasureN() ) 
		//			continue;

		genCount_vector.at( outName ) += 1;

	}

	return genCount_vector;

}





//----------------------------------------------------------------------------------------------------
//
int TAGFselector::Categorize_TruthMC( )
{
	//Categorize events using MC truth
	int flukaID, charge;
	double mass;

	// m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();

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

				if ( m_debug > 0 )		cout << "\tSelected Category: " << outName << "  flukaID=" << flukaID << "  partID="<< *itTrackMC << "  charge="<<charge << "  mass="<<mass<< "\n";

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

				// m_trackCategoryMap->at(outName)->insertPoint( new genfit::TrackPoint(hitToAdd, m_trackCategoryMap->at(outName)) );
				m_trackCategoryMap->at(outName)->insertMeasurement( hitToAdd );

				foundHit++;
			
			} //End of loop on MC particles per cluster/measurement
			// if ( foundHit =! 0 ) break;
		} //End of loop on measurements per sensor
	
	} //End of loop on sensors

	return 0;
}



//----------------------------------------------------------------------------------------------------
// Categorize tracks and representations -> data-like approach avoiding MC truth info
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


/*Function used to handle linear tracks when no magnetic field is used -> GSI2021 
*
*/
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
			Z_Hypo = 8;
			itTrack->second->addTrackRep(new RKTrackRep(UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( Z_Hypo )));
		}
		else
		{
			for ( int nRep=0; nRep < m_trackRepVec.size(); nRep++)
				if( m_trackRepVec.at(nRep)->getPDGCharge() == Z_Hypo )	(itTrack->second)->addTrackRep( m_trackRepVec.at( nRep )->clone() );
		}

		// TVector3 pos = TVector3(0,0,0);
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		TVector3 pos = m_GeoTrafo->FromVTLocalToGlobal( TVector3(
				firstTrackMeas->getRawHitCoords()(0), //X
				firstTrackMeas->getRawHitCoords()(1), //Y
				m_SensorIDMap->GetFitPlane( firstTrackMeas->getPlaneId() )->getO().Z())); //Z
		// pos.SetX( m_GeoTrafo->FromVTLocalToGlobal( firstTrackMeas->getRawHitCoords()).X() );
		// pos.SetY( m_GeoTrafo->FromVTLocalToGlobal( firstTrackMeas->getRawHitCoords()).Y() );
		// pos.SetZ( m_SensorIDMap->GetFitPlane( firstTrackMeas->getPlaneId() )->getO().Z() );
		pos = pos - m_trackSlopeMap[itTrack->first]*pos.Z();

		TVector3 mom = m_trackSlopeMap[itTrack->first];
		double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(Z_Hypo) );
		int A_Hypo = round(mass_Hypo/m_AMU);

		mom.SetMag(TMath::Sqrt( pow(m_BeamEnergy*A_Hypo,2) + 2*mass_Hypo*m_BeamEnergy*A_Hypo ));
		itTrack->second->setStateSeed(pos, mom);

		// preFitter->processTrackWithRep( itTrack->second, itTrack->second->getCardinalRep() );
	}

	FillTrackCategoryMap();

	return 0;
}



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

			// loop over clusters in the tracklet get clusters in tracklet
			for (int iCluster = ncluster - 1; iCluster >= 0; iCluster--) {

				TAVTcluster* clus = (TAVTcluster*) tracklet->GetCluster( iCluster );
				if (!clus->IsValid()) continue;

				//RZ: CHECK THESE NUMBERS!!!!!!!!
				int plane = clus->GetSensorIdx();
				int clusIdPerPlane = clus->GetClusterIdx();

				int index=0;
				while( clusIdPerPlane != vtntuclus->GetCluster(plane, index)->GetClusterIdx() )
					index++;


				// if ( m_allHitMeas->find( plane ) == m_allHitMeas->end() )									continue;
				// if ( m_allHitMeas->at(plane).find( clusIdPerPlane ) == m_allHitMeas->at(plane).end() )		continue;


				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (  m_allHitMeas->at(plane).at(index) ))->clone();
				fitTrack_->insertMeasurement( hitToAdd );
				// fitTrack_->insertPoint( new genfit::TrackPoint(hitToAdd, fitTrack_) );

				if( m_debug > 1) 
				{
					cout << "VTX::PLANE::" << plane << "\n";
					cout << "VTX::CLUS_IDx::" << clusIdPerPlane << "\n";
					cout << "VTX::CLUS_ID::" << index << "\n";
					cout << "VTX::CLUS_POS::"; 
					hitToAdd->getRawHitCoords().Print();
				}


				if ( m_debug > 1 && TAGrecoManager::GetPar()->IsMC() ) {
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

			m_trackTempMap[IdTrack] = fitTrack_;
			m_trackSlopeMap[IdTrack] = tracklet->GetSlopeZ();
		
		}	// end track loop

	} //end loop on vertices

	if(m_trackTempMap.size() == 0)
		Warning("CategorizeVT()","No VT tracklet found in the event!");
}



void TAGFselector::CategorizeIT()	{

	// ExtrapFromVTXtoIT 
	TVector3 tmpExtrap, tmpVTX;
	TAGgeoTrafo* m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

	// same index if VTX_tracklets (for one vertex..)
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end();) {
		int addedMeas = 0;
		PlanarMeasurement* lastVTXMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(-1)->getRawMeasurement());
		tmpVTX = m_GeoTrafo->FromVTLocalToGlobal( TVector3(
				lastVTXMeas->getRawHitCoords()(0), //X
				lastVTXMeas->getRawHitCoords()(1), //Y
				m_SensorIDMap->GetFitPlane( lastVTXMeas->getPlaneId() )->getO().Z())); //Z
		// tmpVTX.SetX( m_GeoTrafo->FromVTLocalToGlobal(lastVTXMeas->getRawHitCoords()).X() );
		// tmpVTX.SetY( m_GeoTrafo->FromVTLocalToGlobal(lastVTXMeas->getRawHitCoords()).Y() );
		// tmpVTX.SetZ( m_SensorIDMap->GetFitPlane( lastVTXMeas->getPlaneId() )->getO().Z() );

		
		int maxITdetPlane = m_SensorIDMap->GetMaxFitPlane("IT");
		int minITdetPlane = m_SensorIDMap->GetMinFitPlane("IT");
		double tmpITz;


		// Info ("TAGFselector::CategorizeIT()", "Min IT plane %d and max IT plane %d", minITdetPlane, maxITdetPlane );
				
		vector<float>* allZinIT = m_SensorIDMap->GetPossibleITz();


		for ( int iz = 0; iz < allZinIT->size(); iz++ ) {

			
			// tmpITz = m_GeoTrafo->FromGlobalToVTLocal(m_SensorIDMap->GetFitPlane(ITnPlane)->getO()).Z();
			tmpITz = allZinIT->at(iz);
			// cout << "FOUND " << m_SensorIDMap->GetPlanesAtZ( tmpITz )->size() << " IT PLANES FOUND AT Z::" << tmpITz << "\n";
			tmpExtrap = tmpVTX + m_trackSlopeMap[itTrack->first]*( tmpITz - tmpVTX.Z() );

			vector<int>* planesAtZ  = m_SensorIDMap->GetPlanesAtZ( tmpITz );
			// cout << "planes at z::" << tmpITz << " -> " << planesAtZ->size() << "\n";

			// select a matching plane -> CHECK!!!!!!!!!!!!!!!
			// RZ: there is a potentially bad issue here with the bending plane!!! the intersection might be in another sensor since it is done with a linear extrapolation. Would i tbe better to only check the y? how much do we risk of f-ing this up?
			

			for ( vector<int>::iterator iPlane = planesAtZ->begin(); iPlane != planesAtZ->end(); iPlane++ ) {
				// cout << "Found plane::" << *iPlane << " at z::" << tmpITz << "\n";

				TVector3 guessOnPlaneIT = m_GeoTrafo->FromGlobalToITLocal( tmpExtrap ); //RZ: IsInActive controls local or global variables????
				if ( !m_SensorIDMap->GetFitPlane( *iPlane )->isInActiveY( tmpExtrap.Y() ) )	
					continue;

	// cout << "TAGFselector::CategorizeIT()     check\n";
				int sensorMatch = (*iPlane);

				int indexOfMinY = -1.;
				double distanceInY = .1, distanceInX;
				//RZ: TO BE CHECKED!! ADDED TO AVOID ERRORS
				Bool_t areLightFragments = false;
				if (areLightFragments) distanceInY = .5;
				// loop all absMeas in the found IT plane

				// cout << "TAGFselector::CategorizeIT()     check1\n";
				if ( m_allHitMeas->find( sensorMatch ) == m_allHitMeas->end() )   {
					// cout << "TAGFselector::CategorizeIT() -- WARNING extapolated plane empty!\n";
					// cin.get();
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
	// cout << "TAGFselector::CategorizeIT()     check2\n";
				// insert measurement in GF Track
				
				
				if (indexOfMinY != -1 && distanceInX < 1.){
					if(m_debug > 0) cout << "ITcheck\tTrack::" << itTrack->first << "\tdistanceInY::" << distanceInY << "\tdistanceinX::" << distanceInX << "\n";
					AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> ( m_allHitMeas->at(sensorMatch).at(indexOfMinY) ))->clone();
					(itTrack->second)->insertMeasurement( hitToAdd );
					// (itTrack->second)->insertPoint( new genfit::TrackPoint(hitToAdd, (itTrack->second)) );
					addedMeas++;
					
				}
	// cout << "TAGFselector::CategorizeIT()     check3\n";
			}	// end loop on IT planes
		} // end loop over z
// cout << "TAGFselector::CategorizeIT()     check4"<<\n";

		++itTrack;

		// cout << "TAGFselector::CategorizeIT()     check5\n";

	}	// end loop on GF Track candidates
}










void TAGFselector::CategorizeMSD()	{

	// cout << "TAGFselector::CategorizeMSD()     MSDcheck\n";

	// for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); itTrack++)
	// {
	// 	for(int i = 0; i< m_chargeVect->size(); ++i)
	// 	{
	// 		(itTrack->second)->addTrackRep( new RKTrackRep(UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( m_chargeVect->at(i) ) ) );
	// 	}
	// }
// cout << "TAGFselector::CategorizeMSD()     MSDcheck1\n";
	// ClearTrackMap();

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
		TVector3 pos = m_GeoTrafo->FromVTLocalToGlobal( TVector3(
						firstTrackMeas->getRawHitCoords()(0), //X
						firstTrackMeas->getRawHitCoords()(1), //Y
						m_SensorIDMap->GetFitPlane( firstTrackMeas->getPlaneId() )->getO().Z())); //Z
		// pos.SetX( m_GeoTrafo->FromVTLocalToGlobal(firstTrackMeas->getRawHitCoords()).X() );
		// pos.SetY( m_GeoTrafo->FromVTLocalToGlobal(firstTrackMeas->getRawHitCoords()).Y() );
		// pos.SetZ( m_SensorIDMap->GetFitPlane( firstTrackMeas->getPlaneId() )->getO().Z() );
		
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
				
				if(m_debug > 0)	cout << "Processed\n";
				
				TVector3 guessOnMSD = ExtrapolateToOuterTracker( testTrack, m_SensorIDMap->GetMinFitPlane("MSD"), repId); //RZ: Local or global?!?!?!?!? CHECK!!!!!!!!!!!!!!!
				// TVector3 guessOnMSD = m_GeoTrafo->FromMSDLocalToGlobal( ExtrapolateToOuterTracker( testTrack, m_SensorIDMap->GetMinFitPlane("MSD"), repId) );
				
				if(m_debug > 0)
				{
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
		// m_fitter_extrapolation->processTrack(itTrack->second);


		//------------------------------ track rep check  ------------------------------
		// int nRep = itTrack->second->getNumReps();
		// float chi2=10000;
		// for (int r=0; r<nRep; r++) {

		// }
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
			TVector3 guessOnMSD = ExtrapolateToOuterTracker( itTrack->second, MSDnPlane ); //RZ: Local or Global?!?!?!? CHECK!!!!!!!!
			// TVector3 guessOnMSD = m_GeoTrafo->FromMSDLocalToGlobal( ExtrapolateToOuterTracker( itTrack->second, MSDnPlane) );
			if( !m_SensorIDMap->GetFitPlane(MSDnPlane)->isInActive( guessOnMSD.x(), guessOnMSD.y() ) )
				continue;
// cout << "TAGFselector::CategorizeMSD()     MSDcheck3\n";
			int indexOfMinY = -1;
			int count = 0;
			double distanceInY = 1;
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
			// cout << "TAGFselector::CategorizeMSD()     MSDcheck5\n";
			// insert measurementi in GF Track
			if (indexOfMinY != -1){

				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(sensorMatch).at(indexOfMinY)))->clone();
				(itTrack->second)->insertMeasurement( hitToAdd );
				// (itTrack->second)->insertPoint( new genfit::TrackPoint(hitToAdd, (itTrack->second) ));
				findMSD++;
			}

		} // end loop MSD planes
		// cout << "TAGFselector::CategorizeMSD()     MSDcheck6\n";
		// if (findMSD < 5)
		// {
		// 	delete itTrack->second;
		// 	m_trackTempMap.erase(itTrack++);
		// }
		// else
		++itTrack;

	}// end loop on GF Track candidates


}


/*MSD selection algorithm when no magnetic field is present
*
*/
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
		// TVector3 pos = TVector3(0,0,0);
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		TVector3 pos = m_GeoTrafo->FromVTLocalToGlobal( TVector3(
						firstTrackMeas->getRawHitCoords()(0), //X
						firstTrackMeas->getRawHitCoords()(1), //Y
						m_SensorIDMap->GetFitPlane( firstTrackMeas->getPlaneId() )->getO().Z())); //Z
		// pos.SetX( m_GeoTrafo->FromMSDLocalToGlobal(firstTrackMeas->getRawHitCoords()).X() );
		// pos.SetY( m_GeoTrafo->FromMSDLocalToGlobal(firstTrackMeas->getRawHitCoords()).Y() );
		// pos.SetZ( m_SensorIDMap->GetFitPlane( firstTrackMeas->getPlaneId() )->getO().Z() );
		
		if(m_debug > 0)
		{
			cout << "***POS SEED***\nVTX: "; pos.Print();
		}

		for ( int MSDnPlane = minMSDdetPlane; MSDnPlane <= maxMSDdetPlane; MSDnPlane++ ) {
			TVector3 guessOnMSD = m_GeoTrafo->FromGlobalToMSDLocal( pos + m_trackSlopeMap[itTrack->first]*(m_SensorIDMap->GetFitPlane(MSDnPlane)->getO().Z() - pos.Z()));
			
			if( !m_SensorIDMap->GetFitPlane(MSDnPlane)->isInActive( guessOnMSD.x(), guessOnMSD.y() ) )
				continue;

			int indexOfMinY = -1;
			int count = 0;
			double distanceInY = 1;
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




void TAGFselector::CategorizeTW()
{
	int planeTW = m_SensorIDMap->GetFitPlaneTW();
	if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() ) {
		if(m_debug > -1) cout << "TAGFselector::CategorizeTW() -- no measurement found in TW layer\n";
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
			guessOnTW = ExtrapolateToOuterTracker( itTrack->second, planeTW); //RZ: Local or global?!?!?!?!? CHECK!!!!!!!!!!!!!!!!!!
			// guessOnTW = m_GeoTrafo->FromTWLocalToGlobal(ExtrapolateToOuterTracker( itTrack->second, planeTW));
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
			// (itTrack->second)->insertPoint( new genfit::TrackPoint(hitToAdd, (itTrack->second)) );
		}
	}
	
}



/*TW selection algorithm when no magnetic field is present
*
*/
void TAGFselector::CategorizeTW_Linear()
{
	// Extrapolate to TW
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); itTrack++) 
	{
		// TVector3 pos = TVector3(0,0,0);
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		TVector3 pos = m_GeoTrafo->FromVTLocalToGlobal( TVector3(
						firstTrackMeas->getRawHitCoords()(0), //X
						firstTrackMeas->getRawHitCoords()(1), //Y
						m_SensorIDMap->GetFitPlane( firstTrackMeas->getPlaneId() )->getO().Z())); //Z
		// pos.SetX( m_GeoTrafo->FromVTLocalToGlobal(firstTrackMeas->getRawHitCoords()).X() );
		// pos.SetY( m_GeoTrafo->FromVTLocalToGlobal(firstTrackMeas->getRawHitCoords()).Y() );
		// pos.SetZ( m_SensorIDMap->GetFitPlane( firstTrackMeas->getPlaneId() )->getO().Z() );

		int planeTW = m_SensorIDMap->GetFitPlaneTW();
		TVector3 guessOnTW = m_GeoTrafo->FromGlobalToTWLocal(pos + m_trackSlopeMap[itTrack->first]*( m_SensorIDMap->GetFitPlane(planeTW)->getO().Z() - pos.Z() ));

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






void TAGFselector::FillTrackCategoryMap()  {

	for(map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack)
	{
		TString outName;
		int MeasId = itTrack->second->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId();
		if( m_SensorIDMap->GetFitPlaneIDFromMeasID(MeasId) != m_SensorIDMap->GetFitPlaneTW())
			continue;


		// int nRep = itTrack->second->getNumReps();
		// int customCardRepID = -1;
		// float tmp_chi2 = 666;
		// // cout << "\nSelectorKalmanGF::FillTrackCategoryMap()  --  number of Reps = "<< nRep <<"\n";
		// for (int r=0; r<nRep; r++) {
		// 	float currentRep_Chi2 = itTrack->second->getFitStatus( itTrack->second->getTrackRep(r) )->getChi2();
		// 	if ( tmp_chi2 > currentRep_Chi2 ){	
		// 		tmp_chi2 = currentRep_Chi2;
		// 		customCardRepID = r;
		// 	}
		// 	// cout << "\t\t charge = " << itTrack->second->getTrackRep(r)->getPDGCharge() 
		// 	// 		<< "  chi2 = " << currentRep_Chi2 << "\n";
		// }
		// cout << "\t\t\t\t CARDINAL charge = " << itTrack->second->getCardinalRep()->getPDGCharge() 
		// 			<< "  chi2 = " << itTrack->second->getFitStatus( itTrack->second->getCardinalRep() )->getChi2() << "\n";

		// if ( customCardRepID >= 0 && customCardRepID < nRep ) {
		// 	itTrack->second->setCardinalRep( customCardRepID );
		// }
		// else {
		// 	cout << "TAGFselector::FillTrackCategoryMap() :: ERROR  -- wrong rep number! = " << customCardRepID<<"\n";
		// }	

		// if ( customCardRepID == -1 )
		// 	cout<< "TAGFselector::FillTrackCategoryMap() :: ERROR  --  Never found a Cardinal rep"<< "\n";


		// int measCharge = int( round( (itTrack->second)->getFittedState(-1).getCharge() ) );
		// int measMass = int( round( (itTrack->second)->getFittedState(-1).getMass()/m_AMU ) );
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
		(*m_trackCategoryMap)[outName] = itTrack->second;

	}
}



//----------------------------------------------------------------------------------------------------
//RZ: Think of a way of passing the raw TW measurements to this function. The vector of possible charge values is already available at this point -> THIS FUNCTION DOES NOTHING RIGHT NOW!!
int TAGFselector::GetChargeFromTW(Track* trackToCheck){

	int charge = -1;

	if( !TAGrecoManager::GetPar()->IsMC() ) //do not use MC!
	{
		TATWpoint* twpoint = 0x0;
		for (unsigned int jTracking = 0; jTracking < trackToCheck->getNumPointsWithMeasurement(); ++jTracking){

			if ( static_cast<genfit::PlanarMeasurement*>(trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId() != m_SensorIDMap->GetFitPlaneTW() ) continue;
			
			int MeasId = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getHitId();

			twpoint = ( (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint","TATWntuPoint")->Object() )->GetPoint( m_SensorIDMap->GetHitIDFromMeasID(MeasId) );

			charge = twpoint->GetChargeZ();
		}
	}	//end of charge calculation from data

	else	//use MC!
	{	
		int MeasId = trackToCheck->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId();
		if(m_SensorIDMap->GetFitPlaneIDFromMeasID(MeasId) != m_SensorIDMap->GetFitPlaneTW())
			return -1;
		
		if(m_measParticleMC_collection->at(MeasId).size() == 1)
		{
			return m_McNtuEve->GetTrack( m_measParticleMC_collection->at(MeasId).at(0) )->GetCharge();
		}
		else
		{
			map<int,int> ChargeOccMap;

			for(vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasId).end(); ++itTrackMC)
			{
				if( m_McNtuEve->GetTrack( *itTrackMC )->GetCharge() < 1 ||  m_McNtuEve->GetTrack( *itTrackMC )->GetCharge() > 8)
					continue;
				ChargeOccMap[ m_McNtuEve->GetTrack( *itTrackMC )->GetCharge() ] = 1;
			}

			for(int i = 0; i < trackToCheck->getNumPointsWithMeasurement() - 1; ++i)
			{
				MeasId = trackToCheck->getPointWithMeasurement(i)->getRawMeasurement()->getHitId();
				for(vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasId).end(); ++itTrackMC)
				{
					charge = m_McNtuEve->GetTrack( *itTrackMC )->GetCharge();
					if(ChargeOccMap.find(charge) == ChargeOccMap.end()) 
						continue;
				
					ChargeOccMap[ charge ]++;
				}
			}
			
			int occ = 0;

			for(map<int,int>::iterator itMap = ChargeOccMap.begin(); itMap != ChargeOccMap.end(); ++itMap)
			{
				if(itMap->second > occ)
				{
					occ = itMap->second;
					charge = itMap->first;
				}
			}
		}
	} //end MC charge calcualtion


	return charge;
}



//----------------------------------------------------------------------------------------------------
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

	// if ( (static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(trackToFit->getCardinalRep()))->hasForwardPrediction() )  == false) {
	//   TVector3 x(0.,0.,0.);
	//   return x;
	// }

	//RZ: Test with last fitted state!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	KalmanFittedStateOnPlane kfTest;

	// if( repId==-1 )
	// {
	// 	kfTest = *(static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(trackToFit->getCardinalRep( )))->getForwardUpdate());
	// 	trackToFit->getCardinalRep()->extrapolateToPlane(kfTest, m_SensorIDMap->GetFitPlane(whichPlane), false, false);
	// }
	// else
	// {
	kfTest = *(static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(trackToFit->getTrackRep( repId )))->getForwardUpdate());
	trackToFit->getTrackRep(repId)->extrapolateToPlane(kfTest, m_SensorIDMap->GetFitPlane(whichPlane), false, false); //RZ: Local or global?!?!??!?! CHECK!!!!!!!
	// }
	// std::cout << "state after extrapolating back to reference plane \n";
	//kfTest.Print();

	TVector3 posi;
	posi.SetXYZ((kfTest.getState()[3]),(kfTest.getState()[4]), m_SensorIDMap->GetFitPlane(whichPlane)->getO().Z());

	return posi;

}







//----------------------------------------------------------------------------------------------------
// pre-fit requirements to be applied to EACH of the hitCollections
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










void TAGFselector::GetTrueParticleType(int trackid, int* flukaID, int* charge, double* mass, TVector3* posV, TVector3* momV ) {

	TAMCpart* particle = m_McNtuEve->GetTrack(trackid);

	*flukaID = particle->GetFlukaID();
	*charge  = particle->GetCharge();
	*mass    = particle->GetMass();

	*posV = particle->GetInitPos();
	*momV = particle->GetInitP();

}


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



