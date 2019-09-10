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
// $Id: TCFOfield.cc 75672 2013-11-05 08:47:41Z gcosmo $
//
/// \file field/field05/src/TCFOfield.cc
/// \brief Implementation of the TCFOfield class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "TCFOfield.hxx"
#include "globals.hh"

#include "TVirtualMagField.h"
#include "G4SystemOfUnits.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TCFOfield::TCFOfield(FootField* magField)
: TCEMfield(),
  fMagField(magField),
  fDebugLevel(0)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TCFOfield::~TCFOfield()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TCFOfield::GetFieldValue(const G4double point[4], G4double* fieldB) const
{
   // point[4] := time
   TVector3 pos;
   for (Int_t i = 0; i < 3; ++i) {
      pos[i] = point[i]/10.; // mmn -> cm
   }
   
   TVector3 field = fMagField->get(pos);
   
   for (Int_t i = 0; i < 3; ++i) {
//      field[i] *= 1e-7; // gauss -> to ??
       field[i] *= 1e-4; // gauss -> to Tesla
   }
   
   fieldB[0] = field[0];
   fieldB[1] = field[1];
   fieldB[2] = field[2];
   
   if (fDebugLevel)
      printf("%f %f\n", pos[2], fieldB[0]);

  return;
}
