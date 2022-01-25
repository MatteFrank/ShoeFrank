/*!
 \file TAIRcluster.cxx
 \brief Implementation of TAIRcluster
 */
/*------------------------------------------+---------------------------------*/

#include "TAIRcluster.hxx"
#include "TAIRtrack.hxx"

/*!
 \class TAIRcluster
 \brief   Cluster for interaction region
 */

//! Class Imp
ClassImp(TAIRcluster) // Description of a cluster

//______________________________________________________________________________
//! Default constructor.
TAIRcluster::TAIRcluster()
:  TAVTbaseCluster()
{
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] cluster cluster to copy
TAIRcluster::TAIRcluster(const TAVTbaseCluster& cluster)
:  TAVTbaseCluster(cluster)
{
}

//______________________________________________________________________________
//! Default destructor.
TAIRcluster::~TAIRcluster()
{ 
}

//______________________________________________________________________________
//! Return the distance between this cluster and the pointed track impact in the plane
//!
//! \param[in] aTrack a given track

Float_t TAIRcluster::Distance(TAIRtrack *aTrack)
{
   TVector3 impactPosition( aTrack->Intersection( GetPositionG()[2]) );
   impactPosition -= GetPositionG();
   // Insure that z position is 0 for 2D length computation
   impactPosition.SetXYZ(impactPosition(0), impactPosition(1), 0.);
   
   return impactPosition.Mag();
}



