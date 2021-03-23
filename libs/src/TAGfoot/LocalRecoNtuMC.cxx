#include "TTree.h"

#include "LocalRecoNtuMC.hxx"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAMCflukaParser.hxx"
#include "TAMCntuEve.hxx"
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
LocalRecoNtuMC::LocalRecoNtuMC(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout),
   fEvtStruct(0x0),
   fActNtuRawVtx(0x0),
   fActNtuRawIt(0x0),
   fActNtuRawMsd(0x0),
   fActEvtReader(0x0)
{
   fFlagMC = true;
  
  if (!GlobalPar::GetPar()->IsReadRootObj())
    fEvtStruct = new EVENT_STRUCT;
}

//__________________________________________________________
LocalRecoNtuMC::~LocalRecoNtuMC()
{
   // default destructor
}

//__________________________________________________________
void LocalRecoNtuMC::CreateRawAction()
{
   fActEvtReader = new TAGactTreeReader("actEvtReader");
  
  fpNtuMcEvt    = new TAGdataDsc("evtMc", new TAMCntuEvent());
  if (GlobalPar::GetPar()->IsReadRootObj())
    fActEvtReader->SetupBranch(fpNtuMcEvt,TAMCntuEvent::GetBranchName());
  
   fpNtuMcEve    = new TAGdataDsc("eveMc", new TAMCntuEve());
   if (GlobalPar::GetPar()->IsReadRootObj())
     fActEvtReader->SetupBranch(fpNtuMcEve,TAMCntuEve::GetBranchName());
  else
    fActNtuMcEve = new TAMCactNtuEve("eveActNtuMc", fpNtuMcEve, fEvtStruct);

   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW()) {
      fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
      
      fpNtuRawSt = new TAGdataDsc("stRaw", new TASTntuRaw());
      fActNtuRawSt = new TASTactNtuHitMC("stActNtu", fpNtuMcSt, fpNtuMcEve, fpNtuRawSt, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawSt->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());

      fpNtuRawBm = new TAGdataDsc("bmRaw", new TABMntuRaw());
      fActNtuRawBm = new TABMactNtuHitMC("bmActNtu", fpNtuMcBm, fpNtuMcEve, fpNtuRawBm, fpParConfBm, fpParCalBm, fpParGeoBm, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawBm->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeVT()) {
      fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
      
      fpNtuRawVtx = new TAGdataDsc("vtRaw", new TAVTntuRaw());
      fActNtuRawVtx = new TAVTactNtuHitMC("vtActNtu", fpNtuMcVt, fpNtuMcEve, fpNtuRawVtx, fpParGeoVtx, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawVtx->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
      fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
      
      fpNtuRawIt = new TAGdataDsc("itRaw", new TAITntuRaw());
      fActNtuRawIt = new TAITactNtuHitMC("itActNtu", fpNtuMcIt, fpNtuMcEve, fpNtuRawIt, fpParGeoIt, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawIt->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
      
      fpNtuRawMsd = new TAGdataDsc("msdRaw", new TAMSDntuRaw());
      fActNtuRawMsd = new TAMSDactNtuHitMC("msdActNtu", fpNtuMcMsd, fpNtuMcEve, fpNtuRawMsd, fpParGeoMsd, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawMsd->CreateHistogram();
   }
   
   if(GlobalPar::GetPar()->IncludeTW()) {
      fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
      
      fpNtuRawTw   = new TAGdataDsc("twRaw", new TATWntuRaw());
      fActNtuRawTw = new TATWactNtuHitMC("twActNtu", fpNtuMcTw, fpNtuMcSt, fpNtuMcEve, fpNtuRawTw,  fpParCalTw, fpParGeoG, fFlagZtrueMC, fFlagZrecPUoff, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawTw->CreateHistogram();
   }
   
   if(GlobalPar::GetPar()->IncludeCA()) {
      fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
      
      fpNtuRawCa   = new TAGdataDsc("caRaw", new TACAntuRaw());
      fActNtuRawCa = new TACAactNtuHitMC("caActNtu", fpNtuMcCa, fpNtuMcEve, fpNtuRawCa, fpParGeoCa, fpParCalCa, fpParGeoG, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawCa->CreateHistogram();
   }
}

//__________________________________________________________
void LocalRecoNtuMC::OpenFileIn()
{
  if (GlobalPar::GetPar()->IsReadRootObj())
    fActEvtReader->Open(GetName(), "READ", "EventTree");
  else {
    fActEvtReader->Open(GetName(), "READ", "EventTree", false);
    TTree* tree = ((TAGactTreeReader*)fActEvtReader)->GetTree();
    TAMCflukaParser::FindBranches(tree, fEvtStruct);
  }
}

//_____________________________________________________________________________
void LocalRecoNtuMC::GlobalChecks()
{
  // base checks
  BaseReco::GlobalChecks();
  
  // from global file
  Bool_t enableRootObjectG = GlobalPar::GetPar()->IsReadRootObj();

  // from root file
  TAGrunInfo info = gTAGroot->CurrentRunInfo();
  TAGrunInfo* p = &info;
  if (!p) return; // if run info not found in MC file
  
  Bool_t enableRootObject = info.GetGlobalPar().EnableRootObject;
  
  if (enableRootObjectG && enableRootObject)
    Info("GlobalChecks()", "Reading MC root file with shoe format");
  
  if (!enableRootObjectG && !enableRootObject)
    Info("GlobalChecks()", "Reading MC root file with Fluka structure format");
  
  if (enableRootObjectG && !enableRootObject) {
    Error("GlobalChecks()", "FootGlobal::enableRootObject set to shoe format but MC file is Fluka structure !");
    exit(0);
  }
  
  if (!enableRootObjectG && enableRootObject) {
    Error("GlobalChecks()", "FootGlobal::enableRootObject set to Fluka structure but MC file is shoe format !");
    exit(0);
  }
}


//__________________________________________________________
void LocalRecoNtuMC::SetRawHistogramDir()
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
void LocalRecoNtuMC::CloseFileIn()
{
   fActEvtReader->Close();
}

//__________________________________________________________
void LocalRecoNtuMC::AddRawRequiredItem()
{
   fTAGroot->AddRequiredItem("actEvtReader");
  if (!GlobalPar::GetPar()->IsReadRootObj())
    fTAGroot->AddRequiredItem("eveActNtuMc");
}

//__________________________________________________________
void LocalRecoNtuMC::SetTreeBranches()
{
   BaseReco::SetTreeBranches();
  
   fActEvtWriter->SetupElementBranch(fpNtuMcEvt, TAMCntuEvent::GetBranchName());
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

