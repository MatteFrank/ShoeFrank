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

#ifndef TCGbaseGeometryConstructor_h
#define TCGbaseGeometryConstructor_h 1

/*!
 \file TCGbaseGeometryConstructor.hxx
 \brief  Geometry construction class for target/beam
 
 \author Ch. Finck
 */

#include "TAGcampaignManager.hxx"

#include "TCGbaseConstructor.hxx"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4String.hh"
#include "TAVTparGeo.hxx"
#include "TAGparGeo.hxx"

class G4LogicalVolume;
class G4Material;
class TCGtargetConstructor;
class TCGmaterials;
class TAGgeometryMessenger;
class TAVTgeometryConstructor;
class TAGgeoTrafo;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TCGbaseGeometryConstructor : public G4VUserDetectorConstruction
{
public:
   TCGbaseGeometryConstructor(const TString expName = "", Int_t runNumber = -1);
   virtual ~TCGbaseGeometryConstructor();
   
public:
   // Set size in Z for world
   void SetWorldSizeZ(G4double sizeZ);
   // Set size in X-Y for world
   void SetWorldSizeXY(G4double sizeXY);
   // Set world material
	void SetWordMaterial(G4String mat);
   
   // Construct
   virtual G4VPhysicalVolume* Construct();
   
   //! Get size in Z for world
   G4double     GetWorldSizeZ()     const  { return fWorldSizeZ;      }
   //! Get size in X-Y for world
   G4double     GetWorldSizeXY()    const  { return fWorldSizeXY;     }
   //! Get world material
   G4Material*  GetWorldMaterial()         { return fWorldMaterial;   }
   
   //! Get geometry parameter for beam/target
   TAGparGeo*   GetParGeoG()               { return fpParGeoG;        }
   //! Get geometry transformations
   TAGgeoTrafo* GetGeoTrafo()              { return fpFootGeo;        }

protected:
   TAGcampaignManager*    fCampManager;   ///< Campaign manager
   TString                fExpName;       ///< Experiment name
   G4int                  fRunNumber;     ///< Run number
   TAGgeoTrafo*           fpFootGeo;      ///<  trafo prointer
   G4double               fWorldSizeZ;    ///< World size in Z
   G4double               fWorldSizeXY;   ///< World size in X-Y
   G4Material*            fWorldMaterial; ///< World material
   G4LogicalVolume*       fLogWorld;      ///< Logical world
   TCGtargetConstructor*  fTarget;        ///< Target constructor
   TAGparGeo*             fpParGeoG;      ///< Target/beam geometry parameters
   TCGmaterials*          fpMaterials;    ///< Materials
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

