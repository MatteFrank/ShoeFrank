

#include "GlobalRecoMC.hxx"

ClassImp(GlobalRecoMC)

//__________________________________________________________
GlobalRecoMC::GlobalRecoMC(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : LocalRecoMC(expName, runNumber, fileNameIn, fileNameout)
{

	EnableTracking();
	GlobalPar::GetPar()->EnableLocalReco();
	fFlagMC = true;

}

//__________________________________________________________
GlobalRecoMC::~GlobalRecoMC()
{
}

























