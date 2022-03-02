/*!
\file TCITsensitiveDetector.cxx
 \brief Implementation of TCITsensitiveDetector.
*/

#include "TCITsensitiveDetector.hxx"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4VTouchable.hh"

/*!
 \class TCITsensitiveDetector
 \brief ITR interface for base sensitive detector class
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] name sensitive detector name
TCITsensitiveDetector::TCITsensitiveDetector(G4String name)
:TCGbaseSensitiveDetector(name)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCITsensitiveDetector::~TCITsensitiveDetector()
{
}

