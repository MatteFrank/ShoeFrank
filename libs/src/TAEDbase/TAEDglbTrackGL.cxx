/*!
 \file TAEDglbTrackGL.cxx
 \brief  Class to display global track line on event.
 */

#include "TGLSelectRecord.h"
#include "TEveChunkManager.h"

#include "TAEDglbTrackGL.hxx"

/*!
 \class TAEDglbTrackGL
 \brief  Class to display global track line on event.
 */


ClassImp(TAEDglbTrackGL);

//__________________________________________________________
TAEDglbTrackGL::TAEDglbTrackGL()
  : TEveStraightLineSetGL()
{
  // default constructor
}

//__________________________________________________________
TAEDglbTrackGL::~TAEDglbTrackGL()
{
   // default destructor
}


//__________________________________________________________
void TAEDglbTrackGL::ProcessSelection(TGLRnrCtx& /*rnrCtx*/, TGLSelectRecord& rec)
{
   TAEDglbTrack &LS = * (TAEDglbTrack*)fExternalObj;

   if (rec.GetN() != 3) return;
   if (rec.GetItem(1) == 1) {
      LS.LineSelected(rec.GetItem(1));
   } else {
      LS.MarkerSelected(rec.GetItem(2));
   }
}
