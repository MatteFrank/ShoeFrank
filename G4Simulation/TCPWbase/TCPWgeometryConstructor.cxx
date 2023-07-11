/*!
 \file TCPWgeometryConstructor.cxx
 \brief Implementation of TCPWgeometryConstructor.
*/

#include "TCPWgeometryConstructor.hxx"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4Element.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "TAPWparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TCPWsensitiveDetector.hxx"

#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"

using namespace CLHEP;

TString TCPWgeometryConstructor::fgkPwSDname  = "pwSD";

/*!
 \class TCPWgeometryConstructor
 \brief Building STC detector geometry
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] pParGeo geometry parameter
TCPWgeometryConstructor::TCPWgeometryConstructor(TAPWparGeo* pParGeo)
: TCGbaseConstructor("TCPWgeometryConstructor", "1.0"),
  fBoxLog(0x0),
  fpParGeo(pParGeo)
{
   DefineMaxMinDimension();
   DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCPWgeometryConstructor::~TCPWgeometryConstructor()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Construct detector
G4LogicalVolume* TCPWgeometryConstructor::Construct()
{
   printf("Construct() - Construct Time of Flight\n");

   for(Int_t i = 0; i< 3; ++i)
      fSizeBoxSt[i] = (fMaxPosition[i] - fMinPosition[i]);
   
   // Vtx box
   TString material1 = fpParGeo->GetMaterial1();
   G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");
   G4Material* mat1 = G4NistManager::Instance()->FindOrBuildMaterial(material1.Data());
   
   TString material2 = fpParGeo->GetMaterial2();
   G4Material* mat2 = G4NistManager::Instance()->FindOrBuildMaterial(material2.Data());
   
   TVector3 size1   = fpParGeo->GetSize1()*cm;
   TVector3 size2   = fpParGeo->GetSize2()*cm;

   G4Box* boxPw = new G4Box("boxPw", 0.5*size1.X(), 0.5*size1.Y(), 0.5*(size1.Z() + size2.Z()));
   
   fBoxLog = new G4LogicalVolume(boxPw, vacuum, "boxPW");
   fBoxLog->SetVisAttributes(G4VisAttributes::GetInvisible());

   
   //logical
   G4Box* laBr = new G4Box("LaBr3", size1[0]/2., size1[1]/2., size1[2]/2.);
   fLaBrLog = new G4LogicalVolume(laBr, mat1, "Labr3Log");

   G4Box* csi = new G4Box("Window", size2[0]/2., size2[1]/2., size2[2]/2.);
   fCsILog = new G4LogicalVolume(csi, mat2, "twWLog");

   
   G4VisAttributes* log_att = new G4VisAttributes(G4Colour(0., 0.4, 0.5)); //light red
   log_att->SetForceSolid(true);
   fBoxLog->SetVisAttributes(log_att);
   
   new G4PVPlacement(0, G4ThreeVector(0, 0, -(size1[2]+size2[2])/2. + size1[2]/2.), fLaBrLog, "LaBr2", fBoxLog, false, 1);
   new G4PVPlacement(0, G4ThreeVector(0, 0, (size1[2]+size2[2])/2. - size2[2]/2.),  fCsILog,  "CsI",   fBoxLog, false, 2);

   DefineSensitive();

   return fBoxLog;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define sensitive detector volume
void  TCPWgeometryConstructor::DefineSensitive()
{
   // sensitive volume
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   
   G4String twSDname;
   twSDname = fgkPwSDname;
   printf("DefineSensitive() - Define sensitive for Phoswich\n");
  
   TCPWsensitiveDetector* twSensitive = new TCPWsensitiveDetector(twSDname);
   SDman->AddNewDetector(twSensitive);
   fBoxLog->SetSensitiveDetector(twSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define the dimension of the detecteor envelop
void TCPWgeometryConstructor::DefineMaxMinDimension()
{
   TVector3 size = fpParGeo->GetSize1();

   size[2] += fpParGeo->GetSize2().Z();
   
   TVector3 minPosition(10e10, 10e10, 10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   
   
   minPosition[0] = -size[0]/2.;   maxPosition[0] = +size[0]/2.;
   minPosition[1] = -size[1]/2.;   maxPosition[1] = +size[1]/2.;
   minPosition[2] = -size[2]/2.;   maxPosition[2] = +size[2]/2.;

   fMinPosition = minPosition*cm;
   fMaxPosition = maxPosition*cm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define materials
void TCPWgeometryConstructor::DefineMaterial()
{
   fpMaterials->CreateG4Material(fpParGeo->GetMaterial1());
   fpMaterials->CreateG4Material(fpParGeo->GetMaterial2());
}


