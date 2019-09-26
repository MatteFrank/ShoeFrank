//TCFOeventoAction

#ifndef TCFOeventoAction_h
#define TCFOeventoAction_h 1

#include <map>
#include "TCFObaseEventAction.hxx"
#include "globals.hh"

#include "TAMCntuEve.hxx"

class TCFOrunAction;
class TCGbaseGeometryConstructor;
class TCFOgeometryConstructor;
class Evento;
class TCGmcHit;

// Event action for HIT geometry

//------------------------------------------------
class TCFOeventoAction : public TCFObaseEventAction
{
    public:
    TCFOeventoAction(TCFOrunAction* runAction, TCGbaseGeometryConstructor* hitGeomConstructor);
    ~TCFOeventoAction();

    public:
    void   EndOfEventAction(const G4Event*);
    void   Collect(const G4Event* evt);

private:
   
   std::map<int, int> fMapTrackIdx;

    private:
    void   FillTrack();
    void   GetHitPerPlane(const G4Event* evt, G4int idColl);
    void   FillHits(Evento* mcHit, TCGmcHit* hit);
};

#endif


