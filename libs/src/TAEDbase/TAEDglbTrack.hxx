#ifndef _TAEDglbTrack_HXX_
#define _TAEDglbTrack_HXX_


#include "TEveTrack.h"
#include "TVector3.h"

/** TAEDglbTrack a class to display global tracks on event
 
*/
class TEveTrackPropagator;

class TAEDglbTrack : public TEveTrack {

public:
   TAEDglbTrack(TEveRecTrackD* track,  TEveTrackPropagator* prop);
   virtual ~TAEDglbTrack();
    
   //! Add track point
   void AddTrackPoint(TVector3& point, TVector3& mom);

   //! Set track id object
   void TrackId(TObject* obj);
   
   //! Reset tracks
   void ResetTrack();
   
   //! Get track id object
   TObject* GetTrackId() const { return fTrackId; }
   
private:
   TObject* fTrackId;
   
};


#endif
