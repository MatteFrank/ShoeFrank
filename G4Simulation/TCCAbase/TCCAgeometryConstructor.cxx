
#include "TCCAgeometryConstructor.hxx"

#include "G4NistManager.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4Element.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "TACAparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TCCAsensitiveDetector.hxx"

#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"

using namespace CLHEP;

TString TCCAgeometryConstructor::fgkCalSDname  = "caSD";

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCCAgeometryConstructor::TCCAgeometryConstructor(TACAparGeo* pParGeo)
: TCGbaseConstructor("TCCAgeometryConstructor", "1.0"),
  fBoxLog(0x0),
  fpParGeo(pParGeo)
{
   DefineMaxMinDimension();
   DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCCAgeometryConstructor::~TCCAgeometryConstructor()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4LogicalVolume* TCCAgeometryConstructor::Construct()
{
   Info("Construct()", "Construct Calorimeter");

   for(Int_t i = 0; i< 3; ++i)
      fSizeBoxCal[i] = (fMaxPosition[i] - fMinPosition[i]);


    // Calorimeter box
   G4Material* air    = G4NistManager::Instance()->FindOrBuildMaterial("Air");
   G4Material* matBGO = G4NistManager::Instance()->FindOrBuildMaterial(fpParGeo->GetCrystalMat().Data());
   G4Material* matAl  = G4NistManager::Instance()->FindOrBuildMaterial(fpParGeo->GetSupportMat().Data());
   G4Box* boxCal = new G4Box("boxCal", 0.5*fSizeBoxCal.X(), 0.5*fSizeBoxCal.Y(), fSizeBoxCal.Z());
   fBoxLog = new G4LogicalVolume(boxCal, air, "boxCaloLog");
   fBoxLog->SetVisAttributes(G4VisAttributes::Invisible);

    Double_t xdimCrys1 = fpParGeo->GetCrystalSize()[0]*cm;
    Double_t xdimCrys2 = fpParGeo->GetCrystalSize()[1]*cm;
    Double_t ydimCrys1 = fpParGeo->GetCrystalSize()[2]*cm;
    Double_t ydimCrys2 = fpParGeo->GetCrystalSize()[3]*cm;
    Double_t zdimCrys  = fpParGeo->GetCrystalSize()[4]*cm;
   
   Double_t xdimS1 = fpParGeo->GetSupportSize()[0]*cm;
   Double_t xdimS2 = fpParGeo->GetSupportSize()[1]*cm;
   Double_t ydimS1 = fpParGeo->GetSupportSize()[2]*cm;
   Double_t ydimS2 = fpParGeo->GetSupportSize()[3]*cm;
   Double_t zdimS  = fpParGeo->GetSupportSize()[4]*cm;
   
   Double_t xdimM1 = fpParGeo->GetModuleSize()[0]*cm;
   Double_t xdimM2 = fpParGeo->GetModuleSize()[1]*cm;
   Double_t ydimM1 = fpParGeo->GetModuleSize()[2]*cm;
   Double_t ydimM2 = fpParGeo->GetModuleSize()[3]*cm;
   Double_t zdimM  = fpParGeo->GetModuleSize()[4]*cm;
   
   G4Trd* bgo = new G4Trd("Crystal", xdimCrys1, xdimCrys2, ydimCrys1, ydimCrys2, zdimCrys);
   fCrysLog = new G4LogicalVolume(bgo, matBGO, "CrystalLog");

   G4Trd* support = new G4Trd("Support", xdimS1, xdimS2, ydimS1, ydimS2, zdimS);
   G4Trd* support1 = new G4Trd("Support1", xdimM1, xdimM2, ydimM1, ydimM2, zdimM);
   G4SubtractionSolid *sup = new G4SubtractionSolid("sup",support,support1);
   fSupLog = new  G4LogicalVolume(sup, matAl, "SupportLog");

   /// Visualisation attributes
   G4VisAttributes* supportLogAtt = new G4VisAttributes(G4Colour(0.5,0.5,0.5,0.6)); //light grey
   supportLogAtt->SetForceSolid(true);
   fSupLog->SetVisAttributes(supportLogAtt);
   
   G4VisAttributes* crysLogAtt = new G4VisAttributes(G4Colour(0.,0.3,1)); //light blue
   crysLogAtt->SetForceWireframe(true);
   fCrysLog->SetVisAttributes(crysLogAtt);
   
   Int_t crystalsN = 0;
   
   printf("%s\n", fpParGeo->GetConfigTypeGeo().Data());
   if ( fpParGeo->GetConfigTypeGeo().CompareTo("ONE_CRY") == 0 )
      crystalsN = 1;
   else if ( fpParGeo->GetConfigTypeGeo().CompareTo("ONE_MOD") == 0)
      crystalsN = 9;
   else if ( fpParGeo->GetConfigTypeGeo().CompareTo("CENTRAL_DET") == 0)
      crystalsN = fpParGeo->GetCrystalsN()/2;
   else if ( fpParGeo->GetConfigTypeGeo().CompareTo("FULL_DET") == 0)
      crystalsN = fpParGeo->GetCrystalsN();
   else
      G4cout << "Error in GetConfigTypeGeo no " << fpParGeo->GetConfigTypeGeo().Data() << " config available" << G4endl;

   
   G4ThreeVector supZ;
   G4ThreeVector posSup;
   
   for (Int_t iCrys = 0; iCrys < crystalsN; ++iCrys) {
      
      TVector3 posCrys = fpParGeo->GetCrystalPosition(iCrys)*cm;
      TVector3 ang     = fpParGeo->GetCrystalAngle(iCrys)*degree;
      G4RotationMatrix* rotCrys = new G4RotationMatrix;
      rotCrys->rotateX(ang[0]);
      rotCrys->rotateY(ang[1]);
      rotCrys->rotateZ(ang[2]);
      
      if (iCrys % fpParGeo->GetCrystalsNperModule() == 0) {
         supZ.set(0,0,fpParGeo->GetSupportPositionZ()*cm);
         posSup = *(rotCrys)*supZ;
      }
      
      rotCrys->invert(); // inversion
      new G4PVPlacement(rotCrys, G4ThreeVector(posCrys[0], posCrys[1], posCrys[2]), fCrysLog, "Crystal", fBoxLog, false, iCrys);
      
       if (iCrys % fpParGeo->GetCrystalsNperModule() == 0) {
          posSup += G4ThreeVector(posCrys[0], posCrys[1], posCrys[2]);
          new G4PVPlacement(rotCrys, posSup, fSupLog, "Support", fBoxLog, false, iCrys + crystalsN);
       }
   }
   
   printf("PosZ %g\n", fpParGeo->GetSupportPositionZ());
   DefineSensitive();

   return fBoxLog;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void  TCCAgeometryConstructor::DefineSensitive()
{
   // sensitive volume
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   
   G4String calSDname;
   calSDname = fgkCalSDname;
   Info("DefineSensitive()", "Define sensitive for Calorimeter");
  
   TCCAsensitiveDetector* calSensitive = new TCCAsensitiveDetector(calSDname);
   calSensitive->SetCopyLevel(0);
   SDman->AddNewDetector(calSensitive);
   fCrysLog->SetSensitiveDetector(calSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCCAgeometryConstructor::DefineMaxMinDimension()
{

   TVector3 size  = fpParGeo->GetCaloSize();
   Float_t  thick = fpParGeo->GetCrystalThick()+2.0;

   TVector3 minPosition(10e10, 10e10, 10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   
   
   minPosition[0] = -size[1]/2.; maxPosition[0] = +size[1]/2.;
   minPosition[1] = -size[1]/2.; maxPosition[1] = +size[1]/2.;
   minPosition[2] = -thick/2.;   maxPosition[2] = +thick/2.;

   fMinPosition = minPosition*cm;
   fMaxPosition = maxPosition*cm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCCAgeometryConstructor::DefineMaterial()
{
    fpMaterials->CreateG4Material(fpParGeo->GetCrystalMat());
    fpMaterials->CreateG4Material(fpParGeo->GetSupportMat());
}


