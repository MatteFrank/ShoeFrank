#ifndef _TAMPntuCluster_HXX
#define _TAMPntuCluster_HXX

// ROOT classes
#include "TObject.h"
#include "TList.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TObjArray.h"

#include "TAGdata.hxx"
#include "TAVTntuHit.hxx"

#include "TAMPcluster.hxx"


class TAVTtrack;
class TAVTbaseParGeo;

/*!
 \file TAMPntuCluster.hxx
 \brief   TAMPcluster class, simple container class for tracks with the associated clusters
 \author Ch. Finck
 */


class TAMPntuCluster : public TAGdata {
   
protected:
   //! Geometry parameter
   TAVTbaseParGeo*    fGeometry;         //! do NOT stored this pointer !
   TObjArray*         fListOfClusters;   ///< List of clusters

public:
   TAMPntuCluster();
   virtual          ~TAMPntuCluster();
   
   // Get cluster
   TAMPcluster*       GetCluster(Int_t iSensor, Int_t i);
   // Get cluster (const)
   const TAMPcluster* GetCluster(Int_t iSensor, Int_t i) const;
   // Number of cluster per sensor
   Int_t              GetClustersN(Int_t iSensor)   const; 
   
   // Get list of clusters
   TClonesArray*      GetListOfClusters(Int_t iSensor);
   // Get list of clusters (const)
   TClonesArray*      GetListOfClusters(Int_t iSensor) const;
   
   // Create new cluster (empty)
   TAMPcluster*       NewCluster(Int_t iSensor);
   // Create new cluster from a existing one
   TAMPcluster*       NewCluster(TAMPcluster* clus, Int_t iSensor);

   //! Set par geo pointer
   void               SetParGeo(TAVTbaseParGeo* par) { fGeometry = par; }

   //  Setup clones
   virtual void       SetupClones();
   // Clear
   virtual void       Clear(Option_t* opt="");
   
   // To stream
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;

   ClassDef(TAMPntuCluster,1)
};

#endif

