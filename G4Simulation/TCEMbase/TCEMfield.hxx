//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//

/*!
 \file TCEMfield.hxx
 \brief Definition of the TCEMfield class
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef TCEMfield_h
#define TCEMfield_h 1

#include "globals.hh"
#include "G4MagneticField.hh"

#include "TADIgeoField.hxx"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TCEMfield : public G4MagneticField
{

private:
   TADIgeoField* fMagField; ///< Dipole field pointer
   
public:

  TCEMfield(TADIgeoField* fMagField);
  virtual ~TCEMfield();

  //! DoesFieldChangeEnergy() returns true.
  virtual G4bool DoesFieldChangeEnergy() const { return false; };

  // GetFieldValue() returns the field value at a given point[].
  // field is really field[6]: Bx,By,Bz.
  // point[] is in global coordinates: x,y,z,t.
   virtual void GetFieldValue(const G4double Point[4], G4double* Bfield) const;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
