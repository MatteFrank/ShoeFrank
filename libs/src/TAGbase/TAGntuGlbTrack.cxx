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
TAGtrack::TAGtrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof)
 : TAGobject(),
   fMass(mass),
   fMom(mom),
   fCharge(charge),
   fTof(tof),
   fTrkId(-1),
   fTgtDir(0,0,0),
   fTgtPos(0,0,0),
   fTofPos(0,0,0),
   fTofDir(0,0,0),
   fListOfMeasPoints(0x0),
   fListOfCorrPoints(0x0)
{
   SetupClones();
   fEnergy = TMath::Sqrt(mass*mass + mom*mom) - mass;
}

//______________________________________________________________________________
//
TAGtrack::TAGtrack(const TAGtrack& aTrack)
:  TAGobject(aTrack),
   fMass(aTrack.fMass),
   fMom(aTrack.fMom),
   fCharge(aTrack.fCharge),
   fTof(aTrack.fTof),
   fTrkId(aTrack.fTrkId),
   fTgtDir(aTrack.fTgtDir),
   fTgtPos(aTrack.fTgtPos),
   fTofPos(aTrack.fTofPos),
   fTofDir(aTrack.fTofDir)
{
   fListOfMeasPoints = (TClonesArray*)aTrack.fListOfMeasPoints->Clone();
   fListOfCorrPoints = (TClonesArray*)aTrack.fListOfCorrPoints->Clone();
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
TAGpoint* TAGtrack::AddMeasPoint(TAGpoint* point)
{
   TClonesArray &pointArray = *fListOfMeasPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(*point);
}

// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddCorrPoint(TAGpoint* point)
{
   TClonesArray &pointArray = *fListOfCorrPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(*point);
}

// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddMeasPoint(TString name, TVector3 pos, TVector3 posErr)
{
   TClonesArray &pointArray = *fListOfMeasPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(name, pos, posErr);
}

// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddMeasPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr)
{
   TClonesArray &pointArray = *fListOfMeasPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(pos, posErr, mom, momErr, fCharge);
}

// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddCorrPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr)
{
   TClonesArray &pointArray = *fListOfCorrPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(pos, posErr, mom, momErr, fCharge);
}

// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddMeasPoint(TString name, TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr)
{
  TClonesArray &pointArray = *fListOfMeasPoints;
  return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(name, pos, posErr, mom, momErr, fCharge);
}

// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddCorrPoint(TString name, TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr)
{
  TClonesArray &pointArray = *fListOfCorrPoints;
  return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(name, pos, posErr, mom, momErr, fCharge);
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAGtrack::Clear(Option_t*)
{
   fListOfMeasPoints->Delete();
   fListOfCorrPoints->Delete();
}

//______________________________________________________________________________
//
Double_t TAGtrack::GetTgtTheta() const
{
   TVector3 direction = fTgtDir.Unit();
   Double_t theta      = direction.Theta();
   
   return theta;
}

//______________________________________________________________________________
//
Double_t TAGtrack::GetTgtPhi() const
{
   TVector3 origin = fTgtDir.Unit();
   Double_t phi     = origin.Phi();
   
   return phi;
}

//______________________________________________________________________________
//
Double_t TAGtrack::GetTofTheta() const
{
   TVector3 direction = fTofDir.Unit();
   Double_t theta      = direction.Theta();
   
   return theta;
}

//______________________________________________________________________________
//
Double_t TAGtrack::GetTofPhi() const
{
   TVector3 origin = fTofDir.Unit();
   Double_t phi     = origin.Phi();
   
   return phi;
}


//______________________________________________________________________________
//
TVector3 TAGtrack::Intersection(Double_t posZ) const
{
   // calculates the Intersection of the Track with the plane in
   // the coordinate system of the tracker.
   
   TVector3 result(fTgtPos);  // track origin in xyz tracker coordinates
   result(2) = 0.;
   result += fTgtDir * posZ; // intersection in xyz frame at z_plane
   result(2) = posZ;

   return  result;
}

//______________________________________________________________________________
//
Double_t TAGtrack::Distance(TAGtrack* track, Float_t z) const
{
   // calculates the distance with a track
   TVector3 pos1 = Intersection(z);
   TVector3 pos2 = track->Intersection(z);
   TVector3 pos0 = pos1-pos2;
   Double_t rho0  = pos0.Mag();
   
   return rho0;
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
TAGtrack* TAGntuGlbTrack::NewTrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof)
{
   Int_t trkId = fListOfTracks->GetEntries();
   TClonesArray &trackArray = *fListOfTracks;
   TAGtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAGtrack(mass, mom, charge, tof);
   track->SetTrackId(trkId);
   
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

