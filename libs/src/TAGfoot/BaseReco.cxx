

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

#include "TAGrecoManager.hxx"

/*!
 \class BaseReco
 \brief  Base class for reconstruction
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
ClassImp(BaseReco)

Bool_t  BaseReco::fgSaveMcFlag     = true;

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
BaseReco::BaseReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
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
   fFlagMC(false),
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

   if (fFlagOut)
     fActEvtWriter = new TAGactTreeWriter("locRecFile");
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

   AddRawRequiredItem();
   AddRecRequiredItem();

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
#ifdef GENFIT_FLAG
   if (TAGrecoManager::GetPar()->IncludeKalman())	fActGlbkFitter->Finalize();
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
  if (fFlagTree)  SetTreeBranches();
   fActEvtWriter->Open(GetTitle(), "RECREATE");

   if (fFlagHisto) {
      SetRawHistogramDir();
      SetRecHistogramDir();
   }
}

//__________________________________________________________
//! Set reconstructed histogram directory
void BaseReco::SetRecHistogramDir()
{
   //Global track
   if(fFlagRecCutter) return;
   
   if (fFlagTrack) {

#ifdef GENFIT_FLAG
      if (!TAGrecoManager::GetPar()->IncludeTOE() && TAGrecoManager::GetPar()->IncludeKalman()) {
        TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAGgeoTrafo::GetBaseName());
        fActGlbkFitter->SetHistogramDir(subfolder);
        if (TAGrecoManager::GetPar()->IsFromLocalReco()) return;
      }
#endif
#ifdef TOE_FLAG
      if (TAGrecoManager::GetPar()->IncludeTOE() && !TAGrecoManager::GetPar()->IncludeKalman()) {
         TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAGgeoTrafo::GetBaseName());
         fActGlbTrack->SetHistogramDir(subfolder);
         if (TAGrecoManager::GetPar()->IsFromLocalReco()) return;
      }
#endif
   }

   //BMN
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      if (fFlagTrack) {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TABMparGeo::GetBaseName());
         fActTrackBm->SetHistogramDir(subfolder);
      }
   }

   // VTX
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TAVTparGeo::GetBaseName());
      fActClusVtx->SetHistogramDir(subfolder);

      if (fFlagTrack) {
         fActTrackVtx->SetHistogramDir(subfolder);
         if (TAGrecoManager::GetPar()->IncludeTG()) {
            fActVtx->SetHistogramDir(subfolder);
         }
      }
   }

   // IT
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TAITparGeo::GetBaseName());
      fActClusIt->SetHistogramDir(subfolder);
   }

   // MSD
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TAMSDparGeo::GetBaseName());
      fActClusMsd->SetHistogramDir(subfolder);
      fActPointMsd->SetHistogramDir(subfolder);
      if (fFlagMsdTrack && fFlagTrack)
         fActTrackMsd->SetHistogramDir(subfolder);
   }

   // TW
   if (TAGrecoManager::GetPar()->IncludeTW() && !TAGrecoManager::GetPar()->CalibTW()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TATWparGeo::GetBaseName());
      fActPointTw->SetHistogramDir(subfolder);
   }

   // CA
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TACAparGeo::GetBaseName());
      fActClusCa->SetHistogramDir(subfolder);
      //fActNtuHitCa->SetHistogramDir(subfolder);
   }

   // Global straight track
   if (TAGrecoManager::GetPar()->IncludeStraight() && !TAGrecoManager::GetPar()->IncludeDI()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->mkdir(TAGgeoTrafo::GetBaseName());
      fActGlbTrackS->SetHistogramDir(subfolder);
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
   TString parFileName = fCampManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), fRunNumber);
   fpFootGeo->FromFile(parFileName);

   // initialise par files for target
   if (TAGrecoManager::GetPar()->IncludeTG() || TAGrecoManager::GetPar()->IncludeBM() || TAGrecoManager::GetPar()->IncludeTW() || TAGrecoManager::GetPar()->IncludeCA() || fFlagItrTrack) {
      fpParGeoG = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
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

     fpParGeoSt = new TAGparaDsc(TASTparGeo::GetDefParaName(), new TASTparGeo());
     TASTparGeo* parGeo = (TASTparGeo*)fpParGeoSt->Object();
     TString parFileName = fCampManager->GetCurGeoFile(TASTparGeo::GetBaseName(), fRunNumber);
     parGeo->FromFile(parFileName.Data());

      if(!fFlagMC) {
         fpParMapSt = new TAGparaDsc("stMap", new TASTparMap());
         TASTparMap* parMapSt = (TASTparMap*) fpParMapSt->Object();
         parFileName = fCampManager->GetCurConfFile(TASTparGeo::GetBaseName(), fRunNumber);
         parMapSt->FromFile(parFileName);

         fpParMapTw = new TAGparaDsc("twMap", new TATWparMap());
         TATWparMap* parMap = (TATWparMap*)fpParMapTw->Object();
         parFileName = fCampManager->GetCurMapFile(TATWparGeo::GetBaseName(), fRunNumber);
         parMap->FromFile(parFileName.Data());

         fpParMapWD = new TAGparaDsc("WDMap", new TAGbaseWDparMap());
         TAGbaseWDparMap* parMapWD = (TAGbaseWDparMap*)fpParMapWD->Object();
         parFileName = fCampManager->GetCurMapFile(TASTparGeo::GetBaseName(), fRunNumber);
         parMapWD->FromFile(parFileName.Data());

         fpParTimeWD = new TAGparaDsc("WDTim", new TAGbaseWDparTime());
         TAGbaseWDparTime* parTimeWD = (TAGbaseWDparTime*) fpParTimeWD->Object();
         TString parFileName = fCampManager->GetCurCalFile(TASTparGeo::GetBaseName(), fRunNumber);
         parTimeWD->FromFileCFD(parFileName.Data());
      }
   }

   // initialise par files for Beam Monitor
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fpParGeoBm = new TAGparaDsc("bmGeo", new TABMparGeo());
      TABMparGeo* parGeo = (TABMparGeo*)fpParGeoBm->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TABMparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());

      fpParConfBm = new TAGparaDsc("bmConf", new TABMparConf());
      TABMparConf* parConf = (TABMparConf*)fpParConfBm->Object();
      parFileName = fCampManager->GetCurConfFile(TABMparGeo::GetBaseName(), fRunNumber);
      parConf->FromFile(parFileName.Data());

      fpParCalBm = new TAGparaDsc("bmCal", new TABMparCal());
      TABMparCal* parCal = (TABMparCal*)fpParCalBm->Object();
      parFileName = fCampManager->GetCurCalFile(TABMparGeo::GetBaseName(), fRunNumber);
      parCal->FromFile(parFileName.Data());

      if(!fFlagMC){
        fpParMapBm = new TAGparaDsc("bmMap", new TABMparMap());
        TABMparMap*  parMapBm = (TABMparMap*)fpParMapBm->Object();
        parFileName = fCampManager->GetCurMapFile(TABMparGeo::GetBaseName(), fRunNumber);
        parMapBm->FromFile(parFileName.Data(), parGeo);
      }
   }

   // initialise par files dipole
   if (TAGrecoManager::GetPar()->IncludeDI() ) {
      fpParGeoDi = new TAGparaDsc(TADIparGeo::GetDefParaName(), new TADIparGeo());
      TADIparGeo* parGeo = (TADIparGeo*)fpParGeoDi->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TADIparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());

      if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) {
         TAGparaDsc* fieldDsc = new TAGparaDsc(TADIgeoField::GetDefParaName(), new TADIgeoField(parGeo));
         fField = (TADIgeoField*)fieldDsc->Object();
      }
   }

   // initialise par files for vertex
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fpParGeoVtx = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
      TAVTparGeo* parGeo = (TAVTparGeo*)fpParGeoVtx->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());

      fpParConfVtx = new TAGparaDsc("vtConf", new TAVTparConf());
      TAVTparConf* parConf = (TAVTparConf*)fpParConfVtx->Object();
      parFileName = fCampManager->GetCurConfFile(TAVTparGeo::GetBaseName(), fRunNumber);
      parConf->FromFile(parFileName.Data());
    
      fVtxTrackingAlgo = parConf->GetAnalysisPar().TrackingAlgo;
      
      if(!fFlagMC) {
         fpParMapVtx = new TAGparaDsc("vtMap", new TAVTparMap());
         TAVTparMap* parMap = (TAVTparMap*)fpParMapVtx->Object();
         parFileName = fCampManager->GetCurMapFile(TAVTparGeo::GetBaseName(), fRunNumber);
         parMap->FromFile(parFileName.Data());
      }
   }

   // initialise par files for inner tracker
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fpParGeoIt = new TAGparaDsc(TAITparGeo::GetDefParaName(), new TAITparGeo());
      TAITparGeo* parGeo = (TAITparGeo*)fpParGeoIt->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TAITparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());

      fpParConfIt = new TAGparaDsc("itConf", new TAITparConf());
      TAITparConf* parConf = (TAITparConf*)fpParConfIt->Object();
      parFileName = fCampManager->GetCurConfFile(TAITparGeo::GetBaseName(), fRunNumber);
      parConf->FromFile(parFileName.Data());

      if ((fFlagItrTrack = parConf->GetAnalysisPar().TrackingFlag)) {
         fVtxTrackingAlgo = parConf->GetAnalysisPar().TrackingAlgo;
      }
      
      if(!fFlagMC) {
         fpParMapIt = new TAGparaDsc("itMap", new TAITparMap());
         TAITparMap* parMap = (TAITparMap*)fpParMapIt->Object();
         parFileName = fCampManager->GetCurMapFile(TAITparGeo::GetBaseName(), fRunNumber);
         parMap->FromFile(parFileName.Data());
      }
   }

   // initialise par files for multi strip detector
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fpParGeoMsd = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
      TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());

      fpParConfMsd = new TAGparaDsc("msdConf", new TAMSDparConf());
      TAMSDparConf* parConf = (TAMSDparConf*)fpParConfMsd->Object();
      parFileName = fCampManager->GetCurConfFile(TAMSDparGeo::GetBaseName(), fRunNumber);
      parConf->FromFile(parFileName.Data());

      if ((fFlagMsdTrack = parConf->GetAnalysisPar().TrackingFlag)) {
         fMsdTrackingAlgo = parConf->GetAnalysisPar().TrackingAlgo;
      }
      
      if(!fFlagMC){
         fpParMapMsd = new TAGparaDsc("msdMap", new TAMSDparMap());
         TAMSDparMap*  parMapMsd = (TAMSDparMap*)fpParMapMsd->Object();
         parFileName = fCampManager->GetCurMapFile(TAMSDparGeo::GetBaseName(), fRunNumber);
         parMapMsd->FromFile(parFileName.Data());

         Bool_t energyFile = true;
         fpParCalMsd = new TAGparaDsc("msdCal", new TAMSDparCal( parGeo->GetStripsN() ));
         TAMSDparCal* parCalMsd = (TAMSDparCal*)fpParCalMsd->Object();
         parFileName = fCampManager->GetCurCalFile(TAMSDparGeo::GetBaseName(), fRunNumber, energyFile);
         parCalMsd->LoadEnergyCalibrationMap(parFileName.Data());

         parFileName = fCampManager->GetCurCalFile(TAMSDparGeo::GetBaseName(), fRunNumber);
         parCalMsd->LoadPedestalMap(parFileName.Data());
      }
   }

   // initialise par files for Tof Wall
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fpParGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
      TATWparGeo* parGeo = (TATWparGeo*)fpParGeoTw->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TATWparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());

      fpParConfTw = new TAGparaDsc("twConf", new TATWparConf());
      TATWparConf* parConf = (TATWparConf*)fpParConfTw->Object();
      parFileName = fCampManager->GetCurConfFile(TATWparGeo::GetBaseName(), fRunNumber);
      parConf->FromFile(parFileName.Data());
      
      fFlagTWbarCalib  = parConf->IsCalibBar();
      fFlagRateSmearTw = parConf->IsRateSmearMc();
      
      fpParCalTw = new TAGparaDsc("twCal", new TATWparCal());
      TATWparCal* parCal = (TATWparCal*)fpParCalTw->Object();
      Bool_t isTof_calib = false;

      if(fFlagRateSmearTw && fFlagMC){
         parFileName = fCampManager->GetCurCalFile(TATWparGeo::GetBaseName(), fRunNumber, isTof_calib, fFlagTWbarCalib, fFlagRateSmearTw);
         parCal->FromRateFile(parFileName, fRateInitRun, fRateEndRun);
      }
      
      if(fFlagTWbarCalib) {
         parFileName = fCampManager->GetCurCalFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                   isTof_calib,fFlagTWbarCalib);
         parCal->FromCalibFile(parFileName.Data(),isTof_calib,fFlagTWbarCalib);
      } else {
         parFileName = fCampManager->GetCurCalFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                   isTof_calib,fFlagTWbarCalib);
         parCal->FromCalibFile(parFileName.Data(),isTof_calib,fFlagTWbarCalib);

         Bool_t elossTuning = false;

         if( !((TString)fCampManager->GetCurCampaign()->GetName()).CompareTo("GSI")) {
            elossTuning = true;

            if(elossTuning) {
               Info("ReadParFiles()","Eloss tuning for GSI data status:: ON");
               parFileName = fCampManager->GetCurCalFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                         false, false, true);
               parCal->FromElossTuningFile(parFileName.Data());
            } else {
               Info("ReadParFiles()","Eloss tuning for GSI data status:: OFF\n");
            }
         }
      }

      isTof_calib = true;
      parFileName = fCampManager->GetCurCalFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                isTof_calib,fFlagTWbarCalib);
      parCal->FromCalibFile(parFileName.Data(),isTof_calib,fFlagTWbarCalib);

      parFileName = fCampManager->GetCurConfFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                 Form("%d%s", A_beam,ion_name.Data()),
                                                 (int)(kinE_beam*TAGgeoTrafo::GevToMev()));
      
      if (!parFileName.IsNull()) // should not happen
         parCal->FromFileZID(parFileName.Data(),Z_beam);

      if(fFlagMC) { // set in MC threshold and active bars from data informations
         parFileName = fCampManager->GetCurMapFile(TATWparGeo::GetBaseName(), fRunNumber);
         parCal->FromBarStatusFile(parFileName.Data());
      }
   }

   // initialise par files for caloriomter
   Bool_t isCalEloss = true;
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fpParGeoCa = new TAGparaDsc(TACAparGeo::GetDefParaName(), new TACAparGeo());
      TACAparGeo* parGeo = (TACAparGeo*)fpParGeoCa->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TACAparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName);

      fpParConfCa = new TAGparaDsc("caConf", new TACAparConf());
      TACAparConf* parConf = (TACAparConf*)fpParConfCa->Object();
      parFileName = fCampManager->GetCurConfFile(TACAparGeo::GetBaseName(), fRunNumber);
      parConf->FromFile(parFileName.Data());
      
      fCalClusterAlgo  = parConf->GetAnalysisPar().ClusteringAlgo;
      
      fpParCalCa = new TAGparaDsc("caCal", new TACAparCal());
      TACAparCal* parCal = (TACAparCal*)fpParCalCa->Object();

      if(fFlagMC) { // set in MC threshold and active crystals from data informations
         parFileName = fCampManager->GetCurMapFile(TACAparGeo::GetBaseName(), fRunNumber);
         parCal->FromCrysStatusFile(parFileName.Data());

         parFileName = fCampManager->GetCurCalFile(TACAparGeo::GetBaseName(), fRunNumber);
         parCal->LoadEnergyCalibrationMap(parFileName.Data());

      } else {
        fpParMapCa = new TAGparaDsc("caMap", new TACAparMap());
        TACAparMap* parMap = (TACAparMap*)fpParMapCa->Object();
        parFileName = fCampManager->GetCurMapFile(TACAparGeo::GetBaseName(), fRunNumber);
        parMap->FromFile(parFileName.Data());
        
        parFileName = fCampManager->GetCurCalFile(TACAparGeo::GetBaseName(), fRunNumber);
        parCal->LoadEnergyCalibrationMap(parFileName.Data());
        parFileName = fCampManager->GetCurCalFile(TACAparGeo::GetBaseName(), fRunNumber, isCalEloss);
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
     fpNtuTrackBm = new TAGdataDsc("bmTrack", new TABMntuTrack());
     if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

     fActTrackBm  = new TABMactNtuTrack("bmActTrack", fpNtuTrackBm, fpNtuHitBm, fpParGeoBm, fpParConfBm, fpParCalBm);
      if (fFlagHisto)
         fActTrackBm->CreateHistogram();
   }
}

//__________________________________________________________
//! Create VTX reconstruction actions
void BaseReco::CreateRecActionVtx()
{
   if(fFlagTrack) {
      fpNtuTrackVtx = new TAGdataDsc("vtTrack", new TAVTntuTrack());
      if (TAGrecoManager::GetPar()->IncludeTG() || TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman())
         fpNtuVtx = new TAGdataDsc("vtVtx",   new TAVTntuVertex());
   }

  fpNtuClusVtx  = new TAGdataDsc("vtClus", new TAVTntuCluster());
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

   if (fM28ClusMtFlag)
      fActClusVtx   = new TAVTactNtuClusterMT("vtActClus", fpNtuHitVtx, fpNtuClusVtx, fpParConfVtx, fpParGeoVtx);
   else
      fActClusVtx   = new TAVTactNtuCluster("vtActClus", fpNtuHitVtx, fpNtuClusVtx, fpParConfVtx, fpParGeoVtx);

   if (fFlagHisto)
      fActClusVtx->CreateHistogram();

   if (fFlagTrack) {
      if (fVtxTrackingAlgo.Contains("Std") ) {
         if (TAGrecoManager::GetPar()->IncludeBM())
            fActTrackVtx  = new TAVTactNtuTrack("vtActTrack", fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx, 0, fpNtuTrackBm);
         else
            fActTrackVtx  = new TAVTactNtuTrack("vtActTrack", fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx);
      } else if (fVtxTrackingAlgo.Contains("Full")) {
         if (TAGrecoManager::GetPar()->IncludeBM())
            fActTrackVtx  = new TAVTactNtuTrackF("vtActTrack", fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx, 0, fpNtuTrackBm);
         else
            fActTrackVtx  = new TAVTactNtuTrackF("vtActTrack", fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx);
      } else {
         Error("CreateRecActionVtx()", "No Tracking algorithm defined !");
      }

      if (fFlagHisto)
         fActTrackVtx->CreateHistogram();

      if (TAGrecoManager::GetPar()->IncludeTG()) {
         if(TAGrecoManager::GetPar()->IncludeBM()) {
            fActVtx = new TAVTactNtuVertexPD("vtActVtx", fpNtuTrackVtx, fpNtuVtx, fpParConfVtx, fpParGeoVtx, fpParGeoG, fpNtuTrackBm);

         } else
            fActVtx = new TAVTactNtuVertexPD("vtActVtx", fpNtuTrackVtx, fpNtuVtx, fpParConfVtx, fpParGeoVtx, fpParGeoG);

         if (fFlagHisto)
            fActVtx->CreateHistogram();
      }
   }
}

//__________________________________________________________
//! Create ITR reconstruction actions
void BaseReco::CreateRecActionIt()
{
   fpNtuClusIt = new TAGdataDsc("itClus", new TAITntuCluster());
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

   if (fM28ClusMtFlag)
      fActClusIt = new TAITactNtuClusterMT("itActClus", fpNtuHitIt, fpNtuClusIt, fpParConfIt, fpParGeoIt);
   else
      fActClusIt = new TAITactNtuCluster("itActClus", fpNtuHitIt, fpNtuClusIt, fpParConfIt, fpParGeoIt);

   if (fFlagHisto)
     fActClusIt->CreateHistogram();

   if (fFlagItrTrack && fFlagTrack) {
      fpNtuTrackIt = new TAGdataDsc("itTrack", new TAITntuTrack());

      if (fItrTrackingAlgo.Contains("Std") ) {
         if (TAGrecoManager::GetPar()->IncludeBM())
            fActTrackIt = new TAITactNtuTrack("itActTrack", fpNtuClusIt, fpNtuTrackIt, fpParConfIt, fpParGeoIt, 0x0, fpNtuTrackBm);
         else
            fActTrackIt = new TAITactNtuTrack("itActTrack", fpNtuClusIt, fpNtuTrackIt, fpParConfIt, fpParGeoIt);

      }  else if (fItrTrackingAlgo.Contains("Full")) {
         fActTrackIt = new TAITactNtuTrackF("itActTrack", fpNtuClusIt, fpNtuTrackIt, fpParConfIt, fpParGeoIt, 0x0, fpParGeoG);
      }

      if (fFlagHisto)
         fActTrackIt->CreateHistogram();
   }
}

//__________________________________________________________
//! Create MSD reconstruction actions
void BaseReco::CreateRecActionMsd()
{
   fpNtuClusMsd = new TAGdataDsc("msdClus", new TAMSDntuCluster());
    fpNtuRecMsd = new TAGdataDsc("msdPoint", new TAMSDntuPoint());

   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

   fActClusMsd = new TAMSDactNtuCluster("msdActClus", fpNtuHitMsd, fpNtuClusMsd, fpParConfMsd, fpParGeoMsd, fpParCalMsd);
   if (fFlagHisto)
      fActClusMsd->CreateHistogram();

   fActPointMsd = new TAMSDactNtuPoint("msdActPoint", fpNtuClusMsd, fpNtuRecMsd, fpParGeoMsd);
   if (fFlagHisto)
      fActPointMsd->CreateHistogram();

   if (fFlagMsdTrack && fFlagTrack) {
      fpNtuTrackMsd = new TAGdataDsc("msdTrack", new TAMSDntuTrack());

      if (fMsdTrackingAlgo.Contains("Std") ) {
         fActTrackMsd = new TAMSDactNtuTrack("msdActTrack", fpNtuRecMsd, fpNtuTrackMsd, fpParConfMsd, fpParGeoMsd);

      }  else if (fMsdTrackingAlgo.Contains("Full")) {
         fActTrackMsd = new TAMSDactNtuTrackF("msdActTrack", fpNtuRecMsd, fpNtuTrackMsd, fpParConfMsd, fpParGeoMsd, fpParGeoG);
      }

      if (fFlagHisto)
         fActTrackMsd->CreateHistogram();
   }
}

//__________________________________________________________
//! Create TW reconstruction actions
void BaseReco::CreateRecActionTw()
{
   fpNtuRecTw = new TAGdataDsc("twPoint", new TATWntuPoint());
   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

   fActPointTw = new TATWactNtuPoint("twActPoint", fpNtuHitTw, fpNtuRecTw, fpParGeoTw, fpParConfTw, fpParCalTw);
   if (fFlagHisto)
     fActPointTw->CreateHistogram();
}

//__________________________________________________________
//! Create CAL reconstruction actions
void BaseReco::CreateRecActionCa()
{
   fpNtuClusCa = new TAGdataDsc("caClus", new TACAntuCluster());
   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

   if (fFlagMC)
      TACAactNtuCluster::DisableChargeThres();
   
   if (fCalClusterAlgo.Contains("Std") ) {
      if (fFlagMC)
         fActClusCa = new TACAactNtuCluster("caActClus", fpNtuHitCa, fpNtuClusCa, fpParGeoCa, 0x0, fpParConfCa, fpNtuRecTw);
      else
         fActClusCa = new TACAactNtuCluster("caActClus", fpNtuHitCa, fpNtuClusCa, fpParGeoCa, fpParCalCa, fpParConfCa, fpNtuRecTw);
   } else if (fCalClusterAlgo.Contains("Padme") ) {
      if (fFlagMC)
         fActClusCa = new TACAactNtuClusterP("caActClus", fpNtuHitCa, fpNtuClusCa, fpParGeoCa, 0x0, fpParConfCa, fpNtuRecTw);
      else
         fActClusCa = new TACAactNtuClusterP("caActClus", fpNtuHitCa, fpNtuClusCa, fpParGeoCa, fpParCalCa, fpParConfCa, fpNtuRecTw);
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
    fpNtuGlbTrack = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
    fActGlbTrack  = new TAGactNtuGlbTrack( "glbActTrack",
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
		fpNtuGlbTrack = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
		fActGlbkFitter = new TAGactKFitter("glbActKFitter", fpNtuGlbTrack);
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
      fpNtuGlbTrack = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
      fActGlbTrackS = new TAGactNtuGlbTrackS("glbActTrackS", fpNtuVtx, fpNtuClusIt, fpNtuRecMsd, fpNtuRecTw, fpNtuClusCa, fpNtuGlbTrack, fpParGeoVtx, fpParGeoIt, fpParGeoMsd, fpParGeoTw, fpParGeoG);
      
      if (fFlagHisto)
         fActGlbTrackS->CreateHistogram();
   }
}

//__________________________________________________________
//! Set L0 tree branches for reading back
void BaseReco::SetL0TreeBranches()
{
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) {
    fActEvtReader = new TAGactTreeReader("evtReader");
     
    if (!fgSaveMcFlag)
       if (!fFriendFileName.IsNull() && !fFriendTreeName.IsNull()) {
          fActEvtReader->AddFriendTree(fFriendFileName,fFriendTreeName);
          Info("SetL0TreeBranches()", "\n Open file %s for friend tree %s\n", fFriendFileName.Data(), fFriendTreeName.Data());
       }
     
    if (TAGrecoManager::GetPar()->IncludeBM())
       fActEvtReader->SetupBranch(fpNtuTrackBm, TABMntuTrack::GetBranchName());
   
    if (TAGrecoManager::GetPar()->IncludeVT()) {
      fActEvtReader->SetupBranch(fpNtuTrackVtx,  TAVTntuTrack::GetBranchName());
      fActEvtReader->SetupBranch(fpNtuClusVtx,   TAVTntuCluster::GetBranchName());
      fActEvtReader->SetupBranch(fpNtuVtx, TAVTntuVertex::GetBranchName());
    }

    if (TAGrecoManager::GetPar()->IncludeIT())
      fActEvtReader->SetupBranch(fpNtuClusIt,  TAITntuCluster::GetBranchName());

    if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fActEvtReader->SetupBranch(fpNtuClusMsd,  TAMSDntuCluster::GetBranchName());
      fActEvtReader->SetupBranch(fpNtuRecMsd,  TAMSDntuPoint::GetBranchName());
    }

    if(TAGrecoManager::GetPar()->IncludeTW())
      fActEvtReader->SetupBranch(fpNtuRecTw,  TATWntuPoint::GetBranchName());

    if(TAGrecoManager::GetPar()->IncludeCA())
      fActEvtReader->SetupBranch(fpNtuClusCa,  TACAntuCluster::GetBranchName());


    if (fReadL0Hits) {
       if (TAGrecoManager::GetPar()->IncludeST()) {
          if (fFlagMC) {
             fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
             fActEvtReader->SetupBranch(fpNtuMcSt,TAMCntuHit::GetStcBranchName());
          }
          fpNtuHitSt   = new TAGdataDsc("stNtu", new TASTntuHit());
          fActEvtReader->SetupBranch(fpNtuHitSt,TASTntuHit::GetBranchName());
       }

       if (TAGrecoManager::GetPar()->IncludeBM()) {
          if (fFlagMC) {
             fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
             fActEvtReader->SetupBranch(fpNtuMcBm,TAMCntuHit::GetBmBranchName());
          }
          fpNtuHitBm = new TAGdataDsc("bmNtu", new TABMntuHit());
          fActEvtReader->SetupBranch(fpNtuHitBm, TABMntuHit::GetBranchName());
       }

       if (TAGrecoManager::GetPar()->IncludeVT() && fFlagMC) {
          fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcVt,TAMCntuHit::GetVtxBranchName());
       }

       if (TAGrecoManager::GetPar()->IncludeIT() && fFlagMC) {
          fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcIt,TAMCntuHit::GetItrBranchName());
       }

       if (TAGrecoManager::GetPar()->IncludeMSD() && fFlagMC) {
          fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcMsd,TAMCntuHit::GetMsdBranchName());
       }

       if(TAGrecoManager::GetPar()->IncludeTW() && fFlagMC) {
          fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcTw,TAMCntuHit::GetTofBranchName());
       }

       if(TAGrecoManager::GetPar()->IncludeCA() && fFlagMC) {
          fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcCa,TAMCntuHit::GetCalBranchName());
       }
     }
  }
}

//__________________________________________________________
//! Set tree branches for writing in output file
void BaseReco::SetTreeBranches()
{
  if (TAGrecoManager::GetPar()->IncludeBM())
     if (fFlagTrack)
        fActEvtWriter->SetupElementBranch(fpNtuTrackBm, TABMntuTrack::GetBranchName());
   
   
  if (TAGrecoManager::GetPar()->IncludeVT()) {
    if (!fFlagTrack)
      fActEvtWriter->SetupElementBranch(fpNtuClusVtx, TAVTntuCluster::GetBranchName());
    else {
      fActEvtWriter->SetupElementBranch(fpNtuClusVtx, TAVTntuCluster::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuTrackVtx, TAVTntuTrack::GetBranchName());
      if (TAGrecoManager::GetPar()->IncludeTG())
         fActEvtWriter->SetupElementBranch(fpNtuVtx, TAVTntuVertex::GetBranchName());
    }
  }
  
  if (TAGrecoManager::GetPar()->IncludeIT()) {
     fActEvtWriter->SetupElementBranch(fpNtuClusIt, TAITntuCluster::GetBranchName());
     if (fFlagItrTrack && fFlagTrack)
        fActEvtWriter->SetupElementBranch(fpNtuTrackIt, TAITntuTrack::GetBranchName());
  }
   
  if (TAGrecoManager::GetPar()->IncludeMSD()) {
    fActEvtWriter->SetupElementBranch(fpNtuClusMsd, TAMSDntuCluster::GetBranchName());
     fActEvtWriter->SetupElementBranch(fpNtuRecMsd, TAMSDntuPoint::GetBranchName());
     if (fFlagMsdTrack && fFlagTrack)
        fActEvtWriter->SetupElementBranch(fpNtuTrackMsd, TAMSDntuTrack::GetBranchName());
  }
   if (TAGrecoManager::GetPar()->IncludeTW() && !TAGrecoManager::GetPar()->CalibTW())
      fActEvtWriter->SetupElementBranch(fpNtuRecTw, TATWntuPoint::GetBranchName());

   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) return;

   if (TAGrecoManager::GetPar()->IncludeCA())
     fActEvtWriter->SetupElementBranch(fpNtuClusCa, TACAntuCluster::GetBranchName());

   if (TAGrecoManager::GetPar()->IncludeStraight() && !TAGrecoManager::GetPar()->IncludeDI()) {
      if (fFlagTrack)
         fActEvtWriter->SetupElementBranch(fpNtuGlbTrack, TAGntuGlbTrack::GetBranchName());
   }
   
  if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) {
    if (fFlagTrack && !fFlagRecCutter)
      fActEvtWriter->SetupElementBranch(fpNtuGlbTrack, TAGntuGlbTrack::GetBranchName());
  }
}

//__________________________________________________________
//! Add required reconstruction actions in list
void BaseReco::AddRecRequiredItem()
{
   if (fFlagOut)
      gTAGroot->AddRequiredItem("locRecFile");
   
   if (TAGrecoManager::GetPar()->IncludeTOE() && TAGrecoManager::GetPar()->IsFromLocalReco()) {
     if (fFlagTrack) {
       if(!fFlagRecCutter)
          gTAGroot->AddRequiredItem("glbActTrack");
       else
          gTAGroot->AddRequiredItem("evtReader");
     }
     return;
   }

   // if (TAGrecoManager::GetPar()->IncludeKalman() && TAGrecoManager::GetPar()->IsFromLocalReco()) {
   //   if (fFlagTrack) {
   //     gTAGroot->AddRequiredItem("glbActKFitter");
   //   }
   //   return;
   // }

   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeBM())
      gTAGroot->AddRequiredItem("stActNtu");

   if (TAGrecoManager::GetPar()->IncludeBM()) {
      gTAGroot->AddRequiredItem("bmActNtu");
      if (fFlagTrack)
         gTAGroot->AddRequiredItem("bmActTrack");
   }

   if (TAGrecoManager::GetPar()->IncludeVT()) {
      gTAGroot->AddRequiredItem("vtActNtu");
      gTAGroot->AddRequiredItem("vtActClus");
      if (fFlagTrack) {
         gTAGroot->AddRequiredItem("vtActTrack");
         if (TAGrecoManager::GetPar()->IncludeTG())
            gTAGroot->AddRequiredItem("vtActVtx");
      }
   }

   if (TAGrecoManager::GetPar()->IncludeIT()) {
      gTAGroot->AddRequiredItem("itActClus");
      if (fFlagItrTrack && fFlagTrack)
         gTAGroot->AddRequiredItem("itActTrack");
   }

   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      gTAGroot->AddRequiredItem("msdActNtu");
      gTAGroot->AddRequiredItem("msdActClus");
      gTAGroot->AddRequiredItem("msdActPoint");
      if (fFlagMsdTrack && fFlagTrack)
         gTAGroot->AddRequiredItem("msdActTrack");
   }

   if (TAGrecoManager::GetPar()->IncludeTW() && !TAGrecoManager::GetPar()->CalibTW()) {
     gTAGroot->AddRequiredItem("twActNtu");
     gTAGroot->AddRequiredItem("twActPoint");
   }

   if (TAGrecoManager::GetPar()->IncludeCA()) {
      gTAGroot->AddRequiredItem("caActNtu");
      gTAGroot->AddRequiredItem("caActClus");
   }

   if (fFlagTrack) {
      if (TAGrecoManager::GetPar()->IncludeTOE() && !TAGrecoManager::GetPar()->IncludeKalman())
         gTAGroot->AddRequiredItem("glbActTrack");

      if (!TAGrecoManager::GetPar()->IncludeTOE() && TAGrecoManager::GetPar()->IncludeKalman()) {
         gTAGroot->AddRequiredItem("glbTrack");
         gTAGroot->AddRequiredItem("glbActKFitter");
      }
   }

   if (TAGrecoManager::GetPar()->IncludeStraight() && !TAGrecoManager::GetPar()->IncludeDI()) 
      gTAGroot->AddRequiredItem("glbActTrackS");
}
