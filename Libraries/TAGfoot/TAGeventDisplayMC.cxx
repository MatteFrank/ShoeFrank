
/*!
 \file TAGeventDisplayMC.cxx
 \brief FOOT class to work on MC event display
 */
/*------------------------------------------+---------------------------------*/

#include "TAGeventDisplayMC.hxx"
#include "TEveGeoNode.h"
#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveWindow.h"
#include "TGeoManager.h"

#include "TAGrecoManager.hxx"
#include "RecoMC.hxx"
#include "GlobalToeReco.hxx"

//! Class Imp
ClassImp(TAGeventDisplayMC)

/*!
 \class TAGeventDisplayMC
 \brief FOOT class to work on MC event display
 */
/*------------------------------------------+---------------------------------*/

TAGeventDisplayMC* TAGeventDisplayMC::fgInstance    = 0x0;
            Bool_t TAGeventDisplayMC::fgMcTrackFlag = false;

//__________________________________________________________
//! Instance
//!
//! \param[in] name experiment name
//! \param[in] runNumber run number
//! \param[in] type toggle for raw or MC data
TAGeventDisplayMC* TAGeventDisplayMC::Instance(const TString name, Int_t runNumber, Int_t type)
{
   if (fgInstance == 0x0)
      fgInstance = new TAGeventDisplayMC(name, runNumber, type);
   
   return fgInstance;
}

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] type toggle for raw or MC data
TAGeventDisplayMC::TAGeventDisplayMC(const TString expName, Int_t runNumber, Int_t type)
 : TAGbaseEventDisplay(expName, runNumber, type),
   fCaMcDisplay(0x0),
   fTwMcDisplay(0x0),
   fMsdMcDisplay(0x0),
   fItMcDisplay(0x0),
   fVtMcDisplay(0x0),
   fBmMcDisplay(0x0),
   fStMcDisplay(0x0),
   fTrackMcDisplay(0x0)
{
   // local reco
   SetReco();
   
   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeBM())
      fStMcDisplay = new TAEDpoint("STC MC hit");
   
   if (TAGrecoManager::GetPar()->IncludeBM())
      fBmMcDisplay = new TAEDpoint("STC MC hit");
   
   if (TAGrecoManager::GetPar()->IncludeVT())
      fVtMcDisplay = new TAEDpoint("VTX MC hit");
   
   if (TAGrecoManager::GetPar()->IncludeIT())
      fItMcDisplay = new TAEDpoint("IT MC hit");
   
   if (TAGrecoManager::GetPar()->IncludeMSD())
      fMsdMcDisplay = new TAEDpoint("MSD MC hit");
   
   if (TAGrecoManager::GetPar()->IncludeTW())
      fTwMcDisplay = new TAEDpoint("ToF MC hit");
   
   if (TAGrecoManager::GetPar()->IncludeCA())
      fCaMcDisplay = new TAEDpoint("Cal MC hit");
   
   if (fgMcTrackFlag)
      fTrackMcDisplay = new TAEDglbTrack("MC tracks");
}

//__________________________________________________________
//! default destructor
TAGeventDisplayMC::~TAGeventDisplayMC()
{
   if (fStMcDisplay)
      delete fStMcDisplay;
   if (fBmMcDisplay)
      delete fBmMcDisplay;
   if (fVtMcDisplay)
      delete fVtMcDisplay;
   if (fItMcDisplay)
      delete fItMcDisplay;
   if(fMsdMcDisplay)
      delete fMsdMcDisplay;
   if(fTwMcDisplay)
      delete fTwMcDisplay;
   if(fCaMcDisplay)
      delete fCaMcDisplay;
   if(fTrackMcDisplay)
      delete fTrackMcDisplay;
}

//__________________________________________________________
//! Set reconstruction
void TAGeventDisplayMC::SetReco()
{
   Bool_t lrc = TAGrecoManager::GetPar()->IsFromLocalReco();
   Bool_t mc  = true;

   if (fType == 1) {
     Warning("SetReco", "Old interface for Fluka Structure not supported anymore, switch to new");
     fType = 2;
   }
   
   if (fType == 2) {
      if (lrc) {
         fReco = new GlobalToeReco(fExpName, fRunNumber, "","",  mc);
         fReco->EnableReadL0Hits();
      } else
         fReco = new RecoMC(fExpName, fRunNumber);
   } else
      Error("SetReco()", "Unknown type %d", fType);
   
   SetRecoOptions();
}

//__________________________________________________________
//! Get to entry (event)
//!
//! \param[in] entry entry to go
Bool_t TAGeventDisplayMC::GetEntry(Int_t entry)
{
   if (fType != 2) return true;
   return fReco->GoEvent(entry);   
}

//__________________________________________________________
//! Add MC TEve elements
void TAGeventDisplayMC::AddMcElements()
{
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fCaMcDisplay->ResetPoints();
      gEve->AddElement(fCaMcDisplay);
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fTwMcDisplay->ResetPoints();
      gEve->AddElement(fTwMcDisplay);
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()){
      fMsdMcDisplay->ResetPoints();
      gEve->AddElement(fMsdMcDisplay);
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fItMcDisplay->ResetPoints();
      gEve->AddElement(fItMcDisplay);
   }
   
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fVtMcDisplay->ResetPoints();
      gEve->AddElement(fVtMcDisplay);
   }
   
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fBmMcDisplay->ResetPoints();
      gEve->AddElement(fBmMcDisplay);
   }
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fStMcDisplay->ResetPoints();
      gEve->AddElement(fStMcDisplay);
   }

   if (fgMcTrackFlag) {
      fTrackMcDisplay->ResetTracklets();
      gEve->AddElement(fTrackMcDisplay);
   }
}

//__________________________________________________________
//! Connect MC elements
void TAGeventDisplayMC::ConnectMcElements()
{
   if (TAGrecoManager::GetPar()->IncludeST() || TAGrecoManager::GetPar()->IncludeBM())
      fStMcDisplay->Connect("PointSelected(Int_t )", "TAGeventDisplayMC", this, "UpdateStInfo(Int_t)");
   
   if (TAGrecoManager::GetPar()->IncludeBM())
      fBmMcDisplay->Connect("PointSelected(Int_t )", "TAGeventDisplayMC", this, "UpdateBmInfo(Int_t)");
   
   if (TAGrecoManager::GetPar()->IncludeVT())
      fVtMcDisplay->Connect("PointSelected(Int_t )", "TAGeventDisplayMC", this, "UpdateVtInfo(Int_t)");
   
   if (TAGrecoManager::GetPar()->IncludeIT())
      fItMcDisplay->Connect("PointSelected(Int_t )", "TAGeventDisplayMC", this, "UpdateItInfo(Int_t)");
   
   if (TAGrecoManager::GetPar()->IncludeMSD())
      fMsdMcDisplay->Connect("PointSelected(Int_t )", "TAGeventDisplayMC", this, "UpdateMsInfo(Int_t)");
   
   if (TAGrecoManager::GetPar()->IncludeTW())
      fTwMcDisplay->Connect("PointSelected(Int_t )", "TAGeventDisplayMC", this, "UpdateTwInfo(Int_t)");
   
   if (TAGrecoManager::GetPar()->IncludeCA())
      fCaMcDisplay->Connect("PointSelected(Int_t )", "TAGeventDisplayMC", this, "UpdateCaInfo(Int_t)");
   
   if (fgMcTrackFlag)
      TQObject::Connect("TAEDglbTrack", "LineSecSelected(TEveStraightLineSet*, Int_t)", "TAGeventDisplayMC", this, "UpdateTrackInfo(TEveStraightLineSet*, Int_t)");

}

//__________________________________________________________
//! Update STC informations
//!
//! \param[in] idx associated object index
void TAGeventDisplayMC::UpdateStInfo(Int_t idx)
{
   UpdateMcInfo("st", idx);
}

//__________________________________________________________
//! Update BM informations
//!
//! \param[in] idx associated object index
void TAGeventDisplayMC::UpdateBmInfo(Int_t idx)
{
   UpdateMcInfo("bm", idx);
}

//__________________________________________________________
//! Update VTX informations
//!
//! \param[in] idx associated object index
void TAGeventDisplayMC::UpdateVtInfo(Int_t idx)
{
   UpdateMcInfo("vt", idx);
}

//__________________________________________________________
//! Update ITR informations
//!
//! \param[in] idx associated object index
void TAGeventDisplayMC::UpdateItInfo(Int_t idx)
{
   UpdateMcInfo("it", idx);
}

//__________________________________________________________
//! Update MSD informations
//!
//! \param[in] idx associated object index
void TAGeventDisplayMC::UpdateMsInfo(Int_t idx)
{
   UpdateMcInfo("ms", idx);
}

//__________________________________________________________
//! Update TW informations
//!
//! \param[in] idx associated object index
void TAGeventDisplayMC::UpdateTwInfo(Int_t idx)
{
   UpdateMcInfo("tw", idx);
}

//__________________________________________________________
//! Update CAL informations
//!
//! \param[in] idx associated object index
void TAGeventDisplayMC::UpdateCaInfo(Int_t idx)
{
   UpdateMcInfo("ca", idx);
}

//__________________________________________________________
//! Update MC informations for a given device
//!
//! \param[in] idx associated object index
void TAGeventDisplayMC::UpdateMcInfo(TString prefix, Int_t idx)
{
   TAMChit* point = 0x0;
   TString name   = "";
   
   if (prefix == "st") {
      point = (TAMChit*)fStMcDisplay->GetPointId(idx);
      name = "STC";
   }

   if (prefix == "bm") {
      point = (TAMChit*)fBmMcDisplay->GetPointId(idx);
      name = "BM";
   }
   
   if (prefix == "vt") {
      point = (TAMChit*)fVtMcDisplay->GetPointId(idx);
      name = "VTX";
   }

   if (prefix == "it") {
      point = (TAMChit*)fItMcDisplay->GetPointId(idx);
      name = "ITR";
   }
   
   if (prefix == "ms") {
      point = (TAMChit*)fMsdMcDisplay->GetPointId(idx);
      name = "MSD";
   }
   
   if (prefix == "tw") {
      point = (TAMChit*)fTwMcDisplay->GetPointId(idx);
      name = "TOF";
   }

   if (prefix == "ca") {
      point = (TAMChit*)fCaMcDisplay->GetPointId(idx);
      name = "CAL";
   }
   
   if(point == 0x0) return;

   TVector3 pos   = point->GetInPosition();
   TVector3 mom   = point->GetInMomentum();
   Int_t trackIdx = point->GetTrackIdx()-1;

   fInfoView->AddLine( Form("%s sensor id: %d, Hit:\n", name.Data(), trackIdx) );
   fInfoView->AddLine( Form("at position:   (%.3g %.3g %.3g) cm\n", pos[0], pos[1], pos[2]) );
   fInfoView->AddLine( Form("with momentum: (%.3g %.3g %.3g) GeV/c\n", mom[0], mom[1], mom[2]) );
   fInfoView->AddLine( Form("eLoss: %.3g MeV time: %.3g ns\n", point->GetDeltaE()*TAGgeoTrafo::GevToMev(), point->GetTof()*TAGgeoTrafo::SecToNs()) );
   
   if (fConsoleButton->IsOn()) {
      cout << Form("%s sensor id: %d, Hit:\n", name.Data(), trackIdx);
      cout << Form("at position:   (%.3g %.3g %.3g) cm\n", pos[0], pos[1], pos[2]);
      cout << Form("with momentum: (%.3g %.3g %.3g) GeV/c\n", mom[0], mom[1], mom[2]);
      cout << Form("eLoss: %.3g MeV time: %.3g ns\n", point->GetDeltaE()*TAGgeoTrafo::GevToMev(), point->GetTof()*TAGgeoTrafo::SecToNs());
   }
   
   TAMCntuPart* pNtuHit = fReco->GetNtuMcTrk();
   TAMCpart* track = pNtuHit->GetTrack(trackIdx);
   
   fInfoView->AddLine( Form("Generated from track with index: %d\n", trackIdx) );
   fInfoView->AddLine( Form("Charge: %d Mass: %d\n", track->GetCharge(),  TMath::Nint(track->GetMass()/TAGgeoTrafo::GetMassFactor())) );
   
   if (fConsoleButton->IsOn()) {
      cout << Form("Generated from track with index: %d\n", trackIdx);
      cout << Form("Charge: %d Mass: %d\n", track->GetCharge(),  TMath::Nint(track->GetMass()/TAGgeoTrafo::GetMassFactor()));
   }
}

//__________________________________________________________
//! Update track line information output
//!
//! \param[in] ts track line set
//! \param[in] idx associated object index
void TAGeventDisplayMC::UpdateTrackInfo(TEveStraightLineSet* ts, Int_t idx)
{
   TAEDglbTrack* lineTracks = dynamic_cast<TAEDglbTrack*> (ts);
   TObject* obj = lineTracks->GetTrackId();
   
   if (obj->InheritsFrom("TAMCpart")) {
      
      TAMCpart* track =  (TAMCpart*)obj;
      if (track == 0x0) return;
      
      fInfoView->AddLine( Form("Track # %2d\n", track->GetFlukaID()) );
      fInfoView->AddLine( Form("Charge: %d A: %d Mass: %.2f GeV/c2\n", track->GetCharge(), track->GetBaryon(), track->GetMass()) );
      fInfoView->AddLine( Form("Momentum: %.2f GeV/c ToF: %.2f ns\n", track->GetInitP().Mag(), track->GetTof()) );
      
      if (fConsoleButton->IsOn()) {
         cout <<  Form("Track # %2d\n", track->GetFlukaID());
         cout <<  Form("Charge: %d A: %d Mass: %.2f GeV/c2\n", track->GetCharge(), track->GetBaryon(), track->GetMass() );
         cout <<  Form("Momentum: %.2f GeV/c ToF: %.2f ns\n", track->GetInitP().Mag(), track->GetTof() );
      }
   }
}

//__________________________________________________________
//! Update MC elements in view
void TAGeventDisplayMC::UpdateMcElements()
{
   if (TAGrecoManager::GetPar()->IncludeST())
      UpdateMcElements("st");
   
   if (TAGrecoManager::GetPar()->IncludeBM())
      UpdateMcElements("bm");
   
   if (TAGrecoManager::GetPar()->IncludeVT())
      UpdateMcElements("vt");
   
   if (TAGrecoManager::GetPar()->IncludeIT())
      UpdateMcElements("it");
   
   if (TAGrecoManager::GetPar()->IncludeMSD())
      UpdateMcElements("ms");
   
   if (TAGrecoManager::GetPar()->IncludeTW())
      UpdateMcElements("tw");
   
   if (TAGrecoManager::GetPar()->IncludeCA())
      UpdateMcElements("ca");
   
   if (fgMcTrackFlag)
      UpdateTrackElements();
}


//__________________________________________________________
//! Update MC elements in view
void TAGeventDisplayMC::UpdateTrackElements()
{
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      fTrackMcDisplay->ResetTracklets();
   }
   
   if (!fgDisplayFlag) // do not update event display
      return;
   
   TAMCntuPart* pNtuHit = fReco->GetNtuMcTrk();
   if (!pNtuHit) return;
   int nTracks = pNtuHit->GetTracksN();
   for (int trackIdx=0; trackIdx<nTracks; ++trackIdx) {
      TAMCpart* track = pNtuHit->GetTrack(trackIdx);
      TVector3 posI = track->GetInitPos();
      TVector3 posF = track->GetFinalPos();
      TVector3 length = posF - posI;
      // tracks that escape have very long path, let made it shorter for visual propose
      if (length.Mag() > 250) {  posF = posI + length.Unit()*35; }
      if (track->GetCharge() <= 0) continue;
      fTrackMcDisplay->AddTracklet(track->GetCharge()*100, posI, posF);
      fTrackMcDisplay->TrackId(track);
   }
}

//__________________________________________________________
//! Update MC elements for a given device
//!
//! \param[in] prefix prefix device
void TAGeventDisplayMC::UpdateMcElements(const TString prefix)
{
   
   if (!fgGUIFlag || (fgGUIFlag && fRefreshButton->IsOn())) {
      if (prefix == "ca")
         fCaMcDisplay->Reset();
      if (prefix == "tw")
         fTwMcDisplay->Reset();
      if (prefix == "ms")
         fMsdMcDisplay->Reset();
      if (prefix == "it")
         fItMcDisplay->Reset();
      if (prefix == "vt")
         fVtMcDisplay->Reset();
      if (prefix == "bm")
         fBmMcDisplay->Reset();
      if (prefix == "st")
         fStMcDisplay->Reset();
   }
   
   if (!fgDisplayFlag) // do not update event display
      return;
   
   Float_t  x = 0.,  y = 0.,  z = 0.;
   
   TAMCntuHit* pNtuHit = 0x0;
   
   if (prefix == "st")
      pNtuHit = fReco->GetNtuMcSt();
   
   if (prefix == "bm")
      pNtuHit = fReco->GetNtuMcBm();
   
   if (prefix == "vt")
      pNtuHit = fReco->GetNtuMcVtx();
   
   if (prefix == "it")
      pNtuHit = fReco->GetNtuMcIt();
   
   if (prefix == "ms")
      pNtuHit = fReco->GetNtuMcMsd();
   
   if (prefix == "tw")
      pNtuHit = fReco->GetNtuMcTw();
   
   if (prefix == "ca")
      pNtuHit = fReco->GetNtuMcCa();
   
   Int_t nHits = pNtuHit->GetHitsN();
   if (nHits == 0) return;
   
   for (Int_t iHit = 0; iHit < nHits; ++iHit) {

      TAMChit *hit = pNtuHit->GetHit(iHit);
      TVector3 pos = hit->GetInPosition();
      
      x = pos(0);
      y = pos(1);
      z = pos(2);
      
      if (prefix == "st") {
         fStMcDisplay->AddPoint(x, y, z);
         fStMcDisplay->SetPointId(hit);
      }
      
      if (prefix == "bm") {
         fBmMcDisplay->AddPoint(x, y, z);
         fBmMcDisplay->SetPointId(hit);
      }

      if (prefix == "vt") {
         fVtMcDisplay->AddPoint(x, y, z);
         fVtMcDisplay->SetPointId(hit);
      }

      if (prefix == "it") {
         fItMcDisplay->AddPoint(x, y, z);
         fItMcDisplay->SetPointId(hit);
      }

      if (prefix == "ms") {
         fMsdMcDisplay->AddPoint(x, y, z);
         fMsdMcDisplay->SetPointId(hit);
      }

      if (prefix == "tw") {
         fTwMcDisplay->AddPoint(x, y, z);
         fTwMcDisplay->SetPointId(hit);
      }
      
      if (prefix == "ca") {
         fCaMcDisplay->AddPoint(x, y, z);
         fCaMcDisplay->SetPointId(hit);
      }

   } //end loop on hits

}


