#ifndef _TAVTtrack_HXX
#define _TAVTtrack_HXX

/*!
 \file TAVTtrack.hxx
 \brief   class for tracks with the associated clusters
  \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

// ROOT classes
#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAVTcluster.hxx"
#include "TAGbaseTrack.hxx"

class TClonesArray;
class TAVTtrack : public TAGbaseTrack {
 
private:
   TVector3       fPosVertex;                    ///< vertex position
   
public:
   TAVTtrack();
   TAVTtrack(const TAVTtrack& aTrack);

   ~TAVTtrack();
   
   //! Set up clones
   void                   SetupClones();

   //! Set pos vertex
   void                   SetPosVertex(TVector3& pos)   { fPosVertex = pos;                               }
   
    //! Get cluster
   TAVTbaseCluster*       GetCluster(Int_t index)       { return (TAVTcluster*)fListOfClusters->At(index); }
   //! Get cluster (const)
   TAVTbaseCluster const* GetCluster(Int_t index) const { return (TAVTcluster*)fListOfClusters->At(index); }

   //! Get pos vertex
   const TVector3&        GetPosVertex()          const { return fPosVertex;                               }
   
   //! Add cluster
   void                   AddCluster(TAGcluster* cluster);
   
   ClassDef(TAVTtrack,9)                      ///< Describes TAVTtrack
};

#endif

