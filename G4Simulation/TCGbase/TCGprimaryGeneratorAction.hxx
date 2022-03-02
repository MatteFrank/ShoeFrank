
#ifndef TCGprimaryGeneratorAction_hxx
#define TCGprimaryGeneratorAction_hxx

/*!
 \file TCGprimaryGeneratorAction.hxx
 \brief  Primary generation action for FOOT
 
 \author Ch. Finck
 */

#include "G4VUserPrimaryGeneratorAction.hh"
#include "TAGparGeo.hxx"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class TCGprimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
   TCGprimaryGeneratorAction(TAGparGeo* pParGeo);
   ~TCGprimaryGeneratorAction();
   
public:
   // Generate primaries
   void GeneratePrimaries(G4Event* anEvent);
   
private:
   G4ParticleGun* fParticleGun;       ///< Particle gun
   TAGparGeo*     fpParGeo;           ///< Geometry parameters for beam
   Bool_t         fHasPartDefined;    ///< Flag for defined particle
};


#endif


