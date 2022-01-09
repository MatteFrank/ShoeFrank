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

#ifndef TCFOgeometryConstructor_h
#define TCFOgeometryConstructor_h 1

/*!
 \file TCFOgeometryConstructor.hxx
 \brief  Geometry construction class of FOOT
 
 \author Ch. Finck
 */

#include "TCGbaseConstructor.hxx"
#include "TCGbaseGeometryConstructor.hxx"
#include "globals.hh"
#include "G4String.hh"

#include "TASTparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TADIparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TACAparGeo.hxx"
#include "TATWparGeo.hxx"


class G4Material;
class TCEMfield;
class TCSTgeometryConstructor;
class TCBMgeometryConstructor;
class TCVTgeometryConstructor;
class TCITgeometryConstructor;
class TCEMgeometryConstructor;
class TCMSDgeometryConstructor;
class TCTWgeometryConstructor;
class TCCAgeometryConstructor;
class TCFOfield;
class TADIgeoField;
class TCEMfieldSetup;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TCFOgeometryConstructor : public TCGbaseGeometryConstructor
{
public:
   TCFOgeometryConstructor(const TString expName = "", Int_t runNumber = -1);
   ~TCFOgeometryConstructor();
   
public:
   //! Construct geometry
   G4VPhysicalVolume* Construct();
   
   //! Get world material
   G4Material*  GetWorldMaterial()         { return fWorldMaterial;   }
   //! Get geometry parameter for STC
   TASTparGeo*  GetParGeoIr()              { return fpParGeoSt;       }
   //! Get geometry parameter for BM
   TABMparGeo*  GetParGeoBm()              { return fpParGeoBm;       }
   //! Get geometry parameter for VTX
   TAVTparGeo*  GetParGeoVtx()             { return fpParGeoVtx;      }
   //! Get geometry parameter for ITR
   TAITparGeo*  GetParGeoIt()              { return fpParGeoIt;       }
   //! Get geometry parameter for dipole
   TADIparGeo*  GetParGeoEm()              { return fpParGeoEm;       }
   //! Get geometry parameter for MSD
   TAMSDparGeo* GetParGeoMsd()             { return fpParGeoMsd;      }
   //! Get geometry parameter for CAL
   TACAparGeo*  GetParGeoCa()              { return fpParGeoCa;       }
   //! Get geometry parameter for TW
   TATWparGeo*  GetParGeoTw()              { return fpParGeoTw;       }
   
   //! Construct field
   void ConstructSDandField();

private:
   TCSTgeometryConstructor*  fStartCounter;  ///< STC geometry constructor
   TCBMgeometryConstructor*  fBeamMonitor;   ///< BM geometry constructor
   TCVTgeometryConstructor*  fVertex;        ///< VTX geometry constructor
   TCITgeometryConstructor*  fInnerTracker;  ///< ITR geometry constructor
   TCMSDgeometryConstructor* fMicroStrip;    ///< MSD geometry constructor
   TCCAgeometryConstructor*  fCalorimeter;   ///< CAL geometry constructor
   TCTWgeometryConstructor*  fTofWall;       ///< TW geometry constructor
   TCEMgeometryConstructor*  fMagnet;        ///< Dipole geometry constructor
   
   TCEMfield*                fField;         ///< Field map
   TADIgeoField*             fFieldImpl;     ///< field map implementation
   TCEMfieldSetup*           fFieldSetup;    ///< Field map setup

   TASTparGeo*               fpParGeoSt;     ///< STC geometry parameters
   TABMparGeo*               fpParGeoBm;     ///< BM geometry parameters
   TAVTparGeo*               fpParGeoVtx;    ///< VTX geometry parameters
   TAITparGeo*               fpParGeoIt;     ///< ITR geometry parameters
   TADIparGeo*               fpParGeoEm;     ///< Dipole geometry parameters
   TAMSDparGeo*              fpParGeoMsd;    ///< MSD geometry parameters
   TACAparGeo*               fpParGeoCa;     ///< CAL geometry parameters
   TATWparGeo*               fpParGeoTw;     ///< TW geometry parameters

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

