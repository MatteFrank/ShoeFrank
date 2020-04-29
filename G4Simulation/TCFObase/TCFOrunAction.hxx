//header file

#ifndef TCFOrunAction_h
#define TCFOrunAction_h 1

#include "G4UserRunAction.hh"

#include "globals.hh"

#include "Evento.hxx"
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

    TFile* GetOutFile()                const { return fpOutFile;    }
    Int_t  GetEventsNToBeProcessed()   const { return fEventsNToBeProcessed;}

    public:
    void         DisableEvento()         { fkEvento = 0 ; }
    Bool_t       GetEvento()       const { return fkEvento  ; }
    Evento*      GetEventoMC()     const { return fpEventoMC; }
    TAMCevent*   GetEventMC()      const { return fpEventMC; }
    void SetEvento(Bool_t akEvento){ fkEvento = akEvento ; }
    static void SetRootFileName(const char* name) { fgRootFileName = name; }
    static const char* GetRootFileName() { return fgRootFileName; }

    private:
    Bool_t         fkEvento;
    Evento*        fpEventoMC; // For data form Simulation
    TAMCevent*     fpEventMC;  // For data form Simulation
    static TString fgRootFileName;
    TFile*         fpOutFile;
    TTree*         fpTree;
    Int_t          fEventsNToBeProcessed;
    TStopwatch     fWatch;

    private:
    void           SetContainers();
    void           ClearContainers();

};

#endif
