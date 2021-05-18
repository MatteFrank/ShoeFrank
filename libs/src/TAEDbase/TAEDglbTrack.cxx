
#include "TEveChunkManager.h"

#include "TAEDglbTrack.hxx"

ClassImp(TAEDglbTrack);

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

//__________________________________________________________
void TAEDglbTrack::LineSecSelected(TEveStraightLineSet* qs, Int_t line)
 {
    Long_t args[2];
    args[0] = (Long_t) qs;
    args[1] = (Long_t) line;
   
    Emit("LineSecSelected(TEveStraightLineSet*, Int_t)", args);
 }

//__________________________________________________________
void TAEDglbTrack::LineSelected(Int_t line)
{
   //printf("Selected GlbTrack %d\n", line);
   LineSecSelected(this, line);
}
