
#include "TAGFuploader.hxx"



TAGFuploader::TAGFuploader ( TAGFdetectorMap* aSensorIDmap ) {

	m_sensorIDmap = aSensorIDmap;
	// m_detectorPlaneID = 

	m_measParticleMC_collection = new map< int, vector<int> >();
	
	m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

	m_debug = TAGrecoManager::GetPar()->Debug();
	// list of detectors used for kalman
	m_systemsON = "";
	for (unsigned int i=0; i<TAGrecoManager::GetPar()->KalSystems().size(); i++ ) {
		if (i != 0)		m_systemsON += " ";
		m_systemsON += TAGrecoManager::GetPar()->KalSystems().at(i);
	}

	switchOff_HHe = true;

}






//----------------------------------------------------------------------------------------------------
// pack together the hits to be fitted, from all the detectors, selct different preselecion m_systemsONs
int TAGFuploader::TakeMeasHits4Fit(  map< int, vector<AbsMeasurement*> > &allHitMeas  ) {

	m_allHitMeas = &allHitMeas;

	if ( m_debug > 0 )		cout << "\n\n*******\tUploaderKalmanGF::PrepareData4Fit\t*******\n" << endl;

	// Vertex -  fill fitter collections
	if ( (m_systemsON == "all" || m_systemsON.find( "VT" ) != string::npos) && TAGrecoManager::GetPar()->IncludeVT() ) {
		UploadClusVT();
		if ( m_debug > 0 )		Info( "TakeMeasHits4Fit", "Filling vertex hit collection " );
	}

	// Inner Tracker -  fill fitter collections
	if ( (m_systemsON == "all" || m_systemsON.find( "IT" ) != string::npos) && TAGrecoManager::GetPar()->IncludeIT() ) {
		UploadClusIT();
		if ( m_debug > 0 )		Info( "TakeMeasHits4Fit", "Filling Inner Tracker hit collection ");
	}

	// MSD -  fill fitter collections
	if ( (m_systemsON == "all" || m_systemsON.find( "MSD" ) != string::npos) && TAGrecoManager::GetPar()->IncludeMSD() ) {
		UploadClusMSD();
		if ( m_debug > 0 )		cout << endl<<endl << "Filling Strip hit collection"<<endl;
	}

	// Tof Wall-  fill fitter collections
	if ( ( m_systemsON.find( "TW" ) != string::npos) && TAGrecoManager::GetPar()->IncludeTW() ) {
		UploadHitsTW();
		if ( m_debug > 0 )		cout <<endl<<endl << "Filling scintillator hit collection " << endl;
	}

	if(m_debug > 0) cout << "TAGFuploader::TakeMeasHits4Fit  ->  " << m_allHitMeas->size() << endl;

	return 1;
  
}








//-------------------------------------------------------------------------------------------------
int TAGFuploader::UploadClusVT(){

	//cluster test
	TAVTntuCluster* vtclus = (TAVTntuCluster*) gTAGroot->FindDataDsc("vtClus","TAVTntuCluster")->Object();

	int totClus = 0;
	int nSensors = ( (TAVTparGeo*)gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() )->GetSensorsN();

	// loop over VT planes
	for( int iSensor = 0; iSensor < nSensors; iSensor++){

		int nclus = vtclus->GetClustersN(iSensor);

		if (m_debug > 1)	    Info( "UploadClusVT()", "\nfound %i  in sensor %i", nclus, iSensor );

		if (nclus == 0) continue;
		totClus += nclus;

		// loop over Clusters
		for( int iClus = 0; iClus < nclus; ++iClus ) {

			if (m_debug > 1)	      Info( "UploadClusVT()", "entered cycle clusVT of sensor %d", iSensor );

			TAVTcluster* clus = vtclus->GetCluster(iSensor, iClus);
			if (!clus->IsValid()) continue;		// Guardare meglio cosa significa...

			// m_VT_clusCollection.push_back(clus);
			Prepare4Vertex( clus, m_sensorIDmap->GetMeasID_eventLevel( "VT", iSensor, iClus ) );

		}
	}
	return totClus;
}









//---------------------------------------------------------------------------------------------------
int TAGFuploader::UploadClusIT(){

	TAITntuCluster* itclus = (TAITntuCluster*) gTAGroot->FindDataDsc("itClus","TAITntuCluster")->Object();

	int totClus = 0;
	int nSensors = ( (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object() )->GetSensorsN();

	for( int iSensor = 0; iSensor < nSensors; iSensor++){

		int nclus = itclus->GetClustersN(iSensor);

		if (m_debug > 1)    std::cout << "\nfound " << nclus << " in sensor " << iSensor << std::endl;

		if (nclus == 0) continue;
		totClus += nclus;

		for( int iClus = 0; iClus < nclus; ++iClus ) {

			if (m_debug > 1)      std::cout << "entered cycle clusIT of sensor " << iSensor << std::endl;

			TAITcluster* clus = itclus->GetCluster(iSensor, iClus);
			if (!clus->IsValid()) continue;		// Guardare meglio cosa significa...

			// m_IT_clusCollection.push_back(clus);
			Prepare4InnerTracker( clus, m_sensorIDmap->GetMeasID_eventLevel( "IT", iSensor, iClus ) );

		}
	}
	return totClus;
}







//----------------------------------------------------------------------------------------------------
// upload clusters from micro strip
int TAGFuploader::UploadClusMSD() {

	TAMSDntuCluster* msdclus = (TAMSDntuCluster*) gTAGroot->FindDataDsc("msdClus","TAMSDntuCluster")->Object();

	int totClus = 0;
	int nSensors = ( (TAMSDparGeo*) gTAGroot->FindParaDsc("msdGeo", "TAMSDparGeo")->Object() )->GetSensorsN();

	for( int iSensor = 0; iSensor < nSensors; iSensor++){

		int nclus = msdclus->GetClustersN(iSensor);

		if (m_debug > 1)	    std::cout << "\nfound " << nclus << " in sensor " << iSensor << std::endl;

		if (nclus == 0) continue;
		totClus += nclus;

		for(Int_t iClus = 0; iClus < nclus; ++iClus){

			if (m_debug > 1)	      std::cout << "entered cycle clusMSD of sensor " << iSensor << std::endl;

			TAMSDcluster* clus = msdclus->GetCluster(iSensor, iClus);
			if (!clus->IsValid()) continue;		// Guardare meglio cosa significa...

			// m_MSD_clusCollection.push_back(clus);
			Prepare4Strip( clus, m_sensorIDmap->GetMeasID_eventLevel( "MSD", iSensor, iClus ) );

		}
	}
	return totClus;
}








//----------------------------------------------------------------------------------------------------
// upload measurement points from Scintillator TofWall
int TAGFuploader::UploadHitsTW() {

	// take the ntuple object already filled
	TATWntuPoint* ntup = (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint", "TATWntuPoint")->Object();
	if ( m_debug > 0 )		cout << "number of TW points read: " << ntup->GetPointsN() << endl;

	int totPoints = ntup->GetPointsN();

	// save hits in the collection
	for ( int iPoint = 0; iPoint < totPoints; iPoint++ ) {

		TATWpoint* point = ntup->GetPoint(iPoint);

		//HACK to prevent fake points (try)
		if ( point->GetTime() > 15 ) 	continue;
		//if (point->GetMcTracksN() == 0) continue;
		//if ( point->GetChargeZ() < 1 ) continue; // wrong association, maybe neutron from calo

		// if ( GetTWTrackFixed( point ) == -1 ) continue;

		// m_TW_hitCollection.push_back( point );
		Prepare4TofWall( point, m_sensorIDmap->GetMeasID_eventLevel( "TW", 0, iPoint ) );

	}

	return totPoints;
}





//----------------------------------------------------------------------------------------------------
map< int, vector<int> >* TAGFuploader::TakeMeasParticleMC_Collection() {

	if ( !TAGrecoManager::GetPar()->IsMC() ) 
		cout << "ERROR in TAGFuploader::TakeMeasParticleMC_Collection() :: not runnninng onn MC!" << endl, exit(0);

  	return m_measParticleMC_collection;
}






// to be checked
int TAGFuploader::GetTWTrackFixed ( TATWpoint* point ) {

	int indexSize = point->GetMcTracksN();

	if ( indexSize == 1 )  {//in this case no pb
		int index = point->GetMcTrackIdx(0);
		if ( index != -1 ) return index;
	}


	TATWhit* rowHit = point->GetRowHit();
	TATWhit* colHit = point->GetColumnHit();

	int matching = -1;
	for (int iRow = 0; iRow < rowHit->GetMcTracksN(); ++iRow){
		for (int iCol = 0; iCol < colHit->GetMcTracksN(); ++iCol){
			if (rowHit->GetMcTrackIdx(iRow) == colHit->GetMcTrackIdx(iCol))
			matching = rowHit->GetMcTrackIdx(iRow);
		}
	}
	return matching;
}






void TAGFuploader::GetPossibleCharges( vector<int>* chVect ) {

	// // -------- TW CHARGE RETRIEVE NOT WORKING with Sept2020 but only with TruthParticles -----------------

	// TATWntuPoint* twPoint = (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint", "TATWntuPoint")->Object();
	// int tmp_ch;

	// // save hits in the collection
	// for (int iPoint = 0; iPoint < twPoint->GetPointsN(); iPoint++) {

	// 	TATWpoint* point = twPoint->GetPoint( iPoint );

	// 	tmp_ch = point->GetChargeZ();
	// 	// if ( m_debug > 1 ) 
	// 		cout << "TAGFuploader::GetPossibleCharges  " << tmp_ch << endl;
	// 	if ( tmp_ch > -1) {

	// 		if ( find( chVect->begin(), chVect->end(), tmp_ch ) == chVect->end() )
	// 			chVect->push_back( tmp_ch );	

	// 	}

	// // // 	// check if correct MC charge... for cross check only
		
	// }


	TAMCntuPart* m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();
	
	for ( int iPart = 0; iPart < m_McNtuEve->GetTracksN(); iPart++ ) {

		TAMCpart* point = m_McNtuEve->GetTrack(iPart);		
		if ( point->GetCharge() > 0 && point->GetCharge() <= 8) {
			if ( find( chVect->begin(), chVect->end(), point->GetCharge() ) == chVect->end() ) {
				chVect->push_back( point->GetCharge() );
				if ( m_debug > 0 )		
					cout << "TAGFuploader::GetPossibleCharges  " << point->GetCharge() << "\n";
			}
		}
		
	}

}







int TAGFuploader::GetNumGenParticle_noFrag() {

	
	TAMCntuPart* m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();
	
	TVector3 kversor (0,0,1);
	TVector3 iversor (1,0,0);
	// double z = 14;
	double theta=0,deltaxy=0, phi=0;


	int count = 0;
	for ( int iPart = 0; iPart < m_McNtuEve->GetTracksN(); iPart++ ) {

		TAMCpart* particle = m_McNtuEve->GetTrack(iPart);		
		if ( particle->GetCharge() > 0 && particle->GetCharge() <= 8) {

			if ( particle->GetInitPos().z() > 1 ) continue;
			if ( particle->GetFinalPos().z() < 99.6 ) continue;
			if ( particle->GetBaryon() < 1 ) continue;
			if ( particle->GetMass()< 0.937)continue;
			
			TVector3 pfinal = particle->GetFinalP();
			
			// theta = pfinal.Theta();
			// deltaxy= tan(theta)*(particle->GetFinalPos().Z()-43.03);

			// phi=pfinal.Angle(iversor);

			// if(particle->GetFinalPos().X() - deltaxy*cos(phi)>4.9075 || particle->GetFinalPos().X() - deltaxy*cos(phi)< -4.9075) continue;
			// if(particle->GetFinalPos().Y() - deltaxy*sin(phi)>4.9075 || particle->GetFinalPos().Y() - deltaxy*sin(phi)< -4.9075) continue; 

			theta =tan(4.9075/43.03);
			if (pfinal.Theta()>theta) continue;

			// if ( particle->)

			count++;

		}
		
	}

	
	return count;
}

// int TAGFuploader::GetNumParticle_noFrag_MC2()
// {
// int count=0;

// for(int iPlane = 0; iPlane < m_SensorIDMap->GetFitPlanesN(); ++iPlane)
// {


// }

// return int;

// }







//----------------------------------------------------------------------------------------------------
// taking all clusters
void TAGFuploader::Prepare4Vertex( TAVTcluster* clus, int iMeas ) {

	if ( m_debug > 0 )		cout << "\nPrepare4Vertex::Entered\n";

	TMatrixDSym hitCov(3);
	TMatrixDSym planarCov(2);
	TVectorD hitCoords(3);
	TVectorD planarCoords(2);


	// get pixel coord
	TVector3 hitPos = m_GeoTrafo->FromVTLocalToGlobal( clus->GetPositionG() );

	if ( m_debug > 1 )	  hitPos.Print();

	// set hit position vector
	hitCoords(0)=hitPos.x();	hitCoords(1)=hitPos.y();	hitCoords(2)=hitPos.z();
	planarCoords(0) = hitPos.x();	planarCoords(1) = hitPos.y();  

	// set covariance matrix
	TVector3 pixReso = clus->GetPosError();    // z???
	//   TVector3 pixReso =  m_GeoTrafo->FromVTLocalToGlobal( pixReso_ );

	// to be tested for SpasePoints
	hitCov.UnitMatrix();
	for (int j = 0; j < 3; j++){
	  hitCov[j][j] = pixReso(j)*pixReso(j);
	}
	hitCov[2][2] = 0.000005;		//hardcoded

	// planar points
	planarCov.UnitMatrix();
	for (int k = 0; k < 2; k++){
	  planarCov[k][k] = pixReso(k)*pixReso(k);
	  // planarCov[k][k] = 0.001*0.001;
	}

	// bool found_HHe = false;
	// TAMCntuPart* m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();

	vector<int> mcParticlesInMeasuerement;
	for (int nPart = 0; nPart < clus->GetMcTracksN(); nPart++) {
		// if ( m_McNtuEve->GetTrack( clus->GetMcTrackIdx( nPart ) )->GetCharge() <= 2 )	found_HHe = true;
		mcParticlesInMeasuerement.push_back( clus->GetMcTrackIdx( nPart ) );
	}

	// if ( switchOff_HHe && found_HHe )	return;

	// m_measParticleMC_collection[iMeas] = mcParticlesInMeasuerement;
	m_measParticleMC_collection->insert( pair<int, vector<int> > ( iMeas, mcParticlesInMeasuerement ) );
	// m_measParticleMC_collection->at( iMeas ) = mcParticlesInMeasuerement;

	int planeID = m_sensorIDmap->GetFitPlaneIDFromMeasID( iMeas );
	int detId = m_sensorIDmap->GetDetIDFromMeasID( iMeas );
	// nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, detId, iMeas, nullptr );
	hit->setPlane( m_sensorIDmap->GetFitPlane(planeID), planeID ); 
	// m_allHitMeas[ planeID ].push_back(hit);
	(*m_allHitMeas)[ planeID ].push_back(hit);


	if ( m_debug > 0 )  cout << "\nPrepare4Vertex::Exiting\n";
}










//----------------------------------------------------------------------------------------------------
void TAGFuploader::Prepare4InnerTracker( TAITcluster* clus, int iMeas ) {

	if ( m_debug > 0 )	  cout << "\nPrepare4InnerTracker::Entered\n";

	TMatrixDSym planarCov(2);
	TVectorD planarCoords(2);

	// get pixel coord
	TVector3 hitPos = m_GeoTrafo->FromITLocalToGlobal( clus->GetPositionG() );
	if ( m_debug > 1 )		hitPos.Print();

	// set hit position vector
	planarCoords(0) = hitPos.x();
	planarCoords(1) = hitPos.y();

	// set covariance matrix

	TVector3 pixReso = clus->GetPosError();
	planarCov.UnitMatrix();
	for (int k = 0; k < 2; k++){
		// planarCov[k][k] = 0.001*0.001;
	  planarCov[k][k] = pixReso(k)*pixReso(k);
	}

	// bool found_HHe = false;
	// TAMCntuPart* m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();

	vector<int> mcParticlesInMeasuerement;
	for (int nPart = 0; nPart < clus->GetMcTracksN(); nPart++) {
		// if ( m_McNtuEve->GetTrack( clus->GetMcTrackIdx( nPart ) )->GetCharge() <= 2 )	found_HHe = true;
		mcParticlesInMeasuerement.push_back( clus->GetMcTrackIdx( nPart ) );
	}
	// if ( switchOff_HHe && found_HHe )	return;

	// m_measParticleMC_collection[ iMeas ] = mcParticlesInMeasuerement;
	m_measParticleMC_collection->insert( pair<int, vector<int> > ( iMeas, mcParticlesInMeasuerement ) );

	int sensorID = m_sensorIDmap->GetFitPlaneIDFromMeasID( iMeas );
	int detId = m_sensorIDmap->GetDetIDFromMeasID( iMeas );
	// nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, detId, iMeas, nullptr );
	hit->setPlane( m_sensorIDmap->GetFitPlane(sensorID), sensorID );	 
	// m_detectorPlanes[tempPlane]->Print();

	// m_allHitMeas[ sensorID ].push_back(hit);
	(*m_allHitMeas)[ sensorID ].push_back(hit);


	if ( m_debug > 0 )			cout << "\nPrepare4InnerTracker::Exiting\n";

}









//----------------------------------------------------------------------------------------------------
void TAGFuploader::Prepare4Strip( TAMSDcluster* clus, int iMeas ) {

	if ( m_debug > 0 )	 cout << "\nPrepare4Strip::Entered\n";

	//TMatrixDSym planarCov(2);
	TMatrixDSym planarCov(1);
	//TVectorD planarCoords(2);
	TVectorD planarCoords(1);


	// get pixel coord
	TVector3 hitPos = m_GeoTrafo->FromMSDLocalToGlobal( clus->GetPositionG() );

	if ( m_debug > 1 )	    hitPos.Print();

	//check the view, 0 ->X, 1->Y
	double pixReso = 0;
	bool isYView = false;

	if ( clus->GetPlaneView() == 0 ){
		planarCoords(0) = hitPos.x();
		// pixReso = clus->GetPosError().X();
		// cout << "Err MSD X: " << pixReso << endl;
		pixReso = 0.003; //hardcoded!!!!!
	}
	else{
		planarCoords(0) = hitPos.y();
		// pixReso = clus->GetPosError().Y();
		// cout << "Err MSD Y: " << pixReso << endl;
		pixReso = 0.003; //hardcoded!!!!!
		isYView = true;
	}

	// set covariance matrix
	planarCov.UnitMatrix();
	for (int k = 0; k < 1; k++) {
		// planarCov[k][k] = 0.001*0.001;
		planarCov[k][k] = pixReso*pixReso;
	}

	// bool found_HHe = false;
	// TAMCntuPart* m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();

	vector<int> mcParticlesInMeasuerement;
	for (int nPart = 0; nPart < clus->GetMcTracksN(); nPart++) {
		// if ( m_McNtuEve->GetTrack( clus->GetMcTrackIdx( nPart ) )->GetCharge() <= 2 )	found_HHe = true;
		mcParticlesInMeasuerement.push_back( clus->GetMcTrackIdx( nPart ) );
	}
	// if ( switchOff_H	He && found_HHe )	return;
	// m_measParticleMC_collection[ iMeas ] = mcParticlesInMeasuerement;
	m_measParticleMC_collection->insert( pair<int, vector<int> > ( iMeas, mcParticlesInMeasuerement ) );

	int sensorID = m_sensorIDmap->GetFitPlaneIDFromMeasID( iMeas );
	int detId = m_sensorIDmap->GetDetIDFromMeasID( iMeas );
	// nullptr is a TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, detId, iMeas, nullptr );
	hit->setPlane( m_sensorIDmap->GetFitPlane(sensorID), sensorID ); 

	if (isYView) hit->setStripV();

	// m_allHitMeas[ sensorID ].push_back(hit);
	(*m_allHitMeas)[ sensorID ].push_back(hit);

	if ( m_debug > 0 )	  cout << "\nPrepare4Strip::Exiting\n";

}









//----------------------------------------------------------------------------------------------------
void TAGFuploader::Prepare4TofWall( TATWpoint* point, int iMeas) {

	if ( m_debug > 0 )	  cout << "\nPrepare4TofWall::Entered\n";

	TMatrixDSym planarCov(2);
	TVectorD planarCoords(2);

	// get point coord
	TVector3 hitPos = m_GeoTrafo->FromTWLocalToGlobal( point->GetPositionG() );

	// if ( m_debug > 1 )	   hitPos.Print();

	// set point position vector
	planarCoords(0) = hitPos.x();
	planarCoords(1) = hitPos.y();
	TVector3 pixReso = point->GetPosErrorG();

	if ( m_debug > 1 )	   pixReso.Print();

	planarCov.UnitMatrix();
	for (int k = 0; k < 2; k++)
	{
		planarCov[k][k] = pixReso(k)*pixReso(k);
		if(m_debug > 1) 
			cout << pixReso(k)*pixReso(k) << endl;
	}

	vector<int> mcParticlesInMeasuerement;
	for (int nPart = 0; nPart < point->GetMcTracksN(); nPart++) {
		mcParticlesInMeasuerement.push_back( point->GetMcTrackIdx( nPart ) );
		if(m_debug > 0) 
		{
			cout << "TW meas::" << iMeas << "\tMCTrack::" << point->GetMcTrackIdx( nPart ) << endl;
			point->GetPosition().Print();
		}
	}
	// m_measParticleMC_collection[ iPoint ] = mcParticlesInMeasuerement;
	m_measParticleMC_collection->insert( pair<int, vector<int> > ( iMeas, mcParticlesInMeasuerement ) );

	int sensorID = m_sensorIDmap->GetFitPlaneTW();
	int detId = m_sensorIDmap->GetDetIDFromMeasID( iMeas);
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, detId, iMeas, nullptr );
	hit->setPlane(m_sensorIDmap->GetFitPlane(sensorID), sensorID);      
	(*m_allHitMeas)[ sensorID ].push_back(hit);
	// m_allHitMeas[ sensorID ].push_back(hit);

	if ( m_debug > 0 )	  cout << "\nPrepare4TofWall::Exiting\n";

}









// void GetTrueInfo() {
// 	GetMcTrackIdx(Int_t index);
//    	GetMcTracksN();
// }










































