#ifndef _TAEDglbTrackList_HXX_
#define _TAEDglbTrackList_HXX_


#include "TEveElement.h"
#include "TVector3.h"

/** TAEDglbTrackList a class to display global tracks on event
 
*/
class TAEDglbTrack;

class TAEDglbTrackList : public TEveElementList {

public:
   TAEDglbTrackList(const Char_t* name = "");
   virtual ~TAEDglbTrackList();
  
   //! return number of tracks
   Int_t GetNofTracks()     const { return fNofTracks; }
   
   //! Add Track
   TAEDglbTrack* NewTrack(const Char_t* label);
  
   //! Reset tracks
   void ResetTracks();
      
private:
   Int_t            fNofTracks;   // number of tracks
};


#endif
