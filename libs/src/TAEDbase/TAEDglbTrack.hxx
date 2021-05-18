#ifndef _TAEDglbTrack_HXX_
#define _TAEDglbTrack_HXX_

#include "TEveStraightLineSet.h"
#include "TEveSecondarySelectable.h"

#include "TVector3.h"

/** TAEDglbTrack a class to display global track line on event
 
 */

class TAEDglbTrack : public  TEveStraightLineSet, public TEveSecondarySelectable
{

public:
   TAEDglbTrack(const Text_t* name);
   virtual ~TAEDglbTrack();
   
   //! Add line
   void AddTracklet(Int_t Z, Float_t x1, Float_t y1, Float_t z1, Float_t x2, Float_t y2, Float_t z2);
   void AddTracklet(Int_t Z, TVector3 pos1, TVector3 pos2);
   
   //! Set track id object
   void TrackId(TObject* obj);
   
   //! Reset lines
   void ResetTracklets();
   
   //! Refit plex
   void RefitPlex();
   
   // secondary selection
   void LineSecSelected(TEveStraightLineSet* qs, Int_t idx); // *SIGNAL*
   void LineSelected(Int_t line);

   //! Get track id object
   TObject* GetTrackId() const { return fTrackId; }
   
private:
   TObject* fTrackId;
   
   ClassDef(TAEDglbTrack, 0);

};


#endif
