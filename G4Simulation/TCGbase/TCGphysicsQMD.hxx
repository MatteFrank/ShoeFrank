
#ifndef TCGphysicsQMD_h
#define TCGphysicsQMD_h 1

/*!
 \file TCGphysicsQMD.hxx
 \brief Physics list from QMD
 
 \author M. Vanstalle
 */

#include "G4VModularPhysicsList.hh"
#include "globals.hh"


class TCGphysicsQMD: public G4VModularPhysicsList
{
public:
    TCGphysicsQMD();
    virtual ~TCGphysicsQMD();

    //! Set cuts
    virtual void SetCuts();
    //! Construct particles
    virtual void ConstructParticle();
    //! Construct process
    virtual void ConstructProcess();

private:
    G4VPhysicsConstructor* hadronElastic;    ///< Physics constructor for hadronic elastic
    G4VPhysicsConstructor* hadronInelastic;  ///< Physics constructor for hadronic inelastic
    G4VPhysicsConstructor* ionElastic;       ///< Physics constructor for ion elastic
    G4VPhysicsConstructor* ionInelastic;     ///< Physics constructor for ion inelastic
    G4VPhysicsConstructor* electromagnetic;  ///< Physics constructor for em
    G4VPhysicsConstructor* decay;            ///< Physics constructor for decay
    G4VPhysicsConstructor* radioactiveDecay; ///< Physics constructor for radioactive decay
};

#endif


