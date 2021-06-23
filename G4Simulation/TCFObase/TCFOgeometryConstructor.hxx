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
   G4VPhysicalVolume* Construct();
   
   G4Material*  GetWorldMaterial()         { return fWorldMaterial;   }
   TASTparGeo*  GetParGeoIr()              { return fpParGeoSt;       }
   TABMparGeo*  GetParGeoBm()              { return fpParGeoBm;       }
   TAVTparGeo*  GetParGeoVtx()             { return fpParGeoVtx;      }
   TAITparGeo*  GetParGeoIt()              { return fpParGeoIt;       }
   TADIparGeo*  GetParGeoEm()              { return fpParGeoEm;       }
   TAMSDparGeo* GetParGeoMsd()             { return fpParGeoMsd;      }
   TACAparGeo*  GetParGeoCa()              { return fpParGeoCa;       }
   TATWparGeo*  GetParGeoTw()              { return fpParGeoTw;       }
   
   void ConstructSDandField();

private:
   TCSTgeometryConstructor*  fStartCounter;
   TCBMgeometryConstructor*  fBeamMonitor;
   TCVTgeometryConstructor*  fVertex;
   TCITgeometryConstructor*  fInnerTracker;
   TCMSDgeometryConstructor* fMicroStrip;
   TCCAgeometryConstructor*  fCalorimeter;
   TCTWgeometryConstructor*  fTofWall;
   TCEMgeometryConstructor*  fMagnet;
   
   TCEMfield*                fField;
   TADIgeoField*             fFieldImpl;
   TCEMfieldSetup*           fFieldSetup;

   TASTparGeo*               fpParGeoSt;
   TABMparGeo*               fpParGeoBm;
   TAVTparGeo*               fpParGeoVtx;
   TAITparGeo*               fpParGeoIt;
   TADIparGeo*               fpParGeoEm;
   TAMSDparGeo*              fpParGeoMsd;
   TACAparGeo*               fpParGeoCa;
   TATWparGeo*               fpParGeoTw;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

