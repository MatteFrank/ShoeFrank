

#include "TAFOeventDisplay.hxx"

// root include
#include "Riostream.h"
#include "TRootEmbeddedCanvas.h"
#include "TEveManager.h"
#include "TEveWindow.h"
#include "TEveBrowser.h"
#include "TEveBrowser.h"

#include "GlobalPar.hxx"
#include "TAGactTreeReader.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGparGeo.hxx"

#include "TASTntuRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TAVTntuRaw.hxx"
#include "TAITntuRaw.hxx"
#include "TAMSDntuRaw.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuRaw.hxx"

#include "TASTdatRaw.hxx"
#include "TABMdatRaw.hxx"
#include "TATWdatRaw.hxx"
#include "TATWntuRaw.hxx"

#include "TABMactVmeReader.hxx"
#include "TABMactDatRaw.hxx"

#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"

#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAVTdatRaw.hxx"
#include "TAVTactNtuRaw.hxx"
#include "TAVTactVmeReader.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuVertexPD.hxx"


ClassImp(TAFOeventDisplay)

Bool_t  TAFOeventDisplay::fgTrackFlag    = true;
TString TAFOeventDisplay::fgTrackingAlgo = "Std";
Bool_t  TAFOeventDisplay::fgStdAloneFlag = false;

TAFOeventDisplay* TAFOeventDisplay::fgInstance = 0x0;

//__________________________________________________________
TAFOeventDisplay* TAFOeventDisplay::Instance(Int_t type, const TString name)
{
   if (fgInstance == 0x0)
      fgInstance = new TAFOeventDisplay(type, name);
   
   return fgInstance;
}

//__________________________________________________________
TAFOeventDisplay::TAFOeventDisplay(Int_t type, const TString expName)
 : TAGbaseEventDisplay(expName),
   fpParGeoSt(0x0),
   fpParGeoG(0x0),
   fpParGeoDi(0x0),
   fpParGeoBm(0x0),
   fpParGeoVtx(0x0),
   fpParGeoIt(0x0),
   fpParGeoMsd(0x0),
   fpParGeoTw(0x0),
   fpParGeoCa(0x0),

   fpParTimeSt(0x0),
   fpParTimeTw(0x0),

   fpParMapSt(0x0),
   fpParMapBm(0x0),
   fpParMapTw(0x0),

   fpParTimTw(0x0),

   fpParCalBm(0x0),
   fpParCalTw(0x0),

   fpParConfBm(0x0),
   fpParConfVtx(0x0),
   fpParConfIt(0x0),
   fpParConfMsd(0x0),

   fpDaqEvent(0x0),
   fpDatRawSt(0x0),
   fpNtuRawSt(0x0),

   fpDatRawBm(0x0),
   fpNtuRawBm(0x0),
   fActNtuRawBm(0x0),
   fpNtuTrackBm(0x0),

   fpNtuRawVtx(0x0),
   fpNtuClusVtx(0x0),
   fpNtuTrackVtx(0x0),
   fpNtuVtx(0x0),

   fpDatRawIt(0x0),
   fpNtuRawIt(0x0),
   fpNtuClusIt(0x0),

   fpDatRawMsd(0x0),
   fpNtuRawMsd(0x0),
   fpNtuClusMsd(0x0),

   fpNtuRawTw(0x0),
   fpNtuRecTw(0x0),
   fpNtuRawCa(0x0),

   fActEvtReader(0x0),

   fActDatRawSt(0x0),

   fActVmeReaderBm(0x0),
   fActDatRawBm(0x0),
   fActTrackBm(0x0),

   fActVmeReaderVtx(0x0),
   fActNtuRawVtx(0x0),
   fActClusVtx(0x0),
   fActTrackVtx(0x0),
   fActVtx(0x0),

   fActNtuRawIt(0x0),
   fActClusIt(0x0),

   fActNtuRawMsd(0x0),
   fActClusMsd(0x0),

   fActNtuRawTw(0x0),
   fActPointTw(0x0),
   fType(type),
   fStClusDisplay(new TAGclusterDisplay("Start counter hit")),
   fBmClusDisplay(new TAGwireDisplay("Beam Monitoring Wires")),
   fBmTrackDisplay(new TAGtrackDisplay("Beam Monitoring Tracks")),
   fBmDriftCircleDisplay(new TEveBoxSet("Beam Monitoring Drift Circle")),
   fVtxClusDisplay(new TAGclusterDisplay("Vertex Cluster")),
   fVtxTrackDisplay(new TAGtrackDisplay("Vertex Tracks")),
   fItClusDisplay(new TAGclusterDisplay("Inner tracker Cluster")),
   fMsdClusDisplay(new TAGclusterDisplay("Multi Strip Cluster")),
   fTwClusDisplay(new TAGclusterDisplay("Tof Wall hit")),
   fCaClusDisplay(new TAGclusterDisplay("Calorimeter hit")),
   fGlbTrackDisplay(new TAGglbTrackDisplay("Global Tracks"))
{
   
   // default constructon
   fStClusDisplay->SetMaxEnergy(fMaxEnergy);
   fStClusDisplay->SetDefWidth(fQuadDefWidth/2.);
   fStClusDisplay->SetDefHeight(fQuadDefHeight/2.);
   fStClusDisplay->SetPickable(true);

   fBmClusDisplay->SetPickable(true);
   fBmTrackDisplay->SetMaxEnergy(fMaxEnergy);
   fBmTrackDisplay->SetDefWidth(fBoxDefWidth);
   fBmTrackDisplay->SetDefHeight(fBoxDefHeight);
   fBmTrackDisplay->SetPickable(true);
    fBmDriftCircleDisplay->SetPickable(true);

   
   fVtxClusDisplay->SetMaxEnergy(fMaxEnergy);
   fVtxClusDisplay->SetDefWidth(fQuadDefWidth/2.);
   fVtxClusDisplay->SetDefHeight(fQuadDefHeight/2.);
   fVtxClusDisplay->SetPickable(true);
   
   fVtxTrackDisplay->SetMaxEnergy(fMaxEnergy/2.);
   fVtxTrackDisplay->SetDefWidth(fBoxDefWidth);
   fVtxTrackDisplay->SetDefHeight(fBoxDefHeight);
   fVtxTrackDisplay->SetPickable(true);
   
   fItClusDisplay->SetMaxEnergy(fMaxEnergy);
   fItClusDisplay->SetDefWidth(fQuadDefWidth);
   fItClusDisplay->SetDefHeight(fQuadDefHeight);
   fItClusDisplay->SetPickable(true);
   
   fMsdClusDisplay->SetMaxEnergy(fMaxEnergy);
   fMsdClusDisplay->SetDefWidth(fQuadDefWidth);
   fMsdClusDisplay->SetDefHeight(fQuadDefHeight);
   fMsdClusDisplay->SetPickable(true);
   
   fTwClusDisplay->SetMaxEnergy(fMaxEnergy);
   fTwClusDisplay->SetDefWidth(fQuadDefWidth*8);
   fTwClusDisplay->SetDefHeight(fQuadDefHeight*8);
   fTwClusDisplay->SetPickable(true);
   
   fCaClusDisplay->SetMaxEnergy(fMaxEnergy);
   fCaClusDisplay->SetDefWidth(fQuadDefWidth*4);
   fCaClusDisplay->SetDefHeight(fQuadDefHeight*4);
   fCaClusDisplay->SetPickable(true);
   
   fGlbTrackDisplay->SetMaxMomentum(fMaxMomentum);
   
   GlobalPar::Instance();
}

//__________________________________________________________
TAFOeventDisplay::~TAFOeventDisplay()
{
   // default destructor
   if (fpParGeoSt)  delete fpParGeoSt;
   if (fpParGeoBm)  delete fpParGeoBm;
   if (fpParGeoVtx) delete fpParGeoVtx;
   if (fpParGeoIt)  delete fpParGeoIt;
   if (fpParGeoMsd) delete fpParGeoMsd;
   if (fpParGeoTw)  delete fpParGeoTw;
   if (fpParGeoCa)  delete fpParGeoCa;
   if (fpParGeoG)   delete fpParGeoG;
   if (fpParGeoDi)  delete fpParGeoDi;
   
   delete fStClusDisplay;
   delete fBmClusDisplay;
   delete fBmTrackDisplay;
   delete fVtxClusDisplay;
   delete fVtxTrackDisplay;
   delete fItClusDisplay;
   delete fMsdClusDisplay;
   delete fTwClusDisplay;
   delete fCaClusDisplay;
    delete fBmDriftCircleDisplay;
   
   delete fGlbTrackDisplay;
   
   if (fField) delete fField;
   if (fFieldImpl) delete fFieldImpl;
}

//__________________________________________________________
void TAFOeventDisplay::ReadParFiles()
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
   }
   
   // initialise par files for Magnet
   if (GlobalPar::GetPar()->IncludeDI()) {
      fpParGeoDi = new TAGparaDsc("diGeo", new TADIparGeo());
      TADIparGeo* parGeo = (TADIparGeo*)fpParGeoDi->Object();
      TString parFileName = Form("./geomaps/TADIdetector%s.map", fExpName.Data());
      parGeo->FromFile(parFileName.Data());
      
      if (GlobalPar::GetPar()->IncludeKalman()) {
         const Char_t* fieldFileName = parGeo->GetMapName().Data();
         fFieldImpl  = new FootField(fieldFileName);
         fField = new TADIeveField(fFieldImpl);
         fGlbTrackDisplay->GetPropagator()->SetMagFieldObj(fField);
         fGlbTrackDisplay->GetPropagator()->SetMaxZ(fWorldSizeZ);
         fGlbTrackDisplay->GetPropagator()->SetMaxR(fWorldSizeXY);
      }
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

      fpParTimTw = new TAGparaDsc("twTim", new TATWparTime());
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
void TAFOeventDisplay::BuildDefaultGeometry()
{
   TAGbaseEventDisplay::BuildDefaultGeometry();
   
   // ST
   if (GlobalPar::GetPar()->IncludeST()) {
      TASTparGeo* parGeo = (TASTparGeo*)fpParGeoSt->Object();
      TGeoVolume* irVol  = parGeo->BuildStartCounter();
   
      const TGeoHMatrix* transfo = fpFootGeo->GetTrafo(TASTparGeo::GetBaseName());
      TGeoHMatrix* transf        = (TGeoHMatrix*)transfo->Clone();
      AddGeometry(irVol, transf);
   }

   // BM
   if (GlobalPar::GetPar()->IncludeBM()) {
      TABMparGeo* parGeo = (TABMparGeo*)fpParGeoBm->Object();
      TGeoVolume* bmVol  = parGeo->BuildBeamMonitor();
      
      const TGeoHMatrix* transfo = fpFootGeo->GetTrafo(TABMparGeo::GetBaseName());
      TGeoHMatrix* transf        = (TGeoHMatrix*)transfo->Clone();
      AddGeometry(bmVol, transf);
   }

   // target
   if (GlobalPar::GetPar()->IncludeTG()) {
      TAGparGeo* parGeo = (TAGparGeo*)fpParGeoG->Object();
      TGeoVolume* tgVol = parGeo->BuildTarget();
      
      const TGeoHMatrix* transfo = fpFootGeo->GetTrafo(TAGparGeo::GetBaseName());
      TGeoHMatrix* transf        = (TGeoHMatrix*)transfo->Clone();
      AddGeometry(tgVol, transf);
   }

   // Vertex
   if (GlobalPar::GetPar()->IncludeVertex()) {
      TAVTparGeo* parGeo = (TAVTparGeo*)fpParGeoVtx->Object();
      TGeoVolume* vtVol  = parGeo->BuildVertex();
      
      const TGeoHMatrix* transfo = fpFootGeo->GetTrafo(TAVTparGeo::GetBaseName());
      TGeoHMatrix* transf        = (TGeoHMatrix*)transfo->Clone();
      AddGeometry(vtVol, transf);
   }

   // Magnet
   if (GlobalPar::GetPar()->IncludeDI()) {
      TADIparGeo* parGeo = (TADIparGeo*)fpParGeoDi->Object();
      TGeoVolume* vtVol = parGeo->BuildMagnet();
      
      const TGeoHMatrix* transfo = fpFootGeo->GetTrafo(TADIparGeo::GetBaseName());
      TGeoHMatrix* transf        = (TGeoHMatrix*)transfo->Clone();
      AddGeometry(vtVol, transf);
   }

   // IT
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      TAITparGeo* parGeo = (TAITparGeo*)fpParGeoIt->Object();
      TGeoVolume* itVol  = parGeo->BuildInnerTracker();
      
      const TGeoHMatrix* transfo = fpFootGeo->GetTrafo(TAITparGeo::GetItBaseName());
      TGeoHMatrix* transf        = (TGeoHMatrix*)transfo->Clone();
      AddGeometry(itVol, transf);
   }
   
   // MSD
   if (GlobalPar::GetPar()->IncludeMSD()) {
      TAMSDparGeo* parGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
      TGeoVolume* msdVol = parGeo->BuildMultiStripDetector();
      
      const TGeoHMatrix* transfo = fpFootGeo->GetTrafo(TAMSDparGeo::GetBaseName());
      TGeoHMatrix* transf        = (TGeoHMatrix*)transfo->Clone();
      AddGeometry(msdVol, transf);
   }

   // TW
   if (GlobalPar::GetPar()->IncludeTW()) {
      TATWparGeo* parGeo = (TATWparGeo*)fpParGeoTw->Object();
      TGeoVolume* twVol = parGeo->BuildTofWall();
      
      const TGeoHMatrix* transfo = fpFootGeo->GetTrafo(TATWparGeo::GetBaseName());
      TGeoHMatrix* transf        = (TGeoHMatrix*)transfo->Clone();
      AddGeometry(twVol, transf);
   }

   // CA
   if (GlobalPar::GetPar()->IncludeCA()) {
      TACAparGeo* parGeo = (TACAparGeo*)fpParGeoCa->Object();
      TGeoVolume* caVol = parGeo->BuildCalorimeter();
      
      const TGeoHMatrix* transfo = fpFootGeo->GetTrafo(TACAparGeo::GetBaseName());
      TGeoHMatrix* transf        = (TGeoHMatrix*)transfo->Clone();
      AddGeometry(caVol, transf);
   }
}

//__________________________________________________________
void TAFOeventDisplay::CreateRecAction()
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
void TAFOeventDisplay::CreateRecActionBm()
{
   if(fgTrackFlag) {
      fpNtuTrackBm = new TAGdataDsc("bmTrack", new TABMntuTrack());
      fActTrackBm  = new TABMactNtuTrack("bmActTrack", fpNtuTrackBm, fpNtuRawBm, fpParGeoBm, fpParConfBm, fpParGeoG);
      fActTrackBm->CreateHistogram();
   }
}

//__________________________________________________________
void TAFOeventDisplay::CreateRecActionVtx()
{
   if(fgTrackFlag) {
      fpNtuTrackVtx = new TAGdataDsc("vtTrack", new TAVTntuTrack());
      if (GlobalPar::GetPar()->IncludeTG())
         fpNtuVtx      = new TAGdataDsc("vtVtx",   new TAVTntuVertex());
   }
   
   fpNtuClusVtx  = new TAGdataDsc("vtClus", new TAVTntuCluster());
   fActClusVtx   = new TAVTactNtuClusterF("vtActClus", fpNtuRawVtx, fpNtuClusVtx, fpParConfVtx, fpParGeoVtx);
   fActClusVtx->CreateHistogram();
   
   if (fgTrackFlag) {
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
      fActTrackVtx->CreateHistogram();
      
      if (GlobalPar::GetPar()->IncludeTG()) {
         fActVtx    = new TAVTactNtuVertexPD("vtActVtx", fpNtuTrackVtx, fpNtuVtx, fpParConfVtx, fpParGeoVtx, fpParGeoG);
         fActVtx->CreateHistogram();
         
      } else if (GlobalPar::GetPar()->IncludeTG() && GlobalPar::GetPar()->IncludeBM()) {
         fActVtx    = new TAVTactNtuVertexPD("vtActVtx", fpNtuTrackVtx, fpNtuVtx, fpParConfVtx, fpParGeoVtx, fpParGeoG, fpNtuTrackBm);
         fActVtx->CreateHistogram();
      }
   }
}

//__________________________________________________________
void TAFOeventDisplay::CreateRecActionIt()
{
   
   fpNtuClusIt  = new TAGdataDsc("itClus", new TAITntuCluster());
   fActClusIt   = new TAITactNtuClusterF("itActClus", fpNtuRawIt, fpNtuClusIt, fpParConfIt, fpParGeoIt);
   fActClusIt->CreateHistogram();
}

//__________________________________________________________
void TAFOeventDisplay::CreateRecActionMsd()
{
   fpNtuClusMsd  = new TAGdataDsc("msdClus", new TAMSDntuCluster());
   fActClusMsd   = new TAMSDactNtuCluster("msdActClus", fpNtuRawMsd, fpNtuClusMsd, fpParConfMsd, fpParGeoMsd);
   fActClusMsd->CreateHistogram();
}

//__________________________________________________________
void TAFOeventDisplay::CreateRecActionTw()
{
   fpNtuRecTw  = new TAGdataDsc("twPoint", new TATWntuPoint());
   fActPointTw = new TATWactNtuPoint("twActPoint", fpNtuRawTw, fpNtuRecTw, fpParGeoTw, fpParCalTw);
   fActPointTw->CreateHistogram();
}

//__________________________________________________________
void TAFOeventDisplay::CreateRawAction()
{
   ReadParFiles();

   if (!fgStdAloneFlag) {
      fpDaqEvent = new TAGdataDsc("daqEvt", new TAGdaqEvent());
      fActEvtReader = new TAGactDaqReader("daqActReader", fpDaqEvent);
   }
   
   if (GlobalPar::GetPar()->IncludeST() ||GlobalPar::GetPar()->IncludeBM()) {
      fpParTimeSt   = new TAGparaDsc("stTime", new TASTparTime());
      fpDatRawSt   = new TAGdataDsc("stDat", new TASTdatRaw());
      fActDatRawSt = new TASTactDatRaw("stActDat", fpDatRawSt, fpDaqEvent, fpParMapSt, fpParTimeSt);
      fActDatRawSt->CreateHistogram();

      fpNtuRawSt   = new TAGdataDsc("stNtu", new TASTntuRaw());
      fActNtuRawSt = new TASTactNtuRaw("stActNtu", fpDatRawSt, fpNtuRawSt);
      if(GlobalPar::GetPar()->Debug()) fActNtuRawSt->SetDebugLevel(1);
      fActNtuRawSt->CreateHistogram();
   }

   if (GlobalPar::GetPar()->IncludeBM()) {
      fpDatRawBm = new TAGdataDsc("bmDat", new TABMdatRaw());
      fpNtuRawBm = new TAGdataDsc("bmNtu", new TABMntuRaw());

      if (fgStdAloneFlag) {
         fActVmeReaderBm  = new TABMactVmeReader("bmActNtu", fpDatRawBm, fpParMapBm, fpParConfBm, fpParGeoBm, fpDatRawSt);
         fActVmeReaderBm->CreateHistogram();
         
      } else {
         fActDatRawBm = new TABMactDatRaw("bmActDat", fpDatRawBm, fpDaqEvent, fpParMapBm, fpParConfBm, fpParGeoBm, fpDatRawSt);
         fActDatRawBm->CreateHistogram();
         
         fActNtuRawBm = new TABMactNtuRaw("bmActNtu", fpNtuRawBm, fpDatRawBm, fpDatRawSt, fpParGeoBm, fpParConfBm);
         fActNtuRawBm->CreateHistogram();
      }
   }

   if (GlobalPar::GetPar()->IncludeVertex()) {
      fpNtuRawVtx   = new TAGdataDsc("vtRaw", new TAVTntuRaw());
      
      if (fgStdAloneFlag) {
         fActVmeReaderVtx  = new TAVTactVmeReader("vtActNtu", fpNtuRawVtx, fpParGeoVtx, fpParConfVtx);
         fActVmeReaderVtx->CreateHistogram();

      } else {
         fActNtuRawVtx = new TAVTactNtuRaw("vtActNtu", fpNtuRawVtx, fpDaqEvent, fpParGeoVtx, fpParConfVtx);
         fActNtuRawVtx->CreateHistogram();
      }
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fpDatRawIt   = new TAGdataDsc("itDat", new TAVTdatRaw());
      fpNtuRawIt   = new TAGdataDsc("itRaw", new TAITntuRaw());
      fActNtuRawIt = new TAITactNtuRaw("itActNtu", fpNtuRawIt, fpDatRawIt, fpParGeoIt);
      fActNtuRawIt->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fpDatRawMsd   = new TAGdataDsc("msdDat", new TAVTdatRaw());
      fpNtuRawMsd   = new TAGdataDsc("msdRaw", new TAMSDntuRaw());
      fActNtuRawMsd = new TAVTactNtuRaw("msdActNtu", fpNtuRawMsd, fpDatRawMsd, fpParGeoMsd);
      fActNtuRawMsd->CreateHistogram();
   }
   
   if(GlobalPar::GetPar()->IncludeTW()) {
      fpParTimeTw   = new TAGparaDsc("twTime", new TATWparTime());
      fpDatRawTw   = new TAGdataDsc("twdDat", new TATWdatRaw());
      //      fpNtuRawTw   = new TAGdataDsc("twRaw", new TATW_ContainerHit());
      fpNtuRawTw   = new TAGdataDsc("twRaw", new TATWntuRaw());
      fActDatRawTw = new TATWactDatRaw("twActDat", fpDatRawTw, fpDaqEvent, fpParMapTw, fpParTimTw);
      fActDatRawTw->CreateHistogram();

      fActNtuRawTw = new TATWactNtuRaw("twActNtu", fpDatRawTw, fpNtuRawTw, fpParGeoTw, fpParMapTw, fpParCalTw);
      fActNtuRawTw->CreateHistogram();
   }
   
//   if(GlobalPar::GetPar()->IncludeCA()) {
//      fpDatRawCa   = new TAGdataDsc("cadDat", new TACAdatRaw());
//      fpNtuRawCa   = new TAGdataDsc("caRaw", new TACAntuRaw());
//      fActNtuRawCa = new TAVTactNtuRaw("caActNtu", fpNtuRawCa, fpNtuRawCa, fpParGeoCa);
//      fActNtuRawCa->CreateHistogram();
//   }
}

//__________________________________________________________
void TAFOeventDisplay::OpenFile(const TString fileName)
{
   if (fgStdAloneFlag) {
      if (GlobalPar::GetPar()->IncludeVertex())
         fActVmeReaderVtx->Open(fileName.Data());
      
      if (GlobalPar::GetPar()->IncludeBM())
         fActVmeReaderBm->Open(fileName.Data());

   } else
      fActEvtReader->Open(fileName.Data());
}

//__________________________________________________________
void TAFOeventDisplay::ResetHistogram()
{
   TList* list = gTAGroot->ListOfAction();
   Int_t hCnt = 0;
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TAGaction* action = (TAGaction*)list->At(i);
      TList* hlist = action->GetHistogrammList();
      if (hlist == 0x0) continue;
      for (Int_t j = 0; j < hlist->GetEntries(); ++j) {
         TH1* h = (TH1*)hlist->At(j);
         if (h) h->Reset();
      }
   }
}

//__________________________________________________________
void TAFOeventDisplay::AddRequiredItem()
{
   AddRequiredRawItem();
   AddRequiredRecItem();
   fTAGroot->BeginEventLoop();
   fTAGroot->Print();
}

//__________________________________________________________
void TAFOeventDisplay::AddRequiredRawItem()
{
   if (!fgStdAloneFlag)
      fTAGroot->AddRequiredItem("daqActReader");

   if (GlobalPar::GetPar()->IncludeST())
      fTAGroot->AddRequiredItem("stActDat");

   
   if (GlobalPar::GetPar()->IncludeBM())
      fTAGroot->AddRequiredItem("bmActDat");
   
   if (GlobalPar::GetPar()->IncludeTW())
      fTAGroot->AddRequiredItem("twActDat");

}


//__________________________________________________________
void TAFOeventDisplay::AddRequiredRecItem()
{
   if (GlobalPar::GetPar()->IncludeBM()) {
      fTAGroot->AddRequiredItem("bmActNtu");
      if (fgTrackFlag)
         fTAGroot->AddRequiredItem("bmActTrack");
   }
   
   if (GlobalPar::GetPar()->IncludeVertex()) {
      fTAGroot->AddRequiredItem("vtActNtu");
      
      if (fgDisplayFlag) {
         fTAGroot->AddRequiredItem("vtActClus");
         if (fgTrackFlag) {
            fTAGroot->AddRequiredItem("vtActTrack");
            if (GlobalPar::GetPar()->IncludeTG())
               fTAGroot->AddRequiredItem("vtActVtx");
         }
      }
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fTAGroot->AddRequiredItem("itActNtu");
      
      if (fgDisplayFlag) {
         fTAGroot->AddRequiredItem("itActClus");
      }
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fTAGroot->AddRequiredItem("msdActNtu");
      
      if (fgDisplayFlag) {
         fTAGroot->AddRequiredItem("msdActClus");
      }
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      fTAGroot->AddRequiredItem("twActNtu");
      fTAGroot->AddRequiredItem("twActPoint");
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
      fTAGroot->AddRequiredItem("caActNtu");
   }
   
   fTAGroot->BeginEventLoop();
   fTAGroot->Print();
}

//__________________________________________________________
void TAFOeventDisplay::AddElements()
{
   if (GlobalPar::GetPar()->IncludeST()){
      fStClusDisplay->ResetHits();
      gEve->AddElement(fStClusDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fBmClusDisplay->ResetWires();
      gEve->AddElement(fBmClusDisplay);
      
      fBmTrackDisplay->ResetTracks();
      gEve->AddElement(fBmTrackDisplay);
       
       fBmDriftCircleDisplay->Reset(TEveBoxSet::kBT_Cone, kFALSE, 32);
       gEve->AddElement(fBmDriftCircleDisplay);
   }

   if (GlobalPar::GetPar()->IncludeVertex()) {
      fVtxClusDisplay->ResetHits();
      gEve->AddElement(fVtxClusDisplay);
	  
      fVtxTrackDisplay->ResetTracks();
      gEve->AddElement(fVtxTrackDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fItClusDisplay->ResetHits();
      gEve->AddElement(fItClusDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fMsdClusDisplay->ResetHits();
      gEve->AddElement(fMsdClusDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      fTwClusDisplay->ResetHits();
      gEve->AddElement(fTwClusDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
      fCaClusDisplay->ResetHits();
      gEve->AddElement(fCaClusDisplay);
   }

   if (GlobalPar::GetPar()->IncludeKalman()) {
      fGlbTrackDisplay->ResetTracks();
      gEve->AddElement(fGlbTrackDisplay);
   }
}

//__________________________________________________________
void TAFOeventDisplay::ConnectElements()
{
   fStClusDisplay->SetEmitSignals(true);
   fStClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");

   fBmTrackDisplay->SetEmitSignals(true);
   fBmTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
    
    fBmDriftCircleDisplay->SetEmitSignals(true);
    fBmDriftCircleDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateDriftCircleInfo(TEveDigitSet*, Int_t)");
    

   fVtxClusDisplay->SetEmitSignals(true);
   fVtxClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   
   fVtxTrackDisplay->SetEmitSignals(true);
   fVtxTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
   
   fItClusDisplay->SetEmitSignals(true);
   fItClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   
   fMsdClusDisplay->SetEmitSignals(true);
   fMsdClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   
   fTwClusDisplay->SetEmitSignals(true);
   fTwClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");

}

//__________________________________________________________
void TAFOeventDisplay::UpdateHitInfo(TEveDigitSet* qs, Int_t idx)
{
   TAGclusterDisplay* quadHits = dynamic_cast<TAGclusterDisplay*> (qs);
   TObject* obj = quadHits->GetId(idx);
   
   if(obj == 0x0) return;
   
   if (obj->InheritsFrom("TAVTbaseCluster")) {
      TAVTcluster* clus = (TAVTcluster*)obj;
      if (clus == 0x0) return;
      TVector3 pos = clus->GetPositionG();
      fInfoView->AddLine( Form("Cluster # %3d\n", idx) );
      fInfoView->AddLine( Form("with %3d pixels in sensor %d\n", clus->GetPixelsN(), clus->GetPlaneNumber()) );
      fInfoView->AddLine( Form("at position: (%.3g %.3g) cm\n", pos.X(), pos.Y()) );
      
   } else if (obj->InheritsFrom("TAVTvertex")) {
      TAVTvertex* vtx = (TAVTvertex*)obj;
      if (vtx == 0x0) return;
      TVector3 pos = vtx->GetVertexPosition();
      fInfoView->AddLine( Form("Vertex# %d at position:\n", idx) );
      fInfoView->AddLine( Form(" (%.3g %.3g %.3g) cm\n", pos.X(), pos.Y(), pos.Z()) );
      fInfoView->AddLine( Form(" BM Matched %d\n", vtx->IsBmMatched()) );
      
   } else if (obj->InheritsFrom("TASTntuHit")) {
      TASTntuHit* hit = (TASTntuHit*)obj;
      fInfoView->AddLine( Form("Charge: %.3g u.a.\n", hit->GetCharge()) );
      fInfoView->AddLine( Form("Time: %.3g ps \n", hit->GetTime()) );

   } else if (obj->InheritsFrom("TATW_Point")) {
      TATWpoint* point = (TATWpoint*)obj;
      if (point == 0x0) return;
      TVector3 pos = point->GetPosition();
      fInfoView->AddLine( Form("Point# %d at position:\n", idx) );
      fInfoView->AddLine( Form(" (%.1f %.1f %.1f) cm\n", pos.X(), pos.Y(), pos.Z()) );
      fInfoView->AddLine( Form("Charge: %.3e u.a.\n", point->GetEnergyLoss()) );
      fInfoView->AddLine( Form("Time: %.3g ps \n", point->GetTime()) );
      fInfoView->AddLine( Form("Charge Z: %d \n", point->GetChargeZ()) );

   } else {
      return;
   }
}

//__________________________________________________________
void TAFOeventDisplay::UpdateTrackInfo(TEveDigitSet* qs, Int_t idx)
{
   TAGtrackDisplay* lineTracks = dynamic_cast<TAGtrackDisplay*> (qs);
   TObject* obj = lineTracks->GetId(idx);
   if (obj == 0x0) return;
   
   if (obj->InheritsFrom("TAVTtrack")) {
      
      TAVTtrack* track =  (TAVTtrack*)obj;
      if (track == 0x0) return;

      fInfoView->AddLine( Form("Track # %2d:", track->GetNumber()) );
      fInfoView->AddLine( Form(" with %3d clusters\n", track->GetClustersN()) );
      
      for (Int_t i = 0; i < track->GetClustersN(); i++) {
         TAVTcluster* clus = track->GetCluster(i);
         TVector3 posG = clus->GetPositionG();
         fInfoView->AddLine( Form(" for plane %d\n", clus->GetPlaneNumber()));
         fInfoView->AddLine( Form(" at position: (%.3g %.3g) \n", posG.X(), posG.Y()) );
         fInfoView->AddLine( Form(" with %d pixels\n", clus->GetPixelsN()));
      }
      
   } else if (obj->InheritsFrom("TABMntuTrackTr")) {
      TABMntuTrackTr* track =  (TABMntuTrackTr*)obj;
      if (track == 0x0) return;

      fInfoView->AddLine( Form("Track:") );
      fInfoView->AddLine( Form(" with %3d hit\n", track->GetNhit()) );
      fInfoView->AddLine( Form(" at Pvers: (%.3g %.3g) \n", track->GetPvers()[0], track->GetPvers()[1]) );
      fInfoView->AddLine( Form(" and R0 (%.3g %.3g)\n",  track->GetR0()[0], track->GetR0()[1]));
   }
}



//__________________________________________________________
void TAFOeventDisplay::UpdateDriftCircleInfo(TEveDigitSet* qs, Int_t idx)
{
    TEveBoxSet* tpCircle = dynamic_cast<TEveBoxSet*>(qs);
    
    TABMntuHit* hit = dynamic_cast<TABMntuHit*>( tpCircle->GetId(idx) );
    if(!hit){return;}
    
    TABMparGeo* pbmGeo = dynamic_cast<TABMparGeo*> (fpParGeoBm->Object());
    if(!pbmGeo){return;}
    
    fInfoView->AddLine( Form("In layer: %d, view: %d\n", hit->Plane(), hit->View()) );
    fInfoView->AddLine( Form("Wire is: %d\n", pbmGeo->GetSenseId( hit->Cell() )) );
    fInfoView->AddLine( Form("Drift radius is: %f (cm)\n", hit->Dist()) );
    
}

//__________________________________________________________
void TAFOeventDisplay::UpdateElements()
{
   if (fgGUIFlag)
      fEventEntry->SetText(Form("Run %d Event %d", fTAGroot->CurrentRunInfo().RunNumber(), fTAGroot->CurrentEventId().EventNumber()));
   
   if (GlobalPar::GetPar()->IncludeST())
      UpdateElements("st");
   
   if (GlobalPar::GetPar()->IncludeBM())
      UpdateElements("bm");

   if (GlobalPar::GetPar()->IncludeVertex())
      UpdateElements("vt");
   
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      UpdateElements("it");
   
   if (GlobalPar::GetPar()->IncludeMSD())
      UpdateElements("ms");
   
   if (GlobalPar::GetPar()->IncludeTW())
      UpdateElements("tw");
   
   if (GlobalPar::GetPar()->IncludeCA())
      UpdateElements("ca");

   gEve->FullRedraw3D(kFALSE);
}

//__________________________________________________________
void TAFOeventDisplay::UpdateElements(const TString prefix)
{
   if (prefix == "tw")
      UpdateBarElements();
   else if (prefix == "ca")
      UpdateCrystalElements();
   else if (prefix == "st")
      UpdateStcElements();
   else if (prefix == "bm") {
      UpdateLayerElements();
      if (fgTrackFlag)
         UpdateTrackElements(prefix);
   } else {
      UpdateQuadElements(prefix);
      if (fgTrackFlag) {
         UpdateTrackElements(prefix);
         if (GlobalPar::GetPar()->IncludeKalman())
            UpdateGlbTrackElements();
      }
   }
}

//__________________________________________________________
void TAFOeventDisplay::UpdateQuadElements(const TString prefix)
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      if (prefix == "vt") {
         fVtxClusDisplay->ResetHits();
      }  else if (prefix == "it") {
         fItClusDisplay->ResetHits();
      }  else if (prefix == "ms") {
         fMsdClusDisplay->ResetHits();
      } else
         return;
   }

   if (!fgDisplayFlag) // do not update event display
      return;
   
   Float_t  x = 0.,  y = 0.,  z = 0.;
   Float_t x1 = 0.,  y1 =0., z1 = 0.;
   
   TAVTbaseParGeo* parGeo = 0x0;
   
   if (prefix == "vt")
      parGeo = (TAVTparGeo*) fpParGeoVtx->Object();
   else if (prefix == "it")
      parGeo = (TAITparGeo*) fpParGeoIt->Object();
   else if (prefix == "ms")
      parGeo = (TAMSDparGeo*) fpParGeoMsd->Object();

   
   Int_t nPlanes = parGeo->GetNSensors();
   
   TAVTntuTrack*  pNtuTrack = 0x0;
   
   if (prefix == "vt") {
      if (fgTrackFlag && GlobalPar::GetPar()->IncludeTG()) {
         // vertex
         pNtuTrack = (TAVTntuTrack*)  fpNtuTrackVtx->Object();
         TAVTvertex*    vtxPD   = 0x0;//NEW
         TVector3 vtxPositionPD = pNtuTrack->GetBeamPosition();
         
         if (fpNtuVtx->Valid()) {
            TAVTntuVertex* pNtuVtxPD = (TAVTntuVertex*) fpNtuVtx->Object();
            for (Int_t iVtx = 0; iVtx < pNtuVtxPD->GetVertexN(); ++iVtx) {
               vtxPD = pNtuVtxPD->GetVertex(iVtx);
               if (vtxPD == 0x0) continue;
               vtxPositionPD = vtxPD->GetVertexPosition();
               vtxPositionPD = fpFootGeo->FromVTLocalToGlobal(vtxPositionPD);
               fVtxClusDisplay->AddHit(50, vtxPositionPD.X(), vtxPositionPD.Y(), vtxPositionPD.Z());
               fVtxClusDisplay->QuadId(vtxPD);
            }
         }
      }
   }
   
   TAVTntuCluster* pNtuClus  =  0x0;
   
   if (prefix == "vt")
      pNtuClus = (TAVTntuCluster*) fpNtuClusVtx->Object();
   else if (prefix == "it")
      pNtuClus = (TAVTntuCluster*) fpNtuClusIt->Object();
   else if (prefix == "ms")
      pNtuClus = (TAVTntuCluster*) fpNtuClusMsd->Object();

   for( Int_t iPlane = 0; iPlane < nPlanes; iPlane++) {
      
      Int_t nclus = pNtuClus->GetClustersN(iPlane);
      
      if (fTAGroot->CurrentRunInfo().RunNumber() == -1 && nclus == 0) // known bug if first event is empty
         fVtxClusDisplay->AddHit(50, 0, 0, 0);
      else if (nclus == 0) continue;
      
      for (Int_t iClus = 0; iClus < nclus; ++iClus) {
         TAVTcluster *clus = pNtuClus->GetCluster(iPlane, iClus);
         if (!clus->IsValid()) continue;
         TVector3 pos = clus->GetPositionG();
         TVector3 posG = pos;
         Int_t nPix = clus->GetListOfPixels()->GetEntries();
         if (prefix == "vt")
            posG = fpFootGeo->FromVTLocalToGlobal(posG);
         else if (prefix == "it")
            posG = fpFootGeo->FromITLocalToGlobal(posG);
         else if (prefix == "ms")
            posG = fpFootGeo->FromMSDLocalToGlobal(posG);


         x = posG(0);
         y = posG(1);
         z = posG(2);
         
         if (prefix == "vt") {
            fVtxClusDisplay->AddHit(nPix*10, x, y, z);
            fVtxClusDisplay->QuadId(clus);
         } else if (prefix == "it") {
            fItClusDisplay->AddHit(nPix*10, x, y, z);
            fItClusDisplay->QuadId(clus);
         } else if (prefix == "ms") {
            fMsdClusDisplay->AddHit(nPix*10, x, y, z);
            fMsdClusDisplay->QuadId(clus);
         }

      } //end loop on hits
      
   } //end loop on planes
   
   if (prefix == "vt")
      fVtxClusDisplay->RefitPlex();
   else if (prefix == "it")
      fItClusDisplay->RefitPlex();
   else if (prefix == "ms")
      fMsdClusDisplay->RefitPlex();
}

//__________________________________________________________
void TAFOeventDisplay::UpdateTrackElements(const TString prefix)
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      if (prefix == "bm")
         fBmTrackDisplay->ResetTracks();

      if (prefix == "vt")
         fVtxTrackDisplay->ResetTracks();
   }
   
   if (!fgDisplayFlag) // do not update event display
      return;
   
   Float_t  x = 0.,  y = 0.,  z = 0.;
   Float_t x1 = 0.,  y1 =0., z1 = 0.;
   
   
   if (prefix == "vt") {
      
      TAVTparGeo*  parGeo = (TAVTparGeo*) fpParGeoVtx->Object();
      
      Int_t nPlanes         = parGeo->GetNSensors();
      Float_t posfirstPlane = parGeo->GetSensorPosition(0)[2]*1.1;
      Float_t posLastPlane  = parGeo->GetSensorPosition(nPlanes-1)[2]*1.1;
      
      TAVTntuTrack*  pNtuTrack = (TAVTntuTrack*)  fpNtuTrackVtx->Object();
      
      if( pNtuTrack->GetTracksN() > 0 ) {
         for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
            fVtxTrackDisplay->AddNewTrack();
            
            TAVTtrack* track = pNtuTrack->GetTrack(iTrack);
            TVector3 pos;
            TVector3 posG;
            
            if (GlobalPar::GetPar()->IncludeTG() && track->GetValidity() == 1)
               pos = track->Intersection(track->GetVertexZ());
            else
               pos = track->Intersection(posfirstPlane);
            
            posG = fpFootGeo->FromVTLocalToGlobal(pos);
            
            x = posG(0); y = posG(1); z = posG(2);
            
            if (GlobalPar::GetPar()->IncludeTW()) {
               Float_t posZtw = fpFootGeo->FromTWLocalToGlobal(TVector3(0,0,0)).Z();
               posZtw = fpFootGeo->FromGlobalToVTLocal(TVector3(0, 0, posZtw)).Z();
               pos = track->Intersection(posZtw);
            } else {
               pos  = track->Intersection(posLastPlane);
            }
            
            posG = fpFootGeo->FromVTLocalToGlobal(pos);

            x1 = posG(0); y1 = posG(1); z1 = posG(2);
            
            Float_t nPix = track->GetMeanPixelsN();
            fVtxTrackDisplay->AddTracklet(nPix*10, x, y, z, x1, y1, z1);
            fVtxTrackDisplay->TrackId(track);
            
         } // end loop on tracks
         
      } // nTracks > 0
      fVtxTrackDisplay->RefitPlex();
   }
   
   if (prefix == "bm") {
      TABMparGeo*  parGeo = (TABMparGeo*) fpParGeoBm->Object();

      TABMntuTrack*  pNtuTrack = (TABMntuTrack*)  fpNtuTrackBm->Object();
      
      if( pNtuTrack->GetTracksN() > 0 ) {
         
         for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
            fBmTrackDisplay->AddNewTrack();
            
            TABMntuTrackTr* track = pNtuTrack->Track(iTrack);

            TVector3 A0 = track->GetMylar1Pos();
            TVector3 A1 = track->GetMylar2Pos();
            
            A0[2] *= 1.1;
            A1[2] *= 1.1;
            
            if (GlobalPar::GetPar()->IncludeTG()) {
               Float_t posZtg = fpFootGeo->FromTGLocalToGlobal(TVector3(0,0,0)).Z();
               posZtg = fpFootGeo->FromGlobalToBMLocal(TVector3(0, 0, posZtg)).Z();
               A1 = track->PointAtLocalZ(posZtg);
            }
            
            TVector3 A0G = fpFootGeo->FromBMLocalToGlobal(A0);
            TVector3 A1G = fpFootGeo->FromBMLocalToGlobal(A1);
            
            x  = A0G(0); y  = A0G(1); z  = A0G(2);
            x1 = A1G(0); y1 = A1G(1); z1 = A1G(2);
            
            Int_t nHits = track->GetNhit();
            // inverse view ??
            fBmTrackDisplay->AddTracklet(nHits*100, y, x, z, y1, x1, z1);
            fBmTrackDisplay->TrackId(track);
            
         } // end loop on tracks
         
      } // nTracks > 0
      fBmTrackDisplay->RefitPlex();
   }
}

//__________________________________________________________
void TAFOeventDisplay::UpdateBarElements()
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
         fTwClusDisplay->ResetHits();
   }
   
   if (!fgDisplayFlag) // do not update event display
      return;
   
   Float_t  x = 0.,  y = 0.,  z = 0.;
   
   // Color bar
   TATWparGeo* parGeo = parGeo = (TATWparGeo*) fpParGeoTw->Object();

   // reset previous fired bar
   if ( fRefreshButton->IsOn()) {
      map< pair<int, int>, int >::iterator it;
      for (it = fFiredTofBar.begin(); it != fFiredTofBar.end(); it++) {
         pair<int, int> idx = it->first;
         Int_t iBar = idx.first;
         Int_t iLayer = idx.second;
         parGeo->SetBarColorOff(iBar, iLayer);
      }
   }

   fFiredTofBar.clear();

   TATWntuRaw* pNtuHit = (TATWntuRaw*) fpNtuRawTw->Object();
   
   for( Int_t iLayer = 0; iLayer < parGeo->GetNLayers(); iLayer++) {
      
      Int_t nHits = pNtuHit->GetHitN(iLayer);
      if (nHits == 0) continue;

      for (Int_t iHit = 0; iHit < nHits; ++iHit) {
         
         TATWntuHit *hit = pNtuHit->GetHit(iLayer, iHit);

         Int_t iBar = hit->GetBar();

         pair<int, int> idx(iBar, iLayer);
         
         fFiredTofBar[idx] = 1;
         parGeo->SetBarColorOn(iBar, iLayer);

      } //end loop on hits
      
   } //end loop on planes

   
   // Draw Quad
   TATWntuPoint* pNtuPoint = (TATWntuPoint*) fpNtuRecTw->Object();

   Int_t nPoints = pNtuPoint->GetPointN();
   
   for (Int_t iPoint = 0; iPoint < nPoints; ++iPoint) {
      
      TATWpoint *point = pNtuPoint->GetPoint(iPoint);
      
      TVector3 posG = point->GetPosition();
      posG = fpFootGeo->FromTWLocalToGlobal(posG);
      
      Float_t edep = point->GetEnergyLoss();

      fTwClusDisplay->AddHit(edep/1e5, posG[0], posG[1], posG[2]);
      fTwClusDisplay->QuadId(point);
   } //end loop on points
   
   
   fTwClusDisplay->RefitPlex();
}


//__________________________________________________________
void TAFOeventDisplay::UpdateCrystalElements()
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
         fCaClusDisplay->ResetHits();
   }
   
   if (!fgDisplayFlag) // do not update event display
      return;
   
   Float_t  x = 0.,  y = 0.,  z = 0.;
   
   TACAparGeo* parGeo = (TACAparGeo*) fpParGeoCa->Object();
   
   // reset previous fired bar
   if ( fRefreshButton->IsOn()) {
      map<int, int >::iterator it;
      for (it = fFiredCaCrystal.begin(); it != fFiredCaCrystal.end(); it++) {
         Int_t idx = it->first;
         parGeo->SetCrystalColorOff(idx);
      }
   }
   
   fFiredCaCrystal.clear();
   
   TACAntuRaw* pNtuHit = (TACAntuRaw*) fpNtuRawCa->Object();
   
   Int_t nHits = pNtuHit->GetHitsN();
   if (nHits == 0) return;
   
   for (Int_t iHit = 0; iHit < nHits; ++iHit) {
      
      TACAntuHit *hit = pNtuHit->GetHit(iHit);
      
      Int_t idx = hit->GetCrystalId();
      
      fFiredCaCrystal[idx] = 1;
      parGeo->SetCrystalColorOn(idx);
      
   } //end loop on hits
   

   fCaClusDisplay->RefitPlex();
}

//__________________________________________________________
void TAFOeventDisplay::UpdateStcElements()
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      fStClusDisplay->ResetHits();
   }
   
   if (!fgDisplayFlag) // do not update event display
      return;

   //STC
   TASTntuRaw* pSTntu = (TASTntuRaw*) fpNtuRawSt->Object();
   Int_t       nHits  = pSTntu->GetHitsN();

   //hits
   cout<<" TAFO:: nHits ST "<<nHits<<endl;
   for (Int_t i = 0; i < nHits; i++) {

      TASTntuHit* hit = pSTntu->Hit(i);
      Float_t charge = hit->GetCharge();
   
      TVector3 posHit(0,0,0); // center
      
      TVector3 posHitG = fpFootGeo->FromSTLocalToGlobal(posHit);
      
      fStClusDisplay->AddHit(charge, posHitG[0], posHitG[1], posHitG[2]);
      fStClusDisplay->QuadId(hit);
   }
   
   fStClusDisplay->RefitPlex();
}

//__________________________________________________________
void TAFOeventDisplay::UpdateLayerElements()
{
    TABMparGeo* pbmGeo = (TABMparGeo*) fpParGeoBm->Object();
    
    
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      fBmClusDisplay->ResetWires();
       fBmDriftCircleDisplay->Reset(TEveBoxSet::kBT_Cone, kFALSE, 32);
       for(auto l = 0; l < 2 * pbmGeo->GetLayersN() ; ++l) {  pbmGeo->SetLayerColorOff(l); }
   }
   
   if (!fgDisplayFlag) // do not update event display
      return;

   TABMntuRaw* pBMntu = (TABMntuRaw*) fpNtuRawBm->Object();
   Int_t       nHits  = pBMntu->GetHitsN();
    
   
  
   double bm_h_side = pbmGeo->GetWidth();
    
    
   //hits
   for (Int_t i = 0; i < nHits; i++) {
      TABMntuHit* hit = pBMntu->Hit(i);
      
      Int_t view  = hit->View();
      Int_t lay  = hit->Plane();
      Int_t cell  = hit->Cell();
       

      //layer
       pbmGeo->SetLayerColorOn(lay + view * pbmGeo->GetLayersN());
       
       
       //wires
      Float_t x = pbmGeo->GetWireX(pbmGeo->GetSenseId(cell),lay,view);
      Float_t y = pbmGeo->GetWireY(pbmGeo->GetSenseId(cell),lay,view);
      Float_t z = pbmGeo->GetWireZ(pbmGeo->GetSenseId(cell),lay,view);

      TVector3 posHit(x, y, z);
      TVector3 posHitG = fpFootGeo->FromBMLocalToGlobal(posHit);
       
       
       TEveVector tPosCWG( static_cast<float>( posHitG.X() ),
                           static_cast<float>( posHitG.Y() ),
                           static_cast<float>( posHitG.Z() ));
       TEveVector tDir{0, 0, 0};

      if(view == 1) {
         //X,Z, top view
         fBmClusDisplay->AddWire(posHitG(0), posHitG(1), posHitG(2), posHitG(0), posHitG(1)+bm_h_side, posHitG(2));
          
          tPosCWG[1]+=bm_h_side/2;
          tDir[1]=0.01;
          
      } else {
         //Y,Z, side view
         fBmClusDisplay->AddWire(posHitG(0), posHitG(1), posHitG(2), posHitG(0)+bm_h_side, posHitG(1), posHitG(2));
          
          tPosCWG[0]+=bm_h_side/2;
          tDir[0]=0.01;
          
      }
       
       
       fBmDriftCircleDisplay->AddCone(tPosCWG, tDir, hit->Dist());
       fBmDriftCircleDisplay->DigitId(hit);
       
   }
   
   fBmClusDisplay->RefitPlex();
}

//__________________________________________________________
void TAFOeventDisplay::UpdateGlbTrackElements()
{
   TVector3 vtx(0,0,0);
   TVector3 momentum(0,0,3);
   Int_t charge = 6;
   fGlbTrackDisplay->AddTrack(vtx, momentum, charge);
   
   fGlbTrackDisplay->MakeTracks();
}

//__________________________________________________________
void TAFOeventDisplay::UpdateDefCanvases()
{
   Int_t nCanvas = fListOfCanvases->GetEntries();
   Int_t nHisto = fHistoList->GetEntries();

   for (Int_t k = 0; k < nHisto; ++k) {
      
      Int_t iCanvas = k / fgMaxHistosN;
      if (iCanvas > 2) continue;
      TCanvas* canvas = (TCanvas*)fListOfCanvases->At(iCanvas);
      if (!canvas) continue;
         
      TH1* h = (TH1*)fHistoList->At(k);
      Int_t iCd = k % fgMaxHistosN + 1;

      if (nHisto == 1)
         canvas->cd();
      else
         canvas->cd(iCd);
      h->Draw();
      
      canvas->Update();
   }
}

//__________________________________________________________
void TAFOeventDisplay::CreateCanvases()
{
   // GUI
   // histo
   TCanvas* canvas = 0x0;
   TVirtualPad* pad    = 0x0;
   TEveWindowSlot* slot0 = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
   TEveWindowTab*  tab0 = slot0->MakeTab();
   tab0->SetElementName("Histograms");
   tab0->SetShowTitleBar(kFALSE);
   
   // canvas tab
   slot0 = tab0->NewSlot();
   TRootEmbeddedCanvas* eCanvas00 = new TRootEmbeddedCanvas();
   TEveWindowFrame* frame00 = slot0->MakeFrame(eCanvas00);
   frame00->SetElementName("Histograms 1");
   canvas = eCanvas00->GetCanvas();
   canvas->SetName("HistoCanvas 1");
   canvas->Resize();
   fListOfCanvases->Add(canvas);
   
   slot0 = tab0->NewSlot();
   TRootEmbeddedCanvas* eCanvas01 = new TRootEmbeddedCanvas();
   TEveWindowFrame* frame01 = slot0->MakeFrame(eCanvas01);
   frame01->SetElementName("Histograms 2");
   canvas = eCanvas01->GetCanvas();
   canvas->SetName("HistoCanvas 2");
   canvas->Resize();
   fListOfCanvases->Add(canvas);
   
   slot0 = tab0->NewSlot();
   TRootEmbeddedCanvas* eCanvas02 = new TRootEmbeddedCanvas();
   TEveWindowFrame* frame02 = slot0->MakeFrame(eCanvas02);
   frame02->SetElementName("Histograms 3");
   canvas = eCanvas02->GetCanvas();
   canvas->SetName("HistoCanvas 3");
   canvas->Resize();
   fListOfCanvases->Add(canvas);
   
   frmMain->MapSubwindows();
   frmMain->Resize();
   frmMain->MapWindow();
}
