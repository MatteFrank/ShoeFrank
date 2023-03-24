
/*!
 \file GlobalToeReco.cxx
 \brief Global reconstruction class using TOE from L0 tree
 */

#include "BaseReco.hxx"
#include "TAMCntuEvent.hxx"
#include "TAMCntuRegion.hxx"
#include "TAMCntuPart.hxx"

#include "GlobalToeReco.hxx"

/*!
 \class GlobalToeReco
 \brief Global reconstruction class using TOE from L0 tree
 */

//! Class Imp
ClassImp(GlobalToeReco)

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
   TAGrecoManager::GetPar()->EnableFromLocalReco();
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
 // fActEvtReader->Close();
}

//__________________________________________________________
//! Set L0 reco tree branches
void GlobalToeReco::SetL0TreeBranches()
{
  BaseReco::SetL0TreeBranches();
  
  if ((TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman()) && TAGrecoManager::GetPar()->IsFromLocalReco()) {
    if (fFlagMC) {
      fpNtuMcTrk = new TAGdataDsc(new TAMCntuPart());
      fActEvtReader->SetupBranch(fpNtuMcTrk);
      
      fpNtuMcEvt = new TAGdataDsc(new TAMCntuEvent());
      fActEvtReader->SetupBranch(fpNtuMcEvt);
      
      if (TAGrecoManager::GetPar()->IsRegionMc()) {
        fpNtuMcReg = new TAGdataDsc(new TAMCntuRegion());
        fActEvtReader->SetupBranch(fpNtuMcReg);
      }
    }
  }
}
