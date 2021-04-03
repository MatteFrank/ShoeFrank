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
// $Id: TCEMfield.cc 75672 2013-11-05 08:47:41Z gcosmo $
//
/// \file field/field05/src/TCEMfield.cc
/// \brief Implementation of the TCEMfield class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "GlobalPar.hxx"

#include "TCEMfield.hxx"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TCEMfield::TCEMfield(TADIgeoField* magField)
 : G4MagneticField(),
   fMagField(magField)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TCEMfield::~TCEMfield()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TCEMfield::GetFieldValue(const G4double point[4], G4double* fieldB) const
{
   // point[4] := time
   fieldB[3] = fieldB[4] = fieldB[5] = 0.;
   
   TVector3 pos;
   for (Int_t i = 0; i < 3; ++i) {
      pos[i] = point[i]/CLHEP::cm; // mmn -> cm
   }
   
   TVector3 field = fMagField->GetField(pos)*CLHEP::gauss;
  
   fieldB[0] = field[0];
   fieldB[1] = field[1];
   fieldB[2] = field[2];
   
//   if (FootMcDebugLevel(1))
//      printf("[%.3e,%.3e,%.3e] \t %.3e %.3e %.3e\n", pos[0],pos[1],pos[2], field[0],field[1],field[2]);
   
   return;
}
