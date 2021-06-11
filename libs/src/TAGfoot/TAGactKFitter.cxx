#include "TAGactKFitter.hxx"
#include "TAGtrackRepoKalman.hxx"
#include "TAGactNtuGlbTrack.hxx"




// ClassImp(TAGactKFitter);

//----------------------------------------------------------------------------------------------------
// TAGactKFitter::TAGactKFitter (const char* name) : TAGaction(name, "TAGactKFitter - Global GenFit Tracker") {
TAGactKFitter::TAGactKFitter (const char* name, TAGdataDsc* outTrackRepoGenFit, TAGdataDsc* outTrackRepo) : TAGaction(name, "TAGactKFitter - Global GenFit Tracker"), fpGlobTrackRepoGenFit(outTrackRepoGenFit), fpGlobTrackRepo(outTrackRepo) {

	// cout << "AGactKFitter::TAGactKFitter -- begin" << endl;

	AddDataOut(outTrackRepoGenFit, "TAGtrackRepoKalman");
	AddDataOut(outTrackRepo, "TAGntuTrackRepository");
	// AddDataIn(p_vtxclus, "TAVTntuCluster");
	m_trueParticleRep = (TAMCntuPart*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();

	int nIter = 20; // max number of iterations
	double dPVal = 1.E-3; // convergence criterion
	m_AMU = 0.9310986964; // in GeV // conversion betweem mass in GeV and atomic mass unit

	gGeoManager->ClearPhysicalNodes();

	m_sensorIDmap = new TAGFdetectorMap();
	genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());
	// genfit::MaterialEffects::getInstance()->setDebugLvl(1);
	genfit::MaterialEffects::getInstance()->setNoEffects(false);

	m_Particles = { "H", "He", "Li", "Be", "B", "C", "N", "O" };
	for ( unsigned int i=0; i<m_Particles.size(); i++ )
		m_ParticleIndex[ m_Particles[i] ] = i;

	m_Isotopes  = { 	"C11", "C12", "C13", "C14", 
						"Li6", "Li7",
						"Be7", "Be9", "Be10",
						"B10", "B11",
						"N14", "N15",
						"He4", "He3", "H1", "H2", "H3",
						"O15", "O16" };
	for ( unsigned int i=0; i<m_Isotopes.size(); i++ )
		m_IsotopesIndex[ m_Isotopes[i] ] = i;

	m_debug = TAGrecoManager::GetPar()->Debug();

	m_mapTrack.clear();

	// new one
	m_outTrackRepoGenFit = (TAGtrackRepoKalman*) fpGlobTrackRepoGenFit->Object();
	m_outTrackRepo = (TAGntuTrackRepository*) fpGlobTrackRepo->Object();

	// checks for the detector to be used for kalman
	IncludeDetectors();
	CreateGeometry();
	
	m_fitter_extrapolation = new KalmanFitter(1, dPVal);
	
	// initialise the kalman method selected from param file
	if ( TAGrecoManager::GetPar()->KalMode() == "ON" )
	m_fitter = new KalmanFitter(nIter, dPVal);
	else if ( TAGrecoManager::GetPar()->KalMode() == "ref" )
	m_refFitter = new KalmanFitterRefTrack(nIter, dPVal);
	else if ( TAGrecoManager::GetPar()->KalMode() == "daf" )
	m_dafRefFitter = new DAF(true, nIter, dPVal);
	else if ( TAGrecoManager::GetPar()->KalMode() == "dafsimple" )
	m_dafSimpleFitter = new DAF(false, nIter, dPVal);
	if ( TAGrecoManager::GetPar()->EnableEventDisplay() )	    InitEventDisplay();
	//---------------------------------------------------------------
	//---------------------------------------------------------------
	//DUMP GEOMETRY

	// // --- close the geometry
	// gGeoManager->CloseGeometry();

	// //--- draw the ROOT box
	// gGeoManager->SetVisLevel(10);
	// m_TopVolume->Draw("ogl");
	// TFile *outfile = TFile::Open("genfitFOOTGeom.root","RECREATE");
	// gGeoManager->Write();
	// outfile->Close();

	// ofs.open ("efficiency.txt", std::ofstream::out);

	TAGrecoManager::GetPar()->Print("all");
	cout << "TAGactKFitter::TAGactKFitter -- 1" << endl;
	m_uploader = new TAGFuploader( m_sensorIDmap );
	cout << "TAGactKFitter::TAGactKFitter -- 2" << endl;
	m_trackAnalysis = new TAGF_KalmanStudies();

// cout << "AGactKFitter::TAGactKFitter -- end" << endl;

	for(int i=0; i<m_sensorIDmap->GetFitPlanesN(); ++i)
	{
		m_NClusTrack.push_back(0);
		m_NClusGood.push_back(0);
	}

	m_NTWTracks = 0;
	m_NTWTracksGoodHypo = 0;

}






//----------------------------------------------------------------------------------------------------
TAGactKFitter::~TAGactKFitter() {
    delete m_fitter;
    delete m_fitter_extrapolation;
    delete m_refFitter;
}





//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAGactKFitter::Action()	{

	long evNum = (long)gTAGroot->CurrentEventId().EventNumber();

	// m_allHitMeasGF[1000]= vector<int>(3,new AbsMeasurement()) ;
	if(m_debug > 0) cout << "TAGactKFitter::Action()  ->  " << m_allHitMeasGF.size() << endl;

	m_uploader->TakeMeasHits4Fit( m_allHitMeasGF );
	vector<int> chVect;
	m_uploader->GetPossibleCharges( &chVect );

	if ( TAGrecoManager::GetPar()->IsMC() ) 
			m_measParticleMC_collection = m_uploader->TakeMeasParticleMC_Collection();


	if(m_debug > 0)	{
		cout << "TAGactKFitter::Action()  ->  " << m_allHitMeasGF.size() << endl;
		cout << "Plane\tN. hits" << endl;
		for(auto it = m_allHitMeasGF.begin(); it != m_allHitMeasGF.end(); ++it)		{
			cout << it->first << "\t" << it->second.size() << endl;
		}
	}


	m_selector = new TAGFselector(&m_allHitMeasGF, &chVect, m_sensorIDmap, &m_mapTrack, m_measParticleMC_collection);
	// if ( m_selector->Categorize() < 0 )
	// 	return true;


	// clear the track map m_allHitMeasGF ma come mappa...
	// if ( m_hitCollectionToFit.size() == 0 ) {

	//   for ( vector<AbsMeasurement*>::iterator it2=m_allHitsInMeasurementFormat.begin(); it2 != m_allHitsInMeasurementFormat.end(); it2++ )
	//   delete (*it2);
	//   m_allHitsInMeasurementFormat.clear();
	//   return 0;
	// }
	if ( m_selector->Categorize() >= 0 )
		MakeFit(evNum);
	
	chVect.clear();
	m_allHitMeasGF.clear();
	m_measParticleMC_collection->clear();

	for ( auto it = m_mapTrack.cbegin(), next_it = m_mapTrack.cbegin(); it != m_mapTrack.cend(); it = next_it)	{
		next_it = it; ++next_it;	
		delete (*it).second;
		m_mapTrack.erase(it);
	}
	// for ( map<TString,Track*>::iterator trackIt = m_mapTrack.begin(); trackIt != m_mapTrack.end(); trackIt++) {
	// 	delete (*trackIt).second;
	// }
	// for (auto it = m_mapTrack.begin(); it != m_mapTrack.end(); /* don't increment here*/) {
	//     delete it->second;
	//     it = m_mapTrack.erase(it);  // update here
	// }


	// m_mapTrack.clear();

	fpGlobTrackRepoGenFit->SetBit(kValid);
	fpGlobTrackRepo->SetBit(kValid);
	return true;

}









//----------------------------------------------------------------------------------------------------
// Called from outside!
void TAGactKFitter::Finalize() {

  // make a directory for each hit category that forms a track candidate
  // struct stat info;
  // for ( unsigned int i=0; i < m_categoryFitted.size(); i++ ) {
  //   string pathName = m_kalmanOutputDir+"/"+m_categoryFitted.at(i);
  //   if( stat( pathName.c_str(), &info ) != 0 )		//cannot access
  //     system(("mkdir "+pathName).c_str());
  // }

	if ( TAGrecoManager::GetPar()->IsMC() ) 		m_trackAnalysis->EvaluateAndFill_MomentumResolution( &h_dPOverP_x_bin, &h_resoP_over_Pkf, &h_biasP_over_Pkf );
	if ( TAGrecoManager::GetPar()->IsMC() ) 		PrintPurity();
	PrintEfficiency();
	
	// map<string, map<float, TH1F*> > h_dPOverP_x_bin
	for ( map<string, map<float, TH1F*> >::iterator collIt=h_dPOverP_x_bin.begin(); collIt != h_dPOverP_x_bin.end(); collIt++ )
		for ( map<float, TH1F*>::iterator it=(*collIt).second.begin(); it != (*collIt).second.end(); it++ ) {
			AddHistogram( (*it).second );
			cout << "TAGactKFitter::Finalize() -- " << (*it).second->GetTitle()<< endl;
		}

	// map<string, TH1F*>* h_resoP_over_Pkf
	for ( map<string, TH1F*>::iterator it=h_resoP_over_Pkf.begin(); it != h_resoP_over_Pkf.end(); it++ ) 
			AddHistogram( (*it).second );

	// map<string, TH1F*>* h_biasP_over_Pkf
	for ( map<string, TH1F*>::iterator it=h_biasP_over_Pkf.begin(); it != h_biasP_over_Pkf.end(); it++ ) 
			AddHistogram( (*it).second );

	// map<string, TH1F*>* h_deltaP
	for ( map<string, TH1F*>::iterator it=h_deltaP.begin(); it != h_deltaP.end(); it++ ) 
			AddHistogram( (*it).second );

	SetValidHistogram(kTRUE);
	SetHistogramDir(m_dir);
	// gDirectory->ls();
	// cout <<  "TAGactKFitter::Finalize() -- " << gDirectory->ls() << endl;

  

  // ofs.close();
  // cout << "how many tracks have no update " << MSDforwardcounter << endl;


  // m_categoryFitted.clear();

  //show event display
  if ( TAGrecoManager::GetPar()->EnableEventDisplay() )		display->open();


	if(m_debug > 0)
	{
		cout << "Check quality of charge hypothesis\nPlaneId\tNClus\tNGood" << endl;
		for(int i=0; i< m_NClusGood.size(); ++i)
		{
			cout << i << "\t" << m_NClusTrack.at(i) << "\t" << m_NClusGood.at(i) << endl;
		}

		cout << "TWtracks\t\tTWtracksGoodHypo\n" << m_NTWTracks << "\t\t" << m_NTWTracksGoodHypo << endl;

	}

}











//----------------------------------------------------------------------------------------------------
// check and print which detectors included and/or used in the kalman
void TAGactKFitter::IncludeDetectors() {

	// check kalman detectors set in param file are correct
	if (TAGrecoManager::GetPar()->KalSystems().size() == 0)
	{
		Error("IncludeDetectors()", "KalSystems parameter not set properly! Size is 0.");
		throw -1;
	}

	else if ( !(TAGrecoManager::GetPar()->KalSystems().size() == 1 && TAGrecoManager::GetPar()->KalSystems().at(0) == "all") )
	{
		for (unsigned int i=0; i<TAGrecoManager::GetPar()->KalSystems().size(); i++ ) 
		{
			if ( !m_sensorIDmap->IsDetectorInMap( TAGrecoManager::GetPar()->KalSystems().at(i) ) )
			{
				Error("IncludeDetectors()", "KalSystems parameter not set properly! Detector '%s' not found in global map.", TAGrecoManager::GetPar()->KalSystems().at(i).c_str());
				throw -1;
			}
		}
	}

  // list of detectors used for kalman
  m_systemsON = "";
  for (unsigned int i=0; i<TAGrecoManager::GetPar()->KalSystems().size(); i++ ) {
    if (i != 0)		m_systemsON += " ";
    m_systemsON += TAGrecoManager::GetPar()->KalSystems().at(i);
  }
  if (m_debug > 0)	cout << "TAGactKFitter::IncludeDetectors() -- Detector systems for Kalman:  " << m_systemsON << endl;

  // print-out of the particle hypothesis used for the fit
  cout << "TAGactKFitter::IncludeDetectors() -- TAGrecoManager::GetPar()->MCParticles()";
  for (unsigned int i=0; i<TAGrecoManager::GetPar()->MCParticles().size(); i++ ) {
    cout << "   " << TAGrecoManager::GetPar()->MCParticles().at(i);
  }
  cout << endl;
}







//----------------------------------------------------------------------------------------------------
void TAGactKFitter::CreateGeometry()  {

	if(m_debug > 0)	cout << "TAGactKFitter::CreateGeometry() -- START" << endl;

	// take geometry objects
  if (TAGrecoManager::GetPar()->IncludeTG())
    m_TG_geo = shared_ptr<TAGparGeo> ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() );

  if (TAGrecoManager::GetPar()->IncludeDI())
  m_DI_geo = shared_ptr<TADIparGeo> ( (TADIparGeo*) gTAGroot->FindParaDsc("diGeo", "TADIparGeo")->Object() );

  if ( (m_systemsON == "all" || m_systemsON.find( "VT" ) != string::npos) && TAGrecoManager::GetPar()->IncludeVT() )
    m_VT_geo = shared_ptr<TAVTparGeo> ( (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() );

  if ( (m_systemsON == "all" || m_systemsON.find( "IT" ) != string::npos) && TAGrecoManager::GetPar()->IncludeIT() )
    m_IT_geo = shared_ptr<TAITparGeo> ( (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object() );

  if ( (m_systemsON == "all" || m_systemsON.find( "MSD" ) != string::npos) && TAGrecoManager::GetPar()->IncludeMSD() )
    m_MSD_geo = shared_ptr<TAMSDparGeo> ( (TAMSDparGeo*) gTAGroot->FindParaDsc("msdGeo", "TAMSDparGeo")->Object() );

  if ( ( m_systemsON.find( "TW" ) != string::npos) && TAGrecoManager::GetPar()->IncludeTW() )
    m_TW_geo = shared_ptr<TATWparGeo> ( (TATWparGeo*) gTAGroot->FindParaDsc("twGeo", "TATWparGeo")->Object() );


  m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());


  //set the stage for TGeoManagerInterface class of GenFit
  // this is the World volume
  TGeoMedium* med = gGeoManager->GetMedium("AIR");
  m_TopVolume = gGeoManager->MakeBox("World",med, 300., 300., 300.);
  m_TopVolume->SetInvisible();
  gGeoManager->SetTopVolume(m_TopVolume);

  m_vecHistoColor = { kBlack, kRed-9, kRed+1, kRed-2, kOrange+7, kOrange, kOrange+3, kGreen+1,
    kGreen+3, kBlue+1, kBlue+3, kAzure+8, kAzure+1, kMagenta+2,
    kMagenta+3, kViolet+1, kViolet+6, kViolet-4 };

	int indexOfPlane = 0;

	// // ST
	// if (TAGrecoManager::GetPar()->IncludeST()) {
	//   TASTparGeo* parGeo = fReco->GetParGeoSt();
	//   TGeoVolume* irVol  = parGeo->BuildStartCounter();

	//   TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TASTparGeo::GetBaseName());
	// }

	// // BM
	// if (TAGrecoManager::GetPar()->IncludeBM()) {
	//   TABMparGeo* parGeo = fReco->GetParGeoBm();;
	//   TGeoVolume* bmVol  = parGeo->BuildBeamMonitor();

	//   TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TABMparGeo::GetBaseName());
		// }


	// target
	if (TAGrecoManager::GetPar()->IncludeTG()) {
		TGeoVolume* tgVol = m_TG_geo->BuildTarget();
		tgVol->SetLineColor(kBlack);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAGparGeo::GetBaseName());
		m_TopVolume->AddNode(tgVol, 1, transfo);
	}

	// Vertex
	if (TAGrecoManager::GetPar()->IncludeVT()) {
		TGeoVolume* vtVol  = m_VT_geo->BuildVertex();
		vtVol->SetLineColor(kRed+1);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAVTparGeo::GetBaseName());
		m_TopVolume->AddNode(vtVol, 2, transfo);

		for ( int i = 0; i < m_VT_geo->GetSensorsN(); ++i ) {
			genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( m_GeoTrafo->FromVTLocalToGlobal(m_VT_geo->GetSensorPosition(i)), TVector3(0,0,1)));
			detectorplane->setU(1.,0.,0.);
			detectorplane->setV(0.,1.,0.);
			m_sensorIDmap->AddFitPlane(indexOfPlane, detectorplane);
			m_sensorIDmap->AddFitPlaneIDToDet(indexOfPlane, "VT");
			++indexOfPlane;
		}
	}

	// Magnet
	if (TAGrecoManager::GetPar()->IncludeDI()) {
		TGeoVolume* diVol = m_DI_geo->BuildMagnet();
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TADIparGeo::GetBaseName());
		m_TopVolume->AddNode(diVol, 3, transfo);
	}

	// IT
	if (TAGrecoManager::GetPar()->IncludeIT()) {
		TGeoVolume* itVol  = m_IT_geo->BuildInnerTracker();
		itVol->SetLineColor(kRed);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAITparGeo::GetBaseName());
		m_TopVolume->AddNode(itVol, 4, transfo);

		for ( int i = 0; i < m_IT_geo->GetSensorsN(); i++ ) {
			TVector3 origin_(0.,0.,m_GeoTrafo->FromITLocalToGlobal(m_IT_geo->GetSensorPosition(i)).Z());

			float xMin = m_GeoTrafo->FromITLocalToGlobal(m_IT_geo->GetSensorPosition(i)).x() - m_IT_geo->GetEpiSize().X()/2;
			float xMax = m_GeoTrafo->FromITLocalToGlobal(m_IT_geo->GetSensorPosition(i)).x() + m_IT_geo->GetEpiSize().X()/2;
			float yMin = m_GeoTrafo->FromITLocalToGlobal(m_IT_geo->GetSensorPosition(i)).y() - m_IT_geo->GetEpiSize().y()/2;
			float yMax = m_GeoTrafo->FromITLocalToGlobal(m_IT_geo->GetSensorPosition(i)).y() + m_IT_geo->GetEpiSize().y()/2;

			// This make all the 32 IT sensors
			genfit::AbsFinitePlane* recta = new RectangularFinitePlane( xMin, xMax, yMin, yMax );
			genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, TVector3(0,0,1), recta));
			detectorplane->setU(1.,0.,0.);
			detectorplane->setV(0.,1.,0.);

			m_sensorIDmap->AddPlane_Zorder( m_GeoTrafo->FromITLocalToGlobal(m_IT_geo->GetSensorPosition(i)).Z(), indexOfPlane );

			m_sensorIDmap->AddFitPlane(indexOfPlane, detectorplane);
			m_sensorIDmap->AddFitPlaneIDToDet(indexOfPlane, "IT");
			++indexOfPlane;
		}
	}

	// MSD
	if (TAGrecoManager::GetPar()->IncludeMSD()) {
		TGeoVolume* msdVol = m_MSD_geo->BuildMultiStripDetector();
		msdVol->SetLineColor(kViolet);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAMSDparGeo::GetBaseName());
		m_TopVolume->AddNode(msdVol, 5, transfo);

		for ( int i = 0; i < m_MSD_geo->GetSensorsN(); i++ ) {
			genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( m_GeoTrafo->FromMSDLocalToGlobal(m_MSD_geo->GetSensorPosition(i)), TVector3(0,0,1)));
			detectorplane->setU(1.,0.,0.);
			detectorplane->setV(0.,1.,0.);
			m_sensorIDmap->AddFitPlane(indexOfPlane, detectorplane);
			m_sensorIDmap->AddFitPlaneIDToDet(indexOfPlane, "MSD");
			++indexOfPlane;

		}
	}

	// TW
	if (TAGrecoManager::GetPar()->IncludeTW()) {
		TGeoVolume* twVol = m_TW_geo->BuildTofWall();
		twVol->SetLineColor(kBlue);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TATWparGeo::GetBaseName());
		m_TopVolume->AddNode(twVol, 6, transfo);

		genfit::SharedPlanePtr detectorplane (new genfit::DetPlane(m_GeoTrafo->FromTWLocalToGlobal(m_TW_geo->GetLayerPosition(1)), TVector3(0,0,1)));
		detectorplane->setU(1.,0.,0.);
		detectorplane->setV(0.,1.,0.);
		m_sensorIDmap->AddFitPlane(indexOfPlane, detectorplane);
		m_sensorIDmap->AddFitPlaneIDToDet(indexOfPlane, "TW");
		++indexOfPlane;
	}

	// // CA
	// if (TAGrecoManager::GetPar()->IncludeCA()) {
	//   TACAparGeo* parGeo = fReco->GetParGeoCa();
	//   TGeoVolume* caVol = parGeo->BuildCalorimeter();

	//   TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TACAparGeo::GetBaseName());
	// m_sensorIDmap->AddDetector(indexOfDet, "CA");
	// ++indexOfDet;
	// }

	if(m_debug > 0)	cout << "TAGactKFitter::CreateGeometry() -- STOP" << endl;

}











//----------------------------------------------------------------------------------------------------
int TAGactKFitter::MakeFit( long evNum ) {

	if ( m_debug > 0 )		cout << "Starting MakeFit " << endl;

	int isConverged = 0;
	int trackCounter = -1;

	m_evNum = evNum;
	// if(m_debug > 0)			
		cout << "\n  ----------------------\nEvento numero " << m_evNum << " track " << m_mapTrack.size() << endl;
	
	// loop over all hit category
	for ( map<TString,Track*>::iterator trackIt = m_mapTrack.begin(); trackIt != m_mapTrack.end(); ++trackIt) {

		isConverged = 0;
		
		vector<string> tok = Tokenize( trackIt->first.Data() , "_" );
		string PartName = tok.at(0);

		cout << "Track candidate: "<<trackCounter<< "  "<< PartName << " " << trackIt->first.Data() << endl;

		// check if the category is defined in UpdatePDG  -->  also done in GetPdgCode()
		if ( TAGrecoManager::GetPar()->PreselectStrategy() == "TrueParticle" )  {
			string partName = tok.at(0) + tok.at(1);
			if ( !UpdatePDG::GetPDG()->IsParticleDefined( partName ) )
			{
				Error("MakeFit()", "Category %s%s not found in UpdatePDG!" , tok.at(0).c_str(), tok.at(1).c_str());
				throw -1;
			}
		}

		// tmp track to be used inside the loop
		Track* fitTrack = trackIt->second;


		trackCounter++;

	    // check of fitTrack filling
	    if ( m_debug > 2 ) {
		    cout << " check of fitTrack filling " << endl;
		    for (unsigned int iMeas = 0; iMeas < fitTrack->getNumPointsWithMeasurement(); ++iMeas){

				fitTrack->getPointWithMeasurement(iMeas)->getRawMeasurement()->getRawHitCoords().Print();
				int indexOfPlane = static_cast<genfit::PlanarMeasurement*> ( fitTrack->getPointWithMeasurement(iMeas)->getRawMeasurement() )->getPlaneId();
				
				cout << " index of plane " << indexOfPlane << "  pointID " << iMeas << endl;
		  	}
		  	cout << endl;
	    }
		if ( fitTrack->getNumPointsWithMeasurement() < 9 ) continue;

	    if ( TAGrecoManager::GetPar()->IsKalReverse() )
	    	fitTrack->reverseTrackPoints();

	    //check
	    fitTrack->checkConsistency();
	    if ( m_debug > 2 )	    fitTrack->Print();
		
		EvaluateProjectionEfficiency(&PartName, fitTrack);

		// map of the number of converged tracks for each track hypothesis
		if ( m_nSelectedTrackCandidates.find( PartName ) == m_nSelectedTrackCandidates.end() )	m_nSelectedTrackCandidates[ PartName ] = 0;
		m_nSelectedTrackCandidates[ PartName ]++;

		if(m_debug > 0)	{
			cout << "PROCESSING TRACK!!!!!" << endl;
			cout << fitTrack->getCardinalRep()->getPDG() << "\t" << fitTrack->getCardinalRep()->getPDGCharge()<< endl;
			fitTrack->getCardinalRep()->Print();
		}

	    // THE REAL FIT with different Kalman modes
	    try{


	    	double dPVal = 1.E-3; // convergence criterion
	    	KalmanFitter* preFitter = new KalmanFitter(1, dPVal);
	    	preFitter->processTrackWithRep( fitTrack, fitTrack->getCardinalRep() );
			
			if(m_debug > 0)
			{
				cout << "Track reps" << endl;
				for(int i = 0; i < fitTrack->getNumReps(); ++i)
					fitTrack->getTrackRep(i)->Print();
				cout << "Cardinal rep: ";
				fitTrack->getCardinalRep()->Print();
			}
	    	// m_fitter_extrapolation->processTrack( fitTrack );

			if ( TAGrecoManager::GetPar()->KalMode() == "ON" ) {
				// m_fitter->processTrack( fitTrack );
				m_fitter->processTrackWithRep( fitTrack, fitTrack->getCardinalRep() );
			}
			else if ( TAGrecoManager::GetPar()->KalMode() == "ref" ) {

				// if(TAGrecoManager::GetPar()->PreselectStrategy() == "Sept2020")
				// {
				// 	int Z_Hypo = fitTrack->getCardinalRep()->getPDGCharge();
				// 	double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(Z_Hypo) );
				// 	int A_Hypo = round(mass_Hypo/m_AMU);

				// 	TVector3 pos = TVector3(0,0,0);
				// 	TVector3 mom = TVector3(0,0,TMath::Sqrt( pow(0.2*A_Hypo,2) + 2*mass_Hypo*0.2*A_Hypo ));
				// 	fitTrack->setStateSeed(pos, mom);
				// }
				m_refFitter->processTrackWithRep( fitTrack, fitTrack->getCardinalRep() );
				// m_refFitter->processTrack(fitTrack);
			}
			else if ( TAGrecoManager::GetPar()->KalMode() == "daf" )
				m_dafRefFitter->processTrack( fitTrack );
			else if ( TAGrecoManager::GetPar()->KalMode() == "dafsimple" )
				m_dafSimpleFitter->processTrack( fitTrack );
			else {
				cout << "TAGactKFitter::MakeFit -- ERROR :: wrong  TAGrecoManager::GetPar()->KalMode()" << endl, exit(0);
			}
			// delete preFitter;
		}
		catch (genfit::Exception& e) {
			std::cerr << e.what();
			std::cerr << "Exception, next track" << std::endl;
			continue;
    	}

		if(m_debug > 0)
		{
			cout << "TRACK PROCESSED!!!!!" << endl;
			fitTrack->getFitStatus(fitTrack->getCardinalRep())->Print();
		}

		if (  fitTrack->getFitStatus(fitTrack->getCardinalRep())->isFitConverged() &&   fitTrack->getFitStatus(fitTrack->getCardinalRep())->isFitted() )		
			isConverged = 1;	// convergence check

		if ( m_debug > 3 )		fitTrack->Print("C");


		// // map of the CONVERGED tracks for each category
		if (isConverged) {

			// if(m_debug > 0) cout << "---------- TRACK EXTRAPOLATION!!!!!" << endl;
			// 			TVector3 target(0,0,0);
			// 			StateOnPlane state = fitTrack->getFittedState(0);
			// 			float old = state.get6DState()[2];
			// 			double extL = fitTrack->getCardinalRep()->extrapolateToPoint( state, target );
			// 			float ext = state.get6DState()[2];
			// if(m_debug > 0) cout << "---------- TRACK EXTRAPOLATION!!!!!   end - l= " << extL << "\n\toldZ = " <<old << "\t newZ = " << ext << endl<<endl;
			

			if ( m_nConvergedTracks_all.find( PartName ) == m_nConvergedTracks_all.end() )	m_nConvergedTracks_all[ PartName ] = 0;
			m_nConvergedTracks_all[ PartName ]++;

			RecordTrackInfo( fitTrack, PartName );
			cout << "DONE" << endl;
			m_vectorConvergedTrack.push_back( fitTrack );
		}
		

		// // fill a vector with the categories fitted at least onece
		// if ( find( m_categoryFitted.begin(), m_categoryFitted.end(), (*hitSample).first ) == m_categoryFitted.end() )
		// m_categoryFitted.push_back( (*hitSample).first );

	}	// end  - loop over all hit category

	// filling with converged and not tracks. Probably needs to use only converged ones!!!!!!!!!!!!!!!!!!!!!!
	if ( TAGrecoManager::GetPar()->EnableEventDisplay() ) {
		display->addEvent(m_vectorConvergedTrack);
		cout << "display->addEvent size  " << m_vectorConvergedTrack.size() << endl;
	}

	if ( m_debug > 0 )		cout << "Ready for the next track fit!" << endl;
	// if ( m_debug > 0 )		cin.get();

	return isConverged;
}









//----------------------------------------------------------------------------------------------------
void TAGactKFitter::MakePrefit() {
// try{
	 //        if ( m_debug > 0 ) 		cout<<"Starting the fitter"<<endl;

	 //        // if (prefit) {
	          // genfit::KalmanFitter prefitter(1, dPVal);
	          // prefitter.setMultipleMeasurementHandling(genfit::weightedClosestToPrediction);
	          // prefitter.processTrackWithRep(fitTrack, fitTrack->getCardinalRep());
	 //        // }

	        // fitter->processTrack(fitTrack, false);

	 //        if ( m_debug > 0 ) cout<<"fitter is finished!"<<endl;
	 //      }
	 //      catch(genfit::Exception& e){
	 //        cout << e.what();
	 //        cout << "Exception, next track" << endl;
	 //        continue;
	 //    }
}










//----------------------------------------------------------------------------------------------------
//
void TAGactKFitter::RecordTrackInfo( Track* track, string fitTrackName ) {

	if(m_debug > 0)		cout << "RECORD START" << endl;
	TAGglobalTrack* shoeOutTrack;
	TAGtrackKalman* shoeOutTrackGenFit;
	vector<TAGshoeTrackPoint*> shoeTrackPointRepo;
	
	// if ( track->getMcTrackId() == trackIDFromMontecarlo ) 		numeratorForPurity+=1;

	// Fill Point and retrieve the true MC particle for each measuerement
	vector<vector<int>> mcParticleID_track;
	for (unsigned int iMeas = 0; iMeas < track->getNumPointsWithMeasurement(); ++iMeas) {
		
		vector<int> mcID_Meas;
		int trackHitID = track->getPointWithMeasurement(iMeas)->getRawMeasurement()->getHitId();
		int trackDetID = track->getPointWithMeasurement(iMeas)->getRawMeasurement()->getDetId();
	
		// get measurement position, position error and other info
		TVector3 measPos, measPos_err;
		GetMeasTrackInfo( trackHitID, &measPos, &measPos_err );
		
		int iPlane, iClus;
		vector<int> iPart;
		GetMeasInfo( trackDetID, trackHitID, &iPlane, &iClus, &iPart );

		// vector with index of the mc truth particles generating the measurement
		for ( unsigned int ip=0; ip<iPart.size()
		; ip++)
			mcID_Meas.push_back( iPart.at(ip) );
		mcParticleID_track.push_back(mcID_Meas);

		// create shoe track points vector repository to be passed to the output track object, wth geenral and measurement info
		TAGshoeTrackPoint* shoeTrackPoint = new TAGshoeTrackPoint( trackDetID, iPlane, iClus, iPart, &measPos );

		// getRecoInfo
		TVector3 recoPos, recoMom;
		TMatrixD recoPos_cov(3,3);
		TMatrixD recoMom_cov(3,3);
		// cout << "TEST1" << endl;

		GetRecoTrackInfo( iMeas, track, &recoPos, &recoMom, &recoPos_cov, &recoMom_cov );
		shoeTrackPoint->SetRecoInfo( &recoPos, &recoMom, &recoPos_cov, &recoMom_cov );   // fill with reconstructed info

		// fill shoe track points vector repository to be passed to the output track object
		shoeTrackPointRepo.push_back( shoeTrackPoint ); 
	}

	TVector3 recoPos_target, recoMom_target;
	TVector3 mcMom, mcPos;
	// TVector3 recoPos_err, recoMom_err;
	TMatrixD recoPos_target_cov(3,3);
	TMatrixD recoMom_target_cov(3,3);

	// cout << "TEST" << endl;
	// get reco info at the target level (by now at the first VT layer)
	GetRecoTrackInfo( 0, track, &recoPos_target, &recoMom_target, &recoPos_target_cov, &recoMom_target_cov );

	// Retrieve tracking info!
	int nMeas 	= track->getCardinalRep()->getDim();
	int pdgID 	= track->getCardinalRep()->getPDG();
	int pdgCh 	= track->getCardinalRep()->getPDGCharge();
	int measCh 	= track->getCardinalRep()->getCharge( track->getFittedState(0) );            // dipendono dallo stato considerato
	double mass = track->getCardinalRep()->getMass( track->getFittedState(0) );              // dipendono dallo stato considerato
	// track->getCardinalRep()->getRadiationLenght();   // to be done! Check update versions...

	if(measCh < 0 || measCh > 8) {return;}

	int trackMC_id = -1;
	if ( TAGrecoManager::GetPar()->IsMC() ) {
		trackMC_id = FindMostFrequent( &mcParticleID_track );
		// trackMC_id = track->getMcTrackId();     //???????

		TAMCpart* particle = m_trueParticleRep->GetTrack( trackMC_id );
		mcMom = particle->GetInitP();
		mcPos = particle->GetInitPos();

		if(m_debug > 0)	cout << "\n\nTAGactKFitter::RecordTrackInfo:: True Pos = "<< mcPos.Mag() << "     p = "<< mcMom.Mag() << "  " << fitTrackName<< endl;
		if(m_debug > 0)	cout << "TAGactKFitter::RecordTrackInfo:: Reco Pos = "<< recoPos_target.Mag() << "     p = "<< recoMom_target.Mag() << endl<<endl<<endl;

		m_trackAnalysis->Fill_MomentumResidual( recoMom_target, mcMom, recoMom_target_cov, fitTrackName, &h_dPOverP_x_bin );
		// m_trackAnalysis->EvaluateAndFill_MomentumResolution( &h_dPOverP_x_bin, &h_resoP_over_Pkf, &h_biasP_over_Pkf );

		m_trackAnalysis->FillMomentumInfo( recoMom_target, mcMom, recoMom_target_cov, fitTrackName, &h_deltaP );

		h_particleFlip->Fill( m_IsotopesIndex[ UpdatePDG::GetPDG()->GetPdgName( pdgID ) ] - trackMC_id ); 
		if(m_debug > 0)	cout << "TAGactKFitter::RecordTrackInfo:: m_IsotopesIndex = "<< m_IsotopesIndex[ UpdatePDG::GetPDG()->GetPdgName( pdgID ) ] << "  trackMC_id = "<< trackMC_id<< endl;
		
		double trackQuality = TrackQuality( &mcParticleID_track );
		if ( particle->GetCharge() == measCh && trackQuality > 0.7 ) {
			if ( m_nConvergedTracks_matched.find( fitTrackName ) == m_nConvergedTracks_matched.end() )	m_nConvergedTracks_matched[ fitTrackName ] = 0;
			m_nConvergedTracks_matched[ fitTrackName ]++;
		}
		// else
		// 	cin.get();


		h_momentum_true.at(measCh)->Fill( particle->GetInitP().Mag() );	// check if not present
		h_ratio_reco_true.at(measCh)->Fill( recoMom_target.Mag() - particle->GetInitP().Mag() );	// check if not present
		

	}
if(m_debug > 1)	cout << "TAGactKFitter::RecordTrackInfo:: DONE MC = "<< endl;
		
	// int firstState = 0;
	// int lastState = -1;
	// double length = track->getTrackLen( track->getCardinalRep(), firstState, finalState );
	// double length 		= track->getTrackLen( track->getCardinalRep() );
	double length 		= 1;
	double tof 			= 0;
	// double tof 			= track->getTOF( track->getCardinalRep() ) ;
if(m_debug > 1)	cout << "TAGactKFitter::RecordTrackInfo:: DONE length = " << length  << endl;
	double chi2 		= m_refFitter->getRedChiSqu(track, track->getCardinalRep());
if(m_debug > 1)	cout << "TAGactKFitter::RecordTrackInfo:: DONE chi2 = " << chi2  << endl;
	int ndof 			= track->getFitStatus( track->getCardinalRep() )->getNdf();
	double chisquare 	= track->getFitStatus( track->getCardinalRep() )->getChi2();
	double pVal 		= track->getFitStatus( track->getCardinalRep() )->getPVal();
if(m_debug > 1)	cout << "TAGactKFitter::RecordTrackInfo:: DONE chisquare = " << chisquare  << endl;
	// track->getFitStatus( track->getCardinalRep() )->isFitConverged();
	// track->getFitStatus( track->getCardinalRep() )->getNFailedPoints();
	// track->getFitStatus( track->getCardinalRep() )->isFitConvergedFully();

	h_isotopeDist->Fill( m_IsotopesIndex[ UpdatePDG::GetPDG()->GetPdgName( pdgID ) ] );  // check if not present
	h_nMeas->Fill ( nMeas );
	h_mass->Fill( mass );
	h_chi2->Fill( chi2 );
	h_chargeFlip->Fill( pdgCh - measCh );

	h_momentum->Fill( recoMom_target.Mag() );
	h_momentum_reco.at(measCh)->Fill( recoMom_target.Mag() );	// check if not present

	
	shoeOutTrackGenFit = m_outTrackRepoGenFit->NewTrack( fitTrackName, track, 
										(long)gTAGroot->CurrentEventId().EventNumber(), 
										pdgID, pdgCh, measCh, mass, length, tof, chi2, ndof, pVal, 
										&recoPos_target, &recoMom_target, &recoPos_target_cov, &recoMom_target_cov, 
										&shoeTrackPointRepo
										);

	shoeOutTrack = m_outTrackRepo->NewTrack( fitTrackName, 
										(long)gTAGroot->CurrentEventId().EventNumber(),
										pdgID, pdgCh, measCh, mass, length, tof, chi2, ndof, pVal, 
										&recoPos_target, &recoMom_target, &recoPos_target_cov, &recoMom_target_cov,
										&shoeTrackPointRepo
										);
	
	//TO BE IMPLEMENTED!!
	if ( TAGrecoManager::GetPar()->IsMC() )		
		shoeOutTrack->SetGeneratorParticleID ( trackMC_id );

if(m_debug > 1)	cout << "TAGactKFitter::RecordTrackInfo:: DONE FILL = "  << endl;

    //! Get the accumulated X/X0 (path / radiation length) of the material crossed in the last extrapolation.
    // virtual double getRadiationLenght() const = 0;

}




int TAGactKFitter::FindMostFrequent( vector<vector<int>>* mcParticleID_track ) {

	map<int,int> qualityMap;
	for ( unsigned int i=0; i<mcParticleID_track->size(); i++)
	{
		for(int j = 0; j < mcParticleID_track->at(i).size(); j++ )
			qualityMap[ mcParticleID_track->at(i).at(j) ]++;
	}

	int occurence = 0;
	int id = 0;
	for (auto &qm: qualityMap) {
		if ( occurence < qm.second ) {
			occurence = qm.second;
			id = qm.first;
		}
	}

	return id;

}





double TAGactKFitter::TrackQuality( vector<vector<int>>* mcParticleID_track ) {

	map<int,int> qualityMap;
	for ( unsigned int i=0; i<mcParticleID_track->size(); i++)
	{
		for(int j = 0; j < mcParticleID_track->at(i).size(); j++ )
			qualityMap[ mcParticleID_track->at(i).at(j) ]++;
	}

	if ( qualityMap.size() == 1 )	return 1;
	else {
		// find higher value
		int occurence = 0;
		int id = 0;
		for (auto &qm: qualityMap) {
			if ( occurence < qm.second ) {
				occurence = qm.second;
				id = qm.first;
			}
		}
		return occurence/mcParticleID_track->size();
	}

}









//----------------------------------------------------------------------------------------------------
void TAGactKFitter::GetMeasInfo( int detID, int hitID, int* iSensor, int* iClus, vector<int>* iPart ) {

	// check
	if ( detID != m_sensorIDmap->GetDetIDFromMeasID( hitID ) )
		cout << "ERROR -- TAGactKFitter::GetMeasInfo " << detID << " instead of " << m_sensorIDmap->GetDetIDFromMeasID( hitID ) << endl, exit(0);

	*iSensor = m_sensorIDmap->GetSensorIDFromMeasID( hitID );
	*iClus = m_sensorIDmap->GetHitIDFromMeasID( hitID );

	if ( TAGrecoManager::GetPar()->IsMC() )
		*iPart = m_measParticleMC_collection->at( hitID );

}






//----------------------------------------------------------------------------------------------------
void TAGactKFitter::GetMeasTrackInfo( int hitID, TVector3* pos, TVector3* posErr ) {

	// cout << "TAGactKFitter::GetMeasTrackInfo - start" << endl;

	string det = m_sensorIDmap->GetDetNameFromMeasID( hitID );

	int iSensor = m_sensorIDmap->GetSensorIDFromMeasID( hitID );
	int iClus = m_sensorIDmap->GetHitIDFromMeasID( hitID );

	// cout << "TAGactKFitter::GetMeasTrackInfo - start - " << det << " sensor= "<< iSensor << " clus="<< iClus << endl;

	if ( det == "VT" ) {
		TAVTcluster* clus = ( (TAVTntuCluster*) gTAGroot->FindDataDsc("vtClus","TAVTntuCluster")->Object() )->GetCluster( iSensor, iClus );
		*pos = m_GeoTrafo->FromVTLocalToGlobal( clus->GetPositionG() );
		*posErr = clus->GetPosError();
	}
	else if ( det == "IT" ) {
		TAITcluster* clus = ( (TAITntuCluster*) gTAGroot->FindDataDsc("itClus","TAITntuCluster")->Object() )->GetCluster( iSensor, iClus );
		*pos = m_GeoTrafo->FromITLocalToGlobal( clus->GetPositionG() );
		*posErr = clus->GetPosError();
	}
	else if ( det == "MSD" ) {
		TAMSDcluster* clus = ( (TAMSDntuCluster*) gTAGroot->FindDataDsc("msdClus","TAMSDntuCluster")->Object() )->GetCluster( iSensor, iClus );
		*pos = m_GeoTrafo->FromMSDLocalToGlobal( clus->GetPositionG() );
		*posErr = clus->GetPosError();
	}
	else if ( det == "TW" ) {
		TATWpoint* clus = ( (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint","TATWntuPoint")->Object() )->GetPoint( iClus );
		*pos = m_GeoTrafo->FromTWLocalToGlobal( clus->GetPositionG() );
		*posErr = clus->GetPosErrorG();
	}
	else {
		cout << "ERROR -- TAGactKFitter::GetMeasTrackInfo -- No correct detector name found: "<< det << endl, exit(0);
	}

}






//----------------------------------------------------------------------------------------------------
void TAGactKFitter::GetRecoTrackInfo ( int i, Track* track,
										TVector3* KalmanPos, TVector3* KalmanMom,
										TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov ) {


	// Get reco track kinematics and errors
	*KalmanPos = TVector3( (track->getFittedState(i).get6DState())[0],	(track->getFittedState(i).get6DState())[1],	(track->getFittedState(i).get6DState())[2] );
	*KalmanMom = TVector3( (track->getFittedState(i).get6DState())[3], (track->getFittedState(i).get6DState())[4],	(track->getFittedState(i).get6DState())[5] );

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
void TAGactKFitter::PrintPurity() {

	int nCollection = m_nConvergedTracks_all.size();
	h_purity = new TH1F( "h_purity", "h_purity", nCollection, 0, nCollection );

	int k = 0;

	for ( map<string, int>::iterator itTotNum = m_nConvergedTracks_all.begin(); itTotNum != m_nConvergedTracks_all.end(); itTotNum++ ) {
		k++;
		float eff = (float)m_nConvergedTracks_matched[ (*itTotNum).first ] / (*itTotNum).second;
		float kk = (float)m_nConvergedTracks_matched[ (*itTotNum).first ];
		float nn = (*itTotNum).second;
		float variance = ( (kk+1)*(kk+2)/((nn+2)*(nn+3)) ) - ( (kk+1)*(kk+1)/((nn+2)*(nn+2)) );
		if ( m_debug > -1 )		cout << "Purity " << (*itTotNum).first << " = " << eff << "  " << m_nConvergedTracks_matched[ (*itTotNum).first ]<< " " << (*itTotNum).second << endl;
		h_purity->SetBinContent(k, eff);
		h_purity->SetBinError(k, sqrt(variance));

		h_purity->GetXaxis()->SetBinLabel(k, (*itTotNum).first.c_str() );
	}

	AddHistogram(h_purity);

}






//----------------------------------------------------------------------------------------------------
void TAGactKFitter::PrintEfficiency() {

  int nCollection = m_nSelectedTrackCandidates.size();
  h_trackEfficiency = new TH1F( "TrackEfficiency", "TrackEfficiency", nCollection, 0, nCollection );

  int k = 0;

  for ( map<string, int>::iterator itTotNum = m_nSelectedTrackCandidates.begin(); itTotNum != m_nSelectedTrackCandidates.end(); itTotNum++ ) {
    k++;
    float eff = (float)m_nConvergedTracks_all[ (*itTotNum).first ] / (*itTotNum).second;
    float kk = (float)m_nConvergedTracks_all[ (*itTotNum).first ];
    float nn = (*itTotNum).second;
    float variance = ( (kk+1)*(kk+2)/((nn+2)*(nn+3)) ) - ( (kk+1)*(kk+1)/((nn+2)*(nn+2)) );
    if ( m_debug > -1 )		cout << "Efficiency " << (*itTotNum).first << " = " << eff << "  " << m_nConvergedTracks_all[ (*itTotNum).first ]<< " " << (*itTotNum).second << endl;
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
  // h_trackEfficiency->Draw("E");
  // mirror->SaveAs( (m_kalmanOutputDir+"/"+"TrackEfficiencyPlot.png").c_str() );
  // mirror->SaveAs( (m_kalmanOutputDir+"/"+"TrackEfficiencyPlot.root").c_str() );

  AddHistogram(h_trackEfficiency);

}











void TAGactKFitter::CreateHistogram()	{

	h_isotopeDist = new TH1F("mean number of tracks","mean number of tracks", 10, 0., 5.);
	AddHistogram(h_isotopeDist);

	// h_purity = new TH1F("purity", "purity", 20, 0., 1.5);
	// AddHistogram(h_purity);

	// h_efficiencyKalman = new TH1F("h_efficiencyKalman", "h_efficiencyKalman", 20, 0., 1.5);
	// AddHistogram(h_efficiencyKalman);

	// h_qOverp = new TH1F("qoverp_all", "qoverp_all", 100, 0., 2.);
	// AddHistogram(qoverp);

	h_nMeas = new TH1F("nMeas", "nMeas", 100, 0., 2.);
	AddHistogram(h_nMeas);  

	h_chargeFlip = new TH1F("h_chargeFlip", "h_chargeFlip", 100, 0., 2.);
	AddHistogram(h_chargeFlip);

  	h_mass = new TH1F("h_mass", "h_mass", 100, 0., 2.);
	AddHistogram(h_mass);

	h_particleFlip = new TH1F("h_particleFlip", "h_particleFlip", 100, 0., 2.);
	AddHistogram(h_particleFlip);

	h_chi2 = new TH1F("h_chi2", "h_chi2", 100, 0., 2.);
	AddHistogram(h_chi2);

	h_momentum = new TH1F("h_momentum", "h_momentum", 100, 0., 10.);
	AddHistogram(h_momentum);

	for (int i = 0; i < 9; ++i){
		h_momentum_true.push_back(new TH1F(Form("histoChargeTrue%d",i), "test", 25, 0.,10.));
		AddHistogram(h_momentum_true[i]);

		h_momentum_reco.push_back(new TH1F(Form("histoChargeReco%d",i), "test", 25, 0.,10.));
		AddHistogram(h_momentum_reco[i]);

		h_ratio_reco_true.push_back(new TH1F(Form("histoRatio%d",i), "test", 25, 0.,10.));
		AddHistogram(h_ratio_reco_true[i]);
	}


	// // map<string, map<float, TH1F*> > h_dPOverP_x_bin
	// for ( map<string, map<float, TH1F*> >::iterator collIt=h_dPOverP_x_bin.begin(); collIt != h_dPOverP_x_bin.end(); collIt++ )
	// 	for ( map<float, TH1F*>::iterator it=(*collIt).second.begin(); it != (*collIt).second.end(); it++ ) 
	// 		AddHistogram( (*it).second );

	// // map<string, TH1F*>* h_resoP_over_Pkf
	// for ( map<string, TH1F*>::iterator it=h_resoP_over_Pkf.begin(); it != h_resoP_over_Pkf.end(); it++ ) 
	// 		AddHistogram( (*it).second );

	// // map<string, TH1F*>* h_biasP_over_Pkf
	// for ( map<string, TH1F*>::iterator it=h_biasP_over_Pkf.begin(); it != h_biasP_over_Pkf.end(); it++ ) 
	// 		AddHistogram( (*it).second );

	// // map<string, TH1F*>* h_deltaP
	// for ( map<string, TH1F*>::iterator it=h_deltaP.begin(); it != h_deltaP.end(); it++ ) 
	// 		AddHistogram( (*it).second );


	SetValidHistogram(kTRUE);

	return;
}




// void TAGactKFitter::FillHisto_Resolution( TVector3 meas, TVector3 expected, double err, ... ) {

// 	for ( particleHp ) {
// 		TAGF_KalmanStudies->PrintMomentumResidual( TVector3 meas, TVector3 expected, double err, string hitSampleName, map<float, TH1F*>* h_dPOverP_x_bin )
// 	}

// }






// //----------------------------------------------------------------------------------------------------
// TVector3 TAGactKFitter::ExtrapolateToTarget( Track* trackToFit ){

  // //+++++++++++++++++++++++++++++++++++++++
  // genfit::TrackPoint* tp = trackToFit->getPointWithMeasurementAndFitterInfo( 0, trackToFit->getCardinalRep() );
  // if (tp == nullptr) {
  //   std::cout << "Track has no TrackPoint with fitterInfo! \n";
  //   TVector3 x(666,666,666);
  //   return x;
  // }
  // if ( (static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(trackToFit->getCardinalRep()))->hasForwardUpdate() )  == false) {
  //   TVector3 x(666,666,666);
  //   return x;
  // }

  // genfit::KalmanFittedStateOnPlane kfTest(*(static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(trackToFit->getCardinalRep()))->getForwardUpdate()));
  // // std::cout << "state before extrapolating back to reference plane \n";
  // // kfTest.Print();

  // trackToFit->getCardinalRep()->extrapolateToPlane(kfTest, m_detectorPlanes[18], true, false);;
  // // std::cout << "state after extrapolating back to reference plane \n";
  // // kfTest.Print();



  // TVector3 posi;
  // posi.SetXYZ(kfTest.getState()[3],kfTest.getState()[4], m_detectorPlanes[18]->getO().Z());

  // return posi;

// }










//----------------------------------------------------------------------------------------------------
// init matrix to zero
void TAGactKFitter::MatrixToZero( TMatrixD *matrix ) {
  for ( int j=0; j<matrix->GetNrows(); j++ ) {
    for ( int k=0; k<matrix->GetNcols(); k++ ) {
      (*matrix)(j,k) = 0;
    }
  }
}






//----------------------------------------------------------------------------------------------------
void TAGactKFitter::InitEventDisplay() {



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


void TAGactKFitter::EvaluateProjectionEfficiency(string* PartName, Track* fitTrack)
{
	int MeasId, PlaneId;
	int chargeHypo, chargeMC;

	if(m_debug > 0)	cout << "Check of charge hypo: " << endl;

	for(int i=0; i<fitTrack->getNumPointsWithMeasurement(); ++i)
	{
		MeasId = fitTrack->getPointWithMeasurement(i)->getRawMeasurement()->getHitId();
		PlaneId = m_sensorIDmap->GetFitPlaneIDFromMeasID( MeasId );
		chargeHypo = fitTrack->getCardinalRep()->getPDGCharge();
		
		m_NClusTrack.at( PlaneId )++;

		bool good = false;
		for(vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasId).end(); ++itTrackMC)
		{
			TAMCpart* particle = m_trueParticleRep->GetTrack(*itTrackMC);
			chargeMC = particle->GetCharge();

			if(m_debug > 0)	cout << "Plane::" << PlaneId << "\tChargeHypo::" << chargeHypo << "\tMCCharge::" << chargeMC << "\tMeasId::" << MeasId << "\tMCTrackId::" << *itTrackMC << "\tflagGood::" << good << endl;
			
			if(chargeHypo == chargeMC && !good)
			{
				m_NClusGood.at( PlaneId )++;
				good=true;
			}
		}

	}

	int chargeFromTW = m_selector->GetChargeFromTW( fitTrack );
	cout << "Charge From TW::" << chargeFromTW << endl;
	if(chargeFromTW == -1)
		return;
	

	m_NTWTracks++;

	if(chargeFromTW != fitTrack->getCardinalRep()->getPDGCharge())
	{
		if(m_debug > 0)	Info("EvaluateProjectionEfficiency()", "Charge Hypo (%d) wrong, changing to measured from TW (%d)", int(fitTrack->getCardinalRep()->getPDGCharge()), chargeFromTW);
		for(int i=0; i<fitTrack->getNumReps(); ++i)
		{
			if(fitTrack->getTrackRep(i)->getPDGCharge() == chargeFromTW)
			{
				fitTrack->setCardinalRep( i );
				break;
			}
		}

		double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(chargeFromTW) );
		int A_Hypo = round(mass_Hypo/m_AMU);

		TVector3 pos, mom;
		TVectorD seed = fitTrack->getStateSeed();
		pos.SetX(seed(0)); pos.SetY(seed(1)); pos.SetZ(seed(2));
		if(m_debug > 1)	pos.Print();

		mom.SetX(seed(3)); mom.SetY(seed(4)); mom.SetZ(seed(5));
		if(m_debug > 1)	mom.Print();


		mom.SetMag(TMath::Sqrt( pow(0.2*A_Hypo,2) + 2*mass_Hypo*0.2*A_Hypo ));
		if(m_debug > 1)	mom.Print();

		fitTrack->setStateSeed(pos,mom);

		//change name of the particle associated to track
		switch(chargeFromTW)
		{
			case 1:	*PartName = "H";	break;
			case 2:	*PartName = "He";	break;
			case 3:	*PartName = "Li";	break;
			case 4:	*PartName = "Be";	break;
			case 5:	*PartName = "B";	break;
			case 6:	*PartName = "C";	break;
			case 7:	*PartName = "N";	break;
			case 8:	*PartName = "O";	break;
			default:
				*PartName = "fail";	break;
		}
	}
	else
	{
		m_NTWTracksGoodHypo++;

	}

}










