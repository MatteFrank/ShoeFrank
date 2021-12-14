/*!
 \file TCVTsensitiveDetector.cxx
 \brief Implementation of TCVTsensitiveDetector.
*/

#include "TCVTsensitiveDetector.hxx"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4VTouchable.hh"

/*!
 \class TCVTsensitiveDetector
 \brief VTX interface for base sensitive detector class
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
TCVTsensitiveDetector::TCVTsensitiveDetector(G4String name)
:TCGbaseSensitiveDetector(name)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCVTsensitiveDetector::~TCVTsensitiveDetector()
{
}
