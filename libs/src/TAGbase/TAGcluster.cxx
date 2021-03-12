
#include "TAGcluster.hxx"

/*!
  \class TAGcluster TAGcluster.hxx
  \brief Mother of all cluster/point objects. **
*/

ClassImp(TAGcluster);

//------------------------------------------+-----------------------------------
//! Constructor
TAGcluster::TAGcluster()
: TAGobject(),
  fClusterIdx(-1),
  fSensorIdx(-1)
{
   fMcTrackIdx.Reset();
   fMcTrackMap.clear();
}

//------------------------------------------+-----------------------------------
//! Copy constructor
TAGcluster::TAGcluster(const TAGcluster& cluster)
 : TAGobject(),
   fMcTrackIdx(cluster.fMcTrackIdx),
   fClusterIdx(cluster.fClusterIdx),
   fSensorIdx(cluster.fSensorIdx)
{
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
