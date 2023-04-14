/*!
 \file TAEDeveGlbTrackList.cxx
 \brief  Class to display global tracks on event.
 */

#include <Riostream.h>

#include "TEveTrackPropagator.h"
#include "TEveRGBAPalette.h"
#include "TColor.h"

#include "TADIeveTrackPropagator.hxx"
#include "TAEDeveGlbTrack.hxx"
#include "TAEDeveGlbTrackList.hxx"

/*!
 \class TAEDeveGlbTrackList
 \brief  Class to display global tracks on event.
 */

//__________________________________________________________
//! Default constructor
//!
//! \param[in] name track list name
//! \param[in] prop eve track propagator
TAEDeveGlbTrackList::TAEDeveGlbTrackList(const Char_t* name, TADIeveTrackPropagator* prop)
 : TEveTrackList(name, prop),
   fPalette(new TEveRGBAPalette()),
   fNofTracks(0),
   fMaxMomentum(0.)
{
   // Set propagator to Runge-Kutta
   fPropagator->SetStepper(TEveTrackPropagator::kRungeKutta);
}

//__________________________________________________________
//! Default destructor
TAEDeveGlbTrackList::~TAEDeveGlbTrackList()
{
   delete fPalette;
}

//__________________________________________________________
//! Add track
//!
//! \param[in] vertex a given vertex
//! \param[in] moment a given momentum
//! \param[in] charge a given charge
TAEDeveGlbTrack* TAEDeveGlbTrackList::AddTrack(TVector3& vertex, TVector3& moment, Int_t charge)
{
   TEveRecTrackD *recTrack = new TEveRecTrackD();
   recTrack->fV.Set(vertex[0], vertex[1], vertex[2]);
   recTrack->fP.Set(moment[0], moment[1], moment[2]);
   recTrack->fSign = charge;
   
   Float_t mag = moment.Mag();
   
   const UChar_t* pix = fPalette->ColorFromValue(mag);
   Int_t          ci  = TColor::GetColor(pix[0], pix[1], pix[2]);
   
   TAEDeveGlbTrack* track = new TAEDeveGlbTrack(recTrack, fPropagator);
   track->SetLineColor(ci);
   
   fNofTracks++;
   track->SetRnrLine(true);
   track->MakeTrack();

   AddElement(track);
   
   return track;
}

//__________________________________________________________
//! Reset tracks
void TAEDeveGlbTrackList::ResetTracks()
{
   RemoveElements();
   fNofTracks = 0;
}

//__________________________________________________________
//! Set maximum momentum
//!
//! \param[in] m momentum maximal 
void TAEDeveGlbTrackList::SetMaxMomentum(Float_t m)
{
   fPalette->SetMax(Int_t(m+0.5));
   fMaxMomentum = Int_t(m+0.5);
}
