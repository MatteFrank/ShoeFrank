/*!
 \file TCPLgeometryConstructor.cxx
 \brief Implementation of TCPLgeometryConstructor.
*/

#include "TCPLgeometryConstructor.hxx"

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

#include "TAPLparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TCPLsensitiveDetector.hxx"

#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"

using namespace CLHEP;

TString TCPLgeometryConstructor::fgkPlSDname  = "plSD";

/*!
 \class TCPLgeometryConstructor
 \brief Building STC detector geometry
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] pParGeo geometry parameter
TCPLgeometryConstructor::TCPLgeometryConstructor(TAPLparGeo* pParGeo)
: TCGbaseConstructor("TCPLgeometryConstructor", "1.0"),
  fBoxLog(0x0),
  fpParGeo(pParGeo)
{
   DefineMaxMinDimension();
   DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCPLgeometryConstructor::~TCPLgeometryConstructor()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Construct detector
G4LogicalVolume* TCPLgeometryConstructor::Construct()
{
   printf("Construct() - Construct Start Counter\n");

   for(Int_t i = 0; i< 3; ++i)
      fSizeBoxSt[i] = (fMaxPosition[i] - fMinPosition[i]);
   
   // Vtx box
   TString material = fpParGeo->GetMaterial();
   G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");
   G4Material* mat = G4NistManager::Instance()->FindOrBuildMaterial(material.Data());
   G4Box* boxSt = new G4Box("boxSt", 0.5*fSizeBoxSt.X(), 0.5*fSizeBoxSt.Y(), 0.5*fSizeBoxSt.Z());
   
   fBoxLog = new G4LogicalVolume(boxSt, vacuum, "boxstLog");
   fBoxLog->SetVisAttributes(G4VisAttributes::GetInvisible());
   
   TVector3 size  = fpParGeo->GetSize()*cm;
      
   //logical
   G4Box* st = new G4Box("StartCounter", size[0]/2., size[1]/2., size[2]/2.);
   fStcLog = new G4LogicalVolume(st, mat, "stLog");
   
   G4VisAttributes* targetLog_att = new G4VisAttributes(G4Colour(0., 0.8, 1)); //light red
   targetLog_att->SetForceSolid(true);
   fStcLog->SetVisAttributes(targetLog_att);
   
   new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fStcLog, "StartCounter", fBoxLog, false, 1);
   
   DefineSensitive();

   return fBoxLog;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define sensitive detector volume
void  TCPLgeometryConstructor::DefineSensitive()
{
   // sensitive volume
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   
   G4String stSDname;
   stSDname = fgkPlSDname;
   printf("DefineSensitive() - Define sensitive for Start Counter\n");
  
   TCPLsensitiveDetector* stSensitive = new TCPLsensitiveDetector(stSDname);
   SDman->AddNewDetector(stSensitive);
   fStcLog->SetSensitiveDetector(stSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define the dimension of the detecteor envelop
void TCPLgeometryConstructor::DefineMaxMinDimension()
{
   TVector3 size = fpParGeo->GetSize();

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
void TCPLgeometryConstructor::DefineMaterial()
{   
   fpMaterials->CreateG4Material(fpParGeo->GetMaterial());
}


