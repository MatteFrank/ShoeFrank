
/*!
 \file TAGbaseEventDisplay.cxx
 \brief FOOT base class to work on event display
 */
/*------------------------------------------+---------------------------------*/

#include "TAGbaseEventDisplay.hxx"

// root include
#include "Riostream.h"
#include "TGComboBox.h"
#include "TRootEmbeddedCanvas.h"
#include "TEveManager.h"
#include "TEveWindow.h"
#include "TEveBrowser.h"
#include "TEveBrowser.h"

#include "TAGrecoManager.hxx"

#include "TASTntuRaw.hxx"
#include "TASTntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAMSDntuHit.hxx"
#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuHit.hxx"

#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDntuTrack.hxx"
#include "TAMSDtrack.hxx"

#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "RecoRaw.hxx"
#include "RecoMC.hxx"

/*!
 \Class TAGbaseEventDisplay
 \brief FOOT base class to work on event display
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
ClassImp(TAGbaseEventDisplay)

TString TAGbaseEventDisplay::fgMsdTrackingAlgo = "Full";
Bool_t  TAGbaseEventDisplay::fgStdAloneFlag    = false;
Bool_t  TAGbaseEventDisplay::fgBmSelectHit     = false;
Bool_t  TAGbaseEventDisplay::fgM28ClusMtFlag   = false;

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] type toggle for raw or MC data
TAGbaseEventDisplay::TAGbaseEventDisplay(const TString expName, Int_t runNumber, Int_t type)
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
   fMsdPointDisplay(0x0),
   fMsdTrackDisplay(0x0),
   fTwClusDisplay(0x0),
   fCaClusDisplay(0x0),
   fGlbTrackDisplay(0x0),
   fIrTrackDisplay(0x0),
   fIrFlag(false)
{
   // Par instance
   TAGrecoManager::Instance(expName);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   fFlagTrack = TAGrecoManager::GetPar()->IsTracking();

   // default constructon
   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeBM()) {
      fStClusDisplay = new TAEDcluster("Start counter Hit");
      fStClusDisplay->SetMaxEnergy(fMaxEnergy);
      fStClusDisplay->SetDefWidth(fQuadDefWidth/2.);
      fStClusDisplay->SetDefHeight(fQuadDefHeight/2.);
      fStClusDisplay->SetPickable(true);
   }

   if (TAGrecoManager::GetPar()->IncludeBM()) {
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

   if (TAGrecoManager::GetPar()->IncludeVT()) {
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

   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fItClusDisplay = new TAEDcluster("Inner Tracker Cluster");
      fItClusDisplay->SetMaxEnergy(fMaxEnergy);
      fItClusDisplay->SetDefWidth(fQuadDefWidth/2.);
      fItClusDisplay->SetDefHeight(fQuadDefHeight/2.);
      fItClusDisplay->SetPickable(true);

      if (fFlagItrTrack) {
         fItTrackDisplay = new TAEDtrack("Inner Tracker Track");
         fItTrackDisplay->SetMaxEnergy(fMaxEnergy/2.);
         fItTrackDisplay->SetDefWidth(fBoxDefWidth);
         fItTrackDisplay->SetDefHeight(fBoxDefHeight);
         fItTrackDisplay->SetPickable(true);
      }
   }

   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fMsdClusDisplay = new TAEDcluster("Micro Strip Cluster");
      fMsdClusDisplay->SetMaxEnergy(fMaxEnergy);
      fMsdClusDisplay->SetDefWidth(fQuadDefWidth);
      fMsdClusDisplay->SetDefHeight(fQuadDefHeight);
      fMsdClusDisplay->SetPickable(true);
      
      
      fMsdPointDisplay = new TAEDcluster("Micro Strip Point");
      fMsdPointDisplay->SetMaxEnergy(fMaxEnergy);
      fMsdPointDisplay->SetDefWidth(fQuadDefWidth*4);
      fMsdPointDisplay->SetDefHeight(fQuadDefHeight*4);
      fMsdPointDisplay->SetPickable(true);
      
      if (fFlagMsdTrack) {
         fMsdTrackDisplay = new TAEDtrack("Micro Strip Track");
         fMsdTrackDisplay->SetMaxEnergy(fMaxEnergy/2.);
         fMsdTrackDisplay->SetDefWidth(fBoxDefWidth);
         fMsdTrackDisplay->SetDefHeight(fBoxDefHeight);
         fMsdTrackDisplay->SetPickable(true);
      }
   }

   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fTwClusDisplay = new TAEDcluster("Tof Wall Hit");
      fTwClusDisplay->SetMaxEnergy(fMaxEnergy);
      fTwClusDisplay->SetDefWidth(fQuadDefWidth*8);
      fTwClusDisplay->SetDefHeight(fQuadDefHeight*8);
      fTwClusDisplay->SetPickable(true);
   }

   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fCaClusDisplay = new TAEDcluster("Calorimeter Cluster");
      fCaClusDisplay->SetMaxEnergy(fMaxEnergy);
      fCaClusDisplay->SetDefWidth(fQuadDefWidth*10);
      fCaClusDisplay->SetDefHeight(fQuadDefHeight*10);
      fCaClusDisplay->SetPickable(true);
   }

   if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) {
      fGlbTrackDisplay = new TAEDglbTrackList("Global Tracks");
   }

   if (TAGrecoManager::GetPar()->IncludeStraight() && !TAGrecoManager::GetPar()->IncludeDI()) {

      fIrTrackDisplay = new TAEDtrack("Global Straight Tracks");
      fIrTrackDisplay->SetMaxEnergy(fMaxEnergy/2.);
      fIrTrackDisplay->SetDefWidth(fBoxDefWidth);
      fIrTrackDisplay->SetDefHeight(fBoxDefHeight);
      fIrTrackDisplay->SetPickable(true);
      fIrFlag = true;
   }
}

//__________________________________________________________
//! Destructor
TAGbaseEventDisplay::~TAGbaseEventDisplay()
{
   // default destructor
   if (fStClusDisplay)        delete fStClusDisplay;
   if (fBmClusDisplay)        delete fBmClusDisplay;
   if (fBmDriftCircleDisplay) delete fBmDriftCircleDisplay;
   if (fBmTrackDisplay)       delete fBmTrackDisplay;
   if (fVtxClusDisplay)       delete fVtxClusDisplay;
   if (fVtxTrackDisplay)      delete fVtxTrackDisplay;
   if (fItClusDisplay)        delete fItClusDisplay;
   if (fItTrackDisplay)       delete fItTrackDisplay;
   if (fMsdClusDisplay)       delete fMsdClusDisplay;
   if (fMsdPointDisplay)      delete fMsdPointDisplay;
   if (fMsdTrackDisplay)      delete fMsdTrackDisplay;
   if (fTwClusDisplay)        delete fTwClusDisplay;
   if (fCaClusDisplay)        delete fCaClusDisplay;
   if (fGlbTrackDisplay)      delete fGlbTrackDisplay;

   delete fReco;
}

//__________________________________________________________
//! Read parameters files
void TAGbaseEventDisplay::ReadParFiles()
{
   fReco->ReadParFiles();
}

//__________________________________________________________
//! Read parameters files
void TAGbaseEventDisplay::AdditionalTracking()
{
   
   fFlagMsdTrack = fReco->IsMsdTracking();
   fFlagItrTrack = fReco->IsItrTracking();

   if (TAGrecoManager::GetPar()->IncludeIT()) {
      if (fFlagItrTrack) {
         fItTrackDisplay = new TAEDtrack("Inner Tracker Track");
         fItTrackDisplay->SetMaxEnergy(fMaxEnergy/2.);
         fItTrackDisplay->SetDefWidth(fBoxDefWidth);
         fItTrackDisplay->SetDefHeight(fBoxDefHeight);
         fItTrackDisplay->SetPickable(true);
      }
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      if (fFlagMsdTrack) {
         fMsdTrackDisplay = new TAEDtrack("Micro Strip Track");
         fMsdTrackDisplay->SetMaxEnergy(fMaxEnergy/2.);
         fMsdTrackDisplay->SetDefWidth(fBoxDefWidth);
         fMsdTrackDisplay->SetDefHeight(fBoxDefHeight);
         fMsdTrackDisplay->SetPickable(true);
      }
   }
}

//__________________________________________________________
//! Set reconstruction alorithm options for trackers
void TAGbaseEventDisplay::SetRecoOptions()
{
   fReco->DisableTree();
   fReco->DisableSaveHits();
   fReco->EnableHisto();
   
   if (fFlagTrack) 
      fReco->EnableTracking();
   
   fReco->GlobalSettings();
   
   fpFootGeo = fReco->GetGeoTrafo();
   
   gTAGroot->SetRunNumber(fRunNumber);
}

//__________________________________________________________
//! Build default geometry
void TAGbaseEventDisplay::BuildDefaultGeometry()
{
   TAEDbaseInterface::BuildDefaultGeometry();

   // ST
   if (TAGrecoManager::GetPar()->IncludeST()) {
      TASTparGeo* parGeo = fReco->GetParGeoSt();
      TGeoVolume* irVol  = parGeo->BuildStartCounter();
      fVolumeNames[irVol->GetName()] = kSTC;


      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(FootBaseName("TASTparGeo"));
      AddGeometry(irVol, transfo);
   }

   // BM
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      TABMparGeo* parGeo = fReco->GetParGeoBm();;
      TGeoVolume* bmVol  = parGeo->BuildBeamMonitor();
      fVolumeNames[bmVol->GetName()] = kBMN;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(FootBaseName("TABMparGeo"));
      AddGeometry(bmVol, transfo);
   }

   // target
   if (TAGrecoManager::GetPar()->IncludeTG()) {
      TAGparGeo* parGeo = fReco->GetParGeoG();;
      TGeoVolume* tgVol = parGeo->BuildTarget();
      fVolumeNames[tgVol->GetName()] = kTGT;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TAGparGeo::GetBaseName());
      AddGeometry(tgVol, transfo);
   }

   // Vertex
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      TAVTparGeo* parGeo = fReco->GetParGeoVtx();
      TGeoVolume* vtVol  = parGeo->BuildVertex();
      fVolumeNames[vtVol->GetName()] = kVTX;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(FootBaseName("TAVTparGeo"));
      AddGeometry(vtVol, transfo);
   }

   // Magnet
   if (TAGrecoManager::GetPar()->IncludeDI() || TAGrecoManager::GetPar()->IncludeTOE()) {
      TADIparGeo* parGeo = fReco->GetParGeoDi();
      TGeoVolume* mgVol = parGeo->BuildMagnet();
      fVolumeNames[mgVol->GetName()] = kDIP;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(TADIparGeo::GetBaseName());
      AddGeometry(mgVol, transfo);
   }

   // IT
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      TAITparGeo* parGeo = fReco->GetParGeoIt();
      TGeoVolume* itVol  = parGeo->BuildInnerTracker();
      fVolumeNames[itVol->GetName()] = kITR;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(FootBaseName("TAITparGeo"));
      AddGeometry(itVol, transfo);
   }

   // MSD
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      TAMSDparGeo* parGeo = fReco->GetParGeoMsd();
      TGeoVolume* msdVol = parGeo->BuildMicroStripDetector();
      fVolumeNames[msdVol->GetName()] = kMSD;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(FootBaseName("TAMSDparGeo"));
      AddGeometry(msdVol, transfo);
   }

   // TW
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      TATWparGeo* parGeo = fReco->GetParGeoTw();;
      TGeoVolume* twVol = parGeo->BuildTofWall();
      fVolumeNames[twVol->GetName()] = kTOF;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(FootBaseName("TATWparGeo"));
      AddGeometry(twVol, transfo);
   }

   // CA
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      TACAparGeo* parGeo = fReco->GetParGeoCa();
      TGeoVolume* caVol = parGeo->BuildCalorimeter();
      fVolumeNames[caVol->GetName()] = kCAL;

      TGeoCombiTrans* transfo = fpFootGeo->GetCombiTrafo(FootBaseName("TACAparGeo"));
      AddGeometry(caVol, transfo);
   }
}

//__________________________________________________________
//! Create reconstruction actions
void TAGbaseEventDisplay::CreateRecAction()
{
   fReco->CreateRecAction();
}

//__________________________________________________________
//! Create raw data/MC actions
void TAGbaseEventDisplay::CreateRawAction()
{
   fReco->CreateRawAction();
}

//__________________________________________________________
//! Set input file name
//!
//! \param[in] fileName input file name
void TAGbaseEventDisplay::SetFileName(const TString fileName)
{
   fReco->SetName(fileName);
   
   if (fRunNumber == -1) {
      fReco->SetRunNumberFromFile();
      fRunNumber = fReco->GetRunNumber();
   }
   
   gTAGroot->SetRunNumber(fRunNumber);
  // fReco->SetRunNumber(fRunNumber);
}

//__________________________________________________________
//! Open file
void TAGbaseEventDisplay::OpenFile()
{
   fReco->OpenFileIn();
}

//__________________________________________________________
//! Add required actions in list
void TAGbaseEventDisplay::AddRequiredItem()
{
   fReco->AddRequiredItem();

   gTAGroot->BeginEventLoop();
   gTAGroot->Print();
}


//__________________________________________________________
//! Add TEve elements
void TAGbaseEventDisplay::AddElements()
{
   if (TAGrecoManager::GetPar()->IncludeST()){
      fStClusDisplay->ResetHits();
      gEve->AddElement(fStClusDisplay);
   }

   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fBmClusDisplay->ResetWires();
      gEve->AddElement(fBmClusDisplay);

      fBmTrackDisplay->ResetTracks();
      gEve->AddElement(fBmTrackDisplay);

       fBmDriftCircleDisplay->Reset(TEveBoxSet::kBT_Cone, kFALSE, 32);
       gEve->AddElement(fBmDriftCircleDisplay);
   }

   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fVtxClusDisplay->ResetHits();
      gEve->AddElement(fVtxClusDisplay);

      fVtxTrackDisplay->ResetTracks();
      gEve->AddElement(fVtxTrackDisplay);
   }

   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fItClusDisplay->ResetHits();
      gEve->AddElement(fItClusDisplay);

      if (fFlagItrTrack) {
         fItTrackDisplay->ResetTracks();
         gEve->AddElement(fItTrackDisplay);
      }
   }

   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fMsdClusDisplay->ResetHits();
      gEve->AddElement(fMsdClusDisplay);
      
      fMsdPointDisplay->ResetHits();
      gEve->AddElement(fMsdPointDisplay);
      
      if (fFlagMsdTrack) {
         fMsdTrackDisplay->ResetTracks();
         gEve->AddElement(fMsdTrackDisplay);
      }
   }

   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fTwClusDisplay->ResetHits();
      gEve->AddElement(fTwClusDisplay);
   }

   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fCaClusDisplay->ResetHits();
      gEve->AddElement(fCaClusDisplay);
   }

   if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) {
      fGlbTrackDisplay->ResetTracks();
      gEve->AddElement(fGlbTrackDisplay);
   }

   if (TAGrecoManager::GetPar()->IncludeStraight() && !TAGrecoManager::GetPar()->IncludeDI()) {
      fIrTrackDisplay->ResetTracks();
      gEve->AddElement(fIrTrackDisplay);
   }
}

//__________________________________________________________
//! Connect to pad TEve elements
void TAGbaseEventDisplay::ConnectElements()
{
   if (TAGrecoManager::GetPar()->IncludeST()){
      fStClusDisplay->SetEmitSignals(true);
      fStClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }

   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fBmTrackDisplay->SetEmitSignals(true);
      fBmTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
      fBmDriftCircleDisplay->SetEmitSignals(true);
      fBmDriftCircleDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateDriftCircleInfo(TEveDigitSet*, Int_t)");
   }

   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fVtxClusDisplay->SetEmitSignals(true);
      fVtxClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");

      fVtxTrackDisplay->SetEmitSignals(true);
      fVtxTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
   }

   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fItClusDisplay->SetEmitSignals(true);
      fItClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");

      if (fFlagItrTrack) {
         fItTrackDisplay->SetEmitSignals(true);
         fItTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
      }
   }

   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fMsdClusDisplay->SetEmitSignals(true);
      fMsdClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
      
      fMsdPointDisplay->SetEmitSignals(true);
      fMsdPointDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
      
      if (fFlagMsdTrack) {
         fMsdTrackDisplay->SetEmitSignals(true);
         fMsdTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
      }
   }

   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fTwClusDisplay->SetEmitSignals(true);
      fTwClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }

   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fCaClusDisplay->SetEmitSignals(true);
      fCaClusDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateHitInfo(TEveDigitSet*, Int_t)");
   }
   
   if (TAGrecoManager::GetPar()->IncludeStraight() && !TAGrecoManager::GetPar()->IncludeDI()) {
      fIrTrackDisplay->SetEmitSignals(true);
      fIrTrackDisplay->Connect("SecSelected(TEveDigitSet*, Int_t )", "TAGbaseEventDisplay", this, "UpdateTrackInfo(TEveDigitSet*, Int_t)");
   }

   if (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) {
      TQObject::Connect("TAEDglbTrack", "LineSecSelected(TEveStraightLineSet*, Int_t)", "TAGbaseEventDisplay", this, "UpdateTrackInfo(TEveStraightLineSet*, Int_t)");
   }
}

//__________________________________________________________
//! Update digit information output
//!
//! \param[in] qs digit set
//! \param[in] idx  associated object index
void TAGbaseEventDisplay::UpdateHitInfo(TEveDigitSet* qs, Int_t idx)
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
      
   } else if (obj->InheritsFrom("TAMSDpoint")) {
      TAMSDpoint* point = (TAMSDpoint*)obj;
      if (point == 0x0) return;
      TVector3 pos = point->GetPositionG();
      fInfoView->AddLine( Form("Point # %3d\n", idx) );
      fInfoView->AddLine( Form("with de: %.1g in sensor %d\n", point->GetEnergyLoss(), point->GetSensorIdx()) );
      fInfoView->AddLine( Form("at position: (%.3g %.3g) cm\n", pos.X(), pos.Y()) );
      
      if (fConsoleButton->IsOn()) {
         cout << Form("Cluster # %3d\n", idx);
         cout << Form("with de: %.1g in sensor %d\n", point->GetEnergyLoss(), point->GetSensorIdx());
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

   } else if (obj->InheritsFrom("TASThit")) {
      TASThit* hit = (TASThit*)obj;
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
      fInfoView->AddLine( Form("Time: %.3g ps \n", point->GetToF()) );
      fInfoView->AddLine( Form("Charge Z: %d \n", point->GetChargeZ()) );

      if (fConsoleButton->IsOn()) {
         cout <<  Form("Point# %d at position:\n", idx);
         cout <<  Form(" (%.1f %.1f %.1f) cm\n", pos.X(), pos.Y(), pos.Z());
         cout <<  Form("Charge: %.3e u.a.\n", point->GetEnergyLoss());
         cout <<  Form("Time: %.3g ps \n", point->GetToF());
         cout <<  Form("Charge Z: %d \n", point->GetChargeZ());
      }
      
   } else if (obj->InheritsFrom("TACAcluster")) {
      TACAcluster* clus = (TACAcluster*)obj;
      if (clus == 0x0) return;
      TVector3 pos = clus->GetPositionG();
      fInfoView->AddLine( Form("Cluster # %3d\n", idx) );
      fInfoView->AddLine( Form("with %3d hits\n", clus->GetHitsN()) );
      fInfoView->AddLine( Form("Energy: %.3g (GeV) \n", clus->GetEnergy()*TAGgeoTrafo::MevToGev()) );
      fInfoView->AddLine( Form("at position: (%.3g %.3g) cm\n", pos.X(), pos.Y()) );
      
      if (fConsoleButton->IsOn()) {
         cout << Form("Cluster # %3d\n", idx);
         cout << Form("with %3d hits\n", clus->GetHitsN());
         cout << Form("Energy: %.3g (MeV) \n", clus->GetEnergy()*TAGgeoTrafo::MevToGev());
         cout << Form("at position: (%.3g %.3g) cm\n", pos.X(), pos.Y());
      }

   } else {
      return;
   }
}

//__________________________________________________________
//! Update track information output
//!
//! \param[in] qs track set
//! \param[in] idx digit associated object index
void TAGbaseEventDisplay::UpdateTrackInfo(TEveDigitSet* qs, Int_t idx)
{
   TAEDtrack* lineTracks = dynamic_cast<TAEDtrack*> (qs);
   TObject* obj = lineTracks->GetId(idx);
   if (obj == 0x0) return;

   if (obj->InheritsFrom("TAGbaseTrack")) {

      TAGbaseTrack* track =  (TAGbaseTrack*)obj;
      if (track == 0x0) return;

      fInfoView->AddLine( Form("Track # %2d (valid: %d): \n", track->GetTrackIdx(), track->GetValidity()) );
      fInfoView->AddLine( Form(" with %3d clusters\n", track->GetClustersN()) );
      if (fConsoleButton->IsOn()) {
         cout << Form("Track # %2d (valid: %d): \n", track->GetTrackIdx(), track->GetValidity());
         cout << Form(" with %3d clusters\n", track->GetClustersN());
      }

      for (Int_t i = 0; i < track->GetClustersN(); i++) {
         TAGcluster* clus = track->GetCluster(i);
         TVector3 posG = clus->GetPositionG();
         fInfoView->AddLine( Form(" for plane %d\n", clus->GetSensorIdx()));
         fInfoView->AddLine( Form(" at position: (%.3g %.3g) \n", posG.X(), posG.Y()) );
         fInfoView->AddLine( Form(" with %d pixels\n", clus->GetElementsN()));

         if (fConsoleButton->IsOn()) {
            cout <<  Form(" for plane %d\n", clus->GetSensorIdx());
            cout <<  Form(" at position: (%.3g %.3g) \n", posG.X(), posG.Y());
            cout <<  Form(" with %d pixels\n", clus->GetElementsN());
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
//! Update track line information output
//!
//! \param[in] ts track line set
//! \param[in] idx associated object index
void TAGbaseEventDisplay::UpdateTrackInfo(TEveStraightLineSet* ts, Int_t idx)
{
   TAEDglbTrack* lineTracks = dynamic_cast<TAEDglbTrack*> (ts);
   TObject* obj = lineTracks->GetTrackId();

   if (obj->InheritsFrom("TAGtrack")) {

      TAGtrack* track =  (TAGtrack*)obj;
      if (track == 0x0) return;

      fInfoView->AddLine( Form("Track # %2d with %2d points\n", track->GetTrackId(), track->GetPointsN()) );
      fInfoView->AddLine( Form("Charge: %d A: %d Mass: %.2f GeV/c2\n", track->GetTwChargeZ(), TMath::Nint(track->GetMass()/TAGgeoTrafo::GetMassFactor()), track->GetMass()) );
      fInfoView->AddLine( Form("Momentum: %.2f GeV/c ToF: %.2f ns\n", track->GetMomentum(), track->GetTwTof()) );

      if (fConsoleButton->IsOn()) {
         cout <<  Form("Track # %2d with %2d points\n", track->GetTrackId(), track->GetPointsN());
         cout <<  Form("Charge: %d A: %d Mass: %.2f GeV/c2\n", track->GetTwChargeZ(), TMath::Nint(track->GetMass()/TAGgeoTrafo::GetMassFactor()), track->GetMass());
         cout <<  Form("Momentum: %.2f GeV/c ToF: %.2f ns\n", track->GetMomentum(), track->GetTwTof());

         for( Int_t iPoint = 0; iPoint < track->GetPointsN(); ++iPoint ) {
            TAGpoint* point = track->GetPoint(iPoint);
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
//! Update BM drift circle information output
//!
//! \param[in] qs digit set
//! \param[in] idx associated object index
void TAGbaseEventDisplay::UpdateDriftCircleInfo(TEveDigitSet* qs, Int_t idx)
{
    if (!fDetectorStatus[kBMN]) return;
    TEveBoxSet* tpCircle = dynamic_cast<TEveBoxSet*>(qs);

    TABMhit* hit = dynamic_cast<TABMhit*>( tpCircle->GetId(idx) );
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
//! Update TEve elements in view
void TAGbaseEventDisplay::UpdateElements()
{
   if (fgGUIFlag)
      fEventEntry->SetText(Form("Run %d Event %d", gTAGroot->CurrentRunNumber(), gTAGroot->CurrentEventId().EventNumber()));

   if (TAGrecoManager::GetPar()->IncludeST())
      UpdateElements("st");

   if (TAGrecoManager::GetPar()->IncludeBM())
      UpdateElements("bm");

   if (TAGrecoManager::GetPar()->IncludeVT())
      UpdateElements("vt");

   if (TAGrecoManager::GetPar()->IncludeIT())
      UpdateElements("it");

   if (TAGrecoManager::GetPar()->IncludeMSD())
      UpdateElements("ms");

   if (TAGrecoManager::GetPar()->IncludeTW())
      UpdateElements("tw");

   if (TAGrecoManager::GetPar()->IncludeCA())
      UpdateElements("ca");

   if (TAGrecoManager::GetPar()->IncludeStraight() && !TAGrecoManager::GetPar()->IncludeDI())
      UpdateElements("ir");

   if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && fFlagTrack)
      UpdateGlbTrackElements();

}

//__________________________________________________________
//! Update TEve elements in view for a given device
//!
//! \param[in] prefix device prefix
void TAGbaseEventDisplay::UpdateElements(const TString prefix)
{
   if (prefix == "tw")
      UpdateBarElements();
   else if (prefix == "ca")
      UpdateCrystalElements();
   else if (prefix == "st")
      UpdateStcElements();
   else if (prefix == "bm") {
      UpdateLayerElements();
      if (fFlagTrack)
         UpdateTrackElements(prefix);
   } else if (prefix == "ms") {
      UpdateStripElements();
      if (fFlagTrack)
         UpdateTrackElements(prefix);
   } else {
      UpdateQuadElements(prefix);
      if (fFlagTrack)
         UpdateTrackElements(prefix);
   }
}

//__________________________________________________________
//! Update quad elements in view for a given device
//!
//! \param[in] prefix device prefix
void TAGbaseEventDisplay::UpdateQuadElements(const TString prefix)
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

   if (prefix == "vt" && !fDetectorStatus[kVTX]) return;
   if (prefix == "it" && !fDetectorStatus[kITR]) return;
   
   Float_t  x = 0.,  y = 0.,  z = 0.;
   Float_t x1 = 0.,  y1 =0., z1 = 0.;

   TAVTbaseParGeo* parGeo = 0x0;

   if (prefix == "vt")
      parGeo = fReco->GetParGeoVtx();
   else if (prefix == "it")
      parGeo = fReco->GetParGeoIt();
   else
      return;

   // known bug if first event is empty
   if (fVtxClusDisplay)
      fVtxClusDisplay->AddHit(-1, 0, 0, 0);

   if (fItClusDisplay)
      fItClusDisplay->AddHit(-1, 0, 0, 0);
   
   Int_t nPlanes = parGeo->GetSensorsN();

   TAVTntuTrack*  pNtuTrack = 0x0;

   if (prefix == "vt") {
      if (fFlagTrack && TAGrecoManager::GetPar()->IncludeTG()) {
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
         Int_t nPix = clus->GetListOfPixels()->GetEntriesFast();
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
//! Update strip elements in view for a given device
//!
//! \param[in] prefix device prefix
void TAGbaseEventDisplay::UpdateStripElements()
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      fMsdClusDisplay->ResetHits();
      fMsdPointDisplay->ResetHits();
   }
   
   if (!fgDisplayFlag) // do not update event display
      return;

   if (!fDetectorStatus[kMSD]) return;

   Float_t  x = 0.,  y = 0.,  z = 0.;

   TAMSDparGeo* parGeo = fReco->GetParGeoMsd();
   Float_t pitch = parGeo->GetPitch()*5;
   TVector3 epi  = parGeo->GetEpiSize();
   Int_t nPlanes = parGeo->GetSensorsN();

   // known bug if first event is empty
   if (fMsdClusDisplay)
      fMsdClusDisplay->AddHit(-1, 0, 0, 0);
   
   // strips
   TAMSDntuCluster* pNtuClus  = (TAMSDntuCluster*)fReco->GetNtuClusterMsd();

   for( Int_t iPlane = 0; iPlane < nPlanes; iPlane++) {

      Int_t nclus = pNtuClus->GetClustersN(iPlane);

      if (nclus == 0) continue;

      for (Int_t iClus = 0; iClus < nclus; ++iClus) {
         TAMSDcluster *clus = pNtuClus->GetCluster(iPlane, iClus);
         if (!clus->IsValid()) continue;
         TVector3 pos = clus->GetPositionG();
         TVector3 posG = fpFootGeo->FromMSDLocalToGlobal(pos);
         Int_t nStrip = clus->GetListOfStrips()->GetEntriesFast();

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
   
   // point
   TAMSDntuPoint* pNtuPoint  = (TAMSDntuPoint*)fReco->GetNtuPointMsd();
   
   for( Int_t iStation = 0; iStation < nPlanes/2; iStation++) {
      
      Int_t npoint = pNtuPoint->GetPointsN(iStation);
      
      if (npoint == 0) continue;
      
      for (Int_t iPoint = 0; iPoint < npoint; ++iPoint) {
         TAMSDpoint *point = pNtuPoint->GetPoint(iStation, iPoint);
        // if (!point->IsValid()) continue;
         TVector3 pos = point->GetPositionG();
         TVector3 posG = fpFootGeo->FromMSDLocalToGlobal(pos);
         
         x = posG(0);
         y = posG(1);
         z = posG(2);
         
         fMsdPointDisplay->AddHit(10, x, y, z);
         fMsdPointDisplay->QuadId(point);
         
      } //end loop on hits
      
   } //end loop on planes
   
   fMsdPointDisplay->RefitPlex();
}

//__________________________________________________________
//! Update track elements in view for a given device
//!
//! \param[in] prefix device prefix
void TAGbaseEventDisplay::UpdateTrackElements(const TString prefix)
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      if (prefix == "bm")
         fBmTrackDisplay->ResetTracks();

      if (prefix == "vt")
         fVtxTrackDisplay->ResetTracks();

      if (prefix == "ir")
         fIrTrackDisplay->ResetTracks();

      if (prefix == "it" && fFlagItrTrack)
         fItTrackDisplay->ResetTracks();
      
      if (prefix == "ms" && fFlagMsdTrack)
         fMsdTrackDisplay->ResetTracks();
   }

   if (prefix == "bm" && !fDetectorStatus[kBMN]) return;
   if (prefix == "vt" && !fDetectorStatus[kVTX]) return;
   if (prefix == "it" && !fDetectorStatus[kITR]) return;
   if (prefix == "ms" && !fDetectorStatus[kMSD]) return;

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

         if (TAGrecoManager::GetPar()->IncludeTG())
            pos = track->Intersection(track->GetPosVertex().Z());
         else
            pos = track->Intersection(posfirstPlane);

         posG = fpFootGeo->FromVTLocalToGlobal(pos);

         x = posG(0); y = posG(1); z = posG(2);

         if (TAGrecoManager::GetPar()->IncludeTW() && !TAGrecoManager::GetPar()->IncludeDI()) {
            Float_t posZtw = fpFootGeo->FromTWLocalToGlobal(TVector3(0,0,0)).Z();
            posZtw = fpFootGeo->FromGlobalToVTLocal(TVector3(0, 0, posZtw)).Z();
            pos = track->Intersection(posZtw);
         } else {
            pos  = track->Intersection(posLastPlane);
         }

         posG = fpFootGeo->FromVTLocalToGlobal(pos);

         x1 = posG(0); y1 = posG(1); z1 = posG(2);

         Float_t nPix = track->GetMeanEltsN();
         fVtxTrackDisplay->AddTracklet(nPix*10, x, y, z, x1, y1, z1);
         fVtxTrackDisplay->TrackId(track);

      } // end loop on tracks

      fVtxTrackDisplay->RefitPlex();
   }

   if (prefix == "it" && !fIrFlag && fFlagItrTrack) {

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

         if (TAGrecoManager::GetPar()->IncludeTG() ) {
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

         Float_t nPix = track->GetMeanEltsN();
         fItTrackDisplay->AddTracklet(nPix*10, x, y, z, x1, y1, z1);
         fItTrackDisplay->TrackId(track);

      } // end loop on tracks

      fItTrackDisplay->RefitPlex();
   }

   if (prefix == "ms" && !fIrFlag && fFlagMsdTrack) {
      TAMSDparGeo*  parGeo  = fReco->GetParGeoMsd();
      Int_t nPlanes         = parGeo->GetSensorsN();
      Float_t posfirstPlane = parGeo->GetSensorPosition(0)[2]*1.1;
      Float_t posLastPlane  = parGeo->GetSensorPosition(nPlanes-1)[2]*1.1;

      TAMSDntuTrack* pNtuTrack = fReco->GetNtuTrackMsd();

      for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
         fMsdTrackDisplay->AddNewTrack();

         TAMSDtrack* track = pNtuTrack->GetTrack(iTrack);
         TVector3 pos;
         TVector3 posG;

         if (TAGrecoManager::GetPar()->IncludeTG() ) {
            Float_t posZtg = fpFootGeo->FromTGLocalToGlobal(TVector3(0,0,0)).Z();
            posZtg = fpFootGeo->FromGlobalToMSDLocal(TVector3(0, 0, posZtg)).Z();
            pos = track->Intersection(posZtg);
         } else
            pos  = track->Intersection(posfirstPlane);

         posG = fpFootGeo->FromMSDLocalToGlobal(pos);
         x = posG(0); y = posG(1); z = posG(2);

         pos  = track->Intersection(posLastPlane);
         posG = fpFootGeo->FromMSDLocalToGlobal(pos);
         x1 = posG(0); y1 = posG(1); z1 = posG(2);

         Float_t nPix = track->GetMeanEltsN();
         fMsdTrackDisplay->AddTracklet(nPix*500, x, y, z, x1, y1, z1);
         fMsdTrackDisplay->TrackId(track);
      
     } // end loop on tracks

      fMsdTrackDisplay->RefitPlex();
   }

   
   if (prefix == "bm") {
      TABMparGeo*   parGeo    = fReco->GetParGeoBm();
      TABMntuTrack* pNtuTrack = fReco->GetNtuTrackBm();

      for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
         fBmTrackDisplay->AddNewTrack();

         TABMtrack* track = pNtuTrack->GetTrack(iTrack);

         TVector3 A0 = track->Intersection(parGeo->GetMylar1().Z());
         TVector3 A1 = track->Intersection(parGeo->GetMylar2().Z());

         if (TAGrecoManager::GetPar()->IncludeTG()) {
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
      
      TAGntuGlbTrack* pNtuTrack = fReco->GetNtuGlbTrack();
      for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
         fIrTrackDisplay->AddNewTrack();

         TAGtrack* track = pNtuTrack->GetTrack(iTrack);
         TVector3 posG(0, 0, 0);

         posG  = track->Intersection(0);
         x = posG(0); y = posG(1); z = posG(2);

         Float_t posZtw = fpFootGeo->FromTWLocalToGlobal(TVector3(0,0,0)).Z();
         posG = track->Intersection(posZtw);
         x1 = posG(0); y1 = posG(1); z1 = posG(2);

         Int_t nPoint = track->GetPointsN();
         fIrTrackDisplay->AddTracklet(nPoint*100, x, y, z, x1, y1, z1);
         fIrTrackDisplay->TrackId(track);

      } // end loop on tracks

      fIrTrackDisplay->RefitPlex();
   }
}

//__________________________________________________________
//! Update global track elements in view for a given device
//!
//! \param[in] prefix device prefix
void TAGbaseEventDisplay::UpdateGlbTrackElements()
{
   TAGntuGlbTrack* pNtuTrack = fReco->GetNtuGlbTrack();
   fGlbTrackDisplay->ResetTracks();

   for( Int_t iTrack = 0; iTrack < pNtuTrack->GetTracksN(); ++iTrack ) {
      TAGtrack* track = pNtuTrack->GetTrack(iTrack);
      Int_t charge    = track->GetTwChargeZ();

      TAEDglbTrack* glbTrack = fGlbTrackDisplay->NewTrack(Form("Track%d", iTrack));
      glbTrack->TrackId(track);

      TAGpoint* point = track->GetPoint(0);
      Float_t z1      = point->GetPosition().Z();
      TVector3 pos1   = track->GetPosition(z1);

      for( Int_t iPoint = 1; iPoint < track->GetPointsN(); ++iPoint ) {
         TAGpoint* point = track->GetPoint(iPoint);
         Float_t z2      = point->GetMeasPosition().Z();
         TVector3 pos2   = track->GetPosition(z2);

         glbTrack->AddTracklet(charge, pos1, pos2);
         pos1 = pos2;
         
      } // end loop on points
   } // end loop on tracks
}

//__________________________________________________________
//! Update bar elements (TW)
void TAGbaseEventDisplay::UpdateBarElements()
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
         fTwClusDisplay->ResetHits();
   }

   if (!fgDisplayFlag) // do not update event display
      return;

   if (!fDetectorStatus[kTOF]) return;

   // Draw Quad
   TATWntuPoint* pNtuPoint =  fReco->GetNtuPointTw();;
   
   Int_t nPoints = pNtuPoint->GetPointsN();
   
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
   
   if (TAGrecoManager::GetPar()->IsFromLocalReco()) return;

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

   TATWntuHit* pNtuHit = fReco->GetNtuHitTw();

   for( Int_t iLayer = 0; iLayer < parGeo->GetLayersN(); iLayer++) {

      Int_t nHits = pNtuHit->GetHitN(iLayer);
      if (nHits == 0) continue;

      for (Int_t iHit = 0; iHit < nHits; ++iHit) {

         TATWhit *hit = pNtuHit->GetHit(iHit, iLayer);
        
         if (!hit->IsValid()) continue;
         if(!hit) continue;

         Int_t iBar = hit->GetBar();

         pair<int, int> idx(iBar, iLayer);

         fFiredTofBar[idx] = 1;
         parGeo->SetBarColorOn(iBar, iLayer);

      } //end loop on hits

   } //end loop on planes

}

//__________________________________________________________
//! Update crystal elements (CAL)
void TAGbaseEventDisplay::UpdateCrystalElements()
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
         fCaClusDisplay->ResetHits();
   }

   if (!fgDisplayFlag) // do not update event display
      return;

   if (!fDetectorStatus[kCAL]) return;

   Float_t  x = 0.,  y = 0.,  z = 0.;

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
      z = posG(2)-1; // let move it a bit foward 
      // let made the cluster size propotional to the num of hists
      fCaClusDisplay->AddHit(nhits*50, x, y, z, nhits/2+1, nhits/2+1);
      fCaClusDisplay->QuadId(clus);
      
   } //end loop on clusters
      
   fCaClusDisplay->RefitPlex();
   
   if (TAGrecoManager::GetPar()->IsFromLocalReco()) return;
   
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

   TACAntuHit* pNtuHit = fReco->GetNtuHitCa();

   Int_t nHits = pNtuHit->GetHitsN();
   if (nHits == 0) return;

   for (Int_t iHit = 0; iHit < nHits; ++iHit) {

      TACAhit *hit = pNtuHit->GetHit(iHit);
      if (!hit->IsValid()) continue;
      Int_t idx = hit->GetCrystalId();

      fFiredCaCrystal[idx] = 1;
      parGeo->SetCrystalColorOn(idx);
      
   } //end loop on hits
}

//__________________________________________________________
//! Update STC elements
void TAGbaseEventDisplay::UpdateStcElements()
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      fStClusDisplay->ResetHits();
   }

   if (!fgDisplayFlag) // do not update event display
      return;

   if (!fDetectorStatus[kSTC]) return;

   //STC
   TASTntuHit* pSTntu = fReco->GetNtuHitSt();
   Int_t       nHits  = pSTntu->GetHitsN();

   //hits
   if(FootDebugLevel(1))
      cout<<" TAFO:: nHits ST "<<nHits<<endl;

   for (Int_t i = 0; i < nHits; i++) {

      TASThit* hit = pSTntu->GetHit(i);
      Float_t charge = hit->GetCharge();

      TVector3 posHit(0,0,0); // center

      TVector3 posHitG = fpFootGeo->FromSTLocalToGlobal(posHit);

      fStClusDisplay->AddHit(charge, posHitG[0], posHitG[1], posHitG[2]);
      fStClusDisplay->QuadId(hit);
   }

   fStClusDisplay->RefitPlex();
}

//__________________________________________________________
//! Update layer elements (BM)
void TAGbaseEventDisplay::UpdateLayerElements()
{
   TABMparGeo* pbmGeo = fReco->GetParGeoBm();;


   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      fBmClusDisplay->ResetWires();
       fBmDriftCircleDisplay->Reset(TEveBoxSet::kBT_Cone, kFALSE, 32);
       for(auto l = 0; l < 2 * pbmGeo->GetLayersN() ; ++l) {  pbmGeo->SetLayerColorOff(l); }
   }

   if (!fgDisplayFlag) // do not update event display
      return;

   if (!fDetectorStatus[kBMN]) return;

   TABMntuHit* pBMntu = fReco->GetNtuHitBm();
   Int_t       nHits  = pBMntu->GetHitsN();
   Double_t bm_h_side = pbmGeo->GetWidth();

   //hits
   for (Int_t i = 0; i < nHits; i++) {
      TABMhit* hit = pBMntu->GetHit(i);

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
