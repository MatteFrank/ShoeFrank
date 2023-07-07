#ifndef _TAMPcluster_HXX
#define _TAMPcluster_HXX

/*!
 \file TAMPcluster.hxx
 \brief   Declaration of TAMPactNtuTrackF.
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAVTbaseCluster.hxx"

class TAMPhit;
class TAMPtrack;

class TAMPcluster : public TAVTbaseCluster {
   
   
public:
   TAMPcluster();
   TAMPcluster(const TAMPcluster& cluster);
   ~TAMPcluster();
   
   virtual void       SetupClones();

   // Add pixel to the list
   void               AddPixel(TAMPhit* pixel);
   
   ClassDef(TAMPcluster,3)                          ///< Describes TAMPcluster
};


#endif

