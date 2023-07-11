/*!
 \file TCCEgeometryConstructor.cxx
 \brief Implementation of TCCEgeometryConstructor.
*/

#include "TCCEgeometryConstructor.hxx"

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

#include "TACEparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TCCEsensitiveDetector.hxx"

#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"

using namespace CLHEP;

TString TCCEgeometryConstructor::fgkCeSDname  = "ceSD";

/*!
 \class TCCEgeometryConstructor
 \brief Building STC detector geometry
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] pParGeo geometry parameter
TCCEgeometryConstructor::TCCEgeometryConstructor(TACEparGeo* pParGeo)
: TCGbaseConstructor("TCCEgeometryConstructor", "1.0"),
  fBoxLog(0x0),
  fpParGeo(pParGeo)
{
   DefineMaxMinDimension();
   DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCCEgeometryConstructor::~TCCEgeometryConstructor()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Construct detector
G4LogicalVolume* TCCEgeometryConstructor::Construct()
{
   printf("Construct() - Construct Time of Flight\n");

   for(Int_t i = 0; i< 3; ++i)
      fSizeBoxSt[i] = (fMaxPosition[i] - fMinPosition[i]);
   
   // Vtx box
   TString material = fpParGeo->GetMaterial();
   G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");
   G4Material* mat = G4NistManager::Instance()->FindOrBuildMaterial(material.Data());
   TString materialW = fpParGeo->GetWindowMaterial();
   G4Material* matW = G4NistManager::Instance()->FindOrBuildMaterial(materialW.Data());

   G4Box* boxTw = new G4Box("boxTw", 0.5*fSizeBoxSt.X(), 0.5*fSizeBoxSt.Y(), 0.5*fSizeBoxSt.Z());
   
   fBoxLog = new G4LogicalVolume(boxTw, vacuum, "boxtwtLog");
   fBoxLog->SetVisAttributes(G4VisAttributes::GetInvisible());
   
   TVector3 size   = fpParGeo->GetSize()*cm;
   TVector3 sizeW  = fpParGeo->GetWindowSize()*cm;
   Double_t phiMin = 0.;
   Double_t phiMax = 360.;

   
   //logical
   G4Tubs* tw = new G4Tubs("TimeOfFlight", 0, size[1]/2., size[2]/2., phiMin, phiMax);
   fTofLog = new G4LogicalVolume(tw, mat, "twtLog");

   G4Tubs* twW = new G4Tubs("Window", 0, sizeW[1]/2., sizeW[2]/2., phiMin, phiMax);
   fTofWLog = new G4LogicalVolume(twW, matW, "twWLog");

   
   G4VisAttributes* targetLog_att = new G4VisAttributes(G4Colour(0., 0.8, 0.5)); //light red
   targetLog_att->SetForceSolid(true);
   fTofLog->SetVisAttributes(targetLog_att);
   
   new G4PVPlacement(0, G4ThreeVector(0, 0, -(size[2]+sizeW[2])/2. + sizeW[2]/2), fTofWLog, "TimeOfFlightWindow", fBoxLog, false, 1);
   new G4PVPlacement(0, G4ThreeVector(0, 0,  (size[2]+sizeW[2])/2. - size[2]/2.), fTofLog,  "TimeOfFlight",       fBoxLog, false, 2);

   DefineSensitive();

   return fBoxLog;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define sensitive detector volume
void  TCCEgeometryConstructor::DefineSensitive()
{
   // sensitive volume
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   
   G4String twSDname;
   twSDname = fgkCeSDname;
   printf("DefineSensitive() - Define sensitive for CeBr3\n");
  
   TCCEsensitiveDetector* twSensitive = new TCCEsensitiveDetector(twSDname);
   SDman->AddNewDetector(twSensitive);
   fTofLog->SetSensitiveDetector(twSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define the dimension of the detecteor envelop
void TCCEgeometryConstructor::DefineMaxMinDimension()
{
   TVector3 size = fpParGeo->GetSize();
   TVector3 sizeW = fpParGeo->GetWindowSize();

   TVector3 minPosition(10e10, 10e10, 10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   
   
   minPosition[0] = -size[0]/2.;   maxPosition[0] = +size[0]/2.;
   minPosition[1] = -size[1]/2.;   maxPosition[1] = +size[1]/2.;
   minPosition[2] = -(size[2]+sizeW[2])/2.;   maxPosition[2] = +(size[2]+sizeW[2]);

   fMinPosition = minPosition*cm;
   fMaxPosition = maxPosition*cm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define materials
void TCCEgeometryConstructor::DefineMaterial()
{
   fpMaterials->CreateG4Material(fpParGeo->GetMaterial());
   fpMaterials->CreateG4Material(fpParGeo->GetWindowMaterial());
}


