#ifndef _TACAcluster_HXX
#define _TACAcluster_HXX

// ROOT classes
#include "TVector3.h"
#include "TClonesArray.h"

#include "TAGcluster.hxx"
#include "TACAntuHit.hxx"

/** TACAcluster class, simple container class for cluster calorimeter
 
 \author Ch. Finck
 */

class TACAcluster : public TAGcluster {
   
protected:
   TVector3           fPosition;                 // position of the cluster in plane frame
   TVector3           fPosError;                 // position's errors of the cluster in plane frame
   TVector3           fPositionG;                // position of the cluster in tracker frame
   TVector3           fPosErrorG;                // position's errors of the cluster in tracker frame
   TClonesArray*      fListOfHits;               // list of hits attached to this cluster
   
   Int_t              fIndexSeed;                // index of seed crystal
   Float_t            fEnergy;                   // sum of energy
   
public:
   TACAcluster();
   TACAcluster(const TACAcluster& cluster);
   ~TACAcluster();
   
   //! Set position in local frame
   void               SetPosition(TVector3& pos);
   //! Set position error in local frame
   void               SetPosError(TVector3& pos);
   //! Set position in global tracker frame
   void               SetPositionG(TVector3& pos);
   //! Set crystal seed index
   void               SetIndexSeed(Int_t index)              { fIndexSeed = index;     }
   //! Set sum of pulse height
   void               SetEnergy(Float_t chg)                 { fEnergy = chg;          }
   
   //! Get position in local frame
   const TVector3&     GetPosition()                   const { return fPosition;       }
   //! Get position error in local frame
   const TVector3&     GetPosError()                   const { return fPosError;       }
   //! Get position in global tracker frame
   const TVector3&     GetPositionG()                  const { return fPositionG ;     }
   //! Get position in global tracker frame
   const TVector3&     GetPosErrorG()                  const { return fPosErrorG ;     }

   //! Get index of seed crystal
   Int_t              GetIndexSeed()                   const { return fIndexSeed;      }
   //! Get sum of pulse height
   Float_t            GetEnergy()                      const { return fEnergy;         }
   //! Get pixel
   TACAhit*           GetHit(Int_t idx);
   
   //! Get list of hits
   TClonesArray*     GetListOfHits()                   const { return fListOfHits;     }
  
   //! Get number of hits
   Int_t             GetHitsN()                        const { return fListOfHits->GetEntries(); }
   
   //! reset pixels
   void               ResetHits();
   
   //! setup clones
   void               SetupClones();

   //! Add pixel to the list
   void               AddHit(TACAhit* hit);
   
   ClassDef(TACAcluster,6)                          // Describes TACAcluster
};


//##############################################################################

class TACAntuCluster : public TAGdata {
   
protected:
   TClonesArray*      fListOfClusters;
   
private:
   static TString    fgkBranchName;    // Branch name in TTree
   
public:
   TACAntuCluster();
   virtual          ~TACAntuCluster();
   
   TACAcluster*       GetCluster(Int_t i);
   const TACAcluster* GetCluster(Int_t i) const;
   
   Int_t              GetClustersN()   const;
   
   TClonesArray*      GetListOfClusters();
   TClonesArray*      GetListOfClusters() const;
   
   TACAcluster*       NewCluster();
   TACAcluster*       NewCluster(TACAcluster* clus);
   
   virtual void       SetupClones();
   virtual void       Clear(Option_t* opt="");
   
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
   ClassDef(TACAntuCluster,1)
};
#endif

