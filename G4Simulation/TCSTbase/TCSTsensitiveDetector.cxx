/*!
 \file TCSTsensitiveDetector.cxx
 \brief Implementation of TCSTsensitiveDetector.
*/

#include "TCSTsensitiveDetector.hxx"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4VTouchable.hh"

/*!
 \class TCSTsensitiveDetector
 \brief STC interface for base sensitive detector class
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
TCSTsensitiveDetector::TCSTsensitiveDetector(G4String name)
:TCGbaseSensitiveDetector(name)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCSTsensitiveDetector::~TCSTsensitiveDetector()
{
}
