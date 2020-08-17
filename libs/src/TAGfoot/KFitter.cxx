

#include "KFitter.hxx"





//----------------------------------------------------------------------------------------------------
KFitter::KFitter () {

  int nIter = 20; // max number of iterations
  double dPVal = 1.E-3; // convergence criterion
  m_AMU = 0.9310986964; // in GeV // conversion betweem mass in GeV and atomic mass unit

  m_debug = GlobalPar::GetPar()->Debug();

  m_recolike1 = true;

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

  m_MCInfo.clear();

  m_fitTrackCollection = new GlobalTrackRepostory();  // contains the fitted track info

  // Create dir for kalman output
  struct stat info;
  m_kalmanOutputDir = (string)getenv("FOOTRES")+"/Kalman";
  if( stat( m_kalmanOutputDir.c_str(), &info ) != 0 )		//cannot access
    system(("mkdir "+m_kalmanOutputDir).c_str());

  // class for control plot dumping
  ControlPlotsRepository::Instance();
  m_controlPlotter = ControlPlotsRepository::GetControlObject( m_kalmanOutputDir );

  // checks for the detector to be used for kalman
  IncludeDetectors();

  // take the geometry object
  if (GlobalPar::GetPar()->IncludeTG())
    m_TG_geo = shared_ptr<TAGparGeo> ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() );

  if ( (m_systemsON == "all" || m_systemsON.find( "VT" ) != string::npos) && GlobalPar::GetPar()->IncludeVT() )
    m_VT_geo = shared_ptr<TAVTparGeo> ( (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() );

  if ( (m_systemsON == "all" || m_systemsON.find( "IT" ) != string::npos) && GlobalPar::GetPar()->IncludeIT() )
    m_IT_geo = shared_ptr<TAITparGeo> ( (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object() );

  if ( (m_systemsON == "all" || m_systemsON.find( "MSD" ) != string::npos) && GlobalPar::GetPar()->IncludeMSD() )
    m_MSD_geo = shared_ptr<TAMSDparGeo> ( (TAMSDparGeo*) gTAGroot->FindParaDsc("msdGeo", "TAMSDparGeo")->Object() );

  if ( ( m_systemsON.find( "TW" ) != string::npos) && GlobalPar::GetPar()->IncludeTW() )
    m_TW_geo = shared_ptr<TATWparGeo> ( (TATWparGeo*) gTAGroot->FindParaDsc("twGeo", "TATWparGeo")->Object() );


  m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());


  //set the stage for TGeoManagerInterface class of GenFit
  // this is the World volume
  TGeoMedium* med = gGeoManager->GetMedium("AIR");
  m_TopVolume = gGeoManager->MakeBox("World",med, 220., 30., 30.);
  m_TopVolume->SetInvisible();
  gGeoManager->SetTopVolume(m_TopVolume);

  // // ST
  // if (GlobalPar::GetPar()->IncludeST()) {
  //   TASTparGeo* parGeo = fReco->GetParGeoSt();
  //   TGeoVolume* irVol  = parGeo->BuildStartCounter();

  //   TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TASTparGeo::GetBaseName());
  // }

  // // BM
  // if (GlobalPar::GetPar()->IncludeBM()) {
  //   TABMparGeo* parGeo = fReco->GetParGeoBm();;
  //   TGeoVolume* bmVol  = parGeo->BuildBeamMonitor();

  //   TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TABMparGeo::GetBaseName());
  // }

  // target
  if (GlobalPar::GetPar()->IncludeTG()) {
    TGeoVolume* tgVol = m_TG_geo->BuildTarget();
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAGparGeo::GetBaseName());
    m_TopVolume->AddNode(tgVol, 1, transfo);
  }

  // Vertex
  if (GlobalPar::GetPar()->IncludeVT()) {
    TGeoVolume* vtVol  = m_VT_geo->BuildVertex();
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAVTparGeo::GetBaseName());
    m_TopVolume->AddNode(vtVol, 2, transfo);
  }

  // // Magnet
  // if (GlobalPar::GetPar()->IncludeDI()) {
  //   TGeoVolume* vtVol = fDipole->BuildMagnet();
  //   TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TADIparGeo::GetBaseName());
  // }

  // IT
  if (GlobalPar::GetPar()->IncludeIT()) {
    TGeoVolume* itVol  = m_IT_geo->BuildInnerTracker();
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAITparGeo::GetItBaseName());
    m_TopVolume->AddNode(itVol, 3, transfo);

  }

  // MSD
  if (GlobalPar::GetPar()->IncludeMSD()) {
    TGeoVolume* msdVol = m_MSD_geo->BuildMultiStripDetector();
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAMSDparGeo::GetBaseName());
    m_TopVolume->AddNode(msdVol, 4, transfo);

  }

  // TW
  if (GlobalPar::GetPar()->IncludeTW()) {
    TGeoVolume* twVol = m_TW_geo->BuildTofWall();
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TATWparGeo::GetBaseName());
    m_TopVolume->AddNode(twVol, 5, transfo);
  }

  // // CA
  // if (GlobalPar::GetPar()->IncludeCA()) {
  //   TACAparGeo* parGeo = fReco->GetParGeoCa();
  //   TGeoVolume* caVol = parGeo->BuildCalorimeter();

  //   TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TACAparGeo::GetBaseName());
  // }




  // initialise the kalman method selected from param file
  if ( GlobalPar::GetPar()->KalMode() == 1 )
    m_fitter = new KalmanFitter(nIter, dPVal);
  else if ( GlobalPar::GetPar()->KalMode() == 2 )
    m_refFitter = new KalmanFitterRefTrack(nIter, dPVal);
  else if ( GlobalPar::GetPar()->KalMode() == 3 )
    m_dafRefFitter = new DAF(true, nIter, dPVal);
  else if ( GlobalPar::GetPar()->KalMode() == 4 )
    m_dafSimpleFitter = new DAF(false, nIter, dPVal);

  // InitEventDisplay();

  //---------------------------------------------------------------
  //---------------------------------------------------------------
  // //DUMP GEOMETRY
  //
  // //--- close the geometry
  //  gGeoManager->CloseGeometry();
  //
  //  //--- draw the ROOT box
  //  gGeoManager->SetVisLevel(10);
  //  m_TopVolume->Draw("ogl");
  //  TFile *outfile = TFile::Open("genfitFOOTGeom.root","RECREATE");
  //  gGeoManager->Write();
  //  outfile->Close();
  //

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


// //-------------------------------------------------------------------------------------------------
// int KFitter::UploadClusVT_track(){

	

// }


//-------------------------------------------------------------------------------------------------
int KFitter::UploadClusVT(){


  //map for m_MCInfo
  map<int, MCTruthInfo> MCVTInfo;


  //cluster test
  //TAVTparGeo* vtxGeo = (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object();
  TAVTntuCluster* vtclus = (TAVTntuCluster*) gTAGroot->FindDataDsc("vtClus","TAVTntuCluster")->Object();

  TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuEve")->Object();
  TAMCntuHit* vtMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("vtMc", "TAMCntuHit")->Object();

  int totClus = 0;
  Int_t nPlanes = m_VT_geo->GetSensorsN();

  for(Int_t iPlane = 0; iPlane < nPlanes; iPlane++){
    Int_t nclus = vtclus->GetClustersN(iPlane);
    if (m_debug > 1)
      std::cout << "\nfound " << nclus << " in plane " << iPlane << std::endl;
    totClus += nclus;
    if (nclus == 0) continue;

    for(Int_t iClus = 0; iClus < nclus; ++iClus){
      if (m_debug > 1)
	std::cout << "entered cycle clusVT of plane " << iPlane << std::endl;
      TAVTcluster* clus = vtclus->GetCluster(iPlane, iClus);
      if (!clus->IsValid()) continue;

      //TVector3 prova = clus->GetPositionG();
      TVector3 prova = m_GeoTrafo->FromVTLocalToGlobal( clus->GetPositionG() );

      if (m_debug > 1)
	prova.Print();

      m_VT_clusCollection.push_back(clus);


      //---------------------------testing access to hits and pixels from cluster--------------------

      //other way using new method in TAVTbaseCluster
      for (Int_t k = 0; k < clus->GetMcTracksN(); ++k) {

      }




      Int_t nHits = clus->GetPixelsN();
      // cout << "nHits: " << nHits  << endl;


      for (Int_t jHit = 0; jHit < nHits; ++jHit) {
        TAVTntuHit* hit = clus->GetPixel(jHit);
        if (m_debug > 1 ) cout<< "hit->GetMcTracksN() " << hit->GetMcTracksN() << endl;
        for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
          Int_t idx = hit->GetMcTrackIdx(k);
          Int_t id = hit->GetMcIndex(k);
          //cout << "McTrackId: " << idx << endl;
          //cout << "McIndex: " << id << endl;
          TAMChit* mcHit = vtMc->GetHit(id);
          Int_t checkTrackId = mcHit->GetTrackId();
          //if ( checkTrackId != idx ) { cout << "WARNING: TRACKID DOES NOT MATCH!" << endl; continue; }
          TVector3 posin = mcHit->GetInPosition();
          TVector3 posout = mcHit->GetOutPosition();
          if (idx > -1 ){
            TAMCeveTrack* track = eve->GetTrack(idx);
            if (m_debug > 1)
            printf("charge %d mass %g ", track->GetCharge(), track->GetMass());
            //if ( checkTrackId != idx ) { cout << "WARNING: TRACKID DOES NOT MATCH!" << endl; continue; }
            TVector3 momin = mcHit->GetInMomentum();
            TVector3 momout = mcHit->GetOutMomentum();
            if (m_debug > 1)
            printf("MC In Position pos (%.4f %.4f %.4f)\n", posin[0], posin[1], posin[2]);
            MCTruthInfo VTInfo;
            VTInfo.MCTrackId = idx;
            VTInfo.MCFlukaId = track->GetFlukaID();
            VTInfo.MCMass = track->GetMass();
            VTInfo.MCCharge = track->GetCharge();
            VTInfo.MCGenPosition = track->GetInitPos();
            VTInfo.MCGenMomentum = track->GetInitP();
            VTInfo.MCPosition = (posin + posout)*.5;
            VTInfo.MCMomentum = (momin + momout)*.5;
            MCVTInfo[m_VT_clusCollection.size()-1] = VTInfo;
            break;
          }
          MCTruthInfo VTInfo;
          VTInfo.MCTrackId = idx;
          VTInfo.MCFlukaId = -999.;
          VTInfo.MCMass = -999.;
          VTInfo.MCCharge = -999.;
          VTInfo.MCGenPosition.SetXYZ(-999., -999, -999);
          VTInfo.MCGenMomentum.SetXYZ(-999., -999, -999);
          VTInfo.MCPosition = (posin + posout)*.5;
          VTInfo.MCMomentum.SetXYZ(-999., -999, -999);
          MCVTInfo[m_VT_clusCollection.size()-1] = VTInfo;
        }
        break;
      }
    }
  }
  m_MCInfo[m_detectorID_map["VT"]] = MCVTInfo;
   
  return 0;
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

  //map for m_MCInfo
  map<int, MCTruthInfo> MCITInfo;

  //cluster test

  // TAITparGeo* itrGeo = (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object();
  TAITntuCluster* itclus = (TAITntuCluster*) gTAGroot->FindDataDsc("itClus","TAITntuCluster")->Object();

  TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuEve")->Object();
  TAMCntuHit* itMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("itMc", "TAMCntuHit")->Object();

  int totClus = 0;

  Int_t nPlanes = m_IT_geo->GetSensorsN();

  for(Int_t iPlane = 0; iPlane < nPlanes; iPlane++){
    Int_t nclus = itclus->GetClustersN(iPlane);
    if (m_debug > 1)
    std::cout << "\nfound " << nclus << " in plane " << iPlane << std::endl;
    totClus += nclus;
    if (nclus == 0) continue;

    for(Int_t iClus = 0; iClus < nclus; ++iClus){
      if (m_debug > 1)
      std::cout << "entered cycle clusIT of plane " << iPlane << std::endl;
      TAITcluster* clus = itclus->GetCluster(iPlane, iClus);
      if (!clus->IsValid()) continue;

      //TVector3 prova = clus->GetPositionG();
      TVector3 prova = m_GeoTrafo->FromITLocalToGlobal( clus->GetPositionG() );

      if (m_debug > 1)
      prova.Print();


      m_IT_clusCollection.push_back(clus);


      //---------------------------testing access to hits and pixels from cluster--------------------
      Int_t nHits = clus->GetPixelsN();
      // cout << "nHits: " << nHits  << endl;


      for (Int_t jHit = 0; jHit < nHits; ++jHit) {
        TAITntuHit* hit = (TAITntuHit*)clus->GetPixel(jHit);
        for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
          Int_t idx = hit->GetMcTrackIdx(k);
          Int_t id = hit->GetMcIndex(k);
          // cout << "id: " << id  << endl;
          // cout << "idxx: " << idx  << endl;
          TAMChit* mcHit = itMc->GetHit(id);
          Int_t checkTrackId = mcHit->GetTrackId();
          //if ( checkTrackId != idx ) { cout << "WARNING: TRACKID DOES NOT MATCH!" << endl; continue; }
          TVector3 posin = mcHit->GetInPosition();
          TVector3 posout = mcHit->GetOutPosition();
          if (idx > -1 ){
            TAMCeveTrack* track = eve->GetTrack(idx);
            if (m_debug > 1)
            printf("charge %d mass %g ", track->GetCharge(), track->GetMass());
            TVector3 momin = mcHit->GetInMomentum();
            TVector3 momout = mcHit->GetOutMomentum();
            if (m_debug > 1)
            printf("MC In Position pos (%.4f %.4f %.4f)\n", posin[0], posin[1], posin[2]);
            MCTruthInfo ITInfo;
            ITInfo.MCTrackId = idx;
            ITInfo.MCFlukaId = track->GetFlukaID();
            ITInfo.MCMass = track->GetMass();
            ITInfo.MCCharge = track->GetCharge();
            ITInfo.MCGenPosition = track->GetInitPos();
            ITInfo.MCGenMomentum = track->GetInitP();
            ITInfo.MCPosition = (posin + posout)*.5;
            ITInfo.MCMomentum = (momin + momout)*.5;
            MCITInfo[m_IT_clusCollection.size()-1] = ITInfo;
            break;
          }
          MCTruthInfo ITInfo;
          ITInfo.MCTrackId = idx;
          ITInfo.MCFlukaId = -999.;
          ITInfo.MCMass = -999.;
          ITInfo.MCCharge = -999.;
          ITInfo.MCGenPosition.SetXYZ(-999., -999, -999);
          ITInfo.MCGenMomentum.SetXYZ(-999., -999, -999);
          ITInfo.MCPosition = (posin + posout)*.5;
          ITInfo.MCMomentum.SetXYZ(-999., -999, -999);
          MCITInfo[m_IT_clusCollection.size()-1] = ITInfo;
        }
        break;
      }
    }
  }
  m_MCInfo[m_detectorID_map["IT"]] = MCITInfo;
   
  return 0;
}



//----------------------------------------------------------------------------------------------------
// upload clusters from micro strip
int KFitter::UploadClusMSD() {

  //map for m_MCInfo
  map<int, MCTruthInfo> MCMSDInfo;

  //cluster test
  TAMSDntuCluster* msdclus = (TAMSDntuCluster*) gTAGroot->FindDataDsc("msdClus","TAMSDntuCluster")->Object();

  TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuEve")->Object();
  TAMCntuHit* msdMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("msdMc", "TAMCntuHit")->Object();

  int totClus = 0;
  Int_t nPlanes = m_MSD_geo->GetSensorsN();

  for(Int_t iPlane = 0; iPlane < nPlanes; iPlane++){
    Int_t nclus = msdclus->GetClustersN(iPlane);
    if (m_debug > 1)
    std::cout << "\nfound " << nclus << " in plane " << iPlane << std::endl;
    totClus += nclus;
    if (nclus == 0) continue;

    for(Int_t iClus = 0; iClus < nclus; ++iClus){
      if (m_debug > 1)
      std::cout << "entered cycle clusMSD of plane " << iPlane << std::endl;
      TAMSDcluster* clus = msdclus->GetCluster(iPlane, iClus);
      if (!clus->IsValid()) continue;

      TVector3 prova = m_GeoTrafo->FromMSDLocalToGlobal( clus->GetPositionG() );

      if (m_debug > 1)
      prova.Print();

      m_MSD_clusCollection.push_back(clus);


      //---------------------------testing access to hits and pixels from cluster--------------------
      Int_t nHits = clus->GetStripsN();
      // cout << "nHits: " << nHits  << endl;


      for (Int_t jHit = 0; jHit < nHits; ++jHit) {
        TAMSDntuHit* hit = (TAMSDntuHit*)clus->GetStrip(jHit);
        for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
          Int_t idx = hit->GetMcTrackIdx(k);
          Int_t id = hit->GetMcIndex(k);
          // cout << "id: " << id  << endl;
          // cout << "idxx: " << idx  << endl;
          TAMChit* mcHit = msdMc->GetHit(id);
          Int_t checkTrackId = mcHit->GetTrackId();
          //if ( checkTrackId != idx ) { cout << "WARNING: TRACKID DOES NOT MATCH!" << endl; continue; }
          TVector3 posin = mcHit->GetInPosition();
          TVector3 posout = mcHit->GetOutPosition();
          if (idx > -1 ){
            TAMCeveTrack* track = eve->GetTrack(idx);
            if (m_debug > 1)
            printf("charge %d mass %g ", track->GetCharge(), track->GetMass());
            TVector3 momin = mcHit->GetInMomentum();
            TVector3 momout = mcHit->GetOutMomentum();
            if (m_debug > 1)
            printf("MC In Position pos (%.4f %.4f %.4f)\n", posin[0], posin[1], posin[2]);
            MCTruthInfo MSDInfo;
            MSDInfo.MCTrackId = idx;
            MSDInfo.MCFlukaId = track->GetFlukaID();
            MSDInfo.MCMass = track->GetMass();
            MSDInfo.MCCharge = track->GetCharge();
            MSDInfo.MCGenPosition = track->GetInitPos();
            MSDInfo.MCGenMomentum = track->GetInitP();
            MSDInfo.MCPosition = (posin + posout)*.5;
            MSDInfo.MCMomentum = (momin + momout)*.5;
            MCMSDInfo[m_MSD_clusCollection.size()-1] = MSDInfo;
            break;
          }
          MCTruthInfo MSDInfo;
          MSDInfo.MCTrackId = idx;
          MSDInfo.MCFlukaId = -999.;
          MSDInfo.MCMass = -999.;
          MSDInfo.MCCharge = -999.;
          MSDInfo.MCGenPosition.SetXYZ(-999., -999, -999);
          MSDInfo.MCGenMomentum.SetXYZ(-999., -999, -999);
          MSDInfo.MCPosition = (posin + posout)*.5;
          MSDInfo.MCMomentum.SetXYZ(-999., -999, -999);
          MCMSDInfo[m_MSD_clusCollection.size()-1] = MSDInfo;
        }
        break;
      }
    }
  }
  m_MCInfo[m_detectorID_map["MSD"]] = MCMSDInfo;
   
  return 0;
}


// //----------------------------------------------------------------------------------------------------
// // upload measurement points from Strip
int KFitter::UploadHitsMSD() {


//   //map for m_MCInfo
//   map<int, MCTruthInfo> MCMSDInfo;

//   TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuEve")->Object();
//   TAMCntuHit* msdMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("msdMc", "TAMCntuHit")->Object();

//   int totClus = 0;
//   Int_t nPlanes = m_MSD_geo->GetSensorsN();

//   // take the ntuple object already filled
//   TAMSDntuRaw* ntup = (TAMSDntuRaw*) gTAGroot->FindDataDsc("msdRaw", "TAMSDntuRaw")->Object();
//   int totStrips = 0;

//   for(Int_t iPlane = 0; iPlane < nPlanes; iPlane++){
//     totStrips += ntup->GetStripsN(iPlane);

//     if (m_debug > 1)
//     std::cout << "\nfound " << ntup->GetStripsN(iPlane)  << " strip hits in plane " << iPlane << std::endl;
//     if ( ntup->GetStripsN(iPlane) == 0) continue;

//     for(Int_t iStrip = 0; iStrip < ntup->GetStripsN(iPlane); ++iStrip){
//       if (m_debug > 1)
//       std::cout << "entered cycle hitMSD of plane " << iPlane << std::endl;

//       //TVector3 prova = ntup->GetStrip(iPlane, iStrip)->GetMCPosition();

//       // if (m_debug > 1)
//       // 	prova.Print();

//       m_MSD_hitCollection.push_back(ntup->GetStrip(iPlane, iStrip));


//       //---------------------------access to MC info from hit--------------------


//       TAMSDntuHit* hit = ntup->GetStrip(iPlane, iStrip);
//       if (m_debug > 1 ) cout<< "hit->GetMcTracksN() " << hit->GetMcTracksN() << endl;
//       for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
//         Int_t idx = hit->GetMcTrackIdx(k);
//         Int_t id = hit->GetMcIndex(k);
//         if (m_debug > 1){
//           cout << "McTrackId: " << idx << endl;
//           cout << "McIndex: " << id << endl;
//         }
//         TAMChit* mcHit = msdMc->GetHit(id);
//         Int_t checkTrackId = mcHit->GetTrackId();
//         //if ( checkTrackId != idx ) { cout << "WARNING: TRACKID DOES NOT MATCH!" << endl; continue; }
//         TVector3 posin = mcHit->GetInPosition();
//         TVector3 posout = mcHit->GetOutPosition();
//         if (idx > -1 ){
//           TAMCeveTrack* track = eve->GetHit(idx);
//           if (m_debug > 1)
//           printf("charge %d mass %g ", track->GetCharge(), track->GetMass());
//           TAMChit* mcHit = msdMc->GetHit(id);
//           Int_t checkTrackId = mcHit->GetTrackId();
//           //if ( checkTrackId != idx ) { cout << "WARNING: TRACKID DOES NOT MATCH!" << endl; continue; }
//           TVector3 momin = mcHit->GetInMomentum();
//           TVector3 momout = mcHit->GetOutMomentum();
//           if (m_debug > 1)
//           printf("MC In Position pos (%.4f %.4f %.4f)\n", posin[0], posin[1], posin[2]);
//           MCTruthInfo MSDInfo;
//           MSDInfo.MCTrackId = idx;
//           MSDInfo.MCFlukaId = track->GetFlukaID();
//           MSDInfo.MCMass = track->GetMass();
//           MSDInfo.MCCharge = track->GetCharge();
//           MSDInfo.MCGenPosition = track->GetInitPos();
//           MSDInfo.MCGenMomentum = track->GetInitP();
//           MSDInfo.MCPosition = (posin + posout)*.5;
//           MSDInfo.MCMomentum = (momin + momout)*.5;
//           MCMSDInfo[m_MSD_hitCollection.size()-1] = MSDInfo;
//           break;
//         }
//         MCTruthInfo MSDInfo;
//         MSDInfo.MCTrackId = idx;
//         MSDInfo.MCFlukaId = -999.;
//         MSDInfo.MCMass = -999.;
//         MSDInfo.MCCharge = -999.;
//         MSDInfo.MCGenPosition.SetXYZ(-999., -999, -999);
//         MSDInfo.MCGenMomentum.SetXYZ(-999., -999, -999);
//         MSDInfo.MCPosition = (posin + posout)*.5;
//         MSDInfo.MCMomentum.SetXYZ(-999., -999, -999);
//         MCMSDInfo[m_MSD_hitCollection.size()-1] = MSDInfo;
//       }
//     }
//   }
//   m_MCInfo[m_detectorID_map["MSD"]] = MCMSDInfo;
   return 0;
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






// TAMCeveTrack* genParticle;
			// if ( clus->GetMcTracksN() > 0 ) { 
			// 	genParticle = eve->GetHit( clus->GetMcTrackIdx(0) );
			// }

//----------------------------------------------------------------------------------------------------
// pack together the hits to be fitted, from all the detectors, selct different preselecion m_systemsONs
int KFitter::PrepareData4Fit_dataLike( Track* fitTrack ) {

	// if ( m_systemsON != "all" || !( m_systemsON.find( "VT" ) != string::npos && m_systemsON.find( "VT" ) != string::npos && m_systemsON.find( "VT" ) != string::npos ) ) 
	// 	return 0;

	// TAVTntuTrack* vtTrk_container = (TAVTntuTrack*) gTAGroot->FindDataDsc("vtTrack","TAVTntuTrack")->Object();

	// //loop over tracks
	// for (int track_i = 0; track_i< vtTrk_container->GetTracksN(); track_i++) {

	// 	TAVTtrack* track = vtTrk_container->GetTrack( track_i );
	// 	// N clusters per track
	// 	int ncluster = track->GetClustersN();

	// 	// get interaction point
	// 	TVector3 interactionPoint = track->GetVertex();

	// 	// loop over clusters in the track  get clusters in track ->  
	// 	for (int i=0; i<ncluster; i++) {

	// 		TAVTcluster* clus = (TAVTcluster*) track->GetCluster( i );
	// 		if (!clus->IsValid()) continue;

	// 		m_VT_clusCollection.push_back(clus);	
	// 		Prepare4Vertex( clus, track_ID );				//fill map m_hitCollectionToFit_dataLike
		
	// 		// create map with helpers

	// 	}

	// 	TVector3 slope = track->GetSlopeZ();
	// 	TVector3 correction(0.28, 0, 0);		

	
	// 	TAITntuCluster* itclus = (TAITntuCluster*) gTAGroot->FindDataDsc("itClus","TAITntuCluster")->Object();

	// 	float distance = 666;
	// 	pair<int, int> itcClusCoordinate = make_pair(0,0);

	// 	// loop on IT Clusters
	// 	for( int iPlane = 0; iPlane < m_IT_geo->GetSensorsN(); iPlane++){
			
	// 		// extrapolate track to IT + correggi
	// 		float itZ = m_GeoTrafo->GetITCenter().Z(); 
	// 		float radius = ( itZ - interactionPoint.z() ) / slope.z();
	// 		TVector3 extrapIT(  
	// 						slope.x() * radius + interactionPoint.x() + corr.x(), 
	// 						slope.y() * radius + interactionPoint.y(), 
	// 						itZ
	// 						);

	// 		int nclus = itclus->GetClustersN(iPlane);
	// 		for(Int_t iClus = 0; iClus < itclus->GetClustersN(iPlane); ++iClus){

	// 			TAITcluster* clus = itclus->GetCluster(iPlane, iClus);
	// 			TVector3 clusPos = m_GeoTrafo->FromITLocalToGlobal( clus->GetPositionG() );
	// 			float tmp_distance = fabs( (extrapIT - clusPos).Mag() );
	// 			if ( tmp_distance < distance )	{
	// 				distance = tmp_distance;
	// 				itClusCoordinate.make_pair(iPlane, iClus);
	// 			}

	// 		}
	// 		//push back in categorise the closer cluster per layer
	// 		if ( distance < 0.3 ) {	
	// 			Prepare4InnerTracker( itclus->GetCluster(iPlane, iClus), track_ID );
	// 		}
	// 	}	//end IT Clusters loop



	// 	// // ****************************  Correct Fit Procedure *****************
	// 	// // int pdg = randomPdg();
	// 	// genfit::AbsTrackRep* rep;
	// 	// rep = new genfit::RKTrackRep( (UpdatePDG::GetPDG()->GetPdgCode( "C11" ) ) );
	// 	// genfit::StateOnPlane state(rep);
	// 	// TVector3 mom();
	// 	// rep->setPosMom(state, pos, mom);
	// 	// // genfit::SharedPlanePtr origPlane = state.getPlane();
	// 	// // point, normal
	// 	// genfit::SharedPlanePtr plane(new genfit::DetPlane( TVector3(0,0,m_GeoTrafo->GetMSDCenter().Z()), TVector3(0,0,1) ) );
	// 	// // forth
	// 	// double extrapLen(0);
	// 	// try {
	// 	// 	extrapLen = rep->extrapolateToPlane(state, plane);

	// 	// 	// mom2 = state.getMom();
	// 	// 	// momLoss1 = mom.Mag()-mom2.Mag();

	// 	// }
	// 	// catch (genfit::Exception& e) {
	// 	// 	std::cerr << "Exception in forth Extrapolation. PDG = " << pdg << "; mom: \n";
	// 	// 	mom.Print();

	// 	// 	std::cerr << e.what();

	// 	// 	delete rep;
	// 	// 	return kException;
	// 	// }
	// 	// // ****************************  Correct Fit Procedure *****************



	// 	// extrapolate to MSD 
	// 	// extrapolate track to MSD + correggi
		


	// 	// loop on MSD Clusters
	// 	map<int, TVector3> msd_x;
	// 	map<int, TVector3> msd_y;
	// 	float posMSDplane;
	// 	TAMSDntuRaw* ntup = (TAMSDntuRaw*) gTAGroot->FindDataDsc("msdRaw", "TAMSDntuRaw")->Object();
	// 	for( int iPlane = 0; iPlane <m_MSD_geo->GetSensorsN(); iPlane++){

	// 		if (iPlane == 0	|| iPlane == 1)  	posMSDplane = -5;
	// 		else if (iPlane == 2 || iPlane == 3)  	posMSDplane = 0;
	// 		else if (iPlane == 4 || iPlane == 5)  	posMSDplane = 5;

	// 		for(Int_t iStrip = 0; iStrip < ntup->GetStripsN(iPlane); ++iStrip){

	// 			TAMSDntuHit* hit = ntup->GetStrip(iPlane, iStrip)

	// 			if ( p_hit->GetView() == 0 ) {
	// 				msd_x[ track_ID ]  ( 	m_GeoTrafo->FromMSDLocalToGlobal( hit.GetPosition() ), 
	// 										0,
	// 										m_GeoTrafo->FromMSDLocalToGlobal( posMSDplane )  
	// 									);
	// 			}
	// 			else {
	// 				msd_y[ track_ID ]  ( 	0,
	// 										m_GeoTrafo->FromMSDLocalToGlobal( hit.GetPosition() ), 
	// 										m_GeoTrafo->FromMSDLocalToGlobal( posMSDplane )  
	// 									);
	// 			}
	// 		}
	// 	}



	// 	for ( map< int, float >::iterator stripx = msd_x.begin(); stripx != msd_x.end(); stripx++ ) {

	// 		for ( map< int, float >::iterator stripy = msd_y.begin(); stripy != msd_y.end(); stripy++ ) {

	// 			TVector3 clusPos ( 	m_GeoTrafo->FromMSDLocalToGlobal( (*stripx).x() ),
	// 								m_GeoTrafo->FromMSDLocalToGlobal( (*stripy).y() ),
	// 								(*stripx).Z()			
	// 							 );
	// 			float itZ = (*stripx).Z(); 
	// 			float radius = ( itZ - interactionPoint.z() ) / slope.z();
	// 			TVector3 extrapIT(  
	// 							slope.x() * radius + interactionPoint.x() + 2*corr.x(), 
	// 							slope.y() * radius + interactionPoint.y(), 
	// 							itZ
	// 							);

	// 			float tmp_distance = fabs( (extrapIT - clusPos).Mag() );
	// 			if ( tmp_distance < 0.6 )	{
	// 				Prepare4Strip( clusPos, track_ID );
	// 			}

	// 		}
	// 	}
		

	// 	// loop on MSD clusters 
	// 		// 	push back 

	// }
   return 0;
}




//----------------------------------------------------------------------------------------------------
// pack together the hits to be fitted, from all the detectors, selct different preselecion m_systemsONs
int KFitter::PrepareData4Fit( Track* fitTrack ) {

  if ( m_debug > 0 )		cout << "\n\n*******\tKFitter::PrepareData4Fit\t*******\n" << endl;

  // Vertex -  fill fitter collections

  if ( (m_systemsON == "all" || m_systemsON.find( "VT" ) != string::npos) && GlobalPar::GetPar()->IncludeVT() ) {
    UploadClusVT();
    //UploadHitsVT();
    if ( m_debug > 0 )		cout << endl<<endl << "Filling vertex hit collection  = " << m_VT_clusCollection.size() << endl;
    Prepare4Vertex(fitTrack);
  }

  // Inner Tracker -  fill fitter collections
  if ( (m_systemsON == "all" || m_systemsON.find( "IT" ) != string::npos) && GlobalPar::GetPar()->IncludeIT() ) {
    //UploadHitsIT();
    UploadClusIT();
    if ( m_debug > 0 )		cout <<endl<<endl << "Filling inner detector hit collection = " << m_IT_clusCollection.size() << endl;
    Prepare4InnerTracker(fitTrack);
  }

  // MSD -  fill fitter collections
  if ( (m_systemsON == "all" || m_systemsON.find( "MSD" ) != string::npos) && GlobalPar::GetPar()->IncludeMSD() ) {
    //    UploadClusMSD();
    UploadHitsMSD();
    if ( m_debug > 0 )		cout << endl<<endl << "Filling Strip hit collection = " << m_MSD_hitCollection.size() << endl;
    Prepare4Strip(fitTrack);
  }

  // Tof Wall-  fill fitter collections
  if ( ( m_systemsON.find( "TW" ) != string::npos) && GlobalPar::GetPar()->IncludeTW() ) {
    UploadHitsTW();
    if ( m_debug > 0 )		cout <<endl<<endl << "Filling scintillator hit collection = " << m_TW_hitCollection.size() << endl;
    Prepare4TofWall(fitTrack);
  }

  // try to categorise the particle that generated the hit. If it fails --> clean the hit object
  CategoriseHitsToFit_withTrueInfo();

  // remove the category if does not satisfy the requirements
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

  //	if no map element survive -> clear the single-detector hit-collections
  if ( m_hitCollectionToFit.size() == 0 ) {
    m_VT_clusCollection.clear();
    m_IT_clusCollection.clear();
    // m_VT_hitCollection.clear();
    // m_IT_hitCollection.clear();
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
void KFitter::Prepare4Vertex( TAVTcluster* clus, int track_ID, int iHit ) {

	if ( m_debug > 0 )
	cout << "\nPrepare4Vertex::Entered\n";

	TMatrixDSym hitCov(3);
	TVectorD hitCoords(3);
	// for (unsigned int i = 0; i < m_VT_clusCollection.size(); i++) {
		// TAVTcluster* p_hit = m_VT_clusCollection.at(i);

		// get pixel coord
		TVector3 hitPos = m_GeoTrafo->FromVTLocalToGlobal( clus->GetPositionG() );

		if ( m_debug > 0 )
		  hitPos.Print();

		// set hit position vector
		hitCoords(0)=hitPos.x();
		hitCoords(1)=hitPos.y();
		hitCoords(2)=hitPos.z();
		// set covariance matrix
		// double pixReso = GlobalPar::GetPar()->VTReso();

		TVector3 pixReso = clus->GetPosError();
		pixReso(2) = 0.005;   // 200 micron
		//   TVector3 pixReso =  m_GeoTrafo->FromVTLocalToGlobal( pixReso_ );
		hitCov.UnitMatrix();
		for (int j = 0; j < 3; j++){
		  hitCov[j][j] = pixReso(j)*pixReso(j);
		}

		//hardcoded
		// hitCov[2][2] = 0.000005;

		//hitCov.Print();

		// nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
		AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["VT"], iHit, nullptr );

		m_hitCollectionToFit_dataLike[ track_ID ].push_back( hit );
		m_allHitsInMeasurementFormat.push_back(hit);

	// }
	if ( m_debug > 0 )
	cout << "\nPrepare4Vertex::Exiting\n";

}



//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4Vertex( Track* fitTrack ) {

  if ( m_debug > 0 )
  cout << "\nPrepare4Vertex::Entered\n";

  TMatrixDSym hitCov(3);
  TVectorD hitCoords(3);
  for (unsigned int i = 0; i < m_VT_clusCollection.size(); i++) {
    TAVTcluster* p_hit = m_VT_clusCollection.at(i);

    // get pixel coord
    TVector3 hitPos = m_GeoTrafo->FromVTLocalToGlobal( p_hit->GetPositionG() );

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
    //   TVector3 pixReso =  m_GeoTrafo->FromVTLocalToGlobal( pixReso_ );
    hitCov.UnitMatrix();
    for (int j = 0; j < 3; j++){
      hitCov[j][j] = pixReso(j)*pixReso(j);
    }

    //hardcoded
    hitCov[2][2] = 0.000005;

    //hitCov.Print();

    // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["VT"], i, nullptr );

    m_allHitsInMeasurementFormat.push_back(hit);

  }
  if ( m_debug > 0 )
  cout << "\nPrepare4Vertex::Exiting\n";
}



//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4InnerTracker( TAITcluster* clus, int track_ID, int iHit ) {

	if ( m_debug > 0 )
	cout << "\nPrepare4Vertex::Entered\n";

	TMatrixDSym hitCov(3);
	TVectorD hitCoords(3);
	

	// get pixel coord
	TVector3 hitPos = m_GeoTrafo->FromITLocalToGlobal( clus->GetPositionG() );

	if ( m_debug > 0 )
	  hitPos.Print();

	// set hit position vector
	hitCoords(0)=hitPos.x();
	hitCoords(1)=hitPos.y();
	hitCoords(2)=hitPos.z();
	// set covariance matrix
	// double pixReso = GlobalPar::GetPar()->VTReso();

	TVector3 pixReso = clus->GetPosError();
	pixReso(2) = 0.005;   // 50 micron
	//   TVector3 pixReso =  m_GeoTrafo->FromVTLocalToGlobal( pixReso_ );
	hitCov.UnitMatrix();
	for (int j = 0; j < 3; j++){
	  hitCov[j][j] = pixReso(j)*pixReso(j);
	}

	//hardcoded
	// hitCov[2][2] = 0.000005;

	//hitCov.Print();

	// nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
	AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["IT"], iHit, nullptr );

	m_hitCollectionToFit_dataLike[ track_ID ].push_back( hit );
	m_allHitsInMeasurementFormat.push_back(hit);

	
	if ( m_debug > 0 )
	cout << "\nPrepare4Vertex::Exiting\n";

}




//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4InnerTracker( Track* fitTrack ) {

  if ( m_debug > 0 )
  cout << "\nPrepare4InnerTracker::Entered\n";

  TMatrixDSym hitCov(3);
  TVectorD hitCoords(3);

  for (unsigned int i = 0; i < m_IT_clusCollection.size(); i++) {
    TAITcluster* p_hit = m_IT_clusCollection.at(i);
    // get pixel coord
    TVector3 hitPos = m_GeoTrafo->FromITLocalToGlobal( p_hit->GetPositionG() );
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
    //    TVector3 pixReso = m_GeoTrafo->FromITLocalToGlobal( pixReso_ );
    hitCov.UnitMatrix();
    for (int j = 0; j < 3; j++){
      hitCov[j][j] = pixReso(j)*pixReso(j);
    }

    //hardcoded
    hitCov[2][2] = 0.000005;

    //hitCov.Print();

    // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["IT"], i, nullptr );
    m_allHitsInMeasurementFormat.push_back(hit);

  }
  if ( m_debug > 0 )
  cout << "\nPrepare4InnerTracker::Exiting\n";
}



//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4TofWall( Track* fitTrack ) {

  if ( m_debug > 0 )
  cout << "\nPrepare4TofWall::Entered\n";

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
  if ( m_debug > 0 )
  cout << "\nPrepare4TofWall::Exiting\n";
}



void KFitter::Prepare4Strip(TVector3 pos, int track_ID, int iHit ) {

	if ( m_debug > 0 )
	cout << "\nPrepare4Strip::Entered\n";

	TMatrixDSym hitCov(3);
	TVectorD hitCoords(3);


    // set hit position vector
    hitCoords(0)=pos.x();
    hitCoords(1)=pos.y();
    hitCoords(2)=pos.z();
    // set covariance matrix
    // double pixReso = 0.001;
    double pixReso = GlobalPar::GetPar()->MSDReso();

    //TVector3 pixReso_ = p_hit->GetPosError();
    // TVector3 pixReso =  m_GeoTrafo->FromVTLocalToGlobal( pixReso_ );
    hitCov.UnitMatrix();
    for (int j = 0; j < 3; j++){
      hitCov[j][j] = pixReso*pixReso;
    }

    //hardcoded
    hitCov[2][2] = 0.0002;

    //hitCov.Print();

    // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["MSD"], iHit, nullptr );

    m_hitCollectionToFit_dataLike[ track_ID ].push_back( hit );
    m_allHitsInMeasurementFormat.push_back(hit);

}






//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4Strip( Track* fitTrack ) {

  if ( m_debug > 0 )
  cout << "\nPrepare4Strip::Entered\n";

  TMatrixDSym hitCov(3);
  TVectorD hitCoords(3);
//
//	vector<TAMSDntuHit*> allStripSignals_x;
//	vector<TAMSDntuHit*> allStripSignals_y;
//
//	m_MSD_posVectorSmearedHit.clear();
//	m_MSD_momVectorSmearedHit.clear();
//	m_MSD_mass.clear();
//
  for (unsigned int i = 0; i < m_MSD_hitCollection.size(); i++) {

    TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuEve")->Object();
    TAMCntuHit* msdMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("msdMc", "TAMCntuHit")->Object();
    TAMSDntuHit* p_hit = m_MSD_hitCollection.at(i);
    if ( p_hit->GetView() == 1 ) continue;
    Int_t idx = p_hit->GetMcTrackIdx(0);
    Int_t id = p_hit->GetMcIndex(0);
    if (m_debug > 1){
      cout << "McTrackId: " << idx << endl;
      cout << "McIndex: " << id << endl;
    }
    TAMChit* mcHit = msdMc->GetHit(id);
    Int_t checkTrackId = mcHit->GetTrackId();
    //if ( checkTrackId != idx ) { cout << "WARNING: TRACKID DOES NOT MATCH!" << endl; continue; }
    TVector3 posin = mcHit->GetInPosition();
    TVector3 posout = mcHit->GetOutPosition();
    if (idx > -1 ){
      TAMCeveTrack* track = eve->GetTrack(idx);
    }
    // if (m_debug > 1)
    // printf("charge %d mass %g \n", track->GetCharge(), track->GetMass());
    TVector3 hitPos = (posin + posout)*.5;

    if ( m_debug > 0 )
    hitPos.Print();

    // set hit position vector
    hitCoords(0)=hitPos.x();
    hitCoords(1)=hitPos.y();
    hitCoords(2)=hitPos.z();
    // set covariance matrix
    // double pixReso = 0.001;
    double pixReso = GlobalPar::GetPar()->MSDReso();

    //TVector3 pixReso_ = p_hit->GetPosError();
    //TVector3 pixReso =  m_GeoTrafo->FromVTLocalToGlobal( pixReso_ );
    hitCov.UnitMatrix();
    for (int j = 0; j < 3; j++){
      hitCov[j][j] = pixReso*pixReso;
    }

    //hardcoded
    hitCov[2][2] = 0.0002;

    //hitCov.Print();

    // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["MSD"], i, nullptr );

    m_allHitsInMeasurementFormat.push_back(hit);

  }

  if ( m_debug > 0 )
  cout << "\nPrepare4Strip::Exiting\n";
}

//----------------------------------------------------------------------------------------------------
// pre-fit requirements to be applied to EACH of the hitCollections
bool KFitter::PrefitRequirements( map< string, vector<AbsMeasurement*> >::iterator element ) {

  if ( m_debug > 0 )		cout << "KFitter::PrefitRequirements()  -  Category = " << (*element).first << " " << m_systemsON << endl;


  int testHitNumberLimit = 0;
  int testHit_VT = 0;
  int testHit_IT = 0;
  int testHit_MSD = 0;
  int testHit_TW = 0;

  // define the number of hits per each detector to consider to satisfy the pre-fit requirements
  if ( m_systemsON == "all" ) {
    testHit_VT = m_VT_geo->GetSensorsN(), testHit_IT = m_IT_geo->GetSensorsN()/16, testHit_MSD = m_MSD_geo->GetSensorsN()/2;
  }

  else {
    if ( m_systemsON.find( "VT" ) != string::npos )			testHit_VT = m_VT_geo->GetSensorsN();
    if ( m_systemsON.find( "IT" ) != string::npos )			testHit_IT = m_IT_geo->GetSensorsN()/16;
    if ( m_systemsON.find( "MSD" ) != string::npos )		        testHit_MSD = m_MSD_geo->GetSensorsN()/2;
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
  }

  if ( m_debug > 0 )	cout << "nHitVT  " <<nHitVT<< " nHitIT " <<nHitIT<< " nHitMSD "<<nHitMSD<< " nHitTW "<<nHitTW<<endl;

  // test the num of hits per each detector
  if ( nHitVT != testHit_VT || nHitIT != testHit_IT || nHitMSD != testHit_MSD ){
    if ( m_debug > 0 )
      cout << "WARNING :: KFitter::PrefitRequirements  -->  number of elements different wrt the expected ones : " <<
	"\n\t nVTX = " << nHitVT << "  Nexp = " << testHit_VT << endl <<
	"\n\t nITR = " << nHitIT << "  Nexp = " << testHit_IT << endl <<
	"\n\t nMSD = " << nHitMSD << "  Nexp = " << testHit_MSD << endl <<
	"\n\t nTW = " << nHitTW << "  Nexp = " << testHit_TW << endl;

    return false;
  }

  return true;
}


// // //----------------------------------------------------------------------------------------------------
// // categorise the hit depending on the generating particle!
// void KFitter::CategoriseHitsToFit_fromVTX() {



// }




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

    if ( m_debug > 0 )		cout << "\tSelected Category: " << outName << "  flukaID=" << flukaID << "  partID="<<partID << "  charge="<<charge << "  mass="<<mass<< endl;

    // if a category already defined but with particle with a different partID  ->  make a new category with an incremental index
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




void KFitter::GetTrueParticleType( AbsMeasurement* hit, int* flukaID, int* trackID, int* charge, double* mass ) {

  int detID = hit->getDetId();
  int hitID = hit->getHitId();

  if ( m_debug > 3 )		cout << "\t\tDetector Type (in KFitter::GetTrueParticleType)= " << detID << "    HitID = " << hitID << endl;

  *flukaID = m_MCInfo[detID][hitID].MCFlukaId;
  *trackID  = m_MCInfo[detID][hitID].MCTrackId;
  *charge  = m_MCInfo[detID][hitID].MCCharge;
  *mass    = m_MCInfo[detID][hitID].MCMass;
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

  // TAVTvertex* vtxPD = 0x0;
  // TVector3 vtxPositionPD(0., 0., 0.);

  // for (Int_t iVtx = 0; iVtx < vtx->GetVertexN(); ++iVtx) {
  //   vtxPD = vtx->GetVertex(iVtx);

  //   if (vtxPD == 0x0) continue;
  //   vtxPositionPD = vtxPD->GetVertexPosition();

  //   // cout << " vtxPositionPD  local " << vtxPositionPD[2]  << endl;
  //   vtxPositionPD = fGeoTrafo->FromVTLocalToGlobal(vtxPositionPD);
  // }

  // fill m_hitCollectionToFit
  PrepareData4Fit( fitTrack );
  // check the hit vector not empty otherwise clear
  if ( m_hitCollectionToFit.size() <= 0 )	{
    if ( m_debug > 0 )		cout << "No category to fit in this event..." << endl;
    m_VT_hitCollection.clear();
    m_VT_clusCollection.clear();
    m_IT_hitCollection.clear();
    m_IT_clusCollection.clear();
    m_MSD_hitCollection.clear();

    // for ( vector<AbsMeasurement*>::iterator it2=m_allHitsInMeasurementFormat.begin(); it2 != m_allHitsInMeasurementFormat.end(); it2++ )
    // 	delete (*it2);
    delete fitTrack;
    return 2;
  }
  if ( m_debug > 0 )		cout << "\nMakeFit::m_hitCollectionToFit.size  =  " << m_hitCollectionToFit.size() << endl << endl;


  // loop over all hit category
  for ( map< string, vector<AbsMeasurement*> >::iterator hitSample=m_hitCollectionToFit.begin(); hitSample != m_hitCollectionToFit.end(); hitSample++ ) {
    vector<string> tok = Tokenize( (*hitSample).first, "-" );
    if ( m_debug > 0 )	{
      // check if the category is defined in UpdatePDG  -->  also done in GetPdgCode()
      if ( !UpdatePDG::GetPDG()->IsParticleDefined( tok.at(0) ) )
	cout << "ERROR :: KFitter::MakeFit  -->	 in UpdatePDG not found the category " << (*hitSample).first << endl, exit(0);
      cout << "\tCategory under fit  =  " << (*hitSample).first << " of size "<< (*hitSample).second.size() << endl;
    }

    // SET PARTICLE HYPOTHESIS  --> set repository
    AbsTrackRep* rep = new RKTrackRep( (UpdatePDG::GetPDG()->GetPdgCode( tok.at(0)) ) );
    fitTrack->addTrackRep( rep );

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // more than one repo can be added and genfit test everyone of them
    // use DetermineCardinalRepresentation to choose the CardinalRep with minChi2 otherwise it takes the first added
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // SetTrueSeed( &pos, &mom );	// get seed from MC for debug

    // set seed
    //here I'd like to access to TAVTVertex
    fitTrack->setStateSeed(pos, mom);

    // insert points to be fitted - loop over each measurement in the current collection
    for ( unsigned int i=0; i < (*hitSample).second.size(); i++ ){
      fitTrack->insertMeasurement( (*hitSample).second.at(i) );
      fitTrack->checkConsistency();
      if ( m_debug > 3 )
	fitTrack->Print("C");
    }

    if ( m_reverse )
      fitTrack->reverseTrackPoints();

    //check
    fitTrack->checkConsistency();
    if ( m_debug > 3 )
      fitTrack->Print();

    //pre-fit
    //MakePrefit();

    // THE REAL FIT with different Kalman modes
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


      // display->addEvent(fitTrack);


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
  m_VT_clusCollection.clear();
  m_IT_clusCollection.clear();
  m_MSD_hitCollection.clear();
  m_TW_hitCollection.clear();

  // clean all hits
  m_allHitsInMeasurementFormat.clear();
  delete fitTrack;

  // clean m_hitCollectionToFit
  for ( auto it = m_hitCollectionToFit.cbegin(), next_it = m_hitCollectionToFit.cbegin(); it != m_hitCollectionToFit.cend(); it = next_it)	{
    next_it = it; ++next_it;
    m_hitCollectionToFit.erase(it);
  }

  m_hitCollectionToFit.clear();
  m_MCInfo.clear();

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
  //double myChi2 = 0;

  //InitAllHistos( hitSampleName );

  TMatrixD KalmanPos_cov(3,3);
  TMatrixD KalmanMom_cov(3,3);

  // loop over hits
  for ( unsigned int i = 0; i<m_hitCollectionToFit[ hitSampleName ].size(); i++ ) {	//
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


    if ( m_debug > 0 )	{
      cout <<endl<< "Single Event Debug\t--\t" << hitSampleName << endl;
      cout << "Hit num = " << i << "  --  MC mass = " << tmp_mass << endl;
      cout << "\t TruePos =       "; tmpPos.Print();
      cout << "\t MeasuredPos =       "; hitPos.Print();
      cout << "\t Kalman Pos da State6D = "; KalmanPos.Print();
      cout <<endl<< "\t Gen_Mom = "<< tmp_genMom.Mag() << endl;
      cout <<endl<< "\t MC_Mom = "<< tmpMom.Mag() <<"     " <<endl;
      // tmpMom.Print();
      //cout << "\t Kalman Mom da State6D = "<< KalmanMom.Mag() << "  = Mom " << track->getFittedState(i).getMom().Mag() << endl;
      cout << "\t Kalman Mom da State6D =  " << KalmanMom.Mag() << " "; KalmanMom.Print();
      // cout << "\t Kalman Mom Error da State6D = "; KalmanMom_err.Print();
    }

    //! Get the accumulated X/X0 (path / radiation length) of the material crossed in the last extrapolation.
    // virtual double getRadiationLenght() const = 0;

    ControlPlotsRepository::GetControlObject( m_kalmanOutputDir )->SetControlMom_4eachState( hitSampleName, i, &KalmanMom, &tmpMom, &tmp_genMom );
    ControlPlotsRepository::GetControlObject( m_kalmanOutputDir )->SetControlPos_4eachState( hitSampleName, i, &KalmanPos, &tmpPos, &hitPos );

    // keep quantities to be plotted of the state CLOSER to the interaction point
    unsigned int measuredState = ( m_reverse ? m_hitCollectionToFit[ hitSampleName ].size()-1 : 0 );

    if ( i == measuredState ) {
      expectedPos = tmpPos;
      expectedMom = tmpMom;
      kalmanMom = KalmanMom;
      kalmanMom_err = KalmanMom_err;
      kalmanPos = KalmanPos;
      massMC = tmp_mass;

      m_fitTrackCollection->AddTrack( hitSampleName, track, m_evNum, i,
				      &KalmanMom, &KalmanPos,
				      &expectedMom, &expectedPos,
				      &KalmanMom_cov );
      m_fitTrackCollection->AddTrack( hitSampleName, track, m_evNum, i, // trackID?
				      &KalmanMom, &KalmanPos,
				      &expectedMom, &expectedPos,
				      &KalmanMom_cov );

  //     m_controlPlotter->SetMom_Gen( hitSampleName, &tmp_genMom );

  //     m_controlPlotter->SetMom_TrueMC( hitSampleName, &expectedMom, massMC );

  //     m_controlPlotter->SetMom_Kal( hitSampleName, &kalmanMom, &kalmanMom_err );

  //     m_controlPlotter->SetPos_Kal( hitSampleName, &kalmanPos, &KalmanPos_err );

  //     m_controlPlotter->SetTrackInfo( hitSampleName, track );

  //     if ( GlobalPar::GetPar()->IsPrintOutputNtuple() )
		// m_controlPlotter->Set_Outputntuple(&kalmanMom, &kalmanPos, &tmp_genMom);
	    }
  }

}




//----------------------------------------------------------------------------------------------------
void KFitter::GetTrueMCInfo( string hitSampleName, int x,
			     TVector3* tmpPos, TVector3* tmpMom, double* tmp_mass,
			     TVector3* tmp_genPos,  TVector3* tmp_genMom, TVector3* hitPos ) {

  int detID = m_hitCollectionToFit[ hitSampleName ].at(x)->getDetId();
  int hitID = m_hitCollectionToFit[ hitSampleName ].at(x)->getHitId();


  if ( m_debug > 3 )		cout << "\t\tDetector Type (in KFitter::GetTrueMCInfo)= " << detID << "    HitID = " << hitID << endl;

  // Generated position and momentum at particle birth
  *tmp_mass = m_MCInfo[detID][hitID].MCMass;
  *tmp_genPos = m_MCInfo[detID][hitID].MCGenPosition;
  *tmp_genMom = m_MCInfo[detID][hitID].MCGenMomentum;

  //MC position and momentum in the hit
  *tmpPos = m_MCInfo[detID][hitID].MCPosition;
  *tmpMom = m_MCInfo[detID][hitID].MCMomentum;

  //Position of the hit
  hitPos->SetX( m_hitCollectionToFit[ hitSampleName ].at(x)->getRawHitCoords()(0) );
  hitPos->SetY( m_hitCollectionToFit[ hitSampleName ].at(x)->getRawHitCoords()(1) );
  hitPos->SetZ( m_hitCollectionToFit[ hitSampleName ].at(x)->getRawHitCoords()(2) );

}


//----------------------------------------------------------------------------------------------------
void KFitter::GetKalmanTrackInfo ( string hitSampleName, int i, Track* track,
				   TVector3* KalmanPos, TVector3* KalmanMom,
				   TVector3* KalmanPos_err, TVector3* KalmanMom_err,
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

  //show event display

  // display->open();

}




//----------------------------------------------------------------------------------------------------
void KFitter::InitEventDisplay() {



  // init event display
  display = genfit::EventDisplay::getInstance();

  // needed for the event display
  //const genfit::eFitterType fitterId = genfit::SimpleKalman;
  //const genfit::eFitterType fitterId = genfit::RefKalman;
  //const genfit::eFitterType fitterId = genfit::DafRef;
  //const genfit::eFitterType fitterId = genfit::DafSimple;

  //const genfit::eMultipleMeasurementHandling mmHandling = genfit::weightedAverage;	//	suggested
  //const genfit::eMultipleMeasurementHandling mmHandling = genfit::unweightedClosestToReference;
  //const genfit::eMultipleMeasurementHandling mmHandling = genfit::unweightedClosestToPrediction;
  //const genfit::eMultipleMeasurementHandling mmHandling = genfit::weightedClosestToReference;
  //const genfit::eMultipleMeasurementHandling mmHandling = genfit::weightedClosestToPrediction;
  //const genfit::eMultipleMeasurementHandling mmHandling = genfit::unweightedClosestToReferenceWire;
  //const genfit::eMultipleMeasurementHandling mmHandling = genfit::unweightedClosestToPredictionWire; //
  //const genfit::eMultipleMeasurementHandling mmHandling = genfit::weightedClosestToReferenceWire;
  //const genfit::eMultipleMeasurementHandling mmHandling = genfit::weightedClosestToPredictionWire;


  //m_fitter->setMultipleMeasurementHandling(mmHandling);
  //f_fitter->setMultipleMeasurementHandling(unweightedClosestToPredictionWire);


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
