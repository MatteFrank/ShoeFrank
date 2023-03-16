/*!
 \file TAMSDntuTrack.cxx
 \brief   Implementation of TAMSDntuTrack.
 */

#include "TMath.h"
#include "TAGgeoTrafo.hxx"
#include "TAMSDparGeo.hxx"
#include "TAMSDntuTrack.hxx"

/*!
 \class TAMSDntuTrack
 \brief track container class for MSD
 */

//! Class imp
ClassImp(TAMSDntuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMSDntuTrack::TAMSDntuTrack()
: TAGdata(),
  fListOfTracks(new TClonesArray("TAMSDtrack")),
  fBeamPosition(),
  fPileup(false)
{
   fListOfTracks->SetOwner(true); 
   fBeamPosition.SetXYZ(0, 0, 0);
}
//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDntuTrack::~TAMSDntuTrack()
{
   delete fListOfTracks;
}

//------------------------------------------+-----------------------------------
//! return number of tracks
Int_t TAMSDntuTrack::GetTracksN() const
{
   return fListOfTracks->GetEntriesFast();
}

//------------------------------------------+-----------------------------------
//! return a Track for a given sensor
//!
//! \param[in] iTrack track index
TAMSDtrack* TAMSDntuTrack::GetTrack(Int_t iTrack)
{
   if (iTrack >=0 && iTrack < GetTracksN())
	  return (TAMSDtrack*)fListOfTracks->At(iTrack);
   else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a Track for a given sensor (const)
//!
//! \param[in] iTrack track index
const TAMSDtrack* TAMSDntuTrack::GetTrack(Int_t iTrack) const
{
   if (iTrack >=0 && iTrack < GetTracksN())
	  return (TAMSDtrack*)fListOfTracks->At(iTrack);
   else
	  return 0x0;
}


//------------------------------------------+-----------------------------------
//! Setup clones.
void TAMSDntuTrack::SetupClones()
{
   if (!fListOfTracks) {
	  fListOfTracks = new TClonesArray("TAMSDtrack");
	  fListOfTracks->SetOwner(true);
   }
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAMSDntuTrack::Clear(Option_t*)
{
   fListOfTracks->Delete();
}

//______________________________________________________________________________
//! New track
TAMSDtrack* TAMSDntuTrack::NewTrack()
{
   TClonesArray &trackArray = *fListOfTracks;
   TAMSDtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAMSDtrack();
   return track;
}

//------------------------------------------+-----------------------------------
//! New track from copy constructor
//!
//! \param[in] trk track to copy
TAMSDtrack* TAMSDntuTrack::NewTrack(TAMSDtrack& trk)
{
   TClonesArray &trackArray = *fListOfTracks;
   TAMSDtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAMSDtrack(trk);
   return track;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAMSDntuTrack::ToStream(ostream& os, Option_t* option) const
{
   os << "TAMSDntuTrack " << GetName()
   << Form("  nStripss=%3d", GetTracksN())
   << endl;
   
   //TODO properly
   //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
   for (Int_t j = 0; j < GetTracksN(); j++) {
	  const TAMSDtrack*  track = GetTrack(j);
	  if (track)
		 os << Form("%4d", track->GetTrackIdx());
	  os << endl;
   }
}

