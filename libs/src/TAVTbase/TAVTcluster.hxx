#ifndef _TAVTcluster_HXX
#define _TAVTcluster_HXX

// ROOT classes

#include "TAVTbaseCluster.hxx"


/** TAVTcluster class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TAVTtrack;
class TAVThit;

class TAVTcluster : public TAVTbaseCluster {
   
   
public:
   TAVTcluster(); 
   TAVTcluster(const TAVTcluster& cluster);
   ~TAVTcluster();
   
   virtual void       SetupClones();


   //! Add pixel to the list
   void               AddPixel(TAVThit* pixel);
   
   ClassDef(TAVTcluster,2)                          // Describes TAVTcluster
};


#endif

