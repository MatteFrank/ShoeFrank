

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

#include "TAVTactNtuRaw.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuVertexPD.hxx"




#include "GlobalPar.hxx"

ClassImp(BaseReco)

Bool_t  BaseReco::fgItrTrackFlag  = false;

//__________________________________________________________
BaseReco::BaseReco(TString expName, TString fileNameIn, TString fileNameout)
 : TNamed(fileNameIn.Data(), fileNameout.Data()),
   fExpName(expName),
   fRunNumber(-1),
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
   fpParCalTw(0x0),
   fpParGeoCa(0x0),
   fpParConfBm(0x0),
   fpParConfVtx(0x0),
   fpParConfIt(0x0),
   fpParConfMsd(0x0),
   fpDatRawSt(0x0),
   fpNtuRawSt(0x0),
   fpDatRawBm(0x0),
   fpNtuRawBm(0x0),
   fpNtuRawVtx(0x0),
   fpNtuRawIt(0x0),
   fpDatRawMsd(0x0),
   fpNtuRawMsd(0x0),
   fpDatRawTw(0x0),
   fpNtuRawTw(0x0),
   fpNtuRawCa(0x0),
   fpNtuClusVtx(0x0),
   fpNtuClusIt(0x0),
   fpNtuClusMsd(0x0),
   fpNtuRecTw(0x0),
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
   if (fFlagOut)
      fActEvtWriter = new TAGactTreeWriter("locRecFile");

   // Read Trafo file
   fpFootGeo = new TAGgeoTrafo();
   TString parFileName = Form("./geomaps/%sFOOT.geo", fExpName.Data());
   fpFootGeo->FromFile(parFileName);
   
   // actvate debug level
   GlobalPar::GetPar()->SetDebugLevels();

   // activate per default Dipole, TGT, VTX, IT and TW if TOE on
   if (GlobalPar::GetPar()->IncludeTOE()) {
      GlobalPar::GetPar()->IncludeDI(true);
      GlobalPar::GetPar()->IncludeTG(true);
      GlobalPar::GetPar()->IncludeVT(true);
      GlobalPar::GetPar()->IncludeIT(true);
      GlobalPar::GetPar()->IncludeTW(true);
   }
   
   // load campaign file (check if experiment name is in database
   fCampManager = new TAGcampaignManager(expName);
   fCampManager->FromFile();
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
}

//__________________________________________________________
void BaseReco::BeforeEventLoop()
{
   ReadParFiles();
   CreateRawAction();
   CreateRecAction();

    
   OpenFileIn();
   SetRunNumber();
 //  CampaignChecks();

   AddRawRequiredItem();
   AddRecRequiredItem();
   
    
   if (fFlagOut)
      OpenFileOut();
   
    
   fTAGroot->BeginEventLoop();
   fTAGroot->Print();
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
   if (fFlagTree)
      SetTreeBranches();
   
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
      if (GlobalPar::GetPar()->IncludeTOE() && !GlobalPar::GetPar()->IncludeKalman()) {
         fActGlbTrack->SetHistogramDir((TDirectory*)fActEvtWriter->File());
         if (TAGactNtuGlbTrack::GetStdAloneFlag()) return;
      }
   }
   
   //BMN
   if (GlobalPar::GetPar()->IncludeBM()) {
     if (fFlagTrack) 
       fActTrackBm->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   }
   
   // VTX
   if (GlobalPar::GetPar()->IncludeVT()) {
      fActClusVtx->SetHistogramDir((TDirectory*)fActEvtWriter->File());
      
      if (fFlagTrack) {
         fActTrackVtx->SetHistogramDir((TDirectory*)fActEvtWriter->File());
         if (GlobalPar::GetPar()->IncludeTG()) 
            fActVtx->SetHistogramDir((TDirectory*)fActEvtWriter->File());
      }
   }
   
   // IT
   if (GlobalPar::GetPar()->IncludeIT())
      fActClusIt->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   
   
   // MSD
   if (GlobalPar::GetPar()->IncludeMSD()) 
      fActClusMsd->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   
   if (GlobalPar::GetPar()->IncludeTOE())
   {
       fActGlbTrack->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   }
    
}

//__________________________________________________________
void BaseReco::CloseFileOut()
{
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
   
   // initialise par files for target
   if (GlobalPar::GetPar()->IncludeTG() || GlobalPar::GetPar()->IncludeBM() || GlobalPar::GetPar()->IncludeTW() || IsItrTracking()) {
      fpParGeoG = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
      TAGparGeo* parGeo = (TAGparGeo*)fpParGeoG->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TAGparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      Z_beam = parGeo->GetBeamPar().AtomicNumber;
      A_beam = parGeo->GetBeamPar().AtomicMass;
      ion_name = parGeo->GetBeamPar().Material;
      kinE_beam = parGeo->GetBeamPar().Energy; //GeV/u
      
      cout<<"\n!!!!  ATTENTION ATTENTION ATTENTION !!!"<<endl;
      printf("In file %s the following beam parameters for a %d%s beam have been set:\n",parFileName.Data(),A_beam,ion_name.Data());
      printf("BeamEnergy:          %.3f GeV/u\n",kinE_beam);
      printf("BeamAtomicMass:      %d\n",A_beam);
      printf("BeamAtomicNumber:    %d\n",Z_beam);
      printf("BeamMaterial:       \"%s\"\n",ion_name.Data());
      printf("Change such parameters in %s accordingly to the input file\n\n",parFileName.Data());

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
      
      fpParConfBm = new TAGparaDsc("bmConf", new TABMparCon());
      TABMparCon* parConf = (TABMparCon*)fpParConfBm->Object();
      parFileName = fCampManager->GetCurConfFile(TABMparGeo::GetBaseName(), fRunNumber);
      parConf->FromFile(parFileName.Data());
      
      if(!fFlagMC){
        parFileName = fCampManager->GetCurCalFile(TABMparGeo::GetBaseName(), fRunNumber);
        parConf->loadT0s(parFileName);
      
        fpParMapBm = new TAGparaDsc("bmMap", new TABMparMap());
        TABMparMap*  parMapBm = (TABMparMap*)fpParMapBm->Object();
        parFileName = fCampManager->GetCurMapFile(TABMparGeo::GetBaseName(), fRunNumber);
        parMapBm->FromFile(parFileName.Data(), parGeo);
      }
   }
   
   // initialise par files dipole
   if (GlobalPar::GetPar()->IncludeDI() || TAGactNtuGlbTrack::GetStdAloneFlag() ) {
      fpParGeoDi = new TAGparaDsc(TADIparGeo::GetDefParaName(), new TADIparGeo());
      TADIparGeo* parGeo = (TADIparGeo*)fpParGeoDi->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TADIparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      if (GlobalPar::GetPar()->IncludeTOE())
         fField = new TADIgeoField(parGeo);
   }
   
   // initialise par files for vertex
   if (GlobalPar::GetPar()->IncludeVT() || TAGactNtuGlbTrack::GetStdAloneFlag()) {
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
   if (GlobalPar::GetPar()->IncludeIT() || TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fpParGeoIt = new TAGparaDsc(TAITparGeo::GetItDefParaName(), new TAITparGeo());
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
   if (GlobalPar::GetPar()->IncludeMSD() || TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fpParGeoMsd = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
      TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
      TString parFileName = fCampManager->GetCurGeoFile(TAITparGeo::GetBaseName(), fRunNumber);
      parGeo->FromFile(parFileName.Data());
      
      fpParConfMsd = new TAGparaDsc("msdConf", new TAMSDparConf());
      TAMSDparConf* parConf = (TAMSDparConf*)fpParConfMsd->Object();
      parFileName = fCampManager->GetCurConfFile(TAMSDparGeo::GetBaseName(), fRunNumber);
      // parConf->FromFile(parFileName.Data());
   }
   
   // initialise par files for Tof Wall
   if (GlobalPar::GetPar()->IncludeTW() || TAGactNtuGlbTrack::GetStdAloneFlag()) {
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
   
   if (GlobalPar::GetPar()->IncludeTW())
      CreateRecActionTw();
   
    
   if (GlobalPar::GetPar()->IncludeTOE() && !GlobalPar::GetPar()->IncludeKalman())
      CreateRecActionGlb();
   
    
   if (GlobalPar::GetPar()->IncludeST() && GlobalPar::GetPar()->IncludeTG() &&
       GlobalPar::GetPar()->IncludeBM() && GlobalPar::GetPar()->IncludeVT() &&
       GlobalPar::GetPar()->IncludeIT() && !GlobalPar::GetPar()->IncludeDI())
       CreateRecActionIr();
}

//__________________________________________________________
void BaseReco::CreateRecActionBm()
{
   if(fFlagTrack) {
      fpNtuTrackBm = new TAGdataDsc("bmTrack", new TABMntuTrack());
      fActTrackBm  = new TABMactNtuTrack("bmActTrack", fpNtuTrackBm, fpNtuRawBm, fpParGeoBm, fpParConfBm, fpParGeoG);
      if (fFlagHisto)
         fActTrackBm->CreateHistogram();
   }
}

//__________________________________________________________
void BaseReco::CreateRecActionVtx()
{
   if(fFlagTrack) {
      fpNtuTrackVtx = new TAGdataDsc("vtTrack", new TAVTntuTrack());
      if (GlobalPar::GetPar()->IncludeTG() || GlobalPar::GetPar()->IncludeTOE())
         fpNtuVtx      = new TAGdataDsc("vtVtx",   new TAVTntuVertex());
   }
   
   fpNtuClusVtx  = new TAGdataDsc("vtClus", new TAVTntuCluster());
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) return;

   if (fM28ClusMtFlag)
      fActClusVtx   = new TAVTactNtuClusterMT("vtActClus", fpNtuRawVtx, fpNtuClusVtx, fpParConfVtx, fpParGeoVtx);
   else
      fActClusVtx   = new TAVTactNtuClusterF("vtActClus", fpNtuRawVtx, fpNtuClusVtx, fpParConfVtx, fpParGeoVtx);

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
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) return;

   if (fM28ClusMtFlag)
      fActClusIt   = new TAITactNtuClusterMT("itActClus", fpNtuRawIt, fpNtuClusIt, fpParConfIt, fpParGeoIt);
   else
      fActClusIt   = new TAITactNtuClusterF("itActClus", fpNtuRawIt, fpNtuClusIt, fpParConfIt, fpParGeoIt);

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
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) return;

   fActClusMsd   = new TAMSDactNtuCluster("msdActClus", fpNtuRawMsd, fpNtuClusMsd, fpParConfMsd, fpParGeoMsd);
   if (fFlagHisto)
      fActClusMsd->CreateHistogram();
}

//__________________________________________________________
void BaseReco::CreateRecActionTw()
{
   fpNtuRecTw  = new TAGdataDsc("twPoint", new TATWntuPoint());
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) return;

   fActPointTw = new TATWactNtuPoint("twActPoint", fpNtuRawTw, fpNtuRecTw, fpParGeoTw, fpParCalTw);
   if (fFlagHisto)
     fActPointTw->CreateHistogram();
}

//__________________________________________________________
void BaseReco::CreateRecActionGlb()
{
  if(fFlagTrack) {
    fpNtuGlbTrack = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
    fActGlbTrack  = new TAGactNtuGlbTrack( "glbActTrack",
                                           fpNtuClusVtx,
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
void BaseReco::SetTreeBranches()
{
   if (GlobalPar::GetPar()->IncludeTOE()) {
      if (fFlagTrack)
         fActEvtWriter->SetupElementBranch(fpNtuGlbTrack, TAGntuGlbTrack::GetBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) return;

  if (GlobalPar::GetPar()->IncludeBM()) {
    if (fFlagTrack)
      fActEvtWriter->SetupElementBranch(fpNtuTrackBm, TABMntuTrack::GetBranchName());
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
  
  if (GlobalPar::GetPar()->IncludeTW())
    fActEvtWriter->SetupElementBranch(fpNtuRecTw, TATWntuPoint::GetBranchName());

}

//__________________________________________________________
void BaseReco::AddRecRequiredItem()
{
   if (fFlagOut)
      gTAGroot->AddRequiredItem("locRecFile");
   
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) {
      if (fFlagTrack)
         gTAGroot->AddRequiredItem("glbActTrack");
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
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      gTAGroot->AddRequiredItem("twActNtu");
      gTAGroot->AddRequiredItem("twActPoint");
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
      gTAGroot->AddRequiredItem("caActNtu");
   }
   
   if (fFlagTrack) {
      if (GlobalPar::GetPar()->IncludeTOE() && !GlobalPar::GetPar()->IncludeKalman())
         gTAGroot->AddRequiredItem("glbActTrack");
   }
   
   if (GlobalPar::GetPar()->IncludeST() && GlobalPar::GetPar()->IncludeTG() &&
       GlobalPar::GetPar()->IncludeBM() && GlobalPar::GetPar()->IncludeVT() &&
       GlobalPar::GetPar()->IncludeIT() && !GlobalPar::GetPar()->IncludeDI())
      gTAGroot->AddRequiredItem("irActTrack");

}

//__________________________________________________________
void BaseReco::SetTrackingAlgo(char c)
{
   switch (c) {
      case 'S':
         fgTrackingAlgo = "Std";
         break;
      case 'F':
         fgTrackingAlgo = "Full";
         break;
      default:
         printf("SetTrackingAlgo: Wrongly set tracking algorithm");
   }
}
