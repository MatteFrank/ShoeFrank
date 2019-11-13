#ifndef _TAIRcluster_HXX
#define _TAIRcluster_HXX

// ROOT classes

#include "TAVTbaseCluster.hxx"


/** TAIRcluster class, simple container class for tracks with the associated clusters
 
 \author Ch. Finck
 */

class TAIRtrack;

class TAIRcluster : public TAVTbaseCluster {
   
   
public:
   TAIRcluster();
   TAIRcluster(const TAIRcluster& cluster);
   ~TAIRcluster();
   
   //! Compute distance from a track
   Float_t            Distance(TAIRtrack *aTrack);

   
   ClassDef(TAIRcluster,2)                          // Describes TAIRcluster
};


#endif

