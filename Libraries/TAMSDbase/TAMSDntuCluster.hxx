#ifndef _TAMSDntuCluster_HXX
#define _TAMSDntuCluster_HXX

/*!
 \file TAMSDntuCluster.hxx
 \brief   Declaration of TAMSDntuCluster.
 */
/*------------------------------------------+---------------------------------*/

// ROOT classes
#include "TObject.h"
#include "TList.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TObjArray.h"

#include "TAGdata.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDcluster.hxx"

class TAMSDtrack;
class TAMSDparGeo;

//##############################################################################

class TAMSDntuCluster : public TAGdata {
   
protected:
  Int_t              fSensorsN;        ///< number of sensors
  TObjArray*         fListOfClusters;  ///< list of clusters

public:
  TAMSDntuCluster(Int_t sensorsN = 0);
  virtual           ~TAMSDntuCluster();
  
  TAMSDcluster*       GetCluster(Int_t iSensor, Int_t i);
  const TAMSDcluster* GetCluster(Int_t iSensor, Int_t i) const;
  
  Int_t               GetClustersN(Int_t iSensor)   const;
  
  TClonesArray*       GetListOfClusters(Int_t iSensor);
  TClonesArray*       GetListOfClusters(Int_t iSensor) const;
  
  TAMSDcluster*       NewCluster(Int_t iSensor);
  TAMSDcluster*       NewCluster(TAMSDcluster* clus, Int_t iSensor);
  
  virtual void        SetupClones();
  virtual void        Clear(Option_t* opt="");
  
  virtual void       ToStream(ostream& os=cout, Option_t* option="") const;
  
  ClassDef(TAMSDntuCluster,1)
};

#endif

