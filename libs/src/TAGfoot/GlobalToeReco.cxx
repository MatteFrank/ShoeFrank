
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
void GlobalToeReco::LoopEvent(Int_t nEvents)
{
   if (nEvents == 0)
      nEvents = fTree->GetEntries();
   
   if (nEvents > fTree->GetEntries())
      nEvents = fTree->GetEntries();
   
   for (Long64_t ientry = 0; ientry < nEvents; ientry++) {
      
      fTree->GetEntry(ientry);
      
      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;

      if (!fTAGroot->NextEvent()) break;

      if (FootDebugLevel(1)) {
      //MC block
	TAMCntuEve*  p_ntuMcEve
	  =  static_cast<TAMCntuEve*>( gTAGroot->FindDataDsc( "eveMc" ) ->Object() );
	int nTrkMC = p_ntuMcEve->GetTracksN();
	for(int iTr = 0; iTr< nTrkMC; iTr++) {
	  TAMCeveTrack *aTr = p_ntuMcEve->GetTrack(iTr);
	  //	cout<<"MCblock:  "<<aTr->GetMass()<<" "<<aTr->GetCharge()<<endl;
	}
	
	
	TAGntuGlbTrack *glbTrack =
	  (TAGntuGlbTrack*) fpNtuGlbTrack->GenerateObject();
	// (fTAGroot->FindDataDsc("glbTrack", "TAGntuGlbTrack")->Object());
	int nTrk = glbTrack->GetTracksN();
	for(int iTr = 0; iTr< nTrk; iTr++) {
	  TAGtrack *aTr = glbTrack->GetTrack(iTr);
	  cout<<"  "<<aTr->GetMass()<<" "<<aTr->GetEnergy()<<" "<<aTr->GetMomentum()<<endl;
	}
      }
   }
}

//__________________________________________________________
void GlobalToeReco::OpenFileIn()
{
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) {

      fActEvtReaderFile->Open(GetName());

      fTree = fActEvtReaderFile->GetTree();


   }
}

//__________________________________________________________
void GlobalToeReco::CloseFileIn()
{
  fActEvtReaderFile->Close();
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

  fActEvtReaderFile = new TAGactTreeReader("evtReader");
  BaseReco::CreateRecAction();

  if (GlobalPar::GetPar()->IncludeVT()) {
    fActEvtReaderFile->SetupBranch(fpNtuTrackVtx,  TAVTntuTrack::GetBranchName());
    fActEvtReaderFile->SetupBranch(fpNtuClusVtx,   TAVTntuCluster::GetBranchName());
    fActEvtReaderFile->SetupBranch(fpNtuVtx, TAVTntuVertex::GetBranchName());
  }
  
  if (GlobalPar::GetPar()->IncludeIT())
    fActEvtReaderFile->SetupBranch(fpNtuClusIt,  TAITntuCluster::GetBranchName());
  
  if (GlobalPar::GetPar()->IncludeMSD())
    fActEvtReaderFile->SetupBranch(fpNtuClusMsd,  TAMSDntuCluster::GetBranchName());
  
  if(GlobalPar::GetPar()->IncludeTW())
    fActEvtReaderFile->SetupBranch(fpNtuRecTw,  TATWntuPoint::GetBranchName());

  fActEvtReaderFile->SetupBranch(fpNtuMcEve,TAMCntuEve::GetBranchName());

}
