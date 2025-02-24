/*!
 \file TAGcluster.cxx
 \brief   Implementation of TAGactTreeReader.
 */

#include "TAGcluster.hxx"
#include "TAGbaseTrack.hxx"

/*!
  \class TAGcluster
  \brief Mother of all cluster/point objects. **
*/

//! Class Imp
ClassImp(TAGcluster);

//------------------------------------------+-----------------------------------
//! Constructor
TAGcluster::TAGcluster()
 : TAGobject(),
   fPosition1(0., 0., 0.),
   fPosError1(0., 0., 0.),
   fPosition2(0., 0., 0.),
   fPosError2(0., 0., 0.),
   fClusterIdx(-1),
   fElementsN(-1),
   fSensorIdx(-1),
   fDeviceType(-1),
   fIsValid(kFALSE)
{
   fMcTrackIdx.Reset();
   fMcTrackMap.clear();

   fMcHitIdx.Reset();
   fMcHitMap.clear();
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] cluster cluster to copy
TAGcluster::TAGcluster(const TAGcluster& cluster)
 : TAGobject(),
   fPosition1(cluster.fPosition1),
   fPosError1(cluster.fPosError1),
   fPosition2(cluster.fPosition2),
   fPosError2(cluster.fPosError2),
   fMcTrackIdx(cluster.fMcTrackIdx),
   fMcHitIdx(cluster.fMcHitIdx),
   fClusterIdx(cluster.fClusterIdx),
   fSensorIdx(cluster.fSensorIdx),
   fElementsN(cluster.fElementsN),
   fDeviceType(cluster.fDeviceType),
   fIsValid(cluster.fIsValid)
{
}

TAGcluster::~TAGcluster()
{
   fMcTrackIdx.Set(0);
   fMcHitIdx.Set(0);

   fMcTrackMap.clear();
   fMcHitMap.clear();
   fIsValid = false;
}

//______________________________________________________________________________
//! Set local position
//!
//! \param[in] pos position  in sensor framework
void TAGcluster::SetPosition(TVector3& pos)
{
   fPosition1.SetXYZ(pos.X(), pos.Y(), pos.Z());
}

//______________________________________________________________________________
//! Set local position error
//!
//! \param[in] pos position error in sensor framework
void TAGcluster::SetPosError(TVector3& pos)
{
   fPosError1.SetXYZ(pos.X(), pos.Y(), pos.Z());
}

//______________________________________________________________________________
//! Set Global position
//!
//! \param[in] posGlo position in detector framework
void TAGcluster::SetPositionG(TVector3& posGlo)
{
   fPosition2.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   fPosError2.SetXYZ(fPosError1.X(), fPosError1.Y(), 0.01);
}

//______________________________________________________________________________
//! Set Global position error
//!
//! \param[in] posGlo position error in detector framework
void TAGcluster::SetPosErrorG(TVector3& posGlo)
{
   fPosError2.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
}

//______________________________________________________________________________
//! Return the distance between this cluster and the pointed track impact in the plane
//!
//! \param[in] aTrack a given track
Float_t TAGcluster::Distance(TAGbaseTrack* aTrack)
{
   TVector3 impactPosition(aTrack->Intersection( GetPositionG()[2]));
   impactPosition -= GetPositionG();
   // Insure that z position is 0 for 2D length computation
   impactPosition.SetXYZ(impactPosition(0), impactPosition(1), 0.);
   
   return impactPosition.Mag();
}

//______________________________________________________________________________
//! Add MC track to list
//!
//! \param[in] trackIdx MC track index
void TAGcluster::AddMcTrackIdx(Int_t trackIdx, Int_t hitIdx)
{
   if (fMcTrackMap[trackIdx] == 0) {
      fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
      fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackIdx;
      fMcTrackMap[trackIdx] = 1;
   }
      
   if (fMcHitMap[hitIdx] == 0) {
      fMcHitIdx.Set(fMcTrackIdx.GetSize()+1);
      fMcHitIdx[fMcTrackIdx.GetSize()-1] = hitIdx;
      fMcHitMap[hitIdx] = 1;
   }
   
}
