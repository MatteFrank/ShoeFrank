#ifndef _TAITcluster_HXX
#define _TAITcluster_HXX

/*!
 \file TAITcluster.hxx
 \brief   Declaration of TAITactNtuTrackF.
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTbaseCluster.hxx"

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

