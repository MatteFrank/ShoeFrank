/*!
 \file TAVTntuTrack.cxx
  \brief   Container class for VTX tracks
 */

#include "TMath.h"
#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx" 
#include "TAVTparGeo.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"

/*!
 \class TAVTntuTrack
 \brief  Container class for VTX tracks
 */

ClassImp(TAVTntuTrack);

TString TAVTntuTrack::fgkBranchName   = "vttrack.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTntuTrack::TAVTntuTrack() 
: TAGdata(),
  fListOfTracks(new TClonesArray("TAVTtrack")),
  fBeamPosition(),
  fPileup(false)
{
   fListOfTracks->SetOwner(true); 
   fBeamPosition.SetXYZ(0, 0, 0);
}
//------------------------------------------+-----------------------------------
//! Destructor.
TAVTntuTrack::~TAVTntuTrack() 
{
   delete fListOfTracks;
}

//------------------------------------------+-----------------------------------
//! return number of tracks
Int_t TAVTntuTrack::GetTracksN() const
{
   return fListOfTracks->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a Track for a given sensor
//!
//! \param[in] iTrack track index
TAVTtrack* TAVTntuTrack::GetTrack(Int_t iTrack)
{
   if (iTrack >=0 || iTrack < GetTracksN())
	  return (TAVTtrack*)fListOfTracks->At(iTrack);
   else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
// return a pixel for a given sensor (const)
//!
//! \param[in] iTrack track index
const TAVTtrack* TAVTntuTrack::GetTrack(Int_t iTrack) const
{
   if (iTrack >=0 || iTrack < GetTracksN())
	  return (TAVTtrack*)fListOfTracks->At(iTrack);
   else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAVTntuTrack::SetupClones()
{
   if (!fListOfTracks) {
	  fListOfTracks = new TClonesArray("TAVTtrack");
	  fListOfTracks->SetOwner(true);
   }
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAVTntuTrack::Clear(Option_t*)
{
   fListOfTracks->Delete();
}

//______________________________________________________________________________
//! Create new track
TAVTtrack* TAVTntuTrack::NewTrack()
{
   TClonesArray &trackArray = *fListOfTracks;
   TAVTtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAVTtrack();
   return track;
}

//______________________________________________________________________________
//! Create new track from copy constructor
//!
//! \param[in] trk a given track
TAVTtrack* TAVTntuTrack::NewTrack(TAVTtrack& trk)
{
   TClonesArray &trackArray = *fListOfTracks;
   TAVTtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAVTtrack(trk);
   return track;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
void TAVTntuTrack::ToStream(ostream& os, Option_t* option) const
{
	  
   os << "TAVTntuTrack " << GetName()
   << Form("  nPixels=%3d", GetTracksN())
   << endl;
   
   //TODO properly
   //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
   for (Int_t j = 0; j < GetTracksN(); j++) {
	  const TAVTtrack*  track = GetTrack(j);
	  if (track)
		 os << Form("%4d", track->GetTrackIdx());
	  os << endl;
   }
}

