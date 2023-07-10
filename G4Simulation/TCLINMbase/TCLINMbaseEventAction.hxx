#ifndef TCLINMbaseEventAction_h
#define TCLINMbaseEventAction_h 1

/*!
 \file TCLINMbaseEventAction.hxx
 \brief  Base event action for FOOT
 
 \author Ch. Finck
 */

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "TAMCntuPart.hxx"

#include "TSysEvtHandler.h"

class TCLINMrunAction;
class TCGbaseGeometryConstructor;
class TCLINMgeometryConstructor;
class TAGgeoTrafo;
class TCGmcHit;
class TAMCntuPart;

class TAGeventInterruptHandler : public TSignalHandler {
    public:
    TAGeventInterruptHandler();
    Bool_t          Notify();
};

//------------------------------------------------
class TCLINMbaseEventAction : public G4UserEventAction
{
public:
    TCLINMbaseEventAction(TCLINMrunAction* runAction, TCGbaseGeometryConstructor* hitGeomConstructor);
    virtual ~TCLINMbaseEventAction();

public:
    // Begin event action
    void   BeginOfEventAction(const G4Event* evt);
    // End event action
    virtual void   EndOfEventAction(const G4Event* evt);
    // Fill collection
    void   ConstructCollection();

   //! Get MC info
    TAMCntuPart* GetMcEvent()          { return fMcEvent;       }
   //! Fill tree
    void SetFillTree(Bool_t fill)      { fFillTree = fill;      }
   
    //! Flag for inelastic process
    void SetInelasticOnly(G4bool frag) { fInelasticOnly = frag; }
    //! Check for inelastic process
    G4bool IsInelasticOnly()           { return fInelasticOnly; }

protected:
    TCLINMrunAction*           fRunAction;             ///< run action for FOOT
    TCLINMgeometryConstructor* fFootGeomConstructor;   ///< geometry for FOOT
    Int_t                    fEventNumber;           ///< event number
    Int_t                    fStCollId;              ///< Id of ST collection
    Int_t                    fBmCollId;              ///< Id of BM collection
    Int_t                    fVtxCollId;             ///< Id of Vtx collection
    Int_t                    fItCollId;              ///< Id of IT collection
    Int_t                    fMsdCollId;             ///< Id of MSD collection
    Int_t                    fTwCollId;              ///< Id of ToF collection
    Int_t                    fCaCollId;              ///< Id of Calo collection
    TString                  fDetName;               ///< Detector name
    TSignalHandler*          fEventInterruptHandler; ///< Event loop interripter handler
    TAMCntuPart*             fMcEvent;               ///< MC info
    Bool_t                   fFillTree;              ///< Filling tree flag
    TAGgeoTrafo*             fpGeoTrafo;             ///< trafo pointer
    G4bool                   fInelasticOnly;         ///< Inelastic process flag

protected:
    // Number of event to be process
    Int_t  GetEventsNToBeProcessed();
    // Fill tree and clear containers
    virtual void   FillAndClear();
};

#endif


