#ifndef _TAITcluster_HXX
#define _TAITcluster_HXX

// ROOT classes

#include "TAVTbaseCluster.hxx"


/** TAITcluster class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TAITntuHit;
class TAITtrack;

class TAITcluster : public TAVTbaseCluster {
   
   
public:
   TAITcluster(); 
   TAITcluster(const TAITcluster& cluster);
   ~TAITcluster();
   
   virtual void       SetupClones();

   //! Compute distance from a track
   Float_t            Distance(TAITtrack *aTrack);

   //! Add pixel to the list
   void               AddPixel(TAITntuHit* pixel);
   
   ClassDef(TAITcluster,2)                          // Describes TAITcluster
};


#endif

