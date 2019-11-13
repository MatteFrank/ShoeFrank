////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAIRcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAIRcluster.hxx"
#include "TAIRtrack.hxx"

ClassImp(TAIRcluster) // Description of a cluster

//______________________________________________________________________________
//  
TAIRcluster::TAIRcluster()
:  TAVTbaseCluster()
{
}


//______________________________________________________________________________
//  
TAIRcluster::TAIRcluster(const TAVTbaseCluster& cluster)
:  TAVTbaseCluster(cluster)
{
}

//______________________________________________________________________________
//  
TAIRcluster::~TAIRcluster()
{ 
}

//______________________________________________________________________________
//
Float_t TAIRcluster::Distance(TAIRtrack *aTrack) {
   // Return the distance between this cluster and the pointed track impact in the plane
   //
   
   TVector3 impactPosition( aTrack->Intersection( GetPositionG()[2]) );
   impactPosition -= GetPositionG();
   // Insure that z position is 0 for 2D length computation
   impactPosition.SetXYZ(impactPosition(0), impactPosition(1), 0.);
   
   return impactPosition.Mag();
}



