/*!
 \file TAEDglbTrackList.cxx
 \brief   Class to display global tracks list on event.
 */

#include <Riostream.h>

#include "TAEDglbTrack.hxx"
#include "TAEDglbTrackList.hxx"

/*!
 \class TAEDglbTrackList
 \brief   Class to display global tracks list on event.
 */

//__________________________________________________________
//! Default constructor
//!
//! \param[in] name global track list name
TAEDglbTrackList::TAEDglbTrackList(const Char_t* name)
 : TEveElementList(name),
   fNofTracks(0)
{
}

//__________________________________________________________
//! default destructor
TAEDglbTrackList::~TAEDglbTrackList()
{
}

//__________________________________________________________
//! New global track
//!
//! \param[in] label label of the track
TAEDglbTrack* TAEDglbTrackList::NewTrack(const Char_t* label)
{
   TAEDglbTrack* track = new TAEDglbTrack(label);

   fNofTracks++;
   AddElement(track);
   
   return track;
}

//__________________________________________________________
//! Reset tracks
void TAEDglbTrackList::ResetTracks()
{
   RemoveElements();
   fNofTracks = 0;
}
