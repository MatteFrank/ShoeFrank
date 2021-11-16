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
   fTwPos(0,0,0),
   fTwDir(0,0,0),
   fListOfPoints(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Constructor.
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
   fTgtMom(0,0,0),
   fTwPos(0,0,0),
   fTwDir(0,0,0),
   fTwMom(0,0,0),
   fListOfPoints(0x0)
{
   SetupClones();
   fFitEnergy = TMath::Sqrt(mass*mass + mom*mom) - mass;
}

//______________________________________________________________________________
//
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
   fTgtMom(aTrack.fTgtMom),
   fTwPos(aTrack.fTwPos),
   fTwDir(aTrack.fTwDir),
   fTwMom(aTrack.fTwMom)
{
   fListOfPoints = (TClonesArray*)aTrack.fListOfPoints->Clone();
}




//Alternative constructor
TAGtrack::TAGtrack( string name, long evNum, 
								int pdgID, int pdgCh, int measCh, float mass, 
								float length, float tof, 
								float chi2, int ndof, float pVal, 
								TVector3* pos, TVector3* mom,
								TMatrixD* pos_cov, TMatrixD* mom_cov,
								vector<TAGpoint*>* shoeTrackPointRepo 
					) 
	: TAGnamed(),
	fListOfPoints(0x0)
{

	SetupClones();

	fName = name;
	fEvtNumber = evNum;
	fPdgId = pdgID;
	// fCharge = pdgCh;
	fFitChargeZ = measCh;
	fFitMass = mass;
	fLength = length;
	fFitTof = tof;
	fChi2 = chi2;
	fNdof = ndof;
	fPval = pVal;
	// m_stateID = stateID;
	fTgtMom = *mom;
	fTgtPos = *pos;

	// fMom = m_target_mom.Mag();



	TClonesArray &pointArray = *fListOfPoints;
	for(int i=0; i < shoeTrackPointRepo->size(); ++i)	{

		new (pointArray[pointArray.GetEntriesFast()]) TAGpoint( *(shoeTrackPointRepo->at(i)) );
	}

	// m_target_pos_cov = *pos_cov;
	// m_target_mom_cov = *mom_cov;
}



void TAGtrack::SetMCInfo( int MCparticle_id, float trackQuality ) {

	fMcTrackIdx = MCparticle_id;
	fQuality = trackQuality;

}


void TAGtrack::SetExtrapInfoTW( TVector3* pos, TVector3* mom, TMatrixD* pos_cov, TMatrixD* mom_cov ) {

   fTwMom = *mom;
	fTwPos = *pos;
	// m_pos_TW_cov = *pos_cov;
	// m_mom_TW_cov = *mom_cov;
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
}

// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddPoint(TAGpoint* point)
{
   TClonesArray &pointArray = *fListOfPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(*point);
}


// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddPoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr)
{
   TClonesArray &pointArray = *fListOfPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(measPos, measPosErr, fitPos, fitPosErr, mom, momErr);
}

// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddPoint(TString name, TVector3 measPos, TVector3 measPosErr)
{
   TClonesArray &pointArray = *fListOfPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(name, measPos, measPosErr);
}

// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddPoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr)
{
   TClonesArray &pointArray = *fListOfPoints;
   return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(name, measPos, measPosErr, fitPos, fitPosErr);
}

// __________________________________________________________________________
//
TAGpoint* TAGtrack::AddPoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr)
{
  TClonesArray &pointArray = *fListOfPoints;
  return new(pointArray[pointArray.GetEntriesFast()]) TAGpoint(name, measPos, measPosErr, fitPos, fitPosErr, mom, momErr);
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAGtrack::Clear(Option_t*)
{
   fListOfPoints->Delete();
   fListOfPoints->Delete();
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
Double_t TAGtrack::GetTwTheta() const
{
   TVector3 direction = fTwDir.Unit();
   Double_t theta      = direction.Theta();
   
   return theta;
}

//______________________________________________________________________________
//
Double_t TAGtrack::GetTwPhi() const
{
   TVector3 origin = fTwDir.Unit();
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

//------------------------------------------+-----------------------------------
TVector3 TAGtrack::GetPosition( double z ){
    return TVector3{
        fParameters.parameter_x[3] * z * z * z + fParameters.parameter_x[2] * z * z + fParameters.parameter_x[1] * z + fParameters.parameter_x[0],
        fParameters.parameter_y[1] * z + fParameters.parameter_y[0],
        z
    };
}

//------------------------------------------+-----------------------------------
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
TArrayI TAGtrack::GetMcTrackIdx()
{
   fMcTrackMap.clear();
   fMcTrackIdx.Reset();
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

ClassImp(TAGntuGlbTrack);

TString TAGntuGlbTrack::fgkBranchName   = "glbtrack.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGntuGlbTrack::TAGntuGlbTrack()
 : TAGdata(),
   fListOfTracks(new TClonesArray("TAGtrack"))
{
  //	SetupClones();
	m_kalmanOutputDir = (string)getenv("FOOTRES")+"/Kalman_new";
    // m_debug = TAGrecoManager::GetPar()->Debug();
    m_debug = 0;

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


/* Add a new track to the repo  --- Genfit
*
*/
TAGtrack* TAGntuGlbTrack::NewTrack(string name, long evNum, int pdgID, int pdgCh, int measCh, float mass, float length, float tof, float chi2, int ndof, float pVal, TVector3* recoPos_target, TVector3* recoMom_target, TMatrixD* recoPos_target_cov, TMatrixD* recoMom_target_cov, vector<TAGpoint*>* shoeTrackPointRepo)
{
	TClonesArray &trackArray = *fListOfTracks;
	TAGtrack* track = new (trackArray[trackArray.GetEntriesFast()]) TAGtrack(
													name, evNum,
													pdgID, pdgCh, measCh, mass, length, tof, chi2, ndof, pVal, 
													recoPos_target, recoMom_target, recoPos_target_cov, recoMom_target_cov,
													shoeTrackPointRepo
												);
	return track;
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

