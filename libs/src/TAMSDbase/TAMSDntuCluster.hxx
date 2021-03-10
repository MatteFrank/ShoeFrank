#ifndef _TAMSDntuCluster_HXX
#define _TAMSDntuCluster_HXX

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
//class TAVTbaseParGeo;
class TAMSDparGeo;

/** TAMSDcluster class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */


//##############################################################################

class TAMSDntuCluster : public TAGdata {
   
protected:
  //   TAVTbaseParGeo*    fGeometry;                 //! do NOT stored this pointer !
  TAMSDparGeo*       fGeometry;                 //! do NOT stored this pointer !
  TObjArray*         fListOfClusters;

private:   
  static TString    fgkBranchName;    // Branch name in TTree

public:
  TAMSDntuCluster();
  virtual          ~TAMSDntuCluster();
  
  TAMSDcluster*       GetCluster(Int_t iSensor, Int_t i);
  const TAMSDcluster* GetCluster(Int_t iSensor, Int_t i) const;
  
  Int_t              GetClustersN(Int_t iSensor)   const; 
  
  TClonesArray*      GetListOfClusters(Int_t iSensor);
  TClonesArray*      GetListOfClusters(Int_t iSensor) const;
  
  TAMSDcluster*       NewCluster(Int_t iSensor);
  TAMSDcluster*       NewCluster(TAMSDcluster* clus, Int_t iSensor);
  
  virtual void       SetupClones();
  virtual void       Clear(Option_t* opt="");
  
  virtual void       ToStream(ostream& os=cout, Option_t* option="") const;
  
public:
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
  ClassDef(TAMSDntuCluster,1)
};

#endif

