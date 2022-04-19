/*!
 \file  TAIRntuTrack.cxx
 \brief Simple container class for tracks with the associated clusters
 */

#include "TMath.h"
#include "TAGgeoTrafo.hxx"
#include "TAIRntuTrack.hxx"

/*!
 \class TAIRntuTrack
 \brief Simple container class for tracks with the associated clusters
 */

//! Class Imp
ClassImp(TAIRntuTrack);

TString TAIRntuTrack::fgkBranchName   = "irtrack.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAIRntuTrack::TAIRntuTrack()
: TAGdata(),
  fListOfTracks(new TClonesArray("TAIRtrack"))
{
   fListOfTracks->SetOwner(true); 
}
//------------------------------------------+-----------------------------------
//! Destructor.
TAIRntuTrack::~TAIRntuTrack()
{
   delete fListOfTracks;
}

//------------------------------------------+-----------------------------------
//! return number of tracks
Int_t TAIRntuTrack::GetTracksN() const
{
   return fListOfTracks->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a Track for a given index
//!
//! \param[in] iTrack track index
TAIRtrack* TAIRntuTrack::GetTrack(Int_t iTrack)
{
   if (iTrack >=0 || iTrack < GetTracksN())
	  return (TAIRtrack*)fListOfTracks->At(iTrack);
   else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a Track for a given index (const)
//!
//! \param[in] iTrack track index
const TAIRtrack* TAIRntuTrack::GetTrack(Int_t iTrack) const
{
   if (iTrack >=0 || iTrack < GetTracksN())
	  return (TAIRtrack*)fListOfTracks->At(iTrack);
   else
	  return 0x0;
}


//------------------------------------------+-----------------------------------
//! Setup clones.
void TAIRntuTrack::SetupClones()
{
   if (!fListOfTracks) {
	  fListOfTracks = new TClonesArray("TAIRtrack");
	  fListOfTracks->SetOwner(true);
   }
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAIRntuTrack::Clear(Option_t*)
{
   fListOfTracks->Delete();
}

//______________________________________________________________________________
//! Create new track
TAIRtrack* TAIRntuTrack::NewTrack()
{
   TClonesArray &trackArray = *fListOfTracks;
   TAIRtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAIRtrack();
   return track;
}

//______________________________________________________________________________
//! Create new track from copy constructor
//!
//! \param[in] trk a given track
TAIRtrack* TAIRntuTrack::NewTrack(TAIRtrack& trk)
{
   TClonesArray &trackArray = *fListOfTracks;
   TAIRtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAIRtrack(trk);
   return track;
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAIRntuTrack::ToStream(ostream& os, Option_t* option) const
{
	  
   os << "TAIRntuTrack " << GetName()
   << Form("  nPixels=%3d", GetTracksN())
   << endl;
   
   //TODO properly
   //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
   for (Int_t j = 0; j < GetTracksN(); j++) {
	  const TAIRtrack*  track = GetTrack(j);
	  if (track)
		 os << Form("%4d", track->GetTrackIdx());
	  os << endl;
   }
}

