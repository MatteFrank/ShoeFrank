/*!
 \file GlobalRecoMC.cxx
 \brief Implementation of GlobalRecoMC
 */
/*------------------------------------------+---------------------------------*/

#include "GlobalRecoMC.hxx"

/*!
 \class GlobalRecoMC
 \brief Global reconstruction class using GenFit for MC data (depecrated)
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
//ClassImp(GlobalRecoMC)

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
GlobalRecoMC::GlobalRecoMC(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : LocalRecoMC(expName, runNumber, fileNameIn, fileNameout), fTree(0x0)
{

	EnableTracking();
	fFlagMC = fCampManager->GetCampaignPar(fCampManager->GetCurrentCamNumber()).McFlag;;		// to be changed

}

//__________________________________________________________
//! Destructor
GlobalRecoMC::~GlobalRecoMC()
{
}

//__________________________________________________________
//! Before loop
void GlobalRecoMC::BeforeEventLoop()
{
	BaseReco::BeforeEventLoop();

	// genfit::FieldManager::getInstance()->init( new TADIgenField(fField) );

	// // set material and geometry into genfit
	// MaterialEffects* materialEffects = MaterialEffects::getInstance();
	// materialEffects->init(new TGeoMaterialInterface());

	// // include the nucleon into the genfit pdg repository
	// if ( TAGrecoManager::GetPar()->IncludeBM() || TAGrecoManager::GetPar()->IncludeKalman() )
	// UpdatePDG::Instance();

	// // study for kalman Filter
	// m_globalTrackingStudies = new TAGFtrackingStudies("glbActTrackStudy");


	// // Initialisation of KFfitter
	// if ( TAGrecoManager::GetPar()->Debug() > 1 )       cout << "KFitter init!" << endl;
	// m_kFitter = new KFitter("glbActTrackStudy");
	// if ( TAGrecoManager::GetPar()->Debug() > 1 )       cout << "KFitter init done!" << endl;
  

}

//__________________________________________________________
//! Loop event (do not work anymore)
//!
//! \param[in] nEvents events to process
//! \param[in] skipEvent events to skip
void GlobalRecoMC::LoopEvent(Int_t nEvents, Int_t skipEvent)
{
	if(skipEvent >= fTree->GetEntries())
	{
		Error("LoopEvent()", "Skip Event (%d) higher than input tree entries (%lld)", skipEvent, fTree->GetEntries());
		throw -1;
	}
	cout << "SkipEv::" << skipEvent << endl;
   	if (nEvents < 0)
    	nEvents = fTree->GetEntries();
   
    if ( skipEvent >= fTree->GetEntries() )	
    	Error("GlobalRecoMC::LoopEvent", "GlobalRecoMC::LoopEvent::skip event grater then event in file"), exit(0);

   	if ( (nEvents + skipEvent) > fTree->GetEntries())
    	nEvents = fTree->GetEntries() - skipEvent;
    
	Int_t frequency = 1;
	if (nEvents > 100000)      frequency = 100000;
	else if (nEvents > 10000)  frequency = 10000;
	else if (nEvents > 1000)   frequency = 1000;
	else if (nEvents > 100)    frequency = 100;
	else if (nEvents > 10)     frequency = 10;
   
   for (Long64_t ientry = skipEvent; ientry < skipEvent + nEvents; ientry++) {
      fTree->GetEntry(ientry);
      
      if(ientry % frequency == 0)
         cout<<" Loaded Event:: " << ientry << endl;
      
      if (!fTAGroot->NextEvent(ientry)) break;

   }
}

//______________________________________________________________
//! After loop
void GlobalRecoMC::AfterEventLoop()
{
  LocalRecoMC::AfterEventLoop();
}

//______________________________________________________________
//! Open input file
void GlobalRecoMC::OpenFileIn()
{
	LocalRecoMC::OpenFileIn();
	fTree = ((TAGactTreeReader*)fActEvtReader)->GetTree();
}

