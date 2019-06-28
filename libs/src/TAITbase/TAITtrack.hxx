#ifndef _TAITtrack_HXX
#define _TAITtrack_HXX

// ROOT classes
#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAVTbaseTrack.hxx"


//##############################################################################

/** TAITtrack class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TAITcluster;
class TClonesArray;
class TAITtrack : public TAVTbaseTrack {
   
public:
   TAITtrack();                                 
   ~TAITtrack();
   
   //! Set up clones
   void           SetupClones();

    //! Get cluster
   TAVTbaseCluster*   GetCluster(Int_t index)  { return (TAITcluster*)fListOfClusters->At(index); }
 
   //! Add cluster
   void           AddCluster(TAITcluster* cluster);
   
   ClassDef(TAITtrack,8)                      // Describes TAITtrack
};

#endif

