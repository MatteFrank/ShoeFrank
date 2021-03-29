

#include "BaseReco.hxx"

// root include

#include "TAGroot.hxx"

#include "TAGactTreeReader.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGparGeo.hxx"

#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"

#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAVTactNtuHit.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuVertexPD.hxx"




#include "GlobalPar.hxx"

ClassImp(BaseReco)

Bool_t  BaseReco::fgItrTrackFlag  = false;

//__________________________________________________________
BaseReco::BaseReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : TNamed(fileNameIn.Data(), fileNameout.Data()),
   fExpName(expName),
   fRunNumber(runNumber),
   fpParTimeWD(0x0),
   fpParMapWD(0x0),
   fpParMapSt(0x0),
   fpParMapBm(0x0),
   fpParMapVtx(0x0),
   fpParMapIt(0x0),
   fpParMapTw(0x0),
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
   fpParCalTw(0x0),
   fpParCalCa(0x0),
   fpParGeoCa(0x0),
   fpParConfBm(0x0),
   fpParConfVtx(0x0),
   fpParConfIt(0x0),
   fpParConfMsd(0x0),
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
   fpNtuRecTw(0x0),
   fpNtuMcTrk(0x0),
   fpNtuTrackBm(0x0),
   fpNtuTrackVtx(0x0),
   fpNtuTrackIt(0x0),
   fpNtuVtx(0x0),
   fpNtuGlbTrack(0x0),
   fpNtuTrackIr(0x0),
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
   fActGlbTrack(0x0),
   fActTrackIr(0x0),
   fFlagOut(true),
   fFlagTree(false),
   fFlagHits(false),
   fFlagHisto(false),
   fFlagTrack(false),
   fFlagTWbarCalib(false),
   fgTrackingAlgo("Full"),
   fFlagZtrueMC(false),
   fFlagZrecPUoff(false),
   fFlagZmatch_TW(false),
   fFlagMC(false),
   fM28ClusMtFlag(false)
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
   
   // Save run info
   gTAGroot->SetRunNumber(fRunNumber);
   gTAGroot->SetCampaignName(fExpName);

   // activate per default Dipole, TGT, VTX and TW if TOE on
   if (GlobalPar::GetPar()->IncludeTOE()) {
      GlobalPar::GetPar()->IncludeDI(true);
      GlobalPar::GetPar()->IncludeTG(true);
      GlobalPar::GetPar()->IncludeVT(true);
      GlobalPar::GetPar()->IncludeTW(true);
   }

   if (fFlagOut)
     fActEvtWriter = new TAGactTreeWriter("locRecFile");
}

//__________________________________________________________
BaseReco::~BaseReco()
{
   // default destructor
   delete fTAGroot; // should delete all data, para and actions
}

//_____________________________________________________________________________
void BaseReco::CampaignChecks()
{
   // check detector include in FootGlobal.par vs current campaign
   vector<TString> list = GlobalPar::GetPar()->DectIncluded();
   for (vector<TString>::const_iterator it = list.begin(); it != list.end(); ++it) {
      TString str = *it;
      
      if (!fCampManager->IsDetectorOn(str)) {
         Error("CampaignChecks()", "the detector %s is NOT referenced in campaign file", str.Data());
         exit(0);
      }
   }
   
   // check run number vs current campaign
   TArrayI runArray = fCampManager->GetCurRunArray();
   Bool_t runOk = false;
   
   for (Int_t i = 0; i < runArray.GetSize(); ++i) {
      if (fRunNumber == runArray[i])
         runOk = true;
   }
   
   if (!runOk) {
      Error("CampaignChecks()", "run %d is NOT referenced in campaign file", fRunNumber);
      exit(0);
   }
  
  // Check MC file
  if (fFlagMC && fCampManager->GetCurCampaignPar().McFlag)
    Info("CampaignChecks()", "Reading MC data");

  if (fFlagMC && !fCampManager->GetCurCampaignPar().McFlag) {
    Error("CampaignChecks()", "Trying to read back MC data file while referenced as raw data in campaign file");
    exit(0);
  }
  
  if (!fFlagMC && fCampManager->GetCurCampaignPar().McFlag) {
    Error("CampaignChecks()", "Trying to read back raw data file while referenced as MC data in campaign file");
    exit(0);
  }
}

//__________________________________________________________
void BaseReco::GlobalChecks()
{
  if (GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) {
    // from global file
    Bool_t enableLocalRecoG = GlobalPar::GetPar()->IsLocalReco();
    Bool_t globalRecoTOE    = GlobalPar::GetPar()->IncludeTOE();
    Bool_t globalRecoGF     = GlobalPar::GetPar()->IncludeKalman();

    // from root file
    TAGrunInfo info = gTAGroot->CurrentRunInfo();
    TAGrunInfo* p = &info;
    if (!p) return; // if run info not found in MC file

    Bool_t enableLocalReco = info.GetGlobalPar().EnableLocalReco;
    
    if (enableLocalRecoG && enableLocalReco)
      Info("GlobalChecks()", "Make global reconstruction from L0 tree");
    
    if (globalRecoTOE)
      Info("GlobalChecks()", "Make global reconstruction with TOE");
    
    if (globalRecoGF)
      Info("GlobalChecks()", "Make global reconstruction with GenFit");

    if (enableLocalRecoG && !enableLocalReco) {
      Error("GlobalChecks()", "FootGlobal::enableLocalReco set but raw data found in root file !");
      exit(0);
    }
    
    if (!enableLocalRecoG && enableLocalReco) {
      Error("GlobalChecks()", "FootGlobal::enableLocalReco not set but L0 tree found in root file!");
      exit(0);
    }
  }
}

//__________________________________________________________
void BaseReco::BeforeEventLoop()
{
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
void BaseReco::LoopEvent(Int_t nEvents)
{
  Int_t frequency = 1;
  
  if (nEvents > 100000)      frequency = 100000;
  else if (nEvents > 10000)  frequency = 10000;
  else if (nEvents > 1000)   frequency = 1000;
  else if (nEvents > 100)    frequency = 100;
  else if (nEvents > 10)     frequency = 10;

  for (Int_t ientry = 0; ientry < nEvents; ientry++) {
    
    if(ientry % frequency == 0)
      cout<<" Loaded Event:: " << ientry << endl;
        
    if (!fTAGroot->NextEvent()) break;
    
    if (FootDebugLevel(1)) {
      if(fpNtuGlbTrack) {
        TAGntuGlbTrack *glbTrack =
        (TAGntuGlbTrack*) fpNtuGlbTrack->GenerateObject();
        
        int nTrk = glbTrack->GetTracksN();
        for(int iTr = 0; iTr< nTrk; iTr++) {
          TAGtrack *aTr = glbTrack->GetTrack(iTr);
          cout<<"  "<<aTr->GetMass()<<" "<<aTr->GetEnergy()<<" "<<aTr->GetMomentum()<<endl;
        }
      }
    }
  }
}

//__________________________________________________________
void BaseReco::AfterEventLoop()
{
   fTAGroot->EndEventLoop();
   if (fFlagOut)
      CloseFileOut();
   CloseFileIn();
}

//__________________________________________________________
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
void BaseReco::SetRecHistogramDir()
{
   //Global track
   if (fFlagTrack) {
     
      if (!GlobalPar::GetPar()->IncludeTOE() && GlobalPar::GetPar()->IncludeKalman()) {
        TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAGgeoTrafo::GetBaseName());
        fActGlbkFitter->SetHistogramDir(subfolder);
        if (GlobalPar::GetPar()->IsLocalReco()) return;
      }
     
      if (GlobalPar::GetPar()->IncludeTOE() && !GlobalPar::GetPar()->IncludeKalman()) {
         TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAGgeoTrafo::GetBaseName());
         fActGlbTrack->SetHistogramDir(subfolder);
         if (GlobalPar::GetPar()->IsLocalReco()) return;
      }
   }
   
   //BMN
   if (GlobalPar::GetPar()->IncludeBM()) {
      if (fFlagTrack) {
         TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TABMparGeo::GetBaseName());
         fActTrackBm->SetHistogramDir(subfolder);
      }
   }
   
   // VTX
   if (GlobalPar::GetPar()->IncludeVT()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TAVTparGeo::GetBaseName());
      fActClusVtx->SetHistogramDir(subfolder);
      
      if (fFlagTrack) {
         fActTrackVtx->SetHistogramDir(subfolder);
         if (GlobalPar::GetPar()->IncludeTG()) {
            fActVtx->SetHistogramDir(subfolder);
         }
      }
   }
   
   // IT
   if (GlobalPar::GetPar()->IncludeIT()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TAITparGeo::GetBaseName());
      fActClusIt->SetHistogramDir(subfolder);
   }
   
   // MSD
   if (GlobalPar::GetPar()->IncludeMSD()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TAMSDparGeo::GetBaseName());
      fActClusMsd->SetHistogramDir(subfolder);
   }

   // TW
   if (GlobalPar::GetPar()->IncludeTW() && !GlobalPar::GetPar()->CalibTW()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TATWparGeo::GetBaseName());
      fActPointTw->SetHistogramDir(subfolder);
   }
   
   // CA
   if (GlobalPar::GetPar()->IncludeCA()) {
      TDirectory* subfolder = (TDirectory*)(fActEvtWriter->File())->Get(TACAparGeo::GetBaseName());
      fActClusCa->SetHistogramDir(subfolder);
   }
}

//__________________________________________________________
void BaseReco::CloseFileOut()
{
   // saving current run info
   GlobalPar::GetPar()->EnableLocalReco();
   TAGrunInfo info = GlobalPar::GetPar()->GetGlobalInfo();
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
   if (GlobalPar::GetPar()->IncludeTG() || GlobalPar::GetPar()->IncludeBM() || GlobalPar::GetPar()->IncludeTW() || GlobalPar::GetPar()->IncludeCA() || IsItrTracking()) {
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
   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW()|| GlobalPar::GetPar()->IncludeBM()) {

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
         parFileName = fCampManager->GetCurCalFile(TASTparGeo::GetBaseName(), fRunNumber);
         parTimeWD->FromFile(parFileName.Data());
      }
   }

   // initialise par files for Beam Monitor
   if (GlobalPar::GetPar()->IncludeBM()) {
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
   if (GlobalPar::GetPar()->IncludeDI() || GlobalPar::GetPar()->IsLocalReco() ) {
      fpParGeoDi = new TAGparaDsc(TADIparGeo::GetDefParaName(), new TADIparGeo());
      TADIparGeo* parGeo = (TADIparGeo*)fpParGeoDi->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TADIparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      if (GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman())
         fField = new TADIgeoField(parGeo);
   }
   
   // initialise par files for vertex
   if (GlobalPar::GetPar()->IncludeVT() || GlobalPar::GetPar()->IsLocalReco()) {
      fpParGeoVtx = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
      TAVTparGeo* parGeo = (TAVTparGeo*)fpParGeoVtx->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      fpParConfVtx = new TAGparaDsc("vtConf", new TAVTparConf());
      TAVTparConf* parConf = (TAVTparConf*)fpParConfVtx->Object();
      parFileName = fCampManager->GetCurConfFile(TAVTparGeo::GetBaseName(), fRunNumber);
      parConf->FromFile(parFileName.Data());
      
      if(!fFlagMC) {
         fpParMapVtx = new TAGparaDsc("vtMap", new TAVTparMap());
         TAVTparMap* parMap = (TAVTparMap*)fpParMapVtx->Object();
         parFileName = fCampManager->GetCurMapFile(TAVTparGeo::GetBaseName(), fRunNumber);
         parMap->FromFile(parFileName.Data());
      }
   }
   
   // initialise par files for inner tracker
   if (GlobalPar::GetPar()->IncludeIT() || GlobalPar::GetPar()->IsLocalReco()) {
      fpParGeoIt = new TAGparaDsc(TAITparGeo::GetDefParaName(), new TAITparGeo());
      TAITparGeo* parGeo = (TAITparGeo*)fpParGeoIt->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TAITparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      fpParConfIt = new TAGparaDsc("itConf", new TAITparConf());
      TAITparConf* parConf = (TAITparConf*)fpParConfIt->Object();
      parFileName = fCampManager->GetCurConfFile(TAITparGeo::GetBaseName(), fRunNumber);
      parConf->FromFile(parFileName.Data());

      if(!fFlagMC) {
         fpParMapIt = new TAGparaDsc("itMap", new TAITparMap());
         TAITparMap* parMap = (TAITparMap*)fpParMapIt->Object();
         parFileName = fCampManager->GetCurMapFile(TAITparGeo::GetBaseName(), fRunNumber);
         // parMap->FromFile(parFileName.Data());
      }
   }

   // initialise par files for multi strip detector
   if (GlobalPar::GetPar()->IncludeMSD() || GlobalPar::GetPar()->IsLocalReco()) {
      fpParGeoMsd = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
      TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      fpParConfMsd = new TAGparaDsc("msdConf", new TAMSDparConf());
      TAMSDparConf* parConf = (TAMSDparConf*)fpParConfMsd->Object();
//      parFileName = fCampManager->GetCurConfFile(TAMSDparGeo::GetBaseName(), fRunNumber);
//      parConf->FromFile(parFileName.Data());
   }

   // initialise par files for Tof Wall
   if (GlobalPar::GetPar()->IncludeTW() || GlobalPar::GetPar()->IsLocalReco()) {
      fpParGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
      TATWparGeo* parGeo = (TATWparGeo*)fpParGeoTw->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TATWparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      fpParCalTw = new TAGparaDsc("twCal", new TATWparCal());
      TATWparCal* parCal = (TATWparCal*)fpParCalTw->Object();
      Bool_t isTof_calib = false;

      if(fFlagTWbarCalib) {
         parFileName = fCampManager->GetCurCalFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                   isTof_calib,fFlagTWbarCalib);
         parCal->FromCalibFile(parFileName.Data(),isTof_calib,fFlagTWbarCalib);
      } else {
         parFileName = fCampManager->GetCurCalFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                   isTof_calib,fFlagTWbarCalib);
         parCal->FromCalibFile(parFileName.Data(),isTof_calib,fFlagTWbarCalib);

         Bool_t elossTuning = false;

         if(!fFlagMC)
            elossTuning = true;
	
         if(elossTuning) {
            Info("ReadParFiles()","Eloss tuning for GSI data status:: ON\n");
            parFileName = fCampManager->GetCurCalFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                      false, false, true);
            parCal->FromElossTuningFile(parFileName.Data());
         }
      }
      
      isTof_calib = true;
      if(fFlagTWbarCalib) {
         parFileName = fCampManager->GetCurCalFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                   isTof_calib,fFlagTWbarCalib);
         parCal->FromCalibFile(parFileName.Data(),isTof_calib,fFlagTWbarCalib);
      } else {
         parFileName = fCampManager->GetCurCalFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                   isTof_calib,fFlagTWbarCalib);
         parCal->FromCalibFile(parFileName.Data(),isTof_calib,fFlagTWbarCalib);
      }

      parFileName = fCampManager->GetCurConfFile(TATWparGeo::GetBaseName(), fRunNumber,
                                                 Form("%d%s", A_beam,ion_name.Data()),
                                                 (int)(kinE_beam*TAGgeoTrafo::GevToMev()));
      if (parFileName.IsNull() && fFlagMC) {
         fFlagZtrueMC = true;
         Warning("ReadParFiles()", "BB parametrization file does not exist for %d%s at %d MeV switch to true MC Z\n", A_beam, ion_name.Data(), (int)(kinE_beam*TAGgeoTrafo::GevToMev()));
      } else
         parCal->FromFileZID(parFileName.Data(),Z_beam);

      if(fFlagMC) { // set in MC threshold and active bars from data informations
         parFileName = fCampManager->GetCurMapFile(TATWparGeo::GetBaseName(), fRunNumber);
         parCal->FromBarStatusFile(parFileName.Data());
      }
   }
   
   // initialise par files for caloriomter
   if (GlobalPar::GetPar()->IncludeCA()) {
      fpParGeoCa = new TAGparaDsc(TACAparGeo::GetDefParaName(), new TACAparGeo());
      TACAparGeo* parGeo = (TACAparGeo*)fpParGeoCa->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TACAparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName);
      
      fpParMapCa = new TAGparaDsc("caMap", new TACAparMap());
      TACAparMap* parMap = (TACAparMap*)fpParMapCa->Object();

      fpParCalCa = new TAGparaDsc("caCal", new TACAparCal(parMap));
      TACAparCal* parCal = (TACAparCal*)fpParCalCa->Object();
      
     
     if(fFlagMC) { // set in MC threshold and active crystals from data informations
       parFileName = fCampManager->GetCurMapFile(TACAparGeo::GetBaseName(), fRunNumber);
       parCal->FromCrysStatusFile(parFileName.Data());
     } else {
              
        parFileName = fCampManager->GetCurMapFile(TACAparGeo::GetBaseName(), fRunNumber);
        parMap->FromFile(parFileName.Data());
        cout << "nCryMap: " << parMap->GetCrystalsN() << endl;

        parFileName = fCampManager->GetCurCalFile(TACAparGeo::GetBaseName(), fRunNumber);
        parCal->FromCalibTempFile(parFileName.Data());
     }
   }

   TAVTparConf::SetHistoMap();
}

//__________________________________________________________
void BaseReco::CreateRecAction()
{
   if (GlobalPar::GetPar()->IncludeBM())
      CreateRecActionBm();
   
   if (GlobalPar::GetPar()->IncludeVT())
      CreateRecActionVtx();
   
    
   if (GlobalPar::GetPar()->IncludeIT())
      CreateRecActionIt();
   
   if (GlobalPar::GetPar()->IncludeMSD())
      CreateRecActionMsd();
   
   if (GlobalPar::GetPar()->IncludeTW() && !GlobalPar::GetPar()->CalibTW())
      CreateRecActionTw();

   if (GlobalPar::GetPar()->IncludeCA())
      CreateRecActionCa();
    
   if (GlobalPar::GetPar()->IncludeTOE() && !GlobalPar::GetPar()->IncludeKalman())
      CreateRecActionGlb();
   
   if (!GlobalPar::GetPar()->IncludeTOE() && GlobalPar::GetPar()->IncludeKalman())
      CreateRecActionGlbGF();
   
   if (GlobalPar::GetPar()->IncludeST() && GlobalPar::GetPar()->IncludeTG() &&
       GlobalPar::GetPar()->IncludeBM() && GlobalPar::GetPar()->IncludeVT() &&
       GlobalPar::GetPar()->IncludeIT() && !GlobalPar::GetPar()->IncludeDI())
       CreateRecActionIr();
}

//__________________________________________________________
void BaseReco::CreateRecActionBm()
{
   if(fFlagTrack) {
     if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) return;
     fpNtuTrackBm = new TAGdataDsc("bmTrack", new TABMntuTrack());

     fActTrackBm  = new TABMactNtuTrack("bmActTrack", fpNtuTrackBm, fpNtuHitBm, fpParGeoBm, fpParConfBm, fpParCalBm);
      if (fFlagHisto)
         fActTrackBm->CreateHistogram();
   }
}

//__________________________________________________________
void BaseReco::CreateRecActionVtx()
{
   if(fFlagTrack) {
      fpNtuTrackVtx = new TAGdataDsc("vtTrack", new TAVTntuTrack());
      if (GlobalPar::GetPar()->IncludeTG() || GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman())
         fpNtuVtx      = new TAGdataDsc("vtVtx",   new TAVTntuVertex());
   }
   
   fpNtuClusVtx  = new TAGdataDsc("vtClus", new TAVTntuCluster());
  if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) return;

   if (fM28ClusMtFlag)
      fActClusVtx   = new TAVTactNtuClusterMT("vtActClus", fpNtuHitVtx, fpNtuClusVtx, fpParConfVtx, fpParGeoVtx);
   else
      fActClusVtx   = new TAVTactNtuClusterF("vtActClus", fpNtuHitVtx, fpNtuClusVtx, fpParConfVtx, fpParGeoVtx);

   if (fFlagHisto)
      fActClusVtx->CreateHistogram();
   
   if (fFlagTrack) {
      if (fgTrackingAlgo.Contains("Std") ) {
         if (GlobalPar::GetPar()->IncludeBM())
            fActTrackVtx  = new TAVTactNtuTrack("vtActTrack", fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx, 0, fpNtuTrackBm);
         else
            fActTrackVtx  = new TAVTactNtuTrack("vtActTrack", fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx);
      } else if (fgTrackingAlgo.Contains("Full")) {
         if (GlobalPar::GetPar()->IncludeBM())
            fActTrackVtx  = new TAVTactNtuTrackF("vtActTrack", fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx, 0, fpNtuTrackBm);
         else
            fActTrackVtx  = new TAVTactNtuTrackF("vtActTrack", fpNtuClusVtx, fpNtuTrackVtx, fpParConfVtx, fpParGeoVtx);
      } else {
         Error("CreateRecActionVtx()", "No Tracking algorithm defined !");
      }
      
      if (fFlagHisto)
         fActTrackVtx->CreateHistogram();
      
      if (GlobalPar::GetPar()->IncludeTG()) {
         if(GlobalPar::GetPar()->IncludeBM()) {
            fActVtx = new TAVTactNtuVertexPD("vtActVtx", fpNtuTrackVtx, fpNtuVtx, fpParConfVtx, fpParGeoVtx, fpParGeoG, fpNtuTrackBm);
            
         } else
            fActVtx = new TAVTactNtuVertexPD("vtActVtx", fpNtuTrackVtx, fpNtuVtx, fpParConfVtx, fpParGeoVtx, fpParGeoG);
         
         if (fFlagHisto)
            fActVtx->CreateHistogram();
      }
   }
}

//__________________________________________________________
void BaseReco::CreateRecActionIt()
{
   fpNtuClusIt  = new TAGdataDsc("itClus", new TAITntuCluster());
  if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) return;

   if (fM28ClusMtFlag)
      fActClusIt   = new TAITactNtuClusterMT("itActClus", fpNtuHitIt, fpNtuClusIt, fpParConfIt, fpParGeoIt);
   else
      fActClusIt   = new TAITactNtuClusterF("itActClus", fpNtuHitIt, fpNtuClusIt, fpParConfIt, fpParGeoIt);

   if (fFlagHisto)
     fActClusIt->CreateHistogram();
   
   if (fgItrTrackFlag) {
      fpNtuTrackIt  = new TAGdataDsc("itTrack", new TAITntuTrack());

      if (fgTrackingAlgo.Contains("Std") ) {
         if (GlobalPar::GetPar()->IncludeBM())
            fActTrackIt  = new TAITactNtuTrack("itActTrack", fpNtuClusIt, fpNtuTrackIt, fpParConfIt, fpParGeoIt, 0x0, fpNtuTrackBm);
         else
            fActTrackIt  = new TAITactNtuTrack("itActTrack", fpNtuClusIt, fpNtuTrackIt, fpParConfIt, fpParGeoIt);
         
      }  else if (fgTrackingAlgo.Contains("Full")) {
         fActTrackIt  = new TAITactNtuTrackF("itActTrack", fpNtuClusIt, fpNtuTrackIt, fpParConfIt, fpParGeoIt, 0x0, fpParGeoG);
      }
      
      if (fFlagHisto)
         fActTrackIt->CreateHistogram();
   }
}

//__________________________________________________________
void BaseReco::CreateRecActionMsd()
{
   fpNtuClusMsd  = new TAGdataDsc("msdClus", new TAMSDntuCluster());
   if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) return;

   fActClusMsd   = new TAMSDactNtuCluster("msdActClus", fpNtuHitMsd, fpNtuClusMsd, fpParConfMsd, fpParGeoMsd);
   if (fFlagHisto)
      fActClusMsd->CreateHistogram();
   
   if (GlobalPar::GetPar()->IncludeKalman() ){
      fpNtuRecMsd   = new TAGdataDsc("msdPoint", new TAMSDntuPoint());
      fActPointMsd  = new TAMSDactNtuPoint("msdActPoint", fpNtuHitMsd, fpNtuRecMsd, fpParGeoMsd);
   }
}

//__________________________________________________________
void BaseReco::CreateRecActionTw()
{
   fpNtuRecTw  = new TAGdataDsc("twPoint", new TATWntuPoint());
   if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) return;

   fActPointTw = new TATWactNtuPoint("twActPoint", fpNtuHitTw, fpNtuRecTw, fpParGeoTw, fpParCalTw,fFlagZmatch_TW,fFlagZtrueMC);
   if (fFlagHisto)
     fActPointTw->CreateHistogram();
}

//__________________________________________________________
void BaseReco::CreateRecActionCa()
{
   fpNtuClusCa  = new TAGdataDsc("caClus", new TACAntuCluster());
   if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) return;

   fActClusCa   = new TACAactNtuCluster("caActClus", fpNtuHitCa, fpNtuClusCa, fpParGeoCa, 0x0, fpNtuRecTw);
   if (fFlagHisto)
      fActClusCa->CreateHistogram();
}

//__________________________________________________________
void BaseReco::CreateRecActionGlb()
{

  if(fFlagTrack) {
    SetL0TreeBranches();
    fpNtuGlbTrack = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
    fActGlbTrack  = new TAGactNtuGlbTrack( "glbActTrack",
					   fpNtuClusVtx,
					   fpNtuTrackVtx,
					   fpNtuVtx,
					   fpNtuClusIt,
					   fpNtuClusMsd,
					   fpNtuRecTw,
					   fpNtuGlbTrack,
					   fpParGeoG,
					   fpParGeoDi,
					   fpParGeoVtx,
					   fpParGeoIt,
					   fpParGeoMsd,
					   fpParGeoTw,
					   fField );
    if (fFlagHisto)
      fActGlbTrack->CreateHistogram();
  }
  
}

//__________________________________________________________
void BaseReco::CreateRecActionGlbGF()
{
  if(fFlagTrack) {
    SetL0TreeBranches();
    
    //   if (fFlagMC) {
    //     if (GlobalPar::GetPar()->IncludeST()) {
    //       fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
    //       fActEvtReader->SetupBranch(fpNtuMcSt,TAMCntuHit::GetStcBranchName());
    //     }
    
    //     if (GlobalPar::GetPar()->IncludeBM()) {
    //       fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
    //       fActEvtReader->SetupBranch(fpNtuMcBm,TAMCntuHit::GetBmBranchName());
    //     }
    
    //     if (GlobalPar::GetPar()->IncludeVT()) {
    //       fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
    //       fActEvtReader->SetupBranch(fpNtuMcVt,TAMCntuHit::GetVtxBranchName());
    //     }
    
    //     if (GlobalPar::GetPar()->IncludeIT()) {
    //       fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
    //       fActEvtReader->SetupBranch(fpNtuMcIt,TAMCntuHit::GetItrBranchName());
    //     }
    
    //     if (GlobalPar::GetPar()->IncludeMSD()) {
    //       fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
    //       fActEvtReader->SetupBranch(fpNtuMcMsd,TAMCntuHit::GetMsdBranchName());
    //     }
    
    //     if(GlobalPar::GetPar()->IncludeTW()) {
    //       fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
    //       fActEvtReader->SetupBranch(fpNtuMcTw,TAMCntuHit::GetTofBranchName());
    //     }
    
    //     if(GlobalPar::GetPar()->IncludeCA()) {
    //       fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
    //       fActEvtReader->SetupBranch(fpNtuMcCa,TAMCntuHit::GetCalBranchName());
    //     }
    //   }
    
    genfit::FieldManager::getInstance()->init( new TADIgenField(fField) );
    
    // set material and geometry into genfit
    MaterialEffects* materialEffects = MaterialEffects::getInstance();
    materialEffects->init(new TGeoMaterialInterface());
    
    // include the nucleon into the genfit pdg repository
    TAMCparTools::UpdatePDG();
    
    // study for kalman Filter
    // fActGlbTrackStudies = new GlobalTrackingStudies("glbActTrackStudyGF");
    // if (fFlagHisto)
    //    fActGlbTrackStudies->CreateHistogram();
    
    // Initialisation of KFfitter
    fpNtuGlbTrackK = new TAGdataDsc("TAGntuGlbTrackK", new TAGntuGlbTrackK());
    fActGlbkFitter = new TAGactKFitter("glbAct_KFitter", fpNtuGlbTrackK);
    if (fFlagHisto)
      fActGlbkFitter->CreateHistogram();
    
  }
}

//__________________________________________________________
void BaseReco::CreateRecActionIr()
{
   if(fFlagTrack) {
      fpNtuTrackIr = new TAGdataDsc("irTrack", new TAIRntuTrack());
      fActTrackIr  = new TAIRactNtuTrack("irActTrack", fpNtuClusIt, fpNtuVtx, fpNtuTrackIr, fpParConfIt, fpParGeoIt, fpParGeoVtx);
      if (fFlagHisto)
         fActTrackIr->CreateHistogram();
   }
   
}

//__________________________________________________________
void BaseReco::SetL0TreeBranches()
{
  if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) {
    fActEvtReader = new TAGactTreeReader("evtReader");
    
    if (GlobalPar::GetPar()->IncludeVT()) {
      fActEvtReader->SetupBranch(fpNtuTrackVtx,  TAVTntuTrack::GetBranchName());
      fActEvtReader->SetupBranch(fpNtuClusVtx,   TAVTntuCluster::GetBranchName());
      fActEvtReader->SetupBranch(fpNtuVtx, TAVTntuVertex::GetBranchName());
    }
    
    if (GlobalPar::GetPar()->IncludeIT())
      fActEvtReader->SetupBranch(fpNtuClusIt,  TAITntuCluster::GetBranchName());
    
    if (GlobalPar::GetPar()->IncludeMSD())
      fActEvtReader->SetupBranch(fpNtuClusMsd,  TAMSDntuCluster::GetBranchName());
    
    if(GlobalPar::GetPar()->IncludeTW())
      fActEvtReader->SetupBranch(fpNtuRecTw,  TATWntuPoint::GetBranchName());
    
    if(GlobalPar::GetPar()->IncludeCA())
      fActEvtReader->SetupBranch(fpNtuClusCa,  TACAntuCluster::GetBranchName());
  }
}

//__________________________________________________________
void BaseReco::SetTreeBranches()
{
  if (GlobalPar::GetPar()->IncludeTOE()) {
    if (fFlagTrack) {
      fActEvtWriter->SetupElementBranch(fpNtuGlbTrack, TAGntuGlbTrack::GetBranchName());
    }
  }
  
  if (GlobalPar::GetPar()->IncludeVT()) {
    if (!fFlagTrack)
      fActEvtWriter->SetupElementBranch(fpNtuClusVtx, TAVTntuCluster::GetBranchName());
    else {
      fActEvtWriter->SetupElementBranch(fpNtuClusVtx, TAVTntuCluster::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuTrackVtx, TAVTntuTrack::GetBranchName());
      if (GlobalPar::GetPar()->IncludeTG())
	fActEvtWriter->SetupElementBranch(fpNtuVtx, TAVTntuVertex::GetBranchName());
    }
  }
  
  if (GlobalPar::GetPar()->IncludeIT())
    fActEvtWriter->SetupElementBranch(fpNtuClusIt, TAITntuCluster::GetBranchName());
  
  if (GlobalPar::GetPar()->IncludeMSD())
    fActEvtWriter->SetupElementBranch(fpNtuClusMsd, TAMSDntuCluster::GetBranchName());
  
   if (GlobalPar::GetPar()->IncludeTW() && !GlobalPar::GetPar()->CalibTW())
     fActEvtWriter->SetupElementBranch(fpNtuRecTw, TATWntuPoint::GetBranchName());
   
   if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) return;

   if (GlobalPar::GetPar()->IncludeBM()) {
     if (fFlagTrack)
       fActEvtWriter->SetupElementBranch(fpNtuTrackBm, TABMntuTrack::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeCA())
     fActEvtWriter->SetupElementBranch(fpNtuClusCa, TACAntuCluster::GetBranchName());
}

//__________________________________________________________
void BaseReco::AddRecRequiredItem()
{
   if (fFlagOut)
      gTAGroot->AddRequiredItem("locRecFile");
   if (GlobalPar::GetPar()->IncludeTOE() && GlobalPar::GetPar()->IsLocalReco()) {
     if (fFlagTrack) {
       gTAGroot->AddRequiredItem("glbActTrack");
     }
     return;
   }
  
   if (GlobalPar::GetPar()->IncludeKalman() && GlobalPar::GetPar()->IsLocalReco()) {
     if (fFlagTrack) {
       gTAGroot->AddRequiredItem("glbActTrackStudyGF");
       gTAGroot->AddRequiredItem("glbActTrackGF");
     }
     return;
   }
  
   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeBM())
      gTAGroot->AddRequiredItem("stActNtu");
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      gTAGroot->AddRequiredItem("bmActNtu");
      if (fFlagTrack)
         gTAGroot->AddRequiredItem("bmActTrack");
   }
   
   if (GlobalPar::GetPar()->IncludeVT()) {
      gTAGroot->AddRequiredItem("vtActNtu");
      gTAGroot->AddRequiredItem("vtActClus");
      if (fFlagTrack) {
         gTAGroot->AddRequiredItem("vtActTrack");
         if (GlobalPar::GetPar()->IncludeTG())
            gTAGroot->AddRequiredItem("vtActVtx");
      }
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
      gTAGroot->AddRequiredItem("itActNtu");
      gTAGroot->AddRequiredItem("itActClus");
      if (fgItrTrackFlag)
         gTAGroot->AddRequiredItem("itActTrack");
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      gTAGroot->AddRequiredItem("msdActNtu");
      gTAGroot->AddRequiredItem("msdActClus");
      if (GlobalPar::GetPar()->IncludeKalman())
         gTAGroot->AddRequiredItem("msdActPoint");
   }
   
   if (GlobalPar::GetPar()->IncludeTW() && !GlobalPar::GetPar()->CalibTW()) {
     gTAGroot->AddRequiredItem("twActNtu");
     gTAGroot->AddRequiredItem("twActPoint");
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
      gTAGroot->AddRequiredItem("caActNtu");
      gTAGroot->AddRequiredItem("caActClus");
   }
   
   if (fFlagTrack) {
      if (GlobalPar::GetPar()->IncludeTOE() && !GlobalPar::GetPar()->IncludeKalman())
         gTAGroot->AddRequiredItem("glbActTrack");
      
      if (!GlobalPar::GetPar()->IncludeTOE() && GlobalPar::GetPar()->IncludeKalman()) {
         gTAGroot->AddRequiredItem("glbActTrackStudyGF");
         gTAGroot->AddRequiredItem("glbActTrackGF");
      }
   }
   
   if (GlobalPar::GetPar()->IncludeST() && GlobalPar::GetPar()->IncludeTG() &&
       GlobalPar::GetPar()->IncludeBM() && GlobalPar::GetPar()->IncludeVT() &&
       GlobalPar::GetPar()->IncludeIT() && !GlobalPar::GetPar()->IncludeDI())
      gTAGroot->AddRequiredItem("irActTrack");

}

//__________________________________________________________
void BaseReco::SetVtxTrackingAlgo(char c)
{
   switch (c) {
      case 'S':
         fgTrackingAlgo = "Std";
         break;
      case 'F':
         fgTrackingAlgo = "Full";
         break;
      default:
         printf("SetVtxTrackingAlgo: Wrongly set tracking algorithm");
   }
}
