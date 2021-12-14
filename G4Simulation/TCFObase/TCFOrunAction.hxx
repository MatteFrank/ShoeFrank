#ifndef TCFOrunAction_h
#define TCFOrunAction_h 1

/*!
 \file TCFOrunAction.hxx
 \brief  Run action for FOOT
 
 \author Ch. Finck
 */

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

    //! Begin run action
    void BeginOfRunAction(const G4Run* aRun);
    //! End run action
    void EndOfRunAction(const G4Run* aRun);
    //! Fill tree and clear containers
    void FillAndClear(Bool_t fill = true);

    //! Get output file
    TFile*     GetOutFile()                const { return fpOutFile;             }
    //! Get number of event to process
    Int_t      GetEventsNToBeProcessed()   const { return fEventsNToBeProcessed; }
    //! Get MC particles informations
    TAMCevent* GetEventMC()                const { return fpEventMC;             }
  
public:
    //! Set output root file name
    static void SetRootFileName(const char* name) { fgRootFileName = name; }
    //! Get output root file name
    static const char* GetRootFileName()          { return fgRootFileName; }

private:
    static TString fgRootFileName; ///< Output root file name

private:
    TAMCevent*     fpEventMC;             ///< For data form Simulation
    TFile*         fpOutFile;             ///< Root output file
    TTree*         fpTree;                ///< Output tree
    Int_t          fEventsNToBeProcessed; ///< Number of event to be processed
    TStopwatch     fWatch;                ///< Timer

private:
    //! Set containers
    void           SetContainers();
    //! Clear containers
    void           ClearContainers();
    //! Print branches
    void           PrintBranches();
 
};

#endif
