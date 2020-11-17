

#include "GlobalReco.hxx"

ClassImp(GlobalReco)

//__________________________________________________________
GlobalReco::GlobalReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : LocalReco(expName, runNumber, fileNameIn, fileNameout)
{
}

//__________________________________________________________
GlobalReco::~GlobalReco()
{
}
