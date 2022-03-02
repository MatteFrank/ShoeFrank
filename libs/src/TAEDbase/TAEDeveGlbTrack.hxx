#ifndef _TAEDeveGlbTrack_HXX_
#define _TAEDeveGlbTrack_HXX_

/*!
 \file TAEDeveGlbTrack.hxx
 \brief   TAEDeveGlbTrack a class to display global tracks on event
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TEveTrack.h"
#include "TVector3.h"

class TEveTrackPropagator;

class TAEDeveGlbTrack : public TEveTrack {

public:
   TAEDeveGlbTrack(TEveRecTrackD* track,  TEveTrackPropagator* prop);
   virtual ~TAEDeveGlbTrack();
    
   // Add track point
   void AddTrackPoint(TVector3& point, TVector3& mom);

   // Set track id object
   void TrackId(TObject* obj);
   
   // Reset tracks
   void ResetTrack();
   
   //! Get track id object
   TObject* GetTrackId() const { return fTrackId; }
   
private:
   TObject* fTrackId; ///< Track objet reference
};

#endif
