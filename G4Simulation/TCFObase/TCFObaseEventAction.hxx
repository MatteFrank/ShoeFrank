//TCFObaseEventAction

#ifndef TCFObaseEventAction_h
#define TCFObaseEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "TAMCntuEve.hxx"

#include "TSysEvtHandler.h"

class TCFOrunAction;
class TCGbaseGeometryConstructor;
class TCFOgeometryConstructor;
class TAGgeoTrafo;
class Evento;
class TCGmcHit;
class TAMCntuEve;

class TAGeventInterruptHandler : public TSignalHandler {
    public:
    TAGeventInterruptHandler();
    Bool_t          Notify();
};

// Event action for HIT geometry

//------------------------------------------------
class TCFObaseEventAction : public G4UserEventAction
{
    public:
    TCFObaseEventAction(TCFOrunAction* runAction, TCGbaseGeometryConstructor* hitGeomConstructor);
    virtual ~TCFObaseEventAction();

    public:
    void   BeginOfEventAction(const G4Event* evt);
    virtual void   EndOfEventAction(const G4Event*);
    void   ConstructCollection();

    TAMCntuEve* GetTrackMc(){ return fMcTrack ; };

    protected:
    TCFOrunAction*           fRunAction;           // run action for FOOT
    TCFOgeometryConstructor* fFootGeomConstructor; // geometry for FOOT
    Int_t                    fDebugLevel;
    Int_t                    fEventNumber;         // event number
    Int_t                    fStCollId;            // Id of ST collection
    Int_t                    fBmCollId;            // Id of BM collection
    Int_t                    fVtxCollId;           // Id of Vtx collection
    Int_t                    fItCollId;            // Id of IT collection
    Int_t                    fMsdCollId;           // Id of MSD collection
    Int_t                    fTwCollId;            // Id of ToF collection
    Int_t                    fCaCollId;            // Id of Calo collection
    TString                  fDetName;
    TSignalHandler*          fEventInterruptHandler;
    TAMCntuEve*              fMcTrack;

    TAGgeoTrafo*             fpGeoTrafo;           // trafo pointer

    protected:
    Int_t  GetEventsNToBeProcessed();
    virtual void   FillAndClear();
};

#endif


