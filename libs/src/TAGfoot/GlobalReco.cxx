
#include "BaseReco.hxx"
#include "TAMCntuEvent.hxx"
#include "TAMCntuRegion.hxx"
#include "GlobalReco.hxx"

ClassImp(GlobalReco)

//__________________________________________________________
GlobalReco::GlobalReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC)
: BaseReco(expName, runNumber, fileNameIn, fileNameout)
{
  GlobalPar::GetPar()->EnableLocalReco();
  fFlagMC = isMC;
}

//__________________________________________________________
GlobalReco::~GlobalReco()
{
}

//__________________________________________________________
void GlobalReco::OpenFileIn()
{
  fActEvtReader->Open(GetName());
}

//__________________________________________________________
void GlobalReco::CloseFileIn()
{
  fActEvtReader->Close();
}

//__________________________________________________________
void GlobalReco::CreateRawAction()
{
}

//__________________________________________________________
void GlobalReco::SetL0TreeBranches()
{
  BaseReco::SetL0TreeBranches();
  
  if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) {
    if (fFlagMC) {
      fpNtuMcTrk = new TAGdataDsc(TAMCntuTrack::GetDefDataName(), new TAMCntuTrack());
      fActEvtReader->SetupBranch(fpNtuMcTrk,TAMCntuTrack::GetBranchName());
      
      fpNtuMcEvt = new TAGdataDsc("evtMc", new TAMCntuEvent());
      fActEvtReader->SetupBranch(fpNtuMcEvt,TAMCntuEvent::GetBranchName());
      
      if (GlobalPar::GetPar()->IsRegionMc()) {
        fpNtuMcReg = new TAGdataDsc("regMc", new TAMCntuRegion());
        fActEvtReader->SetupBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
      }
      
      if (GlobalPar::GetPar()->IncludeKalman() && GlobalPar::GetPar()->IsLocalReco()) {
        if (GlobalPar::GetPar()->IncludeST()) {
          fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcSt,TAMCntuHit::GetStcBranchName());
        }
        
        if (GlobalPar::GetPar()->IncludeBM()) {
          fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcBm,TAMCntuHit::GetBmBranchName());
        }
        
        if (GlobalPar::GetPar()->IncludeVT()) {
          fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcVt,TAMCntuHit::GetVtxBranchName());
        }
        
        if (GlobalPar::GetPar()->IncludeIT()) {
          fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcIt,TAMCntuHit::GetItrBranchName());
        }
        
        if (GlobalPar::GetPar()->IncludeMSD()) {
          fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcMsd,TAMCntuHit::GetMsdBranchName());
        }
        
        if(GlobalPar::GetPar()->IncludeTW()) {
          fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcTw,TAMCntuHit::GetTofBranchName());
        }
        
        if(GlobalPar::GetPar()->IncludeCA()) {
          fpNtuMcCa   = new TAGdataDsc("caMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcCa,TAMCntuHit::GetCalBranchName());
        }
      }
    }
  }
}

//__________________________________________________________
void GlobalReco::SetTreeBranches()
{
  BaseReco::SetTreeBranches();
  
  if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) {
    if (fFlagMC) {
      fActEvtWriter->SetupElementBranch(fpNtuMcEvt, TAMCntuEvent::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcTrk, TAMCntuTrack::GetBranchName());
      
      if (GlobalPar::GetPar()->IsRegionMc() )
        fActEvtWriter->SetupElementBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
    }
  }
}
