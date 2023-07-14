
#ifndef TCLINMeventAction_h
#define TCLINMeventAction_h 1

/*!
 \file TCLINMeventAction.hxx
 \brief  Event action for FOOT
 
 \author Ch. Finck
 */

#include <map>
#include "TCLINMbaseEventAction.hxx"
#include "globals.hh"

#include "TAMCntuPart.hxx"

class TCLINMrunAction;
class TCGbaseGeometryConstructor;
class TCLINMgeometryConstructor;
class TAMCevent;
class TCGmcHit;

//------------------------------------------------
class TCLINMeventAction : public TCLINMbaseEventAction
{
public:
    TCLINMeventAction(TCLINMrunAction* runAction, TCGbaseGeometryConstructor* hitGeomConstructor);
    ~TCLINMeventAction();

public:
    // Begin event action
    void   EndOfEventAction(const G4Event*);
    // Collect hits
    void   Collect(const G4Event* evt);

private:
    // Fill track (particle) containers
    void   FillTrack();
    // Get hits per plane
    void   GetHitPerPlane(const G4Event* evt, G4int idColl);
    // Fill shoe hits from G4 hits
    void   FillHits(TAMCevent* mcHit, TCGmcHit* hit);
   
private:
   std::map<int, int> fMapTrackIdx; ///< Track index map
};

#endif


