#include "TTree.h"

#include "LocalRecoNtuMC.hxx"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"
#include "TASTntuRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TAVTntuRaw.hxx"
#include "TAITntuRaw.hxx"
#include "TAMSDntuRaw.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuRaw.hxx"

ClassImp(LocalRecoNtuMC)

//__________________________________________________________
LocalRecoNtuMC::LocalRecoNtuMC(TString expName, TString fileNameIn, TString fileNameout)
 : BaseReco(expName, fileNameIn, fileNameout),
   fActNtuRawVtx(0x0),
   fActNtuRawIt(0x0),
   fActNtuRawMsd(0x0),
   fActEvtReader(0x0)
{
   fFlagMC = true;
}

//__________________________________________________________
LocalRecoNtuMC::~LocalRecoNtuMC()
{
   // default destructor
   if (fActEvtReader) delete fActEvtReader;
}


//__________________________________________________________
void LocalRecoNtuMC::LoopEvent(Int_t nEvents)
{
   for (Int_t ientry = 0; ientry < nEvents; ientry++) {
      
      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;
      
      if (!fTAGroot->NextEvent()) break;;
   }
}

//__________________________________________________________
void LocalRecoNtuMC::CreateRawAction()
{
   fActEvtReader = new TAGactTreeReader("actEvtReader");
   fpNtuMcEve    = new TAGdataDsc("eveMc", new TAMCntuEve());
   fActEvtReader->SetupBranch(fpNtuMcEve,TAMCntuEve::GetBranchName());

   
   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW()) {
      fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
      fActEvtReader->SetupBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
      
      fpNtuRawSt = new TAGdataDsc("stRaw", new TASTntuRaw());
      fActNtuRawSt = new TASTactNtuHitMC("stActNtu", fpNtuMcSt, fpNtuMcEve, fpNtuRawSt);
      if (fFlagHisto)
         fActNtuRawSt->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
      fActEvtReader->SetupBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());

      fpNtuRawBm = new TAGdataDsc("bmRaw", new TABMntuRaw());
      fActNtuRawBm = new TABMactNtuHitMC("bmActNtu", fpNtuMcBm, fpNtuMcEve, fpNtuRawBm, fpParConfBm, fpParGeoBm);
      if (fFlagHisto)
         fActNtuRawBm->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeVertex()) {
      fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
      fActEvtReader->SetupBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
      
      fpNtuRawVtx = new TAGdataDsc("vtRaw", new TAVTntuRaw());
      fActNtuRawVtx = new TAVTactNtuHitMC("vtActNtu", fpNtuMcVt, fpNtuMcEve, fpNtuRawVtx, fpParGeoVtx);
      if (fFlagHisto)
         fActNtuRawVtx->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
      fActEvtReader->SetupBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
      
      fpNtuRawIt = new TAGdataDsc("itRaw", new TAITntuRaw());
      fActNtuRawIt = new TAITactNtuHitMC("itActNtu", fpNtuMcIt, fpNtuMcEve, fpNtuRawIt, fpParGeoIt);
      if (fFlagHisto)
         fActNtuRawIt->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
      fActEvtReader->SetupBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
      
      fpNtuRawMsd = new TAGdataDsc("msdRaw", new TAMSDntuRaw());
      fActNtuRawMsd = new TAMSDactNtuHitMC("msdActNtu", fpNtuMcMsd, fpNtuMcEve, fpNtuRawMsd, fpParGeoMsd);
      if (fFlagHisto)
         fActNtuRawMsd->CreateHistogram();
   }
   
   if(GlobalPar::GetPar()->IncludeTW()) {
      fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
      fActEvtReader->SetupBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
      
      fpNtuRawTw   = new TAGdataDsc("twRaw", new TATWntuRaw());
      fActNtuRawTw = new TATWactNtuHitMC("twActNtu", fpNtuMcTw, fpNtuMcSt, fpNtuMcEve, fpNtuRawTw,  fpParCalTw, fpParGeoG, fFlagZtrueMC);
      if (fFlagHisto)
         fActNtuRawTw->CreateHistogram();
   }
   
   if(GlobalPar::GetPar()->IncludeCA()) {
      fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
      fActEvtReader->SetupBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
      
      fpNtuRawCa   = new TAGdataDsc("caRaw", new TACAntuRaw());
      fActNtuRawCa = new TACAactNtuHitMC("caActNtu", fpNtuMcCa, fpNtuMcEve, fpNtuRawCa, fpParGeoCa);
      if (fFlagHisto)
         fActNtuRawCa->CreateHistogram();
   }
}

//__________________________________________________________
void LocalRecoNtuMC::OpenFileIn()
{
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fActGlbTrack->Open(GetName());
      return;
   }
   
   fActEvtReader->Open(GetName(), "READ", "EventTree");
}

//__________________________________________________________
void LocalRecoNtuMC::SetRawHistogramDir()
{
   // ST
   if (GlobalPar::GetPar()->IncludeST())
      fActNtuRawSt->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   
   // BM
   if (GlobalPar::GetPar()->IncludeBM())
      fActNtuRawBm->SetHistogramDir((TDirectory*)fActEvtWriter->File());

   // VTX
   if (GlobalPar::GetPar()->IncludeVertex())
      fActNtuRawVtx->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   
   // IT
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      fActNtuRawIt->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   
   // MSD
   if (GlobalPar::GetPar()->IncludeMSD())
      fActNtuRawMsd->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   
   // TOF
   if (GlobalPar::GetPar()->IncludeTW())
      fActNtuRawTw->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   
   // CAL
   if (GlobalPar::GetPar()->IncludeCA())
      fActNtuRawCa->SetHistogramDir((TDirectory*)fActEvtWriter->File());

}

//__________________________________________________________
void LocalRecoNtuMC::CloseFileIn()
{
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fActGlbTrack->Close();
      return;
   }

   fActEvtReader->Close();
}

//__________________________________________________________
void LocalRecoNtuMC::AddRawRequiredItem()
{
   fTAGroot->AddRequiredItem("actEvtReader");
}

//__________________________________________________________
void LocalRecoNtuMC::SetTreeBranches()
{
   BaseReco::SetTreeBranches();
   
   fActEvtWriter->SetupElementBranch(fpNtuMcEve, TAMCntuEve::GetBranchName());

   if (GlobalPar::GetPar()->IncludeST()) {
      fActEvtWriter->SetupElementBranch(fpNtuRawSt, TASTntuRaw::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fActEvtWriter->SetupElementBranch(fpNtuRawBm, TABMntuRaw::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeVertex()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawVtx, TAVTntuRaw::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawIt, TAITntuRaw::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawMsd, TAMSDntuRaw::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawTw, TATWntuRaw::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawCa, TACAntuRaw::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
   }
}

