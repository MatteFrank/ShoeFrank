
#include "TAFOeventDisplayMC.hxx"
#include "TEveGeoNode.h"
#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveWindow.h"
#include "TGeoManager.h"

#include "GlobalPar.hxx"
#include "GlobalRecoMC.hxx"

ClassImp(TAFOeventDisplay)

//__________________________________________________________
TAFOeventDisplay* TAFOeventDisplayMC::Instance(Int_t type, const TString name)
{
   if (fgInstance == 0x0)
      fgInstance = new TAFOeventDisplayMC(type, name);
   
   return fgInstance;
}


//__________________________________________________________
TAFOeventDisplayMC::TAFOeventDisplayMC(Int_t type, const TString expName)
 : TAFOeventDisplay(type, expName),
   fCaMcDisplay(new TAEDpoint("Cal MC hit")),
   fTwMcDisplay(new TAEDpoint("ToF MC hit")),
   fMsdMcDisplay(new TAEDpoint("MSD MC hit")),
   fItMcDisplay(new TAEDpoint("IT MC hit")),
   fVtMcDisplay(new TAEDpoint("VTX MC hit")),
   fBmMcDisplay(new TAEDpoint("STC MC hit")),
   fStMcDisplay(new TAEDpoint("STC MC hit"))
{
   // local reco
   SetLocalReco();
}

//__________________________________________________________
TAFOeventDisplayMC::~TAFOeventDisplayMC()
{
   // default destructor   
   delete fCaMcDisplay;
   delete fTwMcDisplay;
   delete fMsdMcDisplay;
   delete fItMcDisplay;
   delete fVtMcDisplay;
   delete fBmMcDisplay;
   delete fStMcDisplay;
}

//__________________________________________________________
void TAFOeventDisplayMC::SetLocalReco()
{
   if (fType != 1) return;
   
   // local reco
   fReco = new GlobalRecoMC(fExpName);
   
   fReco->DisableTree();
   fReco->DisableSaveHits();
   fReco->EnableHisto();

   if (fgTrackFlag) {
      fReco->SetTrackingAlgo(fgVtxTrackingAlgo[0]);
      fReco->EnableTracking();
   }
   
   fpFootGeo = fReco->GetGeoTrafo();
}

//__________________________________________________________
Bool_t TAFOeventDisplayMC::GetEntry(Int_t entry)
{
   if (!fReco->GetTree()->GetEntry(entry)) return false;
   
   return true;
}

//__________________________________________________________
void TAFOeventDisplayMC::CreateRawAction()
{
   fReco->CreateRawAction();
}

//__________________________________________________________
void TAFOeventDisplayMC::AddRequiredItem()
{
   fReco->AddRawRequiredItem();
   fReco->AddRecRequiredItem();
   
   gTAGroot->BeginEventLoop();
   gTAGroot->Print();
}


//__________________________________________________________
void TAFOeventDisplayMC::AddElements()
{
   TAFOeventDisplay::AddElements();

   if (GlobalPar::GetPar()->IncludeCA()) {
      fCaMcDisplay->ResetPoints();
      gEve->AddElement(fCaMcDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      fTwMcDisplay->ResetPoints();
      gEve->AddElement(fTwMcDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()){
      fMsdMcDisplay->ResetPoints();
      gEve->AddElement(fMsdMcDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fItMcDisplay->ResetPoints();
      gEve->AddElement(fItMcDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeVertex()) {
      fVtMcDisplay->ResetPoints();
      gEve->AddElement(fVtMcDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fBmMcDisplay->ResetPoints();
      gEve->AddElement(fBmMcDisplay);
   }
   
   if (GlobalPar::GetPar()->IncludeST()) {
      fStMcDisplay->ResetPoints();
      gEve->AddElement(fStMcDisplay);
   }
}

//__________________________________________________________
void TAFOeventDisplayMC::ConnectElements()
{
   TAFOeventDisplay::ConnectElements();
   
   fCaMcDisplay->Connect("PointSelected(Int_t )", "TAFOeventDisplayMC", this, "UpdateCaInfo(Int_t)");
   fTwMcDisplay->Connect("PointSelected(Int_t )", "TAFOeventDisplayMC", this, "UpdateTwInfo(Int_t)");
   fMsdMcDisplay->Connect("PointSelected(Int_t )", "TAFOeventDisplayMC", this, "UpdateMsInfo(Int_t)");
   fItMcDisplay->Connect("PointSelected(Int_t )", "TAFOeventDisplayMC", this, "UpdateItInfo(Int_t)");
   fVtMcDisplay->Connect("PointSelected(Int_t )", "TAFOeventDisplayMC", this, "UpdateVtInfo(Int_t)");
   fBmMcDisplay->Connect("PointSelected(Int_t )", "TAFOeventDisplayMC", this, "UpdateBmInfo(Int_t)");
   fStMcDisplay->Connect("PointSelected(Int_t )", "TAFOeventDisplayMC", this, "UpdateStInfo(Int_t)");
}

//__________________________________________________________
void TAFOeventDisplayMC::UpdateStInfo(Int_t idx)
{
   UpdateMcInfo("st", idx);
}

//__________________________________________________________
void TAFOeventDisplayMC::UpdateBmInfo(Int_t idx)
{
   UpdateMcInfo("bm", idx);
}

//__________________________________________________________
void TAFOeventDisplayMC::UpdateVtInfo(Int_t idx)
{
   UpdateMcInfo("vt", idx);
}

//__________________________________________________________
void TAFOeventDisplayMC::UpdateItInfo(Int_t idx)
{
   UpdateMcInfo("it", idx);
}

//__________________________________________________________
void TAFOeventDisplayMC::UpdateMsInfo(Int_t idx)
{
   UpdateMcInfo("ms", idx);
}

//__________________________________________________________
void TAFOeventDisplayMC::UpdateTwInfo(Int_t idx)
{
   UpdateMcInfo("tw", idx);
}

//__________________________________________________________
void TAFOeventDisplayMC::UpdateCaInfo(Int_t idx)
{
   UpdateMcInfo("ca", idx);
}

//__________________________________________________________
void TAFOeventDisplayMC::UpdateMcInfo(TString prefix, Int_t idx)
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

   TVector3 pos = point->GetInPosition();
   TVector3 mom = point->GetInMomentum();
   
   fInfoView->AddLine( Form("%s sensor id: %d, Hit:\n", name.Data(), point->GetID()) );
   fInfoView->AddLine( Form("at position:   (%.3g %.3g %.3g) cm\n", pos[0], pos[1], pos[2]) );
   fInfoView->AddLine( Form("with momentum: (%.3g %.3g %.3g) GeV/c\n", mom[0], mom[1], mom[2]) );
   fInfoView->AddLine( Form("eLoss: %.3g MeV time: %.3g ns\n", point->GetDeltaE()*TAGgeoTrafo::GevToMev(), point->GetTof()*TAGgeoTrafo::SecToNs()) );
   
   Int_t trackId       = point->GetTrackId();
   TAMCntuEve* pNtuHit = fReco->GetNtuMcEve();
   TAMCeveTrack* track   = pNtuHit->GetHit(trackId);
   
   fInfoView->AddLine( Form("Generated from track # %d\n", trackId) );
   fInfoView->AddLine( Form("Charge: %d Mass: %.3g GeV/c2\n", track->GetCharge(), track->GetMass()) );
}

//__________________________________________________________
void TAFOeventDisplayMC::UpdateElements()
{
   if (GlobalPar::GetPar()->IncludeST())
      UpdateMcElements("st");
   
   if (GlobalPar::GetPar()->IncludeBM())
      UpdateMcElements("bm");
   
   if (GlobalPar::GetPar()->IncludeVertex())
      UpdateMcElements("vt");
   
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      UpdateMcElements("it");
   
   if (GlobalPar::GetPar()->IncludeMSD())
      UpdateMcElements("ms");
   
   if (GlobalPar::GetPar()->IncludeTW())
      UpdateMcElements("tw");
   
   if (GlobalPar::GetPar()->IncludeCA())
      UpdateMcElements("ca");
   
   TAFOeventDisplay::UpdateElements();
}

//__________________________________________________________
void TAFOeventDisplayMC::UpdateMcElements(const TString prefix)
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


