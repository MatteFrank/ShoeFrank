/*!
 \file TAITntuTrack.cxx
 \brief   Container class for ITR tracks
 */

#include "TMath.h"
#include "TAGgeoTrafo.hxx"
#include "TAITparGeo.hxx"
#include "TAITntuTrack.hxx"

/*!
 \class TAITntuTrack
 \brief  Container class for ITR tracks
 */

//! Class Imp
ClassImp(TAITntuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAITntuTrack::TAITntuTrack() 
: TAGdata(),
  fListOfTracks(new TClonesArray("TAITtrack")),
  fBeamPosition(),
  fPileup(false)
{
   fListOfTracks->SetOwner(true); 
   fBeamPosition.SetXYZ(0, 0, 0);
}
//------------------------------------------+-----------------------------------
//! Destructor.
TAITntuTrack::~TAITntuTrack() 
{
   delete fListOfTracks;
}

//------------------------------------------+-----------------------------------
//! return number of tracks
Int_t TAITntuTrack::GetTracksN() const
{
   return fListOfTracks->GetEntriesFast();
}

//------------------------------------------+-----------------------------------
// return a pixel for a given sensor
//!
//! \param[in] iTrack track index
TAITtrack* TAITntuTrack::GetTrack(Int_t iTrack)
{
   if (iTrack >=0 && iTrack < GetTracksN())
	  return (TAITtrack*)fListOfTracks->At(iTrack);
   else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
// return a pixel for a given sensor (const)
//!
//! \param[in] iTrack track index
const TAITtrack* TAITntuTrack::GetTrack(Int_t iTrack) const
{
   if (iTrack >=0 && iTrack < GetTracksN())
	  return (TAITtrack*)fListOfTracks->At(iTrack);
   else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAITntuTrack::SetupClones()
{
   if (!fListOfTracks) {
	  fListOfTracks = new TClonesArray("TAITtrack");
	  fListOfTracks->SetOwner(true);
   }
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAITntuTrack::Clear(Option_t*)
{
   fListOfTracks->Delete();
}

//______________________________________________________________________________
//! Create new track
TAITtrack* TAITntuTrack::NewTrack()
{
   TClonesArray &trackArray = *fListOfTracks;
   TAITtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAITtrack();
   return track;
}

//______________________________________________________________________________
//! Create new track from copy constructor
//!
//! \param[in] trk a given track
TAITtrack* TAITntuTrack::NewTrack(TAITtrack& trk)
{
   TClonesArray &trackArray = *fListOfTracks;
   TAITtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAITtrack(trk);
   return track;
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAITntuTrack::ToStream(ostream& os, Option_t* option) const
{
	  
   os << "TAITntuTrack " << GetName()
   << Form("  nPixels=%3d", GetTracksN())
   << endl;
   
   //TODO properly
   //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
   for (Int_t j = 0; j < GetTracksN(); j++) {
	  const TAITtrack*  track = GetTrack(j);
	  if (track)
		 os << Form("%4d", track->GetTrackIdx());
	  os << endl;
   }
}

