
#include "TAGcluster.hxx"
#include "TAGbaseTrack.hxx"

/*!
  \class TAGcluster TAGcluster.hxx
  \brief Mother of all cluster/point objects. **
*/

ClassImp(TAGcluster);

//------------------------------------------+-----------------------------------
//! Constructor
TAGcluster::TAGcluster()
 : TAGobject(),
   fPosition(0., 0., 0.),
   fPosError(0., 0., 0.),
   fPositionG(0., 0., 0.),
   fPosErrorG(0., 0., 0.),
   fClusterIdx(-1),
   fElementsN(-1),
   fSensorIdx(-1),
   fDeviceType(-1),
   fIsValid(kFALSE)
{
   fMcTrackIdx.Reset();
   fMcTrackMap.clear();
}

//------------------------------------------+-----------------------------------
//! Copy constructor
TAGcluster::TAGcluster(const TAGcluster& cluster)
 : TAGobject(),
   fPosition(cluster.fPosition),
   fPosError(cluster.fPosError),
   fPositionG(cluster.fPositionG),
   fPosErrorG(cluster.fPosErrorG),
   fMcTrackIdx(cluster.fMcTrackIdx),
   fClusterIdx(cluster.fClusterIdx),
   fSensorIdx(cluster.fSensorIdx),
   fElementsN(cluster.fElementsN),
   fDeviceType(cluster.fDeviceType),
   fIsValid(cluster.fIsValid)
{
}

//______________________________________________________________________________
//
void TAGcluster::SetPosition(TVector3& pos)
{
   fPosition.SetXYZ(pos.X(), pos.Y(), pos.Z());
}

//______________________________________________________________________________
//
void TAGcluster::SetPosError(TVector3& pos)
{
   fPosError.SetXYZ(pos.X(), pos.Y(), pos.Z());
}

//______________________________________________________________________________
//
void TAGcluster::SetPositionG(TVector3& posGlo)
{
   fPositionG.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   fPosErrorG.SetXYZ(fPosError.X(), fPosError.Y(), 0.01);
}

//______________________________________________________________________________
//
void TAGcluster::SetPosErrorG(TVector3& posGlo)
{
   fPosErrorG.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
}

//______________________________________________________________________________
//
Float_t TAGcluster::Distance(TAGbaseTrack *aTrack) {
   // Return the distance between this cluster and the pointed track impact in the plane
   //
   
   TVector3 impactPosition(aTrack->Intersection( GetPositionG()[2]));
   impactPosition -= GetPositionG();
   // Insure that z position is 0 for 2D length computation
   impactPosition.SetXYZ(impactPosition(0), impactPosition(1), 0.);
   
   return impactPosition.Mag();
}

//______________________________________________________________________________
//
void TAGcluster::AddMcTrackIdx(Int_t trackIdx)
{
   if (fMcTrackMap[trackIdx] == 0) {
      fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
      fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackIdx;
      fMcTrackMap[trackIdx] = 1;
   }
}
