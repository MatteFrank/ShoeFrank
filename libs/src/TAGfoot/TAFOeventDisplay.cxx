

#include "TAFOeventDisplay.hxx"

// root include
#include "Riostream.h"
#include "TRootEmbeddedCanvas.h"
#include "TEveManager.h"
#include "TEveWindow.h"
#include "TEveBrowser.h"
#include "TEveBrowser.h"

#include "GlobalPar.hxx"

#include "TASTdatRaw.hxx"
#include "TASTntuRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TAVTntuRaw.hxx"
#include "TAITntuRaw.hxx"
#include "TAMSDntuRaw.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuRaw.hxx"

#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"

#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

ClassImp(TAFOeventDisplay)

Bool_t  TAFOeventDisplay::fgTrackFlag       = true;
TString TAFOeventDisplay::fgVtxTrackingAlgo = "Std";
Bool_t  TAFOeventDisplay::fgStdAloneFlag    = false;
Bool_t  TAFOeventDisplay::fgBmSelectHt      = false;

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
 : TAEDbaseInterface(expName),
   fType(type),
   fStClusDisplay(0x0),
   fBmClusDisplay(0x0),
   fBmTrackDisplay(0x0),
   fBmDriftCircleDisplay(0x0),
   fVtxClusDisplay(0x0),
   fVtxTrackDisplay(0x0),
   fItClusDisplay(0x0),
   fMsdClusDisplay(0x0),
   fTwClusDisplay(0x0),
   fCaClusDisplay(0x0),
   fGlbTrackDisplay(0x0),
   fFieldImpl(0x0),
   fField(0x0),
   fGlbTrackProp(0x0)
{
   // local reco
   SetLocalReco();
   
   // Par instance
   GlobalPar::Instance();

   // default constructon
   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeBM()) {
      fStClusDisplay = new TAEDcluster("Start counter hit");
      fStClusDisplay->SetMaxEnergy(fMaxEnergy);
      fStClusDisplay->SetDefWidth(fQuadDefWidth/2.);
      fStClusDisplay->SetDefHeight(fQuadDefHeight/2.);
      fStClusDisplay->SetPickable(true);
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fBmClusDisplay = new TAEDwire("Beam Monitoring Wires");
      fBmClusDisplay->SetPickable(true);
      
      fBmTrackDisplay = new TAEDtrack("Beam Monitoring Tracks");
      fBmTrackDisplay->SetMaxEnergy(fMaxEnergy);
      fBmTrackDisplay->SetDefWidth(fBoxDefWidth);
      fBmTrackDisplay->SetDefHeight(fBoxDefHeight);
      fBmTrackDisplay->SetPickable(true);
      
      fBmDriftCircleDisplay = new TEveBoxSet("Beam Monitoring Drift Circle");
      fBmDriftCircleDisplay->SetPickable(true);
   }
   
   if (GlobalPar::GetPar()->IncludeVertex()) {
      fVtxClusDisplay = new TAEDcluster("Vertex Cluster");
      fVtxClusDisplay->SetMaxEnergy(fMaxEnergy);
      fVtxClusDisplay->SetDefWidth(fQuadDefWidth/2.);
      fVtxClusDisplay->SetDefHeight(fQuadDefHeight/2.);
      fVtxClusDisplay->SetPickable(true);
   
      fVtxTrackDisplay = new TAEDtrack("Vertex Tracks");
      fVtxTrackDisplay->SetMaxEnergy(fMaxEnergy/2.);
      fVtxTrackDisplay->SetDefWidth(fBoxDefWidth);
      fVtxTrackDisplay->SetDefHeight(fBoxDefHeight);
      fVtxTrackDisplay->SetPickable(true);
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fItClusDisplay = new TAEDcluster("Inner tracker Cluster");
      fItClusDisplay->SetMaxEnergy(fMaxEnergy);
      fItClusDisplay->SetDefWidth(fQuadDefWidth*2.);
      fItClusDisplay->SetDefHeight(fQuadDefHeight*2.);
      fItClusDisplay->SetPickable(true);
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fMsdClusDisplay = new TAEDcluster("Multi Strip Cluster");
      fMsdClusDisplay->SetMaxEnergy(fMaxEnergy);
      fMsdClusDisplay->SetDefWidth(fQuadDefWidth);
      fMsdClusDisplay->SetDefHeight(fQuadDefHeight);
      fMsdClusDisplay->SetPickable(true);
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      fTwClusDisplay = new TAEDcluster("Tof Wall hit");
      fTwClusDisplay->SetMaxEnergy(fMaxEnergy);
      fTwClusDisplay->SetDefWidth(fQuadDefWidth*8);
      fTwClusDisplay->SetDefHeight(fQuadDefHeight*8);
      fTwClusDisplay->SetPickable(true);
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
      fCaClusDisplay = new TAEDcluster("Calorimeter hit");
      fCaClusDisplay->SetMaxEnergy(fMaxEnergy);
      fCaClusDisplay->SetDefWidth(fQuadDefWidth*4);
      fCaClusDisplay->SetDefHeight(fQuadDefHeight*4);
      fCaClusDisplay->SetPickable(true);
   }
   
   if (GlobalPar::GetPar()->IncludeKalman()) {
      fGlbTrackProp    = new TADIeveTrackPropagator();
      fGlbTrackDisplay = new TAEDglbTrack("Global Tracks", fGlbTrackProp);
      fGlbTrackDisplay->SetMaxMomentum(fMaxMomentum);
   }
}

//__________________________________________________________
TAFOeventDisplay::~TAFOeventDisplay()
{
   // default destructor
   if (fStClusDisplay)        delete fStClusDisplay;
   if (fBmClusDisplay)        delete fBmClusDisplay;
   if (fBmDriftCircleDisplay) delete fBmDriftCircleDisplay;
   if (fBmTrackDisplay)       delete fBmTrackDisplay;
   if (fVtxClusDisplay)       delete fVtxClusDisplay;
   if (fVtxTrackDisplay)      delete fVtxTrackDisplay;
   if (fItClusDisplay)        delete fItClusDisplay;
   if (fMsdClusDisplay)       delete fMsdClusDisplay;
   if (fTwClusDisplay)        delete fTwClusDisplay;
   if (fCaClusDisplay)        delete fCaClusDisplay;
   if (fGlbTrackDisplay)      delete fGlbTrackDisplay;
   
   if (fField)                delete fField;
   if (fFieldImpl)            delete fFieldImpl;
   if (fGlbTrackProp)         delete fGlbTrackProp;
   
   delete fReco;
}

//__________________________________________________________
void TAFOeventDisplay::SetLocalReco()
{
   if (fType != 0) return;
   
   // local reco
   fReco = new GlobalReco(fExpName);
   
   fReco->DisableTree();
   fReco->DisableSaveHits();
   fReco->EnableHisto();
   
   if (fgTrackFlag) {
      fReco->SetTrackingAlgo(fgVtxTrackingAlgo[0]);
      fReco->EnableTracking();
   }
   
   if (fgStdAloneFlag)
      LocalReco::EnableStdAlone();
   
   fpFootGeo = fReco->GetGeoTrafo();
}

//__________________________________________________________
void TAFOeventDisplay::ReadParFiles()
{
   fReco->ReadParFiles();
   
   // Set field for propagator if field defined
   if (GlobalPar::GetPar()->IncludeDI()) {
      TADIparGeo* parGeo = fReco->GetParGeoDi();
      
      fFieldImpl = new FootField("", parGeo);
      fField     = new TADIeveField(fFieldImpl);
      
      if (GlobalPar::GetPar()->IncludeKalman()) {
         fGlbTrackDisplay->GetPropagator()->SetMagFieldObj(fField);
         fGlbTrackDisplay->GetPropagator()->SetMaxZ(fWorldSizeZ);
         fGlbTrackDisplay->GetPropagator()->SetMaxR(fWorldSizeXY);
      }
   }
          
   TAVTparConf::SetHistoMap();
}

//__________________________________________________________
void TAFOeventDisplay::BuildDefaultGeometry()
{
   TAEDbaseInterface::BuildDefaultGeometry();
   
   // ST
   if (GlobalPar::GetPar()->IncludeST()) {
      TASTparGeo* parGeo = fReco->GetParGeoSt();
      TGeoVolume* irVol  = parGeo->BuildStartCounter();
   
      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TASTparGeo::GetBaseName());
      AddGeometry(irVol, transfo);
   }

   // BM
   if (GlobalPar::GetPar()->IncludeBM()) {
      TABMparGeo* parGeo = fReco->GetParGeoBm();;
      TGeoVolume* bmVol  = parGeo->BuildBeamMonitor();
      
      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TABMparGeo::GetBaseName());
      AddGeometry(bmVol, transfo);
   }

   // target
   if (GlobalPar::GetPar()->IncludeTG()) {
      TAGparGeo* parGeo = fReco->GetParGeoG();;
      TGeoVolume* tgVol = parGeo->BuildTarget();
      
      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TAGparGeo::GetBaseName());
      AddGeometry(tgVol, transfo);
   }

   // Vertex
   if (GlobalPar::GetPar()->IncludeVertex()) {
      TAVTparGeo* parGeo = fReco->GetParGeoVtx();
      TGeoVolume* vtVol  = parGeo->BuildVertex();
      
      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TAVTparGeo::GetBaseName());
      AddGeometry(vtVol, transfo);
   }

   // Magnet
   if (GlobalPar::GetPar()->IncludeDI()) {
      TADIparGeo* parGeo = fReco->GetParGeoDi();
      TGeoVolume* vtVol = parGeo->BuildMagnet();
      
      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TADIparGeo::GetBaseName());
      AddGeometry(vtVol, transfo);
   }

   // IT
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      TAITparGeo* parGeo = fReco->GetParGeoIt();
      TGeoVolume* itVol  = parGeo->BuildInnerTracker();
      
      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TAITparGeo::GetItBaseName());
      AddGeometry(itVol, transfo);
   }
   
   // MSD
   if (GlobalPar::GetPar()->IncludeMSD()) {
      TAMSDparGeo* parGeo = fReco->GetParGeoMsd();
      TGeoVolume* msdVol = parGeo->BuildMultiStripDetector();
      
      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TAMSDparGeo::GetBaseName());
      AddGeometry(msdVol, transfo);
   }

   // TW
   if (GlobalPar::GetPar()->IncludeTW()) {
      TATWparGeo* parGeo = fReco->GetParGeoTw();;
      TGeoVolume* twVol = parGeo->BuildTofWall();
      
      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TATWparGeo::GetBaseName());
      AddGeometry(twVol, transfo);
   }

   // CA
   if (GlobalPar::GetPar()->IncludeCA()) {
      TACAparGeo* parGeo = fReco->GetParGeoCa();
      TGeoVolume* caVol = parGeo->BuildCalorimeter();
      
      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TACAparGeo::GetBaseName());
      AddGeometry(caVol, transfo);
   }
}

//__________________________________________________________
void TAFOeventDisplay::CreateRecAction()
{
   fReco->CreateRecAction();
}

//__________________________________________________________
void TAFOeventDisplay::CreateRawAction()
{
   fReco->CreateRawAction();
}

//__________________________________________________________
void TAFOeventDisplay::SetFileName(const TString fileName)
{
   fReco->SetName(fileName);
   fReco->SetRunNumber();
}

//__________________________________________________________
void TAFOeventDisplay::OpenFile()
{
   fReco->OpenFileIn();
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
   fReco->AddRawRequiredItem();
   fReco->AddRecRequiredItem();
   
   gTAGroot->BeginEventLoop();
   gTAGroot->Print();
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
   if (GlobalPar::GetPar()->IncludeST()){
      fStClusDisplay->SetEmitSignals(true);
      fStClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fBmTrackDisplay->SetEmitSignals(true);
      fBmTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
      fBmDriftCircleDisplay->SetEmitSignals(true);
      fBmDriftCircleDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateDriftCircleInfo(TEveDigitSet*, Int_t)");
   }

   if (GlobalPar::GetPar()->IncludeVertex()) {
      fVtxClusDisplay->SetEmitSignals(true);
      fVtxClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
      
      fVtxTrackDisplay->SetEmitSignals(true);
      fVtxTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fItClusDisplay->SetEmitSignals(true);
      fItClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fMsdClusDisplay->SetEmitSignals(true);
      fMsdClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      fTwClusDisplay->SetEmitSignals(true);
      fTwClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFOeventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }
}

//__________________________________________________________
void TAFOeventDisplay::UpdateHitInfo(TEveDigitSet* qs, Int_t idx)
{
   TAEDcluster* quadHits = dynamic_cast<TAEDcluster*> (qs);
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
      if (hit == 0x0) return;
      fInfoView->AddLine( Form("Charge: %.3g u.a.\n", hit->GetCharge()) );
      fInfoView->AddLine( Form("Time: %.3g ps \n", hit->GetTime()) );

   } else if (obj->InheritsFrom("TATWpoint")) {
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
   TAEDtrack* lineTracks = dynamic_cast<TAEDtrack*> (qs);
   TObject* obj = lineTracks->GetId(idx);
   if (obj == 0x0) return;
   
   if (obj->InheritsFrom("TAVTtrack")) {
      
      TAVTtrack* track =  (TAVTtrack*)obj;
      if (track == 0x0) return;

      fInfoView->AddLine( Form("Track # %2d:", track->GetNumber()) );
      fInfoView->AddLine( Form(" with %3d clusters\n", track->GetClustersN()) );
      
      for (Int_t i = 0; i < track->GetClustersN(); i++) {
         TAVTbaseCluster* clus = track->GetCluster(i);
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
    
    TABMparGeo* pbmGeo = fReco->GetParGeoBm();;
    if(!pbmGeo){return;}
    
    fInfoView->AddLine( Form("In layer: %d, view: %d\n", hit->Plane(), hit->View()) );
    fInfoView->AddLine( Form("Wire is: %d\n", pbmGeo->GetSenseId( hit->Cell() )) );
    fInfoView->AddLine( Form("Drift radius is: %g (cm)\n", hit->Dist()) );
    fInfoView->AddLine( Form("Chi2 is: %g \n", hit->GetChi2()) );
}

//__________________________________________________________
void TAFOeventDisplay::UpdateElements()
{
   if (fgGUIFlag)
      fEventEntry->SetText(Form("Run %d Event %d", gTAGroot->CurrentRunInfo().RunNumber(), gTAGroot->CurrentEventId().EventNumber()));
   
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
      parGeo = fReco->GetParGeoVtx();
   else if (prefix == "it")
      parGeo = fReco->GetParGeoIt();
   else if (prefix == "ms")
      parGeo = fReco->GetParGeoMsd();

   // known bug if first event is empty
   if (fVtxClusDisplay)
      fVtxClusDisplay->AddHit(-1, 0, 0, 0);
   
   
   Int_t nPlanes = parGeo->GetSensorsN();
   
   TAVTntuTrack*  pNtuTrack = 0x0;
   
   if (prefix == "vt") {
      if (fgTrackFlag && GlobalPar::GetPar()->IncludeTG()) {
         // vertex
         pNtuTrack = (TAVTntuTrack*)  fReco->GetNtuTrackVtx();
         TAVTvertex*    vtxPD   = 0x0;//NEW
         TVector3 vtxPositionPD = pNtuTrack->GetBeamPosition();
         
         if (fReco->GetDscVertexVtx()->Valid()) {
            TAVTntuVertex* pNtuVtxPD = fReco->GetNtuVertexVtx();
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
      pNtuClus = fReco->GetNtuClusterVtx();
   else if (prefix == "it")
      pNtuClus = (TAVTntuCluster*)fReco->GetNtuClusterIt();
   else if (prefix == "ms")
      pNtuClus = (TAVTntuCluster*)fReco->GetNtuClusterVtx();

   for( Int_t iPlane = 0; iPlane < nPlanes; iPlane++) {
      
      Int_t nclus = pNtuClus->GetClustersN(iPlane);
      
     if (nclus == 0) continue;
      
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
      
      TAVTparGeo*  parGeo   = fReco->GetParGeoVtx();
      Int_t nPlanes         = parGeo->GetSensorsN();
      Float_t posfirstPlane = parGeo->GetSensorPosition(0)[2]*1.1;
      Float_t posLastPlane  = parGeo->GetSensorPosition(nPlanes-1)[2]*1.1;
      
      TAVTntuTrack* pNtuTrack = fReco->GetNtuTrackVtx();
      
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
      TABMparGeo*   parGeo    = fReco->GetParGeoBm();
      TABMntuTrack* pNtuTrack = fReco->GetNtuTrackBm();
      
      if( pNtuTrack->GetTracksN() > 0 ) {
         
         for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
            fBmTrackDisplay->AddNewTrack();
            
            TABMntuTrackTr* track = pNtuTrack->Track(iTrack);

            TVector3 A0 = track->PointAtLocalZ(parGeo->GetMylar1().Z());
            TVector3 A1 = track->PointAtLocalZ(parGeo->GetMylar2().Z());
            
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
   TATWparGeo* parGeo = parGeo = fReco->GetParGeoTw();

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

   TATWntuRaw* pNtuHit = fReco->GetNtuHitTw();
   
   for( Int_t iLayer = 0; iLayer < parGeo->GetNLayers(); iLayer++) {
      
      Int_t nHits = pNtuHit->GetHitN(iLayer);
      if (nHits == 0) continue;

      for (Int_t iHit = 0; iHit < nHits; ++iHit) {
         
         TATWntuHit *hit = pNtuHit->GetHit(iHit, iLayer);

         if(!hit) continue;

         Int_t iBar = hit->GetBar();

         pair<int, int> idx(iBar, iLayer);
         
         fFiredTofBar[idx] = 1;
         parGeo->SetBarColorOn(iBar, iLayer);

      } //end loop on hits
      
   } //end loop on planes

   
   // Draw Quad
   TATWntuPoint* pNtuPoint =  fReco->GetNtuPointTw();;

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
   
   TACAparGeo* parGeo = fReco->GetParGeoCa();;
   
   // reset previous fired bar
   if ( fRefreshButton->IsOn()) {
      map<int, int >::iterator it;
      for (it = fFiredCaCrystal.begin(); it != fFiredCaCrystal.end(); it++) {
         Int_t idx  = TACAparGeo::GetCrystalId(it->first);
         Int_t iMod = TACAparGeo::GetModuleId(it->first);
         parGeo->SetCrystalColorOff(idx, iMod);
      }
   }
   
   fFiredCaCrystal.clear();
   
   TACAntuRaw* pNtuHit = fReco->GetNtuHitCa();
   
   Int_t nHits = pNtuHit->GetHitsN();
   if (nHits == 0) return;
   
   for (Int_t iHit = 0; iHit < nHits; ++iHit) {
      
      TACAntuHit *hit = pNtuHit->GetHit(iHit);
      
      Int_t idx = hit->GetCrystalId();
      Int_t iMod = hit->GetModuleId();

      fFiredCaCrystal[idx + iMod*TACAparGeo::GetCrystalsNperModule()] = 1;
      parGeo->SetCrystalColorOn(idx, iMod);
      
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
   TASTntuRaw* pSTntu = fReco->GetNtuHitSt();
   Int_t       nHits  = pSTntu->GetHitsN();

   //hits
   if(FootDebugLevel(1))
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
   TABMparGeo* pbmGeo = fReco->GetParGeoBm();;
   
    
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      fBmClusDisplay->ResetWires();
       fBmDriftCircleDisplay->Reset(TEveBoxSet::kBT_Cone, kFALSE, 32);
       for(auto l = 0; l < 2 * pbmGeo->GetLayersN() ; ++l) {  pbmGeo->SetLayerColorOff(l); }
   }
   
   if (!fgDisplayFlag) // do not update event display
      return;

   TABMntuRaw* pBMntu = fReco->GetNtuRawBm();
   Int_t       nHits  = pBMntu->GetHitsN();
   Double_t bm_h_side = pbmGeo->GetWidth();
   
   //hits
   for (Int_t i = 0; i < nHits; i++) {
      TABMntuHit* hit = pBMntu->Hit(i);
      
      if (!hit->GetIsSelected() && fgBmSelectHt) continue;

      Int_t view  = hit->View();
      Int_t lay  = hit->Plane();
      Int_t cell  = hit->Cell();
       
      //layer
       pbmGeo->SetLayerColorOn(lay + view * pbmGeo->GetLayersN());
      
       //wires
      Float_t x = pbmGeo->GetWireX(pbmGeo->GetSenseId(cell),lay,view);
      Float_t y = pbmGeo->GetWireY(pbmGeo->GetSenseId(cell),lay,view);
      Float_t z = pbmGeo->GetWireZ(pbmGeo->GetSenseId(cell),lay,view);

      // wire pos
      TVector3 posHit(x, y, z);
      TVector3 posHitG = fpFootGeo->FromBMLocalToGlobal(posHit);
      
      TVector3 posHit1(x, y+bm_h_side, z);
      TVector3 posHit1G = fpFootGeo->FromBMLocalToGlobal(posHit1);
      
      TVector3 posHit0(x+bm_h_side, y, z);
      TVector3 posHit0G = fpFootGeo->FromBMLocalToGlobal(posHit0);
      
      // radius pos
      TVector3 tPosCW1(x, y+bm_h_side/2., z);
      TVector3 tPosCW1G = fpFootGeo->FromBMLocalToGlobal(tPosCW1);
      
      TVector3 tPosCW0(x+bm_h_side/2., y, z);
      TVector3 tPosCW0G = fpFootGeo->FromBMLocalToGlobal(tPosCW0);
      
      
      TEveVector tPosCWG(0, 0, 0);
      TEveVector tDir{0, 0, 0};

      if(view == 1) {
         //X,Z, top view
         fBmClusDisplay->AddWire(posHitG(0), posHitG(1), posHitG(2), posHit1G(0), posHit1G(1), posHit1G(2));
          
         tPosCWG[0] = tPosCW1G[0];
         tPosCWG[1] = tPosCW1G[1];
         tPosCWG[2] = tPosCW1G[2];
         tDir[1]=0.01;
         
      } else {
         //Y,Z, side view
         fBmClusDisplay->AddWire(posHitG(0), posHitG(1), posHitG(2), posHit0G(0), posHit0G(1), posHit0G(2));
          
         tPosCWG[0] = tPosCW0G[0];
         tPosCWG[1] = tPosCW0G[1];
         tPosCWG[2] = tPosCW0G[2];
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
