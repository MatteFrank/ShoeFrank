#ifndef _TAITcluster_HXX
#define _TAITcluster_HXX

// ROOT classes

#include "TAVTbaseCluster.hxx"


/** TAITcluster class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TAIThit;
class TAITtrack;

class TAITcluster : public TAVTbaseCluster {
   
   
public:
   TAITcluster(); 
   TAITcluster(const TAITcluster& cluster);
   ~TAITcluster();
   
   virtual void       SetupClones();

   //! Add pixel to the list
   void               AddPixel(TAIThit* pixel);
   
   ClassDef(TAITcluster,3)                          // Describes TAITcluster
};


#endif

