//myEpiSensitiveDetector

#include "TCGbaseSensitiveDetector.hxx"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4VTouchable.hh"

#include "TCGmcHit.hxx"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCGbaseSensitiveDetector::TCGbaseSensitiveDetector(G4String name)
: G4VSensitiveDetector(name),
  fCopyLevel(1)
{
   collectionName.insert(name);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCGbaseSensitiveDetector::~TCGbaseSensitiveDetector()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCGbaseSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
   //invoked at the end of event but before the routine TAGeventAction::EndoOfAction
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCGbaseSensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
   fCollections = new TCGmcCollections(GetName(),collectionName[0]);
   G4int id     = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
   HCE->AddHitsCollection(id, fCollections);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool TCGbaseSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
   if(aStep->GetTotalEnergyDeposit() <= 0.) return false; //no energy deposited
   
   TCGmcHit* newHit = new TCGmcHit();
   
   FillHits(aStep, newHit);
   
   fCollections->insert(newHit);
   
   return true;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCGbaseSensitiveDetector::FillHits(G4Step* aStep, TCGmcHit* newHit)
{
   G4double edep = aStep->GetTotalEnergyDeposit();// in MeV
   const G4TouchableHandle touchable        = aStep->GetPreStepPoint()->GetTouchableHandle();
   const G4ParticleDefinition* pDefinition  = aStep->GetTrack()->GetParticleDefinition();
   G4int idTouchedSensor                    = touchable->GetCopyNumber(fCopyLevel);
   G4VPhysicalVolume* physVol               = touchable->GetVolume(0);
   
   //Set id and flag and type of particle
   newHit->SetFlagUsed(-1);
   newHit->SetSensorId(idTouchedSensor);
   newHit->SetlocPos(physVol->GetFrameTranslation());
   newHit->SetTrackId (aStep->GetTrack()->GetTrackID());
   newHit->SetParentId (aStep->GetTrack()->GetParentID());
   newHit->SetMomentumDir(aStep->GetTrack()->GetMomentumDirection());
   newHit->SetKineticEnergy(aStep->GetTrack()->GetKineticEnergy());
   newHit->SetGlobalTime(aStep->GetTrack()->GetGlobalTime());
   newHit->SetLocalTime(aStep->GetTrack()->GetLocalTime());

    newHit->SetPosVertex(aStep->GetTrack()->GetVertexPosition());//E' Se on globaali kuten esimerkiksi Z
   newHit->SetParticleName(pDefinition->GetParticleName());
   newHit->SetParticlePDG(pDefinition->GetPDGEncoding ());
   newHit->SetZparticle(pDefinition->GetAtomicNumber());
   newHit->SetNnucleons(pDefinition->GetAtomicMass());
   newHit->SetMass(pDefinition->GetPDGMass()); //in MEV
   newHit->SetCharge(pDefinition->GetPDGCharge());
   
   newHit->SetEdep(edep);
   newHit->SetEinput(aStep->GetPreStepPoint()->GetKineticEnergy());
   
   newHit->SetPosIn(aStep->GetPreStepPoint()->GetPosition());
   newHit->SetPosOut(aStep->GetPostStepPoint()->GetPosition());
   
   newHit->SetMomIn(aStep->GetPreStepPoint()->GetMomentum());
   newHit->SetMomOut(aStep->GetPostStepPoint()->GetMomentum());

}
