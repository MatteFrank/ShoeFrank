#ifndef _TAIRtrack_HXX
#define _TAIRtrack_HXX

// ROOT classes
#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAIRcluster.hxx"
#include "TAVTbaseTrack.hxx"


//##############################################################################

/** TAIRtrack class, simple container class for tracks with the associated clusters
 
 \author Ch. Finck
 */

class TAVTbaseCluster;
class TAIRcluster;
class TClonesArray;
class TAIRtrack : public TAVTbaseTrack {
   
public:
   TAIRtrack();
   ~TAIRtrack();
   TAIRtrack(const TAIRtrack& aTrack);

   
   //! Set up clones
   void           SetupClones();

    //! Get cluster
   TAIRcluster* GetCluster(Int_t index)  { return (TAIRcluster*)fListOfClusters->At(index); }
   
   TAIRcluster* GetLastCluster()         { return (TAIRcluster*)fListOfClusters->Last(); }
 
   //! Add cluster
   void           AddCluster(TAVTbaseCluster* cluster);
   
   ClassDef(TAIRtrack,1)                      // Describes TAIRtrack
};

#endif

