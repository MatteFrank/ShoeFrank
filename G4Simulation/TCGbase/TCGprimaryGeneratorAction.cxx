/*!
 \file TCGprimaryGeneratorAction.cxx
 \brief Implementation of TCGprimaryGeneratorAction.
*/

#include "TCGprimaryGeneratorAction.hxx"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "globals.hh"
#include "TRandom.h"
#include "TMath.h"

using namespace CLHEP;

/*!
 \class TCGprimaryGeneratorAction
 \brief  Primary generation action for FOOT
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
TCGprimaryGeneratorAction::TCGprimaryGeneratorAction(TAGparGeo *pParGeo)
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(new G4ParticleGun(1)),
  fpParGeo(pParGeo),
  fHasPartDefined(false)
{
   G4double A      = fpParGeo->GetBeamPar().AtomicMass;
   G4double energy = fpParGeo->GetBeamPar().Energy;
   fParticleGun->SetParticleEnergy(A*energy*GeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCGprimaryGeneratorAction::~TCGprimaryGeneratorAction()
{
   delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Generate primary ions for a given event
//!
//! \param[in] anEvent a given event
void TCGprimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
   // particle definition
   if (!fHasPartDefined) {
      G4int    Z = fpParGeo->GetBeamPar().AtomicNumber;
      G4double A = fpParGeo->GetBeamPar().AtomicMass;
      
      if (Z == 1 && A == 1.) {
         G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
         G4ParticleDefinition* proton   = particleTable->FindParticle("proton");
         fParticleGun->SetParticleDefinition(proton);
         
      } else {
         G4double ionCharge        = Z*eplus;
         G4double excitEnergy      = 0.*keV;
         G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
         fParticleGun->SetParticleDefinition(ion);
         fParticleGun->SetParticleCharge(ionCharge);
      }
      fHasPartDefined = true;
   }
   
   // beam position
   G4double beamPosX  = fpParGeo->GetBeamPar().Position[0]*cm; //convert in cm
   G4double beamPosY  = fpParGeo->GetBeamPar().Position[1]*cm;
   G4double beamPosZ  = fpParGeo->GetBeamPar().Position[2]*cm;
   G4double beamSize  = fpParGeo->GetBeamPar().Size*cm;
   TString  beamShape = fpParGeo->GetBeamPar().Shape;
   
   // taken into account a circular beam
   if (beamSize > 0 && beamShape.Contains("Gaussian")) {
      if(anEvent->GetEventID() == 0) G4cout<<"Shoot Gaussian with FWHM "<<beamSize<<" mm "<<G4endl;
      
      G4double dx = gRandom->Gaus(0, beamSize/2.35);
      G4double dy = gRandom->Gaus(0, beamSize/2.35);
      beamPosX += dx;
      beamPosY += dy;
      
   } else if(beamSize > 0 && beamShape.Contains("Uniform")) {
      if(anEvent->GetEventID() == 0) G4cout<<"Shoot Uniform between "<<-beamSize<<" and "<<beamSize<<" mm "<<G4endl;
      G4double dx = gRandom->Uniform(-beamSize , beamSize );
      G4double dy = gRandom->Uniform(-beamSize , beamSize );
      beamPosX += dx;
      beamPosY += dy;
      
      
   } else if(beamSize < 0) {
      if(anEvent->GetEventID() == 0) G4cout<<"Shoot in "<<beamPosX<<" "<<beamPosY<<" mm"<<G4endl;
   } else {
      if(anEvent->GetEventID() == 0) {
         G4cout<<"Dirac beam at "<<beamPosX<<" "<<beamPosY<<" mm"<<G4endl;
         G4cout<<"Beam Shape "<<beamShape << G4endl;
      }
   }
   
   G4ThreeVector position(beamPosX, beamPosY, beamPosZ);
   fParticleGun->SetParticlePosition(position);

   // beam angular spread
   G4double angSpreadX = fpParGeo->GetBeamPar().AngSpread[0];
   G4double angSpreadY = fpParGeo->GetBeamPar().AngSpread[1];
   
   G4double spreadX    = gRandom->Gaus(0, TMath::DegToRad()*angSpreadX);
   G4double spreadY    = gRandom->Gaus(0, TMath::DegToRad()*angSpreadY);
   
   G4ThreeVector momentumDirection(TMath::Tan(spreadX), TMath::Tan(spreadY), 1);
   fParticleGun->SetParticleMomentumDirection(momentumDirection);
   
   fParticleGun->GeneratePrimaryVertex(anEvent);
}


