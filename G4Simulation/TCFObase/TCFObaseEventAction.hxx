//TCFObaseEventAction

#ifndef TCFObaseEventAction_h
#define TCFObaseEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "TAMCntuPart.hxx"

#include "TSysEvtHandler.h"

class TCFOrunAction;
class TCGbaseGeometryConstructor;
class TCFOgeometryConstructor;
class TAGgeoTrafo;
class TCGmcHit;
class TAMCntuPart;

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

    TAMCntuPart* GetMcEvent()          { return fMcEvent;       }
    void SetFillTree(Bool_t fill)      { fFillTree = fill;      }
   
    void SetInelasticOnly(G4bool frag) { fInelasticOnly = frag; }
    G4bool IsInelasticOnly()           { return fInelasticOnly; }

    protected:
    TCFOrunAction*           fRunAction;           // run action for FOOT
    TCFOgeometryConstructor* fFootGeomConstructor; // geometry for FOOT
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
    TAMCntuPart*             fMcEvent;
    Bool_t                   fFillTree;
    TAGgeoTrafo*             fpGeoTrafo;           // trafo pointer
    G4bool                   fInelasticOnly;


    protected:
    Int_t  GetEventsNToBeProcessed();
    virtual void   FillAndClear();
};

#endif


