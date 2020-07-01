
#include "TCVTgeometryConstructor.hxx"
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
#include "TCVTsensitiveDetector.hxx"
#include "TAVTparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "G4RunManager.hh"

#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"

//The origin is the center of target

TString TCVTgeometryConstructor::fgkCmosName    = "cmosDetector"; 
TString TCVTgeometryConstructor::fgkPixName     = "pixPhy";
Float_t TCVTgeometryConstructor::fgDefSmearAng  = 0.01;  // in degree
Float_t TCVTgeometryConstructor::fgDefSmearPos  = 5.0e-4;   // in micron
Bool_t  TCVTgeometryConstructor::fgSmearFlag    = false;

TString TCVTgeometryConstructor::fgkVtxEpiSDname = "VtxSD";
TString TCVTgeometryConstructor::fgkBmEpiSDname  = "BmSD";

using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCVTgeometryConstructor::TCVTgeometryConstructor(TAVTbaseParGeo* pParGeo, Bool_t bmFlag)
: TCGbaseConstructor("TCVTgeometryConstructor", "1.0"),
  fCmosLog(0x0),
  fEpiLog(0x0),
  fPixLog(0x0),
  fEpiName("vtxEpiPhy"),
  fPixPhy(0x0),
  fpParGeo(pParGeo),
  fBmFlag(bmFlag)
{
   DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCVTgeometryConstructor::~TCVTgeometryConstructor()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCVTgeometryConstructor::BuildSensor()
{
   
   G4Material* Si      = G4NistManager::Instance()->FindOrBuildMaterial("Si"); //for sensitive and bulk CMOS
   G4Material* Mixture = G4NistManager::Instance()->FindOrBuildMaterial("SiO2/Al"); //for pixels
   
   TVector3 sizeCmos(0,0,0); //(21.2mm, 13.6 mm, 0.05 mm)
   TVector3 sizeEpi(0,0,0);
   TVector3 sizePix(0,0,0);
   
   for(Int_t i = 0; i< 3; ++i) {
      sizeCmos[i] = fpParGeo->GetTotalSize()[i]*cm;
      sizeEpi[i]  = fpParGeo->GetEpiSize()[i]*cm; //21.2*mm;10.6*mm;0.014*mm;*/
   }
   
   sizePix.SetX(fpParGeo->GetEpiSize().X()*cm); //21.2*mum;
   sizePix.SetY(fpParGeo->GetEpiSize().Y()*cm);// 10.6*mum;
   sizePix.SetZ(fpParGeo->GetPixThickness()*cm);//0.0064*mm;
   
   G4double size_tot_epi_z = sizeEpi.Z() + sizePix.Z();
   
   G4Box* cmos = new G4Box("cmos",   0.5*sizeCmos.X(),    0.5*sizeCmos.Y(),    0.5*sizeCmos.Z());
   G4Box* epi  = new G4Box("epi",    0.5*sizeEpi.X(),     0.5*sizeEpi.Y(),     0.5*size_tot_epi_z);
   G4Box* pix  = new G4Box("pix",    0.5*sizePix.X(),     0.5*sizePix.Y(),     0.5*sizePix.Z());
   
   // Total size cmos
   G4VisAttributes* cmosLog_att = new G4VisAttributes(G4Colour(1.,0.,0.)); // red
   cmosLog_att->SetForceWireframe(true);
   
   fCmosLog = new G4LogicalVolume(cmos, Si, "cmosLog"); //for CMOS vertexing
   fCmosLog->SetVisAttributes(cmosLog_att);
   
   // Epi
   G4VisAttributes* epiLog_att = new G4VisAttributes(G4Colour(0.,1.,1.)); //cyan
   epiLog_att->SetForceSolid(true);
   fEpiLog   = new G4LogicalVolume(epi, Si, "epiLog"); //epitaxial layer: fill with "Sensitive" silicon  -- for CMOS
   fEpiLog->SetVisAttributes(epiLog_att);
   
   
   //epi placement
   G4double epi_tx = +(sizeCmos.X() - sizeEpi.X())*0.5;   // useless always zero
   G4double epi_ty = +(sizeCmos.Y() - sizeEpi.Y())*0.5;   // ok, epitaxial layer at zero position
   G4double epi_tz =  (sizeCmos.Z() - size_tot_epi_z)*0.5; // ok, epitaxial layer at bottom of cmos
   fEpiPhy   = new G4PVPlacement(0, G4ThreeVector(epi_tx, epi_ty, epi_tz),fEpiLog, GetEpiName(), fCmosLog, false, 0);
   
   // Pixel
   fPixLog = new G4LogicalVolume(pix, Mixture, "pixLog");
   fPixLog->SetVisAttributes(G4VisAttributes::Invisible);
   
   //Pixel placement
   G4double pix_tz = +0.5*(size_tot_epi_z - sizePix.Z()); //ok
   fPixPhy   = new G4PVPlacement(0, G4ThreeVector(0, 0, pix_tz), fPixLog, GetPixName(), fEpiLog, false, 0);
   
   DefineSensitive();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void  TCVTgeometryConstructor::DefineSensitive()
{
   // sensitive volume
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   
   G4String epiSDname;
   if (fBmFlag) {
      epiSDname = fgkBmEpiSDname;
      Info("DefineSensitive()", "Define sensitive for BM (Vertex)");
   } else {
      epiSDname = fgkVtxEpiSDname;
      Info("DefineSensitive()", "Define sensitive for Vertex");
   }
   
   TCVTsensitiveDetector* epiSensitive = new TCVTsensitiveDetector(epiSDname);
   SDman->AddNewDetector(epiSensitive);
   fEpiLog->SetSensitiveDetector(epiSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4LogicalVolume* TCVTgeometryConstructor::Construct()
{
   //Definition of dimension boxes
   DefineMaxMinDimension();
   
   for(Int_t i = 0; i< 3; ++i)
      fSizeBoxVtx[i] = (fMaxPosition[i] - fMinPosition[i]);
   fSizeBoxVtx[2] += fpParGeo->GetTotalSize()[2]*cm;

   // Vtx box
   G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");
   G4Box* boxVtx = new G4Box("boxVtx", fSizeBoxVtx.X()/2., fSizeBoxVtx.Y()/2., fSizeBoxVtx.Z()/2.);
   fBoxVtxLog = new G4LogicalVolume(boxVtx, vacuum, "boxVtxLog");
   fBoxVtxLog->SetVisAttributes(G4VisAttributes::Invisible);
   
   // Build sensor
   BuildSensor();
   
   // Build sensor
   PlaceSensor();
   
   return fBoxVtxLog;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCVTgeometryConstructor::PlaceSensor()
{
    Int_t nSensor = fpParGeo->GetSensorsN();

   for(Int_t iSensor = 0; iSensor< nSensor; ++iSensor) {
      
      G4ThreeVector rotation(0,0,0);
      for (Int_t c = 0; c < 3; ++c)
         rotation[c] = (fpParGeo->GetSensorPar(iSensor).Tilt(c));
      
      G4RotationMatrix rot;
      rot.rotateX(rotation[0]);
      rot.rotateY(rotation[1]);
      rot.rotateZ(rotation[2]);
      rot.invert();
      
      //Rotation Shift
      G4ThreeVector shift(fpParGeo->GetEpiOffset()[0], fpParGeo->GetEpiOffset()[1], fpParGeo->GetEpiOffset()[2]);
      shift *= cm/2;
      
      G4ThreeVector origin(0,0,0);
      for (Int_t c = 0; c < 3; ++c)
         origin[c]  = (fpParGeo->GetSensorPar(iSensor).Position(c))*cm;
      
      Float_t signY = fpParGeo->GetSensorPar(iSensor).IsReverseX ? +1. : -1.;
      Float_t signX = fpParGeo->GetSensorPar(iSensor).IsReverseY ? +1. : -1.;
      origin[0] += signX*shift[0];
      origin[1] += signY*shift[1];

      
      if (fgSmearFlag) {
         origin[0]    += gRandom->Uniform(-fgDefSmearPos, fgDefSmearPos)*cm;
         origin[1]    += gRandom->Uniform(-fgDefSmearPos, fgDefSmearPos)*cm;
         rotation[2]  += gRandom->Uniform(-fgDefSmearAng, fgDefSmearAng);
      }

      //Take tilt of alignement
      G4RotationMatrix* rot2 = new G4RotationMatrix;
      rot2->rotateX(rotation[0]);
      rot2->rotateY(rotation[1]);
      rot2->rotateZ(rotation[2]-fpParGeo->GetSensorPar(iSensor).TiltW);
      rot2->invert();
      new G4PVPlacement(rot2, origin, fCmosLog, "BulkCMOSPhy", fBoxVtxLog, false, iSensor);
   }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCVTgeometryConstructor::DefineMaterial()
{
    TString pixMat = fpParGeo->GetPixMaterial();
    TString pixDens = fpParGeo->GetPixMatDensities();
    TString pixProp = fpParGeo->GetPixMatProp();
    G4double pixRho = fpParGeo->GetPixMatDensity();
    fpMaterials->CreateG4Material(fpParGeo->GetEpiMaterial());
    fpMaterials->CreateG4Mixture(pixMat,pixDens,pixProp,pixRho);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCVTgeometryConstructor::DefineMaxMinDimension()
{
   TVector3 delta(0, fpParGeo->GetTotalSize()[1] - fpParGeo->GetEpiSize()[1], 0);
   TVector3 sizeEpi = fpParGeo->GetEpiSize();
   
   TVector3 posAct(0, 0, 0);
   Int_t nSens = fpParGeo->GetSensorsN();
   TVector3 EnvDim(0,0,0);
   TVector3 shift(0,0,0);
   
   TVector3 minPosition(10e10, 10e10, 10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   
   for (Int_t iS = 0; iS < nSens; iS++) {
      posAct = fpParGeo->GetSensorPar(iS).Position;
      shift  = fpParGeo->Sensor2DetectorVect(iS, delta) + fpParGeo->Sensor2DetectorVect(iS, sizeEpi)*0.5;
      
      for(Int_t i = 0; i < 3; i++) {
         shift[i] = TMath::Abs(shift[i]);
         minPosition[i] = (minPosition[i] <= posAct[i] - shift[i]) ? minPosition[i] : posAct[i] - shift[i];
         maxPosition[i] = (maxPosition[i] >= posAct[i] + shift[i]) ? maxPosition[i] : posAct[i] + shift[i];
      }
   }
   fMinPosition = minPosition*cm;
   fMaxPosition = maxPosition*cm;
   
   if (TMath::Abs(fMinPosition.Z()) > TMath::Abs(fMaxPosition.Z()))
      fMaxPosition[2] = -fMinPosition[2];
   
   if (TMath::Abs(fMaxPosition.Z()) > TMath::Abs(fMinPosition.Z()))
      fMinPosition[2] = -fMaxPosition[2];

   fMinPosition.Print();
   fMaxPosition.Print();
}

