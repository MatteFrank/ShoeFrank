/*!
  \file
  \version $Id: TAGntuGlbTrack.cxx
  \brief   Implementation of TAGntuGlbTrack.
*/

#include <string.h>

#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>

#include "TString.h"

#include "TAGntuGlbTrack.hxx"

/*!
  \class TAGntuGlbTrack TAGntuGlbTrack.hxx "TAGntuGlbTrack.hxx"
  \brief Mapping and Geometry parameters for IR detectors. **
*/

ClassImp(TAGtrack);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGtrack::TAGtrack()
 : TAGobject(),
   fMass(0.),
   fMom(0.),
   fCharge(0.),
   fTof(0.),
   fEnergy(0.),
   fId(-1),
   fTrkID(-1),
   fTgtDir(0,0,0),
   fTgtPos(0,0,0),
   fTofPos(0,0,0),
   fTofDir(0,0,0),
   fListOfPoints(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Constructor.
TAGtrack::TAGtrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof, Double_t energy, Int_t id, Int_t trkID)
 : TAGobject(),
   fMass(mass),
   fMom(mom),
   fCharge(charge),
   fTof(tof),
   fEnergy(energy),
   fId(id),
   fTrkID(trkID),
   fTgtDir(0,0,0),
   fTgtPos(0,0,0),
   fTofPos(0,0,0),
   fTofDir(0,0,0),
   fListOfPoints(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGtrack::~TAGtrack()
{
  delete fListOfPoints;
}



//______________________________________________________________________________
//
void TAGtrack::SetupClones()
{
  if (!fListOfPoints) fListOfPoints = new TClonesArray("TAGpoint");
  return;
}


// __________________________________________________________________________
//
void TAGtrack::AddPoint(TAGpoint* point)
{
   TClonesArray &pointArray = *fListOfPoints;
   new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(*point);
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TAGtrack::Clear(Option_t*)
{
  fListOfPoints->Delete();
}

//##############################################################################

ClassImp(TAGntuGlbTrack);

TString TAGntuGlbTrack::fgkBranchName   = "glbtrack.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGntuGlbTrack::TAGntuGlbTrack()
 : TAGdata(),
   fListOfTracks(new TClonesArray("TAGtrack"))
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGntuGlbTrack::~TAGntuGlbTrack()
{
   delete fListOfTracks;
}

//------------------------------------------+-----------------------------------
//! return number of tracks
Int_t TAGntuGlbTrack::GetTracksN() const
{
   return fListOfTracks->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a Track for a given sensor
TAGtrack* TAGntuGlbTrack::GetTrack(Int_t iTrack)
{
   if (iTrack >=0 || iTrack < GetTracksN())
      return (TAGtrack*)fListOfTracks->At(iTrack);
   else
      return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TAGtrack* TAGntuGlbTrack::GetTrack(Int_t iTrack) const
{
   if (iTrack >=0 || iTrack < GetTracksN())
      return (TAGtrack*)fListOfTracks->At(iTrack);
   else
      return 0x0;
}


//------------------------------------------+-----------------------------------
//! Setup clones.
void TAGntuGlbTrack::SetupClones()
{
   if (!fListOfTracks) {
      fListOfTracks = new TClonesArray("TAGtrack");
      fListOfTracks->SetOwner(true);
   }
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAGntuGlbTrack::Clear(Option_t*)
{
   fListOfTracks->Delete();
}

//______________________________________________________________________________
//
TAGtrack* TAGntuGlbTrack::NewTrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof, Double_t energy, Int_t id, Int_t trkID)
{
   TClonesArray &trackArray = *fListOfTracks;
   TAGtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAGtrack(mass, mom, charge, tof, energy, id, trkID);
   return track;
}

//______________________________________________________________________________
//
TAGtrack* TAGntuGlbTrack::NewTrack()
{
   TClonesArray &trackArray = *fListOfTracks;
   TAGtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAGtrack();
   return track;
}


//______________________________________________________________________________
//
TAGtrack* TAGntuGlbTrack::NewTrack(TAGtrack& trk)
{
   TClonesArray &trackArray = *fListOfTracks;
   TAGtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAGtrack(trk);
   return track;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAGntuGlbTrack::ToStream(ostream& os, Option_t* option) const
{
	  
   os << "TAGntuGlbTrack " << GetName()
   << Form("  nPixels=%3d", GetTracksN())
   << endl;
   
   //TODO properly
   //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
   for (Int_t j = 0; j < GetTracksN(); j++) {
      const TAGtrack*  track = GetTrack(j);
      if (track)
         os << Form("%4d", track->GetId());
      os << endl;
   }
}

