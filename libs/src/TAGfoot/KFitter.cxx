#include "KFitter.hxx"

ClassImp(KFitter)

//----------------------------------------------------------------------------------------------------
KFitter::KFitter (const char* name)
: TAGaction(name, "KFitter - Global GenFit Tracker")
{

  int nIter = 20; // max number of iterations
  double dPVal = 1.E-3; // convergence criterion
  m_AMU = 0.9310986964; // in GeV // conversion betweem mass in GeV and atomic mass unit

  m_debug = GlobalPar::GetPar()->Debug();

  m_recolike1 = false;

  // if start KF in forward or reverse mode
  m_reverse = GlobalPar::GetPar()->IsKalReverse();

  m_IsEDOn = false;
  m_workWithMC = false;

  // m_diceRoll = new TRandom3();
  // m_diceRoll->SetSeed(0);

  // clear hits collection
  m_VT_hitCollection.clear();
  m_IT_hitCollection.clear();
  m_MSD_hitCollection.clear();

  m_VT_clusCollection.clear();
  m_IT_clusCollection.clear();
  m_MSD_clusCollection.clear();

  m_MSD_pointCollection.clear();
  m_TW_hitCollection.clear();

  m_MCInfo.clear();

  m_vectorTrack.clear();

  m_fitTrackCollection = new GlobalTrackRepostory();  // contains the fitted track info

  // Create dir for kalman output
  struct stat info;
  m_kalmanOutputDir = (string)getenv("FOOTRES")+"/Kalman";
  if( stat( m_kalmanOutputDir.c_str(), &info ) != 0 )		//cannot access
    system(("mkdir "+m_kalmanOutputDir).c_str());


  // checks for the detector to be used for kalman
  IncludeDetectors();

  // take geometry objects
  if (GlobalPar::GetPar()->IncludeTG())
    m_TG_geo = shared_ptr<TAGparGeo> ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() );

  if (GlobalPar::GetPar()->IncludeDI())
  m_DI_geo = shared_ptr<TADIparGeo> ( (TADIparGeo*) gTAGroot->FindParaDsc("diGeo", "TADIparGeo")->Object() );

  if ( (m_systemsON == "all" || m_systemsON.find( "VT" ) != string::npos) && GlobalPar::GetPar()->IncludeVT() )
    m_VT_geo = shared_ptr<TAVTparGeo> ( (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() );

  if ( (m_systemsON == "all" || m_systemsON.find( "IT" ) != string::npos) && GlobalPar::GetPar()->IncludeIT() )
    m_IT_geo = shared_ptr<TAITparGeo> ( (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object() );

  if ( (m_systemsON == "all" || m_systemsON.find( "MSD" ) != string::npos) && GlobalPar::GetPar()->IncludeMSD() )
    m_MSD_geo = shared_ptr<TAMSDparGeo> ( (TAMSDparGeo*) gTAGroot->FindParaDsc("msdGeo", "TAMSDparGeo")->Object() );

  if ( ( m_systemsON.find( "TW" ) != string::npos) && GlobalPar::GetPar()->IncludeTW() )
    m_TW_geo = shared_ptr<TATWparGeo> ( (TATWparGeo*) gTAGroot->FindParaDsc("twGeo", "TATWparGeo")->Object() );


  m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

  CreateDetectorPlanes();

  //set the stage for TGeoManagerInterface class of GenFit
  // this is the World volume
  TGeoMedium* med = gGeoManager->GetMedium("AIR");
  m_TopVolume = gGeoManager->MakeBox("World",med, 220., 30., 30.);
  m_TopVolume->SetInvisible();
  gGeoManager->SetTopVolume(m_TopVolume);

  m_vecHistoColor = { kBlack, kRed-9, kRed+1, kRed-2, kOrange+7, kOrange, kOrange+3, kGreen+1,
    kGreen+3, kBlue+1, kBlue+3, kAzure+8, kAzure+1, kMagenta+2,
    kMagenta+3, kViolet+1, kViolet+6, kViolet-4 };


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
    tgVol->SetLineColor(kBlack);
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAGparGeo::GetBaseName());
    m_TopVolume->AddNode(tgVol, 1, transfo);
  }

  // Vertex
  if (GlobalPar::GetPar()->IncludeVT()) {
    TGeoVolume* vtVol  = m_VT_geo->BuildVertex();
    vtVol->SetLineColor(kRed+1);
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAVTparGeo::GetBaseName());
    m_TopVolume->AddNode(vtVol, 2, transfo);
  }

  // Magnet
  if (GlobalPar::GetPar()->IncludeDI()) {
    TGeoVolume* diVol = m_DI_geo->BuildMagnet();
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TADIparGeo::GetBaseName());
    m_TopVolume->AddNode(diVol, 3, transfo);
  }

  // IT
  if (GlobalPar::GetPar()->IncludeIT()) {
    TGeoVolume* itVol  = m_IT_geo->BuildInnerTracker();
    itVol->SetLineColor(kRed);
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAITparGeo::GetBaseName());
    m_TopVolume->AddNode(itVol, 4, transfo);

  }

  // MSD
  if (GlobalPar::GetPar()->IncludeMSD()) {
    TGeoVolume* msdVol = m_MSD_geo->BuildMultiStripDetector();
    msdVol->SetLineColor(kViolet);
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAMSDparGeo::GetBaseName());
    m_TopVolume->AddNode(msdVol, 5, transfo);

  }

  // TW
  if (GlobalPar::GetPar()->IncludeTW()) {
    TGeoVolume* twVol = m_TW_geo->BuildTofWall();
    twVol->SetLineColor(kBlue);
    TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TATWparGeo::GetBaseName());
    m_TopVolume->AddNode(twVol, 6, transfo);
  }

  // // CA
  // if (GlobalPar::GetPar()->IncludeCA()) {
  //   TACAparGeo* parGeo = fReco->GetParGeoCa();
  //   TGeoVolume* caVol = parGeo->BuildCalorimeter();

  //   TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TACAparGeo::GetBaseName());
  // }



  m_fitter_extrapolation = new KalmanFitter(nIter, dPVal);


  // initialise the kalman method selected from param file
  if ( GlobalPar::GetPar()->KalMode() == 1 )
    m_fitter = new KalmanFitter(nIter, dPVal);
  else if ( GlobalPar::GetPar()->KalMode() == 2 )
    m_refFitter = new KalmanFitterRefTrack(nIter, dPVal);
  else if ( GlobalPar::GetPar()->KalMode() == 3 )
    m_dafRefFitter = new DAF(true, nIter, dPVal);
  else if ( GlobalPar::GetPar()->KalMode() == 4 )
    m_dafSimpleFitter = new DAF(false, nIter, dPVal);

    if (m_IsEDOn)
    InitEventDisplay();

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
   
   outfile = TFile::Open("efficiency.root","RECREATE");
   
   //ofs.open ("test.txt", std::ofstream::out);
   ofs.open ("efficiency.txt", std::ofstream::out);
   
   MSDforwardcounter=0;
}

void KFitter::CreateHistogram()
{
  percentageOfMCTracksVTX = new TH1D("mean number of tracks","mean number of tracks", 10, 0., 5.);
  AddHistogram(percentageOfMCTracksVTX);

  tempPurity = new TH1D("purity", "purity", 20, 0., 1.5);
  AddHistogram(tempPurity);

  ITstudy = new TH1I("it study on checktrack false", "it study", 10, 0, 10);
  AddHistogram(ITstudy);

  MSDstudy = new TH1I("msd study on checktrack false", "msd study", 7, 0, 7);
  AddHistogram(MSDstudy);

  qoverp = new TH1D("qoverp_all", "qoverp_all", 100, 0., 2.);
  AddHistogram(qoverp);

  qoverpsel = new TH1D("qoverp_sel", "qoverp_sel", 100, 0., 2.);
  AddHistogram(qoverpsel);

  for (int i = 0; i < 8; ++i){
     momentum_true[i] = new TH1D(Form("histoChargeTrue%d",i+1), "test", 25, 0.,10.);
     AddHistogram(momentum_true[i]);
     
     momentum_reco[i] = new TH1D(Form("histoChargeReco%d",i+1), "test", 25, 0.,10.);
     AddHistogram(momentum_reco[i]);

     ratio_reco_true[i] = new TH1D(Form("histoRatio%d",i+1), "test", 25, 0.,10.);
     AddHistogram(ratio_reco_true[i]);
  }
}

//____________________________________________________________________________
vector<string>  KFitter::Tokenize(const string str, const string delimiters)
{
  vector<string> tokens;
  
  // stringstream class check1
  stringstream check1(str);
  
  string intermediate;
  
  // Tokenizing w.r.t. delimiters
  while(getline(check1, intermediate, delimiters[0]))
    tokens.push_back(intermediate);
  
  return tokens;
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

//---------------------------------------------------------------
// create detector planes for track finding and fitting
void KFitter::CreateDetectorPlanes() {

  //put variable like a static one to take into account the number of planes created

  int indexOfPlane = 0;

  m_detectorPlanes.clear();

  for ( int i = 0; i < m_VT_geo->GetSensorsN(); ++i ) {
    genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( m_GeoTrafo->FromVTLocalToGlobal(m_VT_geo->GetSensorPosition(i)), TVector3(0,0,1)));
    detectorplane->setU(1.,0.,0.);
    detectorplane->setV(0.,1.,0.);
    m_detectorPlanes[indexOfPlane] = detectorplane;
    ++indexOfPlane;
  }

  for ( int i = 0; i < m_IT_geo->GetSensorsN(); i++ ) {
    TVector3 origin_(0.,0.,m_GeoTrafo->FromITLocalToGlobal(m_IT_geo->GetSensorPosition(i)).Z());
    if ( i == 0 ){
      //WARNING: HARDCODED AHEAD (TO BE REMOVED)
      genfit::AbsFinitePlane* recta = new RectangularFinitePlane( -4.047, 4.012, -3.3918, -1.47 );
      genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, TVector3(0,0,1), recta));
      detectorplane->setU(1.,0.,0.);
      detectorplane->setV(0.,1.,0.);
      m_detectorPlanes[indexOfPlane] = detectorplane;
      ++indexOfPlane;
    }
    else if ( i == 4 ){
      genfit::AbsFinitePlane* recta = new RectangularFinitePlane( -4.047, 4.012, -1.77, 0.15 );
      genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, TVector3(0,0,1), recta));
      detectorplane->setU(1.,0.,0.);
      detectorplane->setV(0.,1.,0.);
      m_detectorPlanes[indexOfPlane] = detectorplane;
      ++indexOfPlane;
    }
    else if ( i == 8 ){
      genfit::AbsFinitePlane* recta = new RectangularFinitePlane( -4.047, 4.012, -0.15, 1.77 );
      genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, TVector3(0,0,1), recta));
      detectorplane->setU(1.,0.,0.);
      detectorplane->setV(0.,1.,0.);
      m_detectorPlanes[indexOfPlane] = detectorplane;
      ++indexOfPlane;
    }
    else if ( i == 12 ){
      genfit::AbsFinitePlane* recta = new RectangularFinitePlane( -4.047, 4.012, 1.47, 3.39 );
      genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, TVector3(0,0,1), recta));
      detectorplane->setU(1.,0.,0.);
      detectorplane->setV(0.,1.,0.);
      m_detectorPlanes[indexOfPlane] = detectorplane;
      ++indexOfPlane;
    }
    else if ( i == 16 ){
      genfit::AbsFinitePlane* recta = new RectangularFinitePlane( -4.047, 4.012, -3.3918, -1.47 );
      genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, TVector3(0,0,1), recta));
      detectorplane->setU(1.,0.,0.);
      detectorplane->setV(0.,1.,0.);
      m_detectorPlanes[indexOfPlane] = detectorplane;
      ++indexOfPlane;
    }
    else if ( i == 20 ){
      genfit::AbsFinitePlane* recta = new RectangularFinitePlane( -4.047, 4.012, -1.77, 0.15 );
      genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, TVector3(0,0,1), recta));
      detectorplane->setU(1.,0.,0.);
      detectorplane->setV(0.,1.,0.);
      m_detectorPlanes[indexOfPlane] = detectorplane;
      ++indexOfPlane;
    }
    else if ( i == 24 ){
      genfit::AbsFinitePlane* recta = new RectangularFinitePlane( -4.047, 4.012, -0.15, 1.77 );
      genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, TVector3(0,0,1), recta));
      detectorplane->setU(1.,0.,0.);
      detectorplane->setV(0.,1.,0.);
      m_detectorPlanes[indexOfPlane] = detectorplane;
      ++indexOfPlane;
    }
    else if ( i == 28 ){
      genfit::AbsFinitePlane* recta = new RectangularFinitePlane( -4.047, 4.012, 1.47, 3.39 );
      genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, TVector3(0,0,1), recta));
      detectorplane->setU(1.,0.,0.);
      detectorplane->setV(0.,1.,0.);
      m_detectorPlanes[indexOfPlane] = detectorplane;
      ++indexOfPlane;
    }

  }

  for ( int i = 0; i < m_MSD_geo->GetSensorsN(); i++ ) {
    genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( m_GeoTrafo->FromMSDLocalToGlobal(m_MSD_geo->GetSensorPosition(i)), TVector3(0,0,1)));
    detectorplane->setU(1.,0.,0.);
    detectorplane->setV(0.,1.,0.);
    m_detectorPlanes[indexOfPlane] = detectorplane;
    ++indexOfPlane;
  }

  TVector3 origin_(0.,0.,99.7);

  //genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, TVector3(0,0,1)));
  genfit::SharedPlanePtr detectorplane (new genfit::DetPlane(m_GeoTrafo->FromTWLocalToGlobal(m_TW_geo->GetLayerPosition(1)), TVector3(0,0,1)));
  detectorplane->setU(1.,0.,0.);
  detectorplane->setV(0.,1.,0.);
  m_detectorPlanes[indexOfPlane] = detectorplane;
  ++indexOfPlane;


  // for ( map< int, genfit::SharedPlanePtr >::iterator planeIt = m_detectorPlanes.begin(); planeIt != m_detectorPlanes.end(); planeIt++ ){
  //   cout << " planeId: " << (*planeIt).first << "   ";
  //   (*planeIt).second->Print();
  // }
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

      //TVector3 prova = m_GeoTrafo->FromVTLocalToGlobal( clus->GetPositionG() );
      m_VT_clusCollection.push_back(clus);


      //---------------------------testing access to hits and pixels from cluster--------------------

      //other way using new method in TAVTbaseCluster, still developing
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
            printf("charge %d mass %g index %d \n", track->GetCharge(), track->GetMass(), idx);
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
  TAITparGeo* vtxGeo = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetDefParaName(), "TAITparGeo")->Object();

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
            printf("charge %d mass %g index %d \n", track->GetCharge(), track->GetMass(), idx);
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


//-------------------------------------------------------------------------------
// upload points from microstrip
int KFitter::UploadHitsMSD() {

  //map for m_MCInfo
  map<int, MCTruthInfo> MCMSDInfo;

  TAMSDntuPoint* ntuP = (TAMSDntuPoint*) gTAGroot->FindDataDsc("msdPoint", "TAMSDntuPoint")->Object();
  TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuEve")->Object();
  TAMCntuHit* msdMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("msdMc", "TAMCntuHit")->Object();

  int totPoints = 0;
  Int_t nPlanes = m_MSD_geo->GetSensorsN()/2;

  for(Int_t iPlane = 0; iPlane < nPlanes; iPlane++){

    Int_t npoints = ntuP->GetPointN(iPlane);
    if (m_debug > 1)
    std::cout << "\nfound " << npoints << " in station " << iPlane << std::endl;
    totPoints+=npoints;
    if (npoints == 0) continue;

    for (int iPoint = 0; iPoint < npoints; iPoint++) {
      if (m_debug > 1)
      std::cout << "entered cycle pointsMSD of plane " << iPlane << std::endl;
      TAMSDpoint* point = (TAMSDpoint*) ntuP->GetPoint(iPlane, iPoint);
      TVector3 pointPosition = point->GetPosition();
      bool isPointGhost = point->IsTrueGhost();
      int genParticle = point->GetGenPartID();

      TVector3 prova = m_GeoTrafo->FromMSDLocalToGlobal( pointPosition );
      if (m_debug > 1)
      prova.Print();

      m_MSD_pointCollection.push_back(point);

      if (m_debug > 1)
      cout << "IsTrueGhost " << isPointGhost << " genPartID is " << genParticle << " " << endl;

      int idx    = point->GetGenPartID();
      int idRow  = point->GetRowMCHitID();
      int idCol  = point->GetColumnMCHitID();
      TAMChit* mcRowHit = msdMc->GetHit(idRow);
      TAMChit* mcColHit = msdMc->GetHit(idCol);
      //Int_t checkTrackId = mcHit->GetTrackId();
      //if ( checkTrackId != idx ) { cout << "WARNING: TRACKID DOES NOT MATCH!" << endl; continue; }
      TVector3 posin = (mcRowHit->GetInPosition() + mcRowHit->GetOutPosition())*.5;
      TVector3 posout = (mcColHit->GetInPosition() + mcColHit->GetOutPosition())*.5;
      if (m_debug > 1)
      cout << "idx  idRow   idCol : " << idx << " " << idRow << " " << idCol << " "  << endl;
      if (idx > -1 ){
        TAMCeveTrack* track = eve->GetTrack(idx);
        if (m_debug > 1)
        printf("charge %d mass %g index %d \n", track->GetCharge(), track->GetMass(), idx);
        TVector3 momin = (mcRowHit->GetInMomentum() + mcRowHit->GetOutMomentum())*.5;
        TVector3 momout = (mcColHit->GetInMomentum() + mcColHit->GetOutMomentum())*.5;
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
        MCMSDInfo[m_MSD_pointCollection.size()-1] = MSDInfo;
      } else {
        MCTruthInfo MSDInfo;
        MSDInfo.MCTrackId = idx;
        MSDInfo.MCFlukaId = -999.;
        MSDInfo.MCMass = -999.;
        MSDInfo.MCCharge = -999.;
        MSDInfo.MCGenPosition.SetXYZ(-999., -999, -999);
        MSDInfo.MCGenMomentum.SetXYZ(-999., -999, -999);
        MSDInfo.MCPosition = (posin + posout)*.5;
        MSDInfo.MCMomentum.SetXYZ(-999., -999, -999);
        MCMSDInfo[m_MSD_pointCollection.size()-1] = MSDInfo;
      }
    }
  }

  m_MCInfo[m_detectorID_map["MSD"]] = MCMSDInfo;

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

      if (m_debug > 1){
        cout << "in clusMSD check plane number " << clus->GetSensorIdx() << " and view " << clus->GetPlaneView() << endl;
        prova.Print();

    }
      m_MSD_clusCollection.push_back(clus);


      //---------------------------testing access to hits and pixels from cluster--------------------
      Int_t nHits = clus->GetStripsN();

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
            printf("charge %d mass %g index %d \n", track->GetCharge(), track->GetMass(), idx);
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

//----------------------------------------------------------------------------------------------------
// upload measurement points from Scintillator TofWall
int KFitter::UploadHitsTW() {

  //map for m_MCInfo
  map<int, MCTruthInfo> MCTWInfo;


  // take the ntuple object already filled
  TATWntuPoint* ntup = (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint", "TATWntuPoint")->Object();
  if ( m_debug > 0 )		cout << "number of TW points read: " << ntup->GetPointN() << endl;

  TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuEve")->Object();
  TAMCntuHit* twMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("twMc", "TAMCntuHit")->Object();

  int totPoints = 0;
  totPoints = ntup->GetPointN();

  // save hits in the collection
  for (int iPoint = 0; iPoint < totPoints; iPoint++) {
    TATWpoint* point = ntup->GetPoint(iPoint);

    //HACK to prevent fake points (try)
    if (point->GetTime() > 15.) continue;
    //if (point->GetMcTracksN() == 0) continue;
    //if ( point->GetChargeZ() < 1 ) continue; // wrong association, maybe neutron from calo

    if ( GetTWTrackFixed(point) == -1 ) continue;

    //TVector3 prova = m_GeoTrafo->FromTWLocalToGlobal(point->GetPositionG());
    //ofs << "TVector prova " << prova.X() << " " << prova.Y() << " " << prova.Z() << endl;
    //ofs << " CHARGE from algo " << point->GetChargeZ() << endl;

    m_TW_hitCollection.push_back( point );

    //get MC info without calling TAMChit
    Int_t idx = GetTWTrackFixed( point );
    //test access to rowhit of TW point to avoid -1
    //TATWntuHit* testRowHit = 0x0;
    //TATWntuHit* testColHit = 0x0;
    //testRowHit = point->GetRowHit();
    //testColHit = point->GetColumnHit();
    //if (!testRowHit) {ofs << " !testRowHit " <<endl; continue;}
    //if (!testColHit) {ofs << " !testColHit " <<endl; continue;}
    //if ( testRowHit->GetMcTracksN() == 0 ) idx = -1;
    //else idx = testRowHit->GetMcTrackIdx(0);
    //if ( testRowHit->GetMcTracksN() > 1 ) ofs << "multiple mc track index in row " << endl;
    //if ( testColHit->GetMcTracksN() > 1 ) ofs << "multiple mc track index in col " << endl;

    //if ( point->GetMcTracksN() == 0 ) idx = -1;
    //else idx = point->GetMcTrackIdx(0);
    // for (int iM = 0; iM < testRowHit->GetMcTracksN(); ++iM){
    //   ofs << " idx " << testRowHit->GetMcTrackIdx(iM) ;
    // }
    // ofs << endl;
    // for (int iC = 0; iC < testColHit->GetMcTracksN(); ++iC){
    //   ofs << " idxC " << testColHit->GetMcTrackIdx(iC) ;
    // }
    // ofs << endl;
    //
    if ( idx > -1 ){
      TAMCeveTrack* track = eve->GetTrack(idx);
      if (m_debug > 1)
      printf("charge %d mass %g index %d \n", track->GetCharge(), track->GetMass(), idx);
      MCTruthInfo TWInfo;
      TWInfo.MCTrackId = idx;
      TWInfo.MCFlukaId = track->GetFlukaID();
      TWInfo.MCMass = track->GetMass();
      TWInfo.MCCharge = track->GetCharge();
      TWInfo.MCGenPosition = track->GetInitPos();
      TWInfo.MCGenMomentum = track->GetInitP();
      TWInfo.MCPosition.SetXYZ(-999., -999, -999);//not interested in MCpos and mom, TW not used for tracking
      TWInfo.MCMomentum.SetXYZ(-999., -999, -999);
      MCTWInfo[m_TW_hitCollection.size()-1] = TWInfo;
    }
    else{
      MCTruthInfo TWInfo;
      TWInfo.MCTrackId = idx;
      TWInfo.MCFlukaId = -999.;
      TWInfo.MCMass = -999.;
      TWInfo.MCCharge = -999.;
      TWInfo.MCGenPosition.SetXYZ(-999., -999, -999);
      TWInfo.MCGenMomentum.SetXYZ(-999., -999, -999);
      TWInfo.MCPosition.SetXYZ(-999., -999, -999);//not interested in MCpos and mom, TW not used for tracking
      TWInfo.MCMomentum.SetXYZ(-999., -999, -999);
      MCTWInfo[m_TW_hitCollection.size()-1] = TWInfo;
    }
  }

  m_MCInfo[m_detectorID_map["TW"]] = MCTWInfo;
  return totPoints;
}


//HACK FOR TW POINT HANDLING
int KFitter::GetTWTrackFixed (TATWpoint* pointToCheck){
  int indexSize = pointToCheck->GetMcTracksN();
  if ( indexSize == 1 ){//in this case no pb
    int index = pointToCheck->GetMcTrackIdx(0);
    if ( index != -1 ) return index;
  }
  TATWntuHit* rowHit = pointToCheck->GetRowHit();
  TATWntuHit* colHit = pointToCheck->GetColumnHit();
  int matching = -1;
  for (int iRow = 0; iRow < rowHit->GetMcTracksN(); ++iRow){
    for (int iCol = 0; iCol < colHit->GetMcTracksN(); ++iCol){
      if (rowHit->GetMcTrackIdx(iRow) == colHit->GetMcTrackIdx(iCol))
      matching = rowHit->GetMcTrackIdx(iRow);
    }
  }
  return matching;
}


//----------------------------------------------------------------------------------------------------
// pack together the hits to be fitted, from all the detectors, selct different preselecion m_systemsONs
int KFitter::PrepareData4Fit_dataLike() {

  if ( m_debug > 0 )		cout << "\n\n*******\tKFitter::PrepareData4FitDataLike\t*******\n" << endl;


  m_allHitsInMeasurementFormat.clear();
  // Inner Tracker -  fill fitter collections
  if ( (m_systemsON == "all" || m_systemsON.find( "IT" ) != string::npos) && GlobalPar::GetPar()->IncludeIT() ) {
    //UploadHitsIT();
    UploadClusIT();
    if ( m_debug > 0 )		cout <<endl<<endl << "Filling inner detector hit collection = " << m_IT_clusCollection.size() << endl;
    Prepare4InnerTracker();
  }

  // MSD -  fill fitter collections
  if ( (m_systemsON == "all" || m_systemsON.find( "MSD" ) != string::npos) && GlobalPar::GetPar()->IncludeMSD() ) {
    UploadClusMSD();
    //UploadHitsMSD();
    if ( m_debug > 0 )		cout << endl<<endl << "Filling Strip hit collection = " << m_MSD_pointCollection.size() << endl;
    Prepare4Strip();
  }

  // Tof Wall-  fill fitter collections
  if ( ( m_systemsON.find( "TW" ) != string::npos) && GlobalPar::GetPar()->IncludeTW() ) {
    UploadHitsTW();
    if ( m_debug > 0 )		cout <<endl<<endl << "Filling scintillator hit collection = " << m_TW_hitCollection.size() << endl;
    Prepare4TofWall();
  }

	// if ( m_systemsON != "all" || !( m_systemsON.find( "VT" ) != string::npos && m_systemsON.find( "VT" ) != string::npos && m_systemsON.find( "VT" ) != string::npos ) )
	// 	return 0;

  TAVTntuVertex* vertexContainer = (TAVTntuVertex*) gTAGroot->FindDataDsc("vtVtx", "TAVTntuVertex")->Object();
  int vertexNumber = vertexContainer->GetVertexN();
  TAVTvertex* vtxPD   = 0x0; //NEW


  TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuEve")->Object();

  //ofs << "EVENTO " << m_evNum << " with vertices number " << vertexNumber << endl;
  //ofs << "TW found " << m_TW_hitCollection.size() << " points " << endl;

  //fill std::set for trackRep
  std::set<int> partHypo;
  bool areLightFragments = false;

  //fill m_nTotTracks
  for (unsigned int iTW = 0; iTW < m_TW_hitCollection.size(); ++iTW){
    TATWpoint* pointTofWall = m_TW_hitCollection.at(iTW);
    //ofs << "CHARGE seen by TW " << pointTofWall->GetChargeZ() << endl;
    //ofs << "TIME seen by TW " << pointTofWall->GetTime() << endl;
    if ( pointTofWall->GetChargeZ() > -1) partHypo.insert(pointTofWall->GetChargeZ());

    //if (pointTofWall->GetMcTracksN()==0) {ofs << "dont know which track is " << endl; continue;}
    int mcTrackid = GetTWTrackFixed(pointTofWall);
    TAMCeveTrack* montecarlotrack = eve->GetTrack(mcTrackid);
    if ( fabs (montecarlotrack->GetInitPos().Z() ) < 0.1 ){
      //ofs << "track " << mcTrackid <<  " begins in the target" << endl;
      string nameParticle = "NONE";
      // if ( montecarlotrack->GetCharge() == 1 ) nameParticle = "H";
      // else if ( montecarlotrack->GetCharge() == 2 ) nameParticle = "He";
      // else if ( montecarlotrack->GetCharge() == 3 ) nameParticle = "Li";
      // else if ( montecarlotrack->GetCharge() == 4 ) nameParticle = "Be";
      // else if ( montecarlotrack->GetCharge() == 5 ) nameParticle = "B";
      // else if ( montecarlotrack->GetCharge() == 6 ) nameParticle = "C";
      // else if ( montecarlotrack->GetCharge() == 7 ) nameParticle = "N";
      // else if ( montecarlotrack->GetCharge() == 8 ) nameParticle = "O";
      if ( pointTofWall->GetChargeZ() == 1 ) nameParticle = "H";
      else if ( pointTofWall->GetChargeZ() == 2 ) nameParticle = "He";
      else if ( pointTofWall->GetChargeZ() == 3 ) nameParticle = "Li";
      else if ( pointTofWall->GetChargeZ() == 4 ) nameParticle = "Be";
      else if ( pointTofWall->GetChargeZ() == 5 ) nameParticle = "B";
      else if ( pointTofWall->GetChargeZ() == 6 ) nameParticle = "C";
      else if ( pointTofWall->GetChargeZ() == 7 ) nameParticle = "N";
      else if ( pointTofWall->GetChargeZ() == 8 ) nameParticle = "O";
      if (nameParticle == "NONE") continue;
      if ( m_nTotTracks.find( nameParticle ) == m_nTotTracks.end() )	m_nTotTracks[ nameParticle ] = 0;
      m_nTotTracks[ nameParticle ]++;

      momentum_true[pointTofWall->GetChargeZ() -1]->Fill(montecarlotrack->GetInitP().Mag());

    }
  }


  for (Int_t iVtx = 0; iVtx < vertexNumber; ++iVtx) {
    vtxPD = vertexContainer->GetVertex(iVtx);
    if (vtxPD == 0x0){
      //ofs << "Vertex number " << iVtx << " seems to be empty" << endl;
      continue;
    }

    //ofs << "vertex numebr " << iVtx << " has this nr of tracks " << vtxPD->GetTracksN() <<endl;

    //loop over tracks
    for (int iTrack = 0; iTrack < vtxPD->GetTracksN(); iTrack++) {

      TAVTtrack* track = vtxPD->GetTrack( iTrack );
      int montecarloTrackIndex = -1;
      int montecarloCharge = -1;
      double montecarloMomentum = -1.;
      double montecarloMass = -1.;
      TVector3 montecarloFinalPos(-1., -1., -1.);
      TVector3 montecarloInitPos(-1., -1., -1.);

      // N clusters per track
      int ncluster = track->GetClustersN();
      //ofs << "Track number " << iTrack << " has this number of clusters " << ncluster << endl;

      // loop over clusters in the track  get clusters in track
      for (int iCluster = 0; iCluster < ncluster; iCluster++) {

        TAVTcluster* clus = (TAVTcluster*) track->GetCluster( iCluster );
        if (!clus->IsValid()) continue;
        if (iCluster == 2){
          montecarloTrackIndex = clus->GetMcTrackIdx(0);
          TAMCeveTrack* montecarlotrack = eve->GetTrack(montecarloTrackIndex);
          montecarloCharge = montecarlotrack->GetCharge();
          montecarloMass = montecarlotrack->GetMass();
          montecarloMomentum = montecarlotrack->GetInitP().Mag();
          montecarloFinalPos = montecarlotrack->GetFinalPos();
          montecarloInitPos = montecarlotrack->GetInitPos();
        }


        //UploadClusVT();
        Prepare4Vertex( clus, iTrack, iCluster );				//fill map m_hitCollectionToFit_dataLike

      }
      //ofs << "MONTECARLO CHARGE OF " << montecarloTrackIndex << " IS " << montecarloCharge << endl;
      //ofs << "MONTECARLO INIT POS IS " <<  montecarloInitPos.X() << " " << montecarloInitPos.Y() << " " << montecarloInitPos.Z() << endl;
      //ofs << "MONTECARLO FINAL POS IS " <<  montecarloFinalPos.X() << " " << montecarloFinalPos.Y() << " " << montecarloFinalPos.Z() << endl;

      //percentageOfMCTracksVTX->Fill(MCindexInTrack/ncluster);
      TVector3 pos_(0, 0, 0);	//global coord [cm]
      TVector3 mom_(0, 0, 6.);	//GeV
      Track*  fitTrack_ = new Track();  // container of the tracking objects

      for (std::set<int>::iterator it = partHypo.begin(); it != partHypo.end(); ++it){
        AbsTrackRep* rep = 0x0;
        AbsTrackRep* rep1 = 0x0;
        AbsTrackRep* rep2 = 0x0;

        string nameParticle_ = "NONE";
        if (*it == 1) {nameParticle_ = "H"; areLightFragments = true;}
        else if ( *it == 2 ) {nameParticle_ = "Alpha"; areLightFragments = true;}
        else if ( *it == 3 ) nameParticle_ = "Li6";
        else if ( *it == 4 ) nameParticle_ = "Be10";
        else if ( *it == 5 ) nameParticle_ = "B10";
        else if ( *it == 6) nameParticle_ = "C12";
        else if ( *it == 7) nameParticle_ = "N14";
        else if ( *it == 8) nameParticle_ = "O16";
        if (nameParticle_ != "NONE"){
          rep = new RKTrackRep( (UpdatePDG::GetPDG()->GetPdgCode( nameParticle_ ) ) );
          fitTrack_->addTrackRep( rep );
        }
        if ( nameParticle_ == "H" ){
          rep1 = new RKTrackRep( (UpdatePDG::GetPDG()->GetPdgCode( "H2" ) ) );
          rep2 = new RKTrackRep( (UpdatePDG::GetPDG()->GetPdgCode( "H3" ) ) );
          fitTrack_->addTrackRep( rep1 );
          fitTrack_->addTrackRep( rep2 );

        }
        if ( nameParticle_ == "Alpha" ){
          rep1 = new RKTrackRep( (UpdatePDG::GetPDG()->GetPdgCode( "He3" ) ) );
          fitTrack_->addTrackRep( rep1 );

        }

      }

      //check id there are trackreps otherwise exit
      if ( fitTrack_->getNumReps() == 0 ){
        //ofs << "deleting fitTrack_ no track reps" << endl;
        delete fitTrack_;
        fitTrack_ = nullptr;
        continue;
      }


      fitTrack_->setStateSeed(pos_, mom_);
      fitTrack_->setMcTrackId(montecarloTrackIndex);

      for (unsigned int g = 0; g < m_hitCollectionToFit_dataLike[iTrack].size(); ++g){
        fitTrack_->insertMeasurement( m_hitCollectionToFit_dataLike[iTrack].at(g) );
      }


      // cout << " check of fitTrack_ filling after vertex" << endl;
      // for (unsigned int jTracking = 0; jTracking < fitTrack_->getNumPointsWithMeasurement(); ++jTracking){
      //   fitTrack_->getPointWithMeasurement(jTracking)->getRawMeasurement()->getRawHitCoords().Print();
      //   int indexOfPlane = static_cast<genfit::PlanarMeasurement*>(fitTrack_->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId();
      //   cout << " index of plane " << indexOfPlane << " " << m_detectorPlanes[indexOfPlane]->getO().Z() << endl;
      //}

      // ExtrapFromVTXtoIT
      // calculate slope on-the-fly

      Double_t x, y, dx, dy, z;
      graphErrorX = new TGraphErrors();
      graphErrorX->Set(fitTrack_->getNumPointsWithMeasurement());

      graphErrorY = new TGraphErrors();
      graphErrorY->Set(fitTrack_->getNumPointsWithMeasurement());

      //cout << " number of points is " << fitTrack_->getNumPointsWithMeasurement() << endl;
      if (fitTrack_->getNumPointsWithMeasurement() > 4){
        //ofs << "Warning: cluster in this track are more than four, skip track for now " << endl;
        delete fitTrack_;
        continue;
      }

      for (unsigned int h = 0; h < fitTrack_->getNumPointsWithMeasurement(); ++h){
        x = fitTrack_->getPointWithMeasurement(h)->getRawMeasurement()->getRawHitCoords()(0);
        dx = fitTrack_->getPointWithMeasurement(h)->getRawMeasurement()->getRawHitCov()[0][0];

        y = fitTrack_->getPointWithMeasurement(h)->getRawMeasurement()->getRawHitCoords()(1);
        dy = fitTrack_->getPointWithMeasurement(h)->getRawMeasurement()->getRawHitCov()[1][1];
        int indexOfPlane = static_cast<genfit::PlanarMeasurement*>(fitTrack_->getPointWithMeasurement(h)->getRawMeasurement())->getPlaneId();
        z = m_detectorPlanes[indexOfPlane]->getO().Z();

        graphErrorX->SetPoint(h, z, x);
        graphErrorX->SetPointError(h, 0., sqrt(dx));

        graphErrorY->SetPoint(h, z, y);
        graphErrorY->SetPointError(h, 0., sqrt(dy));

      }

      graphErrorX->Fit("pol1", "Q");
      graphErrorY->Fit("pol1", "Q");

      TF1* polyX = graphErrorX->GetFunction("pol1");
      TF1* polyY = graphErrorY->GetFunction("pol1");

      //double chichiY = polyY->GetChisquare();
      //cout << polyY->GetParameter(0) << "    " << polyY->GetParameter(1) << " " << polyY->GetChisquare() << endl;
      //cout << polyX->GetParameter(0) << "    " << polyX->GetParameter(1) << " " << polyX->GetChisquare() << endl;

      double firstGuessYOnIT = polyY->Eval(m_detectorPlanes[4]->getO().Z());
      double secondGuessYOnIT = polyY->Eval(m_detectorPlanes[8]->getO().Z());
      double thirdGuessYOnIT = polyY->Eval(m_detectorPlanes[5]->getO().Z());
      double fourthGuessYOnIT = polyY->Eval(m_detectorPlanes[9]->getO().Z());

      double firstGuessXOnIT = polyX->Eval(m_detectorPlanes[4]->getO().Z());



      //cout << "firstGuessXOnIT " << firstGuessXOnIT << " firstGuessYOnIT " << firstGuessYOnIT << endl;
      delete graphErrorX;
      delete graphErrorY;

      //track is too much titled in Y, exit from detector
      if ( firstGuessYOnIT < -3.39 || firstGuessYOnIT > 3.39 ){
        //ofs << "MyTrackFinding: track" << iTrack << "exits in Y from spectrometer, go to next track" << endl;
        delete fitTrack_;
        fitTrack_ = nullptr;
        continue;
      }

      if ( firstGuessXOnIT < -4.5 || firstGuessXOnIT > 4.5){
        //ofs << "MyTrackFinding: track" << iTrack << "exits in X from spectrometer, go to next track" << endl;
        delete fitTrack_;
        fitTrack_ = nullptr;
        continue;
      }

      if ( !m_detectorPlanes[4]->isInActiveX(firstGuessXOnIT) ){
        //ofs << "MyTrackFinding: track" << iTrack << "exits in X from spectrometer, go to next track" << endl;
        delete fitTrack_;
        fitTrack_ = nullptr;
        continue;
      }

      bool isInPlane4 = m_detectorPlanes[4]->isInActiveY(firstGuessYOnIT + .1) || m_detectorPlanes[4]->isInActiveY(firstGuessYOnIT - .1);
      bool isInPlane5 = m_detectorPlanes[5]->isInActiveY(thirdGuessYOnIT + .1) || m_detectorPlanes[5]->isInActiveY(thirdGuessYOnIT - .1);
      bool isInPlane6 = m_detectorPlanes[6]->isInActiveY(firstGuessYOnIT + .1) || m_detectorPlanes[6]->isInActiveY(firstGuessYOnIT - .1);
      bool isInPlane7 = m_detectorPlanes[7]->isInActiveY(thirdGuessYOnIT + .1) || m_detectorPlanes[7]->isInActiveY(thirdGuessYOnIT - .1);
      bool isInPlane8 = m_detectorPlanes[8]->isInActiveY(secondGuessYOnIT + .1) || m_detectorPlanes[8]->isInActiveY(secondGuessYOnIT - .1);
      bool isInPlane9 = m_detectorPlanes[9]->isInActiveY(fourthGuessYOnIT + .1) || m_detectorPlanes[9]->isInActiveY(fourthGuessYOnIT - .1);
      bool isInPlane10 = m_detectorPlanes[10]->isInActiveY(secondGuessYOnIT + .1) || m_detectorPlanes[10]->isInActiveY(secondGuessYOnIT - .1);
      bool isInPlane11 = m_detectorPlanes[11]->isInActiveY(fourthGuessYOnIT + .1) || m_detectorPlanes[11]->isInActiveY(fourthGuessYOnIT - .1);


      // bool isInPlane4 = m_detectorPlanes[4]->isInActiveY(firstGuessYOnIT);
      // bool isInPlane5 = m_detectorPlanes[5]->isInActiveY(thirdGuessYOnIT);
      // bool isInPlane6 = m_detectorPlanes[6]->isInActiveY(firstGuessYOnIT);
      // bool isInPlane7 = m_detectorPlanes[7]->isInActiveY(thirdGuessYOnIT);
      // bool isInPlane8 = m_detectorPlanes[8]->isInActiveY(secondGuessYOnIT);
      // bool isInPlane9 = m_detectorPlanes[9]->isInActiveY(fourthGuessYOnIT);
      // bool isInPlane10 = m_detectorPlanes[10]->isInActiveY(secondGuessYOnIT);
      // bool isInPlane11 = m_detectorPlanes[11]->isInActiveY(fourthGuessYOnIT);

      vector<bool> crossPlane;
      map <unsigned int, double> guessOnPlane;

      guessOnPlane.clear();
      crossPlane.clear();

      crossPlane.push_back(isInPlane4);
      crossPlane.push_back(isInPlane5);
      crossPlane.push_back(isInPlane6);
      crossPlane.push_back(isInPlane7);
      crossPlane.push_back(isInPlane8);
      crossPlane.push_back(isInPlane9);
      crossPlane.push_back(isInPlane10);
      crossPlane.push_back(isInPlane11);

      guessOnPlane[0] = firstGuessYOnIT;
      guessOnPlane[1] = thirdGuessYOnIT;
      guessOnPlane[2] = firstGuessYOnIT;
      guessOnPlane[3] = thirdGuessYOnIT;
      guessOnPlane[4] = secondGuessYOnIT;
      guessOnPlane[5] = fourthGuessYOnIT;
      guessOnPlane[6] = secondGuessYOnIT;
      guessOnPlane[7] = fourthGuessYOnIT;



      for (unsigned int iPlaneBool = 0; iPlaneBool < crossPlane.size(); ++iPlaneBool){
        if ( crossPlane.at(iPlaneBool) ){
          int searchingPlane = iPlaneBool + 4;
          int indexOfMinY = -1.;
          double distanceInY = .05;
          if (areLightFragments) distanceInY = .5;
          AbsMeasurement* hitToAdd = 0x0;
          do {
            for ( unsigned int jMeas = 0; jMeas < m_allHitsInMeasurementFormat.size(); ++jMeas ){
              PlanarMeasurement* hitHit = 0x0;
              hitHit = static_cast<genfit::PlanarMeasurement*> (m_allHitsInMeasurementFormat.at(jMeas));
              if (hitHit == 0x0) continue;
              //cout << "hitHit->getDetId() == " <<hitHit->getDetId() << "hitHit->getHitId()" << hitHit->getHitId() << endl;
              int indexOfPlane = hitHit->getPlaneId();
              if (hitHit->getPlaneId() == searchingPlane){
                //cout << "hitHit->getPlaneId() == " << searchingPlane << endl;
                if ( fabs(firstGuessYOnIT - hitHit->getRawHitCoords()(1) ) < distanceInY ){
                  distanceInY = fabs(guessOnPlane[iPlaneBool] - hitHit->getRawHitCoords()(1));
                  indexOfMinY = jMeas;
                }
              }
              //cout << " distanceInY " << distanceInY << " indexOfMinY " << indexOfMinY <<endl;
            }
            distanceInY += .05;
          }
          while (indexOfMinY == -1 && distanceInY < .7);
          if (indexOfMinY != -1){
            hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitsInMeasurementFormat.at(indexOfMinY)))->clone();
            fitTrack_->insertMeasurement( hitToAdd );
          }

        }
      }

      //      cout << " check of fitTrack_ filling " << endl;
      //      for (unsigned int jTracking = 0; jTracking < fitTrack_->getNumPointsWithMeasurement(); ++jTracking){
      //        fitTrack_->getPointWithMeasurement(jTracking)->getRawMeasurement()->getRawHitCoords().Print();
      //        int indexOfPlane = static_cast<genfit::PlanarMeasurement*>(fitTrack_->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId();
      //        cout << " index of plane " << indexOfPlane << " " << m_detectorPlanes[indexOfPlane]->getO().Z() << endl;
      //      }

      // cout << "check of MSD allHitsInMeasurement " << endl;
      // for ( unsigned int jMeas = 0; jMeas < m_allHitsInMeasurementFormat.size(); ++jMeas ){
      //   PlanarMeasurement* hitHit = 0x0;
      //   hitHit = static_cast<genfit::PlanarMeasurement*> (m_allHitsInMeasurementFormat.at(jMeas));
      //   if (hitHit == 0x0) continue;
      //   int indexOfPlane = hitHit->getPlaneId();
      //   if (indexOfPlane > 11){
      //     hitHit->getRawHitCoords().Print();
      //     cout << " indexOfPlane " << indexOfPlane << " isStripV " << hitHit->getStripV() << endl;
      //   }
      // }



      //now call function to extrapolate to MSD

      for (int iMSDPlane = 12; iMSDPlane < 18; ++iMSDPlane){
        int whichMSDPlane = iMSDPlane;
        TVector3 guessOnMSD;
        guessOnMSD =  ExtrapolateToOuterTracker( fitTrack_, whichMSDPlane );

        // cout << "guessOnMSD" << endl;
        // guessOnMSD.Print();

        //delete fake point
        for (unsigned int jTracking = 0; jTracking < fitTrack_->getNumPointsWithMeasurement(); ++jTracking){
          int indexOfPlane = static_cast<genfit::PlanarMeasurement*>(fitTrack_->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId();
          if (indexOfPlane == whichMSDPlane) fitTrack_->deletePoint(jTracking);
        }

        //calculate distance
        double MSDdistance = 1.;
        if (areLightFragments) MSDdistance = 2.;
        int indexOfMinY = -1.;
        AbsMeasurement* hitToAdd = 0x0;
        for ( unsigned int jMeas = 0; jMeas < m_allHitsInMeasurementFormat.size(); ++jMeas ){
          PlanarMeasurement* hitHit = 0x0;
          hitHit = static_cast<genfit::PlanarMeasurement*> (m_allHitsInMeasurementFormat.at(jMeas));
          if (hitHit == 0x0) continue;
          int indexOfPlane = hitHit->getPlaneId();
          if (hitHit->getPlaneId() == whichMSDPlane){
            //double distanceFromHit = sqrt( ( guessOnMSD.X() - hitHit->getRawHitCoords()(0) )*( guessOnMSD.X() - hitHit->getRawHitCoords()(0) ) + ( guessOnMSD.Y() - hitHit->getRawHitCoords()(1) )*( guessOnMSD.Y() - hitHit->getRawHitCoords()(1) ) );
            double distanceFromHit = 0.;
            if ( !hitHit->getStripV() )
            distanceFromHit = fabs(guessOnMSD.X() - hitHit->getRawHitCoords()(0));
            else distanceFromHit = fabs(guessOnMSD.Y() - hitHit->getRawHitCoords()(0));
            if ( distanceFromHit < MSDdistance ){
              MSDdistance = distanceFromHit;
              indexOfMinY = jMeas;
            }
          }
        }
        if (indexOfMinY != -1){
          hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitsInMeasurementFormat.at(indexOfMinY)))->clone();
          fitTrack_->insertMeasurement( hitToAdd );
        }

      }

      TVector3 guessOnTW;
      guessOnTW =  ExtrapolateToTofWall( fitTrack_ );
      //ofs << "guessOnTW " << guessOnTW.X() << "  " << guessOnTW.Y() << endl;

      //delete fake point
      for (unsigned int jTracking = 0; jTracking < fitTrack_->getNumPointsWithMeasurement(); ++jTracking){
        int indexOfPlane = static_cast<genfit::PlanarMeasurement*>(fitTrack_->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId();
        if (indexOfPlane == 18) fitTrack_->deletePoint(jTracking);
      }

      //calculate distance TW point
      double TWdistance = 4.;
      int indexOfMin = -1.;
      AbsMeasurement* hitToAdd = 0x0;
      for ( unsigned int jMeas = 0; jMeas < m_allHitsInMeasurementFormat.size(); ++jMeas ){
        PlanarMeasurement* hitHit = 0x0;
        hitHit = static_cast<genfit::PlanarMeasurement*> (m_allHitsInMeasurementFormat.at(jMeas));
        if (hitHit == 0x0) continue;
        int indexOfPlane = hitHit->getPlaneId();
        if (hitHit->getPlaneId() == 18){
          double distanceFromHit = sqrt( ( guessOnTW.X() - hitHit->getRawHitCoords()(0) )*( guessOnTW.X() - hitHit->getRawHitCoords()(0) ) + ( guessOnTW.Y() - hitHit->getRawHitCoords()(1) )*( guessOnTW.Y() - hitHit->getRawHitCoords()(1) ) );
          if ( distanceFromHit < TWdistance ){
            TWdistance = distanceFromHit;
            indexOfMin = jMeas;
          }
        }
      }
      if (indexOfMin != -1){
        hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitsInMeasurementFormat.at(indexOfMin)))->clone();
        fitTrack_->insertMeasurement( hitToAdd );
      }

      int chargeFromTW = GetChargeFromTW(fitTrack_);

      string cardinal = "NONE";
      if (chargeFromTW == 1) cardinal = "H";
      else if ( chargeFromTW == 2 ) cardinal = "Alpha";
      else if ( chargeFromTW == 3 ) cardinal = "Li6";
      else if ( chargeFromTW == 4 ) cardinal = "Be10";
      else if ( chargeFromTW == 5 ) cardinal = "B10";
      else if ( chargeFromTW == 6 ) cardinal = "C12";
      else if ( chargeFromTW == 7 ) cardinal = "N14";
      else if ( chargeFromTW == 8 ) cardinal = "O16";
      else if ( chargeFromTW == -1 ) {
        //ofs << "deleting fitTrack_ no valid point on TW found" << endl;
        delete fitTrack_;
        fitTrack_ = nullptr;
        continue;
      }
      if ( cardinal == "NONE" ) {
        //ofs << "deleting fitTrack_ no valid point on TW found bis" << endl;
        delete fitTrack_;
        fitTrack_ = nullptr;
        continue;
      }

      //here I set cardinalrep by chargeFromTW
      int repId = -99;
      for ( unsigned int iRep = 0; iRep < fitTrack_->getNumReps(); iRep++ ){
        AbsTrackRep* tempTrackRep = 0x0;
        tempTrackRep = fitTrack_->getTrackRep(iRep);
        double minChi2 = 99999999;
        //cout << " temptrackrep " << tempTrackRep->getPDGCharge() <<endl;
        if (!tempTrackRep) continue;
        //if (tempTrackRep->getPDG() == UpdatePDG::GetPDG()->GetPdgCode( cardinal ) )
        if ( tempTrackRep->getPDGCharge() == (double)chargeFromTW ){
          if (fitTrack_->hasFitStatus(tempTrackRep) ){
            if (fitTrack_->getFitStatus(tempTrackRep)->getChi2() < minChi2){
              repId = iRep;
            }
          }
        }
      }
      fitTrack_->setCardinalRep(repId);
      //cout << "PDG charge " << fitTrack_->getCardinalRep()->getPDGCharge() << " " << chargeFromTW <<endl;


      //TRY TO SEE IF THERE IS INNER TRACKER MEASUREMENTS
      //fitTrack_->determineCardinalRep();
      //cout << fitTrack_->getCardinalRep()->getPDGCharge() << endl;
      // cout << " check of fitTrack_ filling after deleting fakeMSD and inserting one" << endl;
      for (unsigned int jTracking = 0; jTracking < fitTrack_->getNumPointsWithMeasurement(); ++jTracking){
        fitTrack_->getPointWithMeasurement(jTracking)->getRawMeasurement()->getRawHitCoords().Print();
        int indexOfPlane = static_cast<genfit::PlanarMeasurement*>(fitTrack_->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId();
        //   cout << " index of plane " << indexOfPlane << " " << m_detectorPlanes[indexOfPlane]->getO().Z() << " isStripV " << isStripV << endl;
      }

      //now call check on track finding
      bool checkTrack = CheckTrackFinding(fitTrack_, montecarloCharge, montecarloMomentum, montecarloMass, chargeFromTW);

      if (checkTrack){
        //ofs << "adding fitTrack_ to vector track" << endl;
        m_vectorTrack.push_back(fitTrack_);
        momentum_reco[chargeFromTW-1]->Fill(montecarloMomentum);
        RecordTrackInfoDataLike(fitTrack_, chargeFromTW, cardinal);
      }

      else{
        int howManyWithIT = 0;
        int howManyWithMSD = 0;
        for (unsigned int jTrackin = 0; jTrackin < fitTrack_->getNumPointsWithMeasurement(); ++jTrackin){
          int indexOfPlane = static_cast<genfit::PlanarMeasurement*>(fitTrack_->getPointWithMeasurement(jTrackin)->getRawMeasurement())->getPlaneId();
          if (indexOfPlane > 3 && indexOfPlane < 12) howManyWithIT++;
          if (indexOfPlane > 11 && indexOfPlane < 18) howManyWithMSD++;

        }

        ITstudy->Fill(howManyWithIT);
        MSDstudy->Fill(howManyWithMSD);

      }
      //delete fitTrack_;
      //fitTrack_ = nullptr;

    } //end track loop
    m_hitCollectionToFit_dataLike.clear();
  } //end vertex loop

  if (m_IsEDOn){
    display->addEvent(m_vectorTrack);
    //cout << "display->addEvent size  " << m_vectorTrack.size() << endl;
  }

  for (unsigned int iVectorTrack = 0; iVectorTrack < m_vectorTrack.size(); ++iVectorTrack){
    delete m_vectorTrack.at(iVectorTrack);
  }
  m_vectorTrack.clear();
  //occhio che sfondo la memoria MEMORY LEAK

  return 0;
}


void KFitter::RecordTrackInfoDataLike( Track* trackToRecord, int tCharge, string particlename ){


  //interested in first layer of vertex for now
  int iStation = 0;
  bool isFirstLayer = false;
  int numberplane = 0;

  for (unsigned int jTracking = 0; jTracking < trackToRecord->getNumPointsWithMeasurement(); ++jTracking){
    iStation = jTracking;
    numberplane = static_cast<genfit::PlanarMeasurement*>(trackToRecord->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId();
    if (numberplane == 0) {isFirstLayer = true; break;}
  }

  if(!isFirstLayer){
    for (unsigned int jTracking = 0; jTracking < trackToRecord->getNumPointsWithMeasurement(); ++jTracking){
      iStation = jTracking;
      numberplane = static_cast<genfit::PlanarMeasurement*>(trackToRecord->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId();
      if (numberplane == 1) break;
    }
  }

  int detID = trackToRecord->getPointWithMeasurement(iStation)->getRawMeasurement()->getDetId();
  int hitID = trackToRecord->getPointWithMeasurement(iStation)->getRawMeasurement()->getHitId();

  //cout << " Detector Type (in KFitter::GetTrueMCInfo)= " << detID << " HitID = " << hitID << endl;


  // take kinematic variables to be plotted
  TVector3 tmpPos, tmpMom;
  double tmp_mass = -666;
  TVector3 tmp_genPos;
  TVector3 tmp_genMom;
  TVector3 hitPos;


  // Generated position and momentum at particle birth
  tmp_mass = m_MCInfo[detID][hitID].MCMass;
  tmp_genPos = m_MCInfo[detID][hitID].MCGenPosition;
  tmp_genMom = m_MCInfo[detID][hitID].MCGenMomentum;

  //MC position and momentum in the hit
  tmpPos = m_MCInfo[detID][hitID].MCPosition;
  tmpMom = m_MCInfo[detID][hitID].MCMomentum;

  genfit::KalmanFittedStateOnPlane prova = GetKalmanTrackInfoDataLike_(iStation, trackToRecord);
  //GetKalmanTrackInfoDataLike ( iStation, trackToRecord, &KalmanState, &KalmanCov );
  const TVectorD& KalmanState =  prova.getState();
  const TMatrixDSym& KalmanCov =  prova.getCov();

  // KalmanState.Print();
  // KalmanCov.Print();


  // cout << KalmanState[0] << " " <<  KalmanState[1] << " " << KalmanState[2] << " " << KalmanState[3] << " " << KalmanState[4] <<endl;
  // cout << tmpMom.X() << " " << tmpMom.Y() << " " << tmpMom.Z() << " " << endl;
  //
  // cout << "momRes " << tCharge/KalmanState[0]-tmpMom.Mag() <<endl;
  // cout << "upRes " << KalmanState[1]-tmpMom.X() <<endl;
  // cout << "vpRes " << KalmanState[2]-tmpMom.Y() <<endl;
  // cout << "uRes " << KalmanState[3]-tmpPos.X() <<endl;
  // cout << "vRes " << KalmanState[4]-tmpPos.Y() <<endl;
  //
  // cout << "upPu " << (KalmanState[1]-tmpMom.X())/sqrt(KalmanCov[1][1]) <<endl;
  // cout << "vpPu " << (KalmanState[2]-tmpMom.Y())/sqrt(KalmanCov[2][2]) <<endl;
  // cout << "uPu " << (KalmanState[3]-tmpPos.X())/sqrt(KalmanCov[3][3]) <<endl;
  // cout << "vPu " << (KalmanState[4]-tmpPos.Y())/sqrt(KalmanCov[4][4]) <<endl;


  m_fitTrackCollection->AddTrack( particlename, trackToRecord, m_evNum, 0,
  KalmanState, KalmanCov, &tmpMom, &tmpPos, tCharge);




}


int KFitter::GetChargeFromTW(Track* trackToCheck){

  int charge = -1;
  for (unsigned int jTracking = 0; jTracking < trackToCheck->getNumPointsWithMeasurement(); ++jTracking){
    if ( static_cast<genfit::PlanarMeasurement*>(trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId() != 18 ) continue;
    int trackHitID = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getHitId();
    TATWpoint* twpoint = m_TW_hitCollection.at(trackHitID);
    charge = twpoint->GetChargeZ();
  }
  return charge;
}

bool KFitter::CheckTrackFinding(Track* trackToCheck, int MCEveCharge, double MCEveMomentum, double MCEveMass, int chargeFromTofWall){
  double numeratorForPurity = 0.;
  int hitsNotVT = 0;

  //chisquare of tracks
  //m_fitter->processTrack(trackToCheck);
  m_refFitter->processTrackWithRep(trackToCheck, trackToCheck->getCardinalRep());
  bool trackHasBackUpdate = true;
  bool trackHasFwdUpdate = true;
  for (unsigned int jTracking = 0; jTracking < trackToCheck->getNumPointsWithMeasurement(); ++jTracking){
    int trackHitID = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getHitId();
    int trackDetID = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getDetId();
    AbsFitterInfo* afi = trackToCheck->getPointWithMeasurement(jTracking)->getFitterInfo(nullptr);
    KalmanFitterInfo* fi = dynamic_cast<KalmanFitterInfo*>(afi);
    if(!fi) {MSDforwardcounter++; cout << "NO UPDATE " << MCEveCharge << " " << MCEveMomentum << " " << MCEveMass << " " << chargeFromTofWall << endl; return false;}
    KalmanFittedStateOnPlane* fup = fi->getForwardUpdate();
    KalmanFittedStateOnPlane* bup = fi->getBackwardUpdate();
    if (fup == nullptr || bup == nullptr) {MSDforwardcounter++; return false;}
    if(trackDetID != m_detectorID_map["VT"]){
      hitsNotVT++;
      int trackIDFromMontecarlo = m_MCInfo[trackDetID][trackHitID].MCTrackId;
      if (trackToCheck->getMcTrackId() == trackIDFromMontecarlo) numeratorForPurity+=1;
    }
  }

  double chisquare = m_refFitter->getRedChiSqu(trackToCheck, trackToCheck->getCardinalRep());

  //double chisquare = m_fitter->getRedChiSqu(trackToCheck, trackToCheck->getCardinalRep());
  qoverp->Fill(MCEveCharge/MCEveMomentum);
  //if (numeratorForPurity/hitsNotVT == 0.) ofs << "OCCHIO ";//{
    //ofs << "GUARDA QUI TRACK ID " << trackToCheck->getMcTrackId() << " CHARGE  " << MCEveCharge << " MOMENTUM  " << MCEveMomentum <<  " MASS " << MCEveMass << " CHI " << chisquare << endl;
    //qoverpsel->Fill(MCEveCharge/MCEveMomentum);
    for (unsigned int jTracking = 0; jTracking < trackToCheck->getNumPointsWithMeasurement(); ++jTracking) {
      int trackHitID = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getHitId();
      int trackDetID = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getDetId();
      if (trackDetID == m_detectorID_map["VT"]) continue;

      int trackIDFromMontecarlo = m_MCInfo[trackDetID][trackHitID].MCTrackId;
      //ofs << " X" << trackDetID << "  " << trackIDFromMontecarlo;
    }
    //ofs << endl;
    if ( chisquare > 0 && chisquare < 10. ) {
      tempPurity->Fill(numeratorForPurity/hitsNotVT);
      string nameParticle = "NONE";
      if ( chargeFromTofWall == 1 ) nameParticle = "H";
      else if ( chargeFromTofWall == 2 ) nameParticle = "He";
      else if ( chargeFromTofWall == 3 ) nameParticle = "Li";
      else if ( chargeFromTofWall == 4 ) nameParticle = "Be";
      else if ( chargeFromTofWall == 5 ) nameParticle = "B";
      else if ( chargeFromTofWall == 6 ) nameParticle = "C";
      else if ( chargeFromTofWall == 7 ) nameParticle = "N";
      else if ( chargeFromTofWall == 8 ) nameParticle = "O";
      if ( m_nConvergedTracks.find( nameParticle ) == m_nConvergedTracks.end() )	m_nConvergedTracks[ nameParticle ] = 0;
      m_nConvergedTracks[ nameParticle ]++;
      //ofs << "insert charge " << nameParticle << endl;
      return true;

    }

//  }
  return false;
}


TVector3 KFitter::ExtrapolateToOuterTracker( Track* trackToFit, int whichPlane ){

  TMatrixDSym planarCov(2);
  TVectorD planarCoords(2);
  planarCoords(0) = 5.;
  planarCoords(1) = 5.;
  double pixReso = GlobalPar::GetPar()->MSDReso();
  planarCov.UnitMatrix();
  for (int k = 0; k < 2; k++){
    planarCov[k][k] = pixReso*pixReso;
  }

  PlanarMeasurement* fakeMSD = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["MSD"], whichPlane-12, nullptr );
  fakeMSD->setPlane(m_detectorPlanes[whichPlane], whichPlane);
  //if ( whichPlane%2 != 0 ) fakeMSD->setStripV();

  trackToFit->insertMeasurement(fakeMSD);
  m_fitter_extrapolation->setMaxIterations(1);
  m_fitter_extrapolation->processTrack(trackToFit);

  double tracklenn = 0.;
  TVector3 posi, momi;
  TMatrixDSym covi;
  int numberplane = 0;
  unsigned int sTracking = 0;
  for (unsigned int jTracking = 0; jTracking < trackToFit->getNumPointsWithMeasurement(); ++jTracking){
    sTracking = jTracking;
    numberplane = static_cast<genfit::PlanarMeasurement*>(trackToFit->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId();
    if (numberplane == whichPlane) break;
  }
  if (static_cast<genfit::KalmanFitterInfo*>(trackToFit->getPointWithMeasurement(sTracking)->getFitterInfo(nullptr))->hasForwardPrediction()){
    static_cast<genfit::KalmanFitterInfo*>(trackToFit->getPointWithMeasurement(sTracking)->getFitterInfo(nullptr))->getForwardPrediction()->getPosMomCov(posi, momi, covi);
  }

  return posi;

}

TVector3 KFitter::ExtrapolateToTofWall( Track* trackToFit ){

  TMatrixDSym planarCov(2);
  TVectorD planarCoords(2);
  planarCoords(0) = 5.;
  planarCoords(1) = 5.;
  double twReso = GlobalPar::GetPar()->TWReso();
  planarCov.UnitMatrix();
  for (int k = 0; k < 2; k++){
    planarCov[k][k] = twReso*twReso;
  }

  PlanarMeasurement* fakeTW = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["TW"], 18, nullptr );
  fakeTW->setPlane(m_detectorPlanes[18], 18);

  trackToFit->insertMeasurement(fakeTW);
  m_fitter_extrapolation->setMaxIterations(1);
  m_fitter_extrapolation->processTrack(trackToFit);

  double tracklenn = 0.;
  TVector3 posi, momi;
  TMatrixDSym covi;
  int numberplane = 0;
  unsigned int sTracking = 0;
  for (unsigned int jTracking = 0; jTracking < trackToFit->getNumPointsWithMeasurement(); ++jTracking){
    sTracking = jTracking;
    numberplane = static_cast<genfit::PlanarMeasurement*>(trackToFit->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId();
    if (numberplane == 18) break;
  }
  if (static_cast<genfit::KalmanFitterInfo*>(trackToFit->getPointWithMeasurement(sTracking)->getFitterInfo(nullptr))->hasForwardPrediction()){
    static_cast<genfit::KalmanFitterInfo*>(trackToFit->getPointWithMeasurement(sTracking)->getFitterInfo(nullptr))->getForwardPrediction()->getPosMomCov(posi, momi, covi);
  }

  return posi;

}



//----------------------------------------------------------------------------------------------------
// pack together the hits to be fitted, from all the detectors, selct different preselecion m_systemsONs
int KFitter::PrepareData4Fit() {

  if ( m_debug > 0 )		cout << "\n\n*******\tKFitter::PrepareData4Fit\t*******\n" << endl;

  // Vertex -  fill fitter collections

  if ( (m_systemsON == "all" || m_systemsON.find( "VT" ) != string::npos) && GlobalPar::GetPar()->IncludeVT() ) {
    UploadClusVT();
    //UploadHitsVT();
    if ( m_debug > 0 )		cout << endl<<endl << "Filling vertex hit collection  = " << m_VT_clusCollection.size() << endl;
    Prepare4Vertex();
  }

  // Inner Tracker -  fill fitter collections
  if ( (m_systemsON == "all" || m_systemsON.find( "IT" ) != string::npos) && GlobalPar::GetPar()->IncludeIT() ) {
    //UploadHitsIT();
    UploadClusIT();
    if ( m_debug > 0 )		cout <<endl<<endl << "Filling inner detector hit collection = " << m_IT_clusCollection.size() << endl;
    Prepare4InnerTracker();
  }

  // MSD -  fill fitter collections
  if ( (m_systemsON == "all" || m_systemsON.find( "MSD" ) != string::npos) && GlobalPar::GetPar()->IncludeMSD() ) {
    UploadClusMSD();
    //UploadHitsMSD();
    if ( m_debug > 0 )		cout << endl<<endl << "Filling Strip hit collection = " << m_MSD_clusCollection.size() << endl;
    Prepare4Strip();
  }

  // Tof Wall-  fill fitter collections
  if ( ( m_systemsON.find( "TW" ) != string::npos) && GlobalPar::GetPar()->IncludeTW() ) {
    UploadHitsTW();
    if ( m_debug > 0 )		cout <<endl<<endl << "Filling scintillator hit collection = " << m_TW_hitCollection.size() << endl;
    Prepare4TofWall();
  }
  //NaiveTrackFinding();
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
    m_MSD_clusCollection.clear();
    m_MSD_pointCollection.clear();
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

  TMatrixDSym planarCov(2);
  TVectorD planarCoords(2);
  // get pixel coord
  TVector3 hitPos = m_GeoTrafo->FromVTLocalToGlobal( clus->GetPositionG() );

  if ( m_debug > 0 )
  hitPos.Print();

  //try to make the job of UploadClusVT

  //map for m_MCInfo
  map<int, MCTruthInfo> MCVTInfo;
  TAMCntuEve*  eve = (TAMCntuEve*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuEve")->Object();
  TAMCntuHit* vtMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("vtMc", "TAMCntuHit")->Object();

  Int_t idx = clus->GetMcTrackIdx(0);
  Int_t nHits = clus->GetPixelsN();
  // cout << "nHits: " << nHits  << endl;


  for (Int_t jHit = 0; jHit < nHits; ++jHit) {
    TAVTntuHit* hit = clus->GetPixel(jHit);
    Int_t id = hit->GetMcIndex(0);
    //cout << "McTrackId: " << idx << endl;
    //cout << "McIndex: " << id << endl;
    TAMChit* mcHit = vtMc->GetHit(id);
    TVector3 posin = mcHit->GetInPosition();
    TVector3 posout = mcHit->GetOutPosition();
    if (idx > -1 ){
      TAMCeveTrack* track = eve->GetTrack(idx);
      if (m_debug > 1)
      printf("charge %d mass %g index %d \n", track->GetCharge(), track->GetMass(), idx);
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
      MCVTInfo[iHit] = VTInfo;
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
    MCVTInfo[iHit] = VTInfo;
    break;
  }

  m_MCInfo[m_detectorID_map["VT"]] = MCVTInfo;



  planarCoords(0) = hitPos.x();
  planarCoords(1) = hitPos.y();

  TVector3 pixReso = clus->GetPosError();
  pixReso(2) = 0.005;   // 200 micron

  planarCov.UnitMatrix();
  for (int k = 0; k < 2; k++){
    planarCov[k][k] = pixReso(k)*pixReso(k);
  }

  PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["VT"], iHit, nullptr );
  hit->setPlane(m_detectorPlanes[clus->GetSensorIdx()], clus->GetSensorIdx());
  m_hitCollectionToFit_dataLike[ track_ID ].push_back( hit );

  // }
  if ( m_debug > 0 )
  cout << "\nPrepare4Vertex::Exiting\n";

}



//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4Vertex() {

  if ( m_debug > 0 )
  cout << "\nPrepare4Vertex::Entered\n";

  TMatrixDSym hitCov(3);
  TMatrixDSym planarCov(2);
  TVectorD hitCoords(3);
  TVectorD planarCoords(2);
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

    planarCoords(0) = hitPos.x();
    planarCoords(1) = hitPos.y();

    // set covariance matrix
    // double pixReso = 0.001;
    // double pixReso = GlobalPar::GetPar()->VTReso();

    TVector3 pixReso = p_hit->GetPosError();
    //   TVector3 pixReso =  m_GeoTrafo->FromVTLocalToGlobal( pixReso_ );
    hitCov.UnitMatrix();
    for (int j = 0; j < 3; j++){
      hitCov[j][j] = pixReso(j)*pixReso(j);
    }

    planarCov.UnitMatrix();
    for (int k = 0; k < 2; k++){
      planarCov[k][k] = pixReso(k)*pixReso(k);
    }

    //hardcoded
    hitCov[2][2] = 0.000005;

    //hitCov.Print();

    // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    //AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["VT"], i, nullptr );
    PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["VT"], i, nullptr );
    hit->setPlane(m_detectorPlanes[p_hit->GetSensorIdx()], p_hit->GetSensorIdx());
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
	//AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["IT"], iHit, nullptr );
  AbsMeasurement* hit = new PlanarMeasurement(hitCoords, hitCov, m_detectorID_map["IT"], iHit, nullptr );
//WARNING: FUNCTION IS NOT COMPLETE

	m_hitCollectionToFit_dataLike[ track_ID ].push_back( hit );
	m_allHitsInMeasurementFormat.push_back(hit);


	if ( m_debug > 0 )
	cout << "\nPrepare4Vertex::Exiting\n";

}




//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4InnerTracker() {

  if ( m_debug > 0 )
  cout << "\nPrepare4InnerTracker::Entered\n";

  TMatrixDSym planarCov(2);
  TVectorD planarCoords(2);
  for (unsigned int i = 0; i < m_IT_clusCollection.size(); i++) {
    TAITcluster* p_hit = m_IT_clusCollection.at(i);
    // get pixel coord
    TVector3 hitPos = m_GeoTrafo->FromITLocalToGlobal( p_hit->GetPositionG() );
    if ( m_debug > 0 )
    hitPos.Print();

    int tempPlane = 0;
    int planeNumber = p_hit->GetSensorIdx();

    if ( planeNumber > -1 &&  planeNumber < 4 )
    tempPlane = 4;
    else if ( planeNumber > 3 &&  planeNumber < 8 )
    tempPlane = 5;
    else if ( planeNumber > 7 &&  planeNumber < 12 )
    tempPlane = 6;
    else if ( planeNumber > 11 &&  planeNumber < 16 )
    tempPlane = 7;
    else if ( planeNumber > 15 &&  planeNumber < 20 )
    tempPlane = 8;
    else if ( planeNumber > 19 &&  planeNumber < 24 )
    tempPlane = 9;
    else if ( planeNumber > 23 &&  planeNumber < 28 )
    tempPlane = 10;
    else if ( planeNumber > 27 &&  planeNumber < 32 )
    tempPlane = 11;

    // set hit position vector
    planarCoords(0) = hitPos.x();
    planarCoords(1) = hitPos.y();

    // set covariance matrix

    TVector3 pixReso = p_hit->GetPosError();
    planarCov.UnitMatrix();
    for (int k = 0; k < 2; k++){
      planarCov[k][k] = pixReso(k)*pixReso(k);
    }

    // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    //AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["IT"], i, nullptr );
    PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["IT"], i, nullptr );
    hit->setPlane(m_detectorPlanes[tempPlane], tempPlane);
    // m_detectorPlanes[tempPlane]->Print();
    m_allHitsInMeasurementFormat.push_back(hit);

  }
  if ( m_debug > 0 )
  cout << "\nPrepare4InnerTracker::Exiting\n";
}



//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4TofWall() {

  if ( m_debug > 0 )
  cout << "\nPrepare4TofWall::Entered\n";

  TMatrixDSym planarCov(2);
  TVectorD planarCoords(2);
  for (unsigned int i = 0; i < m_TW_hitCollection.size(); i++) {

    TATWpoint* p_hit = m_TW_hitCollection.at(i);
    // get point coord
    TVector3 hitPos = m_GeoTrafo->FromTWLocalToGlobal( p_hit->GetPositionG() );

    if ( m_debug > 0 )
    hitPos.Print();

    // set point position vector
    planarCoords(0) = hitPos.x();
    planarCoords(1) = hitPos.y();
    TVector3 pixReso = p_hit->GetPosErrorG();
    pixReso.Print();

    planarCov.UnitMatrix();
    for (int k = 0; k < 2; k++){
      planarCov[k][k] = pixReso(k)*pixReso(k);
    }


    PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["TW"], i, nullptr );
    hit->setPlane(m_detectorPlanes[18], 18);
    m_allHitsInMeasurementFormat.push_back(hit);

  }
  if ( m_debug > 0 )
  cout << "\nPrepare4TofWall::Exiting\n";
}


//------------------------------------------------------------------------------
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
    //TVector3 pixReso =  m_GeoTrafo->FromVTLocalToGlobal( pixReso_ );
    hitCov.UnitMatrix();
    for (int j = 0; j < 3; j++){
      hitCov[j][j] = pixReso*pixReso;
    }

    //hardcoded
    hitCov[2][2] = 0.0002;

    //hitCov.Print();

    // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    //AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["MSD"], iHit, nullptr );
    AbsMeasurement* hit = new PlanarMeasurement(hitCoords, hitCov, m_detectorID_map["MSD"], iHit, nullptr );

    m_hitCollectionToFit_dataLike[ track_ID ].push_back( hit );
    m_allHitsInMeasurementFormat.push_back(hit);

}

//----------------------------------------------------------------------------------------------------
void KFitter::Prepare4Strip() {

  if ( m_debug > 0 )
  cout << "\nPrepare4Strip::Entered\n";

  //TMatrixDSym planarCov(2);
  TMatrixDSym planarCov(1);
  //TVectorD planarCoords(2);
  TVectorD planarCoords(1);

  for (unsigned int i = 0; i < m_MSD_clusCollection.size(); i++) {
    TAMSDcluster* p_hit = m_MSD_clusCollection.at(i);
    // get pixel coord
    TVector3 hitPos = m_GeoTrafo->FromMSDLocalToGlobal( p_hit->GetPositionG() );
    // get true MC coord
    // TVector3 hitPos = m_IT_hitCollection.at(i)->GetMCPosition_Global();
    //if ( m_debug > 0 )		cout << "ITR hit = Layer:" << p_hit->GetLayer() <<" col:"<< p_hit->GetPixelColumn() <<" row:"<< p_hit->GetPixelLine() <<
    // 							  " \n\t\tGEN Type: " << p_hit->m_genPartFLUKAid <<
    //   								"  genID= " << p_hit->m_genPartID << endl;
    // if ( m_debug > 0 )		cout << "Hit " << i;
    if ( m_debug > 0 )
    hitPos.Print();

    //check the view, 0 ->X, 1->Y
    double pixReso = 0.;
    bool isYView = false;

    if ( p_hit->GetPlaneView() == 0 ){
      planarCoords(0) = hitPos.x();
      pixReso = p_hit->GetPosError().X();
    }
    else{
      planarCoords(0) = hitPos.y();
      pixReso = p_hit->GetPosError().Y();
      isYView = true;
    }

    // set covariance matrix
    planarCov.UnitMatrix();
    for (int k = 0; k < 1; k++){
      planarCov[k][k] = pixReso*pixReso;
    }

    // nullptr is a TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
    //AbsMeasurement* hit = new SpacepointMeasurement(hitCoords, hitCov, m_detectorID_map["MSD"], i, nullptr );
    PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, m_detectorID_map["MSD"], i, nullptr );
    hit->setPlane(m_detectorPlanes[p_hit->GetSensorIdx()+12], p_hit->GetSensorIdx()+12);
    if (isYView) hit->setStripV();
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
    if ( m_systemsON.find( "MSD" ) != string::npos )		testHit_MSD = m_MSD_geo->GetSensorsN();
    if ( m_systemsON.find( "TW" ) != string::npos )			testHit_TW = 1;
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
  //if ( nHitVT != testHit_VT || nHitIT != testHit_IT || nHitMSD != testHit_MSD ){
  if ( nHitVT != testHit_VT || nHitIT != testHit_IT || nHitMSD < 4 ){
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

  *flukaID = m_MCInfo[detID][hitID].MCFlukaId;
  *trackID  = m_MCInfo[detID][hitID].MCTrackId;
  *charge  = m_MCInfo[detID][hitID].MCCharge;
  *mass    = m_MCInfo[detID][hitID].MCMass;

  if ( m_debug > 2 ) cout << "Detector Type (in KFitter::GetTrueParticleType)= " << detID << " HitID = " << hitID <<
  " " << *flukaID << " " << *trackID << " " << *charge << endl;

}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t KFitter::Action()
{
   long evNum  = (long)gTAGroot->CurrentEventId().EventNumber();
   MakeFit(evNum);
   
   return true;
}

//----------------------------------------------------------------------------------------------------
int KFitter::MakeFit( long evNum ) {

  if ( m_debug > 0 )		cout << "Starting MakeFit " << endl;
  int isConverged = 0;
  m_evNum = evNum;

  cout << " Evento numero " << m_evNum << endl;

  m_evNum_vect.push_back( evNum );

  // fill m_hitCollectionToFit
  if (m_workWithMC)
  PrepareData4Fit();
  else PrepareData4Fit_dataLike();
  // check the hit vector not empty otherwise clear
  if ( m_hitCollectionToFit.size() <= 0 )	{
    if ( m_debug > 0 )		cout << "No category to fit in this event..." << endl;
    m_VT_hitCollection.clear();
    m_VT_clusCollection.clear();
    m_IT_hitCollection.clear();
    m_IT_clusCollection.clear();
    m_MSD_hitCollection.clear();
    m_MSD_clusCollection.clear();
    m_MSD_pointCollection.clear();
    m_TW_hitCollection.clear();

    if (m_workWithMC)
    m_vectorTrack.clear();



    m_allHitsInMeasurementFormat.clear();
    //delete fitTrack;
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


    // start values for the fit (seed), change below
    TVector3 pos(0, 0, 0);	// global coord   [cm]
    TVector3 mom(0, 0, 6);	// GeV
    Track*  fitTrack = new Track();  // container of the tracking objects

    // SET PARTICLE HYPOTHESIS  --> set repository
    AbsTrackRep* rep = new RKTrackRep( (UpdatePDG::GetPDG()->GetPdgCode( tok.at(0)) ) );
    fitTrack->addTrackRep( rep );

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // more than one repo can be added and genfit test everyone of them
    // use DetermineCardinalRepresentation to choose the CardinalRep with minChi2 otherwise it takes the first added
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // SetTrueSeed( &pos, &mom );	// get seed from MC for debug

    // set seed
    fitTrack->setStateSeed(pos, mom);

    // insert points to be fitted - loop over each measurement in the current collection
    for ( unsigned int i=0; i < (*hitSample).second.size(); i++ ){
      fitTrack->insertMeasurement( (*hitSample).second.at(i) );
      fitTrack->checkConsistency();
      if ( m_debug > 3 )
      fitTrack->Print("C");

      // //CHECK WITH MC
      // if ((*hitSample).second.at(i)->getDetId()==m_detectorID_map["MSD"])
      // cout << m_MSD_pointCollection.at( (*hitSample).second.at(i)->getHitId() )->GetGenPartID() << " ";


    }

    //check of fitTrack filling
    cout << " check of fitTrack filling " << endl;
    for (unsigned int jTracking = 0; jTracking < fitTrack->getNumPointsWithMeasurement(); ++jTracking){
      fitTrack->getPointWithMeasurement(jTracking)->getRawMeasurement()->getRawHitCoords().Print();
      int indexOfPlane = static_cast<genfit::PlanarMeasurement*>(fitTrack->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId();
      cout << " index of plane " << indexOfPlane << " " << m_detectorPlanes[indexOfPlane]->getO().Z();

    }

    cout << endl;

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

      //here I have to add Track* in a vector<Track*> to see all the tracks in ED
      m_vectorTrack.push_back(fitTrack);

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

  if (m_IsEDOn){
    display->addEvent(m_vectorTrack);
    cout << "display->addEvent size  " << m_vectorTrack.size() << endl;
  }

  m_VT_hitCollection.clear();
  m_IT_hitCollection.clear();
  m_VT_clusCollection.clear();
  m_IT_clusCollection.clear();
  m_MSD_clusCollection.clear();
  m_MSD_pointCollection.clear();
  m_TW_hitCollection.clear();

  m_vectorTrack.clear();

  // clean all hits
  m_allHitsInMeasurementFormat.clear();
  //delete fitTrack;

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

  // SET SEED  --  debug onlyvectorTrack
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
void KFitter::GetKalmanTrackInfoDataLike ( int indexOfState, Track* track,
  const TVectorD* KalmanState, const TMatrixDSym* KalmanCov) {

  genfit::TrackPoint* tp = track->getPointWithMeasurementAndFitterInfo(indexOfState, nullptr);
  if (tp == nullptr) {
    //ofs << "Track has no TrackPoint with fitterInfo! \n";
    return;
  }
  genfit::KalmanFittedStateOnPlane kfsop(*(static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(nullptr))->getBackwardUpdate()));
  if (false) {
    //ofs << "state before extrapolating back to reference plane \n";
    kfsop.Print();
  }

  // extrapolate back to reference plane.
  //try{
  //  rep->extrapolateToPlane(kfsop, stateRefOrig.getPlane());;
  //}
  //catch(genfit::Exception& e){
  //  std::cerr<<"Exception, next track"<<std::endl;
  //  std::cerr << e.what();
  //  continue;
  //}

  KalmanState = &kfsop.getState();
  KalmanCov = &kfsop.getCov();

}

genfit::KalmanFittedStateOnPlane KFitter::GetKalmanTrackInfoDataLike_ ( int indexOfState, Track* track ) {

  genfit::TrackPoint* tp = track->getPointWithMeasurementAndFitterInfo(indexOfState, nullptr);
  if (tp == nullptr) {
    //ofs << "Track has no TrackPoint with fitterInfo! \n";
    genfit::KalmanFittedStateOnPlane ex;
    return ex;
  }
  genfit::KalmanFittedStateOnPlane kfsop(*(static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(nullptr))->getBackwardUpdate()));
  if (false) {
    //ofs << "state before extrapolating back to reference plane \n";
    kfsop.Print();
  }
  return kfsop;
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


  //cout << "in GetKalmanTrackInfo " << endl;
  //dynamic_cast<genfit::KalmanFitterInfo*>(track->getPointWithMeasurement(i)->getFitterInfo(nullptr))->getForwardPrediction()->Print();

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
    float kk = (float)m_nConvergedTracks[ (*itTotNum).first ];
    float nn = (*itTotNum).second;
    float variance = ( (kk+1)*(kk+2)/((nn+2)*(nn+3)) ) - ( (kk+1)*(kk+1)/((nn+2)*(nn+2)) );
    if ( m_debug > -1 )		cout << "Efficiency " << (*itTotNum).first << " = " << eff << "  " << m_nConvergedTracks[ (*itTotNum).first ]<< " " << (*itTotNum).second << endl;
    h_trackEfficiency->SetBinContent(k, eff);
    h_trackEfficiency->SetBinError(k, sqrt(variance));

    h_trackEfficiency->GetXaxis()->SetBinLabel(k, (*itTotNum).first.c_str() );
  }

  h_trackEfficiency->SetTitle(0);
  h_trackEfficiency->SetStats(0);
  h_trackEfficiency->GetYaxis()->SetTitle("Reco Efficiency");
  h_trackEfficiency->GetYaxis()->SetTitleOffset(1.1);
  h_trackEfficiency->GetYaxis()->SetRange(0.,1.);
  h_trackEfficiency->SetLineWidth(2); // take short ~ int
  h_trackEfficiency->Draw("E");
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
  //histoTrackParamX->SaveAs("trackparamX.root", "RECREATE");
  //histoTrackParamY->SaveAs("trackparamY.root", "RECREATE");
  //MSDresidualOfPrediction->SaveAs("MSDresidual.root", "RECREATE");
  //ITresidualOfPrediction->SaveAs("ITresidual.root", "RECREATE");
  percentageOfMCTracksVTX->SaveAs("percentageOfMCTracksVTX.root", "RECREATE");
  tempPurity->SaveAs("purity.root","RECREATE");
  ITstudy->SaveAs("ITstudy.root","RECREATE");
  MSDstudy->SaveAs("MSDstudy.root","RECREATE");

  qoverp->SaveAs("qoverpall.root","RECREATE");

  qoverpsel->SaveAs("qoverpsel.root","RECREATE");
  ofs.close();
  cout << "how many tracks have no update " << MSDforwardcounter << endl;

  m_fitTrackCollection->EvaluateMomentumResolution();

  m_categoryFitted.clear();
  //momentum_reco.Write();
  //momentum_true.Write();
  for (unsigned int iHisto = 0; iHisto < 8; ++iHisto){
    momentum_reco[iHisto]->Write();
    momentum_true[iHisto]->Write();
    ratio_reco_true[iHisto]->Divide(momentum_reco[iHisto],momentum_true[iHisto]);
    ratio_reco_true[iHisto]->Write();
  }
  outfile->Close();
  //show event display
  if (m_IsEDOn)
  display->open();

}




//----------------------------------------------------------------------------------------------------
void KFitter::InitEventDisplay() {



  // init event display
  display = genfit::EventDisplay::getInstance();
  display->reset();

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
