/*!
 \file TAGactKFitter.cxx
 \brief  Main class of the GenFit Global Reconstruction -> Action
 \author M. Franchini, R. Zarrella and R. Ridolfi
*/
#include "TAGparTools.hxx"
#include "TAGactKFitter.hxx"

/*!
 \class TAGactKFitter
 \brief Main action of the GenFit Global Reconstruction
*/

// ClassImp(TAGactKFitter);

//----------------------------------------------------------------------------------------------------

//! \brief Default constructor for GenFit Kalman fitter
TAGactKFitter::TAGactKFitter (const char* name, TAGdataDsc* outTrackRepo) : TAGaction(name, "TAGactKFitter - Global GenFit Tracker"),
		fpGlobTrackRepo(outTrackRepo),
		m_IsMC(false) {

	AddDataOut(outTrackRepo, "TAGntuGlbTrack");

	int nIter = 20; // max number of iterations
	double dPVal = 1.E-3; // convergence criterion
	m_AMU = 0.9310986964; // in GeV // conversion betweem mass in GeV and atomic mass unit
	m_BeamEnergy = ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().Energy;

	gGeoManager->ClearPhysicalNodes();

	m_SensorIDMap = new TAGFdetectorMap();
	genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());
	genfit::MaterialEffects::getInstance()->setNoEffects(false);

	m_Particles = { "H", "He", "Li", "Be", "B", "C", "N", "O" };
	for ( unsigned int i=0; i<m_Particles.size(); i++ )
		m_ParticleIndex[ m_Particles[i] ] = i;

	m_Isotopes  = {		"H1", "H2", "H3",
						"He3", "He4", "He6", "He8",
						"Li6", "Li7", "Li8", "Li9",
						"Be7", "Be9", "Be10", "Be11", "Be12", "Be14",
						"B8", "B10", "B11", "B12", "B13", "B14", "B15",
						"C9", "C10", "C11", "C12", "C13", "C14", "C15", "C16",
						"N12", "N13", "N14", "N15", "N16",
						"O13", "O14", "O15", "O16" };
	for ( unsigned int i=0; i<m_Isotopes.size(); i++ )
		m_IsotopesIndex[ m_Isotopes[i] ] = i;

	m_debug = TAGrecoManager::GetPar()->Debug();

	m_mapTrack.clear();

	m_outTrackRepo = (TAGntuGlbTrack*) fpGlobTrackRepo->Object();

	// checks for the detector to be used for kalman
	IncludeDetectors();
	CreateGeometry();

	m_fitter_extrapolation = new KalmanFitter(1, dPVal);

	// initialise the kalman method selected from param file
	if ( TAGrecoManager::GetPar()->KalMode() == "on" )
		m_fitter = new KalmanFitter(nIter, dPVal);
	else if ( TAGrecoManager::GetPar()->KalMode() == "ref" )
		m_refFitter = new KalmanFitterRefTrack(nIter, dPVal);
	else if ( TAGrecoManager::GetPar()->KalMode() == "daf" )
		m_dafRefFitter = new DAF(true, nIter, dPVal);
	else if ( TAGrecoManager::GetPar()->KalMode() == "dafsimple" )
		m_dafSimpleFitter = new DAF(false, nIter, dPVal);
	else
	{
		Error("TAGactKFitter()", "Undexpected value for Kalman Mode! Given %s", TAGrecoManager::GetPar()->KalMode().c_str());
		exit(0);
	}
	if ( TAGrecoManager::GetPar()->EnableEventDisplay() )	    InitEventDisplay();

	TAGrecoManager::GetPar()->Print("all");
	cout << "TAGactKFitter::TAGactKFitter -- 1" << endl;
	// m_uploader = new TAGFuploader( m_SensorIDMap );
	cout << "TAGactKFitter::TAGactKFitter -- 2" << endl;
	m_trackAnalysis = new TAGF_KalmanStudies();

	for(int i=0; i<m_SensorIDMap->GetFitPlanesN(); ++i)
	{
		m_NClusTrack.push_back(0);
		m_NClusGood.push_back(0);
	}

	m_NTWTracks = 0;
	m_NTWTracksGoodHypo = 0;
	m_numGenParticle_noFrag = 0;

	m_singleVertexCounter = 0;
	m_noVTtrackletEvents = 0;
	m_noTWpointEvents = 0;
}






//----------------------------------------------------------------------------------------------------

//! \brief Default destructor
//!
//! Delete all the objects used for Kalman Filter extrapolation
TAGactKFitter::~TAGactKFitter() {
	ClearData();
	// if(TAGrecoManager::GetPar()->IsSaveHisto())
	// 	ClearHistos();

	delete m_fitter;
	delete m_fitter_extrapolation;
	delete m_refFitter;
	delete m_dafRefFitter;
	delete m_dafSimpleFitter;

	delete m_trueParticleRep;
	// delete m_outTrackRepo;
	delete m_SensorIDMap;
	delete m_trackAnalysis;

	m_Particles.clear();
	m_ParticleIndex.clear();
	m_Isotopes.clear();
	m_IsotopesIndex.clear();
	m_vecHistoColor.clear();

	delete display;
	delete m_TopVolume;
	delete m_GeoTrafo;

	m_NClusGood.clear();
	m_NClusTrack.clear();
	m_genCount_vector.clear();
	m_nConvergedTracks_all.clear();
	m_nConvergedTracks_matched.clear();
	m_nSelectedTrackCandidates.clear();
}



//----------------------------------------------------------------------------------------------------
//! \brief Set the needed variables when MC sample is available
void TAGactKFitter::SetMcSample()
{
	m_IsMC = true;
	m_trueParticleRep = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();
}



//----------------------------------------------------------------------------------------------------

//! \brief Fill counter of all the MC paticles generated in the FOOT setup
//! \param[in] mappa Map containing the name and number of all MC particles in the event
void TAGactKFitter::FillGenCounter( map< string, int > mappa )	{

	for( map<string, int>::iterator it = mappa.begin(); it != mappa.end(); ++it )	{

		if ( m_genCount_vector.find( (*it).first ) == m_genCount_vector.end() )
			m_genCount_vector[ (*it).first ] = 0;

		m_genCount_vector.at( (*it).first ) += (*it).second;
	}

}




//------------------------------------------+-----------------------------------

//! \brief Main action of the class
//!
//! Upload clusters/points in GenFit format, categorize them and fit the selected tracks
//! \return True if the action was successful
Bool_t TAGactKFitter::Action()	{
	ClearData();

	long evNum = (long)gTAGroot->CurrentEventId().EventNumber();

	if(m_debug > 0) cout << "TAGactKFitter::Action()  ->  " << m_allHitMeasGF.size() << endl;

	TAGFuploader* m_uploader = new TAGFuploader( m_SensorIDMap );

	m_uploader->TakeMeasHits4Fit( m_allHitMeasGF );
	vector<int> chVect;
	m_uploader->GetPossibleCharges( &chVect, m_IsMC );

	if ( m_IsMC ) {
		m_measParticleMC_collection = *(m_uploader->TakeMeasParticleMC_Collection());
		m_numGenParticle_noFrag += m_uploader->GetNumGenParticle_noFrag();
	}

	if(m_debug > 0)	{
		cout << "TAGactKFitter::Action()  ->  " << m_allHitMeasGF.size() << endl;
		cout << "Plane\tN. hits" << endl;
		for(auto it = m_allHitMeasGF.begin(); it != m_allHitMeasGF.end(); ++it)
			cout << it->first << "\t" << it->second.size() << endl;
	}

	// TAGFselectorBase* m_selector = new TAGFselectorBase(&m_allHitMeasGF, &chVect, m_SensorIDMap, &m_mapTrack, &m_measParticleMC_collection, m_IsMC, &m_singleVertexCounter, &m_noVTtrackletEvents);
	TAGFselectorBase* m_selector;
	if (TAGrecoManager::GetPar()->PreselectStrategy() == "TrueParticle")
	{
		if (!m_IsMC)
			Error("TAGactKFitter::Action()", "Asked TrueParticle tracking but running not on MC."), exit(0);
		m_selector = new TAGFselectorTrue();
	}
	else if (TAGrecoManager::GetPar()->PreselectStrategy() == "Standard")
		m_selector = new TAGFselectorStandard();
	else if (TAGrecoManager::GetPar()->PreselectStrategy() == "Linear")
		m_selector = new TAGFselectorLinear();
	else if (TAGrecoManager::GetPar()->PreselectStrategy() == "Backtracking")
		m_selector = new TAGFselectorBack();
	else
		Error("TAGactKFitter::Action()", "TAGrecoManager::GetPar()->PreselectStrategy() not defined"), exit(0);

	m_selector->SetVariables(&m_allHitMeasGF, &chVect, m_SensorIDMap, &m_mapTrack, &m_measParticleMC_collection, m_IsMC, &m_singleVertexCounter, &m_noVTtrackletEvents, &m_noTWpointEvents);

	if (m_selector->FindTrackCandidates() >= 0)
	{

		if ( m_IsMC ) {
			//RZ: Check selection efficiency counts --> better define the "visible" particles, right now is not really compatible with TrueParticle selection
			FillGenCounter( m_selector->CountParticleGenaratedAndVisible() );
		}

		MakeFit(evNum, m_selector);
	}
	if( TAGrecoManager::GetPar()->IsSaveHisto() )
	{
		m_selector->FillPlaneOccupancy(h_PlaneOccupancy);
		h_GFeventType->Fill(m_selector->GetEventType());
	}

	chVect.clear();

	delete m_selector;
	delete m_uploader;

	fpGlobTrackRepo->SetBit(kValid);
	return true;

}









//----------------------------------------------------------------------------------------------------

//! \brief Finalize all the needed histograms for GenFit studies
//!
//! Save control plots and calculate resolution. Called from outside, at the end of the event cycle
void TAGactKFitter::Finalize() {

  // make a directory for each hit category that forms a track candidate
  // struct stat info;
  // for ( unsigned int i=0; i < m_categoryFitted.size(); i++ ) {
  //   string pathName = m_kalmanOutputDir+"/"+m_categoryFitted.at(i);
  //   if( stat( pathName.c_str(), &info ) != 0 )		//cannot access
  //     system(("mkdir "+pathName).c_str());
  // }

	// if ( m_IsMC ) 		m_trackAnalysis->EvaluateAndFill_MomentumResolution( &h_dPOverP_x_bin, &h_resoP_over_Pkf, &h_biasP_over_Pkf );
	if ( m_IsMC )	PrintPurity();
	PrintEfficiency();
	if ( m_IsMC )	PrintSelectionEfficiency();

	if( ValidHistogram() )
	{
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

		TH1F* h_deltaP_tot = new TH1F();
		TH1F* h_sigmaP_tot = new TH1F();

		// map<string, TH1F*>* h_deltaP
		int count = 0;
		for ( map<string, TH1F*>::iterator it=h_deltaP.begin(); it != h_deltaP.end(); it++ ) {
			if ( count == 0 ) 	h_deltaP_tot = (TH1F*)((*it).second)->Clone("dP");
			else 				h_deltaP_tot->Add( (*it).second, 1 );
			AddHistogram( (*it).second );
			count++;
		}
		h_deltaP_tot->SetTitle( "dP" );
		AddHistogram( h_deltaP_tot );

		// map<string, TH1F*>* h_sigmaP
		count=0;
		for ( map<string, TH1F*>::iterator it=h_sigmaP.begin(); it != h_sigmaP.end(); it++ ) {
			if ( count == 0 ) 	h_sigmaP_tot = (TH1F*) (*it).second->Clone();
			else 				h_sigmaP_tot->Add( (*it).second, 1 );
			AddHistogram( (*it).second );
			count++;
		}
		h_sigmaP_tot->SetNameTitle( "errdP", "errdP" );
		AddHistogram( h_sigmaP_tot );


		TH1F* h_numGenParticle_noFrag = new TH1F( "h_numGenParticle_noFrag", "h_numGenParticle_noFrag", 100, 0, 10000 );
		AddHistogram( h_numGenParticle_noFrag );
		h_numGenParticle_noFrag->Fill( m_numGenParticle_noFrag );
		cout << "m_numGenParticle_noFrag = " << m_numGenParticle_noFrag << endl;


		cout << "TAGactKFitter::Finalize() -- END"<< endl;
		SetValidHistogram(kTRUE);
		SetHistogramDir(fDirectory);
	}

	cout << "Single vertex events::" << m_singleVertexCounter << endl;
	cout << "Events w/out valid VT tracklets::" << m_noVTtrackletEvents << endl;
	cout << "Events w/out valid TW point::" << m_noTWpointEvents << endl;

	//show event display
	if ( TAGrecoManager::GetPar()->EnableEventDisplay() )
	{
		display->setOptions("BDEFHGMPT");
		display->open();
	}

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

//! \brief Check and print which detectors included and/or used in the kalman
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
			if ( !m_SensorIDMap->IsDetectorInMap( TAGrecoManager::GetPar()->KalSystems().at(i) ) )
			{
				Error("IncludeDetectors()", "KalSystems parameter not set properly! Detector '%s' not found in global map.", TAGrecoManager::GetPar()->KalSystems().at(i).c_str());
				throw -1;
			}
		}
	}

	// list of detectors used for kalman
	m_systemsON = "";
	if( TAGrecoManager::GetPar()->KalSystems().at(0) == "all" )
	{
		if(TAGrecoManager::GetPar()->IncludeVT())	m_systemsON += "VT ";
		if(TAGrecoManager::GetPar()->IncludeIT())	m_systemsON += "IT ";
		if(TAGrecoManager::GetPar()->IncludeMSD())	m_systemsON += "MSD ";
		if(TAGrecoManager::GetPar()->IncludeTW())	m_systemsON += "TW";
	}
	else
	{
		for (unsigned int i=0; i<TAGrecoManager::GetPar()->KalSystems().size(); i++ ) {
			if (i != 0)		m_systemsON += " ";
			m_systemsON += TAGrecoManager::GetPar()->KalSystems().at(i);
		}
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

//! \brief Create the FOOT geometry and declare the detectors in GenFit format
void TAGactKFitter::CreateGeometry()  {

	if(m_debug > 0)	cout << "TAGactKFitter::CreateGeometry() -- START" << endl;

	// take geometry objects
	if (TAGrecoManager::GetPar()->IncludeST())
		m_ST_geo = shared_ptr<TASTparGeo> ( (TASTparGeo*) gTAGroot->FindParaDsc("stGeo", "TASTparGeo")->Object() );

	if (TAGrecoManager::GetPar()->IncludeBM())
		m_BM_geo = shared_ptr<TABMparGeo> ( (TABMparGeo*) gTAGroot->FindParaDsc("bmGeo", "TABMparGeo")->Object() );

	if (TAGrecoManager::GetPar()->IncludeTG())
		m_TG_geo = shared_ptr<TAGparGeo> ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() );

	if (TAGrecoManager::GetPar()->IncludeDI())
		m_DI_geo = shared_ptr<TADIparGeo> ( (TADIparGeo*) gTAGroot->FindParaDsc("diGeo", "TADIparGeo")->Object() );

	if ( TAGrecoManager::GetPar()->IncludeVT() )
		m_VT_geo = shared_ptr<TAVTparGeo> ( (TAVTparGeo*) gTAGroot->FindParaDsc("vtGeo", "TAVTparGeo")->Object() );

	if ( TAGrecoManager::GetPar()->IncludeIT() )
		m_IT_geo = shared_ptr<TAITparGeo> ( (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object() );

	if ( TAGrecoManager::GetPar()->IncludeMSD() )
		m_MSD_geo = shared_ptr<TAMSDparGeo> ( (TAMSDparGeo*) gTAGroot->FindParaDsc("msdGeo", "TAMSDparGeo")->Object() );

	if ( TAGrecoManager::GetPar()->IncludeTW() )
		m_TW_geo = shared_ptr<TATWparGeo> ( (TATWparGeo*) gTAGroot->FindParaDsc("twGeo", "TATWparGeo")->Object() );

	if (TAGrecoManager::GetPar()->IncludeCA())
		m_CA_geo = shared_ptr<TACAparGeo> ( (TACAparGeo*) gTAGroot->FindParaDsc("caGeo", "TACAparGeo")->Object() );


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

	// ST
	if (TAGrecoManager::GetPar()->IncludeST()) {
		TGeoVolume* stVol = m_ST_geo->BuildStartCounter();
		stVol->SetLineColor(kBlack);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TASTparGeo::GetBaseName());
		m_TopVolume->AddNode(stVol, 1, transfo);
	}

	// BM
	if (TAGrecoManager::GetPar()->IncludeBM()) {
		TGeoVolume* bmVol = m_BM_geo->BuildBeamMonitor();
		bmVol->SetLineColor(kBlack);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TABMparGeo::GetBaseName());
		m_TopVolume->AddNode(bmVol, 2, transfo);
	}


	// target
	if (TAGrecoManager::GetPar()->IncludeTG()) {
		TGeoVolume* tgVol = m_TG_geo->BuildTarget();
		tgVol->SetLineColor(kBlack);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAGparGeo::GetBaseName());
		m_TopVolume->AddNode(tgVol, 3, transfo);

		DetPlane* targetPlane;
		TVector3 TGsize = m_TG_geo->GetTargetPar().Size;
		TVector3 origin_( m_GeoTrafo->FromTGLocalToGlobal(m_TG_geo->GetTargetPar().Position) );
		if(m_TG_geo->GetTargetPar().Shape == "cubic")
		{
			genfit::AbsFinitePlane* targetArea = new RectangularFinitePlane(-TGsize.x()/2, TGsize.x()/2, -TGsize.y()/2, TGsize.y()/2);
			//Target area is now defined in LOCAL coordinates
			targetPlane = new genfit::DetPlane(origin_, TVector3(0,0,1), targetArea);
		}
		else
			targetPlane = new genfit::DetPlane(origin_, TVector3(0,0,1));

		genfit::SharedPlanePtr detectorplane(targetPlane);
		m_SensorIDMap->AddFitPlane(-42, detectorplane);
		m_SensorIDMap->AddFitPlaneIDToDet(-42, "TG");
	}

	// Vertex
	if (TAGrecoManager::GetPar()->IncludeVT()) {
		TGeoVolume* vtVol  = m_VT_geo->BuildVertex();
		vtVol->SetLineColor(kRed+1);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAVTparGeo::GetBaseName());
		m_TopVolume->AddNode(vtVol, 4, transfo);

		if(m_systemsON.Contains("VT"))
		{
			for ( int i = 0; i < m_VT_geo->GetSensorsN(); ++i ) {
				TVector3 origin_(m_GeoTrafo->FromVTLocalToGlobal(m_VT_geo->GetSensorPosition(i)) );

				// RZ, note to self: Careful w/ coordinates here: the "IsInActive" functions uses exactly the coordinates give to define the active area, either they are local or global!! BE CONSISTENT AND RE-CHECK EVERYTHING
				float xMin, xMax, yMin, yMax;
				xMin = m_VT_geo->GetEpiOffset().X() - m_VT_geo->GetEpiSize().X()/2;
				xMax = m_VT_geo->GetEpiOffset().X() + m_VT_geo->GetEpiSize().X()/2;
				yMin = m_VT_geo->GetEpiOffset().Y() - m_VT_geo->GetEpiSize().Y()/2;
				yMax = m_VT_geo->GetEpiOffset().Y() + m_VT_geo->GetEpiSize().Y()/2;
				genfit::AbsFinitePlane* activeArea = new RectangularFinitePlane(xMin, xMax, yMin, yMax);
				TVector3 normal_versor = TVector3(0,0,1);
				TVector3 trafoNorm = m_GeoTrafo->VecFromVTLocalToGlobal(m_VT_geo->Sensor2DetectorVect(i, normal_versor));
				genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, trafoNorm, activeArea));

				//Set versors
				TVector3 U(1.,0,0);
				TVector3 V(0,1.,0);
				TVector3 trafoU = m_GeoTrafo->VecFromVTLocalToGlobal(m_VT_geo->Sensor2DetectorVect(i, U));
				TVector3 trafoV = m_GeoTrafo->VecFromVTLocalToGlobal(m_VT_geo->Sensor2DetectorVect(i, V));
				// Some debug print-outs for geometry
				if(m_debug > 1)
				{
					cout << "VT sensor::" << i << endl;
					cout << "origin::"; origin_.Print();
					cout << "Boundaries::\tx=["<< xMin << "," << xMax << "]\ty=[" << yMin << "," << yMax << "]\n";
					cout << "U::"; U.Print();
					cout << "V::"; V.Print();
					cout << "trafoU::"; trafoU.Print();
					cout << "trafoV::"; trafoV.Print();
					cout << "Z versor::"; trafoNorm.Print();
				}
				detectorplane->setUV(trafoU, trafoV);
				m_SensorIDMap->AddFitPlane(indexOfPlane, detectorplane);
				m_SensorIDMap->AddFitPlaneIDToDet(indexOfPlane, "VT");
				++indexOfPlane;
			}
		}
	}

	// Magnet
	if (TAGrecoManager::GetPar()->IncludeDI()) {
		TGeoVolume* diVol = m_DI_geo->BuildMagnet();
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TADIparGeo::GetBaseName());
		m_TopVolume->AddNode(diVol, 5, transfo);
	}

	// IT
	if (TAGrecoManager::GetPar()->IncludeIT()) {
		TGeoVolume* itVol  = m_IT_geo->BuildInnerTracker();
		itVol->SetLineColor(kRed);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAITparGeo::GetBaseName());
		m_TopVolume->AddNode(itVol, 6, transfo);

		if( m_systemsON.Contains("IT") )
		{
			for ( int i = 0; i < m_IT_geo->GetSensorsN(); i++ ) {
				TVector3 origin_(m_GeoTrafo->FromITLocalToGlobal(m_IT_geo->GetSensorPosition(i)) );
				float xMin, xMax, yMin, yMax;
				xMin = m_IT_geo->GetEpiOffset().X() - m_IT_geo->GetEpiSize().X()/2;
				xMax = m_IT_geo->GetEpiOffset().X() + m_IT_geo->GetEpiSize().X()/2;
				yMin = m_IT_geo->GetEpiOffset().Y() - m_IT_geo->GetEpiSize().Y()/2;
				yMax = m_IT_geo->GetEpiOffset().Y() + m_IT_geo->GetEpiSize().Y()/2;

				// This make all the 32 IT sensors
				genfit::AbsFinitePlane* activeArea = new RectangularFinitePlane( xMin, xMax, yMin, yMax );
				TVector3 normal_versor = TVector3(0,0,1);
				TVector3 trafoNorm = m_GeoTrafo->VecFromITLocalToGlobal(m_IT_geo->Sensor2DetectorVect(i, normal_versor));
				genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, trafoNorm, activeArea));

				// Set versors
				TVector3 U(1.,0,0);
				TVector3 V(0,1.,0);
				TVector3 trafoU = m_GeoTrafo->VecFromITLocalToGlobal(m_IT_geo->Sensor2DetectorVect(i, U));
				TVector3 trafoV = m_GeoTrafo->VecFromITLocalToGlobal(m_IT_geo->Sensor2DetectorVect(i, V));
				detectorplane->setUV(trafoU, trafoV);

				m_SensorIDMap->AddPlane_Zorder( origin_.Z(), indexOfPlane );
				m_SensorIDMap->AddPlane_ZorderLocal( m_IT_geo->GetSensorPosition(i).Z(), indexOfPlane );

				m_SensorIDMap->AddFitPlane(indexOfPlane, detectorplane);
				m_SensorIDMap->AddFitPlaneIDToDet(indexOfPlane, "IT");
				++indexOfPlane;
				if( m_debug > 1 )
					cout << "IT plane::" << indexOfPlane << "\tZ::" << origin_.Z() << endl;

				// Some debug print-outs for geometry
				if(m_debug > 1)
				{
					cout << "IT sensor::" << i << endl;
					cout << "origin::"; origin_.Print();
					cout << "Boundaries::\tx=["<< xMin << "," << xMax << "]\ty=[" << yMin << "," << yMax << "]\n";
					cout << "U::"; U.Print();
					cout << "V::"; V.Print();
					cout << "trafoU::"; trafoU.Print();
					cout << "trafoV::"; trafoV.Print();
				}
			}
		}
	}

	// MSD
	if (TAGrecoManager::GetPar()->IncludeMSD()) {
		TGeoVolume* msdVol = m_MSD_geo->BuildMicroStripDetector();
		msdVol->SetLineColor(kViolet);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAMSDparGeo::GetBaseName());
		m_TopVolume->AddNode(msdVol, 7, transfo);

		if( m_systemsON.Contains("MSD") )
		{
			for ( int i = 0; i < m_MSD_geo->GetSensorsN(); i++ ) {
				TVector3 origin_( m_GeoTrafo->FromMSDLocalToGlobal(m_MSD_geo->GetSensorPosition(i)) );

				float xMin = m_MSD_geo->GetEpiOffset().x() - m_MSD_geo->GetEpiSize().x()/2;
				float xMax = m_MSD_geo->GetEpiOffset().x() + m_MSD_geo->GetEpiSize().x()/2;
				float yMin = m_MSD_geo->GetEpiOffset().y() - m_MSD_geo->GetEpiSize().y()/2;
				float yMax = m_MSD_geo->GetEpiOffset().y() + m_MSD_geo->GetEpiSize().y()/2;

				TVector3 normal_versor = TVector3(0,0,1);
				TVector3 trafoNorm = m_GeoTrafo->VecFromMSDLocalToGlobal(m_MSD_geo->Sensor2DetectorVect(i, normal_versor));
				genfit::AbsFinitePlane* activeArea = new RectangularFinitePlane( xMin, xMax, yMin, yMax );
				genfit::SharedPlanePtr detectorplane ( new genfit::DetPlane( origin_, trafoNorm, activeArea) );

				// Set versors -> MSD still needs some fixes maybe
				TVector3 U(1.,0,0);
				TVector3 V(0,1.,0);
				TVector3 trafoU = m_GeoTrafo->VecFromMSDLocalToGlobal(m_MSD_geo->Sensor2DetectorVect(i,U));
				TVector3 trafoV = m_GeoTrafo->VecFromMSDLocalToGlobal(m_MSD_geo->Sensor2DetectorVect(i,V));
				// detectorplane->setUV(U, V);
				detectorplane->setUV(trafoU, trafoV);

				m_SensorIDMap->AddFitPlane(indexOfPlane, detectorplane);
				m_SensorIDMap->AddFitPlaneIDToDet(indexOfPlane, "MSD");
				++indexOfPlane;

				// Some debug print-outs for geometry
				if(m_debug > 0)
				{
					cout << "MSD sensor::" << i << endl;
					cout << "origin::"; origin_.Print();
					cout << "Boundaries::\tx=["<< xMin << "," << xMax << "]\ty=[" << yMin << "," << yMax << "]\n";
					cout << "U::"; U.Print();
					cout << "V::"; V.Print();
					cout << "trafoU::"; trafoU.Print();
					cout << "trafoV::"; trafoV.Print();
					cout << "trafoNorm::"; trafoNorm.Print();
					cout << "SensorType::"<< m_MSD_geo->GetSensorPar(i).TypeIdx << endl;
				}
			}
		}
	}

	// TW -> RZ: APPLY ROTATIONS TO TW AT SOME POINT
	if (TAGrecoManager::GetPar()->IncludeTW()) {
		TGeoVolume* twVol = m_TW_geo->BuildTofWall();
		twVol->SetLineColor(kBlue);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TATWparGeo::GetBaseName());
		m_TopVolume->AddNode(twVol, 8, transfo);


		if( m_systemsON.Contains("TW") )
		{
			TVector3 origin_( m_GeoTrafo->FromTWLocalToGlobal(m_TW_geo->GetLayerPosition(1)));
			genfit::SharedPlanePtr detectorplane (new genfit::DetPlane(origin_, TVector3(0,0,1)));

			//RZ, note to self: Set the active area of the TW!!!

			// Set versors -> maybe trafo versors not needed
			TVector3 U(1.,0,0);
			TVector3 V(0,1.,0);
			// TVector3 trafoU = m_TW_geo->Detector2SensorVect(i, u);
			// TVector3 trafoV = m_TW_geo->Detector2SensorVect(i, v);
			// detectorplane->setU(trafoU);
			// detectorplane->setV(trafoV);
			detectorplane->setU(U);
			detectorplane->setV(V);

			m_SensorIDMap->AddFitPlane(indexOfPlane, detectorplane);
			m_SensorIDMap->AddFitPlaneIDToDet(indexOfPlane, "TW");
			++indexOfPlane;

			// Some debug print-outs for geometry
			if(m_debug > 1)
			{
				cout << "TW geometry" << endl;
				cout << "origin::"; origin_.Print();
				// cout << "Boundaries::\tx=["<< xMin << "," << xMax << "]\ty=[" << yMin << "," << yMax << "]\n";
				cout << "U::"; U.Print();
				cout << "V::"; V.Print();
			}
		}
	}

	// CA
	if (TAGrecoManager::GetPar()->IncludeCA()) {
		TGeoVolume* caVol = m_CA_geo->BuildCalorimeter();
		caVol->SetLineColor(kBlack);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TACAparGeo::GetBaseName());
		m_TopVolume->AddNode(caVol, 9, transfo);
	}

	if(m_debug > 0)	cout << "TAGactKFitter::CreateGeometry() -- STOP\n";

}





//----------------------------------------------------------------------------------------------------

//! \brief Perform the actual fit of the selected tracks
//!
//! \param[in] evNum Event number
//! \return Number of fitted tracks in the event
int TAGactKFitter::MakeFit( long evNum , TAGFselectorBase* m_selector) {

	if ( m_debug > 0 )		cout << "Starting MakeFit " << endl;

	int isConverged = 0;
	int trackCounter = -1;
	int NconvTracks = 0;

	m_evNum = evNum;
	if(m_debug > 0)
		cout << "\n  ----------------------\nEvento numero " << m_evNum << " track " << m_mapTrack.size() << endl;

	// loop over all hit category
	for ( map<TString,Track*>::iterator trackIt = m_mapTrack.begin(); trackIt != m_mapTrack.end(); ++trackIt) {
		if(trackIt->first == "dummy")
		{
			trackIt->second->setStateSeed(TVector3(0,0,0), TVector3(0,0,10));
			trackIt->second->addTrackRep(new RKTrackRep(UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( 8 )));
			KalmanFitter* preFitter = new KalmanFitter(1, 1e-3);
	    	preFitter->processTrackWithRep( trackIt->second, trackIt->second->getCardinalRep() );
			m_vectorConvergedTrack.push_back(trackIt->second);
			delete preFitter;
			continue;
		}


		vector<string> tok = TAGparTools::Tokenize( trackIt->first.Data() , "_" );
		string PartName = tok.at(0);

		if(m_debug > 0) cout << "Track candidate: "<<trackCounter<< "  "<< PartName << " " << trackIt->first.Data() << "\n";

		// check if the category is defined in UpdatePDG  -->  also done in GetPdgCode()
		// This check has to be done only when using the TrueParticle selection -> Naming is different for data-Like!
		if ( TAGrecoManager::GetPar()->PreselectStrategy() == "TrueParticle" )  {

			if ( !UpdatePDG::GetPDG()->IsParticleDefined( tok.at(0) + tok.at(1) ) )
			{
				Error("MakeFit()", "Category %s%s not found in UpdatePDG!" , tok.at(0).c_str(), tok.at(1).c_str());
				throw -1;
			}
		}

		// tmp track to be used inside the loop
		Track* fitTrack = trackIt->second;

		trackCounter++;

	    // check of fitTrack filling
	    if ( m_debug > 0 ) {
		    cout << " check of fitTrack filling " << endl;
		    for (unsigned int iMeas = 0; iMeas < fitTrack->getNumPointsWithMeasurement(); ++iMeas){

				fitTrack->getPointWithMeasurement(iMeas)->getRawMeasurement()->getRawHitCoords().Print();
				int indexOfPlane = static_cast<genfit::PlanarMeasurement*> ( fitTrack->getPointWithMeasurement(iMeas)->getRawMeasurement() )->getPlaneId();

				cout << " index of plane " << indexOfPlane << "  pointID " << iMeas << "\n";
		  	}
		  	cout << "\n";
	    }
		if ( fitTrack->getNumPointsWithMeasurement() < TAGrecoManager::GetPar()->MeasureN() )
		{
			if( m_debug > 0 )	Info("FillTrackCategoryMap()", "Skipped Track %s with %d TrackPoints with measurement!!", tok.at(2).c_str(), fitTrack->getNumPointsWithMeasurement());
			continue;
		}


	    if ( TAGrecoManager::GetPar()->IsKalReverse() )
	    	fitTrack->reverseTrackPoints();

	    //check
	    fitTrack->checkConsistency();
	    if ( m_debug > 2 )	    fitTrack->Print();

		if( m_IsMC )	EvaluateProjectionEfficiency(fitTrack);

		if( TAGrecoManager::GetPar()->PreselectStrategy() != "TrueParticle" )
			CheckChargeHypothesis(&PartName, fitTrack, m_selector);

		std::string newTrackName = trackIt->first.Data();
		if(PartName != tok.at(0))
		{
			int NewCharge = m_ParticleIndex[PartName] + 1;
			int NewMass = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(NewCharge) );
			newTrackName = Form("%s_%d_%s", PartName.c_str(), NewMass, tok.at(2).c_str());
		}

		// map of the number of converged tracks for each track hypothesis
		if ( m_nSelectedTrackCandidates.find( PartName ) == m_nSelectedTrackCandidates.end() )	m_nSelectedTrackCandidates[ PartName ] = 0;
		m_nSelectedTrackCandidates[ PartName ]++;

		if(m_debug > 0)	{
			cout << "PROCESSING TRACK!!!!!\n";
			cout << fitTrack->getCardinalRep()->getPDG() << "\t" << fitTrack->getCardinalRep()->getPDGCharge()<< "\n";
			fitTrack->getCardinalRep()->Print();
		}

	    // THE REAL FIT with different Kalman modes
	    try{

	    	double dPVal = 1.E-3; // convergence criterion
	    	KalmanFitter* preFitter = new KalmanFitter(1, dPVal);
	    	preFitter->processTrackWithRep( fitTrack, fitTrack->getCardinalRep() );

			if(m_debug > 0)
			{
				cout << "Track reps\n";
				for(int i = 0; i < fitTrack->getNumReps(); ++i)
					fitTrack->getTrackRep(i)->Print();
				cout << "Cardinal rep: ";
				fitTrack->getCardinalRep()->Print();
			}

			if ( TAGrecoManager::GetPar()->KalMode() == "on" ) {
				if(m_debug > 1)
					std::cout << "Event::" << m_evNum << "\tTrack::" << trackCounter << std::endl;
				m_fitter->processTrackWithRep( fitTrack, fitTrack->getCardinalRep() );
			}
			else if ( TAGrecoManager::GetPar()->KalMode() == "ref" ) {

				m_refFitter->processTrackWithRep( fitTrack, fitTrack->getCardinalRep() );
			}
			else if ( TAGrecoManager::GetPar()->KalMode() == "daf" )
				m_dafRefFitter->processTrack( fitTrack );
			else if ( TAGrecoManager::GetPar()->KalMode() == "dafsimple" )
				m_dafSimpleFitter->processTrack( fitTrack );
			else {
				cout << "TAGactKFitter::MakeFit -- ERROR :: wrong  TAGrecoManager::GetPar()->KalMode() -> " << TAGrecoManager::GetPar()->KalMode() << endl, exit(0);
			}
			delete preFitter;
		}
		catch (genfit::Exception& e) {
			std::cerr << e.what();
			std::cerr << "Exception, next track\n";
			continue;
    	}

		if(m_debug > 0)
		{
			cout << "TRACK PROCESSED!!!!!\n";
			fitTrack->getFitStatus(fitTrack->getCardinalRep())->Print();
		}

		isConverged = 0;
		if (  fitTrack->getFitStatus(fitTrack->getCardinalRep())->isFitConverged() &&   fitTrack->getFitStatus(fitTrack->getCardinalRep())->isFitted() )
			isConverged = 1;	// convergence check

		if ( m_debug > 3 )		fitTrack->Print("C");

		// // map of the CONVERGED tracks for each category
		if (isConverged) {

			if ( (TAGrecoManager::GetPar()->Chi2Cut() < 0) || ( m_refFitter->getRedChiSqu(fitTrack, fitTrack->getCardinalRep()) <= TAGrecoManager::GetPar()->Chi2Cut() ) ) {
				NconvTracks++;

				if ( m_nConvergedTracks_all.find( PartName ) == m_nConvergedTracks_all.end() )	m_nConvergedTracks_all[ PartName ] = 0;
				m_nConvergedTracks_all[ PartName ]++;

				RecordTrackInfo( fitTrack, newTrackName );
				if(m_debug > 0) cout << "DONE\n";

			}
			m_vectorConvergedTrack.push_back( fitTrack );
		}

		// // fill a vector with the categories fitted at least onece
		// if ( find( m_categoryFitted.begin(), m_categoryFitted.end(), (*hitSample).first ) == m_categoryFitted.end() )
		// m_categoryFitted.push_back( (*hitSample).first );

	}	// end  - loop over all hit category

	if( TAGrecoManager::GetPar()->IsSaveHisto() )
		h_nTracksPerEv->Fill( m_vectorConvergedTrack.size() );

	// filling event display with converged tracks
	if ( TAGrecoManager::GetPar()->EnableEventDisplay() && m_vectorConvergedTrack.size() > 0) {
		if (m_vectorConvergedTrack.size() > 2)
			cout << "Event::" << (long)gTAGroot->CurrentEventId().EventNumber() << " display->addEvent size " << m_vectorConvergedTrack.size() - 1 << " at position " << m_eventDisplayCounter << "\n";
		m_eventDisplayCounter++;
		display->addEvent(m_vectorConvergedTrack);
	}
	m_vectorConvergedTrack.clear();

	if ( m_debug > 0 )		cout << "Ready for the next track fit!\n";

	return NconvTracks;
}









//! \brief Make the prefit of selected tracks
//!
//! FUNCTION CURRENTLY NOT USED
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










//! \brief Record the information regarding a fitted+converged track
//!
//! \param[in] track Pointer to fitted track to save
//! \param[in] fitTrackName Name of the fitted track (example He_4_2001 -> tracklet 1 of VTX 2, with Helium 4 as starting particle hypothesis)
void TAGactKFitter::RecordTrackInfo( Track* track, string fitTrackName ) {

	if(m_debug > 0)		cout << "RECORD START" << endl;
	TAGtrack* shoeOutTrack;
	vector<TAGpoint*> shoeTrackPointRepo;
	Int_t TwChargeZ = -1;
	Float_t TwTof = -1;
	vector<string> tok = TAGparTools::Tokenize( fitTrackName , "_" );
	string PartName = tok.at(0);



	// Fill Points and retrieve the true MC particle for each measuerement [ nMeasurement, shoeID of generated particle in the particle array ]
	vector<vector<int>> mcParticleID_track;
	for (unsigned int iMeas = 0; iMeas < track->getNumPointsWithMeasurement(); ++iMeas) {

		vector<int> mcID_Meas;
		int trackHitID = track->getPointWithMeasurement(iMeas)->getRawMeasurement()->getHitId();
		int trackDetID = track->getPointWithMeasurement(iMeas)->getRawMeasurement()->getDetId();

		// get measurement position, position error and other info
		TVector3 measPos, measPos_err;
		GetMeasTrackInfo( trackHitID, &measPos, &measPos_err );

		int iSensor, iClus;
		vector<int> iPart;
		GetMeasInfo( trackDetID, trackHitID, &iSensor, &iClus, &iPart );
		string detName = m_SensorIDMap->GetDetNameFromMeasID( trackHitID );



		// vector with index of the mc truth particles generating the measurement
		for ( unsigned int ip=0; ip<iPart.size(); ip++)
			mcID_Meas.push_back( iPart.at(ip) );
		mcParticleID_track.push_back(mcID_Meas);

		// create shoe track points vector repository to be passed to the output track object, with general and measurement info
		TAGpoint* shoeTrackPoint = new TAGpoint( detName, iSensor, iClus, &iPart, &measPos, &measPos_err );

		//RZ: First easy implementation -> Check in future when you have more TWpoints in the track
		if(detName == "TW")
		{
			TATWpoint* point = ( (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint","TATWntuPoint")->Object() )->GetPoint( iClus );
			TwChargeZ = point->GetChargeZ();
			TwTof = point->GetToF();
			shoeTrackPoint->SetEnergyLoss(point->GetEnergyLoss());
		}
		else if(detName == "MSD")
		{
			TAMSDcluster* MSDclus = ( (TAMSDntuCluster*) gTAGroot->FindDataDsc("msdClus","TAMSDntuCluster")->Object() )->GetCluster( iSensor, iClus );
			shoeTrackPoint->SetEnergyLoss(MSDclus->GetEnergyLoss());
		}

		// getRecoInfo
		TVector3 recoPos, recoMom;
		TMatrixD recoPos_cov(3,3);
		TMatrixD recoMom_cov(3,3);

		GetRecoTrackInfo( iMeas, track, &recoPos, &recoMom, &recoPos_cov, &recoMom_cov );
		shoeTrackPoint->SetRecoInfo( &recoPos, &recoMom, &recoPos_cov, &recoMom_cov );   // fill with reconstructed info

		// fill shoe track points vector repository to be passed to the output track object
		shoeTrackPointRepo.push_back( shoeTrackPoint );
	}


	//Start track variables recording

	// Retrieve tracking info!
	int nMeas 	= track->getCardinalRep()->getDim();
	int pdgID 	= track->getCardinalRep()->getPDG();
	int pdgCh 	= track->getCardinalRep()->getPDGCharge();
	float startMass	= std::atoi( tok.at(1).c_str() )*m_AMU;                                  //Initial mass of the fit in GeV
	int fitCh 	= track->getCardinalRep()->getCharge( track->getFittedState(0) );            // dipendono dallo stato considerato
	double fitMass = track->getCardinalRep()->getMass( track->getFittedState(0) );           // dipendono dallo stato considerato in GeV

	//Stop if the fitted charge is outside of boundaries
	if(fitCh < 0 || fitCh > ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber) {return;}

	//Vertexing for track length
	if( m_debug > 1)	cout << "Track length before vertexing::" << track->getTrackLen(track->getCardinalRep(), 0, -1) << endl;

	//Extrapolate to VTX
	//RZ: Issue!!!!! When trueparticle is active this extrapolation breaks
	TVector3 targetMeas;
	if( TAGrecoManager::GetPar()->PreselectStrategy() == "TrueParticle" )
	{
		targetMeas = TVector3(0,0,0); //RZ: DUMMY!!!
	}
	else
	{
		TAVTvertex* vtx = ((TAVTntuVertex*) gTAGroot->FindDataDsc("vtVtx", "TAVTntuVertex")->Object() )->GetVertex( std::atoi(tok.at(2).c_str())/1000 ); //Find the vertex associated to the track using the fitTrackName (1000*iVtx + iTracklet)
		targetMeas = m_GeoTrafo->FromVTLocalToGlobal(vtx->GetPosition());
	}


	StateOnPlane state_target_point = track->getFittedState(0);
	double extL_Tgt = track->getCardinalRep()->extrapolateToPoint( state_target_point, targetMeas );

	if(m_debug > 1)
	{
		cout << "Vertex" << std::atoi(tok.at(2).c_str())/1000  << " has position:"; targetMeas.Print();
		cout << "Extrap to point state::"; state_target_point.Print();
		cout << "Distance point-vtx::" << (state_target_point.getPos() - targetMeas).Mag() << endl;
		cout << "Extrapolation length::" << extL_Tgt << endl;
		cout << "Track length after vertexing::" << fabs(extL_Tgt) + track->getTrackLen(track->getCardinalRep(), 0, -1) << endl;
	}

	//End VTX; start TW: extrap some mm after TW for fitted total energy loss

	StateOnPlane state_TW = track->getFittedState(-1);

	TVector3 origin_( 0, 0, m_GeoTrafo->FromTWLocalToGlobal(m_TW_geo->GetLayerPosition(1)).z() + 0.5 );
	genfit::SharedPlanePtr TWextrapPlane (new genfit::DetPlane(origin_, TVector3(0,0,1)));
	TWextrapPlane->setU(1.,0.,0.);
	TWextrapPlane->setV(0.,1.,0.);

	double energyOutTw = -1;
	try
	{
		double extL_TW = track->getCardinalRep()->extrapolateToPlane( state_TW, TWextrapPlane );
	}
	catch (genfit::Exception& e)
	{
		std::cerr << e.what();
		std::cerr << "Exception, particle is too slow to escape TW. Setting final energy to 0\n";
		state_TW = track->getFittedState(-1);
		energyOutTw = 0;
	}

	//End TW

	// get reco info at the target level (by now at the first VT layer)
	TVector3 recoPos_target, recoMom_target;
	TMatrixD recoPos_target_cov(3,3);
	TMatrixD recoMom_target_cov(3,3);

	GetRecoTrackInfo(0, track, &recoPos_target, &recoMom_target, &recoPos_target_cov, &recoMom_target_cov ); //This might be unnecessary...

	// // getRecoInfo
	TVector3 recoPos_TW, recoMom_TW;
	TMatrixD recoPos_TW_cov(3,3);
	TMatrixD recoMom_TW_cov(3,3);
	GetRecoTrackInfo(-1, track, &recoPos_TW, &recoMom_TW, &recoPos_TW_cov, &recoMom_TW_cov );

	float length, tof;
	if( track->hasKalmanFitStatus(track->getCardinalRep()) )
	{
		length = track->getKalmanFitStatus( track->getCardinalRep() )->getTrackLen() + fabs(extL_Tgt); //Track length from Tgt to TW
		tof	= track->getCardinalRep()->getTime(track->getFittedState(-1)) - track->getCardinalRep()->getTime(state_target_point); //TOF from Tgt to TW
	}
	double energyAtTgt = TMath::Sqrt( pow(track->getCardinalRep()->getMomMag(state_target_point), 2) + pow(fitMass, 2) ) - fitMass; //Energy at Tgt

	if(energyOutTw != 0)	energyOutTw = TMath::Sqrt( pow(track->getCardinalRep()->getMomMag(state_TW), 2) + pow(fitMass, 2) ) - fitMass; //Energy after TW

	if(m_debug > 1)
	{
		cout << "fitCh::" << fitCh << "\tfitMass::" << fitMass << endl;
		cout << "Energy at target::" << energyAtTgt << endl;
		cout << "TOF::" << tof << endl;
		cout << "TOF with extrap states::" << track->getCardinalRep()->getTime(track->getFittedState(-1)) - track->getCardinalRep()->getTime(state_target_point) << endl;
		cout << "Energy loss::" << energyAtTgt - energyOutTw << endl;
	}

	double chi2 		= m_refFitter->getRedChiSqu(track, track->getCardinalRep());
	int ndof 			= track->getFitStatus( track->getCardinalRep() )->getNdf();
	double chisquare 	= track->getFitStatus( track->getCardinalRep() )->getChi2();
	double pVal 		= track->getFitStatus( track->getCardinalRep() )->getPVal();

	if(m_debug > 1)
	{
		cout << "TAGactKFitter::RecordTrackInfo:: DONE chi2 = " << chi2  << endl;
		cout << "TAGactKFitter::RecordTrackInfo:: DONE chisquare = " << chisquare  << endl;
	}

	// track->getFitStatus( track->getCardinalRep() )->isFitConverged();
	// track->getFitStatus( track->getCardinalRep() )->getNFailedPoints();
	// track->getFitStatus( track->getCardinalRep() )->isFitConvergedFully();

	// update at target position
	recoPos_target = state_target_point.getPos();
	recoMom_target = state_target_point.getMom();
	// recoMom_target_cov;  maybe still at VTX...

	shoeOutTrack = m_outTrackRepo->NewTrack( fitTrackName, (long)gTAGroot->CurrentEventId().EventNumber(),
										pdgID, startMass, fitCh, fitMass, length, tof, chi2, ndof, pVal,
										&recoPos_target, &recoMom_target, &recoPos_target_cov,
										&recoMom_target_cov, &recoPos_TW, &recoMom_TW, &recoPos_TW_cov,
										&recoMom_TW_cov, &shoeTrackPointRepo);
	//Set additional variables
	shoeOutTrack->SetTrackId(std::atoi(tok.at(2).c_str()));
	shoeOutTrack->SetTwChargeZ(TwChargeZ);
	shoeOutTrack->SetTwTof(TwTof);
	//Energy in GeV
	shoeOutTrack->SetFitEnergy( energyAtTgt);
	shoeOutTrack->SetFitEnergyLoss( energyAtTgt - energyOutTw );
	TVector3 TrackDir(-100,-100,-100);

	//Calculate emission angles wrt BM track
	if( ((TABMntuTrack*) gTAGroot->FindDataDsc("bmTrack","TABMntuTrack")->Object())->GetTracksN() > 0 )
	{
		TVector3 BMslope = ( (TABMntuTrack*) gTAGroot->FindDataDsc("bmTrack","TABMntuTrack")->Object() )->GetTrack(0)->GetSlope();
		BMslope = m_GeoTrafo->VecFromBMLocalToGlobal(BMslope).Unit();
		shoeOutTrack->SetTgtThetaBm(BMslope.Angle( recoMom_target ));

		//Find vector perpendicular to BMslope and Z-axis and rotate around it of Theta
		TRotation rot;
		TVector3 perp = BMslope.Cross(TVector3(0,0,1));
		rot.Rotate(BMslope.Theta(), perp);
		shoeOutTrack->SetTgtPhiBm( (rot*recoMom_target).Phi() );
		TrackDir = (rot*recoMom_target).Unit();
	}

	//MC additional variables if running on simulations
	int trackMC_id = -1;
	double trackQuality = -1;
	if ( m_IsMC ) {

		TVector3 mcMom, mcPos;

		trackMC_id = FindMostFrequent( &mcParticleID_track );
		// trackMC_id = track->getMcTrackId();     //???????

		TAMCpart* particle = m_trueParticleRep->GetTrack( trackMC_id );
		mcMom = particle->GetInitP();
		mcPos = particle->GetInitPos();
		int mcCharge = particle->GetCharge();

		if( ValidHistogram() )
		{
			h_trackMC_true_id->Fill( trackMC_id );
			h_mcMom->Fill( mcMom.Mag() );
			h_mcPosX->Fill( mcPos.X() );
			h_mcPosY->Fill( mcPos.Y() );
			h_mcPosZ->Fill( mcPos.Z() );
		}


		if(m_debug > 0)
		{
			cout << "\n\nTAGactKFitter::RecordTrackInfo:: True Pos z = "<< mcPos.z() << "     p = "<< mcMom.Mag() << "  " << fitTrackName<< endl;
			cout << "TAGactKFitter::RecordTrackInfo:: Reco Pos = "<< recoPos_target.Mag() << "     p = "<< recoMom_target.Mag() << endl<<endl<<endl;
		}

		m_trackAnalysis->FillMomentumInfo( recoMom_target, mcMom, recoMom_target_cov, PartName, &h_deltaP, &h_sigmaP );

		m_trackAnalysis->Fill_MomentumResidual( recoMom_target, mcMom, recoMom_target_cov, PartName, &h_dPOverP_x_bin );

		trackQuality = TrackQuality( &mcParticleID_track );
		if(m_debug > 0) cout << "trackQuality::" << trackQuality << "\n";


		if ( mcCharge == fitCh && trackQuality > 0.7 ) {
			if ( m_nConvergedTracks_matched.find( PartName ) == m_nConvergedTracks_matched.end() )	m_nConvergedTracks_matched[ PartName ] = 0;
			m_nConvergedTracks_matched[ PartName ]++;
		}
		else
		{
			if(m_debug > 0)	cout << "NOT MATCHED => evt::" << (long)gTAGroot->CurrentEventId().EventNumber() << "\tfitCh::" << fitCh << "\tmcCh::" << mcCharge << "\ttrQ::" << trackQuality << "\n";
		}

	//	shoeOutTrack->SetMCInfo( trackMC_id, trackQuality );
		if( ValidHistogram() )
		{
			h_chargeMC->Fill( mcCharge );
			h_trackQuality->Fill( trackQuality );
			shoeOutTrack->SetQuality( trackQuality );
			h_trackMC_reco_id->Fill( m_IsotopesIndex[ UpdatePDG::GetPDG()->GetPdgName( pdgID ) ] );
			h_momentum_true.at(fitCh)->Fill( particle->GetInitP().Mag() );	// check if not present
			h_ratio_reco_true.at(fitCh)->Fill( recoMom_target.Mag() - particle->GetInitP().Mag() );	// check if not present
		}

		if(m_debug > 1)	cout << "TAGactKFitter::RecordTrackInfo:: DONE MC = "<< endl;
	}

	//Histogram filling
	if( ValidHistogram() )
	{
		h_dR->Fill ( recoMom_target.DeltaR( TVector3(0,0,0) ) );
		h_phi->Fill ( recoMom_target.Phi() );
		h_theta->Fill ( recoMom_target.Theta()*TMath::RadToDeg() );

		// histos wrt BM track
		h_theta_BM->Fill ( shoeOutTrack->GetTgtThetaBm()*TMath::RadToDeg() );
		h_phi_BM->Fill ( shoeOutTrack->GetTgtPhiBm()*TMath::RadToDeg() );
		h_trackDirBM->Fill(TrackDir.X(), TrackDir.Y());

		h_eta->Fill ( recoMom_target.Eta() );
		h_dx_dz->Fill ( recoMom_target.x() / recoMom_target.z() );
		h_dy_dz->Fill ( recoMom_target.y() / recoMom_target.z() );

		h_nMeas->Fill ( nMeas );
		h_mass->Fill( fitMass );
		h_chi2->Fill( chi2 );

		h_chargeMeas->Fill( fitCh );
		h_chargeFlip->Fill( pdgCh - fitCh );

		h_length->Fill( length );
		h_tof->Fill( tof );
		h_pVal->Fill( pVal );

		h_momentum->Fill( recoMom_target.Mag() );
		h_momentum_reco.at(fitCh)->Fill( recoMom_target.Mag() );	// check if not present

		for(auto it : shoeTrackPointRepo){
			// TString detname=it->GetDevName()
			h_resoFitMeas[make_pair(it->GetDevName(),make_pair(it->GetSensorIdx(),0))]->Fill(it->GetMeasPosition().Y()-it->GetFitPosition().Y());
			h_resoFitMeas[make_pair(it->GetDevName(),make_pair(it->GetSensorIdx(),1))]->Fill(it->GetMeasPosition().X()-it->GetFitPosition().X());
		}
	}

	if(m_debug > 0)	cout << "TAGactKFitter::RecordTrackInfo:: DONE HISTOGRAM FILL "  << endl;

    //! Get the accumulated X/X0 (path / radiation length) of the material crossed in the last extrapolation.
    // virtual double getRadiationLenght() const = 0;

	for(auto it : shoeTrackPointRepo)
		delete it;
	shoeTrackPointRepo.clear();

}




//! \brief Find the MC particle that appears more frequently in the selected track
//!
//! \param[in] mcParticleID_track Ponter to vector containing all the vectors of MC particles crossing each cluster/point
//! \return Id of the most frequent MC particle
int TAGactKFitter::FindMostFrequent( vector<vector<int>>* mcParticleID_track ) {

	map<int,int> qualityMap; // [ID in the shoe particle container, multiplicity in the track points]
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


//----------------------------------------------------------------------------------------------------

//! \brief Compute the quality of the track under study
//!
//! The track quality is defined as the occurences of the most frequent particle over the number of track points
//! \param[in] mcParticleID_track Pointer to vector containing all the vectors of MC particles crossing each cluster/point
//! \return Quality of the selected track
double TAGactKFitter::TrackQuality( vector<vector<int>>* mcParticleID_track ) {

	map<int,int> qualityMap;
	for ( unsigned int i=0; i<mcParticleID_track->size(); i++)
	{
		for(int j = 0; j < mcParticleID_track->at(i).size(); j++ )
		{
			if(qualityMap.find(mcParticleID_track->at(i).at(j)) == qualityMap.end() )
				qualityMap[mcParticleID_track->at(i).at(j)] = 0;
			qualityMap[ mcParticleID_track->at(i).at(j) ]++;
		}
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
		return (double)occurence/mcParticleID_track->size();
	}

}






//----------------------------------------------------------------------------------------------------

//! \brief Get additional information on a measurement
//!
//! \param[in] detID Id of the detector
//! \param[in] hitID Global measurement Id
//! \param[out] iSensor Pointer to local sensor Id
//! \param[out] iClus Pointer to local cluster Id
//! \param[out] iPart Pointer to vector of MC particles of the cluster
void TAGactKFitter::GetMeasInfo( int detID, int hitID, int* iSensor, int* iClus, vector<int>* iPart ) {

	// check
	if ( detID != m_SensorIDMap->GetDetIDFromMeasID( hitID ) )
	{
		Error("GetMeasInfo()", "Detector ID not matching between GENFIT (%d) and SensorIDmap (%d)", detID, m_SensorIDMap->GetDetIDFromMeasID( hitID ));
		exit(0);
	}

	*iSensor = m_SensorIDMap->GetSensorIDFromMeasID( hitID );
	*iClus = m_SensorIDMap->GetHitIDFromMeasID( hitID );

	if ( m_IsMC )
		*iPart = m_measParticleMC_collection.at( hitID );

}






//----------------------------------------------------------------------------------------------------

//! \brief Get info on the position measurement and error for an hit
//!
//! \param[in] hitID Global Id of the hit
//! \param[out] pos Pointer to vector where to store the measured position
//! \param[out] posErr Pointer to vector where to store the position error
void TAGactKFitter::GetMeasTrackInfo( int hitID, TVector3* pos, TVector3* posErr ) {

	string det = m_SensorIDMap->GetDetNameFromMeasID( hitID );

	int iSensor = m_SensorIDMap->GetSensorIDFromMeasID( hitID );
	int iClus = m_SensorIDMap->GetHitIDFromMeasID( hitID );

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

//! \brief Get the reconstructed information for a Track Point
//!
//! \param[in] i Index of the Track Point
//! \param[in] track Pointer to track under study
//! \param[out] KalmanPos Pointer to vector used to store the fitted position
//! \param[out] KalmanMom Pointer to vector used to store the fitted momentum
//! \param[out] KalmanPos_cov Pointer to matrix used to store the covariance of fitted position
//! \param[out] KalmanMom_cov Pointer to matrix used to store the covariance of fitted momentum
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
// void TAGactKFitter::GetRecoTrackInfo (  StateOnPlane* state,
// 										TVector3* KalmanPos, TVector3* KalmanMom,
// 										TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov ) {


// 	// Get reco track kinematics and errors
// 	*KalmanPos = TVector3( (state->get6DState())[0],	(state->get6DState())[1],	(state->get6DState())[2] );
// 	*KalmanMom = TVector3( (state->get6DState())[3], (state->get6DState())[4],	(state->get6DState())[5] );

// 	MatrixToZero(KalmanPos_cov);
// 	MatrixToZero(KalmanMom_cov);

// 	// incertainty given only in fitting a StateOnPlane, not for extrapolation, even if in RKTrackRep the noise Jacobian can  be calcualted
// 	// for ( int j=0; j<3; j++ ) {
// 	// 	for ( int k=0; k<3; k++ ) {
// 	// 		(*KalmanMom_cov)(j,k) = (state->get6DCov())[j+3][k+3];
// 	// 		(*KalmanPos_cov)(j,k) = (state->get6DCov())[j][k];
// 	// 	}
// 	// }

// }




//----------------------------------------------------------------------------------------------------

//! \brief Print the overall purity of fitted tracks for each charge value
void TAGactKFitter::PrintPurity() {

	int nCollection = m_nConvergedTracks_all.size();
	h_purity = new TH1F( "h_purity", "h_purity", nCollection, 0, nCollection );
	TH1F* h_trackMatched = new TH1F( "h_trackMatched", "h_trackMatched", nCollection, 0, nCollection );

	int k = 0;
	float totalNum = 0;
	float totalDen = 0;

	for(vector<string>::iterator itPart = m_Particles.begin(); itPart != m_Particles.end(); ++itPart)
	{
		if(m_nConvergedTracks_all.find(*itPart) == m_nConvergedTracks_all.end())
			continue;

		k++;


		float kk = (float)m_nConvergedTracks_matched[ *itPart ];
		float nn = m_nConvergedTracks_all[ *itPart ];
		float eff = (float)kk/nn;
		float variance = ( (kk+1)*(kk+2)/((nn+2)*(nn+3)) ) - ( (kk+1)*(kk+1)/((nn+2)*(nn+2)) );
		if ( m_debug > -1 )		cout << "Purity " << *itPart << " = " << eff << "  " << int(kk) << " " << int(nn) << endl;

		totalNum+=kk;
		totalDen+=nn;

		h_trackMatched->SetBinContent(k, kk);

		h_purity->SetBinContent(k, eff);
		h_purity->SetBinError(k, sqrt(variance));

		h_purity->GetXaxis()->SetBinLabel(k, (*itPart).c_str() );
	}

	if ( m_debug > -1 )		cout << "Total Purity " << " = " << totalNum/totalDen << "  " << int(totalNum) << " " << int(totalDen) << endl;

	AddHistogram(h_trackMatched);
	AddHistogram(h_purity);

}



//----------------------------------------------------------------------------------------------------

//! \brief Print the overall efficiency of the fit algorithm for each charge value
void TAGactKFitter::PrintEfficiency() {

	int nCollection = m_nSelectedTrackCandidates.size();
	h_trackEfficiency = new TH1F( "TrackEfficiency", "TrackEfficiency", nCollection, 0, nCollection );

	TH1F* h_trackConverged = new TH1F( "h_trackConverged", "h_trackConverged", nCollection, 0, nCollection );
	TH1F* h_trackSelected = new TH1F( "h_trackSelected", "h_trackSelected", nCollection, 0, nCollection );

	int k = 0;
	float totalNum = 0;
	float totalDen = 0;

	for(vector<string>::iterator itPart = m_Particles.begin(); itPart != m_Particles.end(); ++itPart)
	{
		if(m_nConvergedTracks_all.find(*itPart) == m_nConvergedTracks_all.end())
			continue;

		k++;

		float kk = (float)m_nConvergedTracks_all[ *itPart ];
		float nn = m_nSelectedTrackCandidates[ *itPart ];
		float eff = (float)kk/nn;
		float variance = ( (kk+1)*(kk+2)/((nn+2)*(nn+3)) ) - ( (kk+1)*(kk+1)/((nn+2)*(nn+2)) );
		if ( m_debug > -1 )		cout << "Efficiency " << *itPart << " = " << eff << "  " << int(kk) << " " << int(nn) << endl;

		totalNum+=kk;
		totalDen+=nn;

		h_trackConverged->SetBinContent(k, kk);
		h_trackSelected->SetBinContent(k, nn);

		h_trackEfficiency->SetBinContent(k, eff);
		h_trackEfficiency->SetBinError(k, sqrt(variance));

		h_trackEfficiency->GetXaxis()->SetBinLabel(k, (*itPart).c_str() );
	}

	if ( m_debug > -1 )		cout << "Total Efficiency " << " = " << totalNum/totalDen << "  " << int(totalNum) << " " << int(totalDen) << endl;

	h_trackEfficiency->SetTitle(0);
	h_trackEfficiency->SetStats(0);
	h_trackEfficiency->GetYaxis()->SetTitle("Reco Efficiency");
	h_trackEfficiency->GetYaxis()->SetTitleOffset(1.1);
	h_trackEfficiency->GetYaxis()->SetRange(0.,1.);
	h_trackEfficiency->SetLineWidth(2); // take short ~ int
	// h_trackEfficiency->Draw("E");
	// mirror->SaveAs( (m_kalmanOutputDir+"/"+"TrackEfficiencyPlot.png").c_str() );
	// mirror->SaveAs( (m_kalmanOutputDir+"/"+"TrackEfficiencyPlot.root").c_str() );

	AddHistogram(h_trackSelected);
	AddHistogram(h_trackConverged);
	AddHistogram(h_trackEfficiency);

}




//----------------------------------------------------------------------------------------------------

//! \brief Print the overall selection efficiency of tracks for each charge value
void TAGactKFitter::PrintSelectionEfficiency() {

	int nCollection = m_genCount_vector.size();
	TH1F* h_selectEfficiency = new TH1F( "h_selectEfficiency", "h_selectEfficiency", nCollection, 0, nCollection );

	TH1F* h_trackProduced = new TH1F( "h_trackProduced", "h_trackProduced", nCollection, 0, nCollection );

	int k = 0;
	float totalNum = 0;
	float totalDen = 0;

	for(vector<string>::iterator itPart = m_Particles.begin(); itPart != m_Particles.end(); ++itPart) 	{

		if(m_genCount_vector.find(*itPart) == m_genCount_vector.end())
			continue;
		if(m_nSelectedTrackCandidates.find(*itPart) == m_nSelectedTrackCandidates.end())
			continue;

		k++;

		float kk = (float)m_nSelectedTrackCandidates[ *itPart ];
		float nn = m_genCount_vector[ *itPart ];
		float eff = (float)kk/nn;
		float variance = ( (kk+1)*(kk+2)/((nn+2)*(nn+3)) ) - ( (kk+1)*(kk+1)/((nn+2)*(nn+2)) );
		if ( m_debug > -1 )		cout << "Efficiency Selection " << *itPart << " = " << eff << "  " << int(kk) << " " << int(nn) << endl;

		totalNum+=kk;
		totalDen+=nn;

		h_trackProduced->SetBinContent(k, nn);

		h_selectEfficiency->SetBinContent(k, eff);
		h_selectEfficiency->SetBinError(k, sqrt(variance));

		h_selectEfficiency->GetXaxis()->SetBinLabel(k, (*itPart).c_str() );
	}

	if ( m_debug > -1 )		cout << "Total Efficiency Selection " << " = " << totalNum/totalDen << "  " << int(totalNum) << " " << int(totalDen) << endl;

	h_selectEfficiency->SetTitle(0);
	h_selectEfficiency->SetStats(0);
	h_selectEfficiency->GetYaxis()->SetTitle("Sel Efficiency");
	h_selectEfficiency->GetYaxis()->SetTitleOffset(1.1);
	h_selectEfficiency->GetYaxis()->SetRange(0.,1.);
	h_selectEfficiency->SetLineWidth(2); // take short ~ int
	// h_selectEfficiency->Draw("E");
	// mirror->SaveAs( (m_kalmanOutputDir+"/"+"TrackEfficiencyPlot.png").c_str() );
	// mirror->SaveAs( (m_kalmanOutputDir+"/"+"TrackEfficiencyPlot.root").c_str() );

	AddHistogram(h_trackProduced);
	AddHistogram(h_selectEfficiency);

}




//! \brief Declare the GenFit histograms
void TAGactKFitter::CreateHistogram()	{

	h_GFeventType = new TH1I("h_GFeventType", "h_GFeventType", 5, 0.5, 5.5);
	AddHistogram(h_GFeventType);

	h_trackMC_true_id = new TH1F("h_trackMC_true_id", "h_trackMC_true_id", 45, 0., 45);
	AddHistogram(h_trackMC_true_id);

	h_trackMC_reco_id = new TH1F("h_trackMC_reco_id", "h_trackMC_reco_id", 45, 0., 45);
	AddHistogram(h_trackMC_reco_id);

	h_nTracksPerEv= new TH1F("h_nTracksPerEv", "h_nTracksPerEv", 15, 0., 15);
	AddHistogram(h_nTracksPerEv);

	h_trackQuality = new TH1F("m_trackQuality", "m_trackQuality", 55, 0, 1.1);
	AddHistogram(h_trackQuality);

	h_length = new TH1F("m_length", "m_length", 400, 0, 200);
	AddHistogram(h_length);

	h_tof = new TH1F("m_tof", "m_tof", 200, 0, 20);
	AddHistogram(h_tof);

	h_pVal = new TH1F("m_pVal", "m_pVal", 300, 0, 3);
	AddHistogram(h_pVal);


	h_mcPosX = new TH1F("h_mcPosX", "h_mcPosX", 400, -1, 1);
	AddHistogram(h_mcPosX);

	h_mcPosY = new TH1F("h_mcPosY", "h_mcPosY", 400, -1, 1);
	AddHistogram(h_mcPosY);

	h_mcPosZ = new TH1F("h_mcPosZ", "h_mcPosZ", 500, -0.25, 0.25);
	AddHistogram(h_mcPosZ);



	h_dR = new TH1F("h_dR", "h_dR", 100, 0., 20.);
	AddHistogram(h_dR);

	h_phi = new TH1F("h_phi", "h_phi", 80, -4, 4);
	AddHistogram(h_phi);

	h_theta = new TH1F("h_theta", "h_theta", 200, 0, 15);
	AddHistogram(h_theta);

	h_theta_BM = new TH1F("h_theta_BM", "h_theta_BM;Track #theta wrt BM [deg]; Entries", 200, 0, 15);
	AddHistogram(h_theta_BM);

	h_phi_BM = new TH1F("h_phi_BM", "h_phi_BM;Track #phi wrt BM [deg]; Entries", 100, -190, 190);
	AddHistogram(h_phi_BM);

	h_trackDirBM = new TH2F("h_trackDirBM", "h_trackDirBM;X;Y", 1001,-1,1,1001,-1,1);
	AddHistogram(h_trackDirBM);

	h_eta = new TH1F("h_eta", "h_eta", 100, 0., 20.);
	AddHistogram(h_eta);

	h_dx_dz = new TH1F("h_dx_dz", "h_dx_dz", 110, 0., 0.3);
	AddHistogram(h_dx_dz);

	h_dy_dz = new TH1F("h_dy_dz", "h_dy_dz", 110, 0., 0.3);
	AddHistogram(h_dy_dz);



	h_nMeas = new TH1F("nMeas", "nMeas", 13, 0., 13.);
	AddHistogram(h_nMeas);

	h_chargeMC = new TH1F("h_chargeMC", "h_chargeMC", 9, 0, 9);
	AddHistogram(h_chargeMC);

	h_chargeMeas = new TH1F("h_chargeMeas", "h_chargeMeas", 9, 0, 9);
	AddHistogram(h_chargeMC);

	h_chargeFlip = new TH1F("h_chargeFlip", "h_chargeFlip", 20, -5, 5);
	AddHistogram(h_chargeFlip);

  	h_mass = new TH1F("h_mass", "h_mass", 100, 0., 20.);
	AddHistogram(h_mass);



	h_chi2 = new TH1F("h_chi2", "h_chi2", 200, 0., 10.);
	AddHistogram(h_chi2);

	h_mcMom = new TH1F("h_mcMom", "h_mcMom", 150, 0., 15.);
	AddHistogram(h_mcMom);

	h_momentum = new TH1F("h_momentum", "h_momentum", 150, 0., 15.);
	AddHistogram(h_momentum);

	for (int i = 0; i < 9; ++i){
		h_momentum_true.push_back(new TH1F(Form("TrueMomentum%d",i), Form("True Momentum %d",i), 150, 0.,15.));
		AddHistogram(h_momentum_true[i]);

		h_momentum_reco.push_back(new TH1F(Form("RecoMomentum%d",i), Form("Reco Momentum %d",i), 150, 0.,15.));
		AddHistogram(h_momentum_reco[i]);

		h_ratio_reco_true.push_back(new TH1F(Form("MomentumRadio%d",i), Form("Momentum Ratio %d",i), 150, 0, 2.5));
		AddHistogram(h_ratio_reco_true[i]);
	}

	h_PlaneOccupancy[0] = new TH2I("h_PlaneOccupancy", "h_PlaneOccupancy; FitPlane Id; # of clusters", m_SensorIDMap->GetFitPlanesN()+2, -1.5, m_SensorIDMap->GetFitPlanesN()+0.5, 41, -0.5, 40.5);
	AddHistogram(h_PlaneOccupancy[0]);

	for(int iEv=1; iEv<=5; iEv++)
	{
		h_PlaneOccupancy[iEv] = new TH2I(Form("h_PlaneOccupancyType%d", iEv), Form("h_PlaneOccupancyType%d; FitPlane Id; # of clusters", iEv), m_SensorIDMap->GetFitPlanesN()+2, -1.5, m_SensorIDMap->GetFitPlanesN()+0.5, 41, -0.5, 40.5);
		AddHistogram(h_PlaneOccupancy[iEv]);
	}

	if(TAGrecoManager::GetPar()->IncludeVT()){
		for(Int_t i=0;i<m_VT_geo->GetSensorsN();i++){
			h_resoFitMeas[make_pair("VT",make_pair(i,0))] = new TH1F(Form("Res_vtY_layer_%d",i),Form("Residual between fitted global track and measured VT point in VT layer %d on Y view;Meas-Fit Y[cm];Number of points",i),600,-0.1,0.1);
			AddHistogram(h_resoFitMeas[make_pair("VT",make_pair(i,0))]);
			h_resoFitMeas[make_pair("VT",make_pair(i,1))] = new TH1F(Form("Res_vtX_layer_%d",i),Form("Residual between fitted global track and measured VT point in VT layer %d on X view;Meas-Fit X[cm];Number of points",i),600,-0.1,0.1);
			AddHistogram(h_resoFitMeas[make_pair("VT",make_pair(i,1))]);
		}
  }
	if(TAGrecoManager::GetPar()->IncludeIT()){
		for(Int_t i=0;i<m_IT_geo->GetSensorsN();i++){
			h_resoFitMeas[make_pair("IT",make_pair(i,0))] = new TH1F(Form("Res_itY_layer_%d",i),Form("Residual between fitted global track and measured IT point in IT layer %d on Y view;Meas-Fit Y[cm];Number of points",i),600,-0.1,0.1);
			AddHistogram(h_resoFitMeas[make_pair("IT",make_pair(i,0))]);
			h_resoFitMeas[make_pair("IT",make_pair(i,1))] = new TH1F(Form("Res_itX_layer_%d",i),Form("Residual between fitted global track and measured IT point in IT layer %d on X view;Meas-Fit X[cm];Number of points",i),600,-0.1,0.1);
			AddHistogram(h_resoFitMeas[make_pair("IT",make_pair(i,1))]);
		}
	}
	if(TAGrecoManager::GetPar()->IncludeMSD()){
		for(Int_t i=0;i<m_MSD_geo->GetSensorsN();i++){
			h_resoFitMeas[make_pair("MSD",make_pair(i,0))] = new TH1F(Form("Res_msdY_layer_%d",i),Form("Residual between fitted global track and measured MSD point in MSD layer %d on Y view;Meas-Fit Y[cm];Number of points",i),600,-0.1,0.1);
			AddHistogram(h_resoFitMeas[make_pair("MSD",make_pair(i,0))]);
			h_resoFitMeas[make_pair("MSD",make_pair(i,1))] = new TH1F(Form("Res_msdX_layer_%d",i),Form("Residual between fitted global track and measured MSD point in MSD layer %d on X view;Meas-Fit X[cm];Number of points",i),600,-0.1,0.1);
			AddHistogram(h_resoFitMeas[make_pair("MSD",make_pair(i,1))]);
		}
	}
	if(TAGrecoManager::GetPar()->IncludeTW()){
		h_resoFitMeas[make_pair("TW",make_pair(0,0))] = new TH1F("Res_twY","Residual between fitted global track and measured TW point on Y view;Meas-Fit Y[cm];Number of points",600,-3.,3.);
		AddHistogram(h_resoFitMeas[make_pair("TW",make_pair(0,0))]);
		h_resoFitMeas[make_pair("TW",make_pair(0,1))] = new TH1F("Res_twX","Residual between fitted global track and measured TW point on X view;Meas-Fit X[cm];Number of points",600,-3.,3.);
		AddHistogram(h_resoFitMeas[make_pair("TW",make_pair(0,1))]);
	}

	SetValidHistogram(kTRUE);

	return;
}


//----------------------------------------------------------------------------------------------------

//! \brief Reset matrix values to zero
void TAGactKFitter::MatrixToZero( TMatrixD *matrix ) {
  for ( int j=0; j<matrix->GetNrows(); j++ ) {
    for ( int k=0; k<matrix->GetNcols(); k++ ) {
      (*matrix)(j,k) = 0;
    }
  }
}






//----------------------------------------------------------------------------------------------------

//! \brief Initialize GenFit event display
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


//! \brief Evaluate the efficiency of the forward projection
//!
//! \param[in] fitTrack Pointer to the track under study
void TAGactKFitter::EvaluateProjectionEfficiency(Track* fitTrack)
{
	int MeasId, PlaneId;
	int chargeHypo, chargeMC;

	if(m_debug > 0)	cout << "Check of charge hypo: " << endl;

	for(int i=0; i<fitTrack->getNumPointsWithMeasurement(); ++i)
	{
		MeasId = fitTrack->getPointWithMeasurement(i)->getRawMeasurement()->getHitId();
		PlaneId = m_SensorIDMap->GetFitPlaneIDFromMeasID( MeasId );
		chargeHypo = fitTrack->getCardinalRep()->getPDGCharge();

		m_NClusTrack.at( PlaneId )++;

		bool good = false;
		for(vector<int>::iterator itTrackMC = m_measParticleMC_collection.at(MeasId).begin(); itTrackMC != m_measParticleMC_collection.at(MeasId).end(); ++itTrackMC)
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
}


//! \brief Check if the current charge hypothesis matches the value obtained from TW
//!
//! In case of mismatch, the function corrects the intial hypothesis and reset the seed for the track fit
//! \param[in,out] PartName Pointer to the name of the particle ("H,"He","Li"...). If the particle hypothesis changes, this variable is updated with the new name
//! \param[in] fitTrack Pointer to the track under study
void TAGactKFitter::CheckChargeHypothesis(string* PartName, Track* fitTrack, TAGFselectorBase* m_selector)
{
	int chargeFromTW = m_selector->GetChargeFromTW( fitTrack );
	if(m_debug > 0 ) cout << "Charge From TW::" << chargeFromTW << endl;
	if( chargeFromTW < 1 || chargeFromTW > ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber )
	{
		// Info("CheckChargeHypothesis()", "Wrong evaluation of TW charge for track candidate %s. No check performed...", PartName->c_str());
		return;
	}

	m_NTWTracks++;

	//Charge hypo != form TW --> change PartName and reset seed
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

		//Reset track seed
		double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(chargeFromTW) );
		int A_Hypo = round(mass_Hypo/m_AMU);

		TVector3 pos, mom;
		TVectorD seed = fitTrack->getStateSeed();
		pos.SetX(seed(0)); pos.SetY(seed(1)); pos.SetZ(seed(2));
		if(m_debug > 1)	pos.Print();

		mom.SetX(seed(3)); mom.SetY(seed(4)); mom.SetZ(seed(5));
		if(m_debug > 1)	mom.Print();

		mom.SetMag(TMath::Sqrt( pow(m_BeamEnergy*A_Hypo,2) + 2*mass_Hypo*m_BeamEnergy*A_Hypo ));

		// int pointID = m_SensorIDMap->GetHitIDFromMeasID(fitTrack->getRawMeasurement(-1)->getHitId());
		// float TOF = ( (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint","TATWntuPoint")->Object() )->GetPoint( pointID )->GetMeanToF();
		// float beam_speed =
		// TOF -= (m_GeoTrafo->GetTGCenter().Z()-m_GeoTrafo->GetSTCenter().Z())/beam_speed;
		// float beta = (m_GeoTrafo->GetTWCenter().Z() - m_GeoTrafo->GetTGCenter().Z())/(TOF*TAGgeoTrafo::GetLightVelocity());
		if(m_debug > 1)	mom.Print();

		fitTrack->setStateSeed(pos,mom);

		//update the name of the particle associated to track
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
	//Charge from TW == chargeHypo --> goodHypo!
	else
		m_NTWTracksGoodHypo++;

}




void TAGactKFitter::ClearData()
{
	for(auto it = m_allHitMeasGF.begin(); it != m_allHitMeasGF.end(); ++it)
	{
		for(auto itvec : it->second)
			delete itvec;
		it->second.clear();
	}
	m_allHitMeasGF.clear();

	for(auto it : m_vectorConvergedTrack)
		delete it;
	m_vectorConvergedTrack.clear();

	if( m_IsMC )
	{
		for(auto it = m_measParticleMC_collection.begin(); it != m_measParticleMC_collection.end(); ++it)
			it->second.clear();
		m_measParticleMC_collection.clear();
	}

	for ( auto trackIt = m_mapTrack.begin(); trackIt != m_mapTrack.end(); trackIt++)
		delete trackIt->second;
	m_mapTrack.clear();

}


void TAGactKFitter::ClearHistos()
{
	if ( m_IsMC ){
		delete h_purity;
	}
	delete h_trackEfficiency;
	delete h_trackQuality;
	delete h_trackMC_true_id;
	delete h_trackMC_reco_id;
	delete h_nTracksPerEv;
	delete h_length;
	delete h_tof;
	delete h_nMeas;
	delete h_mass;
	delete h_chi2;
	delete h_pVal;
	delete h_chargeMC;
	delete h_chargeMeas;
	delete h_chargeFlip;
	delete h_momentum;
	delete h_dR;
	delete h_phi;
	delete h_theta;
	delete h_theta_BM;
	delete h_phi_BM;
	delete h_eta;
	delete h_dx_dz;
	delete h_dy_dz;
	delete h_mcMom;
	delete h_mcPosX;
	delete h_mcPosY;
	delete h_mcPosZ;
	delete h_GFeventType;

	for(auto it = h_deltaP.begin(); it != h_deltaP.end(); ++it)
		delete it->second;
	h_deltaP.clear();

	for(auto it = h_sigmaP.begin(); it != h_sigmaP.end(); ++it)
		delete it->second;
	h_sigmaP.clear();

	for(auto it = h_resoP_over_Pkf.begin(); it != h_resoP_over_Pkf.end(); ++it)
		delete it->second;
	h_resoP_over_Pkf.clear();

	for(auto it = h_biasP_over_Pkf.begin(); it != h_biasP_over_Pkf.end(); ++it)
		delete it->second;
	h_biasP_over_Pkf.clear();

	for(auto it : h_momentum_true)
		delete it;
	h_momentum_true.clear();

	for(auto it : h_momentum_reco)
		delete it;
	h_momentum_reco.clear();

	for(auto it : h_ratio_reco_true)
		delete it;
	h_ratio_reco_true.clear();


	for(auto it = h_dPOverP_x_bin.begin(); it != h_dPOverP_x_bin.end(); ++it)
	{
		for(auto iit = it->second.begin(); iit != it->second.end(); ++iit)
			delete iit->second;
		it->second.clear();
	}
	h_dPOverP_x_bin.clear();

}
