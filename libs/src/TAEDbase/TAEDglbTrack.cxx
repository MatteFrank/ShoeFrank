
#include "TEveChunkManager.h"

#include "TAEDglbTrack.hxx"

//__________________________________________________________
TAEDglbTrack::TAEDglbTrack(const Text_t* name)
  : TEveStraightLineSet(name),
    fTrackId(0x0)
{ 
  // default constructor
}

//__________________________________________________________
TAEDglbTrack::~TAEDglbTrack()
{
   // default destructor
}

//__________________________________________________________
void TAEDglbTrack::AddTracklet(Int_t Z, Float_t x1, Float_t y1, Float_t z1,
                                   Float_t x2, Float_t y2, Float_t z2)
{
   AddLine(x1, y1, z1, x2, y2, z2);
   SetMarkerStyle(1+Z);
}

//__________________________________________________________
void TAEDglbTrack::AddTracklet(Int_t Z, TVector3 pos1, TVector3 pos2)
{
   AddLine(pos1[0], pos1[1], pos1[2], pos2[0], pos2[1], pos2[2]);
   SetMarkerStyle(1+Z);
}

//__________________________________________________________
void TAEDglbTrack::ResetTracklets()
{
   TEveChunkManager& p = GetLinePlex();
   p.Reset(sizeof(TEveStraightLineSet::Line_t), p.N());
}

//__________________________________________________________
void TAEDglbTrack::RefitPlex()
{
   TEveChunkManager& p = GetLinePlex();
   p.Refit();
}

//__________________________________________________________
void TAEDglbTrack::TrackId(TObject* obj)
{
   fTrackId = obj;
}
