#ifndef _TAEDglbTrack_HXX_
#define _TAEDglbTrack_HXX_


#include "TEveTrack.h"
#include "TVector3.h"

/** TAEDglbTrack a class to display global tracks on event
 
*/
class TEveRGBAPalette;
class TADIeveTrackPropagator;
class TEveTrack;

class TAEDglbTrack : public TEveTrackList {

public:
   TAEDglbTrack(const Char_t* name, TADIeveTrackPropagator* prop);
   virtual ~TAEDglbTrack();
  
   //! return number of tracks
   Int_t GetNofTracks()     const { return fNofTracks; }
   
   //! Get Max MaxMomentum
   Float_t GetMaxMomentum() const { return fMaxMomentum; }

   //! Set Max MaxMomentum
   void SetMaxMomentum(Float_t m);
   
   //! Add Track
   void AddTrack(TVector3& vertex, TVector3& momentum, Int_t charge);
  
   //! Add track Marker
   void AddTrackMarker(TVector3& point, TVector3& mom);


   //! Reset tracks
   void ResetTracks();
   
   //! make glb tracks
   void MakeGlbTracks();
   
private:
   TEveRGBAPalette* fPalette;     // color palette
   TEveTrack*       fCurrentTrack; // current track
   Int_t            fNofTracks;   // number of tracks
   Int_t            fMaxMomentum; // max momentum

  ClassDef(TAEDglbTrack, 0);  
};


#endif
