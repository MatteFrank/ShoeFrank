#include "TTree.h"

#include "LocalRecoMC.hxx"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAMCflukaParser.hxx"
#include "TAMCntuTrack.hxx"
#include "TAMCntuEvent.hxx"
#include "TAMCntuRegion.hxx"
#include "TASTntuHit.hxx"
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
   fActNtuHitVtx(0x0),
   fActNtuHitIt(0x0),
   fActNtuHitMsd(0x0),
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
  
  fpNtuMcReg = new TAGdataDsc("regMc", new TAMCntuRegion());
  if (GlobalPar::GetPar()->IsReadRootObj())
      fActEvtReader->SetupBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
  else
    fActNtuMcReg = new TAMCactNtuRegion("regActNtuMc", fpNtuMcReg, fEvtStruct);

   fpNtuMcEvt    = new TAGdataDsc("evtMc", new TAMCntuEvent());
   if (GlobalPar::GetPar()->IsReadRootObj())
    fActEvtReader->SetupBranch(fpNtuMcEvt,TAMCntuEvent::GetBranchName());
   else
     fActNtuMcEvt = new TAMCactNtuEvent("evtActNtuMc", fpNtuMcEvt, fEvtStruct);
  
   fpNtuMcTrk    = new TAGdataDsc("eveMc", new TAMCntuTrack());
   if (GlobalPar::GetPar()->IsReadRootObj())
     fActEvtReader->SetupBranch(fpNtuMcTrk,TAMCntuTrack::GetBranchName());
   else
    fActNtuMcTrk = new TAMCactNtuTrack("eveActNtuMc", fpNtuMcTrk, fEvtStruct);

   if (GlobalPar::GetPar()->IncludeST() || GlobalPar::GetPar()->IncludeTW()) {
      fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
      
      fpNtuHitSt = new TAGdataDsc("stRaw", new TASTntuHit());
      fActNtuHitSt = new TASTactNtuHitMC("stActNtu", fpNtuMcSt, fpNtuMcTrk, fpNtuHitSt, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitSt->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());

      fpNtuHitBm = new TAGdataDsc("bmRaw", new TABMntuHit());
      fActNtuHitBm = new TABMactNtuHitMC("bmActNtu", fpNtuMcBm, fpNtuMcTrk, fpNtuHitBm, fpParConfBm, fpParCalBm, fpParGeoBm, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitBm->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeVT()) {
      fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
      
      fpNtuHitVtx = new TAGdataDsc("vtRaw", new TAVTntuHit());
      fActNtuHitVtx = new TAVTactNtuHitMC("vtActNtu", fpNtuMcVt, fpNtuMcTrk, fpNtuHitVtx, fpParGeoVtx, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitVtx->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
      fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
      
      fpNtuHitIt = new TAGdataDsc("itRaw", new TAITntuHit());
      fActNtuHitIt = new TAITactNtuHitMC("itActNtu", fpNtuMcIt, fpNtuMcTrk, fpNtuHitIt, fpParGeoIt, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitIt->CreateHistogram();
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
      
      fpNtuHitMsd = new TAGdataDsc("msdRaw", new TAMSDntuHit());
      fActNtuHitMsd = new TAMSDactNtuHitMC("msdActNtu", fpNtuMcMsd, fpNtuMcTrk, fpNtuHitMsd, fpParGeoMsd, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitMsd->CreateHistogram();
   }
   
   if(GlobalPar::GetPar()->IncludeTW()) {
      fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
      
      fpNtuHitTw   = new TAGdataDsc("twRaw", new TATWntuHit());
      fActNtuHitTw = new TATWactNtuHitMC("twActNtu", fpNtuMcTw, fpNtuMcSt, fpNtuMcTrk, fpNtuHitTw,  fpParCalTw, fpParGeoG, fFlagZtrueMC, fFlagZrecPUoff, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitTw->CreateHistogram();
   }
   
   if(GlobalPar::GetPar()->IncludeCA()) {
      fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
      if (GlobalPar::GetPar()->IsReadRootObj())
        fActEvtReader->SetupBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
      
      fpNtuHitCa   = new TAGdataDsc("caRaw", new TACAntuHit());
      fActNtuHitCa = new TACAactNtuHitMC("caActNtu", fpNtuMcCa, fpNtuMcTrk, fpNtuHitCa, fpParGeoCa, fpParCalCa, fpParGeoG, fEvtStruct);
      if (fFlagHisto)
         fActNtuHitCa->CreateHistogram();
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
      fActNtuHitSt->SetHistogramDir(subfolder);
   }
  
   // BM
   if (GlobalPar::GetPar()->IncludeBM()) {
     TDirectory* subfolder = fActEvtWriter->File()->mkdir(TABMparGeo::GetBaseName());
      fActNtuHitBm->SetHistogramDir(subfolder);
   }
  
   // VTX
   if (GlobalPar::GetPar()->IncludeVT()) {
     TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAVTparGeo::GetBaseName());
      fActNtuHitVtx->SetHistogramDir(subfolder);
   }
  
   // IT
   if (GlobalPar::GetPar()->IncludeIT()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAITparGeo::GetBaseName());
      fActNtuHitIt->SetHistogramDir(subfolder);
   }
  
   // MSD
   if (GlobalPar::GetPar()->IncludeMSD()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TAMSDparGeo::GetBaseName());
      fActNtuHitMsd->SetHistogramDir(subfolder);
   }
  
   // TOF
   if (GlobalPar::GetPar()->IncludeTW()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TATWparGeo::GetBaseName());
      fActNtuHitTw->SetHistogramDir(subfolder);
   }
   
   // CAL
   if (GlobalPar::GetPar()->IncludeCA()) {
      TDirectory* subfolder = fActEvtWriter->File()->mkdir(TACAparGeo::GetBaseName());
      fActNtuHitCa->SetHistogramDir(subfolder);
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
   if (!GlobalPar::GetPar()->IsReadRootObj()) {
     fTAGroot->AddRequiredItem("eveActNtuMc");
     fTAGroot->AddRequiredItem("evtActNtuMc");
     fTAGroot->AddRequiredItem("regActNtuMc");
   }
}

//__________________________________________________________
void LocalRecoMC::SetTreeBranches()
{
   BaseReco::SetTreeBranches();
  
   fActEvtWriter->SetupElementBranch(fpNtuMcEvt, TAMCntuEvent::GetBranchName());
   fActEvtWriter->SetupElementBranch(fpNtuMcTrk, TAMCntuTrack::GetBranchName());
  
  if (fActEvtReader->CheckBranch(TAMCntuRegion::GetBranchName()) || !GlobalPar::GetPar()->IsReadRootObj() )
    fActEvtWriter->SetupElementBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());


   if (GlobalPar::GetPar()->IncludeST()) {
      fActEvtWriter->SetupElementBranch(fpNtuHitSt, TASTntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      fActEvtWriter->SetupElementBranch(fpNtuHitBm, TABMntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeVT()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuHitVtx, TAVTntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeIT()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuHitIt, TAITntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeMSD()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuHitMsd, TAMSDntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeTW()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuHitTw, TATWntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
   }
   
   if (GlobalPar::GetPar()->IncludeCA()) {
      if (fFlagHits)
         fActEvtWriter->SetupElementBranch(fpNtuHitCa, TACAntuHit::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
   }
}

