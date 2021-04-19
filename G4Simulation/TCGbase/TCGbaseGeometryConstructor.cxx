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

#include "TMath.h"
#include "TCGbaseGeometryConstructor.hxx"

#include "Riostream.h"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"

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
#include "TCGmaterials.hxx"

#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"
#include "TAGroot.hxx"

using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCGbaseGeometryConstructor::TCGbaseGeometryConstructor(const TString expName, Int_t runNumber)
: G4VUserDetectorConstruction(),
  fExpName(expName),
  fRunNumber(runNumber),
  fWorldSizeZ(220*cm),
  fWorldSizeXY(30*cm),
  fWorldMaterial(0x0),
  fLogWorld(0x0),
  fTarget(0x0),
  fpParGeoG(new TAGparGeo()),
  fpMaterials(new TCGmaterials())
{
   // Set run info
   gTAGroot->SetCampaignName(expName);
   gTAGroot->SetRunNumber(runNumber); //number of run
   
   // load campaign file
   fCampManager = new TAGcampaignManager(expName);
   fCampManager->FromFile();
   
   // define material
    fpMaterials->CreateG4DefaultMaterials();

   // default parameter values
    G4Material* pttoMaterial = G4NistManager::Instance()->FindOrBuildMaterial("Air");
	if (pttoMaterial) fWorldMaterial = pttoMaterial;
   
   // initialise geo trafo file
   // global transformation
   fpFootGeo = new TAGgeoTrafo();
   TString geoFileName = fCampManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), fRunNumber);
   fpFootGeo->FromFile(geoFileName.Data());
   
   // initialise map file for target/beam
  if (TAGrecoManager::GetPar()->IncludeTG() || TAGrecoManager::GetPar()->IncludeBM()) {
    TString mapFileName =  fCampManager->GetCurGeoFile(TAGparGeo::GetBaseName(), fRunNumber);
    fpParGeoG->FromFile(mapFileName.Data());
  }
  
   // geometries
  if (TAGrecoManager::GetPar()->IncludeTG())
      fTarget = new TCGtargetConstructor(fpParGeoG);
   
   InfoMc("TCGbaseGeometryConstructor()", "Creating geometry for experiment %s with run number %d\n", expName.Data(), runNumber);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCGbaseGeometryConstructor::~TCGbaseGeometryConstructor()
{
	 delete fTarget;
    delete fpParGeoG;
    delete fpMaterials;
    delete fCampManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* TCGbaseGeometryConstructor::Construct()
{
   G4GeometryManager::GetInstance()->OpenGeometry();
   G4PhysicalVolumeStore::GetInstance()->Clean();
   G4LogicalVolumeStore::GetInstance()->Clean();
   G4SolidStore::GetInstance()->Clean();
      
   // World
   Float_t posBeam = TMath::Abs(fpParGeoG->GetBeamPar().Position.Z()*cm);
   if (fWorldSizeZ <= posBeam*2)
      fWorldSizeZ = posBeam*2.1;
   
   G4Box* sWorld = new G4Box("World", fWorldSizeXY, fWorldSizeXY, fWorldSizeZ);	//dimensions
   
   fLogWorld = new G4LogicalVolume(sWorld, fWorldMaterial,"World");
   fLogWorld->SetVisAttributes(G4VisAttributes::Invisible);
   
   G4VPhysicalVolume* pWorld = new G4PVPlacement(0, G4ThreeVector(), fLogWorld, "World", 0, false, 0);
   
   // Target
   if (TAGrecoManager::GetPar()->IncludeTG()) {

      G4LogicalVolume* logTarget = fTarget->Construct();
   
      //placement
      TVector3 tgAng          = fpFootGeo->GetTGAngles()*TMath::DegToRad(); // in radians
      TVector3 tgCenter       = fpFootGeo->GetTGCenter()*cm;
      G4RotationMatrix* tgRot = new G4RotationMatrix;
      tgRot->rotateX(-tgAng[0]);
      tgRot->rotateY(-tgAng[1]);
      tgRot->rotateZ(-tgAng[2]);
      tgRot->invert(); // inversion ???

      G4Region *regTgt = new G4Region("Target");
      logTarget->SetRegion(regTgt);
      regTgt->AddRootLogicalVolume(logTarget);

      new G4PVPlacement(tgRot, G4ThreeVector(tgCenter[0], tgCenter[1], tgCenter[2]), logTarget, "Target", fLogWorld, false, 0);
   }
   
   return pWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCGbaseGeometryConstructor::SetWordMaterial(G4String materialChoice)
{
   // search the material by its name
   G4Material* pttoMaterial = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);
   if (pttoMaterial) {
      fWorldMaterial = pttoMaterial;
      if(fLogWorld) {
         fLogWorld->SetMaterial(fWorldMaterial);
         G4RunManager::GetRunManager()->PhysicsHasBeenModified();
      }
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCGbaseGeometryConstructor::SetWorldSizeZ(G4double sizeZ)
{
   fWorldSizeZ  = sizeZ;
   G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCGbaseGeometryConstructor::SetWorldSizeXY(G4double sizeXY)
{
   fWorldSizeXY = sizeXY;
   G4RunManager::GetRunManager()->ReinitializeGeometry();
}



