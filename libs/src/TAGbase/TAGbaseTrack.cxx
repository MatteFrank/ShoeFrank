/*!
 \file TAGbaseTrack.cxx
 \brief   Implementation of TAGbaseTrack.
 */

#include "TMath.h"
#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx" 
#include "TAGbaseTrack.hxx"

/*!
 \class TAGbaseTrack
 \brief Base class for track
 
 A particle track from e.g. accelerator passing the tracker
 The track is measured by the tracker with its reference planes
 
 A line is defined by its
                   origin     = (x_0,y_0,z_0),
                   slope      = (dx/dz,dy/dz,1),
  and              length.
  Points on the line at r_i are given as a function
  of the parameter beta. beta has no dimension.
  r_i(beta) = origin_i + beta * direction_i
  If one wants the pair (x,y) as a function of z along (0,0,dz) then beta = z/dz
     and r_1(beta) = x_0 + z * dx/dz
         r_2(beta) = y_0 + z * dy/dz
         r_3(beta) = z_0 + z * 1
  In case one needs pair (x,y,z) as a function of l along (dx,dy,dz) then beta' = l/dl
     and r_1(beta') = x_0 + l * dx/dl
         r_2(beta') = y_0 + l * dy/dl
         r_3(beta') = z_0 + l * dz/dl
  with the relation beta^2 = beta'^2 * (1-x^2/l^2-y^2/l^2) / (1-dx^2/dl^2-dy^2/dl^2)
 */

//! Class Imp
ClassImp(TAGbaseTrack) // Description of a Track

//______________________________________________________________________________
//! Constructor
TAGbaseTrack::TAGbaseTrack()
:  TAGobject(),
   fOrigin(new TVector3()),
   fSlope(new TVector3()),
   fOriginErr(new TVector3()),
   fSlopeErr(new TVector3()),
   fLength(0.0),
   fPileup(kFALSE),
   fType(0),
   fTrackIdx(-1),
   fChiSquare(0.),
   fChiSquareU(0.),
   fChiSquareV(0.),
   fValidity(false),
   fChargeProba(new TArrayF(6)),
   fChargeWithMaxProba(0),
   fChargeMaxProba(0.),
   fChargeProbaNorm(new TArrayF(6)),
   fChargeWithMaxProbaNorm(0),
   fChargeMaxProbaNorm(0.),
   fMeanEltsN(0),
   fMeanCharge(0.)
{
   fMcTrackIdx.Reset();
   fMcTrackMap.clear();
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] aTrack track to copy
TAGbaseTrack::TAGbaseTrack(const TAGbaseTrack& aTrack)
:  TAGobject(aTrack),
   fOrigin(new TVector3(*aTrack.fOrigin)),
   fSlope(new TVector3(*aTrack.fSlope)),
   fOriginErr(new TVector3(*aTrack.fOriginErr)),
   fSlopeErr(new TVector3(*aTrack.fSlopeErr)),
   fLength(aTrack.fLength),
   fPileup(aTrack.fPileup),
   fType(aTrack.GetType()),
   fTrackIdx(aTrack.GetTrackIdx()),
   fChiSquare(aTrack.GetChi2()),
   fChiSquareU(aTrack.GetChi2U()),
   fChiSquareV(aTrack.GetChi2V()),
   fPosVertex(aTrack.fPosVertex),
   fValidity(aTrack.fValidity),
   fChargeProba(new TArrayF(*aTrack.fChargeProba)),
   fChargeWithMaxProba(aTrack.fChargeWithMaxProba),
   fChargeMaxProba(aTrack.fChargeMaxProba),
   fChargeProbaNorm(new TArrayF(*aTrack.fChargeProbaNorm)),
   fChargeWithMaxProbaNorm(aTrack.fChargeWithMaxProbaNorm),
   fChargeMaxProbaNorm(aTrack.fChargeMaxProbaNorm),
   fMeanEltsN(aTrack.fMeanEltsN),
   fMeanCharge(aTrack.fMeanCharge),
   fMcTrackIdx(aTrack.fMcTrackIdx)
{
   fListOfClusters = (TClonesArray*)aTrack.fListOfClusters->Clone();
}

//______________________________________________________________________________
//! Destructor
TAGbaseTrack::~TAGbaseTrack()
{
   delete fOrigin;
   delete fSlope;
   delete fOriginErr;
   delete fSlopeErr;
   delete fListOfClusters;
   delete fChargeProba;
   delete fChargeProbaNorm;
}

//______________________________________________________________________________
//! Get theta angle in deg
Float_t TAGbaseTrack::GetTheta() const
{
   TVector3 direction = fSlope->Unit();
   Float_t theta      = direction.Theta()*TMath::RadToDeg();
   
   return theta;
}

//______________________________________________________________________________
//! Get phi angle in deg
Float_t TAGbaseTrack::GetPhi() const
{
   TVector3 origin = fOrigin->Unit();
   Float_t phi     = origin.Phi()*TMath::RadToDeg();
   
   return phi;
}

//______________________________________________________________________________
//! Set track value
//!
//! \param[in] aOrigin origin of track
//! \param[in] aSlope slope of track
//! \param[in] aLength length of track
void TAGbaseTrack::SetValue(const TVector3& aOrigin, const TVector3& aSlope, const Float_t aLength)
{
   *fOrigin = aOrigin;
   *fSlope  = aSlope;
   fLength  = aLength;
}

//______________________________________________________________________________
//! Set track value error
//!
//! \param[in] aOriginErr error at origin of track
//! \param[in] aSlopeErr error at slope of track
void TAGbaseTrack::SetErrorValue(const TVector3& aOriginErr, const TVector3& aSlopeErr)
{
   *fOriginErr = aOriginErr;
   *fSlopeErr  = aSlopeErr;
}

//______________________________________________________________________________
//! Reset value
void TAGbaseTrack::Zero(){
   fOrigin->SetXYZ(0., 0., 0.);
   fSlope->SetXYZ(0., 0., 0.);
   fLength = 0;
}

//______________________________________________________________________________
//! Get point extrapolation from Z position
//!
//! \param[in] beta Z position
TVector3 TAGbaseTrack::GetPoint(Float_t beta)
{
   TVector3 result;
   result = (*fSlope) * beta;
   TVector3 offset = *fOrigin;
   offset(2) = 0.;
   return result += offset;
}


//______________________________________________________________________________
//! Compute distance between a point M(x,y,z) and a
//!
//! line defined by (origin, slope)
//! x = slopeX*z + originX
//! y = slopeY*z + originY
//! The distance is defined in a plane normal to the line
//! \param[in] p vector point
Float_t TAGbaseTrack::Distance(TVector3& p)
{
   Float_t dX2 =  p.X()-fSlope->X()*p.Z()-fOrigin->X();
   dX2 *= dX2;
   Float_t dY2 =  p.Y()-fSlope->Y()*p.Z()-fOrigin->Y();
   dY2 *= dY2;
   Float_t prod2 = fSlope->Y()*(p.X()-fOrigin->X()) - fSlope->X()*(p.Y()-fOrigin->Y());
   prod2 *= prod2;
   
   Float_t distance = dX2 +dY2 + prod2;
   distance = TMath::Sqrt(distance);
   
   return distance;
}

//____________________________________________________________________________
//! Reset
void TAGbaseTrack::Reset()
{
  Zero();
  fPileup          = kFALSE;
}

//______________________________________________________________________________
//! Set line value (deprecated method)
//!
//! \param[in] aOrigin origin of track
//! \param[in] aSlope slope of track
//! \param[in] aLength length of track
void TAGbaseTrack::SetLineValue(const TVector3& aOrigin, const TVector3& aSlope, const Float_t aLength)
{
   SetValue(aOrigin, aSlope, aLength);
}

//______________________________________________________________________________
//! Set line value error  (deprecated method)
//!
//! \param[in] aOriginErr error at origin of track
//! \param[in] aSlopeErr error at slope of track
void TAGbaseTrack::SetLineErrorValue(const TVector3& aOriginErr, const TVector3& aSlopeErr)
{
   SetErrorValue(aOriginErr, aSlopeErr);
}

//______________________________________________________________________________
//! Calculates the Intersection of the Track with a plane
//!
//! \param[in] posZ Z position
TVector3 TAGbaseTrack::Intersection(Float_t posZ) const
{
  TVector3 result(GetOrigin());  // track origin in xyz tracker coordinates
  result(2) = 0.;
  result += GetSlopeZ() * posZ; // intersection in xyz frame at z_plane
  return  result;
}

//______________________________________________________________________________
//! Calculates the distance of a Track at a given Z position
//!
//! \param[in] track a given track
//! \param[in] z Z position
Float_t TAGbaseTrack::Distance(TAGbaseTrack* track, Float_t z) const
{
   // calculates the distance with a track
   TVector3 pos1 = Intersection(z);
   TVector3 pos2 = track->Intersection(z);
   TVector3 pos0 = pos1-pos2;
   Float_t rho0  = pos0.Mag();
   
   return rho0;
}

//______________________________________________________________________________
//! Calculates the distance of a Track at a given Z position in the normal plane
//!
//! \param[in] track a given track
//! \param[in] z Z position
TVector2 TAGbaseTrack::DistanceXY(TAGbaseTrack* track, Float_t z) const
{
   // calculates the distance in X-Y with a track
   TVector3 pos1 = Intersection(z);
   TVector3 pos2 = track->Intersection(z);
   TVector3 pos0 = pos1-pos2;
   
   return TVector2(pos0.X(), pos0.Y());
}

//______________________________________________________________________________
//! Calculates the minimum distance with a track in a given range of Z
//!
//! \param[in] track a given track
//! \param[in] zMin  lower limit of Z position
//! \param[in] zMax  upper limit of Z position
//! \param[in] eps  tolerance
TVector2 TAGbaseTrack::DistanceMin(TAGbaseTrack* track, Float_t zMin, Float_t zMax, Float_t eps) const
{
   Float_t z = 0.;
   Float_t rhoMin = Distance(track, zMin);
   Float_t rhoMax = Distance(track, zMax);
   Float_t rho  = 0.;
   Float_t rhoh = 0.;
   Float_t h    = eps;
   
   while (TMath::Abs(zMax - zMin) > eps) {
	  z    = (zMax + zMin)/2.;
	  rho  = Distance(track, z);
	  rhoh = Distance(track, z+h);
	  if ((rho - rhoh) < 0) {
		 zMax = z;
		 rhoMax = rho;
	  } else {
		 zMin = z;
		 rhoMin = rho;
	  }
   }
   return TVector2(z, rho);;
}

// __________________________________________________________________________
//! Compute Chi2 for a given error (deprecated method)
//!
//! \param[in] dhs constant error for each cluster
void TAGbaseTrack::MakeChiSquare(Float_t dhs)
{
   // Computes the chi2 of the fit track using dhs as the error
   Float_t tdU, tdV;
   Float_t tUtrack, tVtrack, err;
   Int_t   ndfU, ndfV;
   err = dhs;
   ndfU = ndfV = -2; // indeed, two parameters are fit per dimemsions (4 in all)
   fChiSquare = fChiSquareU = fChiSquareV = 0.0;
   if( GetClustersN() <=2 ) return; // return 0. for chisquare if there is less than 2 hits
   
   for (Int_t ht = 0; ht < GetClustersN(); ht++){
	  Float_t posZ = GetCluster(ht)->GetPosition().Pz();
	  tUtrack = Intersection(posZ).Px();
	  if (dhs == 0.)
		 dhs = GetCluster(ht)->GetPosError().Px();
	  tdU = (GetCluster(ht)->GetPositionG().Px() - tUtrack)/(dhs);
	  ndfU++;
	  fChiSquareU += tdU * tdU;
	  dhs = err;
	  
	  // only for pixel detector
	  tVtrack = Intersection(posZ).Py();
	  if (dhs == 0.)
		 dhs = GetCluster(ht)->GetPosError().Py();
	  tdV = (GetCluster(ht)->GetPositionG().Py() - tVtrack)/(dhs);
	  ndfV++;
	  fChiSquareV += tdV * tdV;
   
   }
   
   fChiSquare = (fChiSquareU+fChiSquareV) / Double_t(2*GetClustersN()-4);
   fChiSquareU /= ndfU;
   fChiSquareV /= ndfV;
   
   if(FootDebugLevel(1))
	  printf("TAGbaseTrack::MakeChiSquare chi2u=%f, ndfu=%d, chi2v=%f, ndfv=%d, chi2=%f, ndf=%d, resol=%f\n",
			 fChiSquareU, ndfU, fChiSquareV, ndfV, fChiSquare, 2*GetClustersN()-4, dhs);
}

// __________________________________________________________________________
//! Set charge probability
//!
//! \param[in] proba charge probability array
void TAGbaseTrack::SetChargeProba(const TArrayF* proba)
{
   const Float_t* array = proba->GetArray();
   fChargeProba->Set(proba->GetSize(), array);
}

// __________________________________________________________________________
//! Set normalised charge probability
//!
//! \param[in] proba charge probability array
void TAGbaseTrack::SetChargeProbaNorm(const TArrayF* proba)
{
   const Float_t* array = proba->GetArray();
   fChargeProbaNorm->Set(proba->GetSize(), array);
}

//______________________________________________________________________________
//! Add MC track to list
//!
//! \param[in] trackIdx MC track index
void TAGbaseTrack::AddMcTrackIdx(Int_t trackIdx)
{
   if (fMcTrackMap[trackIdx] == 0) {
      fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
      fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackIdx;
      fMcTrackMap[trackIdx] = 1;
   }
}
