

/*!
 \file GlobalAna.cxx
 \brief  Base class for reconstruction
 */
/*------------------------------------------+---------------------------------*/

#include "GlobalAna.hxx"

#include "TAGroot.hxx"

#include "TAGactTreeReader.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGparGeo.hxx"

#include "TAGnameManager.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class GlobalAna
 \brief  Base class for reconstruction
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
ClassImp(GlobalAna)


//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
GlobalAna::GlobalAna(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC)
 : TNamed(fileNameIn.Data(), fileNameout.Data()),
   fExpName(expName),
   fRunNumber(runNumber),
   fpTree(0x0),
   fpParMapIt(0x0),
   fpParGeoSt(0x0),
   fpParGeoG(0x0),
   fpParGeoDi(0x0),
   fpParGeoBm(0x0),
   fpParGeoVtx(0x0),
   fpParGeoIt(0x0),
   fpParGeoMsd(0x0),
   fpParGeoCa(0x0),
   fpParGeoTw(0x0),
   fpParConfSt(0x0),
   fpParConfBm(0x0),
   fpParConfVtx(0x0),
   fpParConfIt(0x0),
   fpParConfMsd(0x0),
   fpParConfTw(0x0),
   fpParConfCa(0x0),
   fField(0x0),
   fpNtuHitSt(0x0),
   fpNtuClusVtx(0x0),
   fpNtuClusIt(0x0),
   fpNtuClusMsd(0x0),
   fpNtuRecMsd(0x0),
   fpNtuRecTw(0x0),
   fpNtuMcTrk(0x0),
   fpNtuTrackBm(0x0),
   fpNtuTrackVtx(0x0),
   fpNtuTrackIt(0x0),
   fpNtuVtx(0x0),
   fpNtuGlbTrack(0x0),
   fActGlbAna(0x0),
   fFlagHisto(false),
   fFlagMC(isMC),
   fSkipEventsN(0)
{

   // check folder
   if (!fExpName.IsNull())
      fExpName += "/";

   if (fileNameout == "")
      fFlagOut = false;

   // define TAGroot
   fTAGroot = new TAGroot();

   // global transformation
   fpFootGeo = new TAGgeoTrafo();
   
   // Save run info
   gTAGroot->SetRunNumber(fRunNumber);
   gTAGroot->SetCampaignName(fExpName);

   // load campaign file
   fCampManager = new TAGcampaignManager(expName);
   fCampManager->FromFile();
   
   fAnaManager = new TAGanaManager(expName);
   fAnaManager->FromFile();
   
   if (fFlagOut) {
      const Char_t* name = FootActionDscName("TAGactTreeWriter");
      fActEvtWriter = new TAGactTreeWriter(name);
      fpTree = fActEvtWriter->Tree();
   }
}

//__________________________________________________________
//! default destructor
GlobalAna::~GlobalAna()
{
   // default destructor
   delete fTAGroot; // should delete all data, para and actions
}

//__________________________________________________________
//! Actions before loop event
void GlobalAna::BeforeEventLoop()
{
   ReadParFiles();
   SetTreeBranches();
   
   CreateAnaAction();
   AddRequiredItem();

   OpenFileIn();

   if (fFlagOut)
      OpenFileOut();

   fTAGroot->BeginEventLoop();
   fTAGroot->Print();
}

//__________________________________________________________
//! Loop over events
//!
//! \param[in] nEvents number of events to process
void GlobalAna::LoopEvent(Int_t nEvents)
{
  Int_t frequency = 1;

  if (nEvents >= 100000)      frequency = 10000;
  else if (nEvents >= 10000)  frequency = 1000;
  else if (nEvents >= 1000)   frequency = 100;
  else if (nEvents >= 100)    frequency = 10;
  else if (nEvents >= 10)     frequency = 1;


  if (fSkipEventsN > 0)
     printf(" Skipped Event: %d\n", fSkipEventsN);

  for (Int_t ientry = 0; ientry < nEvents; ientry++) {

    if(ientry % frequency == 0)
      printf(" Loaded Event: %d\n", ientry+fSkipEventsN);

    if (!fTAGroot->NextEvent()) break;

  }
}

//__________________________________________________________
//! Actions after loop event
void GlobalAna::AfterEventLoop()
{
   fTAGroot->EndEventLoop();
    
   if (fFlagOut)
      CloseFileOut();
   
   CloseFileIn();
}

//__________________________________________________________
//! Open input file
void GlobalAna::OpenFileIn()
{
   fActEvtReader->Open(GetName());
}

//__________________________________________________________
//! Close input file
void GlobalAna::CloseFileIn()
{
   fActEvtReader->Close();
}

//__________________________________________________________
//!  Open output file
void GlobalAna::OpenFileOut()
{
   TString name = GetTitle();
      
   Info("OpenFileOut()", "Output file name %s\n", name.Data());

   fActEvtWriter->Open(name.Data(), "RECREATE");
   
   if (fFlagHisto)
      SetHistogramDir();
}

//__________________________________________________________
//! Close output file
void GlobalAna::CloseFileOut()
{
   //add crossing map if enabled in input mc files
   fActEvtWriter->Print();
   fActEvtWriter->Close();
}

//__________________________________________________________
//! Set reconstructed histogram directory
void GlobalAna::SetHistogramDir()
{
   TList* list = gTAGroot->ListOfAction();
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TAGaction* action = (TAGaction*)list->At(i);
      TString tmp(action->GetName());
      
      TDirectory* subfolder;
      
      if (tmp.Contains("Mass")) {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get("Mass");
         if (!subfolder)
            subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir("Mass");
         action->SetHistogramDir(subfolder);
         
      } else {
      }
   }
   
}

//__________________________________________________________
//! Read parameters files
void GlobalAna::ReadParFiles()
{
   Int_t Z_beam = 0;
   Int_t A_beam = 0;
   TString ion_name;
   Float_t kinE_beam = 0.;

   // Read Trafo file
   TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TAGgeoTrafo"), fRunNumber);
   fpFootGeo->FromFile(parFileName);

 
   // initialise par files for target
   if (TAGrecoManager::GetPar()->IncludeTG() || TAGrecoManager::GetPar()->IncludeBM() || TAGrecoManager::GetPar()->IncludeTW() || TAGrecoManager::GetPar()->IncludeCA()) {
      fpParGeoG = new TAGparaDsc(new TAGparGeo());
      TAGparGeo* parGeo = (TAGparGeo*)fpParGeoG->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TAGparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      Z_beam = parGeo->GetBeamPar().AtomicNumber;
      A_beam = parGeo->GetBeamPar().AtomicMass;
      ion_name = parGeo->GetBeamPar().Material;
      kinE_beam = parGeo->GetBeamPar().Energy; //GeV/u
      
      parGeo->Print();
   }
   
   // initialise par files for start counter
   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeTW() || TAGrecoManager::GetPar()->IncludeCA()) {
      fpParGeoSt = new TAGparaDsc(new TASTparGeo());
      TASTparGeo* parGeo = (TASTparGeo*)fpParGeoSt->Object();
      TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TASTparGeo"), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      fpParConfSt = new TAGparaDsc(new TASTparConf());
      TASTparConf* parConf = (TASTparConf*)fpParConfSt->Object();
      parFileName = fCampManager->GetCurConfFile(FootBaseName("TASTparGeo"), fRunNumber);
      parConf->FromFile(parFileName.Data());
   }
   
   // initialise par files for Beam Monitor
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fpParGeoBm = new TAGparaDsc(new TABMparGeo());
      TABMparGeo* parGeo = (TABMparGeo*)fpParGeoBm->Object();
      TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TABMparGeo"), fRunNumber);
      parGeo->FromFile(parFileName.Data());

      fpParConfBm = new TAGparaDsc(new TABMparConf());
      TABMparConf* parConf = (TABMparConf*)fpParConfBm->Object();
      parFileName = fCampManager->GetCurConfFile(FootBaseName("TABMparGeo"), fRunNumber);
      parConf->FromFile(parFileName.Data());
   }
   
   // initialise par files dipole
   if (TAGrecoManager::GetPar()->IncludeDI() ) {
      fpParGeoDi = new TAGparaDsc(new TADIparGeo());
      TADIparGeo* parGeo = (TADIparGeo*)fpParGeoDi->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TADIparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());
   }
   
   // initialise par files for vertex
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fpParGeoVtx = new TAGparaDsc(new TAVTparGeo());
      TAVTparGeo* parGeo = (TAVTparGeo*)fpParGeoVtx->Object();
      TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TAVTparGeo"), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      fpParConfVtx = new TAGparaDsc(new TAVTparConf());
      TAVTparConf* parConf = (TAVTparConf*)fpParConfVtx->Object();
      parFileName = fCampManager->GetCurConfFile(FootBaseName("TAVTparGeo"), fRunNumber);
      parConf->FromFile(parFileName.Data());
   }
   
   // initialise par files for inner tracker
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      TAITparMap* parMap = 0x0;
      if(!fFlagMC) {
         fpParMapIt = new TAGparaDsc(new TAITparMap());
         parMap = (TAITparMap*)fpParMapIt->Object();
         parFileName = fCampManager->GetCurMapFile(FootBaseName("TAITparMap"), fRunNumber);
         parMap->FromFile(parFileName.Data());
      }
      
      fpParGeoIt = new TAGparaDsc(new TAITparGeo(parMap));
      TAITparGeo* parGeo = (TAITparGeo*)fpParGeoIt->Object();
      TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TAITparGeo"), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      fpParConfIt = new TAGparaDsc(new TAITparConf());
      TAITparConf* parConf = (TAITparConf*)fpParConfIt->Object();
      parFileName = fCampManager->GetCurConfFile(FootBaseName("TAITparGeo"), fRunNumber);
      parConf->FromFile(parFileName.Data());
   }
   
   // initialise par files for multi strip detector
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fpParGeoMsd = new TAGparaDsc(new TAMSDparGeo());
      TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
      TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TAMSDparGeo"), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      
      fpParConfMsd = new TAGparaDsc(new TAMSDparConf());
      TAMSDparConf* parConf = (TAMSDparConf*)fpParConfMsd->Object();
      parFileName = fCampManager->GetCurConfFile(FootBaseName("TAMSDparGeo"), fRunNumber);
      parConf->FromFile(parFileName.Data());
   }
   
   // initialise par files for Tof Wall
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fpParGeoTw = new TAGparaDsc(new TATWparGeo());
      TATWparGeo* parGeo = (TATWparGeo*)fpParGeoTw->Object();
      TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TATWparGeo"), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      fpParConfTw = new TAGparaDsc(new TATWparConf());
      TATWparConf* parConf = (TATWparConf*)fpParConfTw->Object();
      parFileName = fCampManager->GetCurConfFile(FootBaseName("TATWparGeo"), fRunNumber);
      parConf->FromFile(parFileName.Data());
   }
   
   // initialise par files for caloriomter
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fpParGeoCa = new TAGparaDsc(new TACAparGeo());
      TACAparGeo* parGeo = (TACAparGeo*)fpParGeoCa->Object();
      TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TACAparGeo"), fRunNumber);
      parGeo->FromFile(parFileName);
      
      fpParConfCa = new TAGparaDsc(new TACAparConf());
      TACAparConf* parConf = (TACAparConf*)fpParConfCa->Object();
      parFileName = fCampManager->GetCurConfFile(FootBaseName("TACAparGeo"), fRunNumber);
      parConf->FromFile(parFileName.Data());
   }
}

//__________________________________________________________
//! Create reconstruction actions
void GlobalAna::CreateAnaAction()
{
   // place here your beloved analysis class
   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman())) {
      if (fAnaManager->GetAnalysisPar().MassResoFlag)
         fActGlbAna = new TANAactNtuMass("anaActMass", fpNtuGlbTrack, fpParGeoG, fpTree);
   }
}

//__________________________________________________________
//! Add required reconstruction actions in list
void GlobalAna::AddRequiredItem()
{
   // Add the required analysis class
   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman())) {
      if (fAnaManager->GetAnalysisPar().MassResoFlag)
         gTAGroot->AddRequiredItem("anaActMass");
   }
}

//__________________________________________________________
//! Go to a given event
//!
//! \param[in] iEvent event number to go
Bool_t GlobalAna::GoEvent(Int_t iEvent)
{
   // only possible for MC data
   if (iEvent < fActEvtReader->NEvents()) {
      fSkipEventsN = iEvent;
      fActEvtReader->Reset(iEvent);
      return true;
   }
   
   return false;
}

//__________________________________________________________
//! Set L0 tree branches for reading back
void GlobalAna::SetTreeBranches()
{
   const Char_t* name = FootActionDscName("TAGactTreeReader");
   fActEvtReader = new TAGactTreeReader(name);
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fpNtuHitSt   = new TAGdataDsc(new TASTntuHit());
      fActEvtReader->SetupBranch(fpNtuHitSt);
      if (fFlagMC) {
         fpNtuMcSt   = new TAGdataDsc(FootDataDscMcName(kST), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcSt, FootBranchMcName(kST));
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fpNtuTrackBm = new TAGdataDsc(new TABMntuTrack());
      fActEvtReader->SetupBranch(fpNtuTrackBm);
      if (fFlagMC) {
         fpNtuMcBm   = new TAGdataDsc(FootDataDscMcName(kBM), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcBm, FootBranchMcName(kBM));
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fpNtuTrackVtx = new TAGdataDsc(new TAVTntuTrack());
      fActEvtReader->SetupBranch(fpNtuTrackVtx);
      
      TAVTparGeo* parGeo = (TAVTparGeo*)fpParGeoVtx->Object();
      Int_t sensorsN = parGeo->GetSensorsN();
      fpNtuClusVtx  = new TAGdataDsc(new TAVTntuCluster(sensorsN));
      fActEvtReader->SetupBranch(fpNtuClusVtx);
      
      fpNtuVtx = new TAGdataDsc(new TAVTntuVertex());
      fActEvtReader->SetupBranch(fpNtuVtx);
      
      if (fFlagMC) {
         fpNtuMcVt   = new TAGdataDsc(FootDataDscMcName(kVTX), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcVt, FootBranchMcName(kVTX));
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      TAITparGeo* parGeo = (TAITparGeo*)fpParGeoIt->Object();
      Int_t sensorsN = parGeo->GetSensorsN();
      fpNtuClusIt = new TAGdataDsc(new TAITntuCluster(sensorsN));
      fActEvtReader->SetupBranch(fpNtuClusIt);
      if ( fFlagMC) {
         fpNtuMcIt   = new TAGdataDsc(FootDataDscMcName(kITR), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcIt, FootBranchMcName(kITR));
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      
      TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
      Int_t sensorsN = parGeo->GetSensorsN();
      fpNtuClusMsd = new TAGdataDsc(new TAMSDntuCluster(sensorsN));
      fActEvtReader->SetupBranch(fpNtuClusMsd);
      
      Int_t stationsN = parGeo->GetStationsN();
      fpNtuRecMsd = new TAGdataDsc(new TAMSDntuPoint(stationsN));
      fActEvtReader->SetupBranch(fpNtuRecMsd);
      
      if (fFlagMC) {
         fpNtuMcMsd   = new TAGdataDsc(FootDataDscMcName(kMSD), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcMsd, FootBranchMcName(kMSD));
      }
   }
   
   if(TAGrecoManager::GetPar()->IncludeTW()) {
      fpNtuRecTw = new TAGdataDsc(new TATWntuPoint());
      fActEvtReader->SetupBranch(fpNtuRecTw);
      if (fFlagMC) {
         fpNtuMcTw   = new TAGdataDsc(FootDataDscMcName(kTW), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcTw, FootBranchMcName(kTW));
      }
   }
   
   if(TAGrecoManager::GetPar()->IncludeCA()) {
      fpNtuClusCa = new TAGdataDsc(new TACAntuCluster());
      fActEvtReader->SetupBranch(fpNtuClusCa);
      if (fFlagMC) {
         fpNtuMcCa   = new TAGdataDsc(FootDataDscMcName(kCAL), new TAMCntuHit());
         fActEvtReader->SetupBranch(fpNtuMcCa, FootBranchMcName(kCAL));
      }
   }
   
   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman())) {
      fpNtuGlbTrack = new TAGdataDsc(new TAGntuGlbTrack());
      fActEvtReader->SetupBranch(fpNtuGlbTrack);
   }
}
