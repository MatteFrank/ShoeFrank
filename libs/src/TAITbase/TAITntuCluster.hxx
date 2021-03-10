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

/** TAITcluster class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */


//##############################################################################

class TAITntuCluster : public TAGdata {
   
protected:
   TAVTbaseParGeo*    fGeometry;                 //! do NOT stored this pointer !
   TObjArray*         fListOfClusters;

private:   
   static TString    fgkBranchName;    // Branch name in TTree

public:
   TAITntuCluster();
   virtual          ~TAITntuCluster();
   
   TAITcluster*       GetCluster(Int_t iSensor, Int_t i);
   const TAITcluster* GetCluster(Int_t iSensor, Int_t i) const;
   
   Int_t              GetClustersN(Int_t iSensor)   const; 
   
   TClonesArray*      GetListOfClusters(Int_t iSensor);
   TClonesArray*      GetListOfClusters(Int_t iSensor) const;
   
   TAITcluster*       NewCluster(Int_t iSensor);
   TAITcluster*       NewCluster(TAITcluster* clus, Int_t iSensor);

   virtual void       SetupClones();
   virtual void       Clear(Option_t* opt="");
   
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

   ClassDef(TAITntuCluster,1)
};

#endif

