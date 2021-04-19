
#include "BaseReco.hxx"
#include "TAMCntuEvent.hxx"
#include "TAMCntuRegion.hxx"
#include "GlobalReco.hxx"

ClassImp(GlobalReco)

//__________________________________________________________
GlobalReco::GlobalReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC)
: BaseReco(expName, runNumber, fileNameIn, fileNameout)
{
  TAGrecoManager::GetPar()->EnableLocalReco();
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
  
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsLocalReco()) {
    if (fFlagMC) {
      fpNtuMcTrk = new TAGdataDsc(TAMCntuPart::GetDefDataName(), new TAMCntuPart());
      fActEvtReader->SetupBranch(fpNtuMcTrk,TAMCntuPart::GetBranchName());
      
      fpNtuMcEvt = new TAGdataDsc("evtMc", new TAMCntuEvent());
      fActEvtReader->SetupBranch(fpNtuMcEvt,TAMCntuEvent::GetBranchName());
      
      if (TAGrecoManager::GetPar()->IsRegionMc()) {
        fpNtuMcReg = new TAGdataDsc("regMc", new TAMCntuRegion());
        fActEvtReader->SetupBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
      }
      
      if (TAGrecoManager::GetPar()->IncludeKalman() && TAGrecoManager::GetPar()->IsLocalReco()) {
        if (TAGrecoManager::GetPar()->IncludeST()) {
          fpNtuMcSt   = new TAGdataDsc("stMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcSt,TAMCntuHit::GetStcBranchName());
        }
        
        if (TAGrecoManager::GetPar()->IncludeBM()) {
          fpNtuMcBm   = new TAGdataDsc("bmMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcBm,TAMCntuHit::GetBmBranchName());
        }
        
        if (TAGrecoManager::GetPar()->IncludeVT()) {
          fpNtuMcVt   = new TAGdataDsc("vtMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcVt,TAMCntuHit::GetVtxBranchName());
        }
        
        if (TAGrecoManager::GetPar()->IncludeIT()) {
          fpNtuMcIt   = new TAGdataDsc("itMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcIt,TAMCntuHit::GetItrBranchName());
        }
        
        if (TAGrecoManager::GetPar()->IncludeMSD()) {
          fpNtuMcMsd   = new TAGdataDsc("msdMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcMsd,TAMCntuHit::GetMsdBranchName());
        }
        
        if(TAGrecoManager::GetPar()->IncludeTW()) {
          fpNtuMcTw   = new TAGdataDsc("twMc", new TAMCntuHit());
          fActEvtReader->SetupBranch(fpNtuMcTw,TAMCntuHit::GetTofBranchName());
        }
        
        if(TAGrecoManager::GetPar()->IncludeCA()) {
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
  
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsLocalReco()) {
    if (fFlagMC) {
      fActEvtWriter->SetupElementBranch(fpNtuMcEvt, TAMCntuEvent::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcTrk, TAMCntuPart::GetBranchName());
      
      if (TAGrecoManager::GetPar()->IsRegionMc() )
        fActEvtWriter->SetupElementBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
    }
  }
}
