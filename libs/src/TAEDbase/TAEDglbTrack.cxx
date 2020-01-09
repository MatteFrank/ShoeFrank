#include <Riostream.h>

#include "TEveTrackPropagator.h"
#include "TEveRGBAPalette.h"
#include "TColor.h"

#include "TAEDglbTrack.hxx"

//
ClassImp(TAEDglbTrack)

//__________________________________________________________
TAEDglbTrack::TAEDglbTrack(TEveRecTrackD* track,  TEveTrackPropagator* prop)
 : TEveTrack(track, prop)
{
}

//__________________________________________________________
TAEDglbTrack::~TAEDglbTrack()
{
  // default destructor
}

//__________________________________________________________
void TAEDglbTrack::AddTrackMarker(TVector3& point, TVector3& mom)
{
   AddPathMark(TEvePathMarkD(TEvePathMarkD::kReference,
                             TEveVectorD(point[0], point[1], point[2]),
                             TEveVectorD(mom[0], mom[1], mom[2])));
}
                              
//__________________________________________________________
void TAEDglbTrack::ResetTrack()
{
   Reset();
}

//__________________________________________________________
void TAEDglbTrack::TrackId(TObject* obj)
{
   fTrackId = obj;
}

//__________________________________________________________
void TAEDglbTrack::SecSelected(TEveTrack* track)
{
   printf("toto\n");
 //  Emit("SecSelected(TEveTrack*)", (Long_t)track);
}
