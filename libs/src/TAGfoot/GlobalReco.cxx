

#include "GlobalReco.hxx"

ClassImp(GlobalReco)

//__________________________________________________________
GlobalReco::GlobalReco(TString expName, TString fileNameIn, TString fileNameout)
 : LocalReco(expName, -1, fileNameIn, fileNameout)
{
}

//__________________________________________________________
GlobalReco::~GlobalReco()
{
}
