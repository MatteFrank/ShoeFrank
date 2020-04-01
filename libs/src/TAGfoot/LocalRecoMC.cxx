#include "TTree.h"

#include "LocalRecoMC.hxx"

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

ClassImp(LocalRecoMC)

//__________________________________________________________
LocalRecoMC::LocalRecoMC(TString expName, TString fileNameIn, TString fileNameout)
 : BaseReco(expName, fileNameIn, fileNameout),
   fActNtuRawVtx(0x0),
   fActNtuRawIt(0x0),
   fActNtuRawMsd(0x0),
   fTree(0x0),
   fActEvtReader(0x0)
{
   fEvtStruct = new EVENT_STRUCT;
   fFlagMC = true;
}

//__________________________________________________________
LocalRecoMC::~LocalRecoMC()
{
   // default destructor
   if (fActEvtReader) delete fActEvtReader;
}


//__________________________________________________________
void LocalRecoMC::LoopEvent(Int_t nEvents)
{
   if (nEvents == 0)
      nEvents = fTree->GetEntries();
   
   if (nEvents > fTree->GetEntries())
      nEvents = fTree->GetEntries();
   
   for (Long64_t ientry = 0; ientry < nEvents; ientry++) {
      
      fTree->GetEntry(ientry);
      
      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;
      
      if (!fTAGroot->NextEvent()) break;
   }
}

//__________________________________________________________
void LocalRecoMC::CreateRawAction()
{
   fpNtuMcEve = new TAGdataDsc("eveMc", new TAMCntuEve());
   fActNtuMcEve = new TAMCactNtuEve("eveActNtuMc", fpNtuMcEve, fEvtStruct);
   
   
   if (GlobalPar::GetPar()->IncludeST()) {
      fpNtuRawSt = new TAGdataDsc("stRaw", new TASTntuRaw());
      fActNtuRawSt = new TASTactNtuMC("stActNtu", fpNtuRawSt, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawSt->CreateHistogram();
      
      fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
      fActNtuMcSt = new TAMCactNtuStc("stActNtuMc", fpNtuMcSt, fEvtStruct);
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fpNtuRawBm = new TAGdataDsc("bmRaw", new TABMntuRaw());
      fActNtuRawBm = new TABMactNtuMC("bmActNtu", fpNtuRawBm, fpParConfBm, fpParGeoBm, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawBm->CreateHistogram();
      
      fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
      fActNtuMcBm = new TAMCactNtuBm("bmActNtuMc", fpNtuMcBm, fEvtStruct);
   }
   
   if (GlobalPar::GetPar()->IncludeVertex()) {
      fpNtuRawVtx = new TAGdataDsc("vtRaw", new TAVTntuRaw());
      fActNtuRawVtx = new TAVTactNtuMC("vtActNtu", fpNtuRawVtx, fpParGeoVtx, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawVtx->CreateHistogram();
      
      fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
      fActNtuMcVt = new TAMCactNtuVtx("vtActNtuMc", fpNtuMcVt, fEvtStruct);
   }
   
   if (GlobalPar::GetPar()->IncludeInnerTracker()) {
      fpNtuRawIt = new TAGdataDsc("itRaw", new TAITntuRaw());
      fActNtuRawIt = new TAITactNtuMC("itActNtu", fpNtuRawIt, fpParGeoIt, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawIt->CreateHistogram();
      
      fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
      fActNtuMcIt = new TAMCactNtuItr("itActNtuMc", fpNtuMcIt, fEvtStruct);
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fpNtuRawMsd = new TAGdataDsc("msdRaw", new TAMSDntuRaw());
      fActNtuRawMsd = new TAMSDactNtuMC("msdActNtu", fpNtuRawMsd, fpParGeoMsd, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawMsd->CreateHistogram();
      
      fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
      fActNtuMcMsd = new TAMCactNtuMsd("msdActNtuMc", fpNtuMcMsd, fEvtStruct);
   }
   
   if(GlobalPar::GetPar()->IncludeTW()) {

      fpNtuRawTw   = new TAGdataDsc("twRaw", new TATWntuRaw());
      fActNtuRawTw = new TATWactNtuMC("twActNtu", fpNtuRawTw, fpParCalTw, fpParGeoTw, fEvtStruct);
      fActNtuRawTw->CreateHistogram();
      
      fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
      fActNtuMcTw = new TAMCactNtuTof("twActNtuMc", fpNtuMcTw, fEvtStruct);
   }
   
   if(GlobalPar::GetPar()->IncludeCA()) {
      fpNtuRawCa   = new TAGdataDsc("caRaw", new TACAntuRaw());
      fActNtuRawCa = new TACAactNtuMC("caActNtu", fpNtuRawCa, fpParGeoCa, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawCa->CreateHistogram();
      
      fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
      fActNtuMcCa = new TAMCactNtuCal("caActNtuMc", fpNtuMcCa, fEvtStruct);
   }
}

//__________________________________________________________
void LocalRecoMC::OpenFileIn()
{
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fActGlbTrack->Open(GetName());
      fTree = fActGlbTrack->GetTree();
      return;
   }
   
   fActEvtReader = new TFile(GetName());
   fTree = (TTree*)fActEvtReader->Get("EventTree");
   
   Evento *ev  = new Evento();
   ev->FindBranches(fTree, fEvtStruct);
}

//__________________________________________________________
void LocalRecoMC::SetRawHistogramDir()
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
      // fActNtuRawTw->SetHistogramDir((TDirectory*)fActEvtWriter->File());
   
   // CAL
   if (GlobalPar::GetPar()->IncludeCA())
      fActNtuRawCa->SetHistogramDir((TDirectory*)fActEvtWriter->File());

}

//__________________________________________________________
void LocalRecoMC::CloseFileIn()
{
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fActGlbTrack->Close();
      return;
   }

   fActEvtReader->Close();
}

//__________________________________________________________
void LocalRecoMC::AddRawRequiredItem()
{
   fTAGroot->AddRequiredItem("eveActNtuMc");
   
   if (GlobalPar::GetPar()->IncludeST())
      AddRequiredMcItemSt();
   
   if (GlobalPar::GetPar()->IncludeBM())
      AddRequiredMcItemBm();
   
   if (GlobalPar::GetPar()->IncludeVertex())
      AddRequiredMcItemVt();
   
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      AddRequiredMcItemIt();
   
   if (GlobalPar::GetPar()->IncludeMSD())
      AddRequiredMcItemMs();
   
   if (GlobalPar::GetPar()->IncludeTW())
      AddRequiredMcItemTw();
   
   if (GlobalPar::GetPar()->IncludeCA())
      AddRequiredMcItemCa();
}
//__________________________________________________________
void LocalRecoMC::AddRequiredMcItemSt()
{
   fTAGroot->AddRequiredItem("stActNtuMc");
}

//__________________________________________________________
void LocalRecoMC::AddRequiredMcItemBm()
{
   fTAGroot->AddRequiredItem("bmActNtuMc");
}

//__________________________________________________________
void LocalRecoMC::AddRequiredMcItemVt()
{
   fTAGroot->AddRequiredItem("vtActNtuMc");
}

//__________________________________________________________
void LocalRecoMC::AddRequiredMcItemIt()
{
   fTAGroot->AddRequiredItem("itActNtuMc");
}

//__________________________________________________________
void LocalRecoMC::AddRequiredMcItemMs()
{
   fTAGroot->AddRequiredItem("msdActNtuMc");
}

//__________________________________________________________
void LocalRecoMC::AddRequiredMcItemTw()
{
   fTAGroot->AddRequiredItem("twActNtuMc");
}

//__________________________________________________________
void LocalRecoMC::AddRequiredMcItemCa()
{
   fTAGroot->AddRequiredItem("caActNtuMc");
}
//__________________________________________________________
void LocalRecoMC::SetTreeBranches()
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

