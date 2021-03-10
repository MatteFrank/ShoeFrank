#include "TTree.h"

#include "LocalRecoMC.hxx"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAMCflukaParser.hxx"
#include "TAMCntuTrack.hxx"
#include "TASTntuRaw.hxx"
#include "TABMntuHit.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAMSDntuHit.hxx"
#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuHit.hxx"

ClassImp(LocalRecoMC)

//__________________________________________________________
LocalRecoMC::LocalRecoMC(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
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
LocalRecoMC::~LocalRecoMC()
{
   // default destructor
}

//__________________________________________________________
void LocalRecoMC::CreateRawAction()
{
   fActEvtReader = new TAGactTreeReader("actEvtReader");
   fpNtuMcTrk    = new TAGdataDsc("eveMc", new TAMCntuTrack());
   if (GlobalPar::GetPar()->IsReadRootObj())
     fActEvtReader->SetupBranch(fpNtuMcTrk,TAMCntuTrack::GetBranchName());
  else
    fActNtuMcTrk = new TAMCactNtuTrack("eveActNtuMc", fpNtuMcTrk, fEvtStruct);

   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW()) {
      fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
      
      fpNtuRawSt = new TAGdataDsc("stRaw", new TASTntuRaw());
      fActNtuRawSt = new TASTactNtuHitMC("stActNtu", fpNtuMcSt, fpNtuMcTrk, fpNtuRawSt, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawSt->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());

      fpNtuRawBm = new TAGdataDsc("bmRaw", new TABMntuHit());
      fActNtuRawBm = new TABMactNtuHitMC("bmActNtu", fpNtuMcBm, fpNtuMcTrk, fpNtuRawBm, fpParConfBm, fpParCalBm, fpParGeoBm, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawBm->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeVT()) {
      fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
      
      fpNtuRawVtx = new TAGdataDsc("vtRaw", new TAVTntuHit());
      fActNtuRawVtx = new TAVTactNtuHitMC("vtActNtu", fpNtuMcVt, fpNtuMcTrk, fpNtuRawVtx, fpParGeoVtx, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawVtx->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
      fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
      
      fpNtuRawIt = new TAGdataDsc("itRaw", new TAITntuHit());
      fActNtuRawIt = new TAITactNtuHitMC("itActNtu", fpNtuMcIt, fpNtuMcTrk, fpNtuRawIt, fpParGeoIt, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawIt->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
      
      fpNtuRawMsd = new TAGdataDsc("msdRaw", new TAMSDntuHit());
      fActNtuRawMsd = new TAMSDactNtuHitMC("msdActNtu", fpNtuMcMsd, fpNtuMcTrk, fpNtuRawMsd, fpParGeoMsd, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawMsd->CreateHistogram();
   }
   
   if(GlobalPar::GetPar()->IncludeTW()) {
      fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
      
      fpNtuRawTw   = new TAGdataDsc("twRaw", new TATWntuHit());
      fActNtuRawTw = new TATWactNtuHitMC("twActNtu", fpNtuMcTw, fpNtuMcSt, fpNtuMcTrk, fpNtuRawTw,  fpParCalTw, fpParGeoG, fFlagZtrueMC, fFlagZrecPUoff, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawTw->CreateHistogram();
   }
   
   if(GlobalPar::GetPar()->IncludeCA()) {
      fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
      
      fpNtuRawCa   = new TAGdataDsc("caRaw", new TACAntuHit());
      fActNtuRawCa = new TACAactNtuHitMC("caActNtu", fpNtuMcCa, fpNtuMcTrk, fpNtuRawCa, fpParGeoCa, fpParCalCa, fpParGeoG, fEvtStruct);
      if (fFlagHisto)
         fActNtuRawCa->CreateHistogram();
   }
}

//__________________________________________________________
void LocalRecoMC::OpenFileIn()
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
void LocalRecoMC::GlobalChecks()
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
  if (!GlobalPar::GetPar()->IsReadRootObj())
    fTAGroot->AddRequiredItem("eveActNtuMc");
}

//__________________________________________________________
void LocalRecoMC::SetTreeBranches()
{
   BaseReco::SetTreeBranches();
   
   fActEvtWriter->SetupElementBranch(fpNtuMcTrk, TAMCntuTrack::GetBranchName());

   if (GlobalPar::GetPar()->IncludeST()) {
      fActEvtWriter->SetupElementBranch(fpNtuRawSt, TASTntuRaw::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fActEvtWriter->SetupElementBranch(fpNtuRawBm, TABMntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeVT()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawVtx, TAVTntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawIt, TAITntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawMsd, TAMSDntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawTw, TATWntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuRawCa, TACAntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
   }
}

