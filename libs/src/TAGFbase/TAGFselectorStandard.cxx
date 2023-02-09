/*!
 \file TAGFselectorStandard.cxx
 \brief  Class for standard track finding/selection in GenFit Global Reconstruction
 \author R. Zarrella and M. Franchini
*/

#include "TAGFselectorStandard.hxx"

/*!
 \class TAGFselectorStandard
 \brief Class for standard data-like track finding/selection in GenFit Global Reconstruction
*/


//----------------------------------------------------------------------------------------------------

//! \brief Default constructor
TAGFselectorStandard::TAGFselectorStandard() : TAGFselectorBase()
{}


//----------------------------------------------------------------------------------------------------

//! \brief Base function for standard track finding/selection/categorization
void TAGFselectorStandard::Categorize( ) {

	if(!TAGrecoManager::GetPar()->IncludeVT() || !m_systemsON.Contains("VT"))
	{
		Error("Categorize_dataLike()", "Standard selection algorithm currently not supported without Vertex!");
		exit(0);
	}
	else
	{
		if( m_debug > 1 ) cout << "******* START OF VT CYCLE *********\n";
		CategorizeVT();
		if( m_debug > 1 ) cout << "******** END OF VT CYCLE **********\n";
	}

	if( m_systemsON.Contains("IT") )
	{
		if( m_debug > 1 ) cout << "******* START OF IT CYCLE *********\n";
		CategorizeIT();
		if( m_debug > 1 ) cout << "******** END OF IT CYCLE **********\n";
	}

	if( m_systemsON.Contains("MSD") )
	{
		if( m_debug > 1 ) cout << "******* START OF MSD CYCLE *********\n";
		CategorizeMSD();
		if( m_debug > 1 ) cout << "******** END OF MSD CYCLE **********\n";
	}
	else
		SetTrackSeedNoMSD();

	if( m_systemsON.Contains("TW") )
	{
		if( m_debug > 1 ) cout << "******* START OF TW CYCLE *********\n";
		CategorizeTW();
		if( m_debug > 1 ) cout << "******** END OF TW CYCLE **********\n";
	}

	FillTrackCategoryMap();
}


//! \brief Track selection in the VT for forward data-like tracking
//!
//! The algorithm currently starts from VT tracklets and checks the number of points in them
void TAGFselectorStandard::CategorizeVT()
{
	TAVTntuVertex* vertexContainer = (TAVTntuVertex*) gTAGroot->FindDataDsc("vtVtx", "TAVTntuVertex")->Object();
		//cluster test
	TAVTntuCluster* vtntuclus = (TAVTntuCluster*) gTAGroot->FindDataDsc("vtClus","TAVTntuCluster")->Object(); //To find the right clus Index -> TO BE CHANGED!

	if(!vertexContainer || !vtntuclus)
	{
		Warning("CategorizeVT()", "VT vertices or ntuCluster pointers are null. Skipping event...");
		return;
	}

	int vertexNumber = vertexContainer->GetVertexN();
	// if( vertexNumber != 1 )
	// {
	// 	Info("CategorizeVT()", "Too many vertices in event. skipping ...");
	// 	return;
	// }
	if( vertexNumber == 1)
		(*m_singleVertexCounter)++;

	TVector3 pos_(0, 0, 0);		//global coord [cm]
    TVector3 mom_(0, 0, 7.);	//GeV //considering that fragments have same velocity of beam this should be changed accordingly

    if ( m_debug > 1 )		cout << "TAGFselectorStandard::CategorizeVT()  --  " << vertexNumber << "\n";

	//loop over all vertices
	for (Int_t iVtx = 0; iVtx < vertexNumber; ++iVtx) {
		TAVTvertex* vtxPD = vertexContainer->GetVertex(iVtx);
		if (vtxPD == 0x0){
			cout << "Vertex number " << iVtx << " seems to be empty\n";
			continue;
		}
		else if( !vtxPD->IsBmMatched() )
		{
			if(m_debug > 0)
			{
				Info("CategorizeVT()", "In event %d: vertex %d found but not matched with BM tracks; Skipping...",gTAGroot->CurrentEventId().EventNumber(), iVtx);
			}
			continue;
		}

		if ( m_debug > 0 )	cout << "vertex number " << iVtx << " has this nr of tracks " << vtxPD->GetTracksN() <<"\n";

		//loop over tracks for each Vertex
		for (int iTrack = 0; iTrack < vtxPD->GetTracksN(); ++iTrack) {

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
						if(iPart[k] != -666)
						{
							TAMCpart* particle = m_McNtuEve->GetTrack( iPart[k] );
							TVector3 mcMom = particle->GetInitP();
							cout << "\t\t-charge: " << particle->GetCharge() << "  mom:"; mcMom.Print();
						}
						else
							cout << "Pile-up particle from VT!" << endl;
					}
				}

			}	// end cluster loop

			if (fitTrack_->getNumPointsWithMeasurement() > 4 || fitTrack_->getNumPointsWithMeasurement() < 3){
				Warning("Categorize_dataLike()", "Track with %d measurements found in VTX => rejected!", fitTrack_->getNumPointsWithMeasurement());
				delete fitTrack_;
				continue;
			}

			for ( int nRep=0; nRep < m_trackRepVec.size(); nRep++) {
				fitTrack_->addTrackRep( m_trackRepVec.at( nRep )->clone() );
				if ( m_debug > 0 ) {
					cout << "TAGFselectorStandard::CategorizeVT() -- rep charge = " << m_trackRepVec.at( nRep )->getPDGCharge() << "\n";
				}
			}

			int IdTrack = iVtx*1000 + iTrack;

			m_trackTempMap[IdTrack] = new Track(*fitTrack_);
			m_trackSlopeMap[IdTrack] = m_GeoTrafo->VecFromVTLocalToGlobal(tracklet->GetSlopeZ());
			m_trackSlopeMap[IdTrack] *= 1./m_trackSlopeMap[IdTrack].Z();
		
			delete fitTrack_;
		}	// end track loop

	} //end loop on vertices

	if(m_trackTempMap.size() == 0)
	{
		// Warning("CategorizeVT()","No valid VT tracklet found in the event!");
		(*m_noVTtrackletEvents)++;
	}
}


//! \brief Track selection at IT level
//!
//! Currently this step is performed w/ a linear extrapolation
void TAGFselectorStandard::CategorizeIT()	{

	// ExtrapFromVTXtoIT 
	TVector3 tmpExtrap, tmpVTX;

	// same index if VTX_tracklets (for one vertex..)
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack) {
		int addedMeas = 0;
		
		//Get last VT measurement for extrapolation
		PlanarMeasurement* lastVTXMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(-1)->getRawMeasurement());
		int VTsensorId = m_SensorIDMap->GetSensorIDFromMeasID( lastVTXMeas->getHitId() );
		tmpVTX = TVector3(lastVTXMeas->getRawHitCoords()(0), lastVTXMeas->getRawHitCoords()(1), 0);
		tmpVTX = m_GeoTrafo->FromVTLocalToGlobal( m_VT_geo->Sensor2Detector(VTsensorId, tmpVTX) ); //move to global coords

		//Get some parameters for IT FitPlanes
		int maxITdetPlane = m_SensorIDMap->GetMaxFitPlane("IT");
		int minITdetPlane = m_SensorIDMap->GetMinFitPlane("IT");
		vector<float>* allZinIT = m_SensorIDMap->GetPossibleITz();


		for ( int iz = 0; iz < allZinIT->size(); iz++ ) {
			
			double tmpITz = allZinIT->at(iz);
			tmpExtrap = tmpVTX + m_trackSlopeMap[itTrack->first]*( tmpITz - tmpVTX.Z() );

			vector<int>* planesAtZ  = m_SensorIDMap->GetPlanesAtZ( tmpITz );

			// select a matching plane -> CHECK!!!!!!!!!!!!!!!
			// RZ: there is a potentially bad issue here with the bending plane!!! the intersection might be in another sensor since it is done with a linear extrapolation. Would it be better to only check the y? how much do we risk of f-ing this up?

			Int_t sensorId;
			for ( vector<int>::iterator iPlane = planesAtZ->begin(); iPlane != planesAtZ->end(); ++iPlane ) {
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
				double distanceInY = m_ITtolerance;
				double distanceInX;

				if ( m_allHitMeas->find( sensorMatch ) == m_allHitMeas->end() )   {
					if( m_debug > 1)	cout << "TAGFselectorStandard::CategorizeIT() -- WARNING extapolated plane empty!\n";
					continue;
				}

				do {
					int count = 0;
					for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( sensorMatch ).begin(); it != m_allHitMeas->at( sensorMatch ).end(); ++it){
					
						// check -> RZ: why do we need this check???? is it for debug only??
						if ( m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != sensorMatch )	cout << "TAGFselectorStandard::Categorize_dataLike() --> ERROR IT" <<endl, exit(0);

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
	}	// end loop on GF Track candidates
}





//! \brief Track selection at the MSD for data-like "Standard" selection
//!
//! This step uses a Kalman Filter extrapolation to find the MSD measurements of the track
void TAGFselectorStandard::CategorizeMSD()	{

	KalmanFitter* m_fitter_extrapolation = new KalmanFitter(1);

	// Extrapolate to MSD
	// same index if VTX_tracklets (for one vertex..)
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end();++itTrack) {

		int findMSD=0;

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
		if(m_debug > 0)	cout << "\nSelectorKalmanGF::CategorizeMSD()  -- track " << itTrack->first << "has this number of Reps = "<< itTrack->second->getNumReps() <<"\n";

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
			double distanceInY = m_MSDtolerance;
			int sensorMatch = MSDnPlane;

			if ( m_allHitMeas->find( MSDnPlane ) == m_allHitMeas->end() ) {
				if(m_debug > 0) cout << "TAGFselectorStandard::CategorizeMSD() -- no measurement found in MSDnPlane "<< MSDnPlane<<"\n";
				continue;
			}

			for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( MSDnPlane ).begin(); it != m_allHitMeas->at( MSDnPlane ).end(); ++it){
			// cout << "TAGFselectorStandard::CategorizeMSD()     MSDcheck4\n";
				if ( m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != sensorMatch )	cout << "TAGFselectorStandard::Categorize_dataLike() --> ERROR MSD" <<endl, exit(0);

				//RZ: CHECK -> AVOID ERRORS
				double distanceFromHit;
				string strip;

				if ( ! static_cast<PlanarMeasurement*>(*it)->getYview() )
				{
					distanceFromHit = fabs(guessOnMSD.X() - (*it)->getRawHitCoords()(0));
					strip = "X";
				}
				else
				{
					distanceFromHit = fabs(guessOnMSD.X() - (*it)->getRawHitCoords()(0));
					strip = "Y";
				}
				if( m_debug > 1 )
				{
					cout << "CHECK MATCH! strip" << strip << " meas::" << (*it)->getRawHitCoords()(0) << " guess::";
					guessOnMSD.Print();
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

	}// end loop on GF Track candidates
	delete m_fitter_extrapolation;

}


//! \brief Set the Track seed and perform first level fit of the track if there is no MSD in the global reconstruction
//!
//! Auxiliary function to have a functioning VT tracklet extrapolation up to the TW when MSD is not included
void TAGFselectorStandard::SetTrackSeedNoMSD()
{
	KalmanFitter* m_fitter_extrapolation = new KalmanFitter(1);

	//Set the seed of the track fit when MSD is not included
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack) {

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
		if(m_debug > 0)	cout << "\nSelectorKalmanGF::SetTrackSeedNoMSD()  --  number of Reps = "<< itTrack->second->getNumReps() <<"\n";

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
		


	}// end loop on GF Track candidates
	delete m_fitter_extrapolation;
}


//! \brief TW track selection for data-like "Standard" categorization
//!
//! This step uses a Kalman Filter extrapolation
void TAGFselectorStandard::CategorizeTW()
{
	int planeTW = m_SensorIDMap->GetFitPlaneTW();
	if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() ) {
		if(m_debug > 0) cout << "TAGFselectorStandard::CategorizeTW() -- no measurement found in TW layer\n";
		return;
	}

	// Extrapolate to TW
	KalmanFitter* m_fitter_extrapolation = new KalmanFitter(1);
	m_fitter_extrapolation->setMaxIterations(1);
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack) 
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
		double TWdistance = m_TWtolerance;
		int indexOfMin = -1;
		int count = 0;

		for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( planeTW ).begin(); it != m_allHitMeas->at( planeTW ).end(); ++it){

			if (  m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != planeTW )
				cout << "TAGFselectorStandard::Categorize_dataLike() --> ERROR TW" <<endl, exit(0);

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