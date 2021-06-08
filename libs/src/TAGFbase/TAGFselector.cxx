
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

	flagMC = true; //HARDCODED -> TO BE CHANGED!!

	if (flagMC)
		m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();
}






//----------------------------------------------------------------------------------------------------
//
int TAGFselector::Categorize( ) {

	if ( FillTrackRepVector() != 0) 
		return -1;


	// fill m_mapTrack
	if ( TAGrecoManager::GetPar()->PreselectStrategy() == "TrueParticle" ){
		if ( !TAGrecoManager::GetPar()->IsMC() )		Error("TAGactKFitter::Action()", "Asked TrueParticle tracking but running not on MC." ), exit(0); 
		if(m_debug > 0) cout << "TAGFselector::Categorize_TruthMC START" << endl;
		
		Categorize_TruthMC();

		if(m_debug > 0) cout << "TAGFselector::Categorize_TruthMC END" << endl;
	}
	else if ( TAGrecoManager::GetPar()->PreselectStrategy() == "Sept2020" )
		Categorize_dataLike();
	else
		cout <<"ERROR :: TAGactKFitter::MakeFit  -->	 TAGrecoManager::GetPar()->PreselectStrategy() not defined" << endl, exit(0);

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
		// cout << "TAGFselector::FillTrackRepVector() -- charge: "<< m_chargeVect->at(i)<< endl;
		AbsTrackRep* rep = new RKTrackRep( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( m_chargeVect->at(i) ) );
		m_trackRepVec.push_back( rep );
	}
	return 0;
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
		cout << "MC particles:\n" << endl;
		for(auto it = m_measParticleMC_collection->begin(); it != m_measParticleMC_collection->end(); ++it)
		{
			for(int i=0; i < (it->second).size(); ++i)
			{
				cout << "GlobId::" << it->first << "\tId::" << i << "\tTrackId::" << it->second.at(i) << endl;
			}
		}
	}

	//Cycle on FitPlanes
	if(m_debug > 0) cout << "Cycle on planes\t"  << m_SensorIDMap->GetFitPlanesN() << endl;
	for(int iPlane = 0; iPlane < m_SensorIDMap->GetFitPlanesN(); ++iPlane)
	{

		if(m_debug > 0) cout << "Plane::" << iPlane << endl;
		
		//Skip plane if no hit was found
		if(m_allHitMeas->find(iPlane) == m_allHitMeas->end()){continue;}

		if(m_debug > 0) cout << "Cycle on hits" << endl;

		int foundHit = 0;
	
		//Cycle on all measurements/clusters of plane
		for(vector<AbsMeasurement*>::iterator itHit = m_allHitMeas->at(iPlane).begin(); itHit != m_allHitMeas->at(iPlane).end(); ++itHit)
		// for(int iHit = 0; iHit < m_allHitMeas->at(iPlane).size(); ++iHit)
		{
			// cout << "NHits::" << m_allHitMeas->at(iPlane).size() << endl;
			// int MeasGlobId = m_SensorIDMap->GetMeasID_eventLevel(iPlane, (*itHit)->getHitId());
			int MeasGlobId = (*itHit)->getHitId();
			// if ( m_SensorIDMap->GetDetNameFromMeasID( MeasGlobId ) == "TW" || m_SensorIDMap->GetDetNameFromMeasID( MeasGlobId ) == "MSD"  ) 
				// continue;

			//Cycle on all the MC particles that created that cluster/measurement
			if(m_debug > 0) cout << "Cycle on MC particles with GlobId::" << MeasGlobId << endl;


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
					continue;
				}

				//Set a unique ID for the particle that will be used in the map of Genfit tracks to fit
				outName += Form("_%d_%d", int(round(mass/m_AMU)), *itTrackMC);

				if ( m_debug > 0 )		cout << "\tSelected Category: " << outName << "  flukaID=" << flukaID << "  partID="<< *itTrackMC << "  charge="<<charge << "  mass="<<mass<< endl;

				if(m_trackCategoryMap->find(outName) == m_trackCategoryMap->end())
				{
					if(m_debug > 0) {
						cout << "Found new particle!! " << outName << endl;
						posV.Print();
						momV.Print();
					}
					(*m_trackCategoryMap)[outName] = new Track();
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

	if( m_debug > 1 ) cout << "******* START OF VT CYCLE *********" << endl;

	if(!TAGrecoManager::GetPar()->IncludeVT())
	{
		Error("Categorize_dataLike()", "Vertex is needed for data-like selection!");
		throw -1;
	}
	else
		CategorizeVT();

	if( m_debug > 1 ) cout << "******** END OF VT CYCLE **********" << endl;

	if( m_debug > 1 ) cout << "******* START OF IT CYCLE *********" << endl;
	
	if(TAGrecoManager::GetPar()->IncludeIT())
		CategorizeIT();
	
	if( m_debug > 1 ) cout << "******** END OF IT CYCLE **********" << endl;

	if( m_debug > 1 ) cout << "******* START OF MSD CYCLE *********" << endl;
	
	if(TAGrecoManager::GetPar()->IncludeMSD())
		CategorizeMSD();
	
	if( m_debug > 1 ) cout << "******** END OF MSD CYCLE **********" << endl;

	if( m_debug > 1 ) cout << "******* START OF TW CYCLE *********" << endl;
	
	if(TAGrecoManager::GetPar()->IncludeTW())
		CategorizeTW();
	
	if( m_debug > 1 ) cout << "******** END OF TW CYCLE **********" << endl;

	FillTrackCategoryMap();

	return 0;
}



void TAGFselector::CategorizeVT()
{
	TAVTntuVertex* vertexContainer = (TAVTntuVertex*) gTAGroot->FindDataDsc("vtVtx", "TAVTntuVertex")->Object();
	int vertexNumber = vertexContainer->GetVertexN();
	TAVTvertex* vtxPD   = 0x0; //NEW

	TVector3 pos_(0, 0, 0);		//global coord [cm]
    TVector3 mom_(0, 0, 2.);	//GeV //considering that fragments have same velocity of beam this should be changed accordingly

    if ( m_debug > 1 )		cout << "TAGFselector::CategorizeVT()  --  " << vertexNumber << endl;

	//loop over all vertices
	for (Int_t iVtx = 0; iVtx < vertexNumber; ++iVtx) {
		vtxPD = vertexContainer->GetVertex(iVtx);
		if (vtxPD == 0x0){
			cout << "Vertex number " << iVtx << " seems to be empty" << endl;
			continue;
		}

		if ( m_debug > 0 )		cout  << "vertex number " << iVtx << " has this nr of tracks " << vtxPD->GetTracksN() <<endl;

		//loop over tracks for each Vertex
		for (int iTrack = 0; iTrack < vtxPD->GetTracksN(); iTrack++) {


			TAVTtrack* tracklet = vtxPD->GetTrack( iTrack );

			// N clusters per tracklet
			int ncluster = tracklet->GetClustersN();

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

				if( m_debug > 1) cout << "VTX::PLANE::" << plane << endl;
				if( m_debug > 1) cout << "VTX::CLUS_ID::" << clusIdPerPlane << endl;

				// if ( m_allHitMeas->find( plane ) == m_allHitMeas->end() )									continue;
				// if ( m_allHitMeas->at(plane).find( clusIdPerPlane ) == m_allHitMeas->at(plane).end() )		continue;


				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (  m_allHitMeas->at(plane).at(clusIdPerPlane) ))->clone();
				fitTrack_->insertMeasurement( hitToAdd );
				// fitTrack_->insertPoint( new genfit::TrackPoint(hitToAdd, fitTrack_) );

				if ( m_debug > 1) {
					vector<int> iPart = m_measParticleMC_collection->at( hitToAdd->getHitId() );
					cout << "\t-- Truth particles of the measurement:\n";
					for (int k=0; k< iPart.size(); k++) {
						TAMCpart* particle = m_McNtuEve->GetTrack( iPart.at(k) );
						TVector3 mcMom = particle->GetInitP();
						cout << "\t\t-charge: " << particle->GetCharge() << "   mom:"; mcMom.Print();
					}
				}

			}	// end cluster loop

			//cout << " number of points is " << fitTrack_->getNumPointsWithMeasurement() << endl;
			if (fitTrack_->getNumPointsWithMeasurement() > 4 || fitTrack_->getNumPointsWithMeasurement() < 3){
				Warning("Categorize_dataLike()", "Track with %d measurements found in VTX => rejected!", fitTrack_->getNumPointsWithMeasurement());
				// delete fitTrack_;
				continue;
			}

			for ( int nRep=0; nRep < m_trackRepVec.size(); nRep++) {
				fitTrack_->addTrackRep( m_trackRepVec.at( nRep )->clone() );
				if ( m_debug > 1 ) {
					cout << "TAGFselector::CategorizeVT() -- rep charge = " << m_trackRepVec.at( nRep )->getPDGCharge() << endl;
				}
			}

			int IdTrack = iVtx*1000 + iTrack;

			m_trackTempMap[IdTrack] = fitTrack_;
			m_trackSlopeMap[IdTrack] = tracklet->GetSlopeZ();
		
		}	// end track loop

	} //end loop on vertices
}



void TAGFselector::CategorizeIT()	{

	// ExtrapFromVTXtoIT 
	TVector3 tmpExtrap, tmpVTX;
	TAGgeoTrafo* m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

	// same index if VTX_tracklets (for one vertex..)
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end();) {
		int addedMeas = 0;
		PlanarMeasurement* lastVTXMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(-1)->getRawMeasurement());
		tmpVTX.SetX( lastVTXMeas->getRawHitCoords()(0) );
		tmpVTX.SetY( lastVTXMeas->getRawHitCoords()(1) );
		tmpVTX.SetZ( m_SensorIDMap->GetFitPlane( lastVTXMeas->getPlaneId() )->getO().Z() );

		
		int maxITdetPlane = m_SensorIDMap->GetMaxFitPlane("IT");
		int minITdetPlane = m_SensorIDMap->GetMinFitPlane("IT");
		double tmpITz;


		// Info ("TAGFselector::CategorizeIT()", "Min IT plane %d and max IT plane %d", minITdetPlane, maxITdetPlane );
				
		vector<float>* allZinIT = m_SensorIDMap->GetPossibleITz();


		for ( int iz = 0; iz < allZinIT->size(); iz++ ) {

			
			// tmpITz = m_GeoTrafo->FromGlobalToVTLocal(m_SensorIDMap->GetFitPlane(ITnPlane)->getO()).Z();
			tmpITz = allZinIT->at(iz);
			// cout << "FOUND " << m_SensorIDMap->GetPlanesAtZ( tmpITz )->size() << " IT PLANES FOUND AT Z::" << tmpITz << endl;
			tmpExtrap = tmpVTX + m_trackSlopeMap[itTrack->first]*( tmpITz - tmpVTX.Z() );

			vector<int>* planesAtZ  = m_SensorIDMap->GetPlanesAtZ( tmpITz );
			// cout << "planes at z::" << tmpITz << " -> " << planesAtZ->size() << endl;

			// select a matching plane -> CHECK!!!!!!!!!!!!!!!
			// RZ: there is a potentially bad issue here with the bending plane!!! the intersection might be in another sensor since it is done with a linear extrapolation. Would i tbe better to only check the y? how much do we risk of f-ing this up?
			

			for ( vector<int>::iterator iPlane = planesAtZ->begin(); iPlane != planesAtZ->end(); iPlane++ ) {
				// cout << "Found plane::" << *iPlane << " at z::" << tmpITz << endl;

				double guessOnPlaneIT = tmpExtrap.Y();
				if ( !m_SensorIDMap->GetFitPlane( *iPlane )->isInActiveY( guessOnPlaneIT ) )	
					continue;

				// Info ("TAGFselector::CategorizeIT()", "Current IT plane %d. Candidate ID %i with vertex %i and tracklet %d", sensorMatch, (itTrack->first), (itTrack->first)/1000 , (itTrack->first)%1000  );

	// cout << "TAGFselector::CategorizeIT()     check"<< endl;
				int sensorMatch = (*iPlane);

				int indexOfMinY = -1.;
				double distanceInY = .1, distanceInX;
				//RZ: TO BE CHECKED!! ADDED TO AVOID ERRORS
				Bool_t areLightFragments = false;
				if (areLightFragments) distanceInY = .5;
				// loop all absMeas in the found IT plane

				// cout << "TAGFselector::CategorizeIT()     check1"<< endl;
				if ( m_allHitMeas->find( sensorMatch ) == m_allHitMeas->end() )   {
					// cout << "TAGFselector::CategorizeIT() -- WARNING extapolated plane empty!"<<endl;
					// cin.get();
					continue;
				}

				do {
					int count = 0;
					for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( sensorMatch ).begin(); it != m_allHitMeas->at( sensorMatch ).end(); ++it){
					
						// check -> RZ: why do we need this check???? is it for debug only??
						if ( m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != sensorMatch )	cout << "TAGFselector::Categorize_dataLike() --> ERROR IT" <<endl, exit(0);

						// find hit at minimum distance
						if ( fabs( guessOnPlaneIT - (*it)->getRawHitCoords()(1) ) < distanceInY ){
							distanceInY = fabs(guessOnPlaneIT - (*it)->getRawHitCoords()(1));
							distanceInX = fabs(tmpExtrap.X() - (*it)->getRawHitCoords()(0));
							indexOfMinY = count;
						}
						count++;
					}
					distanceInY += .05; 

				}while (indexOfMinY == -1 && distanceInY < .3);
	// cout << "TAGFselector::CategorizeIT()     check2"<< endl;
				// insert measurement in GF Track
				if (indexOfMinY != -1){
					cout << "Track::" << itTrack->first << "\tdistanceInY::" << distanceInY << "\tdistanceinX::" << distanceInX << endl;
					AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> ( m_allHitMeas->at(sensorMatch).at(indexOfMinY) ))->clone();
					(itTrack->second)->insertMeasurement( hitToAdd );
					// (itTrack->second)->insertPoint( new genfit::TrackPoint(hitToAdd, (itTrack->second)) );
					addedMeas++;
					
				}
	// cout << "TAGFselector::CategorizeIT()     check3"<< endl;
			}	// end loop on IT planes
		} // end loop over z
// cout << "TAGFselector::CategorizeIT()     check4"<< endl;
		// if(addedMeas == 0)
		// {
		// 	delete itTrack->second;
		// 	m_trackTempMap.erase(itTrack++);
		// }
		// else
			++itTrack;

		// cout << "TAGFselector::CategorizeIT()     check5"<< endl;

	}	// end loop on GF Track candidates
}










void TAGFselector::CategorizeMSD()	{

	// cout << "TAGFselector::CategorizeMSD()     MSDcheck"<< endl;

	// for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); itTrack++)
	// {
	// 	for(int i = 0; i< m_chargeVect->size(); ++i)
	// 	{
	// 		(itTrack->second)->addTrackRep( new RKTrackRep(UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( m_chargeVect->at(i) ) ) );
	// 	}
	// }
// cout << "TAGFselector::CategorizeMSD()     MSDcheck1"<< endl;
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

// cout << "TAGFselector::CategorizeMSD()     MSDcheck2"<< endl;
		//RZ: SET STATE SEED
		TVector3 pos = TVector3(0,0,0);
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		pos.SetX(firstTrackMeas->getRawHitCoords()(0));
		pos.SetY(firstTrackMeas->getRawHitCoords()(1));
		pos.SetZ( m_SensorIDMap->GetFitPlane( firstTrackMeas->getPlaneId() )->getO().Z() );
		cout << "***POS***" << endl;
		pos.Print();
		pos = pos + m_trackSlopeMap[itTrack->first]*( -pos.Z() );
		pos.Print();

		TVector3 mom = m_trackSlopeMap[itTrack->first];
		cout << "momSlope" << endl;
		mom.Print();

		m_fitter_extrapolation->setMaxIterations(1);
		float chi2 = 10000;
		int idCardRep = -1;
		cout << "\nSelectorKalmanGF::CategorizeMSD()  --  number of Reps = "<< itTrack->second->getNumReps() <<endl;

		for(int repId = 0; repId < itTrack->second->getNumReps(); ++repId)
		{
			Track* testTrack = new Track(* itTrack->second );
			int Z_Hypo = testTrack->getTrackRep(repId)->getPDGCharge();
			double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(Z_Hypo) );
			int A_Hypo = round(mass_Hypo/m_AMU);

			cout << "Z_Hypo::" << Z_Hypo << "\tA_Hypo::" << A_Hypo << endl;

			mom.SetMag(TMath::Sqrt( pow(0.2*A_Hypo,2) + 2*mass_Hypo*0.2*A_Hypo ));
			cout << "momScaled" << endl;
			mom.Print();

			testTrack->setStateSeed(pos, mom);
			try
			{
				m_fitter_extrapolation->processTrackWithRep( testTrack, testTrack->getTrackRep(repId) );
				cout << "Processed" << endl;
				TVector3 guessOnMSD = ExtrapolateToOuterTracker( testTrack, m_SensorIDMap->GetMinFitPlane("MSD"), repId);
				guessOnMSD.Print();
				cout << "\t\t charge = " << Z_Hypo << "  chi2 = " << m_fitter_extrapolation->getRedChiSqu(testTrack, testTrack->getTrackRep(repId) ) << endl;
				if(chi2 > m_fitter_extrapolation->getRedChiSqu(testTrack, testTrack->getTrackRep(repId) ))
				{
					chi2 = m_fitter_extrapolation->getRedChiSqu(testTrack, testTrack->getTrackRep(repId) );
					idCardRep = repId;
				}
			}
			catch (genfit::Exception& e)
			{
				std::cerr << e.what();
				std::cerr << "Exception, next rep" << std::endl;
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

		mom.SetMag( TMath::Sqrt( pow(0.2*A_Hypo,2) + 2*mass_Hypo*0.2*A_Hypo ));

		itTrack->second->setStateSeed(pos, mom);
		m_fitter_extrapolation->processTrackWithRep( itTrack->second, itTrack->second->getCardinalRep() );

		itTrack->second->getCardinalRep()->Print();
		cout << "Charge::" << itTrack->second->getCardinalRep()->getPDGCharge() << endl;

		//------------------------------------------------------------------------------------------

		
		for ( int MSDnPlane = minMSDdetPlane; MSDnPlane <= maxMSDdetPlane; MSDnPlane++ ) {
			TVector3 guessOnMSD = ExtrapolateToOuterTracker( itTrack->second, MSDnPlane );
// cout << "TAGFselector::CategorizeMSD()     MSDcheck3"<< endl;
			int indexOfMinY = -1;
			int count = 0;
			double distanceInY = 1;
			int sensorMatch = MSDnPlane;
			//RZ: TO BE CHECKED!! ADDED TO AVOID ERRORS
			Bool_t areLightFragments = false;
			if (areLightFragments) distanceInY = 2;
			// loop all absMeas in the found IT plane

			if ( m_allHitMeas->find( MSDnPlane ) == m_allHitMeas->end() ) {
				cout << "TAGFselector::CategorizeMSD() -- no MSD layer MSDnPlane "<< MSDnPlane<<endl;
				continue;
			}

			for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( MSDnPlane ).begin(); it != m_allHitMeas->at( MSDnPlane ).end(); ++it){
			cout << "TAGFselector::CategorizeMSD()     MSDcheck4"<< endl;
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
					cout << "Plane::" << sensorMatch << "\tTrack::" << itTrack->first << "\tdistanceFromHit::" << distanceFromHit << "\tStrip::" << strip << endl;
					distanceInY = distanceFromHit;
					indexOfMinY = count;
				}
				count++;
			}
cout << "TAGFselector::CategorizeMSD()     MSDcheck5"<< endl;
			// insert measurementi in GF Track
			if (indexOfMinY != -1){

				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(sensorMatch).at(indexOfMinY)))->clone();
				(itTrack->second)->insertMeasurement( hitToAdd );
				// (itTrack->second)->insertPoint( new genfit::TrackPoint(hitToAdd, (itTrack->second) ));
				findMSD++;
			}

		} // end loop MSD planes
cout << "TAGFselector::CategorizeMSD()     MSDcheck6"<< endl;
		// if (findMSD < 5)
		// {
		// 	delete itTrack->second;
		// 	m_trackTempMap.erase(itTrack++);
		// }
		// else
		++itTrack;

	}// end loop on GF Track candidates


}



void TAGFselector::CategorizeTW()
{
	// Extrapolate to TW
	KalmanFitter* m_fitter_extrapolation = new KalmanFitter(1);
	m_fitter_extrapolation->setMaxIterations(1);
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); itTrack++) 
	{
		m_fitter_extrapolation->processTrackWithRep(itTrack->second, itTrack->second->getCardinalRep() );

		TVector3 guessOnTW;
		int planeTW = m_SensorIDMap->GetFitPlaneTW();
		try
		{
			guessOnTW = ExtrapolateToOuterTracker( itTrack->second, planeTW);
		}
		catch(genfit::Exception& ex)
		{
			std::cerr << ex.what();
			std::cerr << "Exception, skip track candidate" << endl;
			continue;
		}

		if( m_debug > -1) cout << "guessOnTW " << guessOnTW.X() << "  " << guessOnTW.Y() << endl;

		//calculate distance TW point
		double TWdistance = 4.;
		int indexOfMin = -1;
		int count = 0;

		if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() ) {
			cout << "TAGFselector::CategorizeTW() -- no TW layer  "<<endl;
			continue;
		}

		for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( planeTW ).begin(); it != m_allHitMeas->at( planeTW ).end(); ++it){

			if (  m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != planeTW )
				cout << "TAGFselector::Categorize_dataLike() --> ERROR TW" <<endl, exit(0);

			double distanceFromHit = sqrt( ( guessOnTW.X() - (*it)->getRawHitCoords()(0) )*( guessOnTW.X() - (*it)->getRawHitCoords()(0) ) +
					( guessOnTW.Y() - (*it)->getRawHitCoords()(1) )*( guessOnTW.Y() - (*it)->getRawHitCoords()(1) ) );
			
			if( m_debug > 1) cout << "measurement: " << (*it)->getRawHitCoords()(0) << "   " << (*it)->getRawHitCoords()(1)<< endl;

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

		// int chargeFromTW = GetChargeFromTW( itTrack->second );

		//RZ: TO DO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// if(charge == -1 ) do cose

	}
	
}







void TAGFselector::FillTrackCategoryMap()  {

	for(map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack)
	{
		if( itTrack->second->getNumPointsWithMeasurement() < 9 )
		{
			// if( m_debug > 1 )
				Info("FillTrackCategoryMap()", "Skipped Track %d with %d TrackPoints with measurement!!", itTrack->first, itTrack->second->getNumPointsWithMeasurement());
			continue;
		}
		
		TString outName;


		// int nRep = itTrack->second->getNumReps();
		// int customCardRepID = -1;
		// float tmp_chi2 = 666;
		// // cout << "\nSelectorKalmanGF::FillTrackCategoryMap()  --  number of Reps = "<< nRep <<endl;
		// for (int r=0; r<nRep; r++) {
		// 	float currentRep_Chi2 = itTrack->second->getFitStatus( itTrack->second->getTrackRep(r) )->getChi2();
		// 	if ( tmp_chi2 > currentRep_Chi2 ){	
		// 		tmp_chi2 = currentRep_Chi2;
		// 		customCardRepID = r;
		// 	}
		// 	// cout << "\t\t charge = " << itTrack->second->getTrackRep(r)->getPDGCharge() 
		// 	// 		<< "  chi2 = " << currentRep_Chi2 << endl;
		// }
		// cout << "\t\t\t\t CARDINAL charge = " << itTrack->second->getCardinalRep()->getPDGCharge() 
		// 			<< "  chi2 = " << itTrack->second->getFitStatus( itTrack->second->getCardinalRep() )->getChi2() << endl;

		// if ( customCardRepID >= 0 && customCardRepID < nRep ) {
		// 	itTrack->second->setCardinalRep( customCardRepID );
		// }
		// else {
		// 	cout << "TAGFselector::FillTrackCategoryMap() :: ERROR  -- wrong rep number! = " << customCardRepID<<endl;
		// }	

		// if ( customCardRepID == -1 )
		// 	cout<< "TAGFselector::FillTrackCategoryMap() :: ERROR  --  Never found a Cardinal rep"<< endl;


		// int measCharge = int( round( (itTrack->second)->getFittedState(-1).getCharge() ) );
		// int measMass = int( round( (itTrack->second)->getFittedState(-1).getMass()/m_AMU ) );
		int measCharge = itTrack->second->getCardinalRep()->getPDGCharge();
		int measMass = round( itTrack->second->getCardinalRep()->getMass( (itTrack->second)->getFittedState(-1) )/m_AMU );

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
	TATWpoint* twpoint = 0x0;
	for (unsigned int jTracking = 0; jTracking < trackToCheck->getNumPointsWithMeasurement(); ++jTracking){

		if ( static_cast<genfit::PlanarMeasurement*>(trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId() != m_SensorIDMap->GetFitPlaneTW() ) continue;
		
		int MeasId = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getHitId();

		twpoint = ( (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint","TATWntuPoint")->Object() )->GetPoint( m_SensorIDMap->GetHitIDFromMeasID(MeasId) );

		charge = twpoint->GetChargeZ();
	}

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
	trackToFit->getTrackRep(repId)->extrapolateToPlane(kfTest, m_SensorIDMap->GetFitPlane(whichPlane), false, false);
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

	if ( m_debug > 0 )		cout << "SelctorKalmanGF::PrefitRequirements()  -  Category = " << (*element).first << " " << m_systemsON << endl;


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
	// 	if ( m_debug > 0 )		cout << "WARNING :: TAGFselector::PrefitRequirements  -->  number of elements different wrt the expected ones : Nel=" << (int)((*element).second.size()) << "   Nexp= " << testHitNumberLimit << endl;
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

	if ( m_debug > 0 )	cout << "nHitVT  " <<nHitVT<< " nHitIT " <<nHitIT<< " nHitMSD "<<nHitMSD<< " nHitTW "<<nHitTW<<endl;

	// test the num of hits per each detector
	// if ( nHitVT != testHit_VT || nHitIT != testHit_IT || nHitMSD != testHit_MSD ) {

	if ( nHitVT != testHit_VT || nHitIT != testHit_IT || nHitMSD < 4 ){
	    if ( m_debug > 0 ) {
		    cout << "WARNING :: TAGFselector::PrefitRequirements  -->  number of elements different wrt the expected ones : " <<
				    "\n\t nVTX = " << nHitVT << "  Nexp = " << testHit_VT << endl <<
				    "\n\t nITR = " << nHitIT << "  Nexp = " << testHit_IT << endl <<
				    "\n\t nMSD = " << nHitMSD << "  Nexp = " << testHit_MSD << endl <<
				    "\n\t nTW = " << nHitTW << "  Nexp = " << testHit_TW << endl;
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
		{
			return it->first;
		}
	}
	Error("GetRecoTrackName()", "Track not found in Category Map!!");
	throw -1;
}



