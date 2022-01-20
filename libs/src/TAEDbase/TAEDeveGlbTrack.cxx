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
//! Default constructor
//!
//! \param[in] track eve track
//! \param[in] prop eve track propagator
TAEDeveGlbTrack::TAEDeveGlbTrack(TEveRecTrackD* track,  TEveTrackPropagator* prop)
 : TEveTrack(track, prop)
{
}

//__________________________________________________________
//! default destructor
TAEDeveGlbTrack::~TAEDeveGlbTrack()
{
}

//__________________________________________________________
//! Add track point
//!
//! \param[in] point a given point
//! \param[in] mom a given momentum
void TAEDeveGlbTrack::AddTrackPoint(TVector3& point, TVector3& mom)
{
   AddPathMark(TEvePathMarkD(TEvePathMarkD::kReference,
                             TEveVectorD(point[0], point[1], point[2]),
                             TEveVectorD(mom[0], mom[1], mom[2])));
}
                              
//__________________________________________________________
//! Reset tracks
void TAEDeveGlbTrack::ResetTrack()
{
   Reset();
}

//__________________________________________________________
//! Associate object to track
//!
//! \param[in] obj object to associate
void TAEDeveGlbTrack::TrackId(TObject* obj)
{
   fTrackId = obj;
}
