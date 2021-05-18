#include "TGLSelectRecord.h"
#include "TEveChunkManager.h"

#include "TAEDglbTrackGL.hxx"

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
      TEveStraightLineSet::Marker_t& m = * (TEveStraightLineSet::Marker_t*) fM->GetMarkerPlex().Atom(rec.GetItem(2));
      printf("Selected point %d on line %d\n", rec.GetItem(2), m.fLineId);
   }
}
