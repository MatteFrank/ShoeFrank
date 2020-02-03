#ifndef _TAEDglbTrackList_HXX_
#define _TAEDglbTrackList_HXX_


#include "TEveTrack.h"
#include "TVector3.h"

/** TAEDglbTrackList a class to display global tracks on event
 
*/
class TEveRGBAPalette;
class TADIeveTrackPropagator;
class TAEDglbTrack;

class TAEDglbTrackList : public TEveTrackList {

public:
   TAEDglbTrackList(const Char_t* name = "", TADIeveTrackPropagator* prop = 0x0);
   virtual ~TAEDglbTrackList();
  
   //! return number of tracks
   Int_t GetNofTracks()     const { return fNofTracks; }
   
   //! Get Max MaxMomentum
   Float_t GetMaxMomentum() const { return fMaxMomentum; }

   //! Set Max MaxMomentum
   void SetMaxMomentum(Float_t m);
   
   //! Add Track
   TAEDglbTrack* AddTrack(TVector3& vertex, TVector3& momentum, Int_t charge);
  
   //! Reset tracks
   void ResetTracks();
      
private:
   TEveRGBAPalette* fPalette;     // color palette
   Int_t            fNofTracks;   // number of tracks
   Int_t            fMaxMomentum; // max momentum

  ClassDef(TAEDglbTrackList, 0);
};


#endif
