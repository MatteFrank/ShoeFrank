#ifndef _TAIRntuCluster_HXX
#define _TAIRntuCluster_HXX

// ROOT classes
#include "TObject.h"
#include "TList.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TObjArray.h"

#include "TAGdata.hxx"
#include "TAVTntuRaw.hxx"

#include "TAIRcluster.hxx"


class TAVTtrack;
class TAVTbaseParGeo;

/** TAIRcluster class, simple container class for tracks with the associated clusters
 
 \author Ch. Finck
 */


//##############################################################################

class TAIRntuCluster : public TAGdata {
   
protected:
   TAVTbaseParGeo*    fGeometry;                 //! do NOT stored this pointer !
   TObjArray*         fListOfClusters;

private:   
   static TString    fgkBranchName;    // Branch name in TTree

public:
   TAIRntuCluster();
   virtual          ~TAIRntuCluster();
   
   TAIRcluster*       GetCluster(Int_t iSensor, Int_t i);
   const TAIRcluster* GetCluster(Int_t iSensor, Int_t i) const;
   
   Int_t              GetClustersN(Int_t iSensor)   const; 
   
   TClonesArray*      GetListOfClusters(Int_t iSensor);
   TClonesArray*      GetListOfClusters(Int_t iSensor) const;
   
   TAIRcluster*       NewCluster(Int_t iSensor);
   TAIRcluster*       NewCluster(TAIRcluster* clus, Int_t iSensor);

   virtual void       SetupClones();
   virtual void       Clear(Option_t* opt="");
   
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

   ClassDef(TAIRntuCluster,1)
};

#endif

