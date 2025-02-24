#ifndef _TAITntuCluster_HXX
#define _TAITntuCluster_HXX

// ROOT classes
#include "TObject.h"
#include "TList.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TObjArray.h"

#include "TAGdata.hxx"
#include "TAVTntuHit.hxx"

#include "TAITcluster.hxx"


class TAVTtrack;
class TAVTbaseParGeo;

/*!
 \file TAITntuCluster.hxx
 \brief   TAITcluster class, simple container class for tracks with the associated clusters
 \author Ch. Finck
 */


class TAITntuCluster : public TAGdata {
   
protected:
   Int_t              fSensorsN;        ///< number of sensors
   TObjArray*         fListOfClusters;   ///< List of clusters

public:
   TAITntuCluster(Int_t sensorsN = 0);
   virtual          ~TAITntuCluster();
   
   // Get cluster
   TAITcluster*       GetCluster(Int_t iSensor, Int_t i);
   // Get cluster (const)
   const TAITcluster* GetCluster(Int_t iSensor, Int_t i) const;
   // Number of cluster per sensor
   Int_t              GetClustersN(Int_t iSensor)   const; 
   
   // Get list of clusters
   TClonesArray*      GetListOfClusters(Int_t iSensor);
   // Get list of clusters (const)
   TClonesArray*      GetListOfClusters(Int_t iSensor) const;
   
   // Create new cluster (empty)
   TAITcluster*       NewCluster(Int_t iSensor);
   // Create new cluster from a existing one
   TAITcluster*       NewCluster(TAITcluster* clus, Int_t iSensor);

   //  Setup clones
   virtual void       SetupClones();
   // Clear
   virtual void       Clear(Option_t* opt="");
   
   // To stream
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;

   ClassDef(TAITntuCluster,1)
};

#endif

