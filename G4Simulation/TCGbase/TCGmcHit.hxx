/*!
 \file TCGmcHit.hxx
 \brief G4 hit class
 
 \author M. Vanstalle
 */
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
   G4double        fEdep;          ///< EnergyDeposed
   G4double        fEnergyInput;   ///< Energy Input
   G4int           fZparticle;     ///< Z of particle
   G4int           fNumberNucleon; ///< Number of nucleons
   G4double        fMass;          ///< Mass of particle
   G4double        fCharge;        ///< Charge of particle
   G4int           fSensorId;      ///< Id of the touched sensor
   G4ThreeVector   flocalPos;      ///< Position of sensitive part in the CMOS container
   G4int           fTrackId;       ///< Id of the track
   G4int           fParentId;      ///< Id of ParentTrack
   G4String        fParticleName;  ///< Name of Particle
   G4int           fParticlePDG;   ///< Particle on PDG
   G4int           fFlagUsed;      ///< Set to 1 if the step in the hit has been used
   G4ThreeVector   fPosOut;        ///< Position after step (out posizion)
   G4ThreeVector   fPosIn;         ///< Position before step (in position)
   G4ThreeVector   fPosVertex;     ///< Vertex position in case of interaction
   G4ThreeVector   fMomentumOut;   ///< Momentum Out case of interaction
   G4ThreeVector   fMomentumIn;    ///< Momentum In in case of interaction
   G4ThreeVector   fMomentumDir;   ///< Momentum dir in case of interaction
   G4double        fKineticEnergy; ///< Charge of particle
   G4double        fGlobalTime;    ///< Global time
   G4double        fLocalTime;     ///< Local time

public:
   //! Add energy loss
   void AddEdep(G4double de)               { fEdep  += de;               }
   //! Set energy loss
   void SetEdep(G4double de)               { fEdep = de;                 } //keV
   G4double GetEdep()                      { return fEdep;               }
   //! Get energy loss
   void SetEinput(G4double de)             { fEnergyInput = de;          } //keV
   //! Get input energy
   G4double GetEinput()                    { return fEnergyInput;        }
   //! Set atomic charge
   void SetZparticle(G4int z)              { fZparticle = z;             }
   //! Get atomic charge
   G4int GetZparticle()                    { return fZparticle;          }
   //! Set nucleons
   void SetNnucleons(G4int n)              { fNumberNucleon = n;         }
   //! Get nucleons
   G4int GetNnucleons()                    { return fNumberNucleon;      }
   //! Set mass
   void SetMass(G4double mass)             { fMass = mass;               }
   //! Get mass
   G4double GetMass()                      { return fMass;               }
   //! Set charge
   void SetCharge(G4double charge)         { fCharge = charge;           }
   //! Get charge
   G4double GetCharge()                    { return fCharge;             }
   
   //! Set position out
   void SetPosOut(G4ThreeVector xyz)       { fPosOut = xyz;              } //mm
   //! Get position out
   G4ThreeVector GetPosOut()               { return fPosOut;             }
   //! Set position in
   void SetPosIn(G4ThreeVector xyz)        { fPosIn = xyz;               } //mm
   //! Get position in
   G4ThreeVector GetPosIn()                { return fPosIn;              }

   //! Set momentum out
   void SetMomOut(G4ThreeVector xyz)       { fMomentumOut = xyz;         }
   //! Get momentum out
   G4ThreeVector GetMomOut()               { return fMomentumOut;        }
   //! Set momentum in
   void SetMomIn(G4ThreeVector xyz)        { fMomentumIn = xyz;          }
   //! Get momentum in
   G4ThreeVector GetMomIn()                { return fMomentumIn;         }

   //! Set vertex position
   void SetPosVertex(G4ThreeVector xyz)    { fPosVertex = xyz;           } //mm
   //! Get vertex position
   G4ThreeVector GetPosVertex()            { return fPosVertex;          } //mm
   
   //! Set momentum direction
   void SetMomentumDir(const G4ThreeVector& p)  { fMomentumDir = p;      }
   //! Get momentum direction
   G4ThreeVector& GetMomentumDir()         { return fMomentumDir;        }
   
   //! Set kinetic energy
   void SetKineticEnergy(G4double en)      { fKineticEnergy = en;        }  //Initial kinetic Energy
   //! Get kinetic energy
   G4double GetKineticEnergy()       const { return fKineticEnergy;      }
   
   //! Set global time
   void SetGlobalTime(G4double time)       { fGlobalTime = time;         }  //Global time of track
   //! Get global time
   G4double GetGlobalTime()          const { return fGlobalTime;         }
   //! Set local time
   void SetLocalTime(G4double time)        { fLocalTime = time;          }  //Local time of track
   //! Get local time
   G4double GetLocalTime()           const { return fLocalTime;          }

   //! Set sensor id
	void SetSensorId(G4int SensorId)        { fSensorId = SensorId;       }
   //! Get sensor id
	G4int GetSensorId()                     { return fSensorId;           }
   //! Set local position
	void SetlocPos(G4ThreeVector xyz)       { flocalPos = xyz;            } //mm
   //! Get local position
	G4ThreeVector GetlocPos()               { return flocalPos;           }
   //! Set track id
	void SetTrackId(G4int TrackId)          { fTrackId = TrackId;         }
   //! Get track id
	G4int GetTrackId()                      { return fTrackId;            }
   //! Set parent id
	void SetParentId(G4int ParentId)        { fParentId = ParentId;       }
   //! Get parent id
	G4int GetParentId()                     { return fParentId;           }
   //! Set particle name
	void SetParticleName(G4String pName)    { fParticleName = pName;      }
   //! Get particle name
	G4String GetParticleName()              { return fParticleName;       }
   //! Set particle PDG
	void SetParticlePDG(G4int particlePDG)  { fParticlePDG = particlePDG; }
   //! Set particle PDG
	G4int GetParticlePDG()                  { return fParticlePDG;        }
   
   //! Set flag used
	void SetFlagUsed(G4int flag)            { fFlagUsed = flag;           }
   //! Get flag used
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


