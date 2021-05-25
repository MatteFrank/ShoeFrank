#ifndef _TAEDglbTrackGL_HXX_
#define _TAEDglbTrackGL_HXX_

#include "TAEDglbTrack.hxx"
#include "TEveStraightLineSetGL.h"
#include "TVector3.h"

/** TAEDglbTrackGL a class to display global track line on event
 
 */

class TAEDglbTrackGL : public  TEveStraightLineSetGL
{
   
public:
   TAEDglbTrackGL();
   virtual ~TAEDglbTrackGL();
   
   Bool_t SupportsSecondarySelect() const { return kTRUE; }
   Bool_t AlwaysSecondarySelect()   const { return ((TAEDglbTrack*)fExternalObj)->GetAlwaysSecSelect(); }
   void   ProcessSelection(TGLRnrCtx& rnrCtx, TGLSelectRecord& rec);

    ClassDef(TAEDglbTrackGL, 0);
};


#endif
