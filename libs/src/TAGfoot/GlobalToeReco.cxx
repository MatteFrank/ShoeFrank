
/*!
 \class GlobalToeReco
 \brief Global reconstruction from local reconstruction Tree **
 author: Ch. Finck
 */

#include "BaseReco.hxx"
#include "TAGactNtuGlbTrack.hxx"

#include "GlobalToeReco.hxx"


ClassImp(GlobalToeReco)

//__________________________________________________________
GlobalToeReco::GlobalToeReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout)
{
   GlobalPar::GetPar()->EnableLocalReco();
   fFlagMC = isMC;
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
  fActEvtReader->Close();
}

//__________________________________________________________
void GlobalToeReco::CreateRawAction()
{
}

//__________________________________________________________
void GlobalToeReco::SetL0TreeBranches()
{
  BaseReco::SetL0TreeBranches();
  
  if ((GlobalPar::GetPar()->IncludeTOE() || GlobalPar::GetPar()->IncludeKalman()) && GlobalPar::GetPar()->IsLocalReco()) {
    if (fFlagMC) {
      fpNtuMcEve = new TAGdataDsc(TAMCntuTrack::GetDefDataName(), new TAMCntuTrack());
      fActEvtReader->SetupBranch(fpNtuMcEve,TAMCntuTrack::GetBranchName());
    }
  }
}
