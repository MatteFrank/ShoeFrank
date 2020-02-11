
#include "TAGcluster.hxx"

/*!
  \class TAGcluster TAGcluster.hxx
  \brief Mother of all cluster/point objects. **
*/

ClassImp(TAGcluster);

//------------------------------------------+-----------------------------------
//! Constructor
TAGcluster::TAGcluster()
: TAGobject()
{
   fMcTrackIdx.Reset();
   fMcTrackMap.clear();
}

//------------------------------------------+-----------------------------------
//! Copy constructor
TAGcluster::TAGcluster(const TAGcluster& cluster)
 : TAGobject(),
   fMcTrackIdx(cluster.fMcTrackIdx)
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
