#ifndef _TAVTntuCluster_HXX
#define _TAVTntuCluster_HXX

/*!
 \file TAVTntuCluster.hxx
 \brief  Class, simple container class for tracks with the associated clusters
 
 \author Ch. Finck
 */


// ROOT classes
#include "TObject.h"
#include "TList.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TObjArray.h"

#include "TAGdata.hxx"
#include "TAVTntuHit.hxx"

#include "TAVTcluster.hxx"


class TAVTtrack;
class TAVTbaseParGeo;

//##############################################################################

class TAVTntuCluster : public TAGdata {
   
protected:
   TAVTbaseParGeo*    fGeometry;        //!< do NOT stored this pointer !
   TObjArray*         fListOfClusters;  ///< list of clusters

private:   
   static TString    fgkBranchName;    ///< Branch name in TTree

public:
   TAVTntuCluster();
   virtual          ~TAVTntuCluster();
   
   //! Get cluster
   TAVTcluster*       GetCluster(Int_t iSensor, Int_t i);
   //! Get cluster const
   const TAVTcluster* GetCluster(Int_t iSensor, Int_t i) const;
   //! Number of cluster per sensor
   Int_t              GetClustersN(Int_t iSensor)   const; 
   
   //! Get list of clusters
   TClonesArray*      GetListOfClusters(Int_t iSensor);
   //! Get list of clusters const
   TClonesArray*      GetListOfClusters(Int_t iSensor) const;
   
   //! Create new cluster (empty)
   TAVTcluster*       NewCluster(Int_t iSensor);
   //! Create new cluster from a existing one
   TAVTcluster*       NewCluster(TAVTcluster* clus, Int_t iSensor);

   //! Set par geo pointer
   void               SetParGeo(TAVTbaseParGeo* par) { fGeometry = par; }
   //!  Setup clones
   virtual void       SetupClones();
   //! Clear
   virtual void       Clear(Option_t* opt="");
   
   //! To stream
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   //! Get branch name
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

   ClassDef(TAVTntuCluster,1)
};

#endif

