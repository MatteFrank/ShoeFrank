#ifndef _TAEDglbTrack_HXX_
#define _TAEDglbTrack_HXX_


#include "TEveTrack.h"
#include "TVector3.h"

/** TAEDglbTrack a class to display global tracks on event
 
*/
class  TEveTrackPropagator;
class TEveTrack;

class TAEDglbTrack : public TEveTrack {

public:
   TAEDglbTrack(TEveRecTrackD* track,  TEveTrackPropagator* prop);
   virtual ~TAEDglbTrack();
    
   //! Add Track Marker
   void AddTrackMarker(TVector3& point, TVector3& mom);

   //! Set Track Id Object
   void TrackId(TObject* obj);
   
   //! Reset tracks
   void ResetTrack();
   
private:
   TObject* fTrackId;
   
  ClassDef(TAEDglbTrack, 0);  
};


#endif
