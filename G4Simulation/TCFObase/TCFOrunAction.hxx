//header file

#ifndef TCFOrunAction_h
#define TCFOrunAction_h 1

#include "G4UserRunAction.hh"

#include "globals.hh"

#include "TAMCevent.hxx"
#include "TAGroot.hxx"
#include "TString.h"

#include <TStopwatch.h>

class G4Run;
class TFile;
class TTree;

class TCFOrunAction : public G4UserRunAction
{
    public:
    TCFOrunAction();
    ~TCFOrunAction();

    void BeginOfRunAction(const G4Run* aRun);
    void EndOfRunAction(const G4Run* aRun);
    void FillAndClear(Bool_t fill = true);

    TFile*     GetOutFile()                const { return fpOutFile;             }
    Int_t      GetEventsNToBeProcessed()   const { return fEventsNToBeProcessed; }
    TAMCevent* GetEventMC()                const { return fpEventMC;             }
  
    public:
    static void SetRootFileName(const char* name) { fgRootFileName = name; }
    static const char* GetRootFileName()          { return fgRootFileName; }

    private:
    static TString fgRootFileName;

    private:
    TAMCevent*     fpEventMC;  // For data form Simulation
    TFile*         fpOutFile;
    TTree*         fpTree;
    Int_t          fEventsNToBeProcessed;
    TStopwatch     fWatch;

    private:
    void           SetContainers();
    void           ClearContainers();
    void           PrintBranches();
 
};

#endif
