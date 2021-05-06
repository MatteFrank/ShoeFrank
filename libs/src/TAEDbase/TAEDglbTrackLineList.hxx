#ifndef _TAEDglbTrackLineList_HXX_
#define _TAEDglbTrackLineList_HXX_


#include "TEveElement.h"
#include "TVector3.h"

/** TAEDglbTrackLineList a class to display global tracks on event
 
*/
class TAEDglbTrackLine;

class TAEDglbTrackLineList : public TEveElementList {

public:
   TAEDglbTrackLineList(const Char_t* name = "");
   virtual ~TAEDglbTrackLineList();
  
   //! return number of tracks
   Int_t GetNofTracks()     const { return fNofTracks; }
   
   //! Add Track
   TAEDglbTrackLine* NewTrack(const Char_t* label);
  
   //! Reset tracks
   void ResetTracks();
      
private:
   Int_t            fNofTracks;   // number of tracks
};


#endif
