#include <Riostream.h>

#include "TAEDglbTrackLine.hxx"
#include "TAEDglbTrackLineList.hxx"

//__________________________________________________________
TAEDglbTrackLineList::TAEDglbTrackLineList(const Char_t* name)
 : TEveElementList(name),
   fNofTracks(0)
{
}

//__________________________________________________________
TAEDglbTrackLineList::~TAEDglbTrackLineList()
{
  // default destructor
}

//__________________________________________________________
TAEDglbTrackLine* TAEDglbTrackLineList::NewTrack(const Char_t* label)
{
   TAEDglbTrackLine* track = new TAEDglbTrackLine(label);
   fNofTracks++;
   AddElement(track);
   
   return track;
}

//__________________________________________________________
void TAEDglbTrackLineList::ResetTracks()
{
   RemoveElements();
   fNofTracks = 0;
}
