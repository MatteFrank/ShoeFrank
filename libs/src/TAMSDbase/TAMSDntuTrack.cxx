#include "TMath.h"
#include "TAGgeoTrafo.hxx"
#include "TAMSDparGeo.hxx"
#include "TAMSDntuTrack.hxx"


ClassImp(TAMSDntuTrack);

TString TAMSDntuTrack::fgkBranchName   = "mstrack.";

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
   return fListOfTracks->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a Track for a given sensor
TAMSDtrack* TAMSDntuTrack::GetTrack(Int_t iTrack)
{
   if (iTrack >=0 || iTrack < GetTracksN())
	  return (TAMSDtrack*)fListOfTracks->At(iTrack);
   else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TAMSDtrack* TAMSDntuTrack::GetTrack(Int_t iTrack) const
{
   if (iTrack >=0 || iTrack < GetTracksN())
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
//  
TAMSDtrack* TAMSDntuTrack::NewTrack()
{
   TClonesArray &trackArray = *fListOfTracks;
   TAMSDtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAMSDtrack();
   return track;
}

//______________________________________________________________________________
//  
TAMSDtrack* TAMSDntuTrack::NewTrack(TAMSDtrack& trk)
{
   TClonesArray &trackArray = *fListOfTracks;
   TAMSDtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAMSDtrack(trk);
   return track;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAMSDntuTrack::ToStream(ostream& os, Option_t* option) const
{
	  
   os << "TAMSDntuTrack " << GetName()
   << Form("  nPixels=%3d", GetTracksN())
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

