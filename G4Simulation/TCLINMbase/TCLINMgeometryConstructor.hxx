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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef TCLINMgeometryConstructor_h
#define TCLINMgeometryConstructor_h 1

/*!
 \file TCLINMgeometryConstructor.hxx
 \brief  Geometry construction class of FOOT
 
 \author Ch. Finck
 */

#include "TCGbaseConstructor.hxx"
#include "TCGbaseGeometryConstructor.hxx"
#include "globals.hh"
#include "G4String.hh"

#include "TAPLparGeo.hxx"
#include "TACEparGeo.hxx"
#include "TAPWparGeo.hxx"


class G4Material;
class TCEMfield;
class TCPLgeometryConstructor;
class TCCEgeometryConstructor;
class TCPWgeometryConstructor;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TCLINMgeometryConstructor : public TCGbaseGeometryConstructor
{
public:
   TCLINMgeometryConstructor(const TString expName = "", Int_t runNumber = -1);
   ~TCLINMgeometryConstructor();
   
public:
   // Construct geometry
   G4VPhysicalVolume* Construct();
   
   //! Get world material
   G4Material*  GetWorldMaterial()         { return fWorldMaterial;   }
   //! Get geometry parameter for STC
   TAPLparGeo*  GetParGeoPl()              { return fpParGeoPl;       }
   //! Get geometry parameter for TW
   TACEparGeo*  GetParGeoCe()              { return fpParGeoCe;       }
   //! Get geometry parameter for CAL
   TAPWparGeo*  GetParGeoPw()              { return fpParGeoPw;       }
   
private:
   TCPLgeometryConstructor*  fPlastic;    ///< Plastic geometry constructor
   TCCEgeometryConstructor*  fCeBr3;      ///< CeBr3 geometry constructor
   TCPWgeometryConstructor*  fPhoswich;   ///< PW geometry constructor
   
   TAPLparGeo*               fpParGeoPl;  ///< Pl geometry parameters
   TACEparGeo*               fpParGeoCe;  ///< Ce geometry parameters
   TAPWparGeo*               fpParGeoPw;  ///< PW geometry parameters

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

