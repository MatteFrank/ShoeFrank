

#include "BaseLocalReco.hxx"

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

ClassImp(BaseLocalReco)

//__________________________________________________________
BaseLocalReco::BaseLocalReco(TString fileNameIn, TString fileNameout)
 : TNamed(fileNameIn.Data(), fileNameout.Data()),
   fExpName(""),
   fpParTimeSt(0x0),
   fpParTimeTw(0x0),
   fpParMapSt(0x0),
   fpParMapVtx(0x0),
   fpParMapIt(0x0),
   fpParGeoSt(0x0),
   fpParGeoBm(0x0),
   fpParGeoIt(0x0),
   fpParGeoMsd(0x0),
   fpParGeoTw(0x0),
   fpParGeoCa(0x0),
   fpParGeoVtx(0x0),
   fpParConfIt(0x0),
   fpParConfVtx(0x0),
   fpNtuRawVtx(0x0),
   fpNtuClusVtx(0x0),
   fpNtuTrackVtx(0x0),
   fpNtuVtx(0x0),
   fpNtuRawIt(0x0),
   fpNtuClusIt(0x0),
   fpNtuRawMsd(0x0),
   fpNtuClusMsd(0x0),
   fActEvtWriter(0x0),
   fActClusVtx(0x0),
   fActTrackVtx(0x0),
   fActVtx(0x0),
   fActClusIt(0x0),
   fFlagOut(true),
   fFlagTree(false),
   fFlagHits(false),
   fFlagHisto(false),
   fFlagTrack(false),
   fgTrackingAlgo("Full")
{

   if (fileNameout == "")
      fFlagOut = false;

   // define TAGroot
   fTAGroot = new TAGroot();
   if (fFlagOut)
      fActEvtWriter = new TAGactTreeWriter("locRecFile");

   // Read Trafo file
   fpFootGeo = new TAGgeoTrafo();
   TString parFileName = Form("./geomaps/FOOT_geo%s.map", fExpName.Data());
   fpFootGeo->FromFile(parFileName);
}

//__________________________________________________________
BaseLocalReco::~BaseLocalReco()
{
   // default destructor
   delete fTAGroot; // should delete all data, para and actions
   
   if (fFlagOut)
      delete fActEvtWriter;
   delete fpFootGeo;
}


//__________________________________________________________
void BaseLocalReco::BeforeEventLoop()
{
   ReadParFiles();
   CreateRawAction();
   CreateRecAction();

   OpenFileIn();

   AddRawRequiredItem();
   AddRecRequiredItem();
   
   if (fFlagOut)
      OpenFileOut();
   
   fTAGroot->BeginEventLoop();
   fTAGroot->Print();
}

//__________________________________________________________
void BaseLocalReco::AfterEventLoop()
{
   fTAGroot->EndEventLoop();
   if (fFlagOut)
      CloseFileOut();
   CloseFileIn();
}

//__________________________________________________________
void BaseLocalReco::OpenFileOut()
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
void BaseLocalReco::SetRecHistogramDir()
{
   //BMN
   if (GlobalPar::GetPar()->IncludeBM()) {
     if (fFlagTrack) 
       fActTrackBm->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   }
   
   // VTX
   if (GlobalPar::GetPar()->IncludeVertex()) {
      fActClusVtx->SetHistogramDir((TDirectory*)fActEvtWriter->File());
      
      if (fFlagTrack) {
         fActTrackVtx->SetHistogramDir((TDirectory*)fActEvtWriter->File());
         if (GlobalPar::GetPar()->IncludeTG()) 
            fActVtx->SetHistogramDir((TDirectory*)fActEvtWriter->File());
      }
   }
   
   // IT
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      fActClusIt->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   
   
   // MSD
   if (GlobalPar::GetPar()->IncludeMSD()) 
      fActClusMsd->SetHistogramDir((TDirectory*)fActEvtWriter->File());
}

//__________________________________________________________
void BaseLocalReco::CloseFileOut()
{
   fActEvtWriter->Print();
   fActEvtWriter->Close();
}

//__________________________________________________________
void BaseLocalReco::ReadParFiles()
{
   // initialise par files for target
   if (GlobalPar::GetPar()->IncludeTG() || GlobalPar::GetPar()->IncludeBM()) {
      fpParGeoG = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
      TAGparGeo* parGeo = (TAGparGeo*)fpParGeoG->Object();
      TString parFileName = Form("./geomaps/TAGdetector%s.map", fExpName.Data());
      parGeo->FromFile(parFileName.Data());
   }
   
   // initialise par files for start counter
   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeBM()) {

     fpParGeoSt = new TAGparaDsc(TASTparGeo::GetDefParaName(), new TASTparGeo());
      TASTparGeo* parGeo = (TASTparGeo*)fpParGeoSt->Object();
      TString parFileName = Form("./geomaps/TASTdetector%s.map", fExpName.Data());
      parGeo->FromFile(parFileName.Data());
      fpParMapSt = new TAGparaDsc("stMap", new TASTparMap()); // need the file
      TASTparMap* parMapSt = (TASTparMap*) fpParMapSt->Object();
      parFileName="./config/TASTdetector.cfg";
      parMapSt->FromFile(parFileName);
      
      fpParTimeSt = new TAGparaDsc("stTime", new TASTparTime()); // need the file
      TASTparTime* parTimeSt = (TASTparTime*) fpParTimeSt->Object();
      parTimeSt->FromFile(GetName());
   }
   
   // initialise par files for Beam Monitor
   if (GlobalPar::GetPar()->IncludeBM()) {
      fpParGeoBm = new TAGparaDsc("bmGeo", new TABMparGeo());
      TABMparGeo* parGeo = (TABMparGeo*)fpParGeoBm->Object();
      TString parFileName = Form("./geomaps/TABMdetector%s.map", fExpName.Data());
      parGeo->FromFile(parFileName.Data());
      
      fpParConfBm = new TAGparaDsc("bmConf", new TABMparCon());
      TABMparCon* parConf = (TABMparCon*)fpParConfBm->Object();
      parFileName = Form("./config/beammonitor%s.cfg", fExpName.Data());
      parConf->FromFile(parFileName.Data());
      
      parFileName = "./config/bmreso_vs_r.root";
      parConf->LoadReso(parFileName);
      
      fpParMapBm = new TAGparaDsc("bmMap", new TABMparMap());
      TABMparMap*  parMapBm = (TABMparMap*)fpParMapBm->Object();
      
      parFileName = "./geomaps/";
      parFileName += parConf->GetParmapfile();
      parMapBm->FromFile(parFileName.Data(), parGeo);
   }
   
   // initialise par files for vertex
   if (GlobalPar::GetPar()->IncludeVertex()) {
      fpParGeoVtx = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
      TAVTparGeo* parGeo = (TAVTparGeo*)fpParGeoVtx->Object();
      TString parVtxFileName = Form("./geomaps/TAVTdetector%s.map", fExpName.Data());
      parGeo->FromFile(parVtxFileName.Data());
      
      fpParConfVtx = new TAGparaDsc("vtConf", new TAVTparConf());
      TAVTparConf* parConf = (TAVTparConf*)fpParConfVtx->Object();
      parVtxFileName = Form("./config/TAVTdetector%s.cfg", fExpName.Data());
      parConf->FromFile(parVtxFileName.Data());
      
      fpParMapVtx = new TAGparaDsc("vtMap", new TAVTparMap());
      TAVTparMap* parMap = (TAVTparMap*)fpParMapVtx->Object();
      parVtxFileName = Form("./config/TAVTdetector%s.map", fExpName.Data());
      parMap->FromFile(parVtxFileName.Data());
   }
   
   // initialise par files for inner tracker
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fpParGeoIt = new TAGparaDsc(TAITparGeo::GetItDefParaName(), new TAITparGeo());
      TAITparGeo* parGeo = (TAITparGeo*)fpParGeoIt->Object();
      TString parItFileName = Form("./geomaps/TAITdetector%s.map", fExpName.Data());
      parGeo->FromFile(parItFileName.Data());
      
      fpParConfIt = new TAGparaDsc("itConf", new TAITparConf());
      TAITparConf* parConf = (TAITparConf*)fpParConfIt->Object();
      parItFileName = Form("./config/TAITdetector%s.cfg", fExpName.Data());
      parConf->FromFile(parItFileName.Data());
      
      fpParMapIt = new TAGparaDsc("itMap", new TAITparMap());
      TAVTparMap* parMap = (TAVTparMap*)fpParMapIt->Object();
      parItFileName = Form("./config/TAITdetector%s.map", fExpName.Data());
     // parMap->FromFile(parItFileName.Data());
   }
   
   // initialise par files for multi strip detector
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fpParGeoMsd = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
      TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
      TString parMsdFileName = Form("./geomaps/TAMSDdetector%s.map", fExpName.Data());
      parGeo->FromFile(parMsdFileName.Data());
      
      fpParConfMsd = new TAGparaDsc("msdConf", new TAMSDparConf());
      TAMSDparConf* parConf = (TAMSDparConf*)fpParConfMsd->Object();
      parMsdFileName = Form("./config/TAMSDdetector%s.cfg", fExpName.Data());
      // parConf->FromFile(parMsdFileName.Data());
   }
   
   // initialise par files for Tof Wall
   if (GlobalPar::GetPar()->IncludeTW()) {
      fpParGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
      TATWparGeo* parGeo = (TATWparGeo*)fpParGeoTw->Object();
      TString parFileName = Form("./geomaps/TATWdetector%s.map", fExpName.Data());
      parGeo->FromFile(parFileName);
      
      fpParCalTw = new TAGparaDsc("twCal", new TATWparCal());
      TATWparCal* parCal = (TATWparCal*)fpParCalTw->Object();
      parFileName = Form("./config/TATWCalibrationMap%s.xml", fExpName.Data());
      parCal->FromFile(parFileName.Data());
      
      fpParMapTw = new TAGparaDsc("twMap", new TATWparMap());
      TATWparMap* parMap = (TATWparMap*)fpParMapTw->Object();
      parFileName = Form("./config/TATWChannelMap%s.xml", fExpName.Data());
      parMap->FromFile(parFileName.Data());
      
      fpParTimeTw = new TAGparaDsc("twTim", new TATWparTime());
      TATWparTime* parTimeTw = (TATWparTime*) fpParTimeTw->Object();
      parTimeTw->FromFile(GetName());
   }
   
   // initialise par files for caloriomter
   if (GlobalPar::GetPar()->IncludeCA()) {
      fpParGeoCa = new TAGparaDsc(TACAparGeo::GetDefParaName(), new TACAparGeo());
      TACAparGeo* parGeo = (TACAparGeo*)fpParGeoCa->Object();
      TString parFileName = Form("./geomaps/TACAdetector%s.map", fExpName.Data());
      parGeo->FromFile(parFileName);
   }

   TAVTparConf::SetHistoMap();
}

//__________________________________________________________
void BaseLocalReco::CreateRecAction()
{
   if (GlobalPar::GetPar()->IncludeBM())
      CreateRecActionBm();
   
   if (GlobalPar::GetPar()->IncludeVertex())
      CreateRecActionVtx();
   
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      CreateRecActionIt();
   
   if (GlobalPar::GetPar()->IncludeMSD())
      CreateRecActionMsd();
   
   if (GlobalPar::GetPar()->IncludeTW())
      CreateRecActionTw();
}

//__________________________________________________________
void BaseLocalReco::CreateRecActionBm()
{
   if(fFlagTrack) {
      fpNtuTrackBm = new TAGdataDsc("bmTrack", new TABMntuTrack());
      fActTrackBm  = new TABMactNtuTrack("bmActTrack", fpNtuTrackBm, fpNtuRawBm, fpParGeoBm, fpParConfBm, fpParGeoG);
      if (fFlagHisto)
         fActTrackBm->CreateHistogram();
   }
}

//__________________________________________________________
void BaseLocalReco::CreateRecActionVtx()
{
   if(fFlagTrack) {
      fpNtuTrackVtx = new TAGdataDsc("vtTrack", new TAVTntuTrack());
      if (GlobalPar::GetPar()->IncludeTG())
         fpNtuVtx      = new TAGdataDsc("vtVtx",   new TAVTntuVertex());
   }
   
   fpNtuClusVtx  = new TAGdataDsc("vtClus", new TAVTntuCluster());
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
	    fActVtx    = new TAVTactNtuVertexPD("vtActVtx", fpNtuTrackVtx, fpNtuVtx, fpParConfVtx, fpParGeoVtx, fpParGeoG);
	    if (fFlagHisto)
	      fActVtx->CreateHistogram();
	  } else {
	    fActVtx    = new TAVTactNtuVertexPD("vtActVtx", fpNtuTrackVtx, fpNtuVtx, fpParConfVtx, fpParGeoVtx, fpParGeoG, fpNtuTrackBm);
	    if (fFlagHisto)
	      fActVtx->CreateHistogram();
	  }
      }
   }
}

//__________________________________________________________
void BaseLocalReco::CreateRecActionIt()
{
   fpNtuClusIt  = new TAGdataDsc("itClus", new TAITntuCluster());
   fActClusIt   = new TAITactNtuClusterF("itActClus", fpNtuRawIt, fpNtuClusIt, fpParConfIt, fpParGeoIt);
   if (fFlagHisto)
      fActClusIt->CreateHistogram();
}

//__________________________________________________________
void BaseLocalReco::CreateRecActionMsd()
{
   fpNtuClusMsd  = new TAGdataDsc("msdClus", new TAMSDntuCluster());
   fActClusMsd   = new TAMSDactNtuCluster("msdActClus", fpNtuRawMsd, fpNtuClusMsd, fpParConfMsd, fpParGeoMsd);
   if (fFlagHisto)
      fActClusMsd->CreateHistogram();
}

//__________________________________________________________
void BaseLocalReco::CreateRecActionTw()
{
   fpNtuRecTw  = new TAGdataDsc("twPoint", new TATWntuPoint());
   fActPointTw = new TATWactNtuPoint("twActPoint", fpNtuRawTw, fpNtuRecTw, fpParGeoTw, fpParCalTw);
   if (fFlagHisto)
     fActPointTw->CreateHistogram();
}

//__________________________________________________________
void BaseLocalReco::SetTreeBranches()
{
   if (GlobalPar::GetPar()->IncludeBM()) {
      if (fFlagTrack)
         fActEvtWriter->SetupElementBranch(fpNtuTrackBm, TABMntuTrack::GetBranchName());
   }

   if (GlobalPar::GetPar()->IncludeVertex()) {
      if (!fFlagTrack)
         fActEvtWriter->SetupElementBranch(fpNtuClusVtx, TAVTntuCluster::GetBranchName());
      else {
	fActEvtWriter->SetupElementBranch(fpNtuClusVtx, TAVTntuCluster::GetBranchName());
         fActEvtWriter->SetupElementBranch(fpNtuTrackVtx, TAVTntuTrack::GetBranchName());
         if (GlobalPar::GetPar()->IncludeTG())
            fActEvtWriter->SetupElementBranch(fpNtuVtx, TAVTntuVertex::GetBranchName());
      }
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      fActEvtWriter->SetupElementBranch(fpNtuClusIt, TAITntuCluster::GetBranchName());

   if (GlobalPar::GetPar()->IncludeMSD()) 
      fActEvtWriter->SetupElementBranch(fpNtuClusMsd, TAMSDntuCluster::GetBranchName());
   
}

//__________________________________________________________
void BaseLocalReco::AddRecRequiredItem()
{
   if (fFlagOut)
      gTAGroot->AddRequiredItem("locRecFile");
   
   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeBM())
      gTAGroot->AddRequiredItem("stActNtu");
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      gTAGroot->AddRequiredItem("bmActNtu");
      if (fFlagTrack)
         gTAGroot->AddRequiredItem("bmActTrack");
   }
   
   if (GlobalPar::GetPar()->IncludeVertex()) {
      gTAGroot->AddRequiredItem("vtActNtu");
      gTAGroot->AddRequiredItem("vtActClus");
      if (fFlagTrack) {
         gTAGroot->AddRequiredItem("vtActTrack");
         if (GlobalPar::GetPar()->IncludeTG())
            gTAGroot->AddRequiredItem("vtActVtx");
      }
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      gTAGroot->AddRequiredItem("itActNtu");
      gTAGroot->AddRequiredItem("itActClus");
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
}

//__________________________________________________________
void BaseLocalReco::SetTrackingAlgo(char c)
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
