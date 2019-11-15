

#include "GlobalRecoMC.hxx"

ClassImp(GlobalRecoMC)

//__________________________________________________________
GlobalRecoMC::GlobalRecoMC(TString expName, TString fileNameIn, TString fileNameout)
 : LocalRecoMC(expName, fileNameIn, fileNameout)
{
}

//__________________________________________________________
GlobalRecoMC::~GlobalRecoMC()
{
}

//__________________________________________________________
void GlobalRecoMC::BeforeEventLoop()
{
  LocalRecoMC::BeforeEventLoop();
  std::cout << "AFTER BEFOREEVENTLOOP" << std::endl;

  TADIparGeo* fDipole = GetParGeoDi();
  //if (!fDipole) std::cout << "WARNING NO MAG FIELD LOADED" << std::endl;
  TString magFieldMapName = fDipole->GetMapName();

  genfit::FieldManager::getInstance()->init( new FootField(magFieldMapName.Data(), fDipole ) );

  // set material and geometry into genfit
  MaterialEffects* materialEffects = MaterialEffects::getInstance();
  materialEffects->init(new TGeoMaterialInterface());

  // include the nucleon into the genfit pdg repository
  if ( GlobalPar::GetPar()->IncludeBM() || GlobalPar::GetPar()->IncludeKalman() )
    UpdatePDG::Instance();
  
  // Initialisation of KFfitter
  if ( GlobalPar::GetPar()->Debug() > 1 )       cout << "KFitter init!" << endl;
  m_kFitter = new KFitter();
  if ( GlobalPar::GetPar()->Debug() > 1 )       cout << "KFitter init done!" << endl;
  


}

//____________________________________________________________
void GlobalRecoMC::LoopEvent(Int_t nEvents)
{
   if (nEvents == 0)
      nEvents = fTree->GetEntries();
   
   if (nEvents > fTree->GetEntries())
      nEvents = fTree->GetEntries();
   
   for (Long64_t ientry = 0; ientry < nEvents; ientry++) {
      
      fTree->GetEntry(ientry);
      
      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;
      
      if (!fTAGroot->NextEvent()) break;

      m_kFitter->MakeFit(ientry);

   }
}

//______________________________________________________________
void GlobalRecoMC::AfterEventLoop()
{
  LocalRecoMC::AfterEventLoop();
  m_kFitter->Finalize();
}
