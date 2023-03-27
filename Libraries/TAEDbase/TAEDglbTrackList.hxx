#ifndef _TAEDglbTrackList_HXX_
#define _TAEDglbTrackList_HXX_

/*!
 \file TAEDglbTrackList.hxx
 \brief  Class to display global tracks list on event
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TEveElement.h"
#include "TVector3.h"

class TAEDglbTrack;
class TAEDglbTrackList : public TEveElementList {

public:
   TAEDglbTrackList(const Char_t* name = "");
   virtual ~TAEDglbTrackList();
  
   //! Return number of tracks
   Int_t      GetNofTracks()     const { return fNofTracks; }
   
   // Add Track
   TAEDglbTrack* NewTrack(const Char_t* label);
  
   // Reset tracks
   void ResetTracks();
      
private:
   Int_t            fNofTracks;   ///< number of tracks
   
   ClassDef(TAEDglbTrackList, 0);
};

#endif
