// \file TAGsimulation.cxx
// \brief Main program of the Qapivi simulation

#include "TString.h"
#include "TRandom.h"

#include "G4RunManager.hh"
#include "G4Version.hh"
#include "G4VisManager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "TCGbaseGeometryConstructor.hxx"
#include "TCVTgeometryConstructor.hxx"
#include "TCGprimaryGeneratorAction.hxx"

//FOOT
#include "TCFOgeometryConstructor.hxx"
#include "TCFOrunAction.hxx"
#include "TCFObaseEventAction.hxx"
#include "TCFOeventAction.hxx"
#include "TCFOtrackingAction.hxx"

#include "G4UImanager.hh"
#include "G4String.hh"

#include "QGSP_BERT.hh"
#include "QGSP_INCLXX.hh"
#include "QGSP_BIC.hh"

#include "TCGphysicsQMD.hxx"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "TAGrecoManager.hxx"
#include "TAVTparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

int main(int argc,char** argv)
{
    // Construct the default run manager
    //
    G4RunManager* runManager = new G4RunManager;

    // initialize root file name
    TString rootFileName("ionO16.root");

    // initialise seed
    UInt_t seed = 0;

    // interactive mode flag
    G4bool interMode(false);

    // run mode flag
    G4bool runMode(false);

    // initialise physics list
    TString physListName("BIC");

    // Fill only fragmented events
    G4bool frag = false;
   
    // number of events to process
    G4int eventsNToBeProcessed = -1;
   
    // expriment name
    TString expName("");
   
    // run number
    G4int runNumber = -1;

    // looking for arguments
    for (int i = 0; i < argc; i++) {
        if(strcmp(argv[i],"-out") == 0)
            rootFileName =TString(argv[++i]);

        if(strcmp(argv[i],"-seed") == 0) {
            TString sSeed = argv[++i];
            seed = sSeed.Atoi();
        }

        if(strcmp(argv[i],"-nev") == 0) {
            TString sEvt = argv[++i];
            eventsNToBeProcessed = sEvt.Atoi();
        }
       
       if(strcmp(argv[i],"-run") == 0) {
          TString sRun = argv[++i];
          runNumber = sRun.Atoi();
       }
       
        if(strcmp(argv[i],"-phys") == 0)
            physListName  = argv[++i];

        if(strcmp(argv[i],"-i") == 0)
            interMode  = true;
       
       if(strcmp(argv[i],"-frag") == 0)
          frag  = true;

        if(strcmp(argv[i],"-r") == 0)
            runMode  = true;

        if(strcmp(argv[i],"-out") == 0)
            rootFileName  = argv[++i];

        if(strcmp(argv[i],"-exp") == 0)
            expName = TString(argv[++i]);

        if(strcmp(argv[i],"-help") == 0) {
            printf("Possible arguments are:\n");
            printf("  -i interactive mode enable \n");
            printf("  -r run.mac is launched \n");
            printf("  -nev nevent: number of events");
            printf("  -out rootFileName: root output file name \n");
            printf("  -phys physList: physics list: BIC, BERT or INCL \n");
            printf("  -seed seedNb: seed number for random initialization  \n");
            printf("  -exp name: [def=""] experiment name for config/geomap extension\n");
            printf("  -run #run [def=-1] run number\n");
            printf("  -frag save only when ion inelastic process occurs in target\n");

            return 1;
        }
    }
  
    // Global Par
    TAGrecoManager::Instance(expName);
    TAGrecoManager::GetPar()->FromFile();
   
    // TAG root
    TAGroot tagRoot;
    tagRoot.SetRunNumber(runNumber);
    tagRoot.SetCampaignName(expName);

    // Set various flag
    TCVTgeometryConstructor::SetSmearFlag(false); // smearing due to the accurancy of the alignment (DeltaPos = 5mu, DeltaAng = 0.1º default)
    TCFOrunAction::SetRootFileName(rootFileName.Data());

    // Detector construction
    TCGbaseGeometryConstructor* theDetector =  new TCFOgeometryConstructor(expName, runNumber);
    runManager->SetUserInitialization(theDetector);

    // Physics list
#if G4VERSION_NUMBER < 1000
    printf("Geant4 v9 not supported");
    exit(0);
#else
    //   G4VUserPhysicsList* physics = 0x0;
    G4VModularPhysicsList* physics = 0x0;
    physListName.ToLower();
    if (physListName.Contains("bert"))
        physics = new QGSP_BERT();
    else if (physListName.Contains("bic"))
        physics = new QGSP_BIC();
    else if (physListName.Contains("incl"))
        physics = new QGSP_INCLXX();
    else if (physListName.Contains("qmd"))
        physics = new TCGphysicsQMD();
    else
        printf("\n\n No physics list defined !!\n\n");
#endif

    physics->SetVerboseLevel(0);
    runManager->SetUserInitialization(physics);

    TCGprimaryGeneratorAction* kin = new TCGprimaryGeneratorAction(theDetector->GetParGeoG());
   
   if (eventsNToBeProcessed == -1)
      eventsNToBeProcessed = theDetector->GetParGeoG()->GetBeamPar().PartNumber;
   
    TCFOrunAction*  run = new TCFOrunAction();
  
    TCFObaseEventAction* event  = new TCFOeventAction(run, theDetector);
    event->SetInelasticOnly(frag);

   
    TCFOtrackingAction *tracking = new TCFOtrackingAction(event) ;

    runManager->SetUserAction(kin);
    runManager->SetUserAction(event);
    runManager->SetUserAction(tracking) ;
    runManager->SetUserAction(run);

    runManager->SetVerboseLevel(-1);
    runManager->Initialize();

    // User Interface
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    G4UImanager* UI = G4UImanager::GetUIpointer();

    // Seed setting
    if (seed != 0) {
        gRandom->SetSeed(seed);
        G4int seed1 = (int)gRandom->Uniform(1,10);
        G4int seed2 = (int)gRandom->Uniform(1,100);

        G4String command = "/random/setSeeds ";
        G4String name = Form("%d %d", seed1, seed2);
        UI->ApplyCommand(command+name);
    }

    // modes
    if (interMode) {// with visualization
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);
      G4cout<<" USE G4_VIS USE"<<G4endl;
      UI->ApplyCommand("/control/execute vis.mac");
      ui->SessionStart();
      delete ui;
      
    } else if (runMode){
        G4String command = "/control/execute run.mac";
        UI->ApplyCommand(command);
      
    } else
      runManager->BeamOn(eventsNToBeProcessed);
  
    delete visManager;
    delete runManager;
    
    return 0;
}


