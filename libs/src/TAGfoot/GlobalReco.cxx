

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
