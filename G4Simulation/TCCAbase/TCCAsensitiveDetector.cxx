/*!
 \file TCCAsensitiveDetector.cxx
 \brief Implementation of TCCAsensitiveDetector.
 */

#include "TCCAsensitiveDetector.hxx"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4VTouchable.hh"

/*!
 \class TCCAsensitiveDetector
 \brief  Sensitive detector of CAL
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
TCCAsensitiveDetector::TCCAsensitiveDetector(G4String name)
:TCGbaseSensitiveDetector(name)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCCAsensitiveDetector::~TCCAsensitiveDetector()
{
}
