/*!
 \file
 \version $Id: TAVTntuTrack.cxx
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
TAVTtrack* TAVTntuTrack::GetTrack(Int_t iTrack)
{
   if (iTrack >=0 || iTrack < GetTracksN())
	  return (TAVTtrack*)fListOfTracks->At(iTrack);
   else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
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
//  
TAVTtrack* TAVTntuTrack::NewTrack()
{
   TClonesArray &trackArray = *fListOfTracks;
   TAVTtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAVTtrack();
   return track;
}

//______________________________________________________________________________
//  
TAVTtrack* TAVTntuTrack::NewTrack(TAVTtrack& trk)
{
   TClonesArray &trackArray = *fListOfTracks;
   TAVTtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAVTtrack(trk);
   return track;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
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

