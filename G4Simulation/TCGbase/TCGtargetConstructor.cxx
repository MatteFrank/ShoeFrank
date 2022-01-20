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

/*!
 \file TCGtargetConstructor.cxx
 \brief Implementation of TCGtargetConstructor.
*/

#include "TCGtargetConstructor.hxx"

#include "Riostream.h"
#include "TMath.h"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4RunManager.hh"
#include "G4VisAttributes.hh"

#include "TAGrecoManager.hxx"
#include "TAGparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

using namespace CLHEP;
TString TCGtargetConstructor::fgkTargetName  = "targetPhy";

/*!
 \class TCGtargetConstructor
 \brief  Target construction
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
TCGtargetConstructor::TCGtargetConstructor(TAGparGeo* parGeo)
: TCGbaseConstructor("TCGtargetConstructor", "1.0"),
  fpParGeo(parGeo),
  fTargetPhy(0x0),
  fTargetLog(0x0),
  fTargetMat(0x0)
{
   // define material target
   DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCGtargetConstructor::~TCGtargetConstructor()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Construct target
G4LogicalVolume* TCGtargetConstructor::Construct()
{
   // Target logical volume
   Float_t sizeX, sizeY, sizeZ;
   
   if (fpParGeo->GetTargetPar().Shape.Contains("cubic")) {
      sizeX = fpParGeo->GetTargetPar().Size[0]*cm;
      sizeY = fpParGeo->GetTargetPar().Size[1]*cm;
      sizeZ = fpParGeo->GetTargetPar().Size[2]*cm;
   } else if ( fpParGeo->GetTargetPar().Shape.Contains("cyl")) {
      sizeX = fpParGeo->GetTargetPar().Size[1]*cm*2;
      sizeY = fpParGeo->GetTargetPar().Size[0]*cm;
      sizeZ = fpParGeo->GetTargetPar().Size[1]*cm*2;
   } else {
      sizeX = sizeY = sizeZ = 0.;
      Error("Construct()", "No target shape defined");
   }

   G4Material* vacMat = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");
   G4Box* sTarget = new G4Box("sTargert", sizeX/2., sizeY/2., sizeZ/2.);
   
   fLogMother = new G4LogicalVolume(sTarget, vacMat, "Target");
   fLogMother->SetVisAttributes(G4VisAttributes::Invisible);

   
   // construction
   if (fpParGeo->GetTargetPar().Shape.Contains("cubic")) {
      BuildCubicTarget();
   } else if ( fpParGeo->GetTargetPar().Shape.Contains("cyl")) {
      BuildCylindricTarget();
   } else {
      Error("Construct()", "NOT TARGET DEFINED CHECK TARGET STATUS PARAMETER:0: No target  1:cubic - 2: cylinder\n\n");
   }
   
   if (fpParGeo->GetInsertsN() != 0)
       BuildInsert();

   
   return fLogMother;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Build cubic target
void TCGtargetConstructor::BuildCubicTarget()
{
   InfoMcMsg("BuildCubicTarget()", "Build cubic target ");
   
   //build the dimension of target
   TVector3 size(fpParGeo->GetTargetPar().Size[0]*cm, fpParGeo->GetTargetPar().Size[1]*cm,
                        fpParGeo->GetTargetPar().Size[2]*cm);
   G4Box* target  = new G4Box("target", 0.5*size.X(), 0.5*size.Y(), 0.5*size.Z());
   
   //logical
   fTargetLog = new G4LogicalVolume(target, fTargetMat, "targetLog");
   
   G4VisAttributes* targetLog_att = new G4VisAttributes(G4Colour(1,0,0)); //Grey
   if (fpParGeo->GetInsertsN() == 0)
      targetLog_att->SetForceSolid(true);
   else
      targetLog_att->SetForceWireframe(true);
   fTargetLog->SetVisAttributes(targetLog_att);

   new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fTargetLog, GetTargetName(), fLogMother, false, 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Build cylindrical target
void TCGtargetConstructor::BuildCylindricTarget()
{
   Info("BuildCylindricTarget()", "Build cylindric target ");

   //build the dimension of target
   G4double radiusMax = fpParGeo->GetTargetPar().Size[1]*0.5*cm;
   G4double radiusMin = 0;
   
   G4double highZ      = fpParGeo->GetTargetPar().Size[0]*0.5*cm;
   G4double angle      = fpParGeo->GetTargetPar().Size[2]*deg;
   G4double startAngle = 0;
   G4Tubs* target = new G4Tubs("target", radiusMin, radiusMax, highZ, startAngle, angle);
   
   G4cout<<"built a cylinder target with height "<<highZ<<" radius "<<radiusMax<<endl;
   
   //logical
   fTargetLog = new G4LogicalVolume(target, fTargetMat, "targetLog");
   
   G4VisAttributes* targetLog_att = new G4VisAttributes(G4Colour(1,0,0)); //rosso
   targetLog_att->SetForceWireframe(true);
   if (fpParGeo->GetInsertsN() == 0)
      targetLog_att->SetForceSolid(true);
   fTargetLog->SetVisAttributes(targetLog_att);
   
   TVector3 tgAng;
   tgAng[0] = 90*TMath::DegToRad(); //cylinder along z axis in creation
   
   G4RotationMatrix* tgRot = new G4RotationMatrix;
   tgRot->rotateX(-tgAng[0]);
   
   new G4PVPlacement(tgRot, G4ThreeVector(0, 0, 0), fTargetLog, GetTargetName(), fLogMother, false, 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCGtargetConstructor::BuildInsert()
{
   // loop over insert
   // N.B. insert position calculated wrt cylinder before rotation
   
   G4String nameLogInsert = "";
   G4String nameInsert = "";
   G4String nameVol = "";
   G4CSGSolid* insertVol = 0x0;
   
   G4NistManager* man = G4NistManager::Instance();

   //parameterised position
   for(Int_t q = 0; q < fpParGeo->GetInsertsN(); q++) {
      
      //Define vector position for each insert
      G4ThreeVector origin(fpParGeo->GetInsertPar(q).Position.X()*cm, fpParGeo->GetInsertPar(q).Position.Y()*cm, fpParGeo->GetInsertPar(q).Position.Z()*cm);
      
      // material
      G4Material *insertMat = man->FindOrBuildMaterial(fpParGeo->GetInsertPar(q).Material.Data());
      
      nameLogInsert = Form("%s%d","insertLog", q);
      nameInsert    = Form("%s%d%s", "insert", q, "Phy");
      nameVol       = Form("insert%d", q);
      
      fpParGeo->GetInsertPar(q).Shape.ToLower();
      if (fpParGeo->GetInsertPar(q).Shape.Contains("cubic")) {
         G4double insertDimX = fpParGeo->GetInsertPar(q).Size.X()/2.*cm;
         G4double insertDimY = fpParGeo->GetInsertPar(q).Size.Y()/2.*cm;
         G4double insertDimZ = fpParGeo->GetInsertPar(q).Size.Z()/2.*cm;
         insertVol = new G4Box(nameVol.data(), insertDimX, insertDimY,insertDimZ);
         
      } else if (fpParGeo->GetInsertPar(q).Shape.Contains("cylindric")) {
         G4double angle        = 360;
         G4double startAngle   = 0;
         G4double insertRadMin = 0;
         G4double insertRadMax = fpParGeo->GetInsertPar(q).Size.X()*cm;
         G4double insertMiHigh = fpParGeo->GetInsertPar(q).Size.Y()*cm;
         
         insertVol = new G4Tubs(nameVol.data(), insertRadMin, insertRadMax, insertMiHigh, startAngle, angle);     //always 360 degrees
         
      }

      fInsertLog[q] = new G4LogicalVolume(insertVol, insertMat, nameLogInsert.data());
      SetInsertAttributes(q, fpParGeo->GetInsertPar(q).Material.Data());
      
      //Physical insert
      new G4PVPlacement(0x0, G4ThreeVector(origin[0], origin[1], origin[2]), fInsertLog[q], nameInsert, fTargetLog, false, q);
   }
   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define materials
void TCGtargetConstructor::DefineMaterial()
{
   G4NistManager* man = G4NistManager::Instance();
   //Defined used element
   G4Material* C = man->FindOrBuildMaterial("G4_C"); //PMMA + BONE 6
   G4Material* O = man->FindOrBuildMaterial("G4_O"); //PMMA + BONE + AIR 8
   G4Material* H = man->FindOrBuildMaterial("G4_H"); //PMMA + BONE 1
   G4Material* N = man->FindOrBuildMaterial("G4_N"); //AIR + BONE 7
   G4Material* Mg = man->FindOrBuildMaterial("G4_Mg"); //BONE 12
   G4Material* P = man->FindOrBuildMaterial("G4_P"); //BONE 15
   G4Material* S = man->FindOrBuildMaterial("G4_S"); //BONE 16
   G4Material* Ca = man->FindOrBuildMaterial("G4_Ca"); //BONE 20
   G4Material* Zn = man->FindOrBuildMaterial("G4_Zn"); //BONE 30
   
   // Water
   G4Material* water = man->FindOrBuildMaterial("G4_WATER");
   
   G4double density, fractionmass;
   G4String name;
   G4int ncomponents;
   
   //PMMA
   //Polyméthacrylate de méthyle - (C5 H8 O2)n - C: 59.9% H: 8.0% O: 31.9%
   density = 1.19*g/cm3;
   G4Material* pmma = new G4Material(name = "PMMA", density, ncomponents=3);
   pmma->AddMaterial(C, fractionmass = 59.98*perCent);
   pmma->AddMaterial(O, fractionmass = 31.96*perCent);
   pmma->AddMaterial(H, fractionmass =  8.05*perCent);
   
   //Bone
   G4Material* Bone = new G4Material(name = "Bone", density= 1.85*g/cm3, ncomponents=9);
   Bone->AddMaterial(H, 4.72*perCent); //1
   Bone->AddMaterial(C, 14.43*perCent); //6
   Bone->AddMaterial(N, 4.20*perCent); //7
   Bone->AddMaterial(O, 44.61*perCent); //8
   Bone->AddMaterial(Mg, 0.22*perCent); //12
   Bone->AddMaterial(P, 10.50*perCent); //15
   Bone->AddMaterial(S, 0.31*perCent); //16
   Bone->AddMaterial(Ca, 21.00*perCent); //20
   Bone->AddMaterial(Zn, 0.01*perCent); //30
   
   //target: fill with material
   TString mat = fpParGeo->GetTargetPar().Material;
   
   mat.ToUpper();
   if (mat.Contains("C")) {
      fTargetMat = C;
   } else if (mat.Contains("PMMA")) {
      fTargetMat = pmma;
   } else if(mat.Contains("WATER")) {
      fTargetMat = water;
   } else
      Warning("DefineMaterial()", "No target material named: ");
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Set insert attributes
//!
//! \param[in] insert insert index
//! \param[in] material insert material 
void TCGtargetConstructor::SetInsertAttributes(G4int insert, G4String material)
{
   if (material == "Air") {
      G4VisAttributes* insert_att = new G4VisAttributes(G4Colour(0,1,1)); //cyano
      insert_att->SetForceWireframe(true);
      insert_att->SetForceSolid(true);
      fInsertLog[insert]->SetVisAttributes(insert_att);
      
   } else if(material == "PMMA") {
      G4VisAttributes* insert_att = new G4VisAttributes(G4Colour(1,0,0)); //red
      insert_att->SetForceWireframe(true);
      insert_att->SetForceSolid(true);
      fInsertLog[insert]->SetVisAttributes(insert_att);
      
   } else if(material == "Bone") {
      G4VisAttributes* insert_att = new G4VisAttributes(G4Colour(1,1,1)); //white
      insert_att->SetForceWireframe(true);
      insert_att->SetForceSolid(true);
      fInsertLog[insert]->SetVisAttributes(insert_att);
      
   } else {
      cout<<endl;
      cout<<endl;
      cout<<"Material "<<material<<" is not defined. Attribute is ignored"<<endl;
      cout<<endl;
      cout<<endl;
   }
   
   G4RunManager::GetRunManager()->GeometryHasBeenModified();
}






