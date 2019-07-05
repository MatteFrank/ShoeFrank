#ifndef _TAGglbTrackDisplay_HXX_
#define _TAGglbTrackDisplay_HXX_


#include "TEveTrack.h"
#include "TVector3.h"

/** TAGglbTrackDisplay a class to display global tracks on event
 
*/
class TEveRGBAPalette;

class TAGglbTrackDisplay : public TEveTrackList {

public:
   TAGglbTrackDisplay(const Char_t* name);
   virtual ~TAGglbTrackDisplay();
  
   //! return number of tracks
   Int_t GetNofTracks()     const { return fNofTracks; }
   
   //! Get Max MaxMomentum
   Float_t GetMaxMomentum() const { return fMaxMomentum; }

   //! Set Max MaxMomentum
   void SetMaxMomentum(Float_t m);
   
   //! Add Track
   void AddTrack(TVector3& vertex, TVector3& momentum, Int_t charge);
  
   //! Reset tracks
   void ResetTracks();
   
   //! make glb tracks
   void MakeGlbTracks();
   
private:
   TEveRGBAPalette* fPalette;     // color palette
   Int_t            fNofTracks;   // number of tracks
   Int_t            fMaxMomentum; // max momentum

  ClassDef(TAGglbTrackDisplay, 0);  
};


#endif
