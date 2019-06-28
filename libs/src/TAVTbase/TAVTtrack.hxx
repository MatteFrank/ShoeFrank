#ifndef _TAVTtrack_HXX
#define _TAVTtrack_HXX

// ROOT classes
#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAVTbaseTrack.hxx"


//##############################################################################

/** TAVTtrack class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TAVTcluster;
class TClonesArray;
class TAVTtrack : public TAVTbaseTrack {
   
public:
   TAVTtrack();                                 
   ~TAVTtrack();
   
   //! Set up clones
   void           SetupClones();

    //! Get cluster
   TAVTbaseCluster*   GetCluster(Int_t index)  { return (TAVTcluster*)fListOfClusters->At(index); }
 
   //! Add cluster
   void           AddCluster(TAVTcluster* cluster);
   
   ClassDef(TAVTtrack,8)                      // Describes TAVTtrack
};

#endif

