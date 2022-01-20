
#ifndef TCFOeventAction_h
#define TCFOeventAction_h 1

/*!
 \file TCFOeventAction.hxx
 \brief  Event action for FOOT
 
 \author Ch. Finck
 */

#include <map>
#include "TCFObaseEventAction.hxx"
#include "globals.hh"

#include "TAMCntuPart.hxx"

class TCFOrunAction;
class TCGbaseGeometryConstructor;
class TCFOgeometryConstructor;
class TAMCevent;
class TCGmcHit;

//------------------------------------------------
class TCFOeventAction : public TCFObaseEventAction
{
public:
    TCFOeventAction(TCFOrunAction* runAction, TCGbaseGeometryConstructor* hitGeomConstructor);
    ~TCFOeventAction();

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


