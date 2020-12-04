
/*!
 \class GlobalToeReco
 \brief Global reconstruction from local reconstruction Tree **
 author: Ch. Finck
 */

#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "TAGactNtuGlbTrack.hxx"

#include "GlobalToeReco.hxx"


ClassImp(GlobalToeReco)

//__________________________________________________________
GlobalToeReco::GlobalToeReco(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC)
 : BaseReco(expName, runNumber, fileNameIn, fileNameout)
{
   TAGactNtuGlbTrack::EnableStdAlone();
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
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) {

      fActEvtReader->Open(GetName());
   }
}

//__________________________________________________________
void GlobalToeReco::CloseFileIn()
{
  fActEvtReader->Close();
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

//__________________________________________________________
void GlobalToeReco::CreateRawAction()
{

}

//__________________________________________________________
void GlobalToeReco::CreateRecAction()
{

  fActEvtReader = new TAGactTreeReader("evtReader");
  BaseReco::CreateRecAction();

  if (GlobalPar::GetPar()->IncludeVT()) {
    fActEvtReader->SetupBranch(fpNtuTrackVtx,  TAVTntuTrack::GetBranchName());
    fActEvtReader->SetupBranch(fpNtuClusVtx,   TAVTntuCluster::GetBranchName());
    fActEvtReader->SetupBranch(fpNtuVtx, TAVTntuVertex::GetBranchName());
  }
  
  if (GlobalPar::GetPar()->IncludeIT())
    fActEvtReader->SetupBranch(fpNtuClusIt,  TAITntuCluster::GetBranchName());
  
  if (GlobalPar::GetPar()->IncludeMSD())
    fActEvtReader->SetupBranch(fpNtuClusMsd,  TAMSDntuCluster::GetBranchName());
  
  if(GlobalPar::GetPar()->IncludeTW())
    fActEvtReader->SetupBranch(fpNtuRecTw,  TATWntuPoint::GetBranchName());

  if (fFlagMC)
    fActEvtReader->SetupBranch(fpNtuMcEve,TAMCntuEve::GetBranchName());

}
