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
#include "TAGactTreeReader.hxx"

ClassImp(LocalRecoMC)

//__________________________________________________________
LocalRecoMC::LocalRecoMC(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout),
   fActNtuRawVtx(0x0),
   fActNtuRawIt(0x0),
   fActNtuRawMsd(0x0),
   fTree(0x0)
{
   fEvtStruct = new EVENT_STRUCT;
   fFlagMC = true;
}

//__________________________________________________________
LocalRecoMC::~LocalRecoMC()
{
   // default destructor
}

//__________________________________________________________
void LocalRecoMC::CreateRawAction()
{
   fActEvtReader = new TAGactTreeReader("actEvtReader");

   fpNtuMcEve = new TAGdataDsc(TAMCntuEve::GetDefDataName(), new TAMCntuEve());
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
      fActNtuRawBm = new TABMactNtuMC("bmActNtu", fpNtuRawBm, fpParConfBm, fpParCalBm, fpParGeoBm, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawBm->CreateHistogram();
      
      fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
      fActNtuMcBm = new TAMCactNtuBm("bmActNtuMc", fpNtuMcBm, fEvtStruct);
   }
   
   if (GlobalPar::GetPar()->IncludeVT()) {
      fpNtuRawVtx = new TAGdataDsc("vtRaw", new TAVTntuRaw());
      fActNtuRawVtx = new TAVTactNtuMC("vtActNtu", fpNtuRawVtx, fpParGeoVtx, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawVtx->CreateHistogram();
      
      fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
      fActNtuMcVt = new TAMCactNtuVtx("vtActNtuMc", fpNtuMcVt, fEvtStruct);
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
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
      fActNtuRawTw = new TATWactNtuMC("twActNtu", fpNtuRawTw,  fpParCalTw, fpParGeoG, fEvtStruct,fFlagZtrueMC,fFlagZrecPUoff);
      if (fFlagHisto)
	fActNtuRawTw->CreateHistogram();
      
      fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
      fActNtuMcTw = new TAMCactNtuTof("twActNtuMc", fpNtuMcTw, fEvtStruct);
   }
   
   if(GlobalPar::GetPar()->IncludeCA()) {
      fpNtuRawCa   = new TAGdataDsc("caRaw", new TACAntuRaw());
      fActNtuRawCa = new TACAactNtuMC("caActNtu", fpNtuRawCa, fpParGeoCa, fpParGeoG, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawCa->CreateHistogram();
      
      fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
      fActNtuMcCa = new TAMCactNtuCal("caActNtuMc", fpNtuMcCa, fEvtStruct);
   }
}

//__________________________________________________________
void LocalRecoMC::OpenFileIn()
{
   fActEvtReader->Open(GetName(), "READ", "EventTree", false);
   fTree = ((TAGactTreeReader*)fActEvtReader)->GetTree();
   
   Evento *ev  = new Evento();
   ev->FindBranches(fTree, fEvtStruct);
}

//__________________________________________________________
void LocalRecoMC::SetRawHistogramDir()
{
   // ST
   if (GlobalPar::GetPar()->IncludeST()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TASTparGeo::GetBaseName());
      fActNtuRawSt->SetHistogramDir(subfolder);
   }
   
   // BM
   if (GlobalPar::GetPar()->IncludeBM()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TABMparGeo::GetBaseName());
      fActNtuRawBm->SetHistogramDir(subfolder);
   }
   
   // VTX
   if (GlobalPar::GetPar()->IncludeVT()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAVTparGeo::GetBaseName());
      fActNtuRawVtx->SetHistogramDir(subfolder);
   }
   
   // IT
   if (GlobalPar::GetPar()->IncludeIT()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAITparGeo::GetBaseName());
      fActNtuRawIt->SetHistogramDir(subfolder);
   }
   
   // MSD
   if (GlobalPar::GetPar()->IncludeMSD()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAMSDparGeo::GetBaseName());
      fActNtuRawMsd->SetHistogramDir(subfolder);
   }
   
   // TOF
   if (GlobalPar::GetPar()->IncludeTW()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TATWparGeo::GetBaseName());
      fActNtuRawTw->SetHistogramDir(subfolder);
   }
   
   // CAL
   if (GlobalPar::GetPar()->IncludeCA()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TACAparGeo::GetBaseName());
      fActNtuRawCa->SetHistogramDir(subfolder);
   }
}

//__________________________________________________________
void LocalRecoMC::CloseFileIn()
{
   fActEvtReader->Close();
}

//__________________________________________________________
void LocalRecoMC::AddRawRequiredItem()
{
  fTAGroot->AddRequiredItem("actEvtReader");
   fTAGroot->AddRequiredItem("eveActNtuMc");
   
   if (GlobalPar::GetPar()->IncludeST())
      AddRequiredMcItemSt();
   
   if (GlobalPar::GetPar()->IncludeBM())
      AddRequiredMcItemBm();
   
   if (GlobalPar::GetPar()->IncludeVT())
      AddRequiredMcItemVt();
   
   if (GlobalPar::GetPar()->IncludeIT())
      AddRequiredMcItemIt();
   
   if (GlobalPar::GetPar()->IncludeMSD())
      AddRequiredMcItemMs();
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      AddRequiredMcItemTw();
   }
   
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
   
   if (GlobalPar::GetPar()->IncludeVT()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawVtx, TAVTntuRaw::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
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
