#ifndef _TAMSDtrack_HXX
#define _TAMSDtrack_HXX

// ROOT classes
#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAGbaseTrack.hxx"


//##############################################################################

/** TAMSDtrack class, simple container class for tracks with the associated points
 
 \author Ch. Finck
 */

class TAVTbaseCluster;
class TClonesArray;
class TAMSDtrack : public TAGbaseTrack {
   
public:
   TAMSDtrack();
   ~TAMSDtrack();
   TAMSDtrack(const TAGbaseTrack& aTrack);

   
   //! Set up clones
   void           SetupClones();

    //! Get cluster
   TAGcluster* GetCluster(Int_t index)  { return (TAMSDpoint*)fListOfClusters->At(index); }
 
   //! Add cluster
   void           AddCluster(TAGcluster* cluster);
   
   ClassDef(TAMSDtrack,8)                      // Describes TAMSDtrack
};

#endif

