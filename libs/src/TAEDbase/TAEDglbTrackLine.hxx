#ifndef _TAEDglbTrackLine_HXX_
#define _TAEDglbTrackLine_HXX_

#include "TEveStraightLineSet.h"
#include "TVector3.h"

/** TAEDglbTrackLine a class to display global track line on event
 
 */

class TAEDglbTrackLine : public  TEveStraightLineSet
{
   
public:
   TAEDglbTrackLine(const Text_t* name);
   virtual ~TAEDglbTrackLine();
   
   //! Add line
   void AddTracklet(Int_t Z, Float_t x1, Float_t y1, Float_t z1, Float_t x2, Float_t y2, Float_t z2);
   void AddTracklet(Int_t Z, TVector3 pos1, TVector3 pos2);
   
   //! Set track id object
   void TrackId(TObject* obj);
   
   //! Reset lines
   void ResetTracklets();
   
   //! Refit plex
   void RefitPlex();
   
   //! Get track id object
   TObject* GetTrackId() const { return fTrackId; }
   
private:
   TObject* fTrackId;
   
};


#endif
