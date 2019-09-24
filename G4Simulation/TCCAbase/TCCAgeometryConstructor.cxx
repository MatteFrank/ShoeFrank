
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

    Float_t xdimCrys1 = fpParGeo->GetCrystalBotBase()*cm;
    Float_t xdimCrys2 = fpParGeo->GetCrystalTopBase()*cm;
    Float_t ydimCrys1 = xdimCrys1; // assume squart
    Float_t ydimCrys2 = xdimCrys2;
    Float_t zdimCrys  = fpParGeo->GetCrystalLength()*cm;
    Float_t space = fpParGeo->GetDelta()*cm;

    Int_t modulesN = fpParGeo->GetModulesN();
    Int_t crystalsN = 9;

    // Calorimeter box
   G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");
   G4Material* air    = G4NistManager::Instance()->FindOrBuildMaterial("Air");
   G4Material* matBGO = G4NistManager::Instance()->FindOrBuildMaterial("BGO");
   G4Material* matAl  = G4NistManager::Instance()->FindOrBuildMaterial("Al");
   G4Box* boxCal = new G4Box("boxCal", 0.5*fSizeBoxCal.X(), 0.5*fSizeBoxCal.Y(), fSizeBoxCal.Z());
   fBoxLog = new G4LogicalVolume(boxCal, vacuum, "boxCaloLog");
   fBoxLog->SetVisAttributes(G4VisAttributes::Invisible);

    G4Trd* bgo = new G4Trd("Crystal", xdimCrys1, xdimCrys2, ydimCrys1, ydimCrys2, zdimCrys);
    fCalLog = new G4LogicalVolume(bgo, matBGO, "CrystalLog");

    Float_t xdimS1 = fpParGeo->GetSupportBotBase()*cm;
    Float_t xdimS2 = fpParGeo->GetSupportTopBase()*cm;
    Float_t ydimS1 = xdimS1;
    Float_t ydimS2 = xdimS2;
    Float_t zdimS  = fpParGeo->GetSupportLength()*cm;

    Float_t xdimMod1 = (crystalsN*(xdimCrys1+space)/3.0)+(space/2.) ;
    Float_t xdimMod2 = xdimS2+(space/2.) ;
    Float_t ydimMod1 = xdimMod1+(space/2.) ;
    Float_t ydimMod2 = xdimMod2+(space/2.) ;
    Float_t zdimMod  = zdimCrys+space ;
    G4Trd* moduleCal = new G4Trd("moduleCal", xdimMod1,xdimMod2,ydimMod1,ydimMod2, zdimMod);
    fModLog = new G4LogicalVolume(moduleCal, air, "ModuleLog");

   G4Trd* support = new G4Trd("Support", xdimS1, xdimS2, ydimS1, ydimS2, zdimS);
    G4SubtractionSolid *sup = new G4SubtractionSolid("sup",support,bgo);

       for (Int_t i = 0; i < modulesN; ++i) {
           for(Int_t j=0 ; j<crystalsN ; ++j){
               TVector3 pos = fpParGeo->GetCrystalPosition(j)*cm;
               TVector3 ang = fpParGeo->GetCrystalAngle(j);
               G4RotationMatrix* rotCrys = new G4RotationMatrix;
               rotCrys->rotateX(ang[0]*degree);
               rotCrys->rotateY(ang[1]*degree);
               rotCrys->rotateZ(ang[2]*degree);
               rotCrys->invert(); // inversion
               if(i==0){
                   if(j==0) sup = new G4SubtractionSolid("sup",support,bgo,rotCrys, G4ThreeVector(pos[0], pos[1],pos[2]-(zdimCrys/2.0)));
                   else sup = new G4SubtractionSolid("sup",sup,bgo,rotCrys, G4ThreeVector(pos[0], pos[1], pos[2]-(zdimCrys/2.0)));
               }
              new G4PVPlacement(rotCrys, G4ThreeVector(pos[0], pos[1], pos[2]), fCalLog, "Crystals", fModLog, false, i*crystalsN + j);
           }
           if(i==0) fSupLog = new G4LogicalVolume(sup, air, "SupportLog");
           TVector3 posMod = fpParGeo->GetModulePosition(i)*cm;
           TVector3 angMod = fpParGeo->GetModuleAngle(i);
           G4RotationMatrix* rotMod = new G4RotationMatrix;
           rotMod->rotateX(angMod[0]*degree);
           rotMod->rotateY(angMod[1]*degree);
           rotMod->rotateZ(angMod[2]*degree);
           rotMod->invert(); // inversion
           new G4PVPlacement(rotMod, G4ThreeVector(posMod[0], posMod[1], posMod[2]), fModLog, "Calo", fBoxLog, false, i + 1);
           TVector3 ang = fpParGeo->GetCrystalAngle(crystalsN-1);
           G4RotationMatrix* rotSup = new G4RotationMatrix;
           rotSup->rotateX(ang[0]*degree);
           rotSup->rotateY(ang[1]*degree);
           rotSup->rotateZ(ang[2]*degree);
           rotSup->invert(); // inversion
           new G4PVPlacement(rotSup, G4ThreeVector(0.0, 0.0,zdimCrys/2.0), fSupLog, "Supports", fModLog, false, 0);
       }


    /// Visualisation attributes
    G4VisAttributes* bgoLogAtt = new G4VisAttributes(G4Colour(0.,0.3,1)); //light blue
    bgoLogAtt->SetForceWireframe(true);
    fCalLog->SetVisAttributes(bgoLogAtt);

    G4VisAttributes* supportLogAtt = new G4VisAttributes(G4Colour(0.5,0.5,0.5,0.6)); //light grey
    supportLogAtt->SetForceSolid(true);
    fSupLog->SetVisAttributes(supportLogAtt);

    G4VisAttributes* modLogAtt = new G4VisAttributes(G4Colour(0.5,0.5,0.5,0.2)); //grey
    modLogAtt->SetForceWireframe(true);
    fModLog->SetVisAttributes(modLogAtt);

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
   fCalLog->SetSensitiveDetector(calSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCCAgeometryConstructor::DefineMaxMinDimension()
{

   TVector3 size  = fpParGeo->GetCaloSize();
   Float_t  thick = fpParGeo->GetCrystalLength()+2.0;

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


