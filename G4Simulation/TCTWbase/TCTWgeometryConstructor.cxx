
#include "TCTWgeometryConstructor.hxx"

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

#include "TATWparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TCTWsensitiveDetector.hxx"

#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"

using namespace CLHEP;

TString TCTWgeometryConstructor::fgkTofSDname   = "TwSD";

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCTWgeometryConstructor::TCTWgeometryConstructor(TATWparGeo* pParGeo)
: TCGbaseConstructor("TCTWgeometryConstructor", "1.0"),
  fBoxLog(0x0),
  fpParGeo(pParGeo)
{
   DefineMaxMinDimension();
   DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCTWgeometryConstructor::~TCTWgeometryConstructor()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4LogicalVolume* TCTWgeometryConstructor::Construct()
{
   InfoMcMsg("Construct()", "Construct ToF Wall");

   for(Int_t i = 0; i< 3; ++i)
      fSizeBoxTof[i] = (fMaxPosition[i] - fMinPosition[i]);
   
   // Vtx box
   G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("Air");
   G4Material* matEJ232 = G4NistManager::Instance()->FindOrBuildMaterial("EJ232");
   G4Box* boxTof = new G4Box("boxMag", 0.5*fSizeBoxTof.X(), 0.5*fSizeBoxTof.Y(), 0.5*fSizeBoxTof.Z());
   
   fBoxLog = new G4LogicalVolume(boxTof, vacuum, "boxTofoLog");
   fBoxLog->SetVisAttributes(G4VisAttributes::Invisible);
   
   Float_t height = fpParGeo->GetBarHeight()*cm;
   Float_t width  = fpParGeo->GetBarWidth()*cm;
   Float_t thick  = fpParGeo->GetBarThick()*cm;
   Int_t slatsN   = fpParGeo->GetNBars();
   Int_t layersN  = fpParGeo->GetLayersN();
   
   //logical
   G4Box* tof     = new G4Box("Slat", width/2., height/2., thick/2.);
   fTofLog        = new G4LogicalVolume(tof, matEJ232, "SlatLog");
   
   G4VisAttributes* targetLog_att = new G4VisAttributes(G4Colour(0.,0.8,1)); //light blue
   targetLog_att->SetForceWireframe(true);
   fTofLog->SetVisAttributes(targetLog_att);
   
   G4RotationMatrix* rot = new G4RotationMatrix;
   rot->rotateX(0);
   rot->rotateY(0);
   rot->rotateZ(90*TMath::DegToRad());
   rot->invert(); // inversion
   
   for (Int_t i = 0; i < layersN; ++i) {
      
      for (Int_t j = 0; j < slatsN; ++j) {
         TVector3 pos = fpParGeo->GetBarPosition(i, j)*cm;
         if (i == 0)
            new G4PVPlacement(0,   G4ThreeVector(pos[0], pos[1], pos[2]), fTofLog, "Tof", fBoxLog, false, j + TATWparGeo::GetLayerOffset()*i);
         else
            new G4PVPlacement(rot, G4ThreeVector(pos[0], pos[1], pos[2]), fTofLog, "Tof", fBoxLog, false, j + TATWparGeo::GetLayerOffset()*i);
      }
   }
   
   DefineSensitive();

   return fBoxLog;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void  TCTWgeometryConstructor::DefineSensitive()
{
   // sensitive volume
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   
   G4String twSDname;
   twSDname = fgkTofSDname;
   InfoMcMsg("DefineSensitive()", "Define sensitive for ToF Wall");
  
   TCTWsensitiveDetector* twSensitive = new TCTWsensitiveDetector(twSDname);
   twSensitive->SetCopyLevel(0);
   SDman->AddNewDetector(twSensitive);
   fTofLog->SetSensitiveDetector(twSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCTWgeometryConstructor::DefineMaxMinDimension()
{

   TVector3 size  = fpParGeo->GetBarSize();
   Float_t  thick = fpParGeo->GetBarThick();

   TVector3 minPosition(10e10, 10e10, 10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   
   
   minPosition[0] = -size[1]/2.; maxPosition[0] = +size[1]/2.;
   minPosition[1] = -size[1]/2.; maxPosition[1] = +size[1]/2.;
   minPosition[2] = -thick/2.;   maxPosition[2] = +thick/2.;

   fMinPosition = minPosition*cm;
   fMaxPosition = maxPosition*cm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCTWgeometryConstructor::DefineMaterial()
{
    fpMaterials->CreateG4Material(fpParGeo->GetBarMat());
}


