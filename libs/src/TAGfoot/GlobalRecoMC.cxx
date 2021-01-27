

#include "GlobalRecoMC.hxx"

ClassImp(GlobalRecoMC)

//__________________________________________________________
GlobalRecoMC::GlobalRecoMC(TString expName, TString fileNameIn, TString fileNameout)
 : LocalRecoMC(expName, fileNameIn, fileNameout)
{

	EnableTracking();
  EnableItrTracking();

}

//__________________________________________________________
GlobalRecoMC::~GlobalRecoMC()
{
  delete m_kFitter;
}

//__________________________________________________________
void GlobalRecoMC::BeforeEventLoop()
{
	LocalRecoMC::BeforeEventLoop();

	TADIparGeo* fDipole = GetParGeoDi();
	//if (!fDipole) std::cout << "WARNING NO MAG FIELD LOADED" << std::endl;

  TADIgeoField* footMagField = new TADIgeoField(fDipole);
  TADIgenField* genfitMagField = new TADIgenField(footMagField);
  genfit::FieldManager::getInstance()->init(genfitMagField);

	// set material and geometry into genfit
	MaterialEffects* materialEffects = MaterialEffects::getInstance();
	materialEffects->init(new TGeoMaterialInterface());

	// include the nucleon into the genfit pdg repository
	if ( GlobalPar::GetPar()->IncludeBM() || GlobalPar::GetPar()->IncludeKalman() )
	UpdatePDG::Instance();

	// study for kalman Filter
	//m_globalTrackingStudies = new GlobalTrackingStudies();


	// Initialisation of KFfitter
	if ( GlobalPar::GetPar()->Debug() > 1 )       cout << "KFitter init!" << endl;
	m_kFitter = new KFitter();
	if ( GlobalPar::GetPar()->Debug() > 1 )       cout << "KFitter init done!" << endl;


}

//____________________________________________________________
void GlobalRecoMC::LoopEvent(Int_t nEvents)
{
   if (nEvents <= 0)
      nEvents = fTree->GetEntries();

   if (nEvents > fTree->GetEntries())
      nEvents = fTree->GetEntries();

   for (Long64_t ientry = 0; ientry < nEvents; ientry++) {

      fTree->GetEntry(ientry);

      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;

      if (!fTAGroot->NextEvent()) break;

      //m_globalTrackingStudies->Execute();
      m_kFitter->MakeFit(ientry);

   }
}

//______________________________________________________________
void GlobalRecoMC::AfterEventLoop()
{

  //m_globalTrackingStudies->Finalize();
  m_kFitter->Finalize();
  LocalRecoMC::AfterEventLoop();
}
