

#include "TAFObaseEventDisplay.hxx"

// root include
#include "Riostream.h"
#include "TGComboBox.h"
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

#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "LocalRecoNtuMC.hxx"

ClassImp(TAFObaseEventDisplay)

Bool_t  TAFObaseEventDisplay::fgTrackFlag       = true;
TString TAFObaseEventDisplay::fgVtxTrackingAlgo = "Std";
Bool_t  TAFObaseEventDisplay::fgStdAloneFlag    = false;
Bool_t  TAFObaseEventDisplay::fgBmSelectHit     = false;
Bool_t  TAFObaseEventDisplay::fgM28ClusMtFlag   = false;


//__________________________________________________________
TAFObaseEventDisplay::TAFObaseEventDisplay(const TString expName, Int_t runNumber, Int_t type)
 : TAEDbaseInterface(type, expName, runNumber),
   fStClusDisplay(0x0),
   fBmClusDisplay(0x0),
   fBmTrackDisplay(0x0),
   fBmDriftCircleDisplay(0x0),
   fVtxClusDisplay(0x0),
   fVtxTrackDisplay(0x0),
   fItClusDisplay(0x0),
   fItTrackDisplay(0x0),
   fMsdClusDisplay(0x0),
   fTwClusDisplay(0x0),
   fCaClusDisplay(0x0),
   fGlbTrackDisplay(0x0),
   fIrTrackDisplay(0x0),
   fIrFlag(false),
   fFieldImpl(0x0),
   fField(0x0),
   fGlbTrackProp(0x0)
{
   // Par instance
   GlobalPar::Instance(expName);

   // default constructon
   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeBM()) {
      fStClusDisplay = new TAEDcluster("Start counter Hit");
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

   if (GlobalPar::GetPar()->IncludeVT()) {
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

   if (GlobalPar::GetPar()->IncludeIT()) {
      fItClusDisplay = new TAEDcluster("Inner Tracker Cluster");
      fItClusDisplay->SetMaxEnergy(fMaxEnergy);
      fItClusDisplay->SetDefWidth(fQuadDefWidth*2.);
      fItClusDisplay->SetDefHeight(fQuadDefHeight*2.);
      fItClusDisplay->SetPickable(true);

      if (IsItrTracking()) {
         fItTrackDisplay = new TAEDtrack("Inner Tracker Track");
         fItTrackDisplay->SetMaxEnergy(fMaxEnergy/2.);
         fItTrackDisplay->SetDefWidth(fBoxDefWidth);
         fItTrackDisplay->SetDefHeight(fBoxDefHeight);
         fItTrackDisplay->SetPickable(true);
      }
   }

   if (GlobalPar::GetPar()->IncludeMSD()) {
      fMsdClusDisplay = new TAEDcluster("Multi Strip Cluster");
      fMsdClusDisplay->SetMaxEnergy(fMaxEnergy);
      fMsdClusDisplay->SetDefWidth(fQuadDefWidth);
      fMsdClusDisplay->SetDefHeight(fQuadDefHeight);
      fMsdClusDisplay->SetPickable(true);
   }

   if (GlobalPar::GetPar()->IncludeTW()) {
      fTwClusDisplay = new TAEDcluster("Tof Wall Hit");
      fTwClusDisplay->SetMaxEnergy(fMaxEnergy);
      fTwClusDisplay->SetDefWidth(fQuadDefWidth*8);
      fTwClusDisplay->SetDefHeight(fQuadDefHeight*8);
      fTwClusDisplay->SetPickable(true);
   }

   if (GlobalPar::GetPar()->IncludeCA()) {
      fCaClusDisplay = new TAEDcluster("Calorimeter Cluster");
      fCaClusDisplay->SetMaxEnergy(fMaxEnergy);
      fCaClusDisplay->SetDefWidth(fQuadDefWidth*4);
      fCaClusDisplay->SetDefHeight(fQuadDefHeight*4);
      fCaClusDisplay->SetPickable(true);
   }

   if (GlobalPar::GetPar()->IncludeTOE()) {
      fGlbTrackProp    = new TADIeveTrackPropagator();
      fGlbTrackDisplay = new TAEDglbTrackList("Global Tracks", fGlbTrackProp);
      fGlbTrackDisplay->SetMaxMomentum(fMaxMomentum);
   }

   if (GlobalPar::GetPar()->IncludeST() && GlobalPar::GetPar()->IncludeTG() &&
       GlobalPar::GetPar()->IncludeBM() && GlobalPar::GetPar()->IncludeVT() &&
       GlobalPar::GetPar()->IncludeIT() && !GlobalPar::GetPar()->IncludeDI()) {

      fIrTrackDisplay = new TAEDtrack("Interaction Region Tracks");
      fIrTrackDisplay->SetMaxEnergy(fMaxEnergy/2.);
      fIrTrackDisplay->SetDefWidth(fBoxDefWidth);
      fIrTrackDisplay->SetDefHeight(fBoxDefHeight);
      fIrTrackDisplay->SetPickable(true);
      fIrFlag = true;
   }
}

//__________________________________________________________
TAFObaseEventDisplay::~TAFObaseEventDisplay()
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
   if (fIrTrackDisplay)       delete fIrTrackDisplay;
   if (fItTrackDisplay)       delete fItTrackDisplay;

   if (fField)                delete fField;
   if (fGlbTrackProp)         delete fGlbTrackProp;

   delete fReco;
}

//__________________________________________________________
void TAFObaseEventDisplay::ReadParFiles()
{
   fReco->ReadParFiles();

   // Set field for propagator if field defined
   if (GlobalPar::GetPar()->IncludeDI()) {
      TADIparGeo* parGeo = fReco->GetParGeoDi();

      fFieldImpl = fReco->GetFootField();
      fField     = new TADIeveField(fFieldImpl);

      if (GlobalPar::GetPar()->IncludeTOE()) {
         fGlbTrackDisplay->GetPropagator()->SetMagFieldObj(fField);
         fGlbTrackDisplay->GetPropagator()->SetMaxZ(fWorldSizeZ);
         fGlbTrackDisplay->GetPropagator()->SetMaxR(fWorldSizeXY);
      }
   }

   TAVTparConf::SetHistoMap();
}

//__________________________________________________________
void TAFObaseEventDisplay::BuildDefaultGeometry()
{
   TAEDbaseInterface::BuildDefaultGeometry();

   // ST
   if (GlobalPar::GetPar()->IncludeST()) {
      TASTparGeo* parGeo = fReco->GetParGeoSt();
      TGeoVolume* irVol  = parGeo->BuildStartCounter();
      fVolumeNames[irVol->GetName()] = kSTC;


      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TASTparGeo::GetBaseName());
      AddGeometry(irVol, transfo);
   }

   // BM
   if (GlobalPar::GetPar()->IncludeBM()) {
      TABMparGeo* parGeo = fReco->GetParGeoBm();;
      TGeoVolume* bmVol  = parGeo->BuildBeamMonitor();
      fVolumeNames[bmVol->GetName()] = kBMN;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TABMparGeo::GetBaseName());
      AddGeometry(bmVol, transfo);
   }

   // target
   if (GlobalPar::GetPar()->IncludeTG()) {
      TAGparGeo* parGeo = fReco->GetParGeoG();;
      TGeoVolume* tgVol = parGeo->BuildTarget();
      fVolumeNames[tgVol->GetName()] = kTGT;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TAGparGeo::GetBaseName());
      AddGeometry(tgVol, transfo);
   }

   // Vertex
   if (GlobalPar::GetPar()->IncludeVT()) {
      TAVTparGeo* parGeo = fReco->GetParGeoVtx();
      TGeoVolume* vtVol  = parGeo->BuildVertex();
      fVolumeNames[vtVol->GetName()] = kVTX;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TAVTparGeo::GetBaseName());
      AddGeometry(vtVol, transfo);
   }

   // Magnet
   if (GlobalPar::GetPar()->IncludeDI() || GlobalPar::GetPar()->IncludeTOE()) {
      TADIparGeo* parGeo = fReco->GetParGeoDi();
      TGeoVolume* mgVol = parGeo->BuildMagnet();
      fVolumeNames[mgVol->GetName()] = kDIP;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TADIparGeo::GetBaseName());
      AddGeometry(mgVol, transfo);
   }

   // IT
   if (GlobalPar::GetPar()->IncludeIT()) {
      TAITparGeo* parGeo = fReco->GetParGeoIt();
      TGeoVolume* itVol  = parGeo->BuildInnerTracker();
      fVolumeNames[itVol->GetName()] = kITR;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TAITparGeo::GetBaseName());
      AddGeometry(itVol, transfo);
   }

   // MSD
   if (GlobalPar::GetPar()->IncludeMSD()) {
      TAMSDparGeo* parGeo = fReco->GetParGeoMsd();
      TGeoVolume* msdVol = parGeo->BuildMultiStripDetector();
      fVolumeNames[msdVol->GetName()] = kMSD;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TAMSDparGeo::GetBaseName());
      AddGeometry(msdVol, transfo);
   }

   // TW
   if (GlobalPar::GetPar()->IncludeTW()) {
      TATWparGeo* parGeo = fReco->GetParGeoTw();;
      TGeoVolume* twVol = parGeo->BuildTofWall();
      fVolumeNames[twVol->GetName()] = kTOF;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TATWparGeo::GetBaseName());
      AddGeometry(twVol, transfo);
   }

   // CA
   if (GlobalPar::GetPar()->IncludeCA()) {
      TACAparGeo* parGeo = fReco->GetParGeoCa();
      TGeoVolume* caVol = parGeo->BuildCalorimeter();
      fVolumeNames[caVol->GetName()] = kCAL;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TACAparGeo::GetBaseName());
      AddGeometry(caVol, transfo);
   }
}

//__________________________________________________________
void TAFObaseEventDisplay::CreateRecAction()
{
   fReco->CreateRecAction();
}

//__________________________________________________________
void TAFObaseEventDisplay::CreateRawAction()
{
   fReco->CreateRawAction();
}

//__________________________________________________________
void TAFObaseEventDisplay::SetFileName(const TString fileName)
{
   fReco->SetName(fileName);
   fReco->SetRunNumber(fRunNumber);
}

//__________________________________________________________
void TAFObaseEventDisplay::OpenFile()
{
   fReco->OpenFileIn();
}

//__________________________________________________________
void TAFObaseEventDisplay::AddRequiredItem()
{
   fReco->AddRawRequiredItem();
   fReco->AddRecRequiredItem();

   gTAGroot->BeginEventLoop();
   gTAGroot->Print();
}


//__________________________________________________________
void TAFObaseEventDisplay::AddElements()
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

   if (GlobalPar::GetPar()->IncludeVT()) {
      fVtxClusDisplay->ResetHits();
      gEve->AddElement(fVtxClusDisplay);

      fVtxTrackDisplay->ResetTracks();
      gEve->AddElement(fVtxTrackDisplay);
   }

   if (GlobalPar::GetPar()->IncludeIT()) {
      fItClusDisplay->ResetHits();
      gEve->AddElement(fItClusDisplay);

      if (IsItrTracking()) {
         fItTrackDisplay->ResetTracks();
         gEve->AddElement(fItTrackDisplay);
      }
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

   if (GlobalPar::GetPar()->IncludeTOE()) {
      fGlbTrackDisplay->ResetTracks();
      gEve->AddElement(fGlbTrackDisplay);
   }

   if (GlobalPar::GetPar()->IncludeST() && GlobalPar::GetPar()->IncludeTG() &&
       GlobalPar::GetPar()->IncludeBM() && GlobalPar::GetPar()->IncludeVT() &&
       GlobalPar::GetPar()->IncludeIT() && !GlobalPar::GetPar()->IncludeDI()) {
      fIrTrackDisplay->ResetTracks();
      gEve->AddElement(fIrTrackDisplay);
   }
}

//__________________________________________________________
void TAFObaseEventDisplay::ConnectElements()
{
   if (GlobalPar::GetPar()->IncludeST()){
      fStClusDisplay->SetEmitSignals(true);
      fStClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }

   if (GlobalPar::GetPar()->IncludeBM()) {
      fBmTrackDisplay->SetEmitSignals(true);
      fBmTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
      fBmDriftCircleDisplay->SetEmitSignals(true);
      fBmDriftCircleDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateDriftCircleInfo(TEveDigitSet*, Int_t)");
   }

   if (GlobalPar::GetPar()->IncludeVT()) {
      fVtxClusDisplay->SetEmitSignals(true);
      fVtxClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");

      fVtxTrackDisplay->SetEmitSignals(true);
      fVtxTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
   }

   if (GlobalPar::GetPar()->IncludeIT()) {
      fItClusDisplay->SetEmitSignals(true);
      fItClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");

      if (IsItrTracking()) {
         fItTrackDisplay->SetEmitSignals(true);
         fItTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
      }
   }

   if (GlobalPar::GetPar()->IncludeMSD()) {
      fMsdClusDisplay->SetEmitSignals(true);
      fMsdClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }

   if (GlobalPar::GetPar()->IncludeTW()) {
      fTwClusDisplay->SetEmitSignals(true);
      fTwClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }

   if (GlobalPar::GetPar()->IncludeCA()) {
      fCaClusDisplay->SetEmitSignals(true);
      fCaClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }
   
   if (GlobalPar::GetPar()->IncludeST() && GlobalPar::GetPar()->IncludeTG() &&
       GlobalPar::GetPar()->IncludeBM() && GlobalPar::GetPar()->IncludeVT() &&
       GlobalPar::GetPar()->IncludeIT() && !GlobalPar::GetPar()->IncludeDI()) {
      fIrTrackDisplay->SetEmitSignals(true);
      fIrTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAFObaseEventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
   }

   if (GlobalPar::GetPar()->IncludeTOE()) {
      TQObject::Connect("TEveTrack", "SecSelected(TEveTrack*)", "TAFObaseEventDisplay", this, "UpdateTrackInfo(TEveTrack*)");
   }
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateHitInfo(TEveDigitSet* qs, Int_t idx)
{
   TAEDcluster* quadHits = dynamic_cast<TAEDcluster*> (qs);
   TObject* obj = quadHits->GetId(idx);

   if(obj == 0x0) return;

   if (obj->InheritsFrom("TAVTbaseCluster")) {
      TAVTcluster* clus = (TAVTcluster*)obj;
      if (clus == 0x0) return;
      TVector3 pos = clus->GetPositionG();
      fInfoView->AddLine( Form("Cluster # %3d\n", idx) );
      fInfoView->AddLine( Form("with %3d pixels in sensor %d\n", clus->GetPixelsN(), clus->GetSensorIdx()) );
      fInfoView->AddLine( Form("at position: (%.3g %.3g) cm\n", pos.X(), pos.Y()) );

      if (fConsoleButton->IsOn()) {
         cout << Form("Cluster # %3d\n", idx);
         cout << Form("with %3d pixels in sensor %d\n", clus->GetPixelsN(), clus->GetSensorIdx());
         cout << Form("at position: (%.3g %.3g) cm\n", pos.X(), pos.Y());
      }

   } else if (obj->InheritsFrom("TAMSDcluster")) {
         TAMSDcluster* clus = (TAMSDcluster*)obj;
         if (clus == 0x0) return;
         TVector3 pos = clus->GetPositionG();
         fInfoView->AddLine( Form("Cluster # %3d\n", idx) );
         fInfoView->AddLine( Form("with %3d strips in sensor %d\n", clus->GetStripsN(), clus->GetSensorIdx()) );
         fInfoView->AddLine( Form("at position: (%.3g %.3g) cm\n", pos.X(), pos.Y()) );

      if (fConsoleButton->IsOn()) {
         cout << Form("Cluster # %3d\n", idx);
         cout << Form("with %3d strips in sensor %d\n", clus->GetStripsN(), clus->GetSensorIdx());
         cout << Form("at position: (%.3g %.3g) cm\n", pos.X(), pos.Y());
      }

   } else if (obj->InheritsFrom("TAVTvertex")) {
      TAVTvertex* vtx = (TAVTvertex*)obj;
      if (vtx == 0x0) return;
      TVector3 pos = vtx->GetPosition();
      TVector3 err = vtx->GetPosError();
      fInfoView->AddLine( Form("Vertex# %d at position:\n", idx) );
      fInfoView->AddLine( Form(" (%.3g %.3g %.3g) cm\n", pos.X(), pos.Y(), pos.Z()) );
      fInfoView->AddLine( Form(" (%.3g %.3g %.3g) cm\n", err.X(), err.Y(), err.Z()) );
      fInfoView->AddLine( Form(" BM Matched %d (valid: %d)\n", vtx->IsBmMatched(), vtx->GetValidity()) );
      fInfoView->AddLine( Form(" With %d tracks\n",  vtx->GetTracksN()) );

      if (fConsoleButton->IsOn()) {
         cout << Form("Vertex# %d at position:\n", idx) << endl;
         cout << Form(" (%.3g %.3g %.3g) cm\n", pos.X(), pos.Y(), pos.Z());
         cout << Form(" (%.3g %.3g %.3g) cm\n", err.X(), err.Y(), err.Z());
         cout << Form(" BM Matched %d (valid: %d)\n", vtx->IsBmMatched(), vtx->GetValidity());
         cout << Form(" With %d tracks\n",  vtx->GetTracksN());
      }

   } else if (obj->InheritsFrom("TASTntuHit")) {
      TASTntuHit* hit = (TASTntuHit*)obj;
      if (hit == 0x0) return;
      fInfoView->AddLine( Form("Charge: %.3g u.a.\n", hit->GetCharge()) );
      fInfoView->AddLine( Form("Time: %.3g ps \n", hit->GetTime()) );

      if (fConsoleButton->IsOn()) {
         cout << Form("Charge: %.3g u.a.\n", hit->GetCharge());
         cout << Form("Time: %.3g ps \n", hit->GetTime());
      }

   } else if (obj->InheritsFrom("TATWpoint")) {
      TATWpoint* point = (TATWpoint*)obj;
      if (point == 0x0) return;
      TVector3 pos = point->GetPosition();
      fInfoView->AddLine( Form("Point# %d at position:\n", idx) );
      fInfoView->AddLine( Form(" (%.1f %.1f %.1f) cm\n", pos.X(), pos.Y(), pos.Z()) );
      fInfoView->AddLine( Form("Charge: %.3e u.a.\n", point->GetEnergyLoss()) );
      fInfoView->AddLine( Form("Time: %.3g ps \n", point->GetTime()) );
      fInfoView->AddLine( Form("Charge Z: %d \n", point->GetChargeZ()) );

      if (fConsoleButton->IsOn()) {
         cout <<  Form("Point# %d at position:\n", idx);
         cout <<  Form(" (%.1f %.1f %.1f) cm\n", pos.X(), pos.Y(), pos.Z());
         cout <<  Form("Charge: %.3e u.a.\n", point->GetEnergyLoss());
         cout <<  Form("Time: %.3g ps \n", point->GetTime());
         cout <<  Form("Charge Z: %d \n", point->GetChargeZ());
      }
      
   } else if (obj->InheritsFrom("TACAcluster")) {
      TACAcluster* clus = (TACAcluster*)obj;
      if (clus == 0x0) return;
      TVector3 pos = clus->GetPositionG();
      fInfoView->AddLine( Form("Cluster # %3d\n", idx) );
      fInfoView->AddLine( Form("with %3d hits\n", clus->GetHitsN()) );
      fInfoView->AddLine( Form("Energy: %.3g (GeV) \n", clus->GetCharge()*TAGgeoTrafo::MevToGev()) );
      fInfoView->AddLine( Form("at position: (%.3g %.3g) cm\n", pos.X(), pos.Y()) );
      
      if (fConsoleButton->IsOn()) {
         cout << Form("Cluster # %3d\n", idx);
         cout << Form("with %3d hits\n", clus->GetHitsN());
         cout << Form("Energy: %.3g (MeV) \n", clus->GetCharge()*TAGgeoTrafo::MevToGev());
         cout << Form("at position: (%.3g %.3g) cm\n", pos.X(), pos.Y());
      }

   } else {
      return;
   }
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateTrackInfo(TEveDigitSet* qs, Int_t idx)
{
   TAEDtrack* lineTracks = dynamic_cast<TAEDtrack*> (qs);
   TObject* obj = lineTracks->GetId(idx);
   if (obj == 0x0) return;

   if (obj->InheritsFrom("TAVTbaseTrack")) {

      TAVTbaseTrack* track =  (TAVTbaseTrack*)obj;
      if (track == 0x0) return;

      fInfoView->AddLine( Form("Track # %2d (valid: %d): \n", track->GetNumber(), track->GetValidity()) );
      fInfoView->AddLine( Form(" with %3d clusters\n", track->GetClustersN()) );
      if (fConsoleButton->IsOn()) {
         cout << Form("Track # %2d (valid: %d): \n", track->GetNumber(), track->GetValidity());
         cout << Form(" with %3d clusters\n", track->GetClustersN());
      }

      for (Int_t i = 0; i < track->GetClustersN(); i++) {
         TAVTbaseCluster* clus = track->GetCluster(i);
         TVector3 posG = clus->GetPositionG();
         fInfoView->AddLine( Form(" for plane %d\n", clus->GetSensorIdx()));
         fInfoView->AddLine( Form(" at position: (%.3g %.3g) \n", posG.X(), posG.Y()) );
         fInfoView->AddLine( Form(" with %d pixels\n", clus->GetPixelsN()));

         if (fConsoleButton->IsOn()) {
            cout <<  Form(" for plane %d\n", clus->GetSensorIdx());
            cout <<  Form(" at position: (%.3g %.3g) \n", posG.X(), posG.Y());
            cout <<  Form(" with %d pixels\n", clus->GetPixelsN());
         }
      }

   } else if (obj->InheritsFrom("TABMtrack")) {
      TABMtrack* track =  (TABMtrack*)obj;
      if (track == 0x0) return;

      fInfoView->AddLine( Form("Track:\n") );
      fInfoView->AddLine( Form(" with %3d hit\n", track->GetHitsNtot()) );
      fInfoView->AddLine( Form(" at Slope: (%.3g %.3g) \n", track->GetSlope()[0], track->GetSlope()[1]) );
      fInfoView->AddLine( Form(" and Origin (%.3g %.3g)\n",  track->GetOrigin()[0], track->GetOrigin()[1]));

      if (fConsoleButton->IsOn()) {
         cout <<  Form("Track:\n");
         cout <<  Form(" with %3d hit\n", track->GetHitsNtot());
         cout <<  Form(" at Slope: (%.3g %.3g) \n", track->GetSlope()[0], track->GetSlope()[1]);
         cout <<  Form(" and Origin (%.3g %.3g)\n",  track->GetOrigin()[0], track->GetOrigin()[1]);
      }
   }
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateTrackInfo(TEveTrack* ts)
{
   TAEDglbTrack* lineTracks = dynamic_cast<TAEDglbTrack*> (ts);
   TObject* obj = lineTracks->GetTrackId();

   if (obj->InheritsFrom("TAGtrack")) {

      TAGtrack* track =  (TAGtrack*)obj;
      if (track == 0x0) return;

      fInfoView->AddLine( Form("Track # %2d with %2d points\n", track->GetTrackId(), track->GetMeasPointsN()) );
      fInfoView->AddLine( Form("Charge: %d A: %d Mass: %.2f GeV/c2\n", track->GetCharge(), TMath::Nint(track->GetMass()/TAGgeoTrafo::GetMassFactor()), track->GetMass()) );
      fInfoView->AddLine( Form("Momentum: %.2f GeV/c ToF: %.2f ns\n", track->GetMomentum(), track->GetTof()) );

      if (fConsoleButton->IsOn()) {
         cout <<  Form("Track # %2d with %2d points\n", track->GetTrackId(), track->GetMeasPointsN());
         cout <<  Form("Charge: %d A: %d Mass: %.2f GeV/c2\n", track->GetCharge(), TMath::Nint(track->GetMass()/TAGgeoTrafo::GetMassFactor()), track->GetMass());
         cout <<  Form("Momentum: %.2f GeV/c ToF: %.2f ns\n", track->GetMomentum(), track->GetTof());

         for( Int_t iPoint = 0; iPoint < track->GetMeasPointsN(); ++iPoint ) {
            TAGpoint* point = track->GetMeasPoint(iPoint);
            cout << Form("%-3s: #%2d ", point->GetDevName(), iPoint);
            cout << Form("Momentum: (%.2f %.2f %.2f) GeV/c ", point->GetMomentum()[0], point->GetMomentum()[1], point->GetMomentum()[2]);

           if (fType != 0) {
             for (Int_t k = 0; k < point->GetMcTracksN(); ++k) {
               Int_t idx = point->GetMcTrackIdx(k);
               cout << Form(" MCtrackIdx: %d ", idx);
             }
           }
           cout << endl;
         }
      }
   }
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateDriftCircleInfo(TEveDigitSet* qs, Int_t idx)
{
    TEveBoxSet* tpCircle = dynamic_cast<TEveBoxSet*>(qs);

    TABMntuHit* hit = dynamic_cast<TABMntuHit*>( tpCircle->GetId(idx) );
    if(!hit){return;}

    TABMparGeo* pbmGeo = fReco->GetParGeoBm();;
    if(!pbmGeo){return;}

    fInfoView->AddLine( Form("In layer: %d, view: %d\n", hit->GetPlane(), hit->GetView()) );
    fInfoView->AddLine( Form("Wire is: %d\n", pbmGeo->GetSenseId( hit->GetCell() )) );
    fInfoView->AddLine( Form("Drift radius is: %g (cm)\n", hit->GetRdrift()) );
    fInfoView->AddLine( Form("Chi2 is: %g \n", hit->GetChi2()) );

   if (fConsoleButton->IsOn()) {
      cout << Form("In layer: %d, view: %d\n", hit->GetPlane(), hit->GetView());
      cout <<  Form("Wire is: %d\n", pbmGeo->GetSenseId( hit->GetCell() ));
      cout <<  Form("Drift radius is: %g (cm)\n", hit->GetRdrift());
      cout <<  Form("Chi2 is: %g \n", hit->GetChi2());
   }
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateElements()
{
   if (fgGUIFlag)
      fEventEntry->SetText(Form("Run %d Event %d", gTAGroot->CurrentRunNumber(), gTAGroot->CurrentEventId().EventNumber()));

   if (GlobalPar::GetPar()->IncludeST())
      UpdateElements("st");

   if (GlobalPar::GetPar()->IncludeBM())
      UpdateElements("bm");

   if (GlobalPar::GetPar()->IncludeVT())
      UpdateElements("vt");

   if (GlobalPar::GetPar()->IncludeIT())
      UpdateElements("it");

   if (GlobalPar::GetPar()->IncludeMSD())
      UpdateElements("ms");

   if (GlobalPar::GetPar()->IncludeTW())
      UpdateElements("tw");

   if (GlobalPar::GetPar()->IncludeCA())
      UpdateElements("ca");

   if (GlobalPar::GetPar()->IncludeST() && GlobalPar::GetPar()->IncludeTG() &&
       GlobalPar::GetPar()->IncludeBM() && GlobalPar::GetPar()->IncludeVT() &&
       GlobalPar::GetPar()->IncludeIT() && !GlobalPar::GetPar()->IncludeDI())
      UpdateElements("ir");

   if (GlobalPar::GetPar()->IncludeTOE() && fgTrackFlag)
      UpdateGlbTrackElements();

}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateElements(const TString prefix)
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
   } else if (prefix == "ms") {
      UpdateStripElements();
   } else {
      UpdateQuadElements(prefix);
      if (fgTrackFlag)
         UpdateTrackElements(prefix);
   }
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateQuadElements(const TString prefix)
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      if (prefix == "vt") {
         fVtxClusDisplay->ResetHits();
      }  else if (prefix == "it") {
         fItClusDisplay->ResetHits();
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
               vtxPositionPD = vtxPD->GetPosition();
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
         x = posG(0);
         y = posG(1);
         z = posG(2);

         if (prefix == "vt") {
            fVtxClusDisplay->AddHit(nPix*10, x, y, z);
            fVtxClusDisplay->QuadId(clus);
         } else if (prefix == "it") {
            fItClusDisplay->AddHit(nPix*10, x, y, z);
            fItClusDisplay->QuadId(clus);
         }

      } //end loop on hits

   } //end loop on planes

   if (prefix == "vt")
      fVtxClusDisplay->RefitPlex();
   else if (prefix == "it")
      fItClusDisplay->RefitPlex();
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateStripElements()
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn()))
      fMsdClusDisplay->ResetHits();

   if (!fgDisplayFlag) // do not update event display
      return;

   Float_t  x = 0.,  y = 0.,  z = 0.;

   TAMSDparGeo* parGeo = fReco->GetParGeoMsd();
   Float_t pitch = parGeo->GetPitch()*5;
   TVector3 epi  = parGeo->GetEpiSize();
   Int_t nPlanes = parGeo->GetSensorsN();

   TAMSDntuCluster* pNtuClus  = (TAMSDntuCluster*)fReco->GetNtuClusterMsd();

   for( Int_t iPlane = 0; iPlane < nPlanes; iPlane++) {

      Int_t nclus = pNtuClus->GetClustersN(iPlane);

      if (nclus == 0) continue;

      for (Int_t iClus = 0; iClus < nclus; ++iClus) {
         TAMSDcluster *clus = pNtuClus->GetCluster(iPlane, iClus);
         if (!clus->IsValid()) continue;
         TVector3 pos = clus->GetPositionG();
         TVector3 posG = fpFootGeo->FromMSDLocalToGlobal(pos);
         Int_t nStrip = clus->GetListOfStrips()->GetEntries();

         x = posG(0);
         y = posG(1);
         z = posG(2);

         if (clus->GetPlaneView() == 0)
            fMsdClusDisplay->AddHit(nStrip*10, x, y, z, pitch, epi[1]);
         else
            fMsdClusDisplay->AddHit(nStrip*10, x, y, z, epi[0], pitch);

         fMsdClusDisplay->QuadId(clus);

      } //end loop on hits

   } //end loop on planes

   fMsdClusDisplay->RefitPlex();
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateTrackElements(const TString prefix)
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      if (prefix == "bm")
         fBmTrackDisplay->ResetTracks();

      if (prefix == "vt")
         fVtxTrackDisplay->ResetTracks();

      if (prefix == "ir")
         fIrTrackDisplay->ResetTracks();

      if (prefix == "it" && IsItrTracking())
         fItTrackDisplay->ResetTracks();
   }

   if (!fgDisplayFlag) // do not update event display
      return;

   Float_t  x = 0.,  y = 0.,  z = 0.;
   Float_t x1 = 0.,  y1 =0., z1 = 0.;


   if (prefix == "vt" && !fIrFlag) {

      TAVTparGeo*  parGeo   = fReco->GetParGeoVtx();
      Int_t nPlanes         = parGeo->GetSensorsN();
      Float_t posfirstPlane = parGeo->GetSensorPosition(0)[2]*1.1;
      Float_t posLastPlane  = parGeo->GetSensorPosition(nPlanes-1)[2]*1.1;

      TAVTntuTrack* pNtuTrack = fReco->GetNtuTrackVtx();

      for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
         fVtxTrackDisplay->AddNewTrack();

         TAVTtrack* track = pNtuTrack->GetTrack(iTrack);
         TVector3 pos;
         TVector3 posG;

         if (GlobalPar::GetPar()->IncludeTG())
            pos = track->Intersection(track->GetPosVertex().Z());
         else
            pos = track->Intersection(posfirstPlane);

         posG = fpFootGeo->FromVTLocalToGlobal(pos);

         x = posG(0); y = posG(1); z = posG(2);

         if (GlobalPar::GetPar()->IncludeTW() && !GlobalPar::GetPar()->IncludeDI()) {
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

      fVtxTrackDisplay->RefitPlex();
   }

   if (prefix == "it" && !fIrFlag && IsItrTracking()) {

      TAITparGeo*  parGeo   = fReco->GetParGeoIt();
      Int_t nPlanes         = parGeo->GetSensorsN();
      Float_t posfirstPlane = parGeo->GetSensorPosition(0)[2]*1.1;
      Float_t posLastPlane  = parGeo->GetSensorPosition(nPlanes-1)[2]*1.1;

      TAITntuTrack* pNtuTrack = fReco->GetNtuTrackIt();

      for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
         fItTrackDisplay->AddNewTrack();

         TAITtrack* track = pNtuTrack->GetTrack(iTrack);
         TVector3 pos;
         TVector3 posG;

         if (GlobalPar::GetPar()->IncludeTG() ) {
            Float_t posZtg = fpFootGeo->FromTGLocalToGlobal(TVector3(0,0,0)).Z();
            posZtg = fpFootGeo->FromGlobalToITLocal(TVector3(0, 0, posZtg)).Z();
            pos = track->Intersection(posZtg);
         } else
            pos  = track->Intersection(posfirstPlane);

         posG = fpFootGeo->FromITLocalToGlobal(pos);
         x = posG(0); y = posG(1); z = posG(2);

         pos  = track->Intersection(posLastPlane);
         posG = fpFootGeo->FromITLocalToGlobal(pos);
         x1 = posG(0); y1 = posG(1); z1 = posG(2);

         Float_t nPix = track->GetMeanPixelsN();
         fItTrackDisplay->AddTracklet(nPix*10, x, y, z, x1, y1, z1);
         fItTrackDisplay->TrackId(track);

      } // end loop on tracks

      fItTrackDisplay->RefitPlex();
   }

   if (prefix == "bm") {
      TABMparGeo*   parGeo    = fReco->GetParGeoBm();
      TABMntuTrack* pNtuTrack = fReco->GetNtuTrackBm();

      for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
         fBmTrackDisplay->AddNewTrack();

         TABMtrack* track = pNtuTrack->GetTrack(iTrack);

         TVector3 A0 = track->Intersection(parGeo->GetMylar1().Z());
         TVector3 A1 = track->Intersection(parGeo->GetMylar2().Z());

         if (GlobalPar::GetPar()->IncludeTG()) {
            Float_t posZtg = fpFootGeo->FromTGLocalToGlobal(TVector3(0,0,0)).Z();
            posZtg = fpFootGeo->FromGlobalToBMLocal(TVector3(0, 0, posZtg)).Z();
            A1 = track->Intersection(posZtg);
         }

         TVector3 A0G = fpFootGeo->FromBMLocalToGlobal(A0);
         TVector3 A1G = fpFootGeo->FromBMLocalToGlobal(A1);

         x  = A0G(0); y  = A0G(1); z  = A0G(2);
         x1 = A1G(0); y1 = A1G(1); z1 = A1G(2);

         Int_t nHits = track->GetHitsNtot();
         // inverse view ??
         fBmTrackDisplay->AddTracklet(nHits*100, x, y, z, x1, y1, z1);
         fBmTrackDisplay->TrackId(track);

      } // end loop on tracks

      fBmTrackDisplay->RefitPlex();
   }

   if (prefix == "ir") {

      TAITparGeo*  parGeo   = fReco->GetParGeoIt();
      Int_t nPlanes         = parGeo->GetSensorsN();
      Float_t posfirstPlane = 0.;
      Float_t posLastPlane  = parGeo->GetSensorPosition(nPlanes-1)[2]*1.1;

      // Go to FOOT frame
      TVector3 pos(0,0,posLastPlane);
      pos = fpFootGeo->FromITLocalToGlobal(pos);
      posLastPlane = pos.Z();

      TAIRntuTrack* pNtuTrack = fReco->GetNtuTrackIr();

      for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
         fIrTrackDisplay->AddNewTrack();

         TAIRtrack* track = pNtuTrack->GetTrack(iTrack);
         TVector3 posG(0, 0, 0);

         posG  = track->Intersection(0);
         x = posG(0); y = posG(1); z = posG(2);


         if (GlobalPar::GetPar()->IncludeTW()) {
            Float_t posZtw = fpFootGeo->FromTWLocalToGlobal(TVector3(0,0,0)).Z();
            posG = track->Intersection(posZtw);
         } else {
            posG  = track->Intersection(posLastPlane);
         }

         x1 = posG(0); y1 = posG(1); z1 = posG(2);

         Float_t nPix = track->GetMeanPixelsN();
         fIrTrackDisplay->AddTracklet(nPix*10, x, y, z, x1, y1, z1);
         fIrTrackDisplay->TrackId(track);

      } // end loop on tracks


      fIrTrackDisplay->RefitPlex();
   }
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateGlbTrackElements()
{
   TAGntuGlbTrack* pNtuTrack = fReco->GetNtuGlbTrack();
   fGlbTrackDisplay->ResetTracks();

   for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
      TAGtrack* track = pNtuTrack->GetTrack(iTrack);

      // vertex
      TAGpoint* point = track->GetMeasPoint(0);
      TVector3 vtx    = point->GetPosition();
      TVector3 mom0   = point->GetMomentum();
      Int_t charge    = point->GetChargeZ();

      TAEDglbTrack* glbTrack = fGlbTrackDisplay->AddTrack(vtx, mom0, charge);
      glbTrack->TrackId(track);

      for( Int_t iPoint = 1; iPoint < track->GetMeasPointsN(); ++iPoint ) {
         TAGpoint* point = track->GetMeasPoint(iPoint);
         TVector3 pos    = point->GetPosition();
         TVector3 mom    = point->GetMomentum();

         glbTrack->AddTrackPoint(pos, mom);
      } // end loop on points
   } // end loop on tracks
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateBarElements()
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

   for( Int_t iLayer = 0; iLayer < parGeo->GetLayersN(); iLayer++) {

      Int_t nHits = pNtuHit->GetHitN(iLayer);
      if (nHits == 0) continue;

      for (Int_t iHit = 0; iHit < nHits; ++iHit) {

         TATWntuHit *hit = pNtuHit->GetHit(iHit, iLayer);
        
         if (!hit->IsValid()) continue;
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
      if (!point->IsValid()) continue;

      TVector3 posG = point->GetPosition();
      posG = fpFootGeo->FromTWLocalToGlobal(posG);

      Float_t edep = point->GetEnergyLoss();

      fTwClusDisplay->AddHit(edep/1e5, posG[0], posG[1], posG[2]);
      fTwClusDisplay->QuadId(point);
   } //end loop on points


   fTwClusDisplay->RefitPlex();
}


//__________________________________________________________
void TAFObaseEventDisplay::UpdateCrystalElements()
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
         Int_t idx  = it->first;
         parGeo->SetCrystalColorOff(idx);
      }
   }

   fFiredCaCrystal.clear();

   TACAntuRaw* pNtuHit = fReco->GetNtuHitCa();

   Int_t nHits = pNtuHit->GetHitsN();
   if (nHits == 0) return;

   for (Int_t iHit = 0; iHit < nHits; ++iHit) {

      TACAntuHit *hit = pNtuHit->GetHit(iHit);
      if (!hit->IsValid()) continue;
      Int_t idx = hit->GetCrystalId();

      fFiredCaCrystal[idx] = 1;
      parGeo->SetCrystalColorOn(idx);

   } //end loop on hits

   // clusters
   TACAntuCluster* pNtuClus  = (TACAntuCluster*)fReco->GetNtuClusterCa();
   
   Int_t nclus = pNtuClus->GetClustersN();
   for (Int_t iClus = 0; iClus < nclus; ++iClus) {
      TACAcluster *clus = pNtuClus->GetCluster(iClus);
      if (!clus->IsValid()) continue;
      TVector3 pos = clus->GetPositionG();

      TVector3 posG = fpFootGeo->FromCALocalToGlobal(pos);
      Int_t nhits = clus->GetHitsN();
      
      x = posG(0);
      y = posG(1);
      z = posG(2);
      
      fCaClusDisplay->AddHit(nhits*10, x, y, z);
      fCaClusDisplay->QuadId(clus);
      
   } //end loop on clusters
      
   fCaClusDisplay->RefitPlex();
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateStcElements()
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

      TASTntuHit* hit = pSTntu->GetHit(i);
      Float_t charge = hit->GetCharge();

      TVector3 posHit(0,0,0); // center

      TVector3 posHitG = fpFootGeo->FromSTLocalToGlobal(posHit);

      fStClusDisplay->AddHit(charge, posHitG[0], posHitG[1], posHitG[2]);
      fStClusDisplay->QuadId(hit);
   }

   fStClusDisplay->RefitPlex();
}

//__________________________________________________________
void TAFObaseEventDisplay::UpdateLayerElements()
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
      TABMntuHit* hit = pBMntu->GetHit(i);

      if (!hit->GetIsSelected() && fgBmSelectHit) continue;

      Int_t view = hit->GetView();
      Int_t lay  = hit->GetPlane();
      Int_t cell = hit->GetCell();

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

       fBmDriftCircleDisplay->AddCone(tPosCWG, tDir, hit->GetRdrift());
       fBmDriftCircleDisplay->DigitId(hit);
   }

   fBmClusDisplay->RefitPlex();
}
