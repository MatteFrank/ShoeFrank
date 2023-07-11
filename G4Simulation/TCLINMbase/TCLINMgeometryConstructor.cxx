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
  fStartCounter(0x0),
  fTofWall(0x0),
  fpParGeoSt(0x0),
  fpParGeoTw(0x0),
  fpParGeoCa(0x0)
{
   // initialise map file for start counter
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fpParGeoSt = new TAPLparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TAPLparGeo::GetBaseName(), fRunNumber);
      fpParGeoSt->FromFile(mapFileName.Data());
      fStartCounter = new TCPLgeometryConstructor(fpParGeoSt);
   }
   
   // initialise map file for TOF
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fpParGeoTw = new TACEparGeo();
      TString mapFileName =  fCampManager->GetCurGeoFile(TACEparGeo::GetBaseName(), fRunNumber);
      fpParGeoTw->FromFile(mapFileName.Data());
      fTofWall = new TCCEgeometryConstructor(fpParGeoTw);
   }
   
   // initialise map file for calorimteer
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fpParGeoCa = new TAPWparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TAPWparGeo::GetBaseName(), fRunNumber);
      fpParGeoCa->FromFile(mapFileName.Data());
      fCalorimeter = new TCPWgeometryConstructor(fpParGeoCa);
   }
 }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCLINMgeometryConstructor::~TCLINMgeometryConstructor()
{
   if (fStartCounter) delete fStartCounter;
   if (fTofWall)      delete fTofWall;
   if (fCalorimeter)  delete fCalorimeter;
   
   if (fpParGeoSt)    delete fpParGeoSt;
   if (fpParGeoTw)    delete fpParGeoTw;
   if (fpParGeoCa)    delete fpParGeoCa;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Construct FOOT geometry
G4VPhysicalVolume* TCLINMgeometryConstructor::Construct()
{
   G4VPhysicalVolume* pWorld = TCGbaseGeometryConstructor::Construct();
   
   // Start Counter
   if (TAGrecoManager::GetPar()->IncludeST()) {
      
      G4LogicalVolume* log  = fStartCounter->Construct();
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
      
      G4LogicalVolume* log  = fTofWall->Construct();
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

    return pWorld;
}

