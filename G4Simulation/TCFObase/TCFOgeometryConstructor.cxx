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

#include "TCFOgeometryConstructor.hxx"

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
#include "TCSTgeometryConstructor.hxx"
#include "TCBMgeometryConstructor.hxx"
#include "TCVTgeometryConstructor.hxx"
#include "TCITgeometryConstructor.hxx"
#include "TCEMgeometryConstructor.hxx"
#include "TCMSDgeometryConstructor.hxx"
#include "TCCAgeometryConstructor.hxx"
#include "TCTWgeometryConstructor.hxx"
#include "TCEMfield.hxx"
#include "TCEMfieldSetup.hxx"

#include "TAGrecoManager.hxx"
#include "TADIgeoField.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCFOgeometryConstructor::TCFOgeometryConstructor(const TString expName, Int_t runNumber)
: TCGbaseGeometryConstructor(expName, runNumber),
  fStartCounter(0x0),
  fBeamMonitor(0x0),
  fVertex(0x0),
  fInnerTracker(0x0),
  fMicroStrip(0x0),
  fCalorimeter(0x0),
  fTofWall(0x0),
  fMagnet(0x0),
  fField(0x0),
  fFieldImpl(0x0),
  fFieldSetup(0x0),
  fpParGeoSt(0x0),
  fpParGeoBm(0x0),
  fpParGeoVtx(0x0),
  fpParGeoIt(0x0),
  fpParGeoEm(0x0),
  fpParGeoMsd(0x0),
  fpParGeoCa(0x0),
  fpParGeoTw(0x0)
{
   // initialise map file for calorimteer
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      fpParGeoCa = new TACAparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TACAparGeo::GetBaseName(), fRunNumber);
      fpParGeoCa->FromFile(mapFileName.Data());
      fCalorimeter = new TCCAgeometryConstructor(fpParGeoCa);
   }
   
   // initialise map file for TOF
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      fpParGeoTw = new TATWparGeo();
      TString mapFileName =  fCampManager->GetCurGeoFile(TATWparGeo::GetBaseName(), fRunNumber);
      fpParGeoTw->FromFile(mapFileName.Data());
      fTofWall = new TCTWgeometryConstructor(fpParGeoTw);
   }

   // initialise map file for Micro Strip Detector
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      fpParGeoMsd = new TAMSDparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), fRunNumber);
      fpParGeoMsd->FromFile(mapFileName.Data());
      fMicroStrip = new TCMSDgeometryConstructor(fpParGeoMsd);
   }

   // initialise map file for Inner Tracker
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      fpParGeoIt = new TAITparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TAITparGeo::GetBaseName(), fRunNumber);
      fpParGeoIt->FromFile(mapFileName.Data());
      fInnerTracker = new TCITgeometryConstructor(fpParGeoIt);
   }
   
   // initialise map file for vertex
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      fpParGeoVtx = new TAVTparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), fRunNumber);
      fpParGeoVtx->FromFile(mapFileName.Data());
      fVertex = new TCVTgeometryConstructor(fpParGeoVtx);
   }
   
   // initialise map file for magnet
   if (TAGrecoManager::GetPar()->IncludeDI()) {
      fpParGeoEm = new TADIparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TADIparGeo::GetBaseName(), fRunNumber);
      fpParGeoEm->FromFile(mapFileName.Data());
      fMagnet = new TCEMgeometryConstructor(fpParGeoEm);
   }
   
   // initialise map file for beam monitor
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      fpParGeoBm = new TABMparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TABMparGeo::GetBaseName(), fRunNumber);
      fpParGeoBm->FromFile(mapFileName.Data());
      fBeamMonitor = new TCBMgeometryConstructor(fpParGeoBm);
   }
   
   // initialise map file for start counter
   if (TAGrecoManager::GetPar()->IncludeST()) {
      fpParGeoSt = new TASTparGeo();
      TString mapFileName = fCampManager->GetCurGeoFile(TASTparGeo::GetBaseName(), fRunNumber);
      fpParGeoSt->FromFile(mapFileName.Data());
      fStartCounter = new TCSTgeometryConstructor(fpParGeoSt);
   }
 }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCFOgeometryConstructor::~TCFOgeometryConstructor()
{
   if (fStartCounter) delete fStartCounter;
   if (fBeamMonitor)  delete fBeamMonitor;
   if (fVertex)       delete fVertex;
   if (fInnerTracker) delete fInnerTracker;
   if (fMicroStrip)   delete fMicroStrip;
   if (fCalorimeter)  delete fCalorimeter;
   if (fTofWall)      delete fTofWall;
   if (fMagnet)       delete fMagnet;
   
   if (fField)        delete fField;
   if (fFieldSetup)   delete fFieldSetup;
   if (fFieldImpl)    delete fFieldImpl;
   
   if (fpParGeoSt)    delete fpParGeoSt;
   if (fpParGeoBm)    delete fpParGeoBm;
   if (fpParGeoVtx)   delete fpParGeoVtx;
   if (fpParGeoIt)    delete fpParGeoIt;
   if (fpParGeoEm)    delete fpParGeoEm;
   if (fpParGeoMsd)   delete fpParGeoMsd;
   if (fpParGeoCa)    delete fpParGeoCa;
   if (fpParGeoTw)    delete fpParGeoTw;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* TCFOgeometryConstructor::Construct()
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
   
   // Beam Monitor
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      
      G4LogicalVolume* log  = fBeamMonitor->Construct();
      TVector3 ang          = fpFootGeo->GetBMAngles()*TMath::DegToRad(); // in radians
      G4RotationMatrix* rot = new G4RotationMatrix;
      rot->rotateX(ang[0]);
      rot->rotateY(ang[1]);
      rot->rotateZ(ang[2]);
      
      TVector3 center2      = fpFootGeo->GetBMCenter()*cm;
      G4ThreeVector center(center2[0], center2[1], center2[2]);

      //To rotate the box wrt hall origin
      G4ThreeVector trans = (*rot)*(center);
      rot->invert(); // inversion ??

       G4Region *regBM = new G4Region("BM");
       log->SetRegion(regBM);
       regBM->AddRootLogicalVolume(log);

      new G4PVPlacement(rot, trans, log, "BeamMonitor", fLogWorld, false, 0);
   }


   // Vertex
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      
      G4LogicalVolume* log  = fVertex->Construct();
      TVector3 ang          = fpFootGeo->GetVTAngles()*TMath::DegToRad(); // in radians
      G4RotationMatrix* rot = new G4RotationMatrix;
      rot->rotateX(ang[0]);
      rot->rotateY(ang[1]);
      rot->rotateZ(ang[2]);
   
      TVector3 center2      = fpFootGeo->GetVTCenter()*cm;
      G4ThreeVector center(center2[0], center2[1], center2[2]);
      
      //To rotate the box wrt hall origin
      G4ThreeVector trans = (*rot)*(center);
      rot->invert(); // inversion ??

       G4Region *regVtx = new G4Region("Vertex");
       log->SetRegion(regVtx);
       regVtx->AddRootLogicalVolume(log);

      new G4PVPlacement(rot, trans, log, "Vertex", fLogWorld, false, 0);
   }

   // Magnets
   G4LogicalVolume* logMag = 0x0;
   Bool_t isMagnet         = false;

   if (TAGrecoManager::GetPar()->IncludeDI()) {
      
      logMag = fMagnet->Construct();
      TVector3 ang          = fpFootGeo->GetDIAngles()*TMath::DegToRad(); // in radians
      G4RotationMatrix* rot = new G4RotationMatrix;
      rot->rotateX(ang[0]);
      rot->rotateY(ang[1]);
      rot->rotateZ(ang[2]);

      TVector3 center2      = fpFootGeo->GetDICenter()*cm;
      G4ThreeVector center(center2[0], center2[1], center2[2]);
      
      //To rotate the box wrt hall origin
      G4ThreeVector trans = (*rot)*(center);
      rot->invert(); // inversion ??

       G4Region *regMag = new G4Region("Magnet");
       logMag->SetRegion(regMag);
       regMag->AddRootLogicalVolume(logMag);

      new G4PVPlacement(rot, trans, logMag, "Magnet", fLogWorld, false, 0);
      isMagnet = true;
   }

   // Inner tracker
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      
      G4LogicalVolume* log  = fInnerTracker->Construct();
      TVector3 ang          = fpFootGeo->GetITAngles()*TMath::DegToRad(); // in radians
      G4RotationMatrix* rot = new G4RotationMatrix;
      rot->rotateX(ang[0]);
      rot->rotateY(ang[1]);
      rot->rotateZ(ang[2]);
      
      TVector3 center2      = fpFootGeo->GetITCenter()*cm;
      G4ThreeVector center(center2[0], center2[1], center2[2]);
      
      if (isMagnet) {
         TVector3 mgCenter2 = fpFootGeo->GetDICenter()*cm;
         center[2] = mgCenter2[2] - center2[2];
      }

      //To rotate the box wrt hall origin
      G4ThreeVector trans = (*rot)*(center);
      rot->invert(); // inversion ??

       G4Region *regIT = new G4Region("InnerTracker");
       log->SetRegion(regIT);
       regIT->AddRootLogicalVolume(log);
      
      if (isMagnet)
         new G4PVPlacement(rot, trans, log, "InnerTracker", logMag, false, 0);
      else
         new G4PVPlacement(rot, trans, log, "InnerTracker", fLogWorld, false, 0);
   }
   
   // Micro Strip Detector
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      
      G4LogicalVolume* log  = fMicroStrip->Construct();
      TVector3 ang          = fpFootGeo->GetMSDAngles()*TMath::DegToRad(); // in radians
      G4RotationMatrix* rot = new G4RotationMatrix;
      rot->rotateX(ang[0]);
      rot->rotateY(ang[1]);
      rot->rotateZ(ang[2]);
      
      TVector3 center2      = fpFootGeo->GetMSDCenter()*cm;
      G4ThreeVector center(center2[0], center2[1], center2[2]);
      
      //To rotate the box wrt hall origin
      G4ThreeVector trans = (*rot)*(center);
      rot->invert(); // inversion ??

       G4Region *regMSD = new G4Region("MSD");
       log->SetRegion(regMSD);
       regMSD->AddRootLogicalVolume(log);

      new G4PVPlacement(rot, trans, log, "MicroStrip", fLogWorld, false, 0);
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

   // Calorimeter
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      
      G4LogicalVolume* log  = fCalorimeter->Construct();
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOgeometryConstructor::ConstructSDandField()
{
   if (!TAGrecoManager::GetPar()->IncludeDI()) return;

   if (!fField) {
      fFieldImpl  = new TADIgeoField(fpParGeoEm);
      fField      = new TCEMfield(fFieldImpl);
      fFieldSetup = new TCEMfieldSetup(fField);
   }
}

