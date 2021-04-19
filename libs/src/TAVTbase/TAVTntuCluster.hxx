#ifndef _TAVTntuCluster_HXX
#define _TAVTntuCluster_HXX

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

/** TAVTcluster class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */


//##############################################################################

class TAVTntuCluster : public TAGdata {
   
protected:
   TAVTbaseParGeo*    fGeometry;                 //! do NOT stored this pointer !
   TObjArray*         fListOfClusters;

private:   
   static TString    fgkBranchName;    // Branch name in TTree

public:
   TAVTntuCluster();
   virtual          ~TAVTntuCluster();
   
   TAVTcluster*       GetCluster(Int_t iSensor, Int_t i);
   const TAVTcluster* GetCluster(Int_t iSensor, Int_t i) const;
   
   Int_t              GetClustersN(Int_t iSensor)   const; 
   
   TClonesArray*      GetListOfClusters(Int_t iSensor);
   TClonesArray*      GetListOfClusters(Int_t iSensor) const;
   
   TAVTcluster*       NewCluster(Int_t iSensor);
   TAVTcluster*       NewCluster(TAVTcluster* clus, Int_t iSensor);

   void               SetParGeo(TAVTbaseParGeo* par) { fGeometry = par; }
   virtual void       SetupClones();
   virtual void       Clear(Option_t* opt="");
   
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

   ClassDef(TAVTntuCluster,1)
};

#endif

