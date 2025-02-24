/*!
 \file TCMSDgeometryConstructor.cxx
 \brief Implementation of TCMSDgeometryConstructor.
*/

#include "TCMSDgeometryConstructor.hxx"
#include "TCMSDsensitiveDetector.hxx"

#include "TAVTparGeo.hxx"
#include "TAGrecoManager.hxx"

#include "G4SDManager.hh"
#include "G4String.hh"

#include "TString.h"

TString TCMSDgeometryConstructor::fgkMsdEpiSDname = "MsdSD";

using namespace CLHEP;

/*!
 \class TCMSDgeometryConstructor
 \brief Building MSD detector geometry
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] pParGeo geometry parameter
TCMSDgeometryConstructor::TCMSDgeometryConstructor(TAVTbaseParGeo* pParGeo)
: TCVTgeometryConstructor(pParGeo)
{
    SetEpiName("msdEpiPhy");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCMSDgeometryConstructor::~TCMSDgeometryConstructor()
{
   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define materials
void TCMSDgeometryConstructor::DefineMaterial()
{
    TString pixMat = fpParGeo->GetPixMaterial();
    G4double pixRho = fpParGeo->GetPixMatDensity();
    fpMaterials->CreateG4Material(pixMat,pixRho);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define sensitive detector volume
void TCMSDgeometryConstructor::DefineSensitive()
{
   // Putting here message
   InfoMcMsg("Construct()", "Construct Micro Strip Detector");
  
   InfoMcMsg("DefineSensitive()", "Define sensitive for Micro Strip Detector");

   // sensitive volume
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   G4String epiSDname;
   epiSDname = fgkMsdEpiSDname;
   
   TCMSDsensitiveDetector* epiSensitive = new TCMSDsensitiveDetector(epiSDname);
   SDman->AddNewDetector(epiSensitive);
   fEpiLog->SetSensitiveDetector(epiSensitive);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Define the dimension of the detecteor envelop
void TCMSDgeometryConstructor::DefineMaxMinDimension()
{
   TVector3 size = fpParGeo->GetTotalSize();
   Int_t nSens   = fpParGeo->GetSensorsN();

   TVector3 minPosition( 10e10,  10e10,  10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   TVector3 posAct(0, 0, 0);

   for (Int_t iS = 0; iS < nSens; iS++) {
      posAct = fpParGeo->GetSensorPar(iS).Position;
      
      for(Int_t i = 0; i < 3; i++) {
         minPosition[i] = (minPosition[i] <= posAct[i] ) ? minPosition[i] : posAct[i];
         maxPosition[i] = (maxPosition[i] >= posAct[i] ) ? maxPosition[i] : posAct[i];
      }
   }
   
   minPosition[0] -= size[0]/2.;  maxPosition[0] += size[0]/2.;
   minPosition[1] -= size[1]/2.;  maxPosition[1] += size[1]/2.;
   minPosition[2] -= size[2]/2.;  maxPosition[2] += size[2]/2.;
   
   fMinPosition = minPosition*cm;
   fMaxPosition = maxPosition*cm;
}
