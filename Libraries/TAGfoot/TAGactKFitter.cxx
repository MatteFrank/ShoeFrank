/*!
 \file TAGactKFitter.cxx
 \brief  Main class of the GenFit Global Reconstruction -> Action
 \author M. Franchini, R. Zarrella and R. Ridolfi
*/
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
m_fitter(0x0),
m_fitter_extrapolation(0x0),
m_outTrackRepo(0x0),
m_SensorIDMap(0x0),
m_trackAnalysis(0x0),
m_measParticleMC_collection(0x0),
m_GeoTrafo(0x0),
m_IsMC(false)
{
	AddDataOut(outTrackRepo, "TAGntuGlbTrack");
	m_outTrackRepo = (TAGntuGlbTrack*) fpGlobTrackRepo->Object();

	// Initialize the FOOT geometry for genfit
	m_SensorIDMap = new TAGFdetectorMap();
	genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());
	genfit::MaterialEffects::getInstance()->setNoEffects(false);

	m_GFgeometry = new TAGFgeometryConstructor(m_SensorIDMap, &m_systemsON);
	m_GFgeometry->IncludeDetectors();
	m_GFgeometry->CreateGeometry();

	m_GeoTrafo = m_GFgeometry->GetGgeoTrafo();

	//Initialize some particle-related variables
	m_AMU = 0.9310986964; // in GeV // conversion betweem mass in GeV and atomic mass unit
	m_BeamEnergy = m_GFgeometry->GetGparGeo()->GetBeamPar().Energy;

	m_Isotopes  = {	"H1", "H2", "H3",
					"He3", "He4", "He6", "He8",
					"Li6", "Li7", "Li8", "Li9",
					"Be7", "Be9", "Be10", "Be11", "Be12", "Be14",
					"B8", "B10", "B11", "B12", "B13", "B14", "B15",
					"C9", "C10", "C11", "C12", "C13", "C14", "C15", "C16",
					"N12", "N13", "N14", "N15", "N16",
					"O13", "O14", "O15", "O16" };
	for ( unsigned int i=0; i<m_Isotopes.size(); i++ )
		m_IsotopesIndex[ m_Isotopes[i] ] = i;

	m_Particles = { "H", "He", "Li", "Be", "B", "C", "N", "O" };
	for ( unsigned int i=0; i<m_Particles.size(); i++ )
		m_ParticleIndex[ m_Particles[i] ] = i;

	//Initialize the track fitter
	int nIter = 20; // max number of iterations
	double dPVal = 1.E-3; // convergence criterion
	m_fitter_extrapolation = new KalmanFitter(1, dPVal);
	m_fitter = InitializeFitter(nIter, dPVal);

	if ( TAGrecoManager::GetPar()->EnableEventDisplay() )	InitEventDisplay();

	TAGrecoManager::GetPar()->Print("all");

	m_trackAnalysis = new TAGF_KalmanStudies();

	//Initialize counters
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
	m_SCpileUpEvts = 0;

	m_eventDisplayCounter = 0;
	m_CALOextrapTolerance = 5;

	if( m_IsMC )
	{
		fTGregion = m_GFgeometry->GetGparGeo()->GetRegTarget();
		fAIRregion = m_GFgeometry->GetGparGeo()->GetRegAirPreTW();
	}

	fTrackUtilities = new TAGFtrackUtilities(m_SensorIDMap, m_GFgeometry);
}



//----------------------------------------------------------------------------------------------------

//! \brief Default destructor
//!
//! Delete all the objects used for Kalman Filter extrapolation
TAGactKFitter::~TAGactKFitter() {
	ClearData();

	delete m_fitter;
	delete m_fitter_extrapolation;

	delete m_SensorIDMap;
	delete m_trackAnalysis;

	m_Particles.clear();
	m_ParticleIndex.clear();
	m_Isotopes.clear();
	m_IsotopesIndex.clear();

	delete display;

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
	m_trueParticleRep = static_cast<TAMCntuPart*> (gTAGroot->FindDataDsc(FootActionDscName("TAMCntuPart"))->Object());
}


//----------------------------------------------------------------------------------------------------

//! \brief Fill counter of all the MC paticles generated in the FOOT setup
//! \param[in] mappa Map containing the name and number of all MC particles in the event
void TAGactKFitter::FillGenCounter( map< string, int > mappa )	{

	for( map<string, int>::iterator it = mappa.begin(); it != mappa.end(); ++it )	{

		if ( m_genCount_vector.size() < 1 || m_genCount_vector.find( (*it).first ) == m_genCount_vector.end() )
			m_genCount_vector[ (*it).first ] = 0;

		m_genCount_vector.at( (*it).first ) += (*it).second;
	}

}



//------------------------------------------+-----------------------------------

//! \brief Main action of the class
//!
//! Upload clusters/points in GenFit format, categorize them and fit the selected tracks
//! \return True if the action was successful
Bool_t TAGactKFitter::Action()
{
	if(FootDebugLevel(1)) cout << "TAGactKFitter::Action()  ->  start!" << endl;

	ClearData();
	long evNum = (long)gTAGroot->CurrentEventId().EventNumber();

	//Check if ST signaled a pile-up -> if so, skip the event
	if ( !m_IsMC && ((TASTntuRaw*)gTAGroot->FindDataDsc(FootActionDscName("TASTntuRaw"))->Object())->GetSuperHit()->GetPileUp() )
	{
		if( FootDebugLevel(1) )
			Info("Action()", "Event %ld flagged as pile-up from the SC! Skipping...", evNum);

		m_SCpileUpEvts++;
		fpGlobTrackRepo->SetBit(kValid);
		return true;
	}

	//Declare uploader -> prepare all measurements in genfit format
	TAGFuploader* GFUploader = new TAGFuploader( m_SensorIDMap, m_IsMC );
	GFUploader->TakeMeasHits4Fit( m_allHitMeasGF, m_systemsON );
	vector<int> chVect;
	GFUploader->GetPossibleCharges( &chVect );

	//Get true information if running on MC
	if ( m_IsMC ) {
		m_measParticleMC_collection = GFUploader->TakeMeasParticleMC_Collection();
		m_numGenParticle_noFrag += GFUploader->GetNumGenParticle_noFrag();

		m_trueParticleRep = static_cast<TAMCntuPart*> (gTAGroot->FindDataDsc(FootActionDscName("TAMCntuPart"))->Object());
		CalculateTrueMomentumAtTgt();
	}

	//Print the number of hits in each GF plane
	if(FootDebugLevel(1))	{
		cout << "TAGactKFitter::Action()  ->  " << m_allHitMeasGF.size() << endl;
		cout << "Plane\tN. hits" << endl;
		for(auto it = m_allHitMeasGF.begin(); it != m_allHitMeasGF.end(); ++it)
			cout << it->first << "\t" << it->second.size() << endl;
	}

	//Declare selector object
	TAGFselectorBase* GFSelector = InitializeSelector();
	GFSelector->SetVariables(fTrackUtilities, &m_allHitMeasGF, m_systemsON, &chVect, &m_mapTrack, m_measParticleMC_collection, m_IsMC, &m_singleVertexCounter, &m_noVTtrackletEvents, &m_noTWpointEvents);
	GFSelector->SetExtrapolationHistogram(h_extrapDist);

	//Find track candidates and fit them
	if (GFSelector->FindTrackCandidates() >= 0)
	{
		//RZ: Check selection efficiency counts --> better define the "visible" particles, right now is not really compatible with TrueParticle selection
		if ( m_IsMC )
			FillGenCounter( GFSelector->CountParticleGeneratedAndVisible() );

		MakeFit(evNum, GFSelector);
	}

	//Check if global tracks match w/ any CALO cluster
	if( TAGrecoManager::GetPar()->IncludeCA() )
		MatchCALOclusters();

	if( TAGrecoManager::GetPar()->IsSaveHisto() )
	{
		GFSelector->FillPlaneOccupancy(h_PlaneOccupancy);
		h_GFeventType->Fill(GFSelector->GetEventType());
	}

	//Clear
	chVect.clear();
	m_trueMomentumAtTgt.clear();
	delete GFUploader;
	delete GFSelector;

	if(FootDebugLevel(1)) cout << "TAGactKFitter::Action()  -> end! " << endl;
	fpGlobTrackRepo->SetBit(kValid);
	return true;
}



//----------------------------------------------------------------------------------------------------

//! \brief Finalize all the needed histograms for GenFit studies
//!
//! Save control plots and calculate resolution. Called from outside, at the end of the event cycle
void TAGactKFitter::Finalize() {

	if ( m_IsMC )	PrintPurity();
	PrintEfficiency();
	if ( m_IsMC )	PrintSelectionEfficiency();

	if( ValidHistogram() )
	{
		if( m_IsMC )
		{
			for ( map<string, map<float, TH1F*> >::iterator collIt=h_dPOverP_x_bin.begin(); collIt != h_dPOverP_x_bin.end(); ++collIt )
				for ( map<float, TH1F*>::iterator it=(*collIt).second.begin(); it != (*collIt).second.end(); ++it ) {
					AddHistogram( (*it).second );
					cout << "TAGactKFitter::Finalize() -- " << (*it).second->GetTitle()<< endl;
				}

			for ( map<string, TH1F*>::iterator it=h_resoP_over_Pkf.begin(); it != h_resoP_over_Pkf.end(); ++it )
				AddHistogram( (*it).second );

			for ( map<string, TH1F*>::iterator it=h_biasP_over_Pkf.begin(); it != h_biasP_over_Pkf.end(); ++it )
				AddHistogram( (*it).second );

			for ( map<string, TH2F*>::iterator it=h_dPOverP_vs_P.begin(); it != h_dPOverP_vs_P.end(); ++it )
				AddHistogram( (*it).second );

			TH1F* h_deltaP_tot = new TH1F();
			TH1F* h_sigmaP_tot = new TH1F();

			int count = 0;
			for ( map<string, TH1F*>::iterator it=h_deltaP.begin(); it != h_deltaP.end(); ++it ) {
				if ( count == 0 ) 	h_deltaP_tot = (TH1F*)((*it).second)->Clone("dP");
				else 				h_deltaP_tot->Add( (*it).second, 1 );
				AddHistogram( (*it).second );
				count++;
			}
			h_deltaP_tot->SetTitle( "dP" );
			AddHistogram( h_deltaP_tot );

			count=0;
			for ( map<string, TH1F*>::iterator it=h_sigmaP.begin(); it != h_sigmaP.end(); ++it ) {
				if ( count == 0 ) 	h_sigmaP_tot = (TH1F*) (*it).second->Clone();
				else 				h_sigmaP_tot->Add( (*it).second, 1 );
				AddHistogram( (*it).second );
				count++;
			}
			h_sigmaP_tot->SetNameTitle( "errdP", "errdP" );
			AddHistogram( h_sigmaP_tot );
		}

		TH1F* h_numGenParticle_noFrag = new TH1F( "h_numGenParticle_noFrag", "h_numGenParticle_noFrag", 100, 0, 10000 );
		AddHistogram( h_numGenParticle_noFrag );
		h_numGenParticle_noFrag->Fill( m_numGenParticle_noFrag );
		cout << "m_numGenParticle_noFrag = " << m_numGenParticle_noFrag << endl;

		cout << "TAGactKFitter::Finalize() -- END"<< endl;
		SetValidHistogram(kTRUE);
		SetHistogramDir(fDirectory);
	}

	cout << "Single vertex events::" << m_singleVertexCounter << "\n";
	cout << "Events w/ pile-up in the SC::" << m_SCpileUpEvts << "\n";
	cout << "Events w/out valid VT tracklets::" << m_noVTtrackletEvents << "\n";
	cout << "Events w/out valid TW point::" << m_noTWpointEvents << "\n";

	//show event display
	if ( TAGrecoManager::GetPar()->EnableEventDisplay() )
	{
		display->setOptions("BDEFHGMPT");
		display->open();
	}

	if( m_IsMC && FootDebugLevel(1) )
	{
		cout << "Check quality of charge hypothesis\nPlaneId\tNClus\tNGood" << endl;
		for(int i=0; i< m_NClusGood.size(); ++i)
			cout << i << "\t" << m_NClusTrack.at(i) << "\t" << m_NClusGood.at(i) << endl;

		cout << "TWtracks\t\tTWtracksGoodHypo\n" << m_NTWTracks << "\t\t" << m_NTWTracksGoodHypo << endl;
	}
}



//! \brief Initialize the track fitter object
//! 
//! \param[in] nIter Maximum number of iterations of the fitter
//! \param[in] dPVal Minimum P-value modification cut for fit stop after n-th iteration
//! \return Pointer to the initialized fitter
AbsKalmanFitter* TAGactKFitter::InitializeFitter(int nIter, double dPVal)
{
	AbsKalmanFitter* fitter;

	// initialise the kalman fitter selected from param file
	if ( TAGrecoManager::GetPar()->KalMode() == "on" )
		fitter = new KalmanFitter(nIter, dPVal);
	else if ( TAGrecoManager::GetPar()->KalMode() == "ref" )
		fitter = new KalmanFitterRefTrack(nIter, dPVal);
	else if ( TAGrecoManager::GetPar()->KalMode() == "daf" )
		fitter = new DAF(true, nIter, dPVal);
	else if ( TAGrecoManager::GetPar()->KalMode() == "dafsimple" )
		fitter = new DAF(false, nIter, dPVal);
	else
	{
		Error("TAGactKFitter()", "Undexpected value for Kalman Mode! Given %s", TAGrecoManager::GetPar()->KalMode().c_str());
		exit(42);
	}

	return fitter;
}


//! \brief Initialize the track selector object
//!
//! \return pointer to the initialized selector
TAGFselectorBase* TAGactKFitter::InitializeSelector()
{
	TAGFselectorBase* selector;
	if (TAGrecoManager::GetPar()->PreselectStrategy() == "TrueParticle")
	{
		if (!m_IsMC)
			Error("TAGactKFitter::InitializeSelector()", "Asked TrueParticle tracking but running not on MC."), exit(42);
		selector = new TAGFselectorTrue();
	}
	else if (TAGrecoManager::GetPar()->PreselectStrategy() == "Standard")
		selector = new TAGFselectorStandard();
	else if (TAGrecoManager::GetPar()->PreselectStrategy() == "Linear")
		selector = new TAGFselectorLinear();
	else if (TAGrecoManager::GetPar()->PreselectStrategy() == "Backtracking")
		selector = new TAGFselectorBack();
	else
		Error("TAGactKFitter::InitializeSelector()", "TAGrecoManager::GetPar()->PreselectStrategy() not defined"), exit(42);

	if( !selector )
		Error("TAGactKFitter::InitializeSelector()", "Error in TAGFselector construction, aborting..."), exit(42);
	
	return selector;
}




//----------------------------------------------------------------------------------------------------

//! \brief Perform the actual fit of the selected tracks
//!
//! \param[in] evNum Event number
//! \return Number of fitted tracks in the event
int TAGactKFitter::MakeFit( long evNum , TAGFselectorBase* GFSelector) {

	if ( FootDebugLevel(1) )		cout << "Starting MakeFit " << endl;

	bool isConverged = false, isConvergedPartial = false;
	bool NmeasureCut = false;
	bool convergeCut = false;
	int trackCounter = -1;
	int NconvTracks = 0, NconvTracksYesTW = 0, NconvTracksNoTW = 0;
	int NstartTracks = 0, NstartTracksYesTW = 0, NstartTracksNoTW = 0;

	m_evNum = evNum;
	if(FootDebugLevel(1))
		cout << "\n  ----------------------\nEvento numero " << m_evNum << " track " << m_mapTrack.size() << endl;

	// loop over all tracks
	for ( map<TString,Track*>::iterator trackIt = m_mapTrack.begin(); trackIt != m_mapTrack.end(); ++trackIt)
	{
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
		NstartTracks++;
		int lastMeasId = trackIt->second->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId();
		if( m_SensorIDMap->GetFitPlaneIDFromMeasID(lastMeasId) != m_SensorIDMap->GetFitPlaneTW() ) NstartTracksNoTW++;
		else NstartTracksYesTW++;

		vector<string> tok = TAGparTools::Tokenize( trackIt->first.Data() , "_" );
		string PartName = tok.at(0);

		if(FootDebugLevel(1)) cout << "Track candidate: "<<trackCounter<< "  "<< PartName << " " << trackIt->first.Data() << "\n";

		// check if the category is defined in UpdatePDG  -->  also done in GetPdgCode()
		// This check has to be done only when using the TrueParticle selection -> Naming is different for data-Like!
		if ( TAGrecoManager::GetPar()->PreselectStrategy() == "TrueParticle" )  {

			if ( !UpdatePDG::GetPDG()->IsParticleDefined( tok.at(0) + tok.at(1) ) )
			{
				Error("MakeFit()", "Category %s%s not found in UpdatePDG!" , tok.at(0).c_str(), tok.at(1).c_str());
				exit(42);
			}
		}

		// tmp track to be used inside the loop
		Track* fitTrack = trackIt->second;
		trackCounter++;

	    // check of fitTrack filling
	    if ( FootDebugLevel(2) ) {
		    cout << " check of fitTrack filling " << endl;
		    for (unsigned int iMeas = 0; iMeas < fitTrack->getNumPointsWithMeasurement(); ++iMeas){

				fitTrack->getPointWithMeasurement(iMeas)->getRawMeasurement()->getRawHitCoords().Print();
				int indexOfPlane = static_cast<genfit::PlanarMeasurement*> ( fitTrack->getPointWithMeasurement(iMeas)->getRawMeasurement() )->getPlaneId();

				cout << " index of plane " << indexOfPlane << "  pointID " << iMeas << "\n";
		  	}
		  	cout << "\n";
	    }

		//Skip track if it has less points than what we want
		if ( fitTrack->getNumPointsWithMeasurement() < TAGrecoManager::GetPar()->MeasureN() )
		{
			if( FootDebugLevel(1) )	Info("FillTrackCategoryMap()", "Skipped Track %s with %d TrackPoints with measurement!!", tok.at(2).c_str(), fitTrack->getNumPointsWithMeasurement());
			NmeasureCut = true;
			continue;
		}

	    if ( TAGrecoManager::GetPar()->IsKalReverse() )
	    	fitTrack->reverseTrackPoints();

	    //check
	    fitTrack->checkConsistency();
	    if ( FootDebugLevel(2) )	    fitTrack->Print();

		if( m_IsMC )	EvaluateProjectionEfficiency(fitTrack);

		if( TAGrecoManager::GetPar()->PreselectStrategy() != "TrueParticle" )
			CheckChargeHypothesis(&PartName, fitTrack);

		std::string newTrackName = trackIt->first.Data();
		if(PartName != tok.at(0))
		{
			int NewCharge = m_ParticleIndex[PartName] + 1;
			int NewMass = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(NewCharge) );
			newTrackName = Form("%s_%d_%s", PartName.c_str(), NewMass, tok.at(2).c_str());
		}

		// map of the number of converged tracks for each track hypothesis
		if ( m_nSelectedTrackCandidates.size() <1 || m_nSelectedTrackCandidates.find( PartName ) == m_nSelectedTrackCandidates.end() )
			m_nSelectedTrackCandidates[ PartName ] = 0;
		m_nSelectedTrackCandidates[ PartName ]++;

		if(FootDebugLevel(1))	{
			cout << "PROCESSING TRACK!!!!!\n";
			cout << fitTrack->getCardinalRep()->getPDG() << "\t" << fitTrack->getCardinalRep()->getPDGCharge()<< "\n";
			fitTrack->getCardinalRep()->Print();
		}

	    // THE REAL FIT with different Kalman modes
	    try{

	    	double dPVal = 1.E-3; // convergence criterion
	    	KalmanFitter* preFitter = new KalmanFitter(1, dPVal);
	    	preFitter->processTrackWithRep( fitTrack, fitTrack->getCardinalRep() );

			if(FootDebugLevel(1))
			{
				cout << "Track reps\n";
				for(int i = 0; i < fitTrack->getNumReps(); ++i)
					fitTrack->getTrackRep(i)->Print();
				cout << "Cardinal rep: ";
				fitTrack->getCardinalRep()->Print();
			}

			// ACTUAL FIT
			m_fitter->processTrackWithRep( fitTrack, fitTrack->getCardinalRep() );

			delete preFitter;
		}
		catch (genfit::Exception& e) {
			if( FootDebugLevel(2) )
			{
				std::cerr << e.what();
				std::cerr << "Exception, next track\n";
			}
			continue;
    	}

		if(FootDebugLevel(1))
		{
			cout << "TRACK PROCESSED!!!!!\n";
			fitTrack->getFitStatus(fitTrack->getCardinalRep())->Print();
		}

		// converge check
		isConverged = fitTrack->getFitStatus(fitTrack->getCardinalRep())->isFitted() && fitTrack->getFitStatus(fitTrack->getCardinalRep())->isFitConverged();

		// map of the CONVERGED tracks for each category
		if (isConverged) {

			if ( (TAGrecoManager::GetPar()->Chi2Cut() < 0) || ( m_fitter->getRedChiSqu(fitTrack, fitTrack->getCardinalRep()) <= TAGrecoManager::GetPar()->Chi2Cut() ) ) {
				NconvTracks++;
				if( m_SensorIDMap->GetFitPlaneIDFromMeasID(lastMeasId) != m_SensorIDMap->GetFitPlaneTW() ) NconvTracksNoTW++;
				else NconvTracksYesTW++;

				if ( m_nConvergedTracks_all.size() < 1 || m_nConvergedTracks_all.find( PartName ) == m_nConvergedTracks_all.end() )
					m_nConvergedTracks_all[ PartName ] = 0;
				m_nConvergedTracks_all[ PartName ]++;

				RecordTrackInfo( fitTrack, newTrackName );
				if(FootDebugLevel(1)) cout << "DONE\n";

			}
			m_vectorConvergedTrack.push_back( fitTrack );
		}
		else
			convergeCut = true;

	} // end  - loop over all tracks

	// filling event display with converged tracks
	if ( TAGrecoManager::GetPar()->EnableEventDisplay() && m_vectorConvergedTrack.size() > 0) {
		if (NconvTracks > 1)
			cout << "Event::" << (long)gTAGroot->CurrentEventId().EventNumber() << " display->addEvent size " << NconvTracks << " at position " << m_eventDisplayCounter << "\n";
		m_eventDisplayCounter++;
		display->addEvent(m_vectorConvergedTrack);
	}
	m_vectorConvergedTrack.clear();

	//Filling control histograms
	if( TAGrecoManager::GetPar()->IsSaveHisto() )
	{
		h_nConvTracksVsStartTracks->Fill(NconvTracks, NstartTracks);
		if(NmeasureCut) h_nConvTracksVsStartTracksNmeasureCut->Fill(NconvTracks, NstartTracks);
		if(convergeCut) h_nConvTracksVsStartTracksConvergeCut->Fill(NconvTracks, NstartTracks);
		h_nConvTracksVsStartTracksNoTW->Fill(NconvTracksNoTW, NstartTracksNoTW);
		h_nConvTracksVsStartTracksYesTW->Fill(NconvTracksYesTW, NstartTracksYesTW);
		h_nTracksPerEv->Fill( NconvTracks );
	}

	if ( FootDebugLevel(1) )		cout << "Ready for the next track fit!\n";

	return NconvTracks;
}



//! \brief Record the information regarding a fitted+converged track
//!
//! \param[in] track Pointer to fitted track to save
//! \param[in] fitTrackName Name of the fitted track (example He_4_2001 -> tracklet 1 of VTX 2, with Helium 4 as starting particle hypothesis)
void TAGactKFitter::RecordTrackInfo( Track* track, string fitTrackName ) {

	if(FootDebugLevel(1))		cout << "RECORD START" << endl;
	TAGtrack* shoeOutTrack(0x0);
	bool hasTwPoint = false;
	vector<TAGpoint*> shoeTrackPointRepo;
	Int_t TwChargeZ = -1;
	TVector3 glbPosAtTW(0,0,0);
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
		int iSensor, iClus;
		vector<int> iPart;
		TVector3 measPos, measPos_err;
		fTrackUtilities->GetMeasInfo( trackDetID, trackHitID, &iSensor, &iClus, &measPos, &measPos_err );
		string detName = m_SensorIDMap->GetDetNameFromMeasID( trackHitID );

		// vector with index of the mc truth particles generating the measurement
		if( m_IsMC )
		{
			iPart = m_measParticleMC_collection->at( trackHitID );
			for ( unsigned int ip=0; ip<iPart.size(); ip++)
				mcID_Meas.push_back( iPart.at(ip) );
			mcParticleID_track.push_back(mcID_Meas);
		}

		// create shoe track points vector repository to be passed to the output track object, with general and measurement info
		TAGpoint* shoeTrackPoint = new TAGpoint( detName, iSensor, iClus, &iPart, &measPos, &measPos_err );

		//RZ: First easy implementation -> Check in future when you have more TWpoints in the track
		if(detName == "TW")
		{
			TATWpoint* point = ( (TATWntuPoint*) gTAGroot->FindDataDsc(FootActionDscName("TATWntuPoint"))->Object() )->GetPoint( iClus );
			TwChargeZ = point->GetChargeZ();
			TwTof = point->GetToF();
			glbPosAtTW = m_GeoTrafo->FromTWLocalToGlobal(point->GetPositionG());
			shoeTrackPoint->SetEnergyLoss(point->GetEnergyLoss());
			shoeTrackPoint->SetElementsN(0);
			hasTwPoint = true;
		}
		else
		{
			TAGcluster* clus;
			if(detName == "MSD")
			{
				clus = ( (TAMSDntuCluster*) gTAGroot->FindDataDsc(FootActionDscName("TAMSDntuCluster"))->Object() )->GetCluster( iSensor, iClus );
				shoeTrackPoint->SetEnergyLoss( ((TAMSDcluster*)clus)->GetEnergyLoss() );
			}
			else if(detName == "IT")
				clus = ( (TAITntuCluster*)gTAGroot->FindDataDsc(FootActionDscName("TAITntuCluster"))->Object() )->GetCluster( iSensor, iClus );
			else if(detName == "VT")
				clus = ( (TAVTntuCluster*)gTAGroot->FindDataDsc(FootActionDscName("TAVTntuCluster"))->Object() )->GetCluster( iSensor, iClus );

			shoeTrackPoint->SetElementsN(clus->GetElementsN());
		}

		// getRecoInfo
		TVector3 recoPos, recoMom;
		TMatrixD recoPos_cov(3,3), recoMom_cov(3,3);

		fTrackUtilities->GetRecoTrackInfo( iMeas, track, &recoPos, &recoMom, &recoPos_cov, &recoMom_cov );
		shoeTrackPoint->SetRecoInfo( &recoPos, &recoMom, &recoPos_cov, &recoMom_cov );   // fill with reconstructed info

		// fill shoe track points vector repository to be passed to the output track object
		shoeTrackPointRepo.push_back( shoeTrackPoint );
	}

	//Start track variables recording

	// Retrieve tracking info!
	int nMeas = track->getNumPointsWithMeasurement();
	int pdgID = track->getCardinalRep()->getPDG();
	int pdgCh = track->getCardinalRep()->getPDGCharge();
	float startMass	= std::atoi( tok.at(1).c_str() )*m_AMU;                                  //Initial mass of the fit in GeV
	int fitCh = track->getCardinalRep()->getCharge( track->getFittedState(0) );            // dipendono dallo stato considerato
	double fitMass = track->getCardinalRep()->getMass( track->getFittedState(0) );           // dipendono dallo stato considerato in GeV

	//Stop if the fitted charge is outside of boundaries
	if(fitCh < 0 || fitCh >  m_GFgeometry->GetGparGeo()->GetBeamPar().AtomicNumber ) return;

	//Vertexing for track length
	if( FootDebugLevel(2))	cout << "Track length before vertexing::" << track->getKalmanFitStatus(track->getCardinalRep())->getTrackLen() << endl;

	//Extrapolate to VTX
	//RZ: Issue!!!!! When trueparticle is active this extrapolation breaks
	TVector3 targetMeas;
	if( TAGrecoManager::GetPar()->PreselectStrategy() == "TrueParticle" )
	{
		targetMeas = TVector3(0,0,0); //RZ: DUMMY!!!
	}
	else if ( TAGrecoManager::GetPar()->PreselectStrategy() == "Backtracking" )
	{
		targetMeas = TVector3(0,0,0); //RZ: DUMMY!!!
	}
	else
	{
		TAVTvertex* vtx = ((TAVTntuVertex*) gTAGroot->FindDataDsc(FootActionDscName("TAVTntuVertex"))->Object() )->GetVertex( std::atoi(tok.at(2).c_str())/1000 ); //Find the vertex associated to the track using the fitTrackName (1000*iVtx + iTracklet)
		targetMeas = m_GeoTrafo->FromVTLocalToGlobal(vtx->GetPosition());
	}

	StateOnPlane state_target_point = track->getFittedState(0);
	double extL_Tgt = 0;
	try
	{
		extL_Tgt = track->getCardinalRep()->extrapolateToPoint( state_target_point, targetMeas );
	}
	catch(const genfit::Exception& e)
	{
		std::cerr << e.what() << "\n";
		state_target_point = track->getFittedState(0);
	}

	if(FootDebugLevel(2))
	{
		cout << "Vertex" << std::atoi(tok.at(2).c_str())/1000  << " has position:"; targetMeas.Print();
		cout << "Extrap to point state::"; state_target_point.Print();
		cout << "Distance point-vtx::" << (state_target_point.getPos() - targetMeas).Mag() << endl;
		cout << "Extrapolation length::" << extL_Tgt << endl;
		cout << "Track length after vertexing::" << fabs(extL_Tgt) + track->getKalmanFitStatus(track->getCardinalRep())->getTrackLen() << endl;
	} //End VTX; 

	StateOnPlane state_tg_air_point = track->getFittedState(0);
	TVector3 recoPos_tgInt;
	TVector3 recoMom_tgInt;
	double extL_TgtAir = 0;
	try
	{
		recoPos_tgInt = fTrackUtilities->ExtrapolateToOuterTracker(track, m_SensorIDMap->GetFitPlaneTGairInterface(), recoMom_tgInt, true);
	}
	catch(const genfit::Exception& e)
	{
		std::cerr << e.what() << "\n";
		state_tg_air_point = track->getFittedState(0);
		recoPos_tgInt = state_tg_air_point.getPos();
		recoMom_tgInt = state_tg_air_point.getMom();
	}

	// start TW: extrap some mm after TW for fitted total energy loss
	TVector3 recoPos_TW(0,0,0), recoMom_TW(0,0,0);
	TMatrixD recoPos_TW_cov(3,3), recoMom_TW_cov(3,3);
	double energyOutTw = -1;
	if( hasTwPoint )
	{
		// getRecoInfo
		fTrackUtilities->GetRecoTrackInfo(-1, track, &recoPos_TW, &recoMom_TW, &recoPos_TW_cov, &recoMom_TW_cov );

		StateOnPlane state_TW = track->getFittedState(-1);

		TVector3 origin_( 0, 0, m_GeoTrafo->FromTWLocalToGlobal(m_GFgeometry->GetTWparGeo()->GetLayerPosition(1)).z() + 0.5 );
		genfit::SharedPlanePtr TWextrapPlane (new genfit::DetPlane(origin_, TVector3(0,0,1)));
		TWextrapPlane->setU(1.,0.,0.);
		TWextrapPlane->setV(0.,1.,0.);

		//Try to extrapolate to the TW plane and get the energy of the fragment at the exit of the TW
		try
		{
			double extL_TW = track->getCardinalRep()->extrapolateToPlane( state_TW, TWextrapPlane );
			energyOutTw = TMath::Sqrt( pow(track->getCardinalRep()->getMomMag(state_TW), 2) + pow(fitMass, 2) ) - fitMass; //Energy after TW
		}
		catch (genfit::Exception& e)
		{
			if( FootDebugLevel(2) )
			{
				std::cerr << e.what();
				std::cerr << "Exception, particle is too slow to escape TW. Setting final energy to 0\n";
			}
			state_TW = track->getFittedState(-1);
			energyOutTw = 0;
		}
	}
	//End TW

	// get reco info at the target level (by now at the first VT layer)
	TVector3 recoPos_target, recoMom_target;
	TMatrixD recoPos_target_cov(3,3), recoMom_target_cov(3,3);

	fTrackUtilities->GetRecoTrackInfo(0, track, &recoPos_target, &recoMom_target, &recoPos_target_cov, &recoMom_target_cov ); //This might be unnecessary...

	//Get length and Time-Of-Flight of the track -> Both from the first to the last point of the track!
	float length, tof;
	if( track->hasKalmanFitStatus(track->getCardinalRep()) )
	{
		length = track->getKalmanFitStatus( track->getCardinalRep() )->getTrackLen() + fabs(extL_Tgt); //Track length from Tgt to TW
		tof	= track->getCardinalRep()->getTime(track->getFittedState(-1)) - track->getCardinalRep()->getTime(state_target_point); //TOF from Tgt to TW
	}
	double energyAtTgt = TMath::Sqrt( pow(track->getCardinalRep()->getMomMag(state_target_point), 2) + pow(fitMass, 2) ) - fitMass; //Energy at Tgt

	if(FootDebugLevel(2))
	{
		cout << "fitCh::" << fitCh << "\tfitMass::" << fitMass << endl;
		cout << "Energy at target::" << energyAtTgt << endl;
		cout << "TOF::" << tof << endl;
		cout << "TOF with extrap states::" << track->getCardinalRep()->getTime(track->getFittedState(-1)) - track->getCardinalRep()->getTime(state_target_point) << endl;
		cout << "Energy loss::" << energyAtTgt - energyOutTw << endl;
	}

	double chi2 		= m_fitter->getRedChiSqu(track, track->getCardinalRep());
	int ndof 			= track->getFitStatus( track->getCardinalRep() )->getNdf();
	double chisquare 	= track->getFitStatus( track->getCardinalRep() )->getChi2();
	double pVal 		= track->getFitStatus( track->getCardinalRep() )->getPVal();

	if(FootDebugLevel(2))
	{
		cout << "TAGactKFitter::RecordTrackInfo:: DONE chi2 = " << chi2  << endl;
		cout << "TAGactKFitter::RecordTrackInfo:: DONE chisquare = " << chisquare  << endl;
	}

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
	shoeOutTrack->SetHasTwPoint(hasTwPoint);
	if( hasTwPoint )
	{
		shoeOutTrack->SetTwChargeZ(TwChargeZ);
		shoeOutTrack->SetTwTof(TwTof);
	}
	//Energy in GeV
	shoeOutTrack->SetFitEnergy( energyAtTgt);
	if(energyOutTw >= 0)
		shoeOutTrack->SetFitEnergyLoss( energyAtTgt - energyOutTw );
	
	//Calculate emission angles wrt BM track
	TVector3 TrackDir(-100,-100,-100);
	if( static_cast<TABMntuTrack*> (gTAGroot->FindDataDsc(FootActionDscName("TABMntuTrack"))->Object())->GetTracksN() > 0 )
	{
		TVector3 BMslope = static_cast<TABMntuTrack*> (gTAGroot->FindDataDsc(FootActionDscName("TABMntuTrack"))->Object() )->GetTrack(0)->GetSlope();
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
	int mcCharge = 0;
	int trackMC_id = -1;
	double trackQuality = -1;
	if ( m_IsMC ) {

		TVector3 mcMom, mcPos;

		trackMC_id = FindMostFrequent( &mcParticleID_track );
		//Check if track is mostly VT pile-up, if not go ahead
		if(trackMC_id != -666) 
		{
			// trackMC_id = track->getMcTrackId();     //???????
			TAMCpart* particle = m_trueParticleRep->GetTrack( trackMC_id );
			if( m_trueMomentumAtTgt.find(trackMC_id) == m_trueMomentumAtTgt.end() )// || particle->GetMotherID() == 0 )
				mcMom = particle->GetInitP();
			else
				mcMom = m_trueMomentumAtTgt[trackMC_id];
			mcPos = particle->GetInitPos();
			mcCharge = particle->GetCharge();

			if( ValidHistogram() )
			{
				h_trackMC_true_id->Fill( trackMC_id );
				h_mcMom->Fill( mcMom.Mag() );
				h_mcPosX->Fill( mcPos.X() );
				h_mcPosY->Fill( mcPos.Y() );
				h_mcPosZ->Fill( mcPos.Z() );
			}
			if(FootDebugLevel(1))
			{
				cout << "\n\nTAGactKFitter::RecordTrackInfo:: True Pos z = "<< mcPos.z() << "     p = "<< mcMom.Mag() << "  " << fitTrackName<< endl;
				cout << "TAGactKFitter::RecordTrackInfo:: Reco Pos z= "<< recoPos_target.z() << "     p = "<< recoMom_target.Mag() << endl<<endl<<endl;
			}

			m_trackAnalysis->FillMomentumInfo( recoMom_tgInt, mcMom, recoMom_target_cov, PartName, &h_deltaP, &h_sigmaP );

			m_trackAnalysis->Fill_MomentumResidual( recoMom_tgInt, mcMom, recoMom_target_cov, PartName, &h_dPOverP_x_bin, &h_dPOverP_vs_P );

			trackQuality = TrackQuality( &mcParticleID_track );
			if(FootDebugLevel(1)) cout << "trackQuality::" << trackQuality << "\n";


			if ( mcCharge == fitCh && trackQuality > 0.7 ) {
				if ( m_nConvergedTracks_matched.size() < 1 || m_nConvergedTracks_matched.find( PartName ) == m_nConvergedTracks_matched.end() )
					m_nConvergedTracks_matched[ PartName ] = 0;
				
				m_nConvergedTracks_matched[ PartName ]++;
			}
			else
			{
				if(FootDebugLevel(1))	cout << "NOT MATCHED => evt::" << (long)gTAGroot->CurrentEventId().EventNumber() << "\tfitCh::" << fitCh << "\tmcCh::" << mcCharge << "\ttrQ::" << trackQuality << "\n";
			}

			shoeOutTrack->SetQuality( trackQuality );
			if( ValidHistogram() )
			{
				h_chargeMC->Fill( mcCharge );
				h_trackQuality->Fill( trackQuality );
				h_trackQualityVsNhits->Fill( trackQuality, shoeOutTrack->GetPointsN() );
				h_trackQualityVsChi2->Fill( trackQuality, chi2 );
				if( shoeOutTrack->HasTwPoint() )
					if(shoeOutTrack->GetTwChargeZ() > 0 && shoeOutTrack->GetTwChargeZ() < 9)
						h_trackQuality_Z[shoeOutTrack->GetTwChargeZ() - 1]->Fill( trackQuality );
				h_trackMC_reco_id->Fill( m_IsotopesIndex[ UpdatePDG::GetPDG()->GetPdgName( pdgID ) ] );
				h_momentum_true.at(fitCh)->Fill( mcMom.Mag() );	// check if not present
				h_ratio_reco_true.at(fitCh)->Fill( recoMom_tgInt.Mag()/mcMom.Mag() );	// check if not present
			}
		}

		if(FootDebugLevel(2))	cout << "TAGactKFitter::RecordTrackInfo:: DONE MC = "<< endl;
	}

	//Histogram filling
	if( ValidHistogram() )
	{
		h_dR->Fill ( recoMom_target.DeltaR( TVector3(0,0,0) ) );
		h_phi->Fill ( recoMom_target.Phi() );
		h_theta->Fill ( recoMom_target.Theta()*TMath::RadToDeg() );

		// histos wrt BM track
		if ( shoeOutTrack->GetTgtThetaBm() >= 0 )
		{
			float theta_deg = shoeOutTrack->GetTgtThetaBm()*TMath::RadToDeg();
			float phi_deg = shoeOutTrack->GetTgtPhiBm()*TMath::RadToDeg();
			h_theta_BM->Fill ( theta_deg );
			h_phi_BM->Fill ( phi_deg );
			h_trackDirBM->Fill(TrackDir.X(), TrackDir.Y());

			if( static_cast<TABMntuTrack*> (gTAGroot->FindDataDsc(FootActionDscName("TABMntuTrack"))->Object())->GetTracksN() > 0 && shoeOutTrack->HasTwPoint() )
			{
				TABMtrack* bmTrack = static_cast<TABMntuTrack*> (gTAGroot->FindDataDsc(FootActionDscName("TABMntuTrack"))->Object() )->GetTrack(0);
				TVector3 BMorigin = bmTrack->GetOrigin();
				TVector3 BMslope = bmTrack->GetSlope();
				BMorigin = m_GeoTrafo->FromBMLocalToGlobal(BMorigin);
				BMslope = m_GeoTrafo->VecFromBMLocalToGlobal(BMslope);

				float posZtgt = m_GeoTrafo->FromTGLocalToGlobal(TVector3(0,0,0)).Z();
				posZtgt = m_GeoTrafo->FromGlobalToBMLocal(TVector3(0,0,posZtgt)).Z();
				TVector3 BMextrapAtTgt = m_GeoTrafo->FromBMLocalToGlobal( bmTrack->Intersection(posZtgt) );

				TVector3 vec_track = glbPosAtTW - BMextrapAtTgt;
				double thetaTW = vec_track.Angle(BMslope)*TMath::RadToDeg();
				h_thetaGlbVsThetaTW->Fill(theta_deg, thetaTW);
			}

			if( shoeOutTrack->HasTwPoint() )
			{
				h_theta_BMyesTw->Fill ( theta_deg );
				h_phi_BMyesTw->Fill ( phi_deg );
				if(shoeOutTrack->GetTwChargeZ() > 0 && shoeOutTrack->GetTwChargeZ() < 9)
					h_theta_BM_Z[shoeOutTrack->GetTwChargeZ()-1]->Fill( theta_deg );
			}
			else
			{
				h_theta_BMnoTw->Fill ( theta_deg );
				h_phi_BMnoTw->Fill ( phi_deg );
			}

			int index = theta_deg >= 15 ? 15 : floor(theta_deg);
			
			//TW projection -> try statement needed to avoid tracks dying way before TW
			try 
			{
				TVector3 guessOnTW = fTrackUtilities->ExtrapolateToOuterTracker(track, m_SensorIDMap->GetFitPlaneTW());
				
				h_TWprojVsTheta[index]->Fill(guessOnTW.X(), guessOnTW.Y());
				h_TWprojVsThetaTot->Fill(guessOnTW.X(), guessOnTW.Y());
				if( shoeOutTrack->HasTwPoint() )
				{
					h_TWprojVsThetaTotYesTW->Fill(guessOnTW.X(), guessOnTW.Y());
					h_TWprojVsThetaYesTW[index]->Fill(guessOnTW.X(), guessOnTW.Y());
					float zTW = shoeOutTrack->GetTwChargeZ();
					zTW = guessOnTW.Y() < 1.1 ? -zTW : zTW;
					h_TWprojZTot->Fill(zTW);
					h_TWprojZvsTheta[index]->Fill(zTW);
				}
				else
				{
					h_TWprojVsThetaTotNoTW->Fill(guessOnTW.X(), guessOnTW.Y());
					h_TWprojVsThetaNoTW[index]->Fill(guessOnTW.X(), guessOnTW.Y());
				}
			}
			catch( genfit::Exception & ex )
			{
				if ( FootDebugLevel(2) )
					Info("RecordTrackInfo()", "Extrapolation to TW failed for track %s! Skipping TW projection plots...", fitTrackName.c_str());
			}

			//TG projection
			h_TGprojVsTheta[index]->Fill(recoPos_target.X(), recoPos_target.Y());
			h_TGprojVsThetaTot->Fill(recoPos_target.X(), recoPos_target.Y());
			if( shoeOutTrack->HasTwPoint() )
			{
				h_TGprojVsThetaTotYesTW->Fill(recoPos_target.X(), recoPos_target.Y());
				h_TGprojVsThetaYesTW[index]->Fill(recoPos_target.X(), recoPos_target.Y());
			}
			else
			{
				h_TGprojVsThetaTotNoTW->Fill(recoPos_target.X(), recoPos_target.Y());
				h_TGprojVsThetaNoTW[index]->Fill(recoPos_target.X(), recoPos_target.Y());
			}
		}
		
		h_eta->Fill ( recoMom_target.Eta() );
		h_dx_dz->Fill ( recoMom_target.x() / recoMom_target.z() );
		h_dy_dz->Fill ( recoMom_target.y() / recoMom_target.z() );

		h_nMeas->Fill ( nMeas );
		h_mass->Fill( fitMass );
		h_chi2->Fill( chi2 );
		if( shoeOutTrack->HasTwPoint() && shoeOutTrack->GetTwChargeZ() > 0 && shoeOutTrack->GetTwChargeZ() < 9)
			h_chi2_Z[shoeOutTrack->GetTwChargeZ()-1]->Fill( chi2 );

		h_chargeMeas->Fill( fitCh );
		h_chargeFlip->Fill( fitCh, shoeOutTrack->GetTwChargeZ() );

		h_length->Fill( length );
		h_tof->Fill( tof );
		h_pVal->Fill( pVal );

		h_momentum->Fill( recoMom_tgInt.Mag() );
		h_momentum_reco.at(fitCh)->Fill( recoMom_tgInt.Mag() );	// check if not present

		if( shoeOutTrack->HasTwPoint() )
		{
			int pointID = m_SensorIDMap->GetHitIDFromMeasID(track->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId());
			float TOF = ( (TATWntuPoint*) gTAGroot->FindDataDsc(FootActionDscName("TATWntuPoint"))->Object() )->GetPoint( pointID )->GetMeanTof();
			float var = m_BeamEnergy/m_AMU;
			float beam_speed = TAGgeoTrafo::GetLightVelocity()*TMath::Sqrt(var*(var + 2))/(var + 1);
			TOF -= (m_GeoTrafo->GetTGCenter().Z()-m_GeoTrafo->GetSTCenter().Z())/beam_speed;
			float beta = shoeOutTrack->GetLength()/(TOF*TAGgeoTrafo::GetLightVelocity());
			if( shoeOutTrack->GetTwChargeZ() != shoeOutTrack->GetFitChargeZ() )
				recoMom_tgInt.SetMag( recoMom_tgInt.Mag()*shoeOutTrack->GetTwChargeZ()/shoeOutTrack->GetFitChargeZ() );
			float recomass = recoMom_tgInt.Mag()*sqrt(1 - pow(beta,2))/(beta*m_AMU);

			h_RecoMass[0]->Fill( recomass );
			h_pvsTOF[0]->Fill(TOF, recoMom_tgInt.Mag());
			if( shoeOutTrack->GetTwChargeZ() < 10 )
			{
				h_RecoMass[shoeOutTrack->GetTwChargeZ()]->Fill( recomass );
				h_pvsTOF[shoeOutTrack->GetTwChargeZ()]->Fill(TOF, recoMom_tgInt.Mag());
			}
		}
		
		//Fill residual and pull plots
		std::pair<string, std::pair<int,int>> sensId;
		int cluster_size;
		float res, pull;
		std::vector<float> msdCoords[2];
		for(auto it : shoeTrackPointRepo){
			//Cycle on X and Y
			for(int view=0; view<=1; ++view)
			{
				if( (string)it->GetDevName() == "MSD" && m_SensorIDMap->GetMSDsensorView(it->GetSensorIdx()) != view) continue;

				sensId = make_pair(it->GetDevName(),make_pair(it->GetSensorIdx(),view));
				cluster_size = it->GetElementsN();
				res = it->GetMeasPosition()(view) - it->GetFitPosition()(view);
				pull = res/TMath::Sqrt(pow(it->GetMeasPosError()(view), 2) - pow(it->GetFitPosError()(view), 2));
				h_residual[sensId]->Fill(res);
				h_residualVsPos[sensId]->Fill(res, it->GetFitPosition()(view));
				h_pull[sensId]->Fill(pull);
				if( (string)it->GetDevName() != "TW" )
					h_pullVsClusSize[sensId]->Fill(pull, cluster_size);
			}
		}
	}

	if(FootDebugLevel(1))	cout << "TAGactKFitter::RecordTrackInfo:: DONE HISTOGRAM FILL "  << endl;

	for(auto it : shoeTrackPointRepo)
		delete it;
	shoeTrackPointRepo.clear();
}


//! \brief Find possible matching CALO clusters for global tracks w/ a converged Kalman Filter fit
void TAGactKFitter::MatchCALOclusters()
{
	TACAntuCluster* caNtuCluster = (TACAntuCluster*) gTAGroot->FindDataDsc(FootActionDscName("TACAntuCluster"))->Object() ;
	if( caNtuCluster->GetClustersN() < 1 )
	{
		if( FootDebugLevel(2))
			Info("MatchCALOClusters()", "No CALO clusters to match in event %ld", (long)gTAGroot->CurrentEventId().EventNumber());
		return;
	}

	double extrapDistZ, extrapDistZmatch;
	double posDist,posDistMatch;
	bool found;
	TVector3 twPos, twDir;
	TVector3 caPos, extrapPos;
	TACAcluster* clus;
	int matchIndex;

	TAGtrack* track;
	for(int iTrack = 0; iTrack < m_outTrackRepo->GetTracksN(); ++iTrack)
	{
		found = false;
		track = m_outTrackRepo->GetTrack(iTrack);
		if( !track->HasTwPoint() ) continue;

		twPos = track->GetTwPosition();
		twDir = track->GetTwMomentum();
		twDir *= 1/twDir.Z();
		posDistMatch = m_CALOextrapTolerance;

		if( FootDebugLevel(2) )
		{
			cout << "TW_POS::"; twPos.Print();
			cout << "TW_DIR::"; twDir.Print();
		}

		for( int i=0; i<caNtuCluster->GetClustersN(); ++i )
		{
			clus = caNtuCluster->GetCluster(i);
			if(FootDebugLevel(2))
			{
				cout << "CLUS_ID::" << i << endl;
				cout << "POS::"; clus->GetPosition().Print();
				cout << "POSG::"; clus->GetPositionG().Print();
				cout << "GLOB::"; m_GeoTrafo->FromCALocalToGlobal(clus->GetPositionG()).Print();
			}
			caPos = m_GeoTrafo->FromCALocalToGlobal( clus->GetPositionG() );

			extrapDistZ =   ( twDir.X()*(caPos.X() - twPos.X())
							+ twDir.Y()*(caPos.Y() - twPos.Y())
							+ twDir.Z()*(caPos.Z() - twPos.Z()) )/(twDir.Mag2());
			
			extrapPos = twPos + extrapDistZ*twDir;
			posDist = (extrapPos - caPos).Mag();

			if( FootDebugLevel(2) )
			{
				cout << "Extrap length::" << extrapDistZ << endl;
				cout << "Extrap pos::"; extrapPos.Print();
				cout << "Dist from cluster::" << posDist << endl;
			}

			if( posDist < posDistMatch)
			{
				if(FootDebugLevel(2))	cout << "MATCHED ID::" << i << endl;

				posDistMatch = posDist;
				extrapDistZmatch = extrapDistZ;
				found = true;
				matchIndex = i;
			}
		}

		if(found)
			track->SetCALOmatchedClusterId(matchIndex);
	}
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

//! \brief Print the overall purity of fitted tracks for each charge value
void TAGactKFitter::PrintPurity()
{
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
		cout << "Purity " << *itPart << " = " << eff << "  " << int(kk) << " " << int(nn) << endl;

		totalNum+=kk;
		totalDen+=nn;

		h_trackMatched->SetBinContent(k, kk);
		h_trackMatched->SetBinError(k, TMath::Sqrt(kk));

		h_purity->SetBinContent(k, eff);
		h_purity->SetBinError(k, sqrt(variance));

		h_trackMatched->GetXaxis()->SetBinLabel(k, (*itPart).c_str() );
		h_purity->GetXaxis()->SetBinLabel(k, (*itPart).c_str() );
	}

	cout << "Total Purity " << " = " << totalNum/totalDen << "  " << int(totalNum) << " " << int(totalDen) << endl;

	AddHistogram(h_trackMatched);
	AddHistogram(h_purity);
}


//----------------------------------------------------------------------------------------------------

//! \brief Print the overall efficiency of the fit algorithm for each charge value
void TAGactKFitter::PrintEfficiency()
{
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
		cout << "Efficiency " << *itPart << " = " << eff << "  " << int(kk) << " " << int(nn) << endl;

		totalNum+=kk;
		totalDen+=nn;

		h_trackConverged->SetBinContent(k, kk);
		h_trackConverged->SetBinError(k, TMath::Sqrt(kk));
		h_trackSelected->SetBinContent(k, nn);
		h_trackSelected->SetBinError(k, TMath::Sqrt(nn));

		h_trackEfficiency->SetBinContent(k, eff);
		h_trackEfficiency->SetBinError(k, sqrt(variance));

		h_trackConverged->GetXaxis()->SetBinLabel(k, (*itPart).c_str() );
		h_trackSelected->GetXaxis()->SetBinLabel(k, (*itPart).c_str() );
		h_trackEfficiency->GetXaxis()->SetBinLabel(k, (*itPart).c_str() );
	}

	cout << "Total Efficiency " << " = " << totalNum/totalDen << "  " << int(totalNum) << " " << int(totalDen) << endl;

	h_trackEfficiency->SetTitle(0);
	h_trackEfficiency->SetStats(0);
	h_trackEfficiency->GetYaxis()->SetTitle("Reco Efficiency");
	h_trackEfficiency->GetYaxis()->SetTitleOffset(1.1);
	h_trackEfficiency->GetYaxis()->SetRange(0.,1.);
	h_trackEfficiency->SetLineWidth(2); // take short ~ int

	AddHistogram(h_trackSelected);
	AddHistogram(h_trackConverged);
	AddHistogram(h_trackEfficiency);
}


//! \brief Calculate the true momentum of particles at the TG-AIR crossing
void TAGactKFitter::CalculateTrueMomentumAtTgt()
{
	if(TAGrecoManager::GetPar()->IsRegionMc())
	{
		TAMCntuRegion* mcNtuReg = (TAMCntuRegion*)gTAGroot->FindDataDsc(FootActionDscName("TAMCntuRegion"))->Object();
		for(int i = 0; mcNtuReg && i < mcNtuReg->GetRegionsN(); ++i)
		{
			TAMCregion* mcReg = (TAMCregion*)mcNtuReg->GetRegion(i);
			if( mcReg->GetOldCrossN() == fTGregion && mcReg->GetCrossN() == fAIRregion )
				m_trueMomentumAtTgt[ mcReg->GetTrackIdx() - 1 ] = mcReg->GetMomentum();
		}
	}

	for(int i=0; i< m_trueParticleRep->GetTracksN(); ++i)
	{
		if( m_trueMomentumAtTgt.find(i) != m_trueMomentumAtTgt.end() ) continue;

		TAMCpart* part = (TAMCpart*)m_trueParticleRep->GetTrack(i);
		m_trueMomentumAtTgt[ i ] = part->GetInitP();
	}
}


//----------------------------------------------------------------------------------------------------

//! \brief Print the overall selection efficiency of tracks for each charge value
void TAGactKFitter::PrintSelectionEfficiency()
{
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
		cout << "Efficiency Selection " << *itPart << " = " << eff << "  " << int(kk) << " " << int(nn) << endl;

		totalNum+=kk;
		totalDen+=nn;

		h_trackProduced->SetBinContent(k, nn);

		h_selectEfficiency->SetBinContent(k, eff);
		h_selectEfficiency->SetBinError(k, sqrt(variance));

		h_selectEfficiency->GetXaxis()->SetBinLabel(k, (*itPart).c_str() );
	}

	cout << "Total Efficiency Selection " << " = " << totalNum/totalDen << "  " << int(totalNum) << " " << int(totalDen) << endl;

	h_selectEfficiency->SetTitle(0);
	h_selectEfficiency->SetStats(0);
	h_selectEfficiency->GetYaxis()->SetTitle("Sel Efficiency");
	h_selectEfficiency->GetYaxis()->SetTitleOffset(1.1);
	h_selectEfficiency->GetYaxis()->SetRange(0.,1.);
	h_selectEfficiency->SetLineWidth(2); // take short ~ int

	AddHistogram(h_trackProduced);
	AddHistogram(h_selectEfficiency);
}


//! \brief Declare the GenFit histograms
void TAGactKFitter::CreateHistogram()	{

	h_GFeventType = new TH1I("h_GFeventType", "h_GFeventType;Event type;Entries", 5, 0.5, 5.5);
	AddHistogram(h_GFeventType);

	h_nTracksPerEv= new TH1F("h_nTracksPerEv", "h_nTracksPerEv;nTracks;Entries", 15, -0.5, 14.5);
	AddHistogram(h_nTracksPerEv);

	h_nConvTracksVsStartTracks = new TH2I("h_nConvTracksVsStartTracks", "h_nConvTracksVsStartTracks;nConvTracks;nStartTracks", 15, -0.5, 14.5, 15, -0.5, 14.5);
	AddHistogram(h_nConvTracksVsStartTracks);

	h_nConvTracksVsStartTracksNmeasureCut = new TH2I("h_nConvTracksVsStartTracksNmeasureCut", "h_nConvTracksVsStartTracksNmeasureCut;nConvTracks;nStartTracks", 15, -0.5, 14.5, 15, -0.5, 14.5);
	AddHistogram(h_nConvTracksVsStartTracksNmeasureCut);

	h_nConvTracksVsStartTracksConvergeCut = new TH2I("h_nConvTracksVsStartTracksConvergeCut", "h_nConvTracksVsStartTracksConvergeCut;nConvTracks;nStartTracks", 15, -0.5, 14.5, 15, -0.5, 14.5);
	AddHistogram(h_nConvTracksVsStartTracksConvergeCut);

	h_nConvTracksVsStartTracksYesTW = new TH2I("h_nConvTracksVsStartTracksYesTW", "h_nConvTracksVsStartTracksYesTW;nConvTracks;nStartTracks", 15, -0.5, 14.5, 15, -0.5, 14.5);
	AddHistogram(h_nConvTracksVsStartTracksYesTW);

	h_nConvTracksVsStartTracksNoTW = new TH2I("h_nConvTracksVsStartTracksNoTW", "h_nConvTracksVsStartTracksNoTW;nConvTracks;nStartTracks", 15, -0.5, 14.5, 15, -0.5, 14.5);
	AddHistogram(h_nConvTracksVsStartTracksNoTW);

	if( m_IsMC )
	{
		h_trackMC_true_id = new TH1F("h_trackMC_true_id", "h_trackMC_true_id", 45, 0., 45);
		AddHistogram(h_trackMC_true_id);

		h_trackMC_reco_id = new TH1F("h_trackMC_reco_id", "h_trackMC_reco_id", 45, 0., 45);
		AddHistogram(h_trackMC_reco_id);

		h_trackQuality = new TH1F("m_trackQuality", "m_trackQuality;Track quality;Entries", 55, 0, 1.1);
		AddHistogram(h_trackQuality);

		h_trackQualityVsNhits = new TH2F("m_trackQualityVsNhits", "m_trackQualityVsNhits;Track quality;N hits in track", 55, 0, 1.1, 16, -0.5, 15.5);
		AddHistogram(h_trackQualityVsNhits);

		h_trackQualityVsChi2 = new TH2F("m_trackQualityVsChi2", "m_trackQualityVsChi2;Track quality;#chi^{2}/n_{dof}", 55, 0, 1.1, 400, 0., 20.);
		AddHistogram(h_trackQualityVsChi2);

		for( int iZ = 1; iZ <= 9; ++iZ )
		{
			h_trackQuality_Z.push_back( new TH1F(Form("h_trackQuality_Z%d",iZ), Form("h_trackQuality_Z%d;Track quality;Entries",iZ), 55, 0, 1.1) );
			AddHistogram(h_trackQuality_Z[h_trackQuality_Z.size() - 1]);
		}
	}

	h_length = new TH1F("m_length", "m_length;Track length [cm];Entries", 400, 0, 200);
	AddHistogram(h_length);

	h_tof = new TH1F("m_tof", "m_tof;Track fitted TOF [ns];Entries", 200, 0, 20);
	AddHistogram(h_tof);

	h_pVal = new TH1F("m_pVal", "m_pVal;p-value of track fit;Entries", 300, -0.1, 1.1);
	AddHistogram(h_pVal);

	if( m_IsMC )
	{
		h_mcPosX = new TH1F("h_mcPosX", "h_mcPosX;MC position X at target [cm];Entries", 400, -1, 1);
		AddHistogram(h_mcPosX);

		h_mcPosY = new TH1F("h_mcPosY", "h_mcPosY;MC position Y at target [cm];Entries", 400, -1, 1);
		AddHistogram(h_mcPosY);

		h_mcPosZ = new TH1F("h_mcPosZ", "h_mcPosZ;MC position Z at target [cm];Entries", 500, -0.25, 0.25);
		AddHistogram(h_mcPosZ);
	}

	h_dR = new TH1F("h_dR", "h_dR", 100, 0., 20.);
	AddHistogram(h_dR);

	h_phi = new TH1F("h_phi", "h_phi;Track #phi at target [rad];Entries", 80, -4, 4);
	AddHistogram(h_phi);

	h_theta = new TH1F("h_theta", "h_theta;Track #theta in FOOT frame [deg];Entries;", 200, 0, 15);
	AddHistogram(h_theta);

	h_theta_BM = new TH1F("h_theta_BM", "h_theta_BM;Track #theta wrt BM [deg]; Entries", 200, 0, 15);
	AddHistogram(h_theta_BM);

	h_phi_BM = new TH1F("h_phi_BM", "h_phi_BM;Track #phi wrt BM [deg]; Entries", 100, -190, 190);
	AddHistogram(h_phi_BM);

	h_trackDirBM = new TH2F("h_trackDirBM", "h_trackDirBM;dX/dZ;dY/dZ", 1001,-1,1,1001,-1,1);
	AddHistogram(h_trackDirBM);

	h_thetaGlbVsThetaTW = new TH2F("h_thetaGlbVsThetaTW", "h_thetaGlbVsThetaTW;theta Glb [deg];theta BM-TW [deg]", 200, 0, 12, 200, 0 ,12);
	AddHistogram(h_thetaGlbVsThetaTW);

	h_theta_BMnoTw = new TH1F("h_theta_BMnoTw", "h_theta_BMnoTw (global track has no TW point);Track #theta wrt BM [deg]; Entries", 200, 0, 15);
	AddHistogram(h_theta_BMnoTw);

	h_phi_BMnoTw = new TH1F("h_phi_BMnoTw", "h_phi_BMnoTw (global track has no TW point);Track #phi wrt BM [deg]; Entries", 100, -190, 190);
	AddHistogram(h_phi_BMnoTw);

	h_theta_BMyesTw = new TH1F("h_theta_BMyesTw", "h_theta_BMyesTw (global track has a TW point);Track #theta wrt BM [deg]; Entries", 200, 0, 15);
	AddHistogram(h_theta_BMyesTw);

	h_phi_BMyesTw = new TH1F("h_phi_BMyesTw", "h_phi_BMyesTw (global track has a TW point);Track #phi wrt BM [deg]; Entries", 100, -190, 190);
	AddHistogram(h_phi_BMyesTw);

	for( int iZ = 1; iZ <= 9; ++iZ )
	{
		h_theta_BM_Z.push_back( new TH1F(Form("h_theta_BM_Z%d",iZ), Form("h_theta_BM_Z%d;Track #theta wrt BM for Z=%d [deg];Entries",iZ,iZ), 200, 0, 15) );
		AddHistogram(h_theta_BM_Z[h_theta_BM_Z.size() - 1]);
	}

	h_eta = new TH1F("h_eta", "h_eta", 100, 0., 20.);
	AddHistogram(h_eta);

	h_dx_dz = new TH1F("h_dx_dz", "h_dx_dz;Track slope X at TG", 110, 0., 0.3);
	AddHistogram(h_dx_dz);

	h_dy_dz = new TH1F("h_dy_dz", "h_dy_dz;Track slope Y at TG", 110, 0., 0.3);
	AddHistogram(h_dy_dz);

	h_nMeas = new TH1F("nMeas", "nMeas;N points in track;Entries", 13, 0., 13.);
	AddHistogram(h_nMeas);

	if ( m_IsMC )
	{
		h_chargeMC = new TH1F("h_chargeMC", "h_chargeMC;MC Z;Entries", 9, 0, 9);
		AddHistogram(h_chargeMC);
	}

	h_chargeMeas = new TH1F("h_chargeMeas", "h_chargeMeas;Measured Z;Entries", 9, 0, 9);
	AddHistogram(h_chargeMeas);

	h_chargeFlip = new TH2F("h_chargeFlip", "h_chargeFlip;Z fit;Z TW", 10, -0.5, 9.5, 10, -0.5, 9.5);
	AddHistogram(h_chargeFlip);

  	h_mass = new TH1F("h_mass", "h_mass;Mass [A.M.U.];Enries", 100, 0., 20.);
	AddHistogram(h_mass);

	h_chi2 = new TH1F("h_chi2", "h_chi2;#chi^{2}/n_{dof};Entries", 400, 0., 20.);
	AddHistogram(h_chi2);
	for( int iZ = 1; iZ <= 9; ++iZ )
	{
		h_chi2_Z.push_back( new TH1F(Form("h_chi2_Z%d",iZ), Form("h_chi2_Z%d;#chi^{2}/n_{dof};Entries",iZ), 400, 0, 20) );
		AddHistogram(h_chi2_Z[h_chi2_Z.size() - 1]);
	}

	if( m_IsMC )
	{
		h_mcMom = new TH1F("h_mcMom", "h_mcMom;Momentum MC at TG [GeV/c];Entries", 200, 0., 17.);
		AddHistogram(h_mcMom);
	}

	h_momentum = new TH1F("h_momentum", "h_momentum;Momentum reco at TG [GeV/c];Entries", 200, 0., 17.);
	AddHistogram(h_momentum);

	h_RecoMass.push_back( new TH1F("h_RecoMass", "h_RecoMass;Mass reco [A.M.U.];Entries", 400, 0, 20) );
	AddHistogram(h_RecoMass[0]);
	for( int iZ = 1; iZ <= 9; ++iZ )
	{
		h_RecoMass.push_back( new TH1F(Form("h_RecoMass_Z%d",iZ), Form("h_RecoMass_Z%d;Mass reco [A.M.U.];Entries",iZ), 400, 0, 20) );
		AddHistogram(h_RecoMass[h_RecoMass.size() - 1]);
	}

	h_pvsTOF.push_back(new TH2F("h_pvsTOF", "h_pvsTOF;TOF [ns];p [GeV/c]", 1000, 0, 25, 400, 0, 17));
	AddHistogram(h_pvsTOF[0]);
	for( int iZ = 1; iZ <= 9; ++iZ )
	{
		h_pvsTOF.push_back( new TH2F(Form("h_pvsTOF_Z%d", iZ), Form("h_pvsTOF_Z%d;TOF [ns];p [GeV/c]", iZ), 1000, 0, 25, 400, 0, 17) );
		AddHistogram(h_pvsTOF[h_pvsTOF.size() - 1]);
	}

	h_TGprojVsThetaTot = new TH2D("TGproj_ThetaTot","TGproj_ThetaTot;X_proj [cm];Y_proj [cm]",400,-5,5,400,-5,5);
	AddHistogram(h_TGprojVsThetaTot);

	h_TGprojVsThetaTotYesTW = new TH2D("TGproj_ThetaTotYesTW","TGproj_ThetaTotYesTW;X_proj [cm];Y_proj [cm]",400,-5,5,400,-5,5);
	AddHistogram(h_TGprojVsThetaTotYesTW);

	h_TGprojVsThetaTotNoTW = new TH2D("TGproj_ThetaTotNoTW","TGproj_ThetaTotNoTW;X_proj [cm];Y_proj [cm]",400,-5,5,400,-5,5);
	AddHistogram(h_TGprojVsThetaTotNoTW);

	for (int i=0; i<16; ++i)
	{
		h_TGprojVsTheta.push_back(new TH2D(Form("TGproj_Theta%d-%d",i,i+1),Form("TGproj_Theta%d-%d;X_proj [cm];Y_proj [cm]",i,i+1),400,-5,5,400,-5,5));
		AddHistogram(h_TGprojVsTheta[i]);

		h_TGprojVsThetaNoTW.push_back(new TH2D(Form("TGproj_ThetaNoTW%d-%d",i,i+1),Form("TGproj_ThetaNoTW%d-%d;X_proj [cm];Y_proj [cm]",i,i+1),400,-5,5,400,-5,5));
		AddHistogram(h_TGprojVsThetaNoTW[i]);

		h_TGprojVsThetaYesTW.push_back(new TH2D(Form("TGproj_ThetaYesTW%d-%d",i,i+1),Form("TGproj_ThetaYesTW%d-%d;X_proj [cm];Y_proj [cm]",i,i+1),400,-5,5,400,-5,5));
		AddHistogram(h_TGprojVsThetaYesTW[i]);
	}

	h_TWprojVsThetaTot = new TH2D("TWproj_ThetaTot","TWproj_ThetaTot;X_proj [cm];Y_proj [cm]",400,-40,40,400,-40,40);
	AddHistogram(h_TWprojVsThetaTot);

	h_TWprojVsThetaTotYesTW = new TH2D("TWproj_ThetaTotYesTW","TWproj_ThetaTotYesTW;X_proj [cm];Y_proj [cm]",400,-40,40,400,-40,40);
	AddHistogram(h_TWprojVsThetaTotYesTW);

	h_TWprojVsThetaTotNoTW = new TH2D("TWproj_ThetaTotNoTW","TWproj_ThetaTotNoTW;X_proj [cm];Y_proj [cm]",400,-40,40,400,-40,40);
	AddHistogram(h_TWprojVsThetaTotNoTW);

	h_TWprojZTot = new TH1D("TWproj_ZTot","TWproj_ZTot;Z*sign(Y_proj)",17,-8.5,8.5);
	AddHistogram(h_TWprojZTot);

	for (int i=0; i<16; ++i)
	{
		h_TWprojVsTheta.push_back(new TH2D(Form("TWproj_Theta%d-%d",i,i+1),Form("TWproj_Theta%d-%d;X_proj [cm];Y_proj [cm]",i,i+1),400,-40,40,400,-40,40));
		AddHistogram(h_TWprojVsTheta[i]);

		h_TWprojVsThetaNoTW.push_back(new TH2D(Form("TWproj_ThetaNoTW%d-%d",i,i+1),Form("TWproj_ThetaNoTW%d-%d;X_proj [cm];Y_proj [cm]",i,i+1),400,-40,40,400,-40,40));
		AddHistogram(h_TWprojVsThetaNoTW[i]);

		h_TWprojVsThetaYesTW.push_back(new TH2D(Form("TWproj_ThetaYesTW%d-%d",i,i+1),Form("TWproj_ThetaYesTW%d-%d;X_proj [cm];Y_proj [cm]",i,i+1),400,-40,40,400,-40,40));
		AddHistogram(h_TWprojVsThetaYesTW[i]);

		h_TWprojZvsTheta.push_back(new TH1D(Form("TWproj_ZvsTheta%d-%d",i,i+1),Form("TWproj_ZvsTheta%d-%d;Z*sign(Y_proj)",i,i+1),17,-8.5,8.5));
		AddHistogram(h_TWprojZvsTheta[i]);
	}

	for (int i = 0; i < 9; ++i)
	{
		h_momentum_reco.push_back(new TH1F(Form("RecoMomentum%d",i), Form("Reco Momentum %d;Momentum [GeV];Entries",i), 1000, 0.,17.));
		AddHistogram(h_momentum_reco[i]);

		if( m_IsMC )
		{
			h_momentum_true.push_back(new TH1F(Form("TrueMomentum%d",i), Form("True Momentum %d;Momentum [GeV];Entries",i), 1000, 0.,17.));
			AddHistogram(h_momentum_true[i]);

			h_ratio_reco_true.push_back(new TH1F(Form("MomentumRatio%d",i), Form("Momentum Ratio %d;p ratio reco/true;Entries",i), 1000, 0, 2.5));
			AddHistogram(h_ratio_reco_true[i]);
		}
	}

	h_PlaneOccupancy[0] = new TH2I("h_PlaneOccupancy", "h_PlaneOccupancy; FitPlane Id; # of clusters", m_SensorIDMap->GetFitPlanesN()+2, -1.5, m_SensorIDMap->GetFitPlanesN()+0.5, 51, -0.5, 50.5);
	AddHistogram(h_PlaneOccupancy[0]);

	for(int iEv=1; iEv<=5; iEv++)
	{
		h_PlaneOccupancy[iEv] = new TH2I(Form("h_PlaneOccupancyType%d", iEv), Form("h_PlaneOccupancyType%d; FitPlane Id; # of clusters", iEv), m_SensorIDMap->GetFitPlanesN()+2, -1.5, m_SensorIDMap->GetFitPlanesN()+0.5, 51, -0.5, 50.5);
		AddHistogram(h_PlaneOccupancy[iEv]);
	}

	//Define residual and pull histograms
	AddResidualAndPullHistograms();

	SetValidHistogram(kTRUE);
	return;
}


//! \brief Declare the histograms for residuals and pulls
void TAGactKFitter::AddResidualAndPullHistograms()
{
	std::pair<string, std::pair<int, int>> sensId;
	std::vector<std::string> detList = TAGparTools::Tokenize(m_systemsON.Data(), " ");

	//Cycle on detectors
	for( auto det : detList )
	{
		int nsensors;
		if( det == "VT" )		nsensors = m_GFgeometry->GetVTparGeo()->GetSensorsN();
		else if( det == "IT" )	nsensors = m_GFgeometry->GetITparGeo()->GetSensorsN();
		else if( det == "MSD" )	nsensors = m_GFgeometry->GetMSDparGeo()->GetSensorsN();
		else if( det == "TW" )	nsensors = 1;

		for(Int_t iSensor=0; iSensor < nsensors; iSensor++)
		{
			// Cycle on X and Y
			for(int iCoord=0; iCoord <= 1; ++iCoord)
			{
				//Skip one coordinate for MSD
				if( det == "MSD" && iCoord != m_SensorIDMap->GetMSDsensorView(iSensor) ) continue;

				sensId = make_pair(det,make_pair(iSensor,iCoord));
				char coord = iCoord ? 'Y' : 'X';
				float maxRes = 0.1, maxExtrap = 0.6;
				if( det == "TW" )
				{
					maxRes = 5;
					maxExtrap = 5;
				}

				h_residual[sensId] = new TH1F(Form("Res_%s_%c_layer_%d",det.c_str(),coord,iSensor),Form("Residual between global track and measured %s cluster in layer %d, %c view;Residual (Meas-Fit) %c [cm];Entries",det.c_str(),iSensor,coord,coord),1000,-maxRes,maxRes);
				AddHistogram(h_residual[sensId]);

				h_residualVsPos[sensId] = new TH2F(Form("Res_VsFitPos_%s_%c_layer_%d",det.c_str(),coord,iSensor),Form("Residual vs %s cluster position in layer %d, %c view;Residual (Meas-Fit) %c [cm];Fitted pos%c [cm]",det.c_str(),iSensor,coord,coord,coord),1000,-maxRes,maxRes,300,-3,3);
				AddHistogram(h_residualVsPos[sensId]);

				h_pull[sensId] = new TH1F(Form("Pull_%s_%c_layer_%d",det.c_str(),coord,iSensor),Form("Pull for %s layer %d on %c view;Pull (Meas-Fit) %c;Entries",det.c_str(),iSensor,coord,coord),1000,-15,15);
				AddHistogram(h_pull[sensId]);

				if(det != "TW")
				{
					std::string clusUnit = "pixels";
					int maxUnits = 80;
					if(det == "MSD")
					{
						clusUnit = "strips";
						maxUnits = 10;
					}

					h_pullVsClusSize[sensId] = new TH2F(Form("PullVsClusSize_%s_%c_layer_%d",det.c_str(),coord,iSensor),Form("Pull vs cluster size for %s layer %d on %c view;Meas-Fit Pull %c;Cluster size [N %s]",det.c_str(),iSensor,coord,coord,clusUnit.c_str()), 1000,-15,15, maxUnits, -0.5, maxUnits-0.5);
					AddHistogram(h_pullVsClusSize[sensId]);
				}

				h_extrapDist[sensId] = new TH1F(Form("ExtrapDist_%s_%c_layer_%d",det.c_str(),coord,iSensor),Form("Extrapolation distance between forward global track and measured %s cluster in layer %d, %c view;Extrapolation distance (Meas-Forward extrap) %c [cm];Entries",det.c_str(),iSensor,coord,coord),600,-maxExtrap,maxExtrap);
				AddHistogram(h_extrapDist[sensId]);
			}
		}
	}
}



//! \brief Set directory for action histograms
//! 
//! Re-implemented from TAGaction to have a subdirectory for global track residuals and pulls
void TAGactKFitter::SetHistogramDir(TDirectory* dir)
{
	TDirectory* subdirRes = 0x0;
	dir->GetObject("TRKRES", subdirRes);
	if( !subdirRes )
		subdirRes = dir->mkdir("TRKRES");

	TDirectory* subdirTWproj = 0x0;
	dir->GetObject("TWproj", subdirTWproj);
	if( !subdirTWproj )
		subdirTWproj = dir->mkdir("TWproj");

	TDirectory* subdirTGproj = 0x0;
	dir->GetObject("TGproj", subdirTGproj);
	if( !subdirTGproj )
		subdirTGproj = dir->mkdir("TGproj");

	TDirectory* subdirExtrap = 0x0;
	dir->GetObject("EXTR", subdirExtrap);
	if( !subdirExtrap )
		subdirExtrap = dir->mkdir("EXTR");

	if (fpHistList) {
		for (TObjLink* lnk = fpHistList->FirstLink(); lnk; lnk=lnk->Next()) {
			TH1* h = (TH1*)lnk->GetObject();
			TString name(h->GetName());
			if( name.Contains("Pull") || name.Contains("Res_") )
				h->SetDirectory(subdirRes);
			else if ( name.Contains("TWproj") )
				h->SetDirectory(subdirTWproj);
			else if ( name.Contains("TGproj") )
				h->SetDirectory(subdirTGproj);
			else if ( name.Contains("ExtrapDist") )
				h->SetDirectory(subdirExtrap);
			else
				h->SetDirectory(dir);
		}
		fbIsOpenFile = true;
	}

	fDirectory = dir;

	if (!dir->IsWritable() || !subdirRes->IsWritable() || !subdirTWproj->IsWritable() || !subdirTGproj->IsWritable() || !subdirExtrap->IsWritable())
		fbIsOpenFile = false;
}


//----------------------------------------------------------------------------------------------------

//! \brief Initialize GenFit event display
void TAGactKFitter::InitEventDisplay() {
  // init event display
  display = genfit::EventDisplay::getInstance();
  display->reset();
}


//! \brief Evaluate the efficiency of the forward projection
//!
//! \param[in] fitTrack Pointer to the track under study
void TAGactKFitter::EvaluateProjectionEfficiency(Track* fitTrack)
{
	int MeasId, PlaneId;
	int chargeHypo, chargeMC;

	if(FootDebugLevel(1))	cout << "Check of charge hypo: " << endl;

	for(int i=0; i<fitTrack->getNumPointsWithMeasurement(); ++i)
	{
		MeasId = fitTrack->getPointWithMeasurement(i)->getRawMeasurement()->getHitId();
		PlaneId = m_SensorIDMap->GetFitPlaneIDFromMeasID( MeasId );
		chargeHypo = fitTrack->getCardinalRep()->getPDGCharge();

		m_NClusTrack.at( PlaneId )++;

		bool good = false;
		for(vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasId).end(); ++itTrackMC)
		{
			if(*itTrackMC == -666)
				chargeMC = -666;
			else
			{
				TAMCpart* particle = m_trueParticleRep->GetTrack(*itTrackMC);
				chargeMC = particle->GetCharge();
			}
			if(FootDebugLevel(1))	
				cout << "Plane::" << PlaneId << "\tChargeHypo::" << chargeHypo << "\tMCCharge::" << chargeMC << "\tMeasId::" << MeasId << "\tMCTrackId::" << *itTrackMC << "\tflagGood::" << good << endl;

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
void TAGactKFitter::CheckChargeHypothesis(string* PartName, Track* fitTrack)
{
	int chargeFromTW = fTrackUtilities->GetChargeFromTW( fitTrack );
	if(FootDebugLevel(1) ) cout << "Charge From TW::" << chargeFromTW << endl;
	if( chargeFromTW < 1 || chargeFromTW > m_GFgeometry->GetGparGeo()->GetBeamPar().AtomicNumber )
	{
		// Info("CheckChargeHypothesis()", "Wrong evaluation of TW charge for track candidate %s. No check performed...", PartName->c_str());
		return;
	}

	m_NTWTracks++;

	//Charge hypo != form TW --> change PartName and reset seed
	if(chargeFromTW != fitTrack->getCardinalRep()->getPDGCharge())
	{
		if(FootDebugLevel(1))	Info("CheckChargeHypothesis()", "Charge Hypo (%d) wrong, changing to measured from TW (%d)", int(fitTrack->getCardinalRep()->getPDGCharge()), chargeFromTW);
		for(int i=0; i<fitTrack->getNumReps(); ++i)
		{
			if(fitTrack->getTrackRep(i)->getPDGCharge() == chargeFromTW)
			{
				fitTrack->setCardinalRep( i );
				break;
			}
		}

		//update the name of the particle associated to track
		*PartName = fTrackUtilities->GetParticleNameFromCharge(chargeFromTW);
	}
	//Charge from TW == chargeHypo --> goodHypo!
	else
		m_NTWTracksGoodHypo++;

	//Reset track seed
	TVector3 pos, mom;
	TVectorD seed = fitTrack->getStateSeed();
	pos.SetX(seed(0)); pos.SetY(seed(1)); pos.SetZ(seed(2));
	if(FootDebugLevel(2))	pos.Print();

	mom.SetX(seed(3)); mom.SetY(seed(4)); mom.SetZ(seed(5));
	if(FootDebugLevel(2))	mom.Print();

	fTrackUtilities->ComputeMainIsotopeMomentum(fitTrack, mom);

	if(FootDebugLevel(2))	mom.Print();

	fitTrack->setStateSeed(pos,mom);
}


//! \brief Clear data for new loop
void TAGactKFitter::ClearData()
{
	m_outTrackRepo->Clear();
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

	m_measParticleMC_collection = 0x0;

	for ( auto trackIt = m_mapTrack.begin(); trackIt != m_mapTrack.end(); ++trackIt)
		delete trackIt->second;
	m_mapTrack.clear();

}


//! \brief Delete all histograms
void TAGactKFitter::ClearHistos()
{
	delete h_trackEfficiency;
	delete h_nTracksPerEv;
	delete h_nConvTracksVsStartTracks;
	delete h_nConvTracksVsStartTracksNmeasureCut;
	delete h_nConvTracksVsStartTracksConvergeCut;
	delete h_nConvTracksVsStartTracksYesTW;
	delete h_nConvTracksVsStartTracksNoTW;
	delete h_length;
	delete h_tof;
	delete h_nMeas;
	delete h_mass;
	delete h_chi2;
	delete h_pVal;
	delete h_chargeMeas;
	delete h_chargeFlip;
	delete h_momentum;
	delete h_dR;
	delete h_phi;
	delete h_theta;
	delete h_theta_BM;
	delete h_thetaGlbVsThetaTW;
	delete h_phi_BM;
	delete h_eta;
	delete h_dx_dz;
	delete h_dy_dz;
	delete h_GFeventType;

	if ( m_IsMC )
	{
		delete h_purity;
		delete h_trackMC_true_id;
		delete h_trackMC_reco_id;
		delete h_mcMom;
		delete h_mcPosX;
		delete h_mcPosY;
		delete h_mcPosZ;
		delete h_trackQuality;
		delete h_chargeMC;

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

	for(auto it : h_momentum_reco)
		delete it;
	h_momentum_reco.clear();

	for(auto it : h_residual)
		delete it.second;
	h_residual.clear();

	for(auto it : h_residualVsPos)
		delete it.second;
	h_residualVsPos.clear();

	for(auto it : h_pull)
		delete it.second;
	h_pull.clear();

	for(auto it : h_pullVsClusSize)
		delete it.second;
	h_pullVsClusSize.clear();
}
