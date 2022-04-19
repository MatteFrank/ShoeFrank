
/*!
 \class GlobalToeReco
 \brief Global reconstruction from local reconstruction Tree **
 author: Ch. Finck
 */

/*!
 \file GlobalToeReco.cxx
 \brief Global reconstruction class using TOE from L0 tree
 */
/*------------------------------------------+---------------------------------*/

#include "BaseReco.hxx"
#include "TAMCntuEvent.hxx"
#include "TAMCntuRegion.hxx"

#include "GlobalToeReco.hxx"

/*!
 \class GlobalToeReco.
 \brief Global reconstruction class using TOE from L0 tree
 */
/*------------------------------------------+---------------------------------*/

ClassImp(GlobalToeReco)

//__________________________________________________________
GlobalToeReco::GlobalToeReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC, TString fileNameMcIn, TString treeNameMc)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout)
{
   TAGrecoManager::GetPar()->EnableLocalReco();
   fFlagMC = isMC;
   
   if (!fileNameMcIn.IsNull() && isMC) {
      DisableSaveMc();
      fFriendFileName = fileNameMcIn;
      fFriendTreeName = treeNameMc;
   }
}

//__________________________________________________________
GlobalToeReco::~GlobalToeReco()
{
   // default destructor
}

//__________________________________________________________
void GlobalToeReco::OpenFileIn()
{
  fActEvtReader->Open(GetName());
}

//__________________________________________________________
void GlobalToeReco::CloseFileIn()
{
 // fActEvtReader->Close();
}

//__________________________________________________________
void GlobalToeReco::CreateRawAction()
{
}

//__________________________________________________________
void GlobalToeReco::SetL0TreeBranches()
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
    }
  }
}

//__________________________________________________________
void GlobalToeReco::SetTreeBranches()
{
  BaseReco::SetTreeBranches();
  
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsLocalReco()) {
    if (fFlagMC && fSaveMcFlag) {
      fActEvtWriter->SetupElementBranch(fpNtuMcEvt, TAMCntuEvent::GetBranchName());
      fActEvtWriter->SetupElementBranch(fpNtuMcTrk, TAMCntuPart::GetBranchName());
      
      if (TAGrecoManager::GetPar()->IsRegionMc() )
        fActEvtWriter->SetupElementBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
    }
  }
}
