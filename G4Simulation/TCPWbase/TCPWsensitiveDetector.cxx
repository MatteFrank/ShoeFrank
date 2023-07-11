/*!
 \file TCPWsensitiveDetector.cxx
 \brief Implementation of TCPWsensitiveDetector.
 */

#include "TCPWsensitiveDetector.hxx"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4VTouchable.hh"

/*!
 \class TCPWsensitiveDetector
 \brief TW interface for base sensitive detector class
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] name sensitive detector name
TCPWsensitiveDetector::TCPWsensitiveDetector(G4String name)
:TCGbaseSensitiveDetector(name)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCPWsensitiveDetector::~TCPWsensitiveDetector()
{
}
