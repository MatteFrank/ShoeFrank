//Run Action

#include "TFile.h"

#include "TCFOrunAction.hxx"

#include "TAGgeoTrafo.hxx"
#include "G4Run.hh"

#include "TFile.h"
#include "TTree.h"

#include "TAGactTreeWriter.hxx"

#include "TASTparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TACAparGeo.hxx"

TString TCFOrunAction::fgRootFileName = "ionCa1000.root";

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCFOrunAction::TCFOrunAction()
: G4UserRunAction(),
fkEvento(1),
fpEventoMC(0x0),
fpEventMC(0x0),
fpOutFile(0x0),
fpTree(0x0),
fEventsNToBeProcessed(-1)
{
    G4cout<<"Construct Run Action"<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCFOrunAction::~TCFOrunAction()
{
   G4cout<<"Distructor of my class run Action "<<G4endl;
   if(fpEventoMC) delete fpEventoMC;
   if(fpEventMC)  delete fpEventMC;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOrunAction::BeginOfRunAction(const G4Run* aRun)
{
    fEventsNToBeProcessed = aRun->GetNumberOfEventToBeProcessed();

    G4cout <<" starts run number "<< aRun->GetRunID() << G4endl;
    G4cout <<" number of events to process "<< fEventsNToBeProcessed << G4endl;

    if (gTAGroot == 0x0) {
        G4cerr << "not TAGroot defined" << G4endl;
        return;
    }

    fWatch.Start();
    SetContainers();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOrunAction::EndOfRunAction(const G4Run* aRun)
{
    G4cout<<"The total number of events number of events is "<<aRun->GetNumberOfEvent()<<G4endl;

    fpTree->Write();
  
    // write run info
    TAGrunInfo info = GlobalPar::Instance()->GetGlobalInfo();
    info.SetCampaignName(gTAGroot->CurrentCampaignName());
    info.SetRunNumber(gTAGroot->CurrentRunNumber());
    info.Write(TAGrunInfo::GetObjectName());
  
    //close file
    fpOutFile->Close();
    fWatch.Print();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOrunAction::FillAndClear(Bool_t fill)
{
    if (fill)
       fpTree->Fill();
    ClearContainers();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOrunAction::SetContainers()
{
   //File for
   fpOutFile = new TFile(GetRootFileName(), "RECREATE");
   if (!fpOutFile) return;
   fpOutFile->cd();
   
   Int_t run          = gTAGroot->CurrentRunNumber();
   const Char_t* name = gTAGroot->CurrentCampaignName();
   
   TAGrunInfo info = GlobalPar::GetPar()->GetGlobalInfo();
   info.SetCampaignName(name);
   info.SetRunNumber(run);
   gTAGroot->SetRunInfo(info);
   info.Write(TAGrunInfo::GetObjectName());
   
   fpTree      = new TTree("EventTree", "FOOT");
    if(fkEvento){
        fpEventoMC = new Evento();
        fpEventoMC->SetBranches(fpTree);
    }
    else{
        fpEventMC = new TAMCevent();
        fpEventMC->SetBranches(fpTree);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOrunAction::ClearContainers()
{
   if(fpEventoMC) fpEventoMC->Clean();
   if(fpEventMC)  fpEventMC->Clean();
}


