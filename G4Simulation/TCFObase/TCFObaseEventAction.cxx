
#include "TCFObaseEventAction.hxx"

/*! \file TCFObaseEventAction.cxx
 \brief Implementation of TCFObaseEventAction.
*/


#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "G4ThreeVector.hh"

#include "TCFOgeometryConstructor.hxx"

#include "TCSTgeometryConstructor.hxx"
#include "TCBMgeometryConstructor.hxx"
#include "TCVTgeometryConstructor.hxx"
#include "TCITgeometryConstructor.hxx"
#include "TCMSDgeometryConstructor.hxx"
#include "TCTWgeometryConstructor.hxx"
#include "TCCAgeometryConstructor.hxx"

#include "TCGmcHit.hxx"
#include "TAMCntuPart.hxx"

#include "TCFOrunAction.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TClonesArray.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TFile.h"

/*! \class TCFObaseEventAction
 \brief Base event action class for FOOT
 
 include hit class
 Endo of event:
 collection hits: all hits of this collections contains the information about what happened in the sensitive volume (epitaxial layer)
 each hit represents a step and in the epi layer there may have been many
 than the position to send to CmosDigitizer is the PosIn of the first hits and the pos out of the last hits
 the energy deposited is the sum of the energy deposited in each step
 All positon in microns and all energy in eV
 */

/*! \class TAGeventInterruptHandler
 \brief Class to handle event interruption

 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
TAGeventInterruptHandler::TAGeventInterruptHandler()
: TSignalHandler(kSigInterrupt, kFALSE)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
Bool_t TAGeventInterruptHandler::Notify()
{
    G4RunManager::GetRunManager()->AbortRun(false);
    return kTRUE;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] runAction run action
//! \param[in] footGeomConstructor FOOT geometry constructor
TCFObaseEventAction::TCFObaseEventAction(TCFOrunAction* runAction, TCGbaseGeometryConstructor* footGeomConstructor)
: G4UserEventAction(),
  fEventNumber(-1),
  fStCollId(-1),
  fBmCollId(-1),
  fVtxCollId(-1),
  fItCollId(-1),
  fMsdCollId(-1),
  fTwCollId(-1),
  fCaCollId(-1),
  fDetName(""),
  fFillTree(true),
  fInelasticOnly(false)
{
    fEventInterruptHandler = new TAGeventInterruptHandler();
    fEventInterruptHandler->Add();

    fMcEvent = new TAMCntuPart();

   if (FootMcDebugLevel(1))
      G4cout<<"Construct event action "<<G4endl;

   fFootGeomConstructor = (TCFOgeometryConstructor*)footGeomConstructor;
   fRunAction           = (TCFOrunAction*)runAction;
   fpGeoTrafo           = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCFObaseEventAction::~TCFObaseEventAction()
{
    if (FootMcDebugLevel(1))
      G4cout<<"Distructor Event Action "<<G4endl;

    delete fEventInterruptHandler;
    delete fMcEvent;

    if (FootMcDebugLevel(1))
       G4cout<<"Out Destructor Event Action "<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Begin event action
//!
//! \param[in] evt a given event
void TCFObaseEventAction::BeginOfEventAction(const G4Event* evt)
{
   if (fInelasticOnly)
      SetFillTree(false);

    fEventNumber = evt->GetEventID()+1;
   if (FootMcDebugLevel(1))
      G4cout<<"********************************************************************Begin event actions "<<evt->GetEventID()<<G4endl;

    static Int_t frequency = 1;
    static Int_t max       = 0;
    static Bool_t hasDigit = false;

    if(max == 0) {
        max = GetEventsNToBeProcessed();

        if( max > 100000)      frequency = 10000;
        else if( max >= 10000) frequency = 1000;
        else if( max >= 1000)  frequency = 100;
        else if( max >= 100)   frequency = 10;
    }

    if(fEventNumber % frequency == 0)
    G4cout<<fEventNumber<<G4endl;

    if (!hasDigit) {
        ConstructCollection();
        hasDigit = true;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! End event action
//!
//! \param[in] evt a given event (not used)
void TCFObaseEventAction::EndOfEventAction(const G4Event* /*evt*/)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Construct hit collection
void TCFObaseEventAction::ConstructCollection()
{
   G4SDManager * SDman = G4SDManager::GetSDMpointer();

   if (TAGrecoManager::GetPar()->IncludeST()) {
      fStCollId = SDman->GetCollectionID(TCSTgeometryConstructor::GetSDname());
   }
   
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fBmCollId = SDman->GetCollectionID(TCBMgeometryConstructor::GetSDname());
   }
   
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fVtxCollId = SDman->GetCollectionID(TCVTgeometryConstructor::GetSDname());
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fItCollId = SDman->GetCollectionID(TCITgeometryConstructor::GetSDname());
   }
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fMsdCollId = SDman->GetCollectionID(TCMSDgeometryConstructor::GetSDname());
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fTwCollId = SDman->GetCollectionID(TCTWgeometryConstructor::GetSDname());
   }
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fCaCollId = SDman->GetCollectionID(TCCAgeometryConstructor::GetSDname());
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Get event to be processed
Int_t TCFObaseEventAction::GetEventsNToBeProcessed()
{
   return fRunAction->GetEventsNToBeProcessed();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Fill tree and clear containers
void TCFObaseEventAction::FillAndClear()
{
   fMcEvent->Clear();
   return fRunAction->FillAndClear(fFillTree);
}


