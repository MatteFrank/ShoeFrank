/*!
 \file TCFOrunAction.cxx
 \brief Implementation of TCFOrunAction.
*/

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

/*!
 \class TCFOrunAction
 \brief  Run action for FOOT
 */


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
TCFOrunAction::TCFOrunAction()
: G4UserRunAction(),
  fpEventMC(0x0),
  fpOutFile(0x0),
  fpTree(0x0),
  fEventsNToBeProcessed(-1)
{
    G4cout<<"Construct Run Action"<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCFOrunAction::~TCFOrunAction()
{
   G4cout<<"Distructor of my class run Action "<<G4endl;
   if(fpEventMC)  delete fpEventMC;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Begin of run action
//!
//! \param[in] aRun a given run
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
//! End of run action
//!
//! \param[in] aRun a given run
void TCFOrunAction::EndOfRunAction(const G4Run* aRun)
{
    G4cout<<"The total number of events number of events is "<<aRun->GetNumberOfEvent()<<G4endl;

    PrintBranches();
    fpTree->Write();

    //close file
    fpOutFile->Close();
    fWatch.Print();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Fill tree and clear container
//!
//! \param[in] fill filling tree flag
void TCFOrunAction::FillAndClear(Bool_t fill)
{
    if (fill)
       fpTree->Fill();
    ClearContainers();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Set containers
void TCFOrunAction::SetContainers()
{
   //File for
   fpOutFile = new TFile(GetRootFileName(), "RECREATE");
   if (!fpOutFile) return;
   fpOutFile->cd();
   
   Int_t run          = gTAGroot->CurrentRunNumber();
   const Char_t* name = gTAGroot->CurrentCampaignName();
  
   fpTree = new TTree("EventTree", "FOOT");
   fpEventMC = new TAMCevent();
   fpEventMC->SetBranches(fpTree);
   TAGrecoManager::GetPar()->EnableRootObject();
  
   TAGrunInfo info = TAGrecoManager::GetPar()->GetGlobalInfo();
   info.SetCampaignName(name);
   info.SetRunNumber(run);
   gTAGroot->SetRunInfo(info);
   info.Write(TAGrunInfo::GetObjectName());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Clear containers
void TCFOrunAction::ClearContainers()
{
   if(fpEventMC)  fpEventMC->Clean();
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Print tree branches
void TCFOrunAction::PrintBranches()
{
  if (!fpTree) return;
  
  Double_t d_tree_usize = fpTree->GetTotBytes();
  Double_t d_tree_csize = fpTree->GetZipBytes();
  Double_t d_comp1 = 1.;
  
  if (d_tree_csize > 0.) d_comp1 = d_tree_usize / d_tree_csize;
  
  cout  << "branch name               index/type  "
  <<" bt bid  tot size file size   comp   frac"
  << endl;
  
  cout  << "tree:" << Form("%-19s", fpTree->GetName()) << "  "
  << "-- tree ----"
  << "       "
  << Form(" %8.1fk", d_tree_usize/1000)
  << Form(" %8.1fk", d_tree_csize/1000)
  << Form(" %6.2f",  d_comp1)
  << endl;
  
  TIter it_top(fpTree->GetListOfBranches());
  TBranch* p_top;
  while ((p_top= (TBranch*)it_top())) {
    Int_t    i_btyp  = TAGactTreeWriter::get_be_type(p_top);
    Int_t    i_bid   = TAGactTreeWriter::get_be_id(p_top);
    Int_t    i_nsub  = 0;
    Double_t d_usize = 0;
    Double_t d_csize = 0;
    Double_t d_comp  = 1.;
    Double_t d_frac  = 0.;
    
    if (i_bid == -2) {
      TAGactTreeWriter::sum_branch(i_nsub, d_usize, d_csize, p_top);
      if (d_csize > 0.) d_comp = d_usize/d_csize;
    }
    else {
      d_usize = p_top->GetTotalSize();
      d_csize = p_top->GetZipBytes();
      if (d_csize > 0.) d_comp = p_top->GetTotBytes() / d_csize;
    }
    
    if (d_tree_csize > 0.) d_frac = d_csize / d_tree_csize;
    
    cout  << Form("%-24s", p_top->GetName()) << "  ";
    cout  << "-- branch --";
    cout  << Form(" %2d %3d", i_btyp, i_bid);
    cout  << Form(" %8.1fk %8.1fk", d_usize/1000, d_csize/1000)
    << Form(" %6.2f %5.1f%%", d_comp, 100*d_frac) << endl;
    
  }
}

