
/*!
 \file GlobalToeReco.cxx
 \brief Global reconstruction class using TOE from L0 tree
 */
/*------------------------------------------+---------------------------------*/

#include "BaseReco.hxx"
#include "TAMCntuEvent.hxx"
#include "TAMCntuRegion.hxx"

#include "GlobalToeReco.hxx"

//! Class Imp
ClassImp(GlobalToeReco)

/*!
 \class GlobalToeReco
 \brief Global reconstruction class using TOE from L0 tree
 */
/*------------------------------------------+---------------------------------*/

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
//! \param[in] isMC MC flag
//! \param[in] fileNameMcIn data MC input file name
//! \param[in] treeNameMc MC tree name
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
//! default destructor
GlobalToeReco::~GlobalToeReco()
{
}

//__________________________________________________________
//! Open inout file
void GlobalToeReco::OpenFileIn()
{
  fActEvtReader->Open(GetName());
}

//__________________________________________________________
//! Close inout file
void GlobalToeReco::CloseFileIn()
{
  fActEvtReader->Close();
}

//__________________________________________________________
//! Set L0 reco tree branches
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
//! Set output tree branches
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
