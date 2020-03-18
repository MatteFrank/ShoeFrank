

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

Bool_t  BaseReco::fgItrTrackFlag = false;

//__________________________________________________________
BaseReco::BaseReco(TString expName, TString fileNameIn, TString fileNameout)
 : TNamed(fileNameIn.Data(), fileNameout.Data()),
   fExpName(expName),
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
   fgTrackingAlgo("Full"),
   fFlagMC(false)
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
   TString parFileName = Form("./geomaps/%sFOOT_geo.map", fExpName.Data());
   fpFootGeo->FromFile(parFileName);
   
   // activate per default Dipole, TGT, VTX, IT and TW if TOE on
   if (GlobalPar::GetPar()->IncludeTOE()) {
      GlobalPar::GetPar()->IncludeDI(true);
      GlobalPar::GetPar()->IncludeTG(true);
      GlobalPar::GetPar()->IncludeVertex(true);
      GlobalPar::GetPar()->IncludeInnerTracker(true);
      GlobalPar::GetPar()->IncludeTW(true);
   }
}

//__________________________________________________________
BaseReco::~BaseReco()
{
   // default destructor
   delete fTAGroot; // should delete all data, para and actions
   
   if (fFlagOut)
      delete fActEvtWriter;
   delete fpFootGeo;
}


//__________________________________________________________
void BaseReco::BeforeEventLoop()
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
void BaseReco::CloseFileOut()
{
   fActEvtWriter->Print();
   fActEvtWriter->Close();
}

//__________________________________________________________
void BaseReco::ReadParFiles()
{
   // initialise par files for target
   if (GlobalPar::GetPar()->IncludeTG() || GlobalPar::GetPar()->IncludeBM() || IsItrTracking()) {
      fpParGeoG = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
      TAGparGeo* parGeo = (TAGparGeo*)fpParGeoG->Object();
      TString parFileName = Form("./geomaps/%sTAGdetector.map", fExpName.Data());
      parGeo->FromFile(parFileName.Data());
   }
   
   // initialise par files for start counter
   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW()|| GlobalPar::GetPar()->IncludeBM()) {

     fpParGeoSt = new TAGparaDsc(TASTparGeo::GetDefParaName(), new TASTparGeo());
     TASTparGeo* parGeo = (TASTparGeo*)fpParGeoSt->Object();
     TString parFileName = "./geomaps/TASTdetector.map";
     parGeo->FromFile(parFileName.Data());
     
     fpParMapSt = new TAGparaDsc("stMap", new TASTparMap()); // need the file
     TASTparMap* parMapSt = (TASTparMap*) fpParMapSt->Object();
     parFileName = Form("./config/%sTASTdetector.cfg", fExpName.Data());
     parMapSt->FromFile(parFileName);
        
     fpParMapTw = new TAGparaDsc("twMap", new TATWparMap());
     TATWparMap* parMap = (TATWparMap*)fpParMapTw->Object();
     parFileName = Form("./config/%sTATWChannelMap.xml", fExpName.Data());
     parMap->FromFile(parFileName.Data());

     fpParMapWD = new TAGparaDsc("WDMap", new TAGbaseWDparMap());
     TAGbaseWDparMap* parMapWD = (TAGbaseWDparMap*)fpParMapWD->Object();
     parFileName = Form("./config/%sWDChannelMap.txt", fExpName.Data());
     parMapWD->FromFile(parFileName.Data());
       
     fpParTimeWD = new TAGparaDsc("WDTim", new TAGbaseWDparTime());
     TAGbaseWDparTime* parTimeWD = (TAGbaseWDparTime*) fpParTimeWD->Object();
     if(!fFlagMC)parTimeWD->FromFile(fExpName.Data(), 0);

   }
   
   // initialise par files for Beam Monitor
   if (GlobalPar::GetPar()->IncludeBM()) {
      fpParGeoBm = new TAGparaDsc("bmGeo", new TABMparGeo());
      TABMparGeo* parGeo = (TABMparGeo*)fpParGeoBm->Object();
      TString parFileName = "./geomaps/TABMdetector.map";
      parGeo->FromFile(parFileName.Data());
      
      fpParConfBm = new TAGparaDsc("bmConf", new TABMparCon());
      TABMparCon* parConf = (TABMparCon*)fpParConfBm->Object();
      parFileName = "./config/TABMdetector.cfg";
      parConf->FromFile(parFileName.Data());
      parFileName = Form("./config/%sT0_beammonitor.cfg", fExpName.Data());
      parConf->loadT0s(parFileName);
            
      fpParMapBm = new TAGparaDsc("bmMap", new TABMparMap());
      TABMparMap*  parMapBm = (TABMparMap*)fpParMapBm->Object();
      parFileName = Form("./config/%sTABMdetector.map", fExpName.Data());
      parMapBm->FromFile(parFileName.Data(), parGeo);
   }
   
   // initialise par files dipole
   if (GlobalPar::GetPar()->IncludeDI() || TAGactNtuGlbTrack::GetStdAloneFlag() ) {
      fpParGeoDi = new TAGparaDsc(TADIparGeo::GetDefParaName(), new TADIparGeo());
      TADIparGeo* parGeo = (TADIparGeo*)fpParGeoDi->Object();
      TString parFileName = "./geomaps/TADIdetector.map";
      parGeo->FromFile(parFileName.Data());
      
      if (GlobalPar::GetPar()->IncludeTOE())
         fField = new TADIgeoField(parGeo);
   }
   
   // initialise par files for vertex
   if (GlobalPar::GetPar()->IncludeVertex() || TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fpParGeoVtx = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
      TAVTparGeo* parGeo = (TAVTparGeo*)fpParGeoVtx->Object();
      TString parVtxFileName = Form("./geomaps/%sTAVTdetector.map", fExpName.Data());
      parGeo->FromFile(parVtxFileName.Data());
      
      fpParConfVtx = new TAGparaDsc("vtConf", new TAVTparConf());
      TAVTparConf* parConf = (TAVTparConf*)fpParConfVtx->Object();
      parVtxFileName = Form("./config/%sTAVTdetector.cfg", fExpName.Data());
      parConf->FromFile(parVtxFileName.Data());
      
      fpParMapVtx = new TAGparaDsc("vtMap", new TAVTparMap());
      TAVTparMap* parMap = (TAVTparMap*)fpParMapVtx->Object();
      parVtxFileName = Form("./config/%sTAVTdetector.map", fExpName.Data());
      parMap->FromFile(parVtxFileName.Data());
   }
   
   // initialise par files for inner tracker
   if (GlobalPar::GetPar()->IncludeInnerTracker() || TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fpParGeoIt = new TAGparaDsc(TAITparGeo::GetItDefParaName(), new TAITparGeo());
      TAITparGeo* parGeo = (TAITparGeo*)fpParGeoIt->Object();
      TString parItFileName = Form("./geomaps/%sTAITdetector.map", fExpName.Data());
      parGeo->FromFile(parItFileName.Data());
      
      fpParConfIt = new TAGparaDsc("itConf", new TAITparConf());
      TAITparConf* parConf = (TAITparConf*)fpParConfIt->Object();
      parItFileName = Form("./config/%sTAITdetector.cfg", fExpName.Data());
      parConf->FromFile(parItFileName.Data());
      
      fpParMapIt = new TAGparaDsc("itMap", new TAITparMap());
      TAITparMap* parMap = (TAITparMap*)fpParMapIt->Object();
      parItFileName = Form("./config/%sTAITdetector.map", fExpName.Data());
     // parMap->FromFile(parItFileName.Data());
   }
   
   // initialise par files for multi strip detector
   if (GlobalPar::GetPar()->IncludeMSD() || TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fpParGeoMsd = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
      TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
      TString parMsdFileName = Form("./geomaps/%sTAMSDdetector.map", fExpName.Data());
      parGeo->FromFile(parMsdFileName.Data());
      
      fpParConfMsd = new TAGparaDsc("msdConf", new TAMSDparConf());
      TAMSDparConf* parConf = (TAMSDparConf*)fpParConfMsd->Object();
      parMsdFileName = Form("./config/%sTAMSDdetector.cfg", fExpName.Data());
      // parConf->FromFile(parMsdFileName.Data());
   }
   
   // initialise par files for Tof Wall
   if (GlobalPar::GetPar()->IncludeTW() || TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fpParGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
      TATWparGeo* parGeo = (TATWparGeo*)fpParGeoTw->Object();
      TString parFileName = "./geomaps/TATWdetector.map";
      parGeo->FromFile(parFileName);
      
      fpParCalTw = new TAGparaDsc("twCal", new TATWparCal());
      TATWparCal* parCal = (TATWparCal*)fpParCalTw->Object();
      parFileName = Form("./config/%sTATWCalibrationMap.xml", fExpName.Data());
      // parCal->FromFile(parFileName.Data());
      parCal->FromFile(parFileName.Data(),0);
      parFileName = Form("./config/%sTATW_BBparameters_16O.cfg", fExpName.Data());
      parCal->FromFile(parFileName.Data(),1);

   }
   
   // initialise par files for caloriomter
   if (GlobalPar::GetPar()->IncludeCA()) {
      fpParGeoCa = new TAGparaDsc(TACAparGeo::GetDefParaName(), new TACAparGeo());
      TACAparGeo* parGeo = (TACAparGeo*)fpParGeoCa->Object();
      TString parFileName = Form("./geomaps/%sTACAdetector.map", fExpName.Data());
      parGeo->FromFile(parFileName);
   }

   TAVTparConf::SetHistoMap();
}

//__________________________________________________________
void BaseReco::CreateRecAction()
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
   
    
   if (GlobalPar::GetPar()->IncludeTOE() && !GlobalPar::GetPar()->IncludeKalman())
      CreateRecActionGlb();
   
    
   if (GlobalPar::GetPar()->IncludeST() && GlobalPar::GetPar()->IncludeTG() &&
       GlobalPar::GetPar()->IncludeBM() && GlobalPar::GetPar()->IncludeVertex() &&
       GlobalPar::GetPar()->IncludeInnerTracker() && !GlobalPar::GetPar()->IncludeDI())
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
       GlobalPar::GetPar()->IncludeBM() && GlobalPar::GetPar()->IncludeVertex() &&
       GlobalPar::GetPar()->IncludeInnerTracker() && !GlobalPar::GetPar()->IncludeDI())
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
