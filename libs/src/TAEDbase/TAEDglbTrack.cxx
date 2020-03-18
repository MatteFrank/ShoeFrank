#include <Riostream.h>

#include "TEveTrackPropagator.h"
#include "TEveRGBAPalette.h"
#include "TColor.h"

#include "TAEDglbTrack.hxx"

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
void TAEDglbTrack::AddTrackPoint(TVector3& point, TVector3& mom)
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
