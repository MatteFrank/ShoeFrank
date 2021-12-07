/*!
 \file TAEDeveGlbTrack.cxx
 \brief  class to display global tracks on event.
 */

#include <Riostream.h>

#include "TEveTrackPropagator.h"
#include "TEveRGBAPalette.h"
#include "TColor.h"

#include "TAEDeveGlbTrack.hxx"

/*!
 \class TAEDeveGlbTrack
 \brief  class to display global tracks on event.
 */

//__________________________________________________________
TAEDeveGlbTrack::TAEDeveGlbTrack(TEveRecTrackD* track,  TEveTrackPropagator* prop)
 : TEveTrack(track, prop)
{
}

//__________________________________________________________
TAEDeveGlbTrack::~TAEDeveGlbTrack()
{
  // default destructor
}

//__________________________________________________________
void TAEDeveGlbTrack::AddTrackPoint(TVector3& point, TVector3& mom)
{
   AddPathMark(TEvePathMarkD(TEvePathMarkD::kReference,
                             TEveVectorD(point[0], point[1], point[2]),
                             TEveVectorD(mom[0], mom[1], mom[2])));
}
                              
//__________________________________________________________
void TAEDeveGlbTrack::ResetTrack()
{
   Reset();
}

//__________________________________________________________
void TAEDeveGlbTrack::TrackId(TObject* obj)
{
   fTrackId = obj;
}
