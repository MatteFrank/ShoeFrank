
#include "TCITgeometryConstructor.hxx"
#include "TCITsensitiveDetector.hxx"

#include "G4SDManager.hh"
#include "G4String.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4ThreeVector.hh"
#include "G4Element.hh"
#include "G4VisAttributes.hh"
#include "G4NistManager.hh"


#include "TAITparGeo.hxx"

TString TCITgeometryConstructor::fgkItEpiSDname = "ItSD";

using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCITgeometryConstructor::TCITgeometryConstructor(TAVTbaseParGeo* pParGeo)
: TCVTgeometryConstructor(pParGeo)
{
    SetEpiName("itEpiPhy");
    DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCITgeometryConstructor::~TCITgeometryConstructor()
{
   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCITgeometryConstructor::DefineSensitive()
{
   Info("DefineSensitive()", "Define sensitive for Inner Tracker");

   // sensitive volume
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   G4String epiSDname;
   epiSDname = fgkItEpiSDname;
   
   TCITsensitiveDetector* epiSensitive = new TCITsensitiveDetector(epiSDname);
   SDman->AddNewDetector(epiSensitive);
   fEpiLog->SetSensitiveDetector(epiSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4LogicalVolume* TCITgeometryConstructor::Construct()
{
   fBoxVtxLog = TCVTgeometryConstructor::Construct();
   
   // build support
   BuildPlumeSupport();

   // place support
   PlacePlumeSupport();
   
   return fBoxVtxLog;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCITgeometryConstructor::DefineMaterial()
{
   TAITparGeo* parGeo = (TAITparGeo*)fpParGeo;
   fpMaterials->CreateG4Material(parGeo->GetFoamMaterial());
   fpMaterials->CreateG4Material(parGeo->GetKaptonMaterial());
   fpMaterials->CreateG4Material(parGeo->GetEpoxyMaterial());
   fpMaterials->CreateG4Material(parGeo->GetAlMaterial());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCITgeometryConstructor::PlacePlumeSupport()
{
   Int_t halfSensors  = fpParGeo->GetSensorsN()/2;
   TAITparGeo* parGeo = (TAITparGeo*)fpParGeo;

   for(Int_t iSup = 0; iSup < halfSensors; iSup+=4) {
      
      G4ThreeVector origin(0,0,0);
      
      Float_t sign = (iSup > 7) ? +1 : -1;
      origin[0] = sign*parGeo->GetSupportOffset()[0]/2.*cm;
      origin[1] = 0.5*(parGeo->GetSensorPar(iSup).Position(1))*cm + 0.5*(parGeo->GetSensorPar(iSup+halfSensors).Position(1))*cm + 0.5*sign*parGeo->GetSupportOffset()[1]*cm;
      origin[2] = 0.5*(parGeo->GetSensorPar(iSup).Position(2))*cm + 0.5*(parGeo->GetSensorPar(iSup+halfSensors).Position(2))*cm;
   
      new G4PVPlacement(0, origin, fSupportLog, Form("Support%d", iSup+100), fBoxVtxLog, false, iSup+100);
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4LogicalVolume* TCITgeometryConstructor::BuildPlumeSupport()
{
   TAITparGeo* parGeo = (TAITparGeo*)fpParGeo;

   // fetch support material
   const Char_t* matName = parGeo->GetFoamMaterial().Data();
   G4Material*   medFoam = G4NistManager::Instance()->FindOrBuildMaterial(matName);
   
   matName = parGeo->GetKaptonMaterial().Data();
   G4Material*  medKapton = G4NistManager::Instance()->FindOrBuildMaterial(matName);
   
   matName = parGeo->GetAlMaterial().Data();
   G4Material*  medAl = G4NistManager::Instance()->FindOrBuildMaterial(matName);
   
   matName = parGeo->GetEpoxyMaterial().Data();
   G4Material*  medEpoxy = G4NistManager::Instance()->FindOrBuildMaterial(matName);
   
   G4Material* medAir = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");
   
   // Support volume
   G4Box *box = new G4Box("Support", parGeo->GetSupportSize().X()/2.*cm, parGeo->GetSupportSize().Y()/2.*cm, parGeo->GetSupportSize().Z()/2.*cm);
   fSupportLog = new G4LogicalVolume(box, medAir, "Support");
   
   // Foam volume
   G4Box *foamBox = new G4Box("Foam", parGeo->GetSupportSize().X()/2.*cm, parGeo->GetSupportSize().Y()/2.*cm,  parGeo->GetFoamThickness()/2.*cm);
   G4LogicalVolume *foam = new G4LogicalVolume(foamBox, medFoam, "Foam");
   
   G4VisAttributes* foamLog_att = new G4VisAttributes(G4Colour(0.,1.,1.)); // cyan
   foamLog_att->SetForceWireframe(true);
   foam->SetVisAttributes(foamLog_att);
   
   // Kapton1 volume
   G4Box *kapton1Box = new G4Box("Kapton1", parGeo->GetSupportSize().X()/2.*cm, parGeo->GetSupportSize().Y()/2.*cm,  parGeo->GetKaptonThickness()/2.*cm);
   G4LogicalVolume *kapton1 = new G4LogicalVolume(kapton1Box, medKapton, "Kapton1");
   
   G4VisAttributes* kapton1Log_att = new G4VisAttributes(G4Colour(1.,0.5,0.)); // orange
   kapton1Log_att->SetForceWireframe(true);
   kapton1->SetVisAttributes(kapton1Log_att);
   
   // Kapton2 volume
   G4Box *kapton2Box = new G4Box("Kapton2", parGeo->GetSupportSize().X()/2.*cm, parGeo->GetSupportSize().Y()/2.*cm,  parGeo->GetKaptonThickness()*cm); // double thickness
   G4LogicalVolume *kapton2 = new G4LogicalVolume(kapton2Box, medKapton, "Kapton2");

   G4VisAttributes* kapton2Log_att = new G4VisAttributes(G4Colour(1.,0.5,0.)); // orange
   kapton2Log_att->SetForceWireframe(true);
   kapton2->SetVisAttributes(kapton2Log_att);

   // Al volume
   G4Box *alBox = new G4Box("Al", parGeo->GetSupportSize().X()/2.*cm, parGeo->GetSupportSize().Y()/2.*cm,  parGeo->GetAlThickness()/2.*cm);
   G4LogicalVolume *al = new G4LogicalVolume(alBox, medAl, "Al");

   G4VisAttributes* alLog_att = new G4VisAttributes(G4Colour(0.,0.0,1.)); // blue
   alLog_att->SetForceWireframe(true);
   al->SetVisAttributes(alLog_att);

   // Epoxy volume
   G4Box *epoxyBox = new G4Box("Epoxy", parGeo->GetSupportSize().X()/2.*cm, parGeo->GetSupportSize().Y()/2.*cm,  parGeo->GetEpoxyThickness()/2.*cm);
   G4LogicalVolume *epoxy = new G4LogicalVolume(epoxyBox, medEpoxy, "Epoxy");

   G4VisAttributes* epoxyLog_att = new G4VisAttributes(G4Colour(1.,0.0,1.)); // magenta
   epoxyLog_att->SetForceWireframe(true);
   epoxy->SetVisAttributes(epoxyLog_att);

   // placement
   Int_t count = 0;
   
   // foam
    new G4PVPlacement(0, G4ThreeVector(0,0,0),foam, "foam", fSupportLog, false, count++);

   // first kapton layer
   Float_t posZ = (parGeo->GetFoamThickness()/2. + parGeo->GetKaptonThickness()/2.)*cm;
   new G4PVPlacement(0, G4ThreeVector(0,0, posZ), kapton1, "kapton1",  fSupportLog, false, count++);
   new G4PVPlacement(0, G4ThreeVector(0,0,-posZ), kapton1, "kapton1-", fSupportLog, false, count++);
   
   // first layer aluminium
   posZ += (parGeo->GetKaptonThickness()/2. + parGeo->GetAlThickness()/2.)*cm;
   new G4PVPlacement(0, G4ThreeVector(0,0, posZ), al, "al1",  fSupportLog, false, count++);
   new G4PVPlacement(0, G4ThreeVector(0,0,-posZ), al, "al1-", fSupportLog, false, count++);

   // Second kapton layer
   posZ += (parGeo->GetAlThickness()/2. + parGeo->GetKaptonThickness())*cm;
   new G4PVPlacement(0, G4ThreeVector(0,0, posZ), kapton2, "kapton2",  fSupportLog, false, count++);
   new G4PVPlacement(0, G4ThreeVector(0,0,-posZ), kapton2, "kapton2-", fSupportLog, false, count++);

   // second layer aluminium
   posZ += (parGeo->GetKaptonThickness() +  parGeo->GetAlThickness()/2.)*cm;
   new G4PVPlacement(0, G4ThreeVector(0,0, posZ), al, "al2",  fSupportLog, false, count++);
   new G4PVPlacement(0, G4ThreeVector(0,0,-posZ), al, "al2-", fSupportLog, false, count++);

   // Third kapton layer
   posZ += (parGeo->GetAlThickness()/2. + parGeo->GetKaptonThickness()/2.)*cm;
   new G4PVPlacement(0, G4ThreeVector(0,0, posZ), kapton1, "kapton3",  fSupportLog, false, count++);
   new G4PVPlacement(0, G4ThreeVector(0,0,-posZ), kapton1, "kapton3-", fSupportLog, false, count++);

   // layer of epoxy
   posZ += (parGeo->GetKaptonThickness()/2. + parGeo->GetEpoxyThickness()/2.)*cm;
   new G4PVPlacement(0, G4ThreeVector(0,0, posZ), epoxy, "epoxy",  fSupportLog, false, count++);
   new G4PVPlacement(0, G4ThreeVector(0,0,-posZ), epoxy, "epoxy-", fSupportLog, false, count++);
   
   return fSupportLog;
}
