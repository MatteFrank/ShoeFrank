/*!
  \file TAGntuGlbTrack.cxx
  \brief   Implementation of TAGntuGlbTrack.
*/

#include "TString.h"
#include "TVector3.h"

#include "TAGntuGlbTrack.hxx"

/*!
  \class TAGtrack
  \brief Global tracks **
*/

//! Class Imp
ClassImp(TAGtrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGtrack::TAGtrack()
 : TAGnamed(),
   fEvtNumber(-1),
   fPdgId(-1),
   fLength(-99.),
   fChi2(-99.),
   fNdof(-1),
   fPval(-99.),
   fQuality(-99),
   fMass(0.),
   fMomModule(0.),
   fTwChargeZ(0.),
   fTwTof(0.),
   fTrkId(-1),
   fFitMass(-99.),
   fFitChargeZ(-1),
   fFitTof(-99.),
   fFitEnergyLoss(-99.),
   fFitEnergy(-99.),
   fTgtDir(0,0,0),
   fTgtPos(0,0,0),
   fTgtPosError(0,0,0),
   fTgtMom(0,0,0),
   fTgtMomError(0,0,0),
   fTwPos(0,0,0),
   fTwPosError(0,0,0),
   fTwMom(0,0,0),
   fTwMomError(0,0,0),
   fListOfPoints(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Constructor.
//!
//! \param[in] mass mass of particle
//! \param[in] mom momentum of particle
//! \param[in] charge atomic charge of particle
//! \param[in] tof time of flight of particle
TAGtrack::TAGtrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof)
 : TAGnamed(),
   fEvtNumber(-1),
   fPdgId(-1),
   fLength(-99.),
   fChi2(-99.),
   fNdof(-1),
   fPval(-99.),
   fQuality(-99),
   fMass(mass),
   fMomModule(mom),
   fTwChargeZ(charge),
   fTwTof(tof),
   fTrkId(-1),
   fFitMass(-99.),
   fFitChargeZ(-1),
   fFitTof(-99.),
   fFitEnergyLoss(-99.),
   fTgtDir(0,0,0),
   fTgtPos(0,0,0),
   fTgtPosError(0,0,0),
   fTgtMom(0,0,0),
   fTgtMomError(0,0,0),
   fTwPos(0,0,0),
   fTwPosError(0,0,0),
   fTwMom(0,0,0),
   fTwMomError(0,0,0),
   fListOfPoints(0x0)
{
   SetupClones();
   fFitEnergy = TMath::Sqrt(mass*mass + mom*mom) - mass;
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] aTrack a track to copy
TAGtrack::TAGtrack(const TAGtrack& aTrack)
:  TAGnamed(aTrack),
   fEvtNumber(aTrack.fEvtNumber),
   fPdgId(aTrack.fPdgId),
   fLength(aTrack.fLength),
   fChi2(aTrack.fChi2),
   fNdof(aTrack.fNdof),
   fPval(aTrack.fPval),
   fQuality(aTrack.fQuality),
   fMass(aTrack.fMass),
   fMomModule(aTrack.fMomModule),
   fTwChargeZ(aTrack.fTwChargeZ),
   fTwTof(aTrack.fTwTof),
   fTrkId(aTrack.fTrkId),
   fFitMass(aTrack.fFitMass),
   fFitChargeZ(aTrack.fFitChargeZ),
   fFitTof(aTrack.fFitTof),
   fFitEnergyLoss(aTrack.fFitEnergyLoss),
   fFitEnergy(aTrack.fFitEnergy),
   fTgtDir(aTrack.fTgtDir),
   fTgtPos(aTrack.fTgtPos),
   fTgtPosError(aTrack.fTgtPosError),
   fTgtMom(aTrack.fTgtMom),
   fTgtMomError(aTrack.fTgtMomError),
   fTwPos(aTrack.fTwPos),
   fTwPosError(aTrack.fTwPosError),
   fTwMom(aTrack.fTwMom),
   fTwMomError(aTrack.fTwMomError)
{
   fListOfPoints = (TClonesArray*)aTrack.fListOfPoints->Clone();
}

//______________________________________________________________________________
//! Alternative constructor
//!
//! \param[in] name device name
//! \param[in] evNum event number
//! \param[in] pdgID PDG id
//! \param[in] startMass initial mass
//! \param[in] fitCh fitted atomic charge
//! \param[in] fitMass fitted mass
//! \param[in] length length of track
//! \param[in] tof time of flight of track
//! \param[in] chi2 of fitted track
//! \param[in] ndof of fitted track
//! \param[in] pVal of fitted track
//! \param[in] TgtPos track at target position
//! \param[in] TgtMom track momentum at target position
//! \param[in] TgtPos_cov track position matrix at target position
//! \param[in] TgtMom_cov track momentum matrix at target position
//! \param[in] TwPos track at TW position
//! \param[in] TwMom track momentum at TW position
//! \param[in] TwPos_cov track position matrix at TW position
//! \param[in] TwMom_cov track momentum matrix at TW position
//! \param[in] shoeTrackPointRepo array of points
TAGtrack::TAGtrack(string name, long evNum,
                   int pdgID, float startMass, int fitCh, float fitMass,
                   float length, float tof,
                   float chi2, int ndof, float pVal,
                   TVector3* TgtPos, TVector3* TgtMom,
                   TMatrixD* TgtPos_cov, TMatrixD* TgtMom_cov,
                   TVector3* TwPos, TVector3* TwMom,
                   TMatrixD* TwPos_cov, TMatrixD* TwMom_cov,
                   vector<TAGpoint*>* shoeTrackPointRepo)
	: TAGnamed(),
	fListOfPoints(0x0)
{
	SetupClones();

	fName = name;
	fEvtNumber = evNum;
	fPdgId = pdgID;
   fMass = startMass;
	fFitChargeZ = fitCh;
	fFitMass = fitMass;
	fLength = length;
	fFitTof = tof;
	fChi2 = chi2;
	fNdof = ndof;
	fPval = pVal;

	fTgtPos = *TgtPos;
	TVector3 temp = TVector3(pow( (*TgtPos_cov)(0,0),2), pow( (*TgtPos_cov)(1,1),2), pow( (*TgtPos_cov)(2,2),2) );
	fTgtPosError = temp;

	fTgtMom = *TgtMom;
	temp = TVector3(pow( (*TgtMom_cov)(0,0),2), pow( (*TgtMom_cov)(1,1),2), pow( (*TgtMom_cov)(2,2),2) );
	fTgtMomError = temp;
	fMomModule = TgtMom->Mag();
	fTgtDir = TgtMom->Unit();

	fTwPos = *TwPos;
	temp = TVector3(pow( (*TwPos_cov)(0,0),2), pow( (*TwPos_cov)(1,1),2), pow( (*TwPos_cov)(2,2),2) );
	fTwPosError = temp;

	fTwMom = *TwMom;
	temp = TVector3(pow( (*TwMom_cov)(0,0),2), pow( (*TwMom_cov)(1,1),2), pow( (*TwMom_cov)(2,2),2) );
	fTwMomError = temp;

	TClonesArray &pointArray = *fListOfPoints;
	for(int i=0; i < shoeTrackPointRepo->size(); ++i)	{

		new (pointArray[pointArray.GetEntriesFast()]) TAGpoint( *(shoeTrackPointRepo->at(i)) );
	}
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGtrack::~TAGtrack()
{
   delete fListOfPoints;
}

//______________________________________________________________________________
//! Set up clones
void TAGtrack::SetupClones()
{
   if (!fListOfPoints) fListOfPoints = new TClonesArray("TAGpoint");
}

// __________________________________________________________________________
//! Add point to list
//!
//! \param[in] point point to add
TAGpoint* TAGtrack::AddPoint(TAGpoint* point)
{
   TClonesArray &pointArray = *fListOfPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(*point);
}

//______________________________________________________________________________
//!  Add a point
//!
//! \param[in] measPos measured position
//! \param[in] measPosErr measured position error
//! \param[in] fitPos fitted position
//! \param[in] fitPosErr fitted position error
//! \param[in] mom momentum
//! \param[in] momErr momentum error
TAGpoint* TAGtrack::AddPoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr)
{
   TClonesArray &pointArray = *fListOfPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(measPos, measPosErr, fitPos, fitPosErr, mom, momErr);
}

//______________________________________________________________________________
//!  build a point
//!
//! \param[in] name device name
//! \param[in] measPos measured position
//! \param[in] measPosErr measured position error
TAGpoint* TAGtrack::AddPoint(TString name, TVector3 measPos, TVector3 measPosErr)
{
   TClonesArray &pointArray = *fListOfPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(name, measPos, measPosErr);
}

//______________________________________________________________________________
//!  build a point
//!
//! \param[in] name device name
//! \param[in] measPos measured position
//! \param[in] measPosErr measured position error
//! \param[in] fitPos fitted position
//! \param[in] fitPosErr fitted position error
TAGpoint* TAGtrack::AddPoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr)
{
   TClonesArray &pointArray = *fListOfPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(name, measPos, measPosErr, fitPos, fitPosErr);
}

//______________________________________________________________________________
//!  build a point
//!
//! \param[in] name device name
//! \param[in] measPos measured position
//! \param[in] measPosErr measured position error
//! \param[in] fitPos fitted position
//! \param[in] fitPosErr fitted position error
//! \param[in] mom momentum
//! \param[in] momErr momentum error
TAGpoint* TAGtrack::AddPoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr)
{
  TClonesArray &pointArray = *fListOfPoints;
  return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(name, measPos, measPosErr, fitPos, fitPosErr, mom, momErr);
}

//------------------------------------------+-----------------------------------
//! Clear event.
//!
//! \param[in] opt clear options
void TAGtrack::Clear(Option_t*)
{
   fListOfPoints->Delete();
   fListOfPoints->Delete();
}

//______________________________________________________________________________
//! Get theta angle at target
Double_t TAGtrack::GetTgtTheta() const
{
   TVector3 direction = fTgtDir.Unit();
   Double_t theta      = direction.Theta();

   return theta;
}

//______________________________________________________________________________
//! Get phi angle at target
Double_t TAGtrack::GetTgtPhi() const
{
   TVector3 origin = fTgtDir.Unit();
   Double_t phi     = origin.Phi();

   return phi;
}

//______________________________________________________________________________
//! calculates the Intersection of the Track with the plane
//!
//! \param[in] posZ Z position
TVector3 TAGtrack::Intersection(Double_t posZ) const
{
   TVector3 result(fTgtPos);  // track origin in xyz tracker coordinates
   result(2) = 0.;
   result += fTgtDir * posZ; // intersection in xyz frame at z_plane
   result(2) = posZ;

   return  result;
}

//______________________________________________________________________________
//! calculates the distance with a track at a given position z
//!
//! \param[in] track a given track
//! \param[in] z position in Z
Double_t TAGtrack::Distance(TAGtrack* track, Float_t z) const
{
   TVector3 pos1 = Intersection(z);
   TVector3 pos2 = track->Intersection(z);
   TVector3 pos0 = pos1-pos2;
   Double_t rho0  = pos0.Mag();

   return rho0;
}

//------------------------------------------+-----------------------------------
//! Get position of track for a given position in z
//!
//! \param[in] z position in Z
TVector3 TAGtrack::GetPosition( double z )
{
    return TVector3{
        fParameters.parameter_x[3] * z * z * z + fParameters.parameter_x[2] * z * z + fParameters.parameter_x[1] * z + fParameters.parameter_x[0],
        fParameters.parameter_y[1] * z + fParameters.parameter_y[0],
        z};
}

//------------------------------------------+-----------------------------------
//! Get total energy loss MSD+TW+CAL
Double_t TAGtrack::GetTotEnergyLoss() const
{
 Double_t energyLoss = 0.;

   for( Int_t iPoint = 0; iPoint < GetPointsN(); ++iPoint ) {
      const TAGpoint* point = GetPoint(iPoint);
      TString name = point->GetDevName();
      if (name.Contains("MSD"))
         energyLoss += point->GetEnergyLoss();
      if (name.Contains("TW"))
         energyLoss += point->GetEnergyLoss();
      if (name.Contains("CA"))
         energyLoss += point->GetEnergyLoss();
   }

   return energyLoss;
}

//------------------------------------------+-----------------------------------
//! Get total energy loss MSD
Double_t TAGtrack::GetMsdEnergyLoss() const
{
   Double_t energyLoss = 0.;

   for( Int_t iPoint = 0; iPoint < GetPointsN(); ++iPoint ) {
      const TAGpoint* point = GetPoint(iPoint);
      TString name = point->GetDevName();
      if (name.Contains("MSD"))
         energyLoss += point->GetEnergyLoss();
   }

   return energyLoss;
}

//------------------------------------------+-----------------------------------
//! Get total energy loss TW
Double_t TAGtrack::GetTwEnergyLoss() const
{
   Double_t energyLoss = 0.;

   for( Int_t iPoint = 0; iPoint < GetPointsN(); ++iPoint ) {
      const TAGpoint* point = GetPoint(iPoint);
      TString name = point->GetDevName();
      if (name.Contains("TW"))
         energyLoss += point->GetEnergyLoss();
   }

   return energyLoss;
}

//------------------------------------------+-----------------------------------
//! Get MC track indexes
TArrayI TAGtrack::GetMcTrackIdx()
{
   fMcTrackMap.clear();
   fMcTrackIdx.Set(0);
   for( Int_t iPoint = 0; iPoint < GetPointsN(); ++iPoint ) {
      const TAGpoint* point = GetPoint(iPoint);
      for( Int_t i = 0; i < point->GetMcTracksN(); ++i) {
         Int_t trackIdx = point->GetMcTrackIdx(i);
         if (fMcTrackMap[trackIdx] == 0) {
            fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
            fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackIdx;
            fMcTrackMap[trackIdx] = 1;
         }
      }
   }

   return fMcTrackIdx;
}

//##############################################################################

/*!
 \class TAGntuGlbTrack
 \brief container for global tracks **
 */

//! Class Imp
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
//! return a Track for a given index
//!
//! \param[in] iTrack track index
TAGtrack* TAGntuGlbTrack::GetTrack(Int_t iTrack)
{
   if (iTrack >=0 || iTrack < GetTracksN())
      return (TAGtrack*)fListOfTracks->At(iTrack);
   else
      return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a Track for a given index (const)
//!
//! \param[in] iTrack track index
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
//!
//! \param[in] opt clear option
void TAGntuGlbTrack::Clear(Option_t*)
{
   fListOfTracks->Delete();
}

//______________________________________________________________________________
//! Add new track for GenFit
//!
//! \param[in] name device name
//! \param[in] evNum event number
//! \param[in] pdgID PDG id
//! \param[in] pdgMass PDG mass
//! \param[in] measCh measured atomic charge
//! \param[in] mass fitted mass
//! \param[in] length length of track
//! \param[in] tof time of flight of track
//! \param[in] chi2 of fitted track
//! \param[in] ndof of fitted track
//! \param[in] pVal of fitted track
//! \param[in] recoPos_target track at target position
//! \param[in] recoMom_target track momentum at target position
//! \param[in] recoPos_target_cov track position matrix at target position
//! \param[in] recoMom_target_cov track momentum matrix at target position
//! \param[in] recoPos_Tw track at TW position
//! \param[in] recoMom_Tw track momentum at TW position
//! \param[in] recoPos_Tw_cov track position matrix at TW position
//! \param[in] recoMom_Tw_cov track momentum matrix at TW position
//! \param[in] shoeTrackPointRepo array of points
TAGtrack* TAGntuGlbTrack::NewTrack(string name, long evNum, int pdgID, float pdgMass, int measCh, float mass, float length, float tof, float chi2, int ndof, float pVal, TVector3* recoPos_target, TVector3* recoMom_target, TMatrixD* recoPos_target_cov, TMatrixD* recoMom_target_cov, TVector3* recoPos_Tw, TVector3* recoMom_Tw, TMatrixD* recoPos_Tw_cov, TMatrixD* recoMom_Tw_cov, vector<TAGpoint*>* shoeTrackPointRepo)
{
	TClonesArray &trackArray = *fListOfTracks;
	TAGtrack* track = new (trackArray[trackArray.GetEntriesFast()]) TAGtrack(name, evNum,
                                                                            pdgID, pdgMass, measCh, mass, length, tof, chi2, ndof, pVal,
                                                                            recoPos_target, recoMom_target, recoPos_target_cov, recoMom_target_cov,
                                                                            recoPos_Tw, recoMom_Tw, recoPos_Tw_cov, recoMom_Tw_cov,
                                                                            shoeTrackPointRepo);
	return track;
}

//______________________________________________________________________________
//! Add new track
//!
//! \param[in] mass mass of particle
//! \param[in] mom momentum of particle
//! \param[in] charge atomic charge of particle
//! \param[in] tof time of flight of particle
TAGtrack* TAGntuGlbTrack::NewTrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof)
{
   Int_t trkId = fListOfTracks->GetEntries();
   TClonesArray &trackArray = *fListOfTracks;
   TAGtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAGtrack(mass, mom, charge, tof);
   track->SetTrackId(trkId);

   return track;
}

//______________________________________________________________________________
//! Add new track
TAGtrack* TAGntuGlbTrack::NewTrack()
{
   TClonesArray &trackArray = *fListOfTracks;
   TAGtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAGtrack();
   return track;
}

//______________________________________________________________________________
//! Add new track from existing one
//!
//! \param[in] trk a track to copy
TAGtrack* TAGntuGlbTrack::NewTrack(TAGtrack& trk)
{
   TClonesArray &trackArray = *fListOfTracks;
   TAGtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TAGtrack(trk);
   return track;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
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
