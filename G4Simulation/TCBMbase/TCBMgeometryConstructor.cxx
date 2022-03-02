
/*! \file TCBMgeometryConstructor.cxx
 \brief   Implementation of TCBMgeometryConstructor.
 */

#include "TCBMgeometryConstructor.hxx"

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

#include "TABMparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TCGmaterials.hxx"
#include "TCBMsensitiveDetector.hxx"

#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"

using namespace CLHEP;

TString TCBMgeometryConstructor::fgkBmSDname  = "bmSD";

/*! \class TCBMgeometryConstructor
 \brief Beam monitor geometry construction
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Default constructor
//!
//! \param[in] pParGeo geometry parameter
TCBMgeometryConstructor::TCBMgeometryConstructor(TABMparGeo* pParGeo)
: TCGbaseConstructor("TCBMgeometryConstructor", "1.0"),
  fBoxLog(0x0),
  fLayerLog(0x0),
  fpParGeo(pParGeo)
{
   DefineMaxMinDimension();
   DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Default destructor
TCBMgeometryConstructor::~TCBMgeometryConstructor()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Get number of layers
void TCBMgeometryConstructor::GetLayersN()
{
    fpParGeo->GetLayersN();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Construct detector
G4LogicalVolume* TCBMgeometryConstructor::Construct()
{
   InfoMcMsg("Construct()", "Construct Beam Monitor");

    Float_t shieldThick = fpParGeo->GetShieldThick()*cm;

   for(Int_t i = 0; i< 3; ++i)
      fSizeBoxBm[i] = (fMaxPosition[i] - fMinPosition[i]) + shieldThick;
   
   // BM box
   G4Material* matSense = G4NistManager::Instance()->FindOrBuildMaterial("W");
   G4Material* matField = G4NistManager::Instance()->FindOrBuildMaterial("Al");
   G4Material* matGas   = G4NistManager::Instance()->FindOrBuildMaterial("Ar/CO2");
   G4Material* matFoil  = G4NistManager::Instance()->FindOrBuildMaterial("Mylar");

   Float_t senseRad     = fpParGeo->GetSenseRad()*cm;
   Float_t fieldRad     = fpParGeo->GetFieldRad()*cm;
   Float_t width        = fpParGeo->GetWidth()*cm - shieldThick;
   Float_t deltaLayer   = fpParGeo->GetDeltaPlane()*cm;
   Float_t foilThick    = fpParGeo->GetFoilThick()*cm;

   Int_t layersN        = fpParGeo->GetLayersN();
   Int_t wiresN         = fpParGeo->GetWiresN();
   Int_t cellsN         = fpParGeo->GetCellsN();

    /// Warning ! cell height and width are already divided by two in TABMdetector.map
   Float_t cellHeight   = fpParGeo->GetCellHeight()*cm;
   Float_t cellWidth    = fpParGeo->GetCellWidth()*cm;

   //logical
    G4Box* box      = new G4Box("box", 0.5*fSizeBoxBm.X(), 0.5*fSizeBoxBm.Y(), 0.5*fSizeBoxBm.Z());
    G4Box* gazBm = new G4Box("gazBM",0.5*fSizeBoxBm.X()-shieldThick, 0.5*fSizeBoxBm.Y()-shieldThick, 0.5*fSizeBoxBm.Z()+(foilThick/2.));
    fBoxLog         = new G4LogicalVolume(box, matField, "boxBmLog");
    G4LogicalVolume* gazLog = new G4LogicalVolume(gazBm, matGas, "gazLog");
    new G4PVPlacement(0, G4ThreeVector(0.0,0.0,0.0), gazLog, "Gaz", fBoxLog, false,0);

    G4VisAttributes* bmLog_att = new G4VisAttributes(G4Colour(0.5,0.5,0.5,0.2)); //grey
    bmLog_att->SetForceSolid(true);
    fBoxLog->SetVisAttributes(bmLog_att);

    G4Box* foilBm = new G4Box("foilBm",0.5*fSizeBoxBm.X()-shieldThick,0.5*fSizeBoxBm.Y()-shieldThick,foilThick/2.);
    G4LogicalVolume* foilLog = new G4LogicalVolume(foilBm, matFoil, "foilLog");
    new G4PVPlacement(0, G4ThreeVector(0.0,0.0,-gazBm->GetZHalfLength()), foilLog, "Foil", gazLog, false,0);
    new G4PVPlacement(0, G4ThreeVector(0.0,0.0,gazBm->GetZHalfLength()), foilLog, "Foil", gazLog, false,0);

    G4Box* layerBm = new G4Box("layerBm",width/2.,cellsN*cellWidth+senseRad,cellHeight+senseRad);
    fLayerLog = new G4LogicalVolume(layerBm,matGas,"layerBmLog");

   G4Tubs* senseWire    = new G4Tubs("SenseWire", 0, senseRad, width/2., 0, 360.);
   G4LogicalVolume* senseWireLog = new G4LogicalVolume(senseWire, matSense, "SenseWireLog");

   G4Tubs* fieldWire    = new G4Tubs("FieldWire", 0, fieldRad, width/2., 0, 360.);
   G4LogicalVolume* fieldWireLog = new G4LogicalVolume(fieldWire, matField, "FieldWireLog");

   G4VisAttributes* senseLog_att = new G4VisAttributes(G4Colour(1.,0.,0.)); // red
   senseWireLog->SetVisAttributes(senseLog_att);
   
   G4VisAttributes* fieldLog_att = new G4VisAttributes(G4Colour(0.,0.,1)); // blue
   fieldWireLog->SetVisAttributes(fieldLog_att);

    G4VisAttributes* foilLog_att = new G4VisAttributes(G4Colour(0.,0.3,0.3)); // green
    foilLog->SetVisAttributes(foilLog_att);

    G4VisAttributes* layerLog_att = new G4VisAttributes(G4Colour(0.5,0.0,0.5));
    fLayerLog->SetVisAttributes(layerLog_att);

    G4VisAttributes* gazLog_att = new G4VisAttributes(G4Colour(0.3,0.,0.));
    gazLog->SetVisAttributes(gazLog_att);

   G4RotationMatrix* rotX = new G4RotationMatrix;
   rotX->rotateZ(0);
   rotX->rotateY(90*TMath::DegToRad());
   rotX->rotateX(90*TMath::DegToRad());
   rotX->invert(); // inversion
   
   G4RotationMatrix* rotY = new G4RotationMatrix;
   rotY->rotateZ(0);
   rotY->rotateX(0);
   rotY->rotateY(90*TMath::DegToRad());
   rotY->invert(); // inversion

    G4RotationMatrix* rotZ = new G4RotationMatrix;
    rotZ->rotateZ(90*TMath::DegToRad());
    rotZ->rotateX(0);
    rotZ->rotateY(0);

   TVector3 delta = fpParGeo->GetDelta()*cm;
   Int_t copy = 0;
    Int_t viewN = 2 ;
    Float_t zTotLayers = (layersN*viewN-1)*(2.0*cellHeight + deltaLayer);
    Float_t shift = 0.0;

    /// positioning the wires inside the layer logical volume
    for(Int_t k=0 ; k<cellsN ; ++k){
        for (Int_t l = 0; l < (Int_t)wiresN/cellsN; ++l) {
            TVector3 pos(0.0,(l-cellsN)*cellWidth,(k-1)*cellHeight);
            if((k==1) && (l%2!=0)) new G4PVPlacement(rotX, G4ThreeVector(pos[0], pos[1], pos[2]), senseWireLog, "BM", fLayerLog, false,0);
            else  new G4PVPlacement(rotX, G4ThreeVector(pos[0], pos[1], pos[2]), fieldWireLog, "BM", fLayerLog, false,0);
        }
    }
    /// positioning the layers inside the BM box
    for (Int_t i = 0; i < viewN; ++i) {
        for(Int_t j=0 ; j<layersN ; ++j){
            copy = i + (2*j);
            if(j%2==0) shift = -(delta[1]-(cellsN*cellWidth));
            else shift = delta[1]-(cellsN*cellWidth);
            if(i==1) new G4PVPlacement(rotZ,G4ThreeVector(shift,0.0,(-zTotLayers/2.0)+(i+2*j)*(deltaLayer+(2.0*cellHeight))),fLayerLog,"layer",gazLog,false,copy);
            else new G4PVPlacement(0,G4ThreeVector(0.0,shift,(-zTotLayers/2.0)+copy*(deltaLayer+(2.0*cellHeight))),fLayerLog,"layer",gazLog,false,copy);
        }
    }
   DefineSensitive();

   return fBoxLog;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define sensitive detector volume
void  TCBMgeometryConstructor::DefineSensitive()
{
   // sensitive volume
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   
   G4String bmSDname;
   bmSDname = fgkBmSDname;
   InfoMcMsg("DefineSensitive()", "Define sensitive for Beam Monitor");
  
   TCBMsensitiveDetector* bmSensitive = new TCBMsensitiveDetector(bmSDname);
   bmSensitive->SetCopyLevel(0);
   SDman->AddNewDetector(bmSensitive);
   fLayerLog->SetSensitiveDetector(bmSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define the dimension of the detecteor envelop
void TCBMgeometryConstructor::DefineMaxMinDimension()
{

   TVector3 size  = fpParGeo->GetSide();
   
   TVector3 minPosition(10e10, 10e10, 10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   
   
   minPosition[0] = -size[0]/2.; maxPosition[0] = +size[0]/2.;
   minPosition[1] = -size[1]/2.; maxPosition[1] = +size[1]/2.;
   minPosition[2] = -size[2]/2.; maxPosition[2] = +size[2]/2.;

   fMinPosition = minPosition*cm;
   fMaxPosition = maxPosition*cm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define materials
void TCBMgeometryConstructor::DefineMaterial()
{
    TString gasMat    = fpParGeo->GetGasMixture();
    TString gasProp   = fpParGeo->GetGasProp();
    TString gasDens   = fpParGeo->GetGasDensities();
    G4double gasRho = fpParGeo->GetGasRho();

    fpMaterials->CreateG4Material(fpParGeo->GetSenseMat());
    fpMaterials->CreateG4Material(fpParGeo->GetFieldMat());
    fpMaterials->CreateG4Material(fpParGeo->GetFoilMat());
    fpMaterials->CreateG4Mixture(gasMat,gasDens,gasProp,gasRho);
}


