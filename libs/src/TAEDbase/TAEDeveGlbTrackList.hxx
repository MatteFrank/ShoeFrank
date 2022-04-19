#ifndef _TAEDeveGlbTrackList_HXX_
#define _TAEDeveGlbTrackList_HXX_

/*!
 \file TAEDeveGlbTrackList.hxx
 \brief  TAEDeveGlbTrackList a class to display global tracks on event
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TEveTrack.h"
#include "TVector3.h"

class TEveRGBAPalette;
class TADIeveTrackPropagator;
class TAEDglbTrack;

class TAEDeveGlbTrackList : public TEveTrackList {

public:
   TAEDeveGlbTrackList(const Char_t* name = "", TADIeveTrackPropagator* prop = 0x0);
   virtual ~TAEDeveGlbTrackList();
  
   //! return number of tracks
   Int_t GetNofTracks()     const { return fNofTracks; }
   
   //! Get Max MaxMomentum
   Float_t GetMaxMomentum() const { return fMaxMomentum; }

   // Set Max MaxMomentum
   void SetMaxMomentum(Float_t m);
   
   // Add Track
   TAEDeveGlbTrack* AddTrack(TVector3& vertex, TVector3& momentum, Int_t charge);
  
   // Reset tracks
   void ResetTracks();
      
private:
   TEveRGBAPalette* fPalette;     ///< color palette
   Int_t            fNofTracks;   ///< number of tracks
   Int_t            fMaxMomentum; ///< max momentum
};

#endif
