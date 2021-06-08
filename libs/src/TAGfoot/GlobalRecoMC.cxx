

#include "GlobalRecoMC.hxx"

ClassImp(GlobalRecoMC)

//__________________________________________________________
GlobalRecoMC::GlobalRecoMC(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : LocalRecoMC(expName, runNumber, fileNameIn, fileNameout), fTree(0x0)
{

	EnableTracking();
	fFlagMC = true;
	TAGrecoManager::GetPar()->SetIsMC( fFlagMC );		// to be changed

}

//__________________________________________________________
GlobalRecoMC::~GlobalRecoMC()
{
}

//__________________________________________________________
void GlobalRecoMC::BeforeEventLoop()
{
	LocalRecoMC::BeforeEventLoop();

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

//____________________________________________________________
void GlobalRecoMC::LoopEvent(Int_t nEvents, Int_t skipEvent)
{
	cout << "SkipEv2::" << skipEvent << endl;
   if (nEvents <= 0)
      nEvents = fTree->GetEntries();
   
   if ( (nEvents + skipEvent) > fTree->GetEntries())
      nEvents = fTree->GetEntries();

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
void GlobalRecoMC::AfterEventLoop()
{
  LocalRecoMC::AfterEventLoop();
}

void GlobalRecoMC::OpenFileIn()
{
	LocalRecoMC::OpenFileIn();
	fTree = ((TAGactTreeReader*)fActEvtReader)->GetTree();
}























