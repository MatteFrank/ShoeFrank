
#include "TCGphysicsQMD.hxx"

#include "G4IonElasticPhysics.hh"
#include "G4IonQMDPhysics.hh"
#include "G4IonINCLXXPhysics.hh"
#if G4VERSION_NUMBER > 1003
  #include "G4IonPhysicsPHP.hh"
#endif

#include "G4IonPhysics.hh"

#include "G4EmStandardPhysics_option3.hh"
#include "G4EmExtraPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_AllHP.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronPhysicsINCLXX.hh"

#include "G4ProcessManager.hh"

// particles

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

using namespace CLHEP	;

TCGphysicsQMD::TCGphysicsQMD():  G4VModularPhysicsList(),
hadronElastic(nullptr), hadronInelastic(nullptr),ionElastic(nullptr),
ionInelastic(nullptr), electromagnetic(nullptr), decay(nullptr),
radioactiveDecay(nullptr)
{

    // Ion Elastic scattering
    ionElastic = new G4IonElasticPhysics();
    RegisterPhysics(ionElastic);

    // Ion Inelastic physics
    //    ionInelastic = new G4IonPhysicsPHP();
    ionInelastic = new G4IonQMDPhysics();
    RegisterPhysics(ionInelastic);

    // Particle decays
    decay = new G4DecayPhysics();
    radioactiveDecay = new G4RadioactiveDecayPhysics();
    RegisterPhysics(decay);
    RegisterPhysics(radioactiveDecay);

    // Leptons
    RegisterPhysics(new G4EmExtraPhysics());
    electromagnetic = new G4EmStandardPhysics_option3();
    RegisterPhysics(electromagnetic);

//    hadronElastic = new G4HadronElasticPhysicsHP();
    hadronElastic = new G4HadronElasticPhysics();
    RegisterPhysics( hadronElastic);

    // Hadron Inelastic physics
    //    hadronInelastic = new G4HadronPhysicsQGSP_BIC_AllHP();
    //    RegisterPhysics( new G4HadronPhysicsQGSP_BIC());
    hadronInelastic = new G4HadronPhysicsQGSP_BIC_HP();
    RegisterPhysics( hadronInelastic);


}

TCGphysicsQMD::~TCGphysicsQMD()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TCGphysicsQMD::ConstructProcess()
{
    // Transportation first (mandatory)
    //
    AddTransportation();

    // Physics constructors
    //
    hadronElastic->ConstructProcess();
    hadronInelastic->ConstructProcess();
    ionInelastic->ConstructProcess();
    electromagnetic->ConstructProcess();
    decay->ConstructProcess();
    radioactiveDecay->ConstructProcess();

    ///// NB : this lines must be uncommented if you want to use G4HadronPhysicsQGSP_BIC_AllHP + G4IonPhysicsPHP
    //    // Get Models
    //    //
    //    G4ProcessManager* pManager = G4Neutron::Neutron()->GetProcessManager();
    //    G4HadronicProcess* process = dynamic_cast<G4HadronicProcess*>(pManager->GetProcess("nCapture"));
    //    std::vector<G4HadronicInteraction*>& list
    //    = process->GetHadronicInteractionList();
    //    for (size_t li=0; li<list.size(); li++) {
    //        if (list[li]->GetModelName() == "nRadCapture")
    //            list[li]->SetMinEnergy(19.9*MeV);
    //    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TCGphysicsQMD::ConstructParticle()
{
    G4BosonConstructor  pBosonConstructor;
    pBosonConstructor.ConstructParticle();

    G4LeptonConstructor pLeptonConstructor;
    pLeptonConstructor.ConstructParticle();

    G4MesonConstructor pMesonConstructor;
    pMesonConstructor.ConstructParticle();

    G4BaryonConstructor pBaryonConstructor;
    pBaryonConstructor.ConstructParticle();

    G4IonConstructor pIonConstructor;
    pIonConstructor.ConstructParticle();

    G4ShortLivedConstructor pShortLivedConstructor;
    pShortLivedConstructor.ConstructParticle();
}

void TCGphysicsQMD::SetCuts()
{
    G4double cutGammas = 1.0*m;
    G4double cutElectrons = 1.0*m;
    G4double cutPositrons = 1.0*m;

    SetCutValue(cutGammas,"gamma");
    SetCutValue(cutElectrons,"e-");
    SetCutValue(cutPositrons,"e+");

//    // Use default cut values gamma and e processes
//    SetCutsWithDefault();   
}
