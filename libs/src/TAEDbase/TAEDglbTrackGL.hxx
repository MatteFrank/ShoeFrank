#ifndef _TAEDglbTrackGL_HXX_
#define _TAEDglbTrackGL_HXX_

/*!
 \file TAEDglbTrackGL.hxx
 \brief  TAEDglbTrackGL a class to display global track line on event
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAEDglbTrack.hxx"
#include "TEveStraightLineSetGL.h"
#include "TVector3.h"

class TAEDglbTrackGL : public  TEveStraightLineSetGL
{
   
public:
   TAEDglbTrackGL();
   virtual ~TAEDglbTrackGL();
   
   //! Support secondary selection
   Bool_t SupportsSecondarySelect() const { return kTRUE; }
   //! Secondary selection active
   Bool_t AlwaysSecondarySelect()   const { return ((TAEDglbTrack*)fExternalObj)->GetAlwaysSecSelect(); }
   //! Process secondary selection
   void   ProcessSelection(TGLRnrCtx& rnrCtx, TGLSelectRecord& rec);

    ClassDef(TAEDglbTrackGL, 0);
};


#endif
