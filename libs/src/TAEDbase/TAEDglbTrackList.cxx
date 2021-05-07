#include <Riostream.h>

#include "TAEDtrack.hxx"
#include "TAEDglbTrackList.hxx"

//__________________________________________________________
TAEDglbTrackList::TAEDglbTrackList(const Char_t* name)
 : TEveElementList(name),
   fNofTracks(0),
   fBoxDefWidth(0.5),
   fBoxDefHeight(0.5),
   fMaxEnergy(16)
{
}

//__________________________________________________________
TAEDglbTrackList::~TAEDglbTrackList()
{
  // default destructor
}

//__________________________________________________________
TAEDtrack* TAEDglbTrackList::NewTrack(const Char_t* label)
{
   TAEDtrack* track = new TAEDtrack(label);
   track->AddNewTrack();
   
   track->SetMaxEnergy(fMaxEnergy);
   track->SetDefWidth(fBoxDefWidth);
   track->SetDefHeight(fBoxDefHeight);
   track->SetPickable(true);
   
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
