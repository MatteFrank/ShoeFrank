/*!
 \file TAEDglbTrack.cxx
 \brief  Implementation of TAEDglbTrack
 */

#include "TEveChunkManager.h"

#include "TAEDglbTrack.hxx"

/*!
 \class TAEDglbTrack
 \brief  Class to display global track line on event.
 */

//! Class Imp
ClassImp(TAEDglbTrack);

//__________________________________________________________
//! default constructor
//!
//! \param[in] name global track name
TAEDglbTrack::TAEDglbTrack(const Text_t* name)
  : TEveStraightLineSet(name),
    fTrackId(0x0)
{ 
}

//__________________________________________________________
//! default destructor
TAEDglbTrack::~TAEDglbTrack()
{
}

//------------------------------------------+-----------------------------------
//! Add tracklet
//!
//! \param[in] Z atomic charge
//! \param[in] x1 initial position in X-direction
//! \param[in] y1 initial position in Y-direction
//! \param[in] z1 initial position in Z-direction
//! \param[in] x2 final position in X-direction
//! \param[in] y2 final position in Y-direction
//! \param[in] z2 final position in Z-direction
void TAEDglbTrack::AddTracklet(Int_t Z, Float_t x1, Float_t y1, Float_t z1,
                                   Float_t x2, Float_t y2, Float_t z2)
{
   AddLine(x1, y1, z1, x2, y2, z2);
   SetMarkerStyle(1+Z);
}

//------------------------------------------+-----------------------------------
//! Add tracklet
//!
//! \param[in] Z atomic charge
//! \param[in] pos1 initial position
//! \param[in] pos2 final position
void TAEDglbTrack::AddTracklet(Int_t Z, TVector3 pos1, TVector3 pos2)
{
   AddLine(pos1[0], pos1[1], pos1[2], pos2[0], pos2[1], pos2[2]);
   SetMarkerStyle(1+Z);
}

//__________________________________________________________
//! Reset tracklets
void TAEDglbTrack::ResetTracklets()
{
   TEveChunkManager& p = GetLinePlex();
   p.Reset(sizeof(TEveStraightLineSet::Line_t), p.N());
}

//__________________________________________________________
//! Reset basket
void TAEDglbTrack::RefitPlex()
{
   TEveChunkManager& p = GetLinePlex();
   p.Refit();
}

//__________________________________________________________
//! Associate object to track
//!
//! \param[in] obj object to associate
void TAEDglbTrack::TrackId(TObject* obj)
{
   fTrackId = obj;
}

//__________________________________________________________
//! Line second selection
//!
//! \param[in] qs straight line set
//! \param[in] line line index
void TAEDglbTrack::LineSecSelected(TEveStraightLineSet* qs, Int_t line)
 {
    Long_t args[2];
    args[0] = (Long_t) qs;
    args[1] = (Long_t) line;
   
    Emit("LineSecSelected(TEveStraightLineSet*, Int_t)", args);
 }

//__________________________________________________________
//! Line selected
//!
//! \param[in] line line index
void TAEDglbTrack::LineSelected(Int_t line)
{
   LineSecSelected(this, line);
}

//__________________________________________________________
//! Marker second selection
//!
//! \param[in] qs straight line set
//! \param[in] line line index
void TAEDglbTrack::MarkerSecSelected(TEveStraightLineSet* qs, Int_t line)
{
   Long_t args[2];
   args[0] = (Long_t) qs;
   args[1] = (Long_t) line;
   
   Emit("MarkerSecSelected(TEveStraightLineSet*, Int_t)", args);
}

//__________________________________________________________
//! Marker selected
//!
//! \param[in] marker marker index
void TAEDglbTrack::MarkerSelected(Int_t marker)
{
   MarkerSecSelected(this, marker);
}
