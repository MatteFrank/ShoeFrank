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

/*! \file TCLINMgeometryConstructor.cxx
 \brief Implementation of TCLINMgeometryConstructor.
 */

#include "TCLINMgeometryConstructor.hxx"

#include "Riostream.h"
#include "TMath.h"
#include "TVirtualMagField.h"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4RunManager.hh"
#include "G4VisAttributes.hh"
#include "G4Region.hh"

#include "TCGtargetConstructor.hxx"
#include "TCPLgeometryConstructor.hxx"
#include "TCCEgeometryConstructor.hxx"
#include "TCPWgeometryConstructor.hxx"

#include "TAGrecoManager.hxx"
#include "TAGnameManager.hxx"
#include "TADIgeoField.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

using namespace CLHEP;

/*! \class TCLINMgeometryConstructor
 \brief  Geometry construction class of FOOT
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
TCLINMgeometryConstructor::TCLINMgeometryConstructor(const TString expName, Int_t runNumber)
: TCGbaseGeometryConstructor(expName, runNumber),
  fPlastic(0x0),
  fCeBr3(0x0),
  fPhoswich(0x0),
  fpParGeoPl(0x0),
  fpParGeoCe(0x0),
  fpParGeoPw(0x0)
{
   // initialise map file for start counter
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fpParGeoPl = new TAPLparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TAPLparGeo::GetBaseName(), fRunNumber);
      fpParGeoPl->FromFile(mapFileName.Data());
      fPlastic = new TCPLgeometryConstructor(fpParGeoPl);
   }
   
   // initialise map file for TOF
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fpParGeoCe = new TACEparGeo();
      TString mapFileName =  fCampManager->GetCurGeoFile(TACEparGeo::GetBaseName(), fRunNumber);
      fpParGeoCe->FromFile(mapFileName.Data());
      fCeBr3 = new TCCEgeometryConstructor(fpParGeoCe);
   }
   
   // initialise map file for calorimteer
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fpParGeoPw = new TAPWparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TAPWparGeo::GetBaseName(), fRunNumber);
      fpParGeoPw->FromFile(mapFileName.Data());
      fPhoswich = new TCPWgeometryConstructor(fpParGeoPw);
   }
 }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCLINMgeometryConstructor::~TCLINMgeometryConstructor()
{
   if (fPlastic)  delete fPlastic;
   if (fCeBr3)    delete fCeBr3;
   if (fPhoswich) delete fPhoswich;
   
   if (fpParGeoPl)    delete fpParGeoPl;
   if (fpParGeoCe)    delete fpParGeoCe;
   if (fpParGeoPw)    delete fpParGeoPw;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Construct FOOT geometry
G4VPhysicalVolume* TCLINMgeometryConstructor::Construct()
{
   G4VPhysicalVolume* pWorld = TCGbaseGeometryConstructor::Construct();
   
   // Start Counter
   if (TAGrecoManager::GetPar()->IncludeST()) {
      
      G4LogicalVolume* log  = fPlastic->Construct();
      TVector3 ang          = fpFootGeo->GetSTAngles()*TMath::DegToRad(); // in radians
      G4RotationMatrix* rot = new G4RotationMatrix;
      rot->rotateX(ang[0]);
      rot->rotateY(ang[1]);
      rot->rotateZ(ang[2]);
      
      TVector3 center2      = fpFootGeo->GetSTCenter()*cm;
      G4ThreeVector center(center2[0], center2[1], center2[2]);

      //To rotate the box wrt hall origin
      G4ThreeVector trans = (*rot)*(center);
      rot->invert(); // inversion ??

       G4Region *regST = new G4Region("Start");
       log->SetRegion(regST);
       regST->AddRootLogicalVolume(log);

      new G4PVPlacement(rot, trans, log, "StartCounter", fLogWorld, false, 0);
   }
   
   // ToF wall
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      
      G4LogicalVolume* log  = fCeBr3->Construct();
      TVector3 ang          = fpFootGeo->GetTWAngles()*TMath::DegToRad(); // in radians
      G4RotationMatrix* rot = new G4RotationMatrix;
      rot->rotateX(ang[0]);
      rot->rotateY(ang[1]);
      rot->rotateZ(ang[2]);
      
      TVector3 center2      = fpFootGeo->GetTWCenter()*cm;
      G4ThreeVector center(center2[0], center2[1], center2[2]);
      
      //To rotate the box wrt hall origin
      G4ThreeVector trans = (*rot)*(center);
      rot->invert(); // inversion ??

       G4Region *regTW = new G4Region("Tof");
       log->SetRegion(regTW);
       regTW->AddRootLogicalVolume(log);

      new G4PVPlacement(rot, trans, log, "TofWall", fLogWorld, false, 0);
   }

   // Calorimeter
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      
      G4LogicalVolume* log  = fPhoswich->Construct();
      TVector3 ang          = fpFootGeo->GetCAAngles()*TMath::DegToRad(); // in radians
      G4RotationMatrix* rot = new G4RotationMatrix;
      rot->rotateX(ang[0]);
      rot->rotateY(ang[1]);
      rot->rotateZ(ang[2]);
      
      TVector3 caCenter2      = fpFootGeo->GetCACenter()*cm;
      G4ThreeVector caCenter(caCenter2[0], caCenter2[1], caCenter2[2]);
      
      //To rotate the box wrt hall origin
      G4ThreeVector trans = (*rot)*(caCenter);
      rot->invert(); // inversion ??

       G4Region *regCA = new G4Region("Calorimeter");
       log->SetRegion(regCA);
       regCA->AddRootLogicalVolume(log);

      new G4PVPlacement(rot, trans, log, "Calorimeter", fLogWorld, false, 0);
   }

    return pWorld;
}

