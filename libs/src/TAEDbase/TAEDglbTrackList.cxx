#include <Riostream.h>

#include "TAEDglbTrack.hxx"
#include "TAEDglbTrackList.hxx"

//__________________________________________________________
TAEDglbTrackList::TAEDglbTrackList(const Char_t* name)
 : TEveElementList(name),
   fNofTracks(0)
{
}

//__________________________________________________________
TAEDglbTrackList::~TAEDglbTrackList()
{
  // default destructor
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
