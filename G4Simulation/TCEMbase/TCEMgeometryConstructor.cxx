
#include "TCEMgeometryConstructor.hxx"

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

#include "TADIparGeo.hxx"
#include "TADIgeoField.hxx"
#include "TCEMfield.hxx"
#include "TCEMfieldSetup.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"

using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCEMgeometryConstructor::TCEMgeometryConstructor(TADIparGeo* pParGeo)
: TCGbaseConstructor("TCEMgeometryConstructor", "1.0"),
  fBoxLog(0x0),
  fpParGeo(pParGeo),
   fField(0x0),
   fFieldImpl(0x0),
   fFieldSetup(0x0)
{
   DefineMaxMinDimension();
   DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCEMgeometryConstructor::~TCEMgeometryConstructor()
{
   if (fField)        delete fField;
   if (fFieldImpl)    delete fFieldImpl;
   if (fFieldSetup)   delete fFieldSetup;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void  TCEMgeometryConstructor::DefineSensitive()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4LogicalVolume* TCEMgeometryConstructor::Construct()
{
   Info("Construct()", "Construct Magnets");

   for(Int_t i = 0; i< 3; ++i)
      fSizeBoxMg[i] = (fMaxPosition[i] - fMinPosition[i]);
   
   // Vtx box
   G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("Air");
   G4Material* matMagnet = G4NistManager::Instance()->FindOrBuildMaterial("SmCo");
   G4Box* boxMag = new G4Box("boxMag", 0.5*fSizeBoxMg.X(), 0.5*fSizeBoxMg.Y(), 0.5*fSizeBoxMg.Z());
   
   fBoxLog = new G4LogicalVolume(boxMag, vacuum, "boxMagLog");
   fBoxLog->SetVisAttributes(G4VisAttributes::Invisible);
   
   Int_t nMagnets = fpParGeo->GetMagnetsN();
   
   for (Int_t i = 0; i < nMagnets; ++i) {
      TVector3 size = fpParGeo->GetMagnetPar(i).Size*cm;
      TVector3 pos  = fpParGeo->GetMagnetPar(i).Position*cm;
            
      G4Tubs* magnet = new G4Tubs(Form("Magnet%d", i+1), size[0], size[1], size[2]/2., 0, 360);
      
      //logical
      G4LogicalVolume* magnetLog = new G4LogicalVolume(magnet, matMagnet, Form("MagnetLog%d", i+1));

      G4VisAttributes* targetLog_att = new G4VisAttributes(G4Colour(0.,0.6,1)); //light blue
      targetLog_att->SetForceWireframe(true);
      magnetLog->SetVisAttributes(targetLog_att);
      
      new G4PVPlacement(0, G4ThreeVector(pos[0], pos[1], pos[2]), magnetLog, "Magnets", fBoxLog, false, i+1);
   }
   
   return fBoxLog;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCEMgeometryConstructor::ConstructSDandField()
{
   printf("\n\ntoto\n\n");
   if (fField == 0x0) {
      fFieldImpl  = new TADIgeoField(fpParGeo);
      fField      = new TCEMfield(fFieldImpl);
      fFieldSetup = new TCEMfieldSetup(fField);
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCEMgeometryConstructor::DefineMaxMinDimension()
{
   TVector3 size(0, 0, 0);
   TVector3 posAct(0, 0, 0);
   
   Int_t nSens = fpParGeo->GetMagnetsN();

   TVector3 minPosition(10e10, 10e10, 10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   
   for (Int_t iS = 0; iS < nSens; iS++) {
      posAct = fpParGeo->GetMagnetPar(iS).Position;
      size  = fpParGeo->GetMagnetPar(iS).Size;
      
      for(Int_t i = 0; i < 3; i++) {
         minPosition[i] = (minPosition[i] <= posAct[i] ) ? minPosition[i] : posAct[i];
         maxPosition[i] = (maxPosition[i] >= posAct[i] ) ? maxPosition[i] : posAct[i];
      }
   }
   
   minPosition[0] -= size[1];    maxPosition[0] += size[1];
   minPosition[1] -= size[1];    maxPosition[1] += size[1];
   minPosition[2] -= size[2]/2.; maxPosition[2] += size[2]/2.;

   fMinPosition = minPosition*cm;
   fMaxPosition = maxPosition*cm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCEMgeometryConstructor::DefineMaterial()
{
    fpMaterials->CreateG4Material(fpParGeo->GetMagMat());
}


