// my hit class

#ifndef TCGmcHit_h
#define TCGmcHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class TCGmcHit : public G4VHit
{
public:
   TCGmcHit();
   ~TCGmcHit();
   TCGmcHit(const TCGmcHit &right);
   const TCGmcHit& operator=(const TCGmcHit &right);
   G4int operator==(const TCGmcHit &right) const;

   inline void *operator new(size_t);
   inline void operator delete(void *aHit);

   void Print();
   
private:
   G4double        fEdep; //EnergyDeposed
   G4double        fEnergyInput; //EnergyInput
   G4int           fZparticle; //Z of particle
   G4int           fNumberNucleon; //Number of nucleons
   G4double        fMass; //mass of particle
   G4double        fCharge; // charge of particle
   G4int           fSensorId; //id of the touched sensor
   G4ThreeVector   flocalPos; //position of sensitive part in the CMOS container
   G4int           fTrackId; //id of the track
   G4int           fParentId; //id of ParentTrack
   G4String        fParticleName; //name of Particle
   G4int           fParticlePDG; //particle on PDG
   G4int           fFlagUsed; //set to 1 if the step in the hit has been used
   G4ThreeVector   fPosOut; //position after step (out posizion)
   G4ThreeVector   fPosIn; //position before step (in position)
   G4ThreeVector   fPosVertex; //vertex position in case of interaction
   G4ThreeVector   fMomentumOut; //momentum Out case of interaction
   G4ThreeVector   fMomentumIn; // momentum In in case of interaction
   G4ThreeVector   fMomentumDir; //momentum dir in case of interaction
   G4double        fKineticEnergy; // charge of particle
   G4double        fGlobalTime; // global time
   G4double        fLocalTime; // local time

public:
   void AddEdep(G4double de)               { fEdep  += de;               }
   void SetEdep(G4double de)               { fEdep = de;                 } //keV
   G4double GetEdep()                      { return fEdep;               }
   void SetEinput(G4double de)             { fEnergyInput = de;          } //keV
   G4double GetEinput()                    { return fEnergyInput;        }
   void SetZparticle(G4int z)              { fZparticle = z;             }
   G4int GetZparticle()                    { return fZparticle;          }
   void SetNnucleons(G4int n)              { fNumberNucleon = n;         }
   G4int GetNnucleons()                    { return fNumberNucleon;      }
   void SetMass(G4double mass)             { fMass = mass;               }
   G4double GetMass()                      { return fMass;               }
   void SetCharge(G4double charge)         { fCharge = charge;           }
   G4double GetCharge()                    { return fCharge;             }
   
   void SetPosOut(G4ThreeVector xyz)       { fPosOut = xyz;              } // mm
   G4ThreeVector GetPosOut()               { return fPosOut;             }
   void SetPosIn(G4ThreeVector xyz)        { fPosIn = xyz;               } // mm
   G4ThreeVector GetPosIn()                { return fPosIn;              }

   void SetMomOut(G4ThreeVector xyz)       { fMomentumOut = xyz;         }
   G4ThreeVector GetMomOut()               { return fMomentumOut;        }
   void SetMomIn(G4ThreeVector xyz)        { fMomentumIn = xyz;          }
   G4ThreeVector GetMomIn()                { return fMomentumIn;         }

   
   void SetPosVertex(G4ThreeVector xyz)    {fPosVertex = xyz;            } //mm
   G4ThreeVector GetPosVertex()            {return fPosVertex;           } //mm
   
   void SetMomentumDir(const G4ThreeVector& p)  { fMomentumDir = p;           }
   G4ThreeVector& GetMomentumDir()         { return fMomentumDir;        }
   
   void SetKineticEnergy(G4double en)      { fKineticEnergy = en;        }  //Initial kinetic Energy
   G4double GetKineticEnergy()       const { return fKineticEnergy;      }
   
   void SetGlobalTime(G4double time)       { fGlobalTime = time;         }  //Global time of track
   G4double GetGlobalTime()          const { return fGlobalTime;         }
    void SetLocalTime(G4double time)       { fLocalTime = time;         }  //Local time of track
    G4double GetLocalTime()          const { return fLocalTime;         }

	void SetSensorId(G4int SensorId)        { fSensorId = SensorId;       }
	G4int GetSensorId()                     { return fSensorId;           }
	void SetlocPos(G4ThreeVector xyz)       { flocalPos = xyz;            } //mm
	G4ThreeVector GetlocPos()               { return flocalPos;           }
	void SetTrackId(G4int TrackId)          { fTrackId = TrackId;         }
	G4int GetTrackId()                      { return fTrackId;            }
	void SetParentId(G4int ParentId)        { fParentId = ParentId;       }
	G4int GetParentId()                     { return fParentId;           }
	void SetParticleName(G4String pName)    { fParticleName = pName;      }
	G4String GetParticleName()              { return fParticleName;       }
	void SetParticlePDG(G4int particlePDG)  { fParticlePDG = particlePDG; }
	G4int GetParticlePDG()                  { return fParticlePDG;        }
   
	void SetFlagUsed(G4int flag)            { fFlagUsed = flag;           }
	G4int GetFlagUsed()                     { return fFlagUsed;           }
};


typedef G4THitsCollection<TCGmcHit> TCGmcCollections;

extern G4Allocator<TCGmcHit> TCGmcHitAllocator;


inline void* TCGmcHit::operator new(size_t)
{
   void *aHit;
   aHit = (void *) TCGmcHitAllocator.MallocSingle();
   return aHit;
}

inline void TCGmcHit::operator delete(void *aHit)
{
   TCGmcHitAllocator.FreeSingle((TCGmcHit*) aHit);
}


#endif


