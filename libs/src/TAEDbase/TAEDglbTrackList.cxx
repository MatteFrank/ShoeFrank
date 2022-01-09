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
TAEDglbTrack* TAEDglbTrackList::NewTrack(const Char_t* label)
{
   TAEDglbTrack* track = new TAEDglbTrack(label);

   fNofTracks++;
   AddElement(track);
   
   return track;
}

//__________________________________________________________
void TAEDglbTrackList::ResetTracks()
{
   RemoveElements();
   fNofTracks = 0;
}
