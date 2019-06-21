
#include "TCITgeometryConstructor.hxx"
#include "TCITsensitiveDetector.hxx"

#include "G4SDManager.hh"
#include "G4String.hh"

TString TCITgeometryConstructor::fgkItEpiSDname = "ItSD";

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCITgeometryConstructor::TCITgeometryConstructor(TAVTbaseParGeo* pParGeo)
: TCVTgeometryConstructor(pParGeo)
{
    SetEpiName("itEpiPhy");
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
