/*!
 \file TCGmcHit.cxx
 \brief Implementation of TCGmcHit.
 */

#include "TCGmcHit.hxx"
#include "G4ios.hh"
#include "G4UnitsTable.hh"


G4Allocator<TCGmcHit> TCGmcHitAllocator;

/*!
 \class TCGmcHit
 \brief G4 hit class
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
TCGmcHit::TCGmcHit()
: G4VHit(),
  fEdep(0.),
  fEnergyInput(0.),
  fZparticle(0),
  fNumberNucleon(0),
  fMass(0.),
  fCharge(0.),
  fSensorId(-1),
  flocalPos(),
  fTrackId(-1),
  fParentId(-1),
  fParticleName(""),
  fParticlePDG(0),
  fFlagUsed(-1),
  fPosOut(),
  fPosIn(),
  fPosVertex(),
  fMomentumOut(),
  fMomentumIn(),
  fMomentumDir(),
  fKineticEnergy(0.),
  fGlobalTime(0.)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCGmcHit::~TCGmcHit()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCGmcHit::TCGmcHit(const TCGmcHit &right)
: G4VHit(),
  fEdep(right.fEdep),
  fEnergyInput(right.fEnergyInput),
  fZparticle(right.fZparticle),
  fNumberNucleon(right.fNumberNucleon),
  fMass(right.fMass),
  fCharge(right.fCharge),
  fSensorId(right.fSensorId),
  flocalPos(right.flocalPos),
  fTrackId(right.fTrackId),
  fParentId(right.fParentId),
  fParticleName(right.fParticleName),
  fParticlePDG(right.fParticlePDG),
  fFlagUsed(right.fFlagUsed),
  fPosOut(right.fPosOut),
  fPosIn(right.fPosIn),
  fPosVertex(right.fPosVertex),
  fMomentumOut(right.fMomentumOut),
  fMomentumIn(right.fMomentumIn),
  fMomentumDir(right.fMomentumDir),
  fKineticEnergy(right.fKineticEnergy),
  fGlobalTime(right.fGlobalTime)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
const TCGmcHit& TCGmcHit::operator=(const TCGmcHit &right)
{  
   fEdep          = right.fEdep;
   fEnergyInput   = right.fEnergyInput;
   fZparticle     = right.fZparticle;
   fNumberNucleon = right.fNumberNucleon;
   fMass          = right.fMass;
   fCharge        = right.fCharge;
   fSensorId      = right.fSensorId;
   flocalPos      = right.flocalPos;
   fTrackId       = right.fTrackId;
   fParentId      = right.fParentId;
   fParticleName  = right.fParticleName;
   fParticlePDG   = right.fParticlePDG;
   fFlagUsed      = right.fFlagUsed;
   fPosOut        = right.fPosOut;
   fPosIn         = right.fPosIn;
   fPosVertex     = right.fPosVertex;
   fMomentumDir   = right.fMomentumOut;
   fMomentumDir   = right.fMomentumIn;
   fMomentumDir   = right.fMomentumDir;
   fKineticEnergy = right.fKineticEnergy;
   fGlobalTime    = right.fGlobalTime;

  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int TCGmcHit::operator==(const TCGmcHit &right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCGmcHit::Print()
{
    G4cout<<"\n SensorId "<<fSensorId<<G4endl;
    G4cout<<"\n fTrackId "<<fTrackId<<G4endl;
    G4cout<<"\n fParentId "<<fParentId<<G4endl;
    G4cout<<"\n fParticleName "<<fParticleName<<G4endl;
    G4cout<<"\n fParticlePDG "<<fParticlePDG<<G4endl;
    G4cout<<"\n fepiPos "<<flocalPos<<G4endl;
   
  //G4cout << " \n  energy deposit: " << G4BestUnit(fEdep,"Energy")
	// << " \n  position: " << G4BestUnit(fPos,"Length") << G4endl;
}
