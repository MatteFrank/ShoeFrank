/*!
  \file
  \version $Id: TAGntuGlbTrack.cxx
  \brief   Implementation of TAGntuGlbTrack.
*/

#include "TString.h"
#include "TVector3.h"

#include "TAGntuGlbTrack.hxx"

/*!
  \class TAGntuGlbTrack TAGntuGlbTrack.hxx "TAGntuGlbTrack.hxx"
  \brief container for global tracks **
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
   fTrkId(-1),
   fTgtDir(0,0,0),
   fTgtPos(0,0,0),
   fTofPos(0,0,0),
   fTofDir(0,0,0),
   fListOfMeasPoints(0x0),
   fListOfCorrPoints(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Constructor.
TAGtrack::TAGtrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof, Double_t energy, Int_t trkId)
 : TAGobject(),
   fMass(mass),
   fMom(mom),
   fCharge(charge),
   fTof(tof),
   fEnergy(energy),
   fTrkId(trkId),
   fTgtDir(0,0,0),
   fTgtPos(0,0,0),
   fTofPos(0,0,0),
   fTofDir(0,0,0),
   fListOfMeasPoints(0x0),
   fListOfCorrPoints(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGtrack::~TAGtrack()
{
   delete fListOfMeasPoints;
   delete fListOfCorrPoints;
}

//______________________________________________________________________________
//
void TAGtrack::SetupClones()
{
   if (!fListOfMeasPoints) fListOfMeasPoints = new TClonesArray("TAGpoint");
   if (!fListOfCorrPoints) fListOfCorrPoints = new TClonesArray("TAGpoint");
}

// __________________________________________________________________________
//
void TAGtrack::AddMeasPoint(TAGpoint* point)
{
   TClonesArray &pointArray = *fListOfMeasPoints;
   new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(*point);
}

// __________________________________________________________________________
//
void TAGtrack::AddCorrPoint(TAGpoint* point)
{
   TClonesArray &pointArray = *fListOfCorrPoints;
   new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(*point);
}

// __________________________________________________________________________
//
void TAGtrack::AddMeasPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr)
{
   TClonesArray &pointArray = *fListOfMeasPoints;
   new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(pos, posErr, mom, momErr, fCharge);
}

// __________________________________________________________________________
//
void TAGtrack::AddCorrPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr)
{
   TClonesArray &pointArray = *fListOfCorrPoints;
   new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(pos, posErr, mom, momErr, fCharge);
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TAGtrack::Clear(Option_t*)
{
   fListOfMeasPoints->Delete();
   fListOfCorrPoints->Delete();
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
TAGtrack* TAGntuGlbTrack::NewTrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof, Double_t energy, Int_t trkId)
{
   TClonesArray &trackArray = *fListOfTracks;
   TAGtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAGtrack(mass, mom, charge, tof, energy, trkId);
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

//______________________________________________________________________________
//! ostream insertion.
void TAGntuGlbTrack::ToStream(ostream& os, Option_t* option) const
{
   os << "TAGntuGlbTrack " << GetName()
   << Form("  nPixels=%3d", GetTracksN())
   << endl;
   
   //TODO properly
   for (Int_t j = 0; j < GetTracksN(); j++) {
      const TAGtrack*  track = GetTrack(j);
      if (track)
         os << Form("%4d", track->GetTrackId());
      os << endl;
   }
}

