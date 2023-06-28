

/*!
 \file BaseReco.cxx
 \brief  Base class for reconstruction
 */
/*------------------------------------------+---------------------------------*/

#include "BaseReco.hxx"

#include "TAGroot.hxx"

#include "TAGactTreeReader.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGparGeo.hxx"

#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDntuPoint.hxx"

#include "TAMSDntuTrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAVTactNtuHit.hxx"
#include "TAVTactNtuVertexPD.hxx"

#include "TAGnameManager.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class BaseReco
 \brief  Base class for reconstruction
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
ClassImp(BaseReco)

Bool_t  BaseReco::fgSaveMcFlag = true;

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
BaseReco::BaseReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC)
 : TNamed(fileNameIn.Data(), fileNameout.Data()),
   fExpName(expName),
   fRunNumber(runNumber),
   fRateInitRun(-1),
   fRateEndRun(-1),
   fFriendFileName(""),
   fFriendTreeName(""),
   fpParTimeWD(0x0),
   fpParMapWD(0x0),
   fpParMapSt(0x0),
   fpParMapBm(0x0),
   fpParMapVtx(0x0),
   fpParMapIt(0x0),
   fpParMapMsd(0x0),
   fpParMapTw(0x0),
   fpParMapCa(0x0),
   fpParGeoSt(0x0),
   fpParGeoG(0x0),
   fpParGeoDi(0x0),
   fpParGeoBm(0x0),
   fpParGeoVtx(0x0),
   fpParGeoIt(0x0),
   fpParGeoMsd(0x0),
   fpParGeoTw(0x0),
   fField(0x0),
   fpParCalBm(0x0),
   fpParCalMsd(0x0),
   fpParCalTw(0x0),
   fpParCalCa(0x0),
   fpParGeoCa(0x0),
   fpParConfBm(0x0),
   fpParConfVtx(0x0),
   fpParConfIt(0x0),
   fpParConfMsd(0x0),
   fpParConfTw(0x0),
   fpParConfCa(0x0),
   fpDatRawSt(0x0),
   fpDatRawCa(0x0),
   fpNtuHitSt(0x0),
   fpDatRawBm(0x0),
   fpNtuHitBm(0x0),
   fpNtuHitVtx(0x0),
   fpNtuHitIt(0x0),
   fpDatRawMsd(0x0),
   fpNtuHitMsd(0x0),
   fpDatRawTw(0x0),
   fpNtuHitTw(0x0),
   fpNtuHitCa(0x0),
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
   fActEvtReader(0x0),
   fActEvtWriter(0x0),
   fActTrackBm(0x0),
   fActClusVtx(0x0),
   fActTrackVtx(0x0),
   fActVtx(0x0),
   fActClusIt(0x0),
   fActTrackIt(0x0),
   fActClusMsd(0x0),
   fActPointTw(0x0),
#ifdef TOE_FLAG
   fActGlbTrack(0x0),
#endif
#ifdef GENFIT_FLAG
   fActGlbkFitter(0x0),
#endif
   fActGlbTrackS(0x0),
   fFlagOut(true),
   fFlagFlatTree(false),
   fFlagTree(false),
   fFlagHits(false),
   fFlagHisto(false),
   fFlagTrack(false),
   fFlagTWbarCalib(false),
   fFlagMsdTrack(false),
   fFlagItrTrack(false),
   fFlagRateSmearTw(false),
   fVtxTrackingAlgo("Full"),
   fItrTrackingAlgo("Full"),
   fMsdTrackingAlgo("Full"),
   fCalClusterAlgo("Padme"),
   fFlagMC(isMC),
   fReadL0Hits(false),
   fM28ClusMtFlag(false),
   fFlagRecCutter(false),
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

   // load campaign file
   fCampManager = new TAGcampaignManager(expName);
   fCampManager->FromFile();

   // load run file
   fRunManager = new TAGrunManager(expName);
   
   // Save run info
   gTAGroot->SetRunNumber(fRunNumber);
   gTAGroot->SetCampaignName(fExpName);

   // activate per default only TW if TOE on
   if (TAGrecoManager::GetPar()->IncludeTOE())
      TAGrecoManager::GetPar()->IncludeTW(true);

   fFlagFlatTree = TAGrecoManager::GetPar()->IsSaveFlatTree();

   if (fFlagOut) {
      if (fFlagFlatTree) {
         const Char_t* name = FootActionDscName("TAGactFlatTreeWriter");
         fActEvtWriter = new TAGactFlatTreeWriter(name, fFlagMC);
      } else {
         const Char_t* name = FootActionDscName("TAGactDscTreeWriter");
         fActEvtWriter = new TAGactDscTreeWriter(name, fFlagMC);
      }
   }
}

//__________________________________________________________
//! default destructor
BaseReco::~BaseReco()
{
   // default destructor
   delete fTAGroot; // should delete all data, para and actions
}

//_____________________________________________________________________________
//! Campaign information checks
void BaseReco::CampaignChecks()
{
   // campaign checks
   if (!fCampManager->CampaignChecks(fRunNumber, fFlagMC))
      exit(0);
   
   // Run Condition Checks
   if (fpParGeoG) {
      TAGparGeo* parGeo = (TAGparGeo*)fpParGeoG->Object();
      if (!fRunManager->ConditionChecks(fRunNumber, parGeo))
         exit(0);
   }
}

//__________________________________________________________
//! Global reconstruction information checks
void BaseReco::GlobalChecks()
{
   if (!TAGrecoManager::GetPar()->GlobalChecks(fFlagMC))
      exit(0);
}

//__________________________________________________________
//! Global reconstruction settings
void BaseReco::GlobalSettings()
{
   Bool_t ntu    = TAGrecoManager::GetPar()->IsSaveTree();
   Bool_t his    = TAGrecoManager::GetPar()->IsSaveHisto();
   Bool_t hit    = TAGrecoManager::GetPar()->IsSaveHits();
   Bool_t trk    = TAGrecoManager::GetPar()->IsTracking();
   Bool_t obj    = TAGrecoManager::GetPar()->IsReadRootObj();

   // global setting
   if (ntu)
      EnableTree();
   
   if(his)
      EnableHisto();
   
   if(hit) {
      EnableTree();
      EnableSaveHits();
   }
   
   if (trk)
      EnableTracking();
}

//__________________________________________________________
//! Add friend tree in root file
//!
//! \param[in] fileName root file name
//! \param[in] treeName tree name
void BaseReco::AddFriendTree(TString fileName, TString treeName)
{
   fFriendFileName = fileName;
   fFriendTreeName = treeName;
}

//__________________________________________________________
//! Actions before loop event
void BaseReco::BeforeEventLoop()
{
   GlobalSettings();
   
   ReadParFiles();
   CreateRawAction();
   CreateRecAction();

   CampaignChecks();

   AddRequiredItem();

   OpenFileIn();

   GlobalChecks();

   if (fFlagOut)
      OpenFileOut();

   fTAGroot->BeginEventLoop();
   fTAGroot->Print();
}

//__________________________________________________________
//! Loop over events
//!
//! \param[in] nEvents number of events to process
void BaseReco::LoopEvent(Int_t nEvents)
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
void BaseReco::AfterEventLoop()
{

  fActTrackVtx->Finalize();
#ifdef GENFIT_FLAG
      if (TAGrecoManager::GetPar()->IncludeKalman()) fActGlbkFitter->Finalize();
#endif

   fTAGroot->EndEventLoop();
    
   if (fFlagOut)
      CloseFileOut();
   CloseFileIn();
}

//__________________________________________________________
//!  Open output file
void BaseReco::OpenFileOut()
{
   fActEvtWriter->Open(GetTitle(), "RECREATE");
   
   if (fFlagHisto)
      SetHistogramDir();
}

//__________________________________________________________
//! Set reconstructed histogram directory
void BaseReco::SetHistogramDir()
{
   //Global track
   if(fFlagRecCutter) return;
   
   TList* list = gTAGroot->ListOfAction();
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TAGaction* action = (TAGaction*)list->At(i);
      TString tmp(action->GetName());
      TString prefix(tmp(0,2));
      
      TDirectory* subfolder;
      
      if (prefix == "st") {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(FootBaseName("TASTparGeo"));
         if (!subfolder)
            subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(FootBaseName("TASTparGeo"));
         action->SetHistogramDir(subfolder);
         
      } else if (prefix == "bm") {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(FootBaseName("TABMparGeo"));
         if (!subfolder)
            subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(FootBaseName("TABMparGeo"));
         action->SetHistogramDir(subfolder);
         
      } else if (prefix == "vt") {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(FootBaseName("TAVTparGeo"));
         if (!subfolder)
            subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(FootBaseName("TAVTparGeo"));
         action->SetHistogramDir(subfolder);
         
      } else if (prefix == "it") {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(FootBaseName("TAITparGeo"));
         if (!subfolder)
            subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(FootBaseName("TAITparGeo"));
         action->SetHistogramDir(subfolder);
         
      } else if (prefix == "ms") {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(FootBaseName("TAMSDparGeo"));
         if (!subfolder)
            subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(FootBaseName("TAMSDparGeo"));
         action->SetHistogramDir(subfolder);
         
      } else if (prefix == "tw") {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(FootBaseName("TATWparGeo"));
         if (!subfolder)
            subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(FootBaseName("TATWparGeo"));
         action->SetHistogramDir(subfolder);
         
      } else if (prefix == "ca") {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(FootBaseName("TACAparGeo"));
         if (!subfolder)
            subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(FootBaseName("TACAparGeo"));
         action->SetHistogramDir(subfolder);
         
      } else if (prefix == "wd") {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(FootBaseName("TAWD"));
         if (!subfolder)
            subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(FootBaseName("TAWD"));
         action->SetHistogramDir(subfolder);
         
      } else if (prefix == "ev") {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(FootBaseName("TAGevent"));
         if (!subfolder)
            subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(FootBaseName("TAGevent"));
         action->SetHistogramDir(subfolder);
         
      } else {
      }
   }
   
   if (fFlagTrack) {
      
#ifdef GENFIT_FLAG
      if (!TAGrecoManager::GetPar()->IncludeTOE() && TAGrecoManager::GetPar()->IncludeKalman()) {
         TDirectory* subfolder = fActEvtWriter->File()->mkdir(FootBaseName("TAGgeoTrafo"));
         fActGlbkFitter->SetHistogramDir(subfolder);
         if (TAGrecoManager::GetPar()->IsFromLocalReco()) return;
      }
#endif
#ifdef TOE_FLAG
      if (TAGrecoManager::GetPar()->IncludeTOE() && !TAGrecoManager::GetPar()->IncludeKalman()) {
         TDirectory* subfolder = fActEvtWriter->File()->mkdir(FootBaseName("TAGgeoTrafo"));
         fActGlbTrack->SetHistogramDir(subfolder);
         if (TAGrecoManager::GetPar()->IsFromLocalReco()) return;
      }
#endif
      
      // Global straight track
      if (TAGrecoManager::GetPar()->IncludeStraight() && !TAGrecoManager::GetPar()->IncludeDI()) {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(FootBaseName("TAGgeoTrafo"));
         fActGlbTrackS->SetHistogramDir(subfolder);
      }
   }
}

//__________________________________________________________
//! Close output file
void BaseReco::CloseFileOut()
{
   // saving current run info
   TAGrecoManager::GetPar()->EnableFromLocalReco();
   TAGrunInfo info = TAGrecoManager::GetPar()->GetGlobalInfo();
   info.SetCampaignName(fExpName);
   info.SetRunNumber(fRunNumber);

   //add crossing map if enabled in input mc files
   if(fFlagMC){
     TAGrunInfo inputinfo = gTAGroot->CurrentRunInfo();
     if(inputinfo.GetGlobalPar().EnableRegionMc==true && info.GetGlobalPar().EnableRegionMc==true)
       info.ImportCrossMap(inputinfo);
   }
   gTAGroot->SetRunInfo(info);
   fActEvtWriter->Print();
   fActEvtWriter->Close();
}

//__________________________________________________________
//! Read parameters files
void BaseReco::ReadParFiles()
{
   Int_t Z_beam = 0;
   Int_t A_beam = 0;
   TString ion_name;
   Float_t kinE_beam = 0.;

   // Read Trafo file
   TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TAGgeoTrafo"), fRunNumber);
   fpFootGeo->FromFile(parFileName);

   // initialise par files for target
   if (TAGrecoManager::GetPar()->IncludeTG() || TAGrecoManager::GetPar()->IncludeBM() || TAGrecoManager::GetPar()->IncludeTW() || TAGrecoManager::GetPar()->IncludeCA() || fFlagItrTrack) {
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

      if(!fFlagMC) {
         fpParMapSt = new TAGparaDsc(new TASTparMap());
         TASTparMap* parMapSt = (TASTparMap*) fpParMapSt->Object();
         parFileName = fCampManager->GetCurConfFile(FootBaseName("TASTparGeo"), fRunNumber);
         parMapSt->FromFile(parFileName);

         fpParMapTw = new TAGparaDsc(new TATWparMap());
         TATWparMap* parMap = (TATWparMap*)fpParMapTw->Object();
         parFileName = fCampManager->GetCurMapFile(FootBaseName("TATWparGeo"), fRunNumber);
         parMap->FromFile(parFileName.Data());

         fpParMapWD = new TAGparaDsc(new TAWDparMap());
         TAWDparMap* parMapWD = (TAWDparMap*)fpParMapWD->Object();
         parFileName = fCampManager->GetCurMapFile(FootBaseName("TASTparGeo"), fRunNumber);
         parMapWD->FromFile(parFileName.Data());

         fpParTimeWD = new TAGparaDsc(new TAWDparTime());
         TAWDparTime* parTimeWD = (TAWDparTime*) fpParTimeWD->Object();
         TString parFileName = fCampManager->GetCurCalFile(FootBaseName("TASTparGeo"), fRunNumber);
         parTimeWD->FromFileCFD(parFileName.Data());
      }
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

      fpParCalBm = new TAGparaDsc(new TABMparCal());
      TABMparCal* parCal = (TABMparCal*)fpParCalBm->Object();
      parFileName = fCampManager->GetCurCalFile(FootBaseName("TABMparGeo"), fRunNumber);
      parCal->FromFile(parFileName.Data());

      if(!fFlagMC){
        fpParMapBm = new TAGparaDsc(new TABMparMap());
        TABMparMap*  parMapBm = (TABMparMap*)fpParMapBm->Object();
        parFileName = fCampManager->GetCurMapFile(FootBaseName("TABMparGeo"), fRunNumber);
        parMapBm->FromFile(parFileName.Data(), parGeo);
      }
   }

   // initialise par files dipole
   if (TAGrecoManager::GetPar()->IncludeDI() ) {
      fpParGeoDi = new TAGparaDsc(new TADIparGeo());
      TADIparGeo* parGeo = (TADIparGeo*)fpParGeoDi->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TADIparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());

      if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) {
         TAGparaDsc* fieldDsc = new TAGparaDsc(new TADIgeoField(parGeo));
         fField = (TADIgeoField*)fieldDsc->Object();
      }
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
    
      fVtxTrackingAlgo = parConf->GetAnalysisPar().TrackingAlgo;
      
      if(!fFlagMC) {
         fpParMapVtx = new TAGparaDsc(new TAVTparMap());
         TAVTparMap* parMap = (TAVTparMap*)fpParMapVtx->Object();
         parFileName = fCampManager->GetCurMapFile(FootBaseName("TAVTparGeo"), fRunNumber);
         parMap->FromFile(parFileName.Data());
      }
   }

   // initialise par files for inner tracker
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fpParGeoIt = new TAGparaDsc(new TAITparGeo());
      TAITparGeo* parGeo = (TAITparGeo*)fpParGeoIt->Object();
      TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TAITparGeo"), fRunNumber);
      parGeo->FromFile(parFileName.Data());

      fpParConfIt = new TAGparaDsc(new TAITparConf());
      TAITparConf* parConf = (TAITparConf*)fpParConfIt->Object();
      parFileName = fCampManager->GetCurConfFile(FootBaseName("TAITparGeo"), fRunNumber);
      parConf->FromFile(parFileName.Data());

      if ((fFlagItrTrack = parConf->GetAnalysisPar().TrackingFlag)) {
         fVtxTrackingAlgo = parConf->GetAnalysisPar().TrackingAlgo;
      }
      
      if(!fFlagMC) {
         fpParMapIt = new TAGparaDsc(new TAITparMap());
         TAITparMap* parMap = (TAITparMap*)fpParMapIt->Object();
         parFileName = fCampManager->GetCurMapFile(FootBaseName("TAITparGeo"), fRunNumber);
         parMap->FromFile(parFileName.Data());
      }
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

      if ((fFlagMsdTrack = parConf->GetAnalysisPar().TrackingFlag)) {
         fMsdTrackingAlgo = parConf->GetAnalysisPar().TrackingAlgo;
      }
      
      fpParCalMsd = new TAGparaDsc("msdCal", new TAMSDparCal( parGeo->GetStripsN() ));
      TAMSDparCal* parCalMsd = (TAMSDparCal*)fpParCalMsd->Object();
      
      if(!fFlagMC){
         fpParMapMsd = new TAGparaDsc(new TAMSDparMap());
         TAMSDparMap*  parMapMsd = (TAMSDparMap*)fpParMapMsd->Object();
         parFileName = fCampManager->GetCurMapFile(FootBaseName("TAMSDparGeo"), fRunNumber);
         parMapMsd->FromFile(parFileName.Data());

         Bool_t energyFile = true;
         parFileName = fCampManager->GetCurCalFile(FootBaseName("TAMSDparGeo"), fRunNumber, energyFile);
         parCalMsd->LoadEnergyCalibrationMap(parFileName.Data());

         parFileName = fCampManager->GetCurCalFile(FootBaseName("TAMSDparGeo"), fRunNumber);
         parCalMsd->LoadPedestalMap(parFileName.Data());
      } else {
         parFileName = fCampManager->GetCurCalFile(FootBaseName("TAMSDparGeo"), fRunNumber);
         parCalMsd->LoadPedestalMap(parFileName.Data());
      }
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
      
      fFlagTWbarCalib  = parConf->IsCalibBar();
      fFlagRateSmearTw = parConf->IsRateSmearMc();
      
      fpParCalTw = new TAGparaDsc(new TATWparCal());
      TATWparCal* parCal = (TATWparCal*)fpParCalTw->Object();
      Bool_t isTof_calib = false;

      if(fFlagRateSmearTw && fFlagMC){
         parFileName = fCampManager->GetCurCalFile(FootBaseName("TATWparGeo"), fRunNumber, isTof_calib, fFlagTWbarCalib, fFlagRateSmearTw);
         parCal->FromRateFile(parFileName, fRateInitRun, fRateEndRun);
      }
      
      if(fFlagTWbarCalib) {
         parFileName = fCampManager->GetCurCalFile(FootBaseName("TATWparGeo"), fRunNumber,
                                                   isTof_calib,fFlagTWbarCalib);
         parCal->FromCalibFile(parFileName.Data(),isTof_calib,fFlagTWbarCalib);
      } else {
         parFileName = fCampManager->GetCurCalFile(FootBaseName("TATWparGeo"), fRunNumber,
                                                   isTof_calib,fFlagTWbarCalib);
         parCal->FromCalibFile(parFileName.Data(),isTof_calib,fFlagTWbarCalib);

         Bool_t elossTuning = false;

         if( !((TString)fCampManager->GetCurCampaign()->GetName()).CompareTo("GSI")) {
            elossTuning = true;

            if(elossTuning) {
               Info("ReadParFiles()","Eloss tuning for GSI data status:: ON");
               parFileName = fCampManager->GetCurCalFile(FootBaseName("TATWparGeo"), fRunNumber,
                                                         false, false, true);
               parCal->FromElossTuningFile(parFileName.Data());
            } else {
               Info("ReadParFiles()","Eloss tuning for GSI data status:: OFF\n");
            }
         }
      }

      isTof_calib = true;
      parFileName = fCampManager->GetCurCalFile(FootBaseName("TATWparGeo"), fRunNumber,
                                                isTof_calib,fFlagTWbarCalib);
      parCal->FromCalibFile(parFileName.Data(),isTof_calib,fFlagTWbarCalib);

      parFileName = fCampManager->GetCurConfFile(FootBaseName("TATWparGeo"), fRunNumber,
                                                 Form("%d%s", A_beam,ion_name.Data()),
                                                 (int)(kinE_beam*TAGgeoTrafo::GevToMev()));

      
      if (!parFileName.IsNull()) // should not happen
         parCal->FromFileZID(parFileName.Data(),Z_beam);      
      
      if(fFlagMC) { // set in MC threshold and active bars from data informations
         parFileName = fCampManager->GetCurMapFile(FootBaseName("TATWparGeo"), fRunNumber);
         parCal->FromBarStatusFile(parFileName.Data());
      }
      else { // file not needed in MC: opened only for raw data

        parFileName = fCampManager->GetCurMapFile(FootBaseName("TATWparGeo"), fRunNumber,1);
        parCal->FromDeltaTimeFile(parFileName.Data());
      }
   }

   // initialise par files for caloriomter
   Bool_t isCalEloss = true;
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fpParGeoCa = new TAGparaDsc(new TACAparGeo());
      TACAparGeo* parGeo = (TACAparGeo*)fpParGeoCa->Object();
      TString parFileName = fCampManager->GetCurGeoFile(FootBaseName("TACAparGeo"), fRunNumber);
      parGeo->FromFile(parFileName);

      fpParConfCa = new TAGparaDsc(new TACAparConf());
      TACAparConf* parConf = (TACAparConf*)fpParConfCa->Object();
      parFileName = fCampManager->GetCurConfFile(FootBaseName("TACAparGeo"), fRunNumber);
      parConf->FromFile(parFileName.Data());
      
      fCalClusterAlgo  = parConf->GetAnalysisPar().ClusteringAlgo;
      
      fpParCalCa = new TAGparaDsc(new TACAparCal());
      TACAparCal* parCal = (TACAparCal*)fpParCalCa->Object();

      if(fFlagMC) { // set in MC threshold and active crystals from data informations
         parFileName = fCampManager->GetCurMapFile(FootBaseName("TACAparGeo"), fRunNumber);
         parCal->FromCrysStatusFile(parFileName.Data());

         parFileName = fCampManager->GetCurCalFile(FootBaseName("TACAparGeo"), fRunNumber);
         parCal->LoadEnergyCalibrationMap(parFileName.Data());

      } else {
        fpParMapCa = new TAGparaDsc(new TACAparMap());
        TACAparMap* parMap = (TACAparMap*)fpParMapCa->Object();
        parFileName = fCampManager->GetCurMapFile(FootBaseName("TACAparGeo"), fRunNumber);
        parMap->FromFile(parFileName.Data());
        
        parFileName = fCampManager->GetCurCalFile(FootBaseName("TACAparGeo"), fRunNumber);
        parCal->LoadEnergyCalibrationMap(parFileName.Data());
        parFileName = fCampManager->GetCurCalFile(FootBaseName("TACAparGeo"), fRunNumber, isCalEloss);
        parCal->LoadCryTemperatureCalibrationMap(parFileName.Data());
      }
   }
}

//__________________________________________________________
//! Create reconstruction actions
void BaseReco::CreateRecAction()
{
   if (TAGrecoManager::GetPar()->IncludeBM())
      CreateRecActionBm();

   if (TAGrecoManager::GetPar()->IncludeVT())
      CreateRecActionVtx();

   if (TAGrecoManager::GetPar()->IncludeIT())
      CreateRecActionIt();

   if (TAGrecoManager::GetPar()->IncludeMSD())
      CreateRecActionMsd();

   if (TAGrecoManager::GetPar()->IncludeTW() && !TAGrecoManager::GetPar()->CalibTW())
      CreateRecActionTw();

   if (TAGrecoManager::GetPar()->IncludeCA())
      CreateRecActionCa();

   if (TAGrecoManager::GetPar()->IncludeTOE() && !TAGrecoManager::GetPar()->IncludeKalman())
      CreateRecActionGlb();

   if (!TAGrecoManager::GetPar()->IncludeTOE() && TAGrecoManager::GetPar()->IncludeKalman())
      CreateRecActionGlbGF();

   if (TAGrecoManager::GetPar()->IncludeStraight() && !TAGrecoManager::GetPar()->IncludeDI())
       CreateRecActionGlbS();
}

//__________________________________________________________
//! Create BM reconstruction actions
void BaseReco::CreateRecActionBm()
{
   if(fFlagTrack) {
     fpNtuTrackBm = new TAGdataDsc(new TABMntuTrack());
     if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

      const Char_t* name = FootActionDscName("TABMactNtuTrack");
     fActTrackBm  = new TABMactNtuTrack(name, fpNtuTrackBm, fpNtuHitBm, fpParGeoBm, fpParConfBm, fpParCalBm);
      if (fFlagHisto)
         fActTrackBm->CreateHistogram();
   }
}

//__________________________________________________________
//! Create VTX reconstruction actions
void BaseReco::CreateRecActionVtx()
{
   if(fFlagTrack) {
      fpNtuTrackVtx = new TAGdataDsc(new TAVTntuTrack());
      if (TAGrecoManager::GetPar()->IncludeTG() || TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman())
         fpNtuVtx = new TAGdataDsc(new TAVTntuVertex());
   }

  fpNtuClusVtx  = new TAGdataDsc(new TAVTntuCluster());
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

   const Char_t* name = FootActionDscName("TAVTactNtuCluster");
   if (fM28ClusMtFlag)
      fActClusVtx   = new TAVTactNtuClusterMT(name, fpNtuHitVtx, fpNtuClusVtx, fpParConfVtx, fpParGeoVtx);
   else
      fActClusVtx   = new TAVTactNtuCluster(name, fpNtuHitVtx, fpNtuClusVtx, fpParConfVtx, fpParGeoVtx);

   if (fFlagHisto)
      fActClusVtx->CreateHistogram();

   if (fFlagTrack) {
      const Char_t* name = FootActionDscName("TAVTactNtuTrack");
      if (fVtxTrackingAlgo.Contains("Std") ) {
         if (TAGrecoManager::GetPar()->IncludeBM())
            fActTrackVtx  = new TAVTactNtuTrack(name, fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx, 0, fpNtuTrackBm);
         else
            fActTrackVtx  = new TAVTactNtuTrack(name, fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx);
      } else if (fVtxTrackingAlgo.Contains("Full")) {
         if (TAGrecoManager::GetPar()->IncludeBM())
            fActTrackVtx  = new TAVTactNtuTrackF(name, fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx, 0, fpNtuTrackBm);
         else
            fActTrackVtx  = new TAVTactNtuTrackF(name, fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx);
      } else {
         Error("CreateRecActionVtx()", "No Tracking algorithm defined !");
      }

      

      if (fFlagMC)
         fActTrackVtx->SetFlagMC(true);
      else
         fActTrackVtx->SetFlagMC(false);

      if (fFlagHisto)
         fActTrackVtx->CreateHistogram();

      if (TAGrecoManager::GetPar()->IncludeTG()) {
         const Char_t* name = FootActionDscName("TAVTactNtuVertexPD");
         if(TAGrecoManager::GetPar()->IncludeBM()) {
            fActVtx = new TAVTactNtuVertexPD(name, fpNtuTrackVtx, fpNtuVtx, fpParConfVtx, fpParGeoVtx, fpParGeoG, fpNtuTrackBm);

         } else
            fActVtx = new TAVTactNtuVertexPD(name, fpNtuTrackVtx, fpNtuVtx, fpParConfVtx, fpParGeoVtx, fpParGeoG);


         if (fFlagMC) fActVtx->SetFlagMC(true);
         else fActVtx->SetFlagMC(false);

         if (fFlagHisto)
            fActVtx->CreateHistogram();
      }
   }
}

//__________________________________________________________
//! Create ITR reconstruction actions
void BaseReco::CreateRecActionIt()
{
   fpNtuClusIt = new TAGdataDsc(new TAITntuCluster());
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

   const Char_t* name = FootActionDscName("TAITactNtuCluster");

   if (fM28ClusMtFlag)
      fActClusIt = new TAITactNtuClusterMT(name, fpNtuHitIt, fpNtuClusIt, fpParConfIt, fpParGeoIt);
   else
      fActClusIt = new TAITactNtuCluster(name, fpNtuHitIt, fpNtuClusIt, fpParConfIt, fpParGeoIt);

   if (fFlagHisto)
     fActClusIt->CreateHistogram();

   if (fFlagItrTrack && fFlagTrack) {
      fpNtuTrackIt = new TAGdataDsc(new TAITntuTrack());
      
      const Char_t* name = FootActionDscName("TAITactNtuTrack");
      if (fItrTrackingAlgo.Contains("Std") ) {
         if (TAGrecoManager::GetPar()->IncludeBM())
            fActTrackIt = new TAITactNtuTrack(name, fpNtuClusIt, fpNtuTrackIt, fpParConfIt, fpParGeoIt, 0x0, fpNtuTrackBm);
         else
            fActTrackIt = new TAITactNtuTrack(name, fpNtuClusIt, fpNtuTrackIt, fpParConfIt, fpParGeoIt);

      }  else if (fItrTrackingAlgo.Contains("Full")) {
         fActTrackIt = new TAITactNtuTrackF(name, fpNtuClusIt, fpNtuTrackIt, fpParConfIt, fpParGeoIt, 0x0, fpParGeoG);
      }

      if (fFlagHisto)
         fActTrackIt->CreateHistogram();
   }
}

//__________________________________________________________
//! Create MSD reconstruction actions
void BaseReco::CreateRecActionMsd()
{
   fpNtuClusMsd = new TAGdataDsc(new TAMSDntuCluster());
   fpNtuRecMsd = new TAGdataDsc(new TAMSDntuPoint());

   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

   const Char_t* name = FootActionDscName("TAMSDactNtuCluster");
   fActClusMsd = new TAMSDactNtuCluster(name, fpNtuHitMsd, fpNtuClusMsd, fpParConfMsd, fpParGeoMsd, fpParCalMsd, fFlagMC);

   if (fFlagHisto)
      fActClusMsd->CreateHistogram();

   name = FootActionDscName("TAMSDactNtuPoint");
   fActPointMsd = new TAMSDactNtuPoint(name, fpNtuClusMsd, fpNtuRecMsd, fpParGeoMsd);
   if (fFlagHisto)
      fActPointMsd->CreateHistogram();

   if (fFlagMsdTrack && fFlagTrack) {
      fpNtuTrackMsd = new TAGdataDsc(new TAMSDntuTrack());

      const Char_t* name = FootActionDscName("TAMSDactNtuTrack");
      if (fMsdTrackingAlgo.Contains("Std") ) {
         fActTrackMsd = new TAMSDactNtuTrack(name, fpNtuRecMsd, fpNtuTrackMsd, fpParConfMsd, fpParGeoMsd);

      }  else if (fMsdTrackingAlgo.Contains("Full")) {
         fActTrackMsd = new TAMSDactNtuTrackF(name, fpNtuRecMsd, fpNtuTrackMsd, fpParConfMsd, fpParGeoMsd, fpParGeoG);
      }

      if (fFlagHisto)
         fActTrackMsd->CreateHistogram();
   }
}

//__________________________________________________________
//! Create TW reconstruction actions
void BaseReco::CreateRecActionTw()
{
   fpNtuRecTw = new TAGdataDsc(new TATWntuPoint());
   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

   const Char_t* name = FootActionDscName("TATWactNtuPoint");
   fActPointTw = new TATWactNtuPoint(name, fpNtuHitTw, fpNtuRecTw, fpParGeoTw, fpParConfTw, fpParCalTw);
   if (fFlagHisto)
     fActPointTw->CreateHistogram();
}

//__________________________________________________________
//! Create CAL reconstruction actions
void BaseReco::CreateRecActionCa()
{
   fpNtuClusCa = new TAGdataDsc(new TACAntuCluster());
   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;
   
   const Char_t* name = FootActionDscName("TACAactNtuCluster");
   if (fCalClusterAlgo.Contains("Std") ) {
      fActClusCa = new TACAactNtuCluster(name, fpNtuHitCa, fpNtuClusCa, fpParGeoCa, fpParCalCa, fpParConfCa, fpNtuRecTw, fFlagMC);
   } else if (fCalClusterAlgo.Contains("Padme") ) {
      fActClusCa = new TACAactNtuClusterP(name, fpNtuHitCa, fpNtuClusCa, fpParGeoCa, fpParCalCa, fpParConfCa, fpNtuRecTw, fFlagMC);
   }
   
   if (fFlagHisto)
      fActClusCa->CreateHistogram();
}

//__________________________________________________________
//! Create global track reconstruction TOE action
void BaseReco::CreateRecActionGlb()
{
#ifdef TOE_FLAG
  if(fFlagRecCutter) {
     SetL0TreeBranches();
     return;
  }
   
  if(fFlagTrack) {
    SetL0TreeBranches();
    fpNtuGlbTrack = new TAGdataDsc(new TAGntuGlbTrack());
    const Char_t* name = FootActionDscName("TAGactNtuGlbTrack");
    fActGlbTrack  = new TAGactNtuGlbTrack(name,
					   fpNtuClusVtx,
					   fpNtuTrackVtx,
					   fpNtuVtx,
					   fpNtuClusIt,
					   fpNtuClusMsd,
                  fpNtuRecMsd,
					   fpNtuRecTw,
					   fpNtuGlbTrack,
					   fpParGeoG,
					   fpParGeoDi,
					   fpParGeoVtx,
					   fpParGeoIt,
					   fpParGeoMsd,
					   fpParGeoTw,
					   fField);
    if (fFlagHisto)
      fActGlbTrack->CreateHistogram();
  }
#endif
}

//__________________________________________________________
//! Create global track reconstruction GenFit action
void BaseReco::CreateRecActionGlbGF()
{
#ifdef GENFIT_FLAG
	if(fFlagTrack) {
		SetL0TreeBranches();

		if (TAGrecoManager::GetPar()->IncludeDI()) {
         genfit::FieldManager::getInstance()->init( new TADIgenField(fField) );
         if( FootDebugLevel(1) )
         {
            cout << "Check of magnetic field value in random position (0,0,14)" << endl;
            genfit::FieldManager::getInstance()->getFieldVal( TVector3(0,0,14) ).Print();
         }
		}
      //Write function for zero field inizialization
      else
      {
         genfit::FieldManager::getInstance()->init( new TADIgenField() );
         if( FootDebugLevel(1) )
         {
            cout << "Check of magnetic field value in random position (0,0,14)" << endl;
            genfit::FieldManager::getInstance()->getFieldVal( TVector3(0,0,14) ).Print();
         }
      }

		// set material and geometry into genfit
		MaterialEffects* materialEffects = MaterialEffects::getInstance();
		materialEffects->init(new TGeoMaterialInterface());

		// include the nucleon into the genfit pdg repository
		UpdatePDG::Instance();

		// Initialisation of KFfitter
		fpNtuGlbTrack = new TAGdataDsc(new TAGntuGlbTrack());
      const Char_t* name = FootActionDscName("TAGactKFitter");
		fActGlbkFitter = new TAGactKFitter(name, fpNtuGlbTrack);
      if(fFlagMC)
         fActGlbkFitter->SetMcSample();
		if (fFlagHisto)
			fActGlbkFitter->CreateHistogram();
	}
#endif
}

//__________________________________________________________
//! Create global straight track reconstruction action
void BaseReco::CreateRecActionGlbS()
{
   if(fFlagTrack) {
      fpNtuGlbTrack = new TAGdataDsc(new TAGntuGlbTrack());
      const Char_t* name = FootActionDscName("TAGactNtuGlbTrackS");
      fActGlbTrackS = new TAGactNtuGlbTrackS(name, fpNtuVtx, fpNtuClusIt, fpNtuRecMsd, fpNtuRecTw, fpNtuClusCa, fpNtuGlbTrack, fpParGeoVtx, fpParGeoIt, fpParGeoMsd, fpParGeoTw, fpParGeoG);
      
      if (fFlagHisto)
         fActGlbTrackS->CreateHistogram();
   }
}

//__________________________________________________________
//! Set L0 tree branches for reading back
void BaseReco::SetL0TreeBranches()
{
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) {
    const Char_t* name = FootActionDscName("TAGactTreeReader");
    fActEvtReader = new TAGactTreeReader(name);
     
    if (!fgSaveMcFlag)
       if (!fFriendFileName.IsNull() && !fFriendTreeName.IsNull()) {
          fActEvtReader->AddFriendTree(fFriendFileName,fFriendTreeName);
          Info("SetL0TreeBranches()", "\n Open file %s for friend tree %s\n", fFriendFileName.Data(), fFriendTreeName.Data());
       }
     
    if (TAGrecoManager::GetPar()->IncludeBM())
       fActEvtReader->SetupBranch(fpNtuTrackBm);
   
    if (TAGrecoManager::GetPar()->IncludeVT()) {
      fActEvtReader->SetupBranch(fpNtuTrackVtx);
      fActEvtReader->SetupBranch(fpNtuClusVtx);
      fActEvtReader->SetupBranch(fpNtuVtx);
    }

    if (TAGrecoManager::GetPar()->IncludeIT())
      fActEvtReader->SetupBranch(fpNtuClusIt);

    if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fActEvtReader->SetupBranch(fpNtuClusMsd);
      fActEvtReader->SetupBranch(fpNtuRecMsd);
    }

    if(TAGrecoManager::GetPar()->IncludeTW())
      fActEvtReader->SetupBranch(fpNtuRecTw);

    if(TAGrecoManager::GetPar()->IncludeCA())
      fActEvtReader->SetupBranch(fpNtuClusCa);


    if (fReadL0Hits) {
       if (TAGrecoManager::GetPar()->IncludeST()) {
          if (fFlagMC) {
             fpNtuMcSt   = new TAGdataDsc(FootDataDscMcName(kST), new TAMCntuHit());
             fActEvtReader->SetupBranch(fpNtuMcSt, FootBranchMcName(kST));
          }
          fpNtuHitSt   = new TAGdataDsc(new TASTntuHit());
          fActEvtReader->SetupBranch(fpNtuHitSt);
       }

       if (TAGrecoManager::GetPar()->IncludeBM()) {
          if (fFlagMC) {
             fpNtuMcBm   = new TAGdataDsc(FootDataDscMcName(kBM), new TAMCntuHit());
             fActEvtReader->SetupBranch(fpNtuMcBm, FootBranchMcName(kBM));
          }
          fpNtuHitBm = new TAGdataDsc(new TABMntuHit());
          fActEvtReader->SetupBranch(fpNtuHitBm);
       }

       if (TAGrecoManager::GetPar()->IncludeVT() && fFlagMC) {
          fpNtuMcVt   = new TAGdataDsc(FootDataDscMcName(kVTX), new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcVt, FootBranchMcName(kVTX));
       }

       if (TAGrecoManager::GetPar()->IncludeIT() && fFlagMC) {
          fpNtuMcIt   = new TAGdataDsc(FootDataDscMcName(kITR), new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcIt, FootBranchMcName(kITR));
       }

       if (TAGrecoManager::GetPar()->IncludeMSD() && fFlagMC) {
          fpNtuMcMsd   = new TAGdataDsc(FootDataDscMcName(kMSD), new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcMsd, FootBranchMcName(kMSD));
       }

       if(TAGrecoManager::GetPar()->IncludeTW() && fFlagMC) {
          fpNtuMcTw   = new TAGdataDsc(FootDataDscMcName(kTW), new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcTw, FootBranchMcName(kTW));
       }

       if(TAGrecoManager::GetPar()->IncludeCA() && fFlagMC) {
          fpNtuMcCa   = new TAGdataDsc(FootDataDscMcName(kCAL), new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcCa, FootBranchMcName(kCAL));
       }
     }
  }
}


//__________________________________________________________
//! Add required reconstruction actions in list
void BaseReco::AddRequiredItem()
{
   if (TAGrecoManager::GetPar()->IncludeTOE() && TAGrecoManager::GetPar()->IsFromLocalReco()) {
     if (fFlagTrack) {
       if(!fFlagRecCutter)
          gTAGroot->AddRequiredItem(FootActionDscName("TAGactNtuGlbTrack"));
       else
          gTAGroot->AddRequiredItem(FootActionDscName("TAGactTreeReader"));
     }
     return;
   }
   
   TList* list = gTAGroot->ListOfAction();
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TAGaction* action = (TAGaction*)list->At(i);
      TString name(action->GetName());
      if(name.BeginsWith("act")) continue;
      if (name.IsNull()) continue;
      if (name == FootActionDscName("TAGactDscTreeWriter")) continue; // skip must be at the end
      if (name == FootActionDscName("TAGactFlatTreeWriter")) continue; // skip must be at the end

      gTAGroot->AddRequiredItem(name.Data());
      if (FootDebugLevel(1))
         printf("[%s]\n", name.Data());
   }
   
   if (fFlagOut) {
      if (fFlagFlatTree)
         gTAGroot->AddRequiredItem(FootActionDscName("TAGactFlatTreeWriter"));
      else
         gTAGroot->AddRequiredItem(FootActionDscName("TAGactDscTreeWriter"));
   }

}
