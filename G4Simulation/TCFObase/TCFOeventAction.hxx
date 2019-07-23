//TCFOeventAction

#ifndef TCFOeventAction_h
#define TCFOeventAction_h 1

#include <map>
#include "TCFObaseEventAction.hxx"
#include "globals.hh"

#include "TAMCntuEve.hxx"

class TCFOrunAction;
class TCGbaseGeometryConstructor;
class TCFOgeometryConstructor;
class TAMCevent;
class TCGmcHit;


// Event action for HIT geometry

//------------------------------------------------
class TCFOeventAction : public TCFObaseEventAction
{
    public:
    TCFOeventAction(TCFOrunAction* runAction, TCGbaseGeometryConstructor* hitGeomConstructor);
    ~TCFOeventAction();

    public:
    void   EndOfEventAction(const G4Event*);
    void   Collect(const G4Event* evt);

    private:
    void   FillTrack();
    void   GetHitPerPlane(const G4Event* evt, G4int idColl);
    void   FillHits(TAMCevent* mcHit, TCGmcHit* hit);
   
    private:
   std::map<int, int> fMapTrackIdx; 
};

#endif


