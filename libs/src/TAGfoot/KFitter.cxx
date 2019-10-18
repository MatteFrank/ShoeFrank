

#include "KFitter.hxx"





//----------------------------------------------------------------------------------------------------
KFitter::KFitter () {

  int nIter = 20; // max number of iterations
  double dPVal = 1.E-3; // convergence criterion
  m_AMU = 0.9310986964; // in GeV // conversion betweem mass in GeV and atomic mass unit
  
  m_debug = GlobalPar::GetPar()->Debug();
  
  // if start KF in forward or reverse mode
  m_reverse = GlobalPar::GetPar()->IsKalReverse();

  // m_diceRoll = new TRandom3();
  //    m_diceRoll->SetSeed(0);

  // clear hits collection
  m_VT_hitCollection.clear();
  m_IT_hitCollection.clear();
  m_MSD_hitCollection.clear();

  m_VT_clusCollection.clear();
  m_IT_clusCollection.clear();
  m_MSD_clusCollection.clear();

  m_fitTrackCollection = new GlobalTrackRepostory();  // contains the fitted track info

  // Create dir for kalman output
  struct stat info;
  m_kalmanOutputDir = (string)getenv("FOOTRES")+"/Kalman";
  if( stat( m_kalmanOutputDir.c_str(), &info ) != 0 )		//cannot access
    system(("mkdir "+m_kalmanOutputDir).c_str());

  // class for control plot dumping
  m_controlPlotter = ControlPlotsRepository::GetControlObject( m_kalmanOutputDir );

  // checks for the detector to be used for kalman
  IncludeDetectors();

  // take the geometry object
  if ( (m_systemsON == "all" || m_systemsON.find( "VT" ) != string::npos) && GlobalPar::GetPar()->IncludeVertex() )
    m_VT_geo = shared_ptr<TAVTparGeo> ( (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() );
  if ( (m_systemsON == "all" || m_systemsON.find( "IT" ) != string::npos) && GlobalPar::GetPar()->IncludeInnerTracker() )
    m_IT_geo = shared_ptr<TAITparGeo> ( (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object() );
  if ( (m_systemsON == "all" || m_systemsON.find( "MSD" ) != string::npos) && GlobalPar::GetPar()->IncludeMSD() )
    m_MSD_geo = shared_ptr<TAMSDparGeo> ( (TAMSDparGeo*) gTAGroot->FindParaDsc("msdGeo", "TAMSDparGeo")->Object() );
  // if ( (m_systemsON == "all" || m_systemsON.find( "SCT" ) != string::npos) && GlobalPar::GetPar()->IncludeMSD() )
  if ( ( m_systemsON.find( "TW" ) != string::npos) && GlobalPar::GetPar()->IncludeTW() )
    m_TW_geo = shared_ptr<TATWparGeo> ( (TATWparGeo*) gTAGroot->FindParaDsc("twGeo", "TATWparGeo")->Object() );


  // initialise the kalman method selected from param file
  if ( GlobalPar::GetPar()->KalMode() == 1 )
    m_fitter = new KalmanFitter(nIter, dPVal);
  else if ( GlobalPar::GetPar()->KalMode() == 2 )
    m_refFitter = new KalmanFitterRefTrack(nIter, dPVal);
  else if ( GlobalPar::GetPar()->KalMode() == 3 )
    m_dafRefFitter = new DAF(true, nIter, dPVal);
  else if ( GlobalPar::GetPar()->KalMode() == 4 )
    m_dafSimpleFitter = new DAF(false, nIter, dPVal);

  InitEventDisplay();		// empty!!!!


  m_vecHistoColor = { kBlack, kRed-9, kRed+1, kRed-2, kOrange+7, kOrange, kOrange+3, kGreen+1,
		      kGreen+3, kBlue+1, kBlue+3, kAzure+8, kAzure+1, kMagenta+2,
		      kMagenta+3, kViolet+1, kViolet+6, kViolet-4 };

}





//----------------------------------------------------------------------------------------------------
// check and print which detectors included and/or used in the kalman
void KFitter::IncludeDetectors() {

  // all possible detector and a map with an ID num
  vector<string> tmp_detName = { "STC", "BM", "TG", "VT", "IT", "MSD", "TW", "CALO" };
  for (unsigned int i=0; i<tmp_detName.size(); i++)
    m_detectorID_map[ tmp_detName[i] ] = i;

  // check kalman detectors set in param file are correct
  if ( !(GlobalPar::GetPar()->KalSystems().size() == 1 && GlobalPar::GetPar()->KalSystems().at(0) == "all") )	 {
    // if ( GlobalPar::GetPar()->KalSystems().size() != 0 )	 {
    for (unsigned int i=0; i<GlobalPar::GetPar()->KalSystems().size(); i++ ) {
      if ( m_detectorID_map.find( GlobalPar::GetPar()->KalSystems().at(i) ) == m_detectorID_map.end() )
	cout<< "ERROR::KFitter::KFitter  --> KalSystems parameter not set properly, check before continue."<< endl, exit(0);
    }
  }
  if (GlobalPar::GetPar()->KalSystems().size() == 0)		cout<< "ERROR::KFitter::KFitter  --> KalSystems parameter not set properly, zero parameters, check befor continue."<< endl, exit(0);

  // list of detectors used for kalman
  m_systemsON = "";
  for (unsigned int i=0; i<GlobalPar::GetPar()->KalSystems().size(); i++ ) {
    if (i != 0)		m_systemsON += " ";
    m_systemsON += GlobalPar::GetPar()->KalSystems().at(i);
  }
  if (m_debug > 0)	cout << "Detector systems for Kalman:  " << m_systemsON << endl;

  // print-out of the particle hypothesis used for the fit
  cout << "GlobalPar::GetPar()->MCParticles()";
  for (unsigned int i=0; i<GlobalPar::GetPar()->MCParticles().size(); i++ ) {
    cout << "   " << GlobalPar::GetPar()->MCParticles().at(i);
  }
  cout << endl;
}







//----------------------------------------------------------------------------------------------------
// upload measurement points from vertex pixel
int KFitter::UploadHitsVT() {
  

  // take the ntuple object already filled
  TAVTntuRaw* ntup = (TAVTntuRaw*) gTAGroot->FindDataDsc("vtRaw", "TAVTntuRaw")->Object();
  TAVTparGeo* vtxGeo = (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object();
  
  if ( m_debug > 0 )		cout << "N vertex sensors: " << vtxGeo->GetSensorsN() << endl;
  
  // MC hits example
  // TAMCntuEve* ntuMC = (TAMCntuEve*) gTAGroot->FindDataDsc("myn_mceve", "TAMCntuEve")->Object();
  // cout << "Number of MC tracks from repo  " << ntuMC->nhit  << endl;
  
  int totPix = 0;
  // save pixels in the collection
  for (int nSensor = 0; nSensor < vtxGeo->GetSensorsN(); nSensor++) {	// over all sensors
    totPix += ntup->GetPixelsN( nSensor );
    if ( m_debug > 0 )		cout << "N vertex pixel in sensor " << nSensor << ": " << ntup->GetPixelsN( nSensor ) << endl;
    
    for (int nPx = 0; nPx < ntup->GetPixelsN( nSensor ); nPx++) 		// over all pixels for each sensor
      m_VT_hitCollection.push_back( ntup->GetPixel( nSensor, nPx) );
  }
  
  return totPix;
}



//-------------------------------------------------------------------------------------------------
int KFitter::UploadClusVT(){

  fGeoTrafo = new TAGgeoTrafo();
  fGeoTrafo->FromFile();

  //cluster test
  TAVTparGeo* vtxGeo = (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object();
  TAVTntuCluster* vtclus = (TAVTntuCluster*) gTAGroot->FindDataDsc("vtClus","TAVTntuCluster")->Object();

  TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("myn_mceve", "TAMCntuEve")->Object(); 
  TAMCntuHit* vtMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("vtMc", "TAMCntuHit")->Object();

  int totClus = 0;
  Int_t nPlanes = vtxGeo->GetSensorsN();

  for(Int_t iPlane = 0; iPlane < nPlanes; iPlane++){
    Int_t nclus = vtclus->GetClustersN(iPlane);
    totClus += nclus;
    if (nclus == 0) continue;

    for(Int_t iClus = 0; iClus < nclus; ++iClus){
      TAVTcluster* clus = vtclus->GetCluster(iPlane, iClus);
      if (!clus->IsValid()) continue;

      m_VT_clusCollection.push_back(clus);


      //---------------------------testing access to hits and pixels from cluster--------------------
      Int_t nHits = clus->GetPixelsN();
      // cout << "nHits: " << nHits  << endl;
      

      for (Int_t jHit = 0; jHit < nHits; ++jHit) {
      	TAVTntuHit* hit = clus->GetPixel(jHit);
      	for (Int_t k = 0; k < hit->GetMcTrackCount(); ++k) {
	  Int_t id = hit->GetMcTrackId(k);
	  Int_t idx = hit->GetMcIndex(k);
	  // cout << "id: " << id  << endl;
	  // cout << "idxx: " << idx  << endl;
	  // printf("TrackMcId %d ", id);
	  TAMCeveTrack* track = eve->GetHit(id);
	  //printf("charge %d mass %g ", track->GetCharge(), track->GetMass());
	  TAMChit* mcHit = vtMc->GetHit(idx);
	  TVector3 pos = mcHit->GetInPosition();
	  // printf("MC pos (%.4f %.4f %.4f)\n", pos[0], pos[1], pos[2]);
	}
      }

      //---------------------------------------------------------------------------------------------
    }
  }
  
}

//----------------------------------------------------------------------------------------------------
// upload measurement points from Inner Tracker pixel
int KFitter::UploadHitsIT() {

  // take the ntuple object already filled
  TAITntuRaw* ntup = (TAITntuRaw*) gTAGroot->FindDataDsc("itRaw", "TAITntuRaw")->Object();
  TAITparGeo* vtxGeo = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetItDefParaName(), "TAITparGeo")->Object();

  if ( m_debug > 0 )		cout << "N IT sensors: " << vtxGeo->GetSensorsN() << endl;


  int totPix = 0;
  // save pixels in the collection
  for (int nSensor = 0; nSensor < vtxGeo->GetSensorsN(); nSensor++) {	// over all sensors
    totPix += ntup->GetPixelsN( nSensor);
    if ( m_debug > 0 )		cout << "N IT pixel in sensor " << nSensor << ": " << ntup->GetPixelsN( nSensor) << endl;

    for (int nPx = 0; nPx < ntup->GetPixelsN( nSensor); nPx++) 		// over all pixels for each sensor
      m_IT_hitCollection.push_back( (TAITntuHit*)ntup->GetPixel( nSensor, nPx) );

  }

  return totPix;
}


//---------------------------------------------------------------------------------------------------
int KFitter::UploadClusIT(){

  fGeoTrafo = new TAGgeoTrafo();
  fGeoTrafo->FromFile();

  //cluster test
  TAITparGeo* itGeo = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetDefParaName(), "TAITparGeo")->Object();
  TAITntuCluster* itclus = (TAITntuCluster*) gTAGroot->FindDataDsc("itClus","TAITntuCluster")->Object();

  TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("myn_mceve", "TAMCntuEve")->Object(); 
  TAMCntuHit* itMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("vtMc", "TAMCntuHit")->Object();

  int totClus = 0;
  Int_t nPlanes = itGeo->GetSensorsN();

  for(Int_t iPlane = 0; iPlane < nPlanes; iPlane++){
    Int_t nclus = itclus->GetClustersN(iPlane);
    totClus += nclus;
    if (nclus == 0) continue;

    for(Int_t iClus = 0; iClus < nclus; ++iClus){
      TAITcluster* clus = itclus->GetCluster(iPlane, iClus);
      if (!clus->IsValid()) continue;

      m_IT_clusCollection.push_back(clus);


      //---------------------------testing access to hits and pixels from cluster--------------------
      Int_t nHits = clus->GetPixelsN();
      // cout << "nHits: " << nHits  << endl;
      

      for (Int_t jHit = 0; jHit < nHits; ++jHit) {
      	TAITntuHit* hit = (TAITntuHit*)clus->GetPixel(jHit);
      	for (Int_t k = 0; k < hit->GetMcTrackCount(); ++k) {
	  Int_t id = hit->GetMcTrackId(k);
	  Int_t idx = hit->GetMcIndex(k);
	  // cout << "id: " << id  << endl;
	  // cout << "idxx: " << idx  << endl;
	  // printf("TrackMcId %d ", id);
	  TAMCeveTrack* track = eve->GetHit(id);
	  //printf("charge %d mass %g ", track->GetCharge(), track->GetMass());
	  TAMChit* mcHit = itMc->GetHit(idx);
	  TVector3 pos = mcHit->GetInPosition();
	  // printf("MC pos (%.4f %.4f %.4f)\n", pos[0], pos[1], pos[2]);
	}
      }

      //---------------------------------------------------------------------------------------------
    }
  }
  
}



//----------------------------------------------------------------------------------------------------
// upload measurement points from Strip
int KFitter::UploadHitsMSD() {

  // take the ntuple object already filled
  TAMSDntuRaw* ntup = (TAMSDntuRaw*) gTAGroot->FindDataDsc("msdRaw", "TAMSDntuRaw")->Object();
  if ( m_debug > 0 )		cout << "Strip detector hits read: " << ntup->GetStripsN(0) << endl;

  // save strips hits in the collection
  for (int i = 0; i < ntup->GetStripsN(0); i++)
    m_MSD_hitCollection.push_back( ntup->GetStrip(0,i) );

  return ntup->GetStripsN(0);
}




//----------------------------------------------------------------------------------------------------
// upload measurement points from Scintillator TofWall
int KFitter::UploadHitsTW() {

  // take the ntuple object already filled
  TATWntuPoint* ntup = (TATWntuPoint*) gTAGroot->FindDataDsc("containerPoint", "TATWntuPoint")->Object();
  if ( m_debug > 0 )		cout << "N point read: " << ntup->GetPointN() << endl;

  // save hits in the collection
  for (int i = 0; i < ntup->GetPointN(); i++) {
    //if ( ntup->GetPoint(i)->IsTrueGhost() )		continue;		// skip the ghost hits
    m_TW_hitCollection.push_back( ntup->GetPoint(i) );
  }

  return ntup->GetPointN();
}




//----------------------------------------------------------------------------------------------------
// pack together the hits to be fitted, from all the detectors, selct different preselecion m_systemsONs
int KFitter::PrepareData4Fit( Track* fitTrack ) {

  if ( m_debug > 0 )		cout << "\n\n*******\tKFitter::PrepareData4Fit\t*******\n" << endl;

  // Vertex -  fill fitter collections
  // Vertex -  fill fitter collections
  if ( (m_systemsON == "all" || m_systemsON.find( "VT" ) != string::npos) && GlobalPar::GetPar()->IncludeVertex() ) {
    UploadHitsVT();
    if ( m_debug > 0 )		cout << endl<<endl << "Filling vertex hit collection  = " << m_VT_hitCollection.size() << endl;
    Prepare4Vertex(fitTrack);
  }
  // Inner Tracker -  fill fitter collections
  if ( (m_systemsON == "all" || m_systemsON.find( "IT" ) != string::npos) && GlobalPar::GetPar()->IncludeInnerTracker() ) {
    UploadHitsIT();
    if ( m_debug > 0 )		cout <<endl<<endl << "Filling inner detector hit collection = " << m_IT_hitCollection.size() << endl;
    Prepare4InnerTracker(fitTrack);
  }
  // MSD -  fill fitter collections
  if ( (m_systemsON == "all" || m_systemsON.find( "MSD" ) != string::npos) && GlobalPar::GetPar()->IncludeMSD() ) {
    UploadHitsMSD();
    if ( m_debug > 0 )		cout << endl<<endl << "Filling Strip hit collection = " << m_MSD_hitCollection.size() << endl;
    Prepare4Strip(fitTrack);
  }
  // Inner Tracker -  fill fitter collections
  // if ( (m_systemsON == "all" || m_systemsON.find( "IT" ) != string::npos) && GlobalPar::GetPar()->IncludeInnerTracker() ) {
  if ( ( m_systemsON.find( "TW" ) != string::npos) && GlobalPar::GetPar()->IncludeTW() ) {
    UploadHitsTW();
    if ( m_debug > 0 )		cout <<endl<<endl << "Filling scintillator hit collection = " << m_TW_hitCollection.size() << endl;
    Prepare4TofWall(fitTrack);
  }

  // try to categorise the particle that generated the hit. If it fails --> clean the hit object
  CategoriseHitsToFit_withTrueInfo();

  // remove the category if does not satisfy the requirements  ///////////////////
  vector <int> hitsToBeRemoved;
  int hitsCount = 0;
  for ( map< string, vector<AbsMeasurement*> >::iterator it=m_hitCollectionToFit.begin(); it != m_hitCollectionToFit.end(); it++ ) {
    if ( !PrefitRequirements( it ) )	{	// to be exactely 1 hit per layer
      hitsToBeRemoved.push_back( hitsCount );
      // // if requirements are FALSE -> delete each AbsMeasurement objects
      // for ( vector<AbsMeasurement*>::iterator it2=(*it).second.begin(); it2 != (*it).second.end(); it2++ ) {
      // 	delete (*it2);
      // 	// delete (*it).second.at(i);	// wrong!
      // }
    }
    hitsCount++;
  }
  hitsCount = 0;
  for ( auto it = m_hitCollectionToFit.cbegin(), next_it = m_hitCollectionToFit.cbegin(); it != m_hitCollectionToFit.cend(); it = next_it)	{
    next_it = it; ++next_it;
    // check the pre-fit requirements
    if ( find( hitsToBeRemoved.begin(), hitsToBeRemoved.end(), hitsCount ) != hitsToBeRemoved.end() ) {
      // if requirements are FALSE -> clear the map element
      m_hitCollectionToFit.erase(it);
    }
    hitsCount++;
  }
  ///////////////////////////////////////////////////////////////////////////////

  //	if no map element survive -> clear the single-detector hit-collections
  if ( m_hitCollectionToFit.size() == 0 ) {
    m_VT_hitCollection.clear();
    m_IT_hitCollection.clear();
    m_MSD_hitCollection.clear();
    m_TW_hitCollection.clear();
    for ( vector<AbsMeasurement*>::iterator it2=m_allHitsInMeasurementFormat.begin(); it2 != m_allHitsInMeasurementFormat.end(); it2++ )
      delete (*it2);
    m_allHitsInMeasurementFormat.clear();
    return 0;
  }

  return m_hitCollectionToFit.size();
}







//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4Vertex( Track* fitTrack ) {

  TMatrixDSym hitCov(3);
  TVectorD hitCoords(3);
  for (unsigned int i = 0; i < m_VT_clusCollection.size(); i++) {
    TAVTcluster* p_hit = m_VT_clusCollection.at(i);
    
    // get pixel coord
    TVector3 hitPos = p_hit->GetPositionG();
    // get true MC coord
    // TVector3 hitPos = m_VT_hitCollection.at(i)->GetMCPosition_Global();
    
    // if ( m_debug > 0 )		cout << "VTX hit = Layer:" << p_hit->GetLayer() <<" col:"<< p_hit->GetPixelColumn() <<" row:"<< p_hit->GetPixelLine() <<
    // 								" \n\t\tGEN Type: " << p_hit->m_genPartFLUKAid <<
    //  								"  genID= " << p_hit->m_genPartID << endl;
    // // if ( m_debug > 0 )		cout << "Hit " << i;
    if ( m_debug > 0 )
      hitPos.Print();
    
    // set hit position vector
    hitCoords(0)=hitPos.x();
    hitCoords(1)=hitPos.y();
    hitCoords(2)=hitPos.z();
    // set covariance matrix
    // double pixReso = 0.001;
    // double pixReso = GlobalPar::GetPar()->VTReso();

    TVector3 pixReso = p_hit->GetPosError();
    hitCov.UnitMatrix();
    for (int j = 0; j < 3; j++){
      hitCov[j][j] = pixReso(i)*pixReso(i);
    }

    //hitCov *= pixReso*pixReso;
    // double zErr = 0.02;
    // double zErr = 0.005;
    // hitCov[2][2] = zErr*zErr;
    
    // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["VT"], i, nullptr );
    
    m_allHitsInMeasurementFormat.push_back(hit);
    
  }
}




//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4InnerTracker( Track* fitTrack ) {

  TMatrixDSym hitCov(3);
  TVectorD hitCoords(3);

  for (unsigned int i = 0; i < m_IT_clusCollection.size(); i++) {
    TAITcluster* p_hit = m_IT_clusCollection.at(i);
    // get pixel coord
    TVector3 hitPos = p_hit->GetPositionG();
    // get true MC coord
    // TVector3 hitPos = m_IT_hitCollection.at(i)->GetMCPosition_Global();
    //if ( m_debug > 0 )		cout << "ITR hit = Layer:" << p_hit->GetLayer() <<" col:"<< p_hit->GetPixelColumn() <<" row:"<< p_hit->GetPixelLine() <<
    // 							  " \n\t\tGEN Type: " << p_hit->m_genPartFLUKAid <<
    //   								"  genID= " << p_hit->m_genPartID << endl;
    // if ( m_debug > 0 )		cout << "Hit " << i;
    if ( m_debug > 0 )
      hitPos.Print();

    // set hit position vector
    hitCoords(0)=hitPos.x();
    hitCoords(1)=hitPos.y();
    hitCoords(2)=hitPos.z();
    // set covariance matrix
    // double pixReso = 0.001;

    TVector3 pixReso = p_hit->GetPosError();
    hitCov.UnitMatrix();
    for (int j = 0; j < 3; j++){
      hitCov[j][j] = pixReso(i)*pixReso(i);
    }

    // double pixReso = GlobalPar::GetPar()->ITReso();
    // hitCov.UnitMatrix();
    // hitCov *= pixReso*pixReso;
    // // double zErr = 0.01;	// >95%
    // double zErr = 0.02;	// >90%
    // // double zErr = 0.005; // >99%
    // // double zErr = 0.05;	// >86%
    // hitCov[2][2] = zErr*zErr;
    
    // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["IT"], i, nullptr );
    m_allHitsInMeasurementFormat.push_back(hit);
    
  }
}



//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4TofWall( Track* fitTrack ) {

  TMatrixDSym hitCov(3);
  TVectorD hitCoords(3);
  for (unsigned int i = 0; i < m_TW_hitCollection.size(); i++) {
    
    TATWpoint* p_hit = m_TW_hitCollection.at(i);
    
    // get pixel coord// should used TAGgeoTrafo
    TVector3 hitPos = p_hit->GetPosition();
    
    // if ( m_debug > 0 )		cout << "Hit " << i;
    if ( m_debug > 0 )		hitPos.Print();
    
    // set hit position vector
    hitCoords(0)=hitPos.x();
    hitCoords(1)=hitPos.y();
    hitCoords(2)=hitPos.z();
    // set covariance matrix
    // double pixReso = 0.001;
    double pixReso = GlobalPar::GetPar()->TWReso();
    
    hitCov.UnitMatrix();
    hitCov *= pixReso*pixReso;
    double zErr = 0.5;
    hitCov[2][2] = zErr*zErr;
    
    // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["TW"], i, nullptr );
    
    m_allHitsInMeasurementFormat.push_back(hit);
    
  }
}



//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4Strip( Track* fitTrack ) {

//	TMatrixDSym hitCov(3);
//	TVectorD hitCoords(3);
//
//	vector<TAMSDntuHit*> allStripSignals_x;
//	vector<TAMSDntuHit*> allStripSignals_y;
//
//	m_MSD_posVectorSmearedHit.clear();
//	m_MSD_momVectorSmearedHit.clear();
//	m_MSD_mass.clear();
//
//    for (unsigned int i = 0; i < m_MSD_hitCollection.size(); i++) {
//
//        TAMSDntuHit* p_hit = m_MSD_hitCollection.at(i);
//
//        // if ( m_debug > 0 )		cout << "Hit " << i << endl;
//        if ( m_debug > 0 )		cout << "MSD hit = Layer:" << p_hit->GetLayer() <<" view:"<< p_hit->GetPixelView() <<
//        								" strip:"<< p_hit->GetPixelStrip() <<
//        								" \n\t\tGEN Type: " << p_hit->m_genPartFLUKAid <<
//        								"  genID= " << p_hit->m_genPartID << endl;
//        // if ( m_debug > 0 )		hitPos.Print();
//
//
//        // set covariance matrix
//        // double stripReso = 0.001;
//		double stripReso = GlobalPar::GetPar()->MSDReso();
//		hitCov.UnitMatrix();
//		hitCov *= stripReso*stripReso;
//		double zErr = 0.01;
//		hitCov[2][2] = zErr*zErr;
//
////		double simulatedStripHit_X = p_hit->GetMCPosition_Global().X();
////		double simulatedStripHit_Y = p_hit->GetMCPosition_Global().Y();
////		double simulatedStripHit_Z = p_hit->GetMCPosition_Global().Z();
////		TVector3 gen_hitPos = TVector3 ( simulatedStripHit_X, simulatedStripHit_Y, simulatedStripHit_Z );
//		// if ( m_debug > 0 )		cout << "\tSimulated hits coordinate using smearing (nope!): \t\t ";
//		// if ( m_debug > 0 )		gen_hitPos.Print();
//		// if ( m_debug > 0 )		hitPos.Print();
//
//       TVector3 hitPos = m_MSD_geo->GetPixelPos_footFrame( p_hit->GetSensorID(), p_hit->GetPixelColumn(), p_hit->GetPixelLine() );
//
////        hitCoords(0)=gen_hitPos.x();
////		hitCoords(1)=gen_hitPos.y();
////		hitCoords(2)=gen_hitPos.z();
//		 hitCoords(0)=hitPos.x();
//		 hitCoords(1)=hitPos.y();
//		 hitCoords(2)=hitPos.z();
//
//
//
//		// MC info, provvisorio solo per il test!!!!!
//		//m_MSD_posVectorSmearedHit.push_back( gen_hitPos );
////		TVector3 hitMomMC( p_hit->GetMCMomentum_Global().X(), p_hit->GetMCMomentum_Global().Y(), p_hit->GetMCMomentum_Global().Z() );
////		m_MSD_momVectorSmearedHit.push_back( hitMomMC );
////		m_MSD_mass.push_back( p_hit->m_genPartMass );
//
//        // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
//    	AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["MSD"], i, nullptr );
//
//    	m_allHitsInMeasurementFormat.push_back(hit);
//
//
//        // if ( p_hit->GetPixelView() == 0 )        allStripSignals_x.push_back(p_hit);
//        // if ( p_hit->GetPixelView() == 1 )        allStripSignals_y.push_back(p_hit);
//
//    }
//
//	// if ( m_debug > 0 )		cout << "x=  " << allStripSignals_x.size() << "  y= " << allStripSignals_y.size() << endl;
//
//	// //***********************************************************************************************
//	// //		test senza smearing su x
//	// //***********************************************************************************************
//	// int countStripHits = 0;
// //    m_MSD_posVectorSmearedHit.clear();
// //    m_MSD_momVectorSmearedHit.clear();
// //    m_MSD_mass.clear();
// //    for ( vector<TAMSDntuHit*>::iterator xIt=allStripSignals_x.begin(); xIt != allStripSignals_x.end(); xIt++ ) {
//
//	//         // TVector3 hitPos = m_MSD_geo->GetPosition( (*xIt)->GetLayer(), (*xIt)->GetPixelView(), (*xIt)->GetPixelStrip() );
//
//	//         // set covariance matrix
//	// 		double stripReso = GlobalPar::GetPar()->MSDReso();
//	// 		hitCov.UnitMatrix();
//	// 		hitCov *= stripReso*stripReso;
//	// 		double zErr = 0.001;
//	// 		hitCov[2][2] = zErr*zErr;
//
//	// 		double simulatedStripHit_X = (*xIt)->GetMCPosition_Global().X();
//	// 		double simulatedStripHit_Y = (*xIt)->GetMCPosition_Global().Y();
//	// 		double simulatedStripHit_Z = (*xIt)->GetMCPosition_Global().Z();
//	// 		TVector3 gen_hitPos = TVector3 ( simulatedStripHit_X, simulatedStripHit_Y, simulatedStripHit_Z );
//	// 		if ( m_debug > 0 )		cout << "\tSimulated hits coordinate using smearing (nope!): \t\t ";
//	// 		if ( m_debug > 0 )		gen_hitPos.Print();
//	// 		// if ( m_debug > 0 )		hitPos.Print();
//
//	//         hitCoords(0)=gen_hitPos.x();
//	// 		hitCoords(1)=gen_hitPos.y();
//	// 		hitCoords(2)=gen_hitPos.z();
//	// 		// hitCoords(0)=hitPos.x();
//	// 		// hitCoords(1)=hitPos.y();
//	// 		// hitCoords(2)=hitPos.z();
//
//
//
//	// 		// MC info, provvisorio solo per il test!!!!!
//	// 		m_MSD_posVectorSmearedHit.push_back( gen_hitPos );
//	// 		TVector3 hitMomMC( (*xIt)->GetMCMomentum_Global().X(), (*xIt)->GetMCMomentum_Global().Y(), (*xIt)->GetMCMomentum_Global().Z() );
//	// 		m_MSD_momVectorSmearedHit.push_back( hitMomMC );
//	// 		m_MSD_mass.push_back( (*xIt)->m_genPartMass );
//
//	//         // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
// //        	AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["MSD"], countStripHits, nullptr );
//
// //        	m_allHitsInMeasurementFormat.push_back(hit);
//
//	//         // count the combined hits
//	// 		countStripHits++;
//
// //    }
//	//***********************************************************************************************
//	//***********************************************************************************************
//
//
////***********************************************************************************************
////		include smearing
////***********************************************************************************************
//
//   //  int countStripHits = 0;
//   //  m_MSD_posVectorSmearedHit.clear();
//   //  m_MSD_momVectorSmearedHit.clear();
//   //  m_MSD_mass.clear();
//   //  for ( vector<TAMSDntuHit*>::iterator xIt=allStripSignals_x.begin(); xIt != allStripSignals_x.end(); xIt++ ) {
//	  //   for ( vector<TAMSDntuHit*>::iterator yIt=allStripSignals_y.begin(); yIt != allStripSignals_y.end(); yIt++ ) {
//
//	  //   	if ( (*xIt)->GetLayer() != (*yIt)->GetLayer() )		continue;
//
//	  //   	// combine strip signals ONLY if coming from th same fragment
//	  //   	if ( (*xIt)->m_genPartID != (*yIt)->m_genPartID )		continue;
//
//
//	  //       TVector3 x_hitPos = m_MSD_geo->GetPosition( (*xIt)->GetLayer(), (*xIt)->GetPixelView(), (*xIt)->GetPixelStrip() );
//	  //       TVector3 y_hitPos = m_MSD_geo->GetPosition( (*yIt)->GetLayer(), (*yIt)->GetPixelView(), (*yIt)->GetPixelStrip() );
//
//	  //       // TVector3 geo_hitPos = TVector3 ( x_hitPos.x(), x_hitPos.y(), x_hitPos.z() );
//	  //       TVector3 geo_hitPos = TVector3 ( x_hitPos.y(), y_hitPos.y(), 0.5*(x_hitPos.z() + y_hitPos.z()) );
//
//	  //       if ( m_debug > 0 )		cout << "Hit strip " << countStripHits << endl;
//	  //       if ( m_debug > 0 )		cout << "\tMSD hit = Layer:" << (*xIt)->GetLayer() << " xStrip:" << (*xIt)->GetPixelStrip() <<" yStrip:"<<  (*yIt)->GetPixelStrip() << endl;
//
//	  //       if ( m_debug > 0 )		cout << "\t", geo_hitPos.Print();
//
//			// double stripReso = GlobalPar::GetPar()->MSDReso();
//			// hitCov.UnitMatrix();
//			// hitCov *= stripReso*stripReso;
//			// double zErr = 0.0025*2;
//			// hitCov[2][2] = zErr*zErr;
//
//			// double simulatedStripHit_X = (*xIt)->GetMCPosition_Global().X() + m_diceRoll->Gaus(0.,1.) * stripReso/4;
//			// double simulatedStripHit_Y = (*yIt)->GetMCPosition_Global().Y() + m_diceRoll->Gaus(0.,1.) * stripReso/4;
//			// double simulatedStripHit_Z = m_MSD_geo->GetLayerCenter( (*xIt)->GetLayer() ).Z();
//			// // double simulatedStripHit_Z = m_MSD_geo->GetLayerCenter( (*xIt)->GetLayer() ).Z() + m_diceRoll->Gaus(0.,1.) * zErr;
//
//			// TVector3 hitPos = TVector3 ( simulatedStripHit_X, simulatedStripHit_Y, simulatedStripHit_Z );
//			// if ( m_debug > 0 )		cout << "\tSimulated hits coosrdinate using smearing: \t\t ";
//			// if ( m_debug > 0 )		hitPos.Print();
//
//	  //       hitCoords(0)=hitPos.x();
//			// hitCoords(1)=hitPos.y();
//			// hitCoords(2)=hitPos.z();
//
//			// // MC info
//			// TVector3 hitPosMC( (*xIt)->GetMCPosition_Global().X(), (*yIt)->GetMCPosition_Global().Y(), m_MSD_geo->GetLayerCenter( (*xIt)->GetLayer() ).Z() );
//			// // TVector3 hitPosMC( (*xIt)->GetMCPosition_Global().X(), (*xIt)->GetMCPosition_Global().Y(), m_MSD_geo->GetLayerCenter( (*xIt)->GetLayer() ).Z() );
//			// m_MSD_posVectorSmearedHit.push_back( hitPosMC );
//			// TVector3 hitMomMC( (*xIt)->GetMCMomentum_Global().X(), (*yIt)->GetMCMomentum_Global().Y(), (*xIt)->GetMCMomentum_Global().Z() );
//			// m_MSD_momVectorSmearedHit.push_back( hitMomMC );
//			// m_MSD_mass.push_back( (*xIt)->m_genPartMass );
//
//	  //       // AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["MSD"], i, new TrackPoint(fitTrack));
//	  //       AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["MSD"], countStripHits, nullptr );
//
//	  //       // count the combined hits
//			// countStripHits++;
//
//	  //        // try to categorise the particle that generated the hit. If it fails --> clean the hit object
//	  //       string category = CategoriseHitsToFit_withTrueInfo( (*xIt)->m_genPartFLUKAid, (*xIt)->m_genPartCharge, (*xIt)->m_genPartMass );
//	  //       if (category == "fail")	{
//	  //       	delete hit;
//	  //       	continue;
//	  //       }
//	  //       // cout << category << "   " << p_hit->m_genPartMass << endl;
//	  //       m_hitCollectionToFit[ category ].push_back(hit);
//
//	  //   }
//   //  }
//    //***********************************************************************************************
//	//***********************************************************************************************
//
//
}


//----------------------------------------------------------------------------------------------------
// pre-fit requirements to be applied to EACH of the hitCollections
bool KFitter::PrefitRequirements( map< string, vector<AbsMeasurement*> >::iterator element ) {

	if ( m_debug > 0 )		cout << "KFitter::PrefitRequirements()  -  Category = " << (*element).first << endl;

	int testHitNumberLimit = 0;
	int testHit_VT = 0;
	int testHit_IT = 0;
	int testHit_MSD = 0;
	int testHit_TW = 0;
	// define the number of hits per each detector to consider to satisfy the pre-fit requirements
	if ( m_systemsON == "all" ) {
		testHit_VT = m_VT_geo->GetNLayers(), testHit_IT = m_IT_geo->GetNLayers(), testHit_MSD = m_MSD_geo->GetNLayers();
	}
	else {
		if ( m_systemsON.find( "VT" ) != string::npos )			testHit_VT = m_VT_geo->GetNLayers();
		if ( m_systemsON.find( "IT" ) != string::npos )			testHit_IT = m_IT_geo->GetNLayers();
		if ( m_systemsON.find( "MSD" ) != string::npos )		testHit_MSD = m_MSD_geo->GetNLayers();
		if ( m_systemsON.find( "TW" ) != string::npos )			testHit_TW = m_TW_geo->GetNLayers()/2;
	}
	// num of total hits
	testHitNumberLimit = testHit_VT + testHit_IT + testHit_MSD + testHit_TW;
	if ( testHitNumberLimit == 0 ) 			cout << "ERROR >> KFitter::PrefitRequirements :: m_systemsON mode is wrong!!!" << endl, exit(0);

	// // test the total number of hits ->  speed up the test
	// if ( (int)((*element).second.size()) != testHitNumberLimit ) {
	// 	if ( m_debug > 0 )		cout << "WARNING :: KFitter::PrefitRequirements  -->  number of elements different wrt the expected ones : Nel=" << (int)((*element).second.size()) << "   Nexp= " << testHitNumberLimit << endl;
	// 	return false;
	// }

 	int nHitVT = 0;
	int nHitIT = 0;
	int nHitMSD = 0;
	int nHitTW = 0;
	// count the hits per each detector
	for ( vector<AbsMeasurement*>::iterator it=(*element).second.begin(); it != (*element).second.end(); it++ ) {
		if ( (*it)->getDetId() == m_detectorID_map["VT"] )	nHitVT++;
		if ( (*it)->getDetId() == m_detectorID_map["IT"] )	nHitIT++;
		if ( (*it)->getDetId() == m_detectorID_map["MSD"] )	nHitMSD++;
		if ( (*it)->getDetId() == m_detectorID_map["TW"] )	nHitTW++;
		if ( m_debug > 0 )	cout << "nHitVT  " <<nHitVT<< " nHitIT" <<nHitIT<< " nHitMSD "<<nHitMSD<< " nHitTW "<<nHitTW<<endl;
	}
	// test the num of hits per each detector
	if ( nHitVT != testHit_VT || nHitIT != testHit_IT || nHitMSD != testHit_MSD ){
		if ( m_debug > 0 )
			cout << "WARNING :: KFitter::PrefitRequirements  -->  number of elements different wrt the expected ones : " <<
									"\n\t nVTX = " << nHitVT << "  Nexp = " << testHit_VT << endl <<
									"\n\t nITR = " << nHitIT << "  Nexp = " << testHit_IT << endl <<
									"\n\t nITR = " << nHitMSD << "  Nexp = " << testHit_MSD << endl <<
									"\n\t nMSD = " << nHitTW << "  Nexp = " << testHit_TW << endl;

		return false;
	}

	return true;
}







//----------------------------------------------------------------------------------------------------
// categorise the hit depending on the generating particle!
void KFitter::CategoriseHitsToFit_withTrueInfo() {

	if ( m_debug > 0 )		cout <<endl<< "KFitter::CategoriseHitsToFit_withTrueInfo()  -  allHitsInMeasurement size = " << m_allHitsInMeasurementFormat.size() << endl;

	int flukaID, partID, charge;
	double mass;
	for ( unsigned int i=0; i < m_allHitsInMeasurementFormat.size(); i++ ) {

		GetTrueParticleType( m_allHitsInMeasurementFormat.at(i), &flukaID, &partID, &charge, &mass );

		string outName = "fail";

		if ( flukaID == -2 && charge == 6 )  outName =  "C"  + build_string( round(mass/m_AMU) );
		if ( flukaID == -2 && charge == 3 )  outName =  "Li" + build_string( round(mass/m_AMU) );
		if ( flukaID == -2 && charge == 4 )  outName =  "B"  + build_string( round(mass/m_AMU) );
		if ( flukaID == -2 && charge == 5 )  outName =  "Be" + build_string( round(mass/m_AMU) );
		if ( flukaID == -2 && charge == 7 )  outName =  "N"  + build_string( round(mass/m_AMU) );
		if ( flukaID == -2 && charge == 8 )  outName =  "O"  + build_string( round(mass/m_AMU) );

		// if ( flukaID == -2 && charge == 6 &&  round(mass) == 10 )  outName =  "C10";
		// if ( flukaID == -2 && charge == 6 &&  round(mass) == 11 )  outName =  "C11";
		// if ( flukaID == -2 && charge == 6 &&  round(mass) == 12 )  outName =  "C12";

		// if ( flukaID == -2 && charge == 3 &&  round(mass) == 6 )  outName =  "Li6";
		// if ( flukaID == -2 && charge == 3 &&  round(mass) == 7 && mass < 7 )  outName =  "Li7";

		// if ( flukaID == -2 && charge == 4 &&  round(mass) == 7 )  outName =  "B7";
		// if ( flukaID == -2 && charge == 4 &&  round(mass) == 8 )  outName =  "B8";
		// if ( flukaID == -2 && charge == 4 &&  round(mass) == 9 )  outName =  "B9";

		// if ( flukaID == -2 && charge == 5 &&  round(mass) == 9 )  outName =  "Be9";
		// if ( flukaID == -2 && charge == 5 &&  round(mass) == 10 )  outName =  "Be10";

		// if ( flukaID == -2 && charge == 7 &&  round(mass) == 12 )  outName =  "N12";
		// // if ( flukaID == -2 && charge == 7 &&  round(mass) == 13 )  outName =  "N13";
		// if ( flukaID == -2 && charge == 7 &&  round(mass) == 14 )  outName =  "N14";

		// if ( flukaID == -2 && charge == 8 &&  round(mass) == 15 )  outName =  "O15";
		// if ( flukaID == -2 && charge == 8 &&  round(mass) == 16 )  outName =  "O16";

		if ( flukaID == -6 && charge == 2 )  outName =  "Alpha";
		if ( flukaID == 1 && charge == 1 )  outName =  "H";

		// if ( m_debug > 4 )		cout << "Selected Category: " << outName << "  flukaID=" << flukaID << "  partID="<<partID << "  charge="<<charge << "  mass="<<mass<<endl;


		// diventa Find_Category( outName )
		if ( !GlobalPar::GetPar()->Find_MCParticle( outName ) )
			continue;

		if ( m_debug > 2 )		cout << "\tSelected Category: " << outName << "  flukaID=" << flukaID << "  partID="<<partID << "  charge="<<charge << "  mass="<<mass<< endl;

		// if a category already defineed but with particle with a different partID  ->  make a new category with an incremental index
		int coll = 0;
		for ( map< string, vector<AbsMeasurement*> >::iterator it = m_hitCollectionToFit.begin(); it != m_hitCollectionToFit.end(); it++ ) {
			if ( (*it).first.find( outName ) != string::npos ) {	// enter if a category have part of the found outname

				int tmp_flukaID, tmp_partID, tmp_charge;
				double tmp_mass;
				GetTrueParticleType( (*it).second.at(0), &tmp_flukaID, &tmp_partID, &tmp_charge, &tmp_mass );

				if ( partID == tmp_partID ) {	// if from the same particle break and fill this category
					outName = (*it).first;
					coll = 0;
					break;
				}
				coll++;
			}
		}
		if ( coll > 0 )	// if its category not exist yet and found category from another particle of the same type  -->  change outName as C11-1, c11-2
			outName = outName +"-"+ build_string( coll );

		// fill the collection to be fitted in the proper category
		m_hitCollectionToFit[ outName ].push_back( m_allHitsInMeasurementFormat[i] );

	}
}




void KFitter::GetTrueParticleType( AbsMeasurement* hit, int* flukaID, int* partID, int* charge, double* mass ) {

	int detID = hit->getDetId();
	int hitID = hit->getHitId();

	if ( m_debug > 3 )		cout << "\t\tDetector Type = " << detID << "    HitID = " << hitID << endl;

	// Generated positions and momentums
//	if ( detID == m_detectorID_map["VT"] ) {
//		*flukaID = m_VT_hitCollection.at( hitID )->m_genPartFLUKAid;
//		*partID  = m_VT_hitCollection.at( hitID )->m_genPartID;
//		*charge  = m_VT_hitCollection.at( hitID )->m_genPartCharge;
//		*mass    = m_VT_hitCollection.at( hitID )->m_genPartMass;
//	}
//	else if ( detID == m_detectorID_map["IT"] ) {
//		*flukaID = m_IT_hitCollection.at( hitID )->m_genPartFLUKAid;
//		*partID  = m_IT_hitCollection.at( hitID )->m_genPartID;
//		*charge  = m_IT_hitCollection.at( hitID )->m_genPartCharge;
//		*mass    = m_IT_hitCollection.at( hitID )->m_genPartMass;
//	}
//	else if ( detID == m_detectorID_map["MSD"] ) {
//		if ( m_MSD_hitCollection.size() > hitID ) {
//			*flukaID = m_MSD_hitCollection.at( hitID )->m_genPartFLUKAid;
//			*partID  = m_MSD_hitCollection.at( hitID )->m_genPartID;
//			*charge  = m_MSD_hitCollection.at( hitID )->m_genPartCharge;
//			*mass    = m_MSD_hitCollection.at( hitID )->m_genPartMass;
//		}
//	}
//	else if ( detID == m_detectorID_map["TW"] ) {
//		TAMCntuEveHit* twGeneratorParticle = m_TW_hitCollection.at( hitID )->GetGenParticle();
//		*flukaID = twGeneratorParticle->FlukaID();
//		*partID  = m_TW_hitCollection.at( hitID )->GetGenPartID();
//		*charge  = twGeneratorParticle->Chg();
//		*mass    = twGeneratorParticle->Mass();
//		// *flukaID = m_TW_hitCollection.at( hitID )->m_genPartFLUKAid;
//		// *partID  = m_TW_hitCollection.at( hitID )->m_genPartID;
//		// *charge  = m_TW_hitCollection.at( hitID )->m_genPartCharge;
//		// *mass    = m_TW_hitCollection.at( hitID )->m_genPartMass;
//	}
}



//----------------------------------------------------------------------------------------------------
int KFitter::MakeFit( long evNum ) {

	if ( m_debug > 0 )		cout << "Starting MakeFit " << endl;
	int isConverged = 0;
	m_evNum = evNum;
	m_evNum_vect.push_back( evNum );

	// start values for the fit (seed), change below
	TVector3 pos(0, 0, 0);	// global coord   [cm]
	TVector3 mom(0, 0, 6);	// GeV
	Track*  fitTrack = new Track();  // container of the tracking objects

	// fill m_hitCollectionToFit
    PrepareData4Fit( fitTrack );
    // check the hit vector not empty otherwise clear
	if ( m_hitCollectionToFit.size() <= 0 )	{
		if ( m_debug > 0 )		cout << "No category to fit in this event..." << endl;
		// m_VT_hitCollection.clear();
		// m_IT_hitCollection.clear();
		// m_MSD_hitCollection.clear();
		// for ( vector<AbsMeasurement*>::iterator it2=m_allHitsInMeasurementFormat.begin(); it2 != m_allHitsInMeasurementFormat.end(); it2++ )
		// 	delete (*it2);
		delete fitTrack;
		return 2;
	}
	if ( m_debug > 0 )		cout << "\nMakeFit::m_hitCollectionToFit.size  =  " << m_hitCollectionToFit.size() << endl << endl;


	// loop over all hit category
	for ( map< string, vector<AbsMeasurement*> >::iterator hitSample=m_hitCollectionToFit.begin(); hitSample != m_hitCollectionToFit.end(); hitSample++ ) {

		if ( m_debug > 0 )	{
			// check if the category is defined in UpdatePDG  -->  also done in GetPdgCode()
			if ( !UpdatePDG::GetPDG()->IsParticleDefined( (*hitSample).first ) )
				cout << "ERROR :: KFitter::MakeFit  -->	 in UpdatePDG not found the category " << (*hitSample).first << endl, exit(0);
			cout << "\tCategory under fit  =  " << (*hitSample).first << " of size "<< (*hitSample).second.size() << endl;
		}

		// SET PARTICLE HYPOTHESIS  --> set repository
		AbsTrackRep* rep = new RKTrackRep( (UpdatePDG::GetPDG()->GetPdgCode( (*hitSample).first )) );
		fitTrack->addTrackRep( rep );

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//	si puo aggiungere piu di una repository e lui le prova tutte
		//  usa DEtermineCardinalRepresentation per discriminare la CardinalRep con minChi2. Se no ti prende la prima della fila
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		// SetTrueSeed( &pos, &mom );	// get seed from MC for debug
		// set seed
		fitTrack->setStateSeed(pos, mom);

		// insert points to be fitted  -   loop over each measurement in the current collection
		for ( unsigned int i=0; i < (*hitSample).second.size(); i++ )	{
			fitTrack->insertMeasurement( (*hitSample).second.at(i) );
			fitTrack->checkConsistency();
			if ( m_debug > 3 )		fitTrack->Print("C");
		}

		if ( m_reverse )		fitTrack->reverseTrackPoints();

		//check
		fitTrack->checkConsistency();
		if ( m_debug > 3 )		fitTrack->Print();

		//pre-fit
		MakePrefit();

		// THE REAL FIT 	-->		different kalman modes
		try{
			if ( GlobalPar::GetPar()->KalMode() == 1 )
				m_fitter->processTrack(fitTrack);
			else if ( GlobalPar::GetPar()->KalMode() == 2 )
				m_refFitter->processTrack(fitTrack);
			else if ( GlobalPar::GetPar()->KalMode() == 3 )
				m_dafRefFitter->processTrack(fitTrack);
			else if ( GlobalPar::GetPar()->KalMode() == 4 )
				m_dafSimpleFitter->processTrack(fitTrack);

			if ( m_debug > 3 )		fitTrack->Print();
			if ( m_debug > 0 )		cout << "Fitted " << fitTrack->getFitStatus(rep)->isFitted() << endl;
			if ( fitTrack->getFitStatus(rep)->isFitConverged() &&  fitTrack->getFitStatus(rep)->isFitted() )	isConverged = 1;	// convergence check
			if ( m_debug > 3 )		fitTrack->Print("C");

			// map of the tracked particles for each category
			if ( m_nTotTracks.find( (*hitSample).first ) == m_nTotTracks.end() )	m_nTotTracks[ (*hitSample).first ] = 0;
			m_nTotTracks[ (*hitSample).first ]++;

			// map of the CONVERGED tracks for each category
			if (isConverged) {
				if ( m_nConvergedTracks.find( (*hitSample).first ) == m_nConvergedTracks.end() )	m_nConvergedTracks[ (*hitSample).first ] = 0;
				m_nConvergedTracks[ (*hitSample).first ]++;
				RecordTrackInfo( fitTrack, (*hitSample).first );
			}
		}
		catch (genfit::Exception& e){
          std::cerr << e.what();
          std::cerr << "Exception, next track" << std::endl;
          continue;
		}
		// fill a vector with the categories fitted at least onece
		if ( find( m_categoryFitted.begin(), m_categoryFitted.end(), (*hitSample).first ) == m_categoryFitted.end() )
			m_categoryFitted.push_back( (*hitSample).first );

	}	// end  - loop over all hit category


	m_VT_hitCollection.clear();
	m_IT_hitCollection.clear();
	m_MSD_hitCollection.clear();
	m_TW_hitCollection.clear();
	// // clean all hits
	m_allHitsInMeasurementFormat.clear();
	delete fitTrack;	// include un delete rep pare
	// clean m_hitCollectionToFit
	for ( auto it = m_hitCollectionToFit.cbegin(), next_it = m_hitCollectionToFit.cbegin(); it != m_hitCollectionToFit.cend(); it = next_it)	{
		next_it = it; ++next_it;
		m_hitCollectionToFit.erase(it);
	}

	m_hitCollectionToFit.clear();

	if ( m_debug > 0 )		cout << "Ready for the next track fit!" << endl;
	// if ( m_debug > 0 )		cin.get();

	return isConverged;
}







void KFitter::SetTrueSeed( TVector3* pos, TVector3* mom ) {

	// SET SEED  --  debug only
		// int firstHitToProcess = -666;
		// if ( !m_reverse ) 		firstHitToProcess = 0;
		// else 					firstHitToProcess = (*hitSample).second.size()-1;

		// int detID = (*hitSample).second.at( firstHitToProcess )->getDetId();
		// int hitID = (*hitSample).second.at( firstHitToProcess )->getHitId();
		// if ( detID == m_detectorID_map["VT"] ) {
		// 	pos = m_VT_hitCollection.at( hitID )->GetMCPosition_Global();
		// 	mom = m_VT_hitCollection.at( hitID )->GetMCMomentum_Global();
		// }
		// else if ( detID == m_detectorID_map["IT"] ) {
		// 	pos = m_IT_hitCollection.at( hitID )->GetMCPosition_Global();
		// 	mom = m_IT_hitCollection.at( hitID )->GetMCMomentum_Global();
		// }
		// else if ( detID == m_detectorID_map["MSD"] ) {
		// 	pos = m_MSD_posVectorSmearedHit.at( hitID );
		// 	mom = m_MSD_momVectorSmearedHit.at( hitID );
		// }

		// if ( m_debug > 2 )		cout << "pos mom "<< endl, pos.Print(), mom.Print();

}




void KFitter::MakePrefit() {
// try{
	 //        if ( m_debug > 0 ) 		cout<<"Starting the fitter"<<endl;

	 //        // if (prefit) {
	 //          genfit::KalmanFitter prefitter(1, dPVal);
	 //          prefitter.setMultipleMeasurementHandling(genfit::weightedClosestToPrediction);
	 //          prefitter.processTrackWithRep(fitTrack, fitTrack->getCardinalRep());
	 //        // }

	 //        fitter->processTrack(fitTrack, false);

	 //        if ( m_debug > 0 ) cout<<"fitter is finished!"<<endl;
	 //      }
	 //      catch(genfit::Exception& e){
	 //        cout << e.what();
	 //        cout << "Exception, next track" << endl;
	 //        continue;
	 //    }
}


//----------------------------------------------------------------------------------------------------
// init matrix to zero
void KFitter::MatrixToZero( TMatrixD *matrix ) {
	for ( int j=0; j<matrix->GetNrows(); j++ ) {
		for ( int k=0; k<matrix->GetNcols(); k++ ) {
			(*matrix)(j,k) = 0;
		}
	}
}






//----------------------------------------------------------------------------------------------------
//
void KFitter::RecordTrackInfo( Track* track, string hitSampleName ) {

	TVector3 expectedPos;
	TVector3 expectedMom;
	TVector3 kalmanMom, kalmanPos;
	TVector3 kalmanMom_err;
	double massMC = -666;
	// double myChi2 = 0;

	// InitAllHistos( hitSampleName );

	TMatrixD KalmanPos_cov(3,3);
	TMatrixD KalmanMom_cov(3,3);

	// loop over hits
	for ( unsigned int i =0; i<m_hitCollectionToFit[ hitSampleName ].size(); i++ ) {	//
	// for ( unsigned int i =0; i<track.getNpoint(); i++ ) {

		if ( m_debug > 0 )		cout << "Start hit num: " << i << endl;
		int x = i;	// hit collection index, same in case of forward and reverse
		if ( m_reverse ) 	x = m_hitCollectionToFit[ hitSampleName ].size() - 1 - i;	// index last to first

		// take kinematic variables to be plotted
		TVector3 tmpPos, tmpMom;
		double tmp_mass = -666;
		TVector3 tmp_genPos;
		TVector3 tmp_genMom;
		TVector3 hitPos;

		GetTrueMCInfo( hitSampleName, x,
						&tmpPos, &tmpMom, &tmp_mass,
						&tmp_genPos, &tmp_genMom, &hitPos );

		TVector3 KalmanPos;
		TVector3 KalmanMom;
		TVector3 KalmanPos_err;
		TVector3 KalmanMom_err;
		double KalmanMass = 0;

		GetKalmanTrackInfo ( hitSampleName, i, track,
								&KalmanPos, &KalmanMom, &KalmanPos_err, &KalmanMom_err,
								&KalmanPos_cov, &KalmanMom_cov,
								&KalmanMass );
		/////////////////////////////////////////////////

		if ( m_debug > 0 )	{
			cout <<endl<< "Single Event Debug\t--\t" << hitSampleName << endl;
			cout << "Hit num = " << i << "  --  MC mass = " << tmp_mass << endl;
			cout << "\t TruePos =       " << tmpPos.Mag() << "  ~=  measuredPos  " << hitPos.Mag() << endl;
			// cout << "\t TruePos =       "; tmpPos.Print();
			cout << "\t Kalman Pos da State6D = " << KalmanPos.Mag() << "  = Pos " << track->getFittedState(i).getPos().Mag() << endl;
			// cout << "\t Kalman Pos da State6D = "; KalmanPos.Print();
			// cout << "\t Kalman Pos Error da State6D = "; KalmanPos_err.Print();
			cout <<endl<< "\t Gen_Mom = "<< tmp_genMom.Mag() << endl;
			cout <<endl<< "\t MC_Mom = "<< tmpMom.Mag() <<"     " <<endl;
			// tmpMom.Print();
			cout << "\t Kalman Mom da State6D = "<< KalmanMom.Mag() << "  = Mom " << track->getFittedState(i).getMom().Mag() << endl;
			// cout << "\t Kalman Mom da State6D = "<< KalmanMom.Mag() <<"    "; KalmanMom.Print();
			// cout << "\t Kalman Mom Error da State6D = "; KalmanMom_err.Print();
		}

		//! Get the accumulated X/X0 (path / radiation length) of the material crossed in the last extrapolation.
		// virtual double getRadiationLenght() const = 0;

		ControlPlotsRepository::GetControlObject( m_kalmanOutputDir )->SetControlMom_4eachState( hitSampleName, i, &KalmanMom, &tmpMom, &tmp_genMom );
		ControlPlotsRepository::GetControlObject( m_kalmanOutputDir )->SetControlPos_4eachState( hitSampleName, i, &KalmanPos, &tmpPos, &hitPos );

		// keep quantities to be plotted of the state CLOSER to the interaction point
		unsigned int measuredState = ( m_reverse ? m_hitCollectionToFit[ hitSampleName ].size()-1 : 0 );
		// if ( i == m_hitCollectionToFit[ hitSampleName ].size() -1 ) {
		if ( i == measuredState ) {
			expectedPos = tmpPos;
			expectedMom = tmpMom;
			kalmanMom = KalmanMom;
			kalmanMom_err = KalmanMom_err;
			kalmanPos = KalmanPos;
			massMC = tmp_mass;

			// tuuta la info andra esportata per essere messa in ntupla x l'analisi....
			m_fitTrackCollection->AddTrack( hitSampleName, track, m_evNum, i, // trackID?
															&KalmanMom, &KalmanPos,
															&expectedMom, &expectedPos,
															&KalmanMom_cov );

			m_controlPlotter->SetMom_Gen( hitSampleName, &tmp_genMom );
			m_controlPlotter->SetMom_TrueMC( hitSampleName, &expectedMom, massMC );
			m_controlPlotter->SetMom_Kal( hitSampleName, &kalmanMom, &kalmanMom_err );

			m_controlPlotter->SetPos_Kal( hitSampleName, &kalmanPos, &KalmanPos_err );

			m_controlPlotter->SetTrackInfo( hitSampleName, track );


			if ( GlobalPar::GetPar()->IsPrintOutputNtuple() )
				m_controlPlotter->Set_Outputntuple(&kalmanMom, &kalmanPos, &tmp_genMom);

		}
	}

}




//----------------------------------------------------------------------------------------------------
void KFitter::GetTrueMCInfo( string hitSampleName, int x,
						TVector3* tmpPos, TVector3* tmpMom, double* tmp_mass,
						TVector3* tmp_genPos,  TVector3* tmp_genMom, TVector3* hitPos ) {

//	int detID = m_hitCollectionToFit[ hitSampleName ].at(x)->getDetId();
//	int hitID = m_hitCollectionToFit[ hitSampleName ].at(x)->getHitId();
//
//	// Generated positions and momentums
//	if ( detID == m_detectorID_map["VT"] ) {
//		*tmpPos = m_VT_hitCollection.at( hitID )->GetMCPosition_footFrame();
//		*tmpMom = m_VT_hitCollection.at( hitID )->GetMCMomentum_footFrame();
//		// information on the particle that genearated the hit
//		*tmp_mass = m_VT_hitCollection.at( hitID )->m_genPartMass;
//		*tmp_genPos = m_VT_hitCollection.at( hitID )->m_genPartPosition;   // genaration position
//		*tmp_genMom = m_VT_hitCollection.at( hitID )->m_genPartMomentum;		// genaration momentum
//		// TAVTntuHit* p_hit = m_VT_hitCollection.at(hitID);
//        *hitPos = m_VT_hitCollection.at(hitID)->GetPixelPosition_footFrame(); // pixel coord
//
//	}
//	else if ( detID == m_detectorID_map["IT"] ) {
//		*tmpPos = m_IT_hitCollection.at( hitID )->GetMCPosition_footFrame();
//		*tmpMom = m_IT_hitCollection.at( hitID )->GetMCMomentum_footFrame();
//		// information on the particle that genearated the hit
//		*tmp_mass = m_IT_hitCollection.at( hitID )->m_genPartMass;
//		*tmp_genPos = m_IT_hitCollection.at( hitID )->m_genPartPosition;	// genaration position
//		*tmp_genMom = m_IT_hitCollection.at( hitID )->m_genPartMomentum;	// genaration momentum
//		// TAITntuHit* p_hit = m_IT_hitCollection.at(hitID);
//        *hitPos = m_IT_hitCollection.at(hitID)->GetPixelPosition_footFrame(); // pixel coord
//	}
//	else if ( detID == m_detectorID_map["MSD"] ) {
//		*tmpPos = m_MSD_posVectorSmearedHit.at( hitID );
//		*tmpMom =  m_MSD_momVectorSmearedHit.at( hitID );
//		*tmp_mass = m_MSD_mass.at( hitID );
//		*tmp_genPos = TVector3(-1, -1, -1);
//		*tmp_genMom = TVector3(-1, -1, -1);;
//		hitPos = tmpPos;
//	}
//	else if ( detID == m_detectorID_map["TW"] ) {
//		*tmpPos = m_TW_hitCollection.at( hitID )->GetMCPosition_footFrame();
//		*tmpMom = m_TW_hitCollection.at( hitID )->GetMCMomentum_footFrame();
//		// // information on the particle that genearated the hit
//		// *tmp_mass = m_TW_hitCollection.at( hitID )->m_genPartMass;
//		// *tmp_genPos = m_TW_hitCollection.at( hitID )->m_genPartPosition;   // genaration position
//		// *tmp_genMom = m_TW_hitCollection.at( hitID )->m_genPartMomentum;		// genaration momentum
//        *hitPos = m_TW_hitCollection.at(hitID)->GetPosition_footFrame(); // pixel coord
//
//		TAMCntuEveHit* twGeneratorParticle = m_TW_hitCollection.at( hitID )->GetGenParticle();
//		*tmp_genPos  = twGeneratorParticle->InitPos();
//		*tmp_genMom  = twGeneratorParticle->InitP();
//		*tmp_mass    = twGeneratorParticle->Mass();
//
//	}
}





//----------------------------------------------------------------------------------------------------
void KFitter::GetKalmanTrackInfo ( string hitSampleName, int i, Track* track,
								TVector3* KalmanPos, TVector3* KalmanMom, TVector3* KalmanPos_err, TVector3* KalmanMom_err,
								TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov,
								double* KalmanMass ) {


	// Get reco track kinematics and errors
	*KalmanPos = TVector3( (track->getFittedState(i).get6DState())[0],	(track->getFittedState(i).get6DState())[1],	(track->getFittedState(i).get6DState())[2] );
	*KalmanMom = TVector3( (track->getFittedState(i).get6DState())[3], (track->getFittedState(i).get6DState())[4],	(track->getFittedState(i).get6DState())[5] );
	*KalmanPos_err = TVector3( (track->getFittedState(i).get6DCov())[0][0], (track->getFittedState(i).get6DCov())[1][1], (track->getFittedState(i).get6DCov())[2][2] );
	*KalmanMom_err = TVector3( (track->getFittedState(i).get6DCov())[3][3],	(track->getFittedState(i).get6DCov())[4][4], (track->getFittedState(i).get6DCov())[5][5] );

	// AbsMeasurement* measurement = track->getPointWithMeasurement(i)->getRawMeasurement(0);		// return the given coord -> aka the pixel coord

	// prduce the covariance matrix on the measured state
	MatrixToZero(KalmanPos_cov);
	MatrixToZero(KalmanMom_cov);
	for ( int j=0; j<3; j++ ) {
		for ( int k=0; k<3; k++ ) {
			(*KalmanMom_cov)(j,k) = (track->getFittedState(i).get6DCov())[j+3][k+3];
			(*KalmanPos_cov)(j,k) = (track->getFittedState(i).get6DCov())[j][k];
		}
	}

}






//----------------------------------------------------------------------------------------------------
void KFitter::PrintEfficiency() {

	TCanvas* mirror = new TCanvas("TrackEfficiencyPlot", "TrackEfficiencyPlot", 700, 700);
	mirror->SetRightMargin(0.05);
    mirror->SetLeftMargin(0.13);

	int nCollection = m_nTotTracks.size();
	TH1F* h_trackEfficiency = new TH1F( "TrackEfficiency", "TrackEfficiency", nCollection, 0, nCollection );

	int k = 0;
	for ( map<string, int>::iterator itTotNum = m_nTotTracks.begin(); itTotNum != m_nTotTracks.end(); itTotNum++ ) {
		k++;
		float eff = (float)m_nConvergedTracks[ (*itTotNum).first ] / (*itTotNum).second;
		if ( m_debug > -1 )		cout << "Efficiency " << (*itTotNum).first << " = " << eff << "  " << m_nConvergedTracks[ (*itTotNum).first ]<< " " << (*itTotNum).second << endl;
		h_trackEfficiency->SetBinContent(k, eff);
		h_trackEfficiency->GetXaxis()->SetBinLabel(k, (*itTotNum).first.c_str() );
	}
	h_trackEfficiency->SetTitle(0);
	h_trackEfficiency->SetStats(0);
	h_trackEfficiency->GetYaxis()->SetTitle("KF Efficiency");
	h_trackEfficiency->GetYaxis()->SetTitleOffset(1.1);
	h_trackEfficiency->SetLineWidth(2); // take short ~ int
	h_trackEfficiency->Draw();
	mirror->SaveAs( (m_kalmanOutputDir+"/"+"TrackEfficiencyPlot.png").c_str() );
	mirror->SaveAs( (m_kalmanOutputDir+"/"+"TrackEfficiencyPlot.root").c_str() );

}




//----------------------------------------------------------------------------------------------------
//  evaluate uncertainty from the diagonal of the covariance matrix ONLY. No correlation is considered!!!
double KFitter::EvalError( TVector3 mom, TVector3 err ) {

	double dx2 = 2 * sqrt(err.x()) * mom.x();
	double dy2 = 2 * sqrt(err.y()) * mom.y();
	double dz2 = 2 * sqrt(err.z()) * mom.z();

	double p = mom.Mag();
	double dp2 = dx2 + dy2 + dz2;
	double dp = dp2 / ( 2 * p );

	return dp;
}



//----------------------------------------------------------------------------------------------------
//  measure the Kalman uncertainty INCLUDING the cross terms in the covariance matrix. CORRELATION considered!!!
double KFitter::EvalError( TVector3 mom, TMatrixD cov ) {

	if ( cov.GetNcols() != 3 || cov.GetNrows() != 3 )
		cout << "ERROR :: KFitter::EvalError  >>  covariance dimension (should be 6) is wrong " << cov.GetNcols() << " x " << cov.GetNrows() << endl, exit(0);

	array<double,3> partialDer = { 	( mom.x()/( sqrt(mom.Mag()) ) ),
									( mom.y()/( sqrt(mom.Mag()) ) ),
									( mom.z()/( sqrt(mom.Mag()) ) )   };


	double err = 0;
	for ( int j=0; j<cov.GetNrows(); j++ ) {
		for ( int k=0; k<cov.GetNcols(); k++ ) {

			err += partialDer[j] * partialDer[k] * cov(j,k); // * diagFactor;

		}
	}

	double dp = sqrt(err);

	return dp;
}



//----------------------------------------------------------------------------------------------------
// Called from outside!
void KFitter::Finalize() {

	// make a directory for each hit category that forms a track candidate
	struct stat info;
	for ( unsigned int i=0; i < m_categoryFitted.size(); i++ ) {
		string pathName = m_kalmanOutputDir+"/"+m_categoryFitted.at(i);
		if( stat( pathName.c_str(), &info ) != 0 )		//cannot access
		    system(("mkdir "+pathName).c_str());
	}

	PrintEfficiency();

	m_fitTrackCollection->EvaluateMomentumResolution();

	m_categoryFitted.clear();

}




//----------------------------------------------------------------------------------------------------
void KFitter::InitEventDisplay() {


	// needed for the event display
	// //const genfit::eFitterType fitterId = genfit::SimpleKalman;
 //    const genfit::eFitterType fitterId = genfit::RefKalman;
 //    //const genfit::eFitterType fitterId = genfit::DafRef;
 //    //const genfit::eFitterType fitterId = genfit::DafSimple;

    // const genfit::eMultipleMeasurementHandling mmHandling = genfit::weightedAverage;	//	suggested
    // const genfit::eMultipleMeasurementHandling mmHandling = genfit::unweightedClosestToReference;
    //const genfit::eMultipleMeasurementHandling mmHandling = genfit::unweightedClosestToPrediction;
    //const genfit::eMultipleMeasurementHandling mmHandling = genfit::weightedClosestToReference;
    //const genfit::eMultipleMeasurementHandling mmHandling = genfit::weightedClosestToPrediction;
    // const genfit::eMultipleMeasurementHandling mmHandling = genfit::unweightedClosestToReferenceWire;
    // const genfit::eMultipleMeasurementHandling mmHandling = genfit::unweightedClosestToPredictionWire; //
    //const genfit::eMultipleMeasurementHandling mmHandling = genfit::weightedClosestToReferenceWire;
    //const genfit::eMultipleMeasurementHandling mmHandling = genfit::weightedClosestToPredictionWire;


	// m_fitter->setMultipleMeasurementHandling(mmHandling);
	// f_fitter->setMultipleMeasurementHandling(unweightedClosestToPredictionWire);


}




////////////////////     remember for the future!   ///////////////////////////////////////

// clear & delete objects  -  measueremnt delete made by Track class

	// no perche' gestito come puntatore esterno
	// for ( vector<TAVTntuHit*>::iterator it=m_VT_hitCollection.begin(); it != m_VT_hitCollection.end(); it++ ) {
	// 	delete (*it);
	// }
	// for ( vector<TAITntuHit*>::iterator it=m_IT_hitCollection.begin(); it != m_IT_hitCollection.end(); it++ ) {
	// 	delete (*it);
	// }
