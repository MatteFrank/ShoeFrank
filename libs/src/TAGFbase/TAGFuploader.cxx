
#include "TAGFuploader.hxx"



TAGFuploader::TAGFuploader ( map<string, int> adetectorID_map, map<int, genfit::SharedPlanePtr> adetectorPlanes ) {

	m_detectorID_map = adetectorID_map;
	m_detectorPlanes = adetectorPlanes;
	// m_detectorPlaneID = 

	m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

	m_debug = TAGrecoManager::GetPar()->Debug();
	// list of detectors used for kalman
	m_systemsON = "";
	for (unsigned int i=0; i<TAGrecoManager::GetPar()->KalSystems().size(); i++ ) {
		if (i != 0)		m_systemsON += " ";
		m_systemsON += TAGrecoManager::GetPar()->KalSystems().at(i);
	}

}






//----------------------------------------------------------------------------------------------------
// pack together the hits to be fitted, from all the detectors, selct different preselecion m_systemsONs
int TAGFuploader::TakeMeasHits4Fit(  map< int, vector<AbsMeasurement*> >* allHitMeas  ) {

	m_allHitMeas = allHitMeas;

	if ( m_debug > 0 )		cout << "\n\n*******\tTAGFuploader::PrepareData4Fit\t*******\n" << endl;

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
		if ( m_debug > 0 )		cout << endl<<endl << "Filling Strip hit collection = "<<endl;
	}

	// Tof Wall-  fill fitter collections
	if ( ( m_systemsON.find( "TW" ) != string::npos) && TAGrecoManager::GetPar()->IncludeTW() ) {
		UploadHitsTW();
		if ( m_debug > 0 )		cout <<endl<<endl << "Filling scintillator hit collection = " << endl;
	}

	return 1;
  
}







//-------------------------------------------------------------------------------------------------
int TAGFuploader::UploadClusVT(){

	//cluster test
	TAVTntuCluster* vtclus = (TAVTntuCluster*) gTAGroot->FindDataDsc("vtClus","TAVTntuCluster")->Object();

	int totClus = 0;
	int nPlanes = ( (TAVTparGeo*)gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() )->GetSensorsN();

	// loop over VT planes
	for( int iPlane = 0; iPlane < nPlanes; iPlane++){

		int nclus = vtclus->GetClustersN(iPlane);

		if (m_debug > 1)	    Info( "UploadClusVT()", "\nfound %i  in plane %i", nclus, iPlane );

		if (nclus == 0) continue;
		totClus += nclus;

		// loop over Clusters
		for( int iClus = 0; iClus < nclus; ++iClus ) {

			if (m_debug > 1)	      Info( "UploadClusVT()", "entered cycle clusVT of plane %d", iPlane );

			TAVTcluster* clus = vtclus->GetCluster(iPlane, iClus);
			if (!clus->IsValid()) continue;		// Guardare meglio cosa significa...

			// m_VT_clusCollection.push_back(clus);
			Prepare4Vertex( clus, iClus );

		}
	}
	return totClus;
}









//---------------------------------------------------------------------------------------------------
int TAGFuploader::UploadClusIT(){

	TAITntuCluster* itclus = (TAITntuCluster*) gTAGroot->FindDataDsc("itClus","TAITntuCluster")->Object();

	int totClus = 0;
	int nPlanes = ( (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object() )->GetSensorsN();

	for( int iPlane = 0; iPlane < nPlanes; iPlane++){

		int nclus = itclus->GetClustersN(iPlane);

		if (m_debug > 1)    std::cout << "\nfound " << nclus << " in plane " << iPlane << std::endl;

		if (nclus == 0) continue;
		totClus += nclus;

		for( int iClus = 0; iClus < nclus; ++iClus ) {

			if (m_debug > 1)      std::cout << "entered cycle clusIT of plane " << iPlane << std::endl;

			TAITcluster* clus = itclus->GetCluster(iPlane, iClus);
			if (!clus->IsValid()) continue;		// Guardare meglio cosa significa...

			// m_IT_clusCollection.push_back(clus);
			Prepare4InnerTracker( clus, iClus );

		}
	}
	return totClus;
}







//----------------------------------------------------------------------------------------------------
// upload clusters from micro strip
int TAGFuploader::UploadClusMSD() {

	TAMSDntuCluster* msdclus = (TAMSDntuCluster*) gTAGroot->FindDataDsc("msdClus","TAMSDntuCluster")->Object();

	int totClus = 0;
	int nPlanes = ( (TAMSDparGeo*) gTAGroot->FindParaDsc("msdGeo", "TAMSDparGeo")->Object() )->GetSensorsN();

	for( int iPlane = 0; iPlane < nPlanes; iPlane++){

		int nclus = msdclus->GetClustersN(iPlane);

		if (m_debug > 1)	    std::cout << "\nfound " << nclus << " in plane " << iPlane << std::endl;

		if (nclus == 0) continue;
		totClus += nclus;

		for(Int_t iClus = 0; iClus < nclus; ++iClus){

			if (m_debug > 1)	      std::cout << "entered cycle clusMSD of plane " << iPlane << std::endl;

			TAMSDcluster* clus = msdclus->GetCluster(iPlane, iClus);
			if (!clus->IsValid()) continue;		// Guardare meglio cosa significa...

			// m_MSD_clusCollection.push_back(clus);
			Prepare4Strip( clus, iClus );

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

		if ( GetTWTrackFixed( point ) == -1 ) continue;

		// m_TW_hitCollection.push_back( point );
		Prepare4TofWall( point, iPoint );

	}

	return totPoints;
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

	TATWntuPoint* twPoint = (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint", "TATWntuPoint")->Object();
	int tmp_ch;

	// save hits in the collection
	for (int iPoint = 0; iPoint < twPoint->GetPointsN(); iPoint++) {

		TATWpoint* point = twPoint->GetPoint( iPoint );

		tmp_ch = point->GetChargeZ();
		if ( tmp_ch > -1) {

			if ( find( chVect->begin(), chVect->end(), tmp_ch ) == chVect->end() )
				chVect->push_back( tmp_ch );	

		}

		// check if correct MC charge... for cross check only
		
	}

}









//----------------------------------------------------------------------------------------------------
// taking all clusters
void TAGFuploader::Prepare4Vertex( TAVTcluster* clus, int iClus ) {

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
	}


	// nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
	//AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["VT"], i, nullptr );	// to be tested for SpasePoints
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["VT"], iClus, nullptr );
	hit->setPlane( m_detectorPlanes[clus->GetSensorIdx()], clus->GetSensorIdx() );  // to be changed!!!!!!!!!!!!!!!!!!
	m_allHitMeas->at( clus->GetSensorIdx() ).push_back(hit);


	if ( m_debug > 0 )  cout << "\nPrepare4Vertex::Exiting\n";
}










//----------------------------------------------------------------------------------------------------
void TAGFuploader::Prepare4InnerTracker( TAITcluster* clus, int iClus ) {

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
	  planarCov[k][k] = pixReso(k)*pixReso(k);
	}

	// nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
	//AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["IT"], i, nullptr );
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["IT"], iClus, nullptr );
	hit->setPlane( m_detectorPlanes[clus->GetSensorIdx()], clus->GetSensorIdx() );	  // to be changed!!!!!!!!!!!!!!!!!!
	// m_detectorPlanes[tempPlane]->Print();

	m_allHitMeas->at( clus->GetSensorIdx() ).push_back(hit);


	if ( m_debug > 0 )			cout << "\nPrepare4InnerTracker::Exiting\n";

}









//----------------------------------------------------------------------------------------------------
void TAGFuploader::Prepare4Strip( TAMSDcluster* clus, int iClus ) {

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
		pixReso = clus->GetPosError().X();
	}
	else{
		planarCoords(0) = hitPos.y();
		pixReso = clus->GetPosError().Y();
		isYView = true;
	}

	// set covariance matrix
	planarCov.UnitMatrix();
	for (int k = 0; k < 1; k++)
		planarCov[k][k] = pixReso*pixReso;

	// nullptr is a TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
	//AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["MSD"], i, nullptr );
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["MSD"], iClus, nullptr );
	hit->setPlane( m_detectorPlanes[clus->GetSensorIdx()+12], clus->GetSensorIdx()+12 );  // to be changed!!!!!!!!!!!!!!!!!!

	if (isYView) hit->setStripV();

	m_allHitMeas->at( clus->GetSensorIdx()+12 ).push_back(hit);

	if ( m_debug > 0 )	  cout << "\nPrepare4Strip::Exiting\n";

}









//----------------------------------------------------------------------------------------------------
void TAGFuploader::Prepare4TofWall( TATWpoint* point, int iPoint) {

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
	// if ( m_debug > 1 )	   pixReso.Print();

	planarCov.UnitMatrix();
	for (int k = 0; k < 2; k++)
		planarCov[k][k] = pixReso(k)*pixReso(k);



	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["TW"], iPoint, nullptr );
	hit->setPlane(m_detectorPlanes[18], 18);       // to be changed!!!!!!!!!!!!!!!!!!
	m_allHitMeas->at( 18 ).push_back(hit);

	if ( m_debug > 0 )	  cout << "\nPrepare4TofWall::Exiting\n";

}









// void GetTrueInfo() {
// 	GetMcTrackIdx(Int_t index);
//    	GetMcTracksN();
// }










































