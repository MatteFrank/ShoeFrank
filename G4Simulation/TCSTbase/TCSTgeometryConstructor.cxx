/*!
 \file TCSTgeometryConstructor.cxx
 \brief Implementation of TCSTgeometryConstructor.
*/

#include "TCSTgeometryConstructor.hxx"

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

#include "TASTparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TCSTsensitiveDetector.hxx"

#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"

using namespace CLHEP;

TString TCSTgeometryConstructor::fgkIrSDname  = "IrSD";

/*!
 \class TCSTgeometryConstructor
 \brief Building STC detector geometry
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] pParGeo geometry parameter
TCSTgeometryConstructor::TCSTgeometryConstructor(TASTparGeo* pParGeo)
: TCGbaseConstructor("TCSTgeometryConstructor", "1.0"),
  fBoxLog(0x0),
  fpParGeo(pParGeo)
{
   DefineMaxMinDimension();
   DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCSTgeometryConstructor::~TCSTgeometryConstructor()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Construct detector
G4LogicalVolume* TCSTgeometryConstructor::Construct()
{
   InfoMcMsg("Construct()", "Construct Start Counter");

   for(Int_t i = 0; i< 3; ++i)
      fSizeBoxSt[i] = (fMaxPosition[i] - fMinPosition[i]);
   
   // Vtx box
   G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");
   G4Material* matEJ228 = G4NistManager::Instance()->FindOrBuildMaterial("EJ228");
   G4Box* boxIr = new G4Box("boxIr", 0.5*fSizeBoxSt.X(), 0.5*fSizeBoxSt.Y(), 0.5*fSizeBoxSt.Z());
   
   fBoxLog = new G4LogicalVolume(boxIr, vacuum, "boxIrLog");
   fBoxLog->SetVisAttributes(G4VisAttributes::Invisible);
   
   TVector3 size  = fpParGeo->GetSize()*cm;
   
   //logical
   G4Box* st = new G4Box("StartCounter", size[0]/2, size[1]/2., size[2]/2.);
   fStcLog = new G4LogicalVolume(st, matEJ228, "IrLog");
   
   G4VisAttributes* targetLog_att = new G4VisAttributes(G4Colour(0., 0.8 ,1)); //light red
   targetLog_att->SetForceSolid(true);
   fStcLog->SetVisAttributes(targetLog_att);
   
   new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fStcLog, "StartCounter", fBoxLog, false, 1);
   
   
   DefineSensitive();

   return fBoxLog;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define sensitive detector volume
void  TCSTgeometryConstructor::DefineSensitive()
{
   // sensitive volume
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   
   G4String calSDname;
   calSDname = fgkIrSDname;
   InfoMcMsg("DefineSensitive()", "Define sensitive for Start Counter");
  
   TCSTsensitiveDetector* calSensitive = new TCSTsensitiveDetector(calSDname);
   SDman->AddNewDetector(calSensitive);
   fStcLog->SetSensitiveDetector(calSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define the dimension of the detecteor envelop
void TCSTgeometryConstructor::DefineMaxMinDimension()
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
void TCSTgeometryConstructor::DefineMaterial()
{
    fpMaterials->CreateG4Material(fpParGeo->GetMaterial());   
}


