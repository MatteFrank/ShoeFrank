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

//! Class Imp
ClassImp(TAEDglbTrackGL);

//__________________________________________________________
//! default constructor
TAEDglbTrackGL::TAEDglbTrackGL()
  : TEveStraightLineSetGL()
{
}

//__________________________________________________________
//! default destructor
TAEDglbTrackGL::~TAEDglbTrackGL()
{
}

//------------------------------------------+-----------------------------------
//! Process selection interface with GL
//!
//! \param[in] rnrCtx GL renderer (not used)
//! \param[in] rec selected track
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
