#ifndef _TAIRcluster_HXX
#define _TAIRcluster_HXX

/*!
 \file TAIRcluster.hxx
 \brief   Cluster for interaction region
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/
#include "TAVTbaseCluster.hxx"

class TAIRtrack;
class TAVTbaseCluster;
class TAIRcluster : public TAVTbaseCluster {
   
   
public:
   TAIRcluster();
   TAIRcluster(const TAVTbaseCluster& cluster);
   ~TAIRcluster();
   
   // Compute distance from a track
   Float_t            Distance(TAIRtrack *aTrack);

   
   ClassDef(TAIRcluster,2)                          ///< Describes TAIRcluster
};


#endif

