
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
GlobalToeReco::GlobalToeReco(TString expName, TString fileNameIn, TString fileNameout, Bool_t isMC)
 : BaseReco(expName, fileNameIn, fileNameout)
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

      
      if (FootDebugLevel(2)) {
         TAGntuGlbTrack *glbTrack = (TAGntuGlbTrack*) (fTAGroot->FindDataDsc("glbTrack", "TAGntuGlbTrack")->Object());
         int nTrk = glbTrack->GetTracksN();
         for(int iTr = 0; iTr< nTrk; iTr++) {
            TAGtrack *aTr = glbTrack->GetTrack(iTr);
            cout<<"  "<<aTr->GetMass()<<" "<<aTr->GetEnergy()<<endl;
         }
      }
   }
}

//__________________________________________________________
void GlobalToeReco::OpenFileIn()
{
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fActGlbTrack->Open(GetName());
      fTree = fActGlbTrack->GetTree();
   }
}

//__________________________________________________________
void GlobalToeReco::CloseFileIn()
{
   if (GlobalPar::GetPar()->IncludeTOE() && TAGactNtuGlbTrack::GetStdAloneFlag()) {
      fActGlbTrack->Close();
   }
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
  //Disabled: you already have everything you need inside the root file
   if (fFlagMC)
      fpNtuMcEve = new TAGdataDsc("eveMc", new TAMCntuEve());
  
}

//__________________________________________________________
void GlobalToeReco::AddRawRequiredItem()
{
}
