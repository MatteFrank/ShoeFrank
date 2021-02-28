
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

// --------------------------------------------------------------------------------------
void GlobalToeReco::SetRunNumber()
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
