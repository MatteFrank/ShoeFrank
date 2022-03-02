#ifndef _TAVTcluster_HXX
#define _TAVTcluster_HXX

/*!
 \file TAVTcluster.hxx
 \brief   Class, simple container class for clusters with the associated hits
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTbaseCluster.hxx"

class TAVTtrack;
class TAVThit;

class TAVTcluster : public TAVTbaseCluster {
      
public:
   TAVTcluster(); 
   TAVTcluster(const TAVTcluster& cluster);
   ~TAVTcluster();
   
   // Set clones
   virtual void       SetupClones();

   // Add pixel to the list
   void               AddPixel(TAVThit* pixel);
   
   ClassDef(TAVTcluster,2)                          ///< Describes TAVTcluster
};

#endif

