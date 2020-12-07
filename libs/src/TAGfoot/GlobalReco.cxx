

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

// --------------------------------------------------------------------------------------
void GlobalReco::SetRunNumber()
{
  if (fRunNumber != -1) { // if set from outside return, else take from name
    gTAGroot->SetRunNumber(fRunNumber);
    return;
  }
  
  // Done by hand shoud be given by DAQ header
  TString name = GetName();
  if (name.IsNull()) return;
  
  // protection about file name starting with .
  if (name[0] == '.')
    name.Remove(0,1);
  
  if (fFlagMC) {
    // assuming name XXX_run.root
    Int_t pos1   = name.Last('_');
    Int_t len    = name.Length();
    
    TString tmp1 = name(pos1+1, len);
    Int_t pos2   = tmp1.First(".");
    TString tmp  = tmp1(0, pos2);
    fRunNumber = tmp.Atoi();
    
  } else {
    // assuming XXX.run.XXX.dat
    Int_t pos1   = name.First(".");
    Int_t len    = name.Length();
    
    TString tmp1 = name(pos1+1, len);
    Int_t pos2   = tmp1.First(".");
    TString tmp  = tmp1(0, pos2);
    fRunNumber = tmp.Atoi();
  }
  
  Warning("SetRunNumber()", "Run number not set !, taking number from file: %d", fRunNumber);
  
  gTAGroot->SetRunNumber(fRunNumber);
}
