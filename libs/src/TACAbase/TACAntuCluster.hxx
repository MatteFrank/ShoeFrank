#ifndef _TACAcluster_HXX
#define _TACAcluster_HXX

/*!
 \file TACAntuCluster.hxx
 \brief Declaration of TACAntuCluster
 
 \author Ch. Finck
 */


// ROOT classes
#include "TVector3.h"
#include "TClonesArray.h"

#include "TAGcluster.hxx"
#include "TACAntuHit.hxx"


/*!
 \class TACAcluster
 \brief class for cluster calorimeter
 */

class TACAcluster : public TAGcluster {
   
protected:
   TVector3           fPosition;                 ///< position of the cluster in plane frame
   TVector3           fPosError;                 ///< position's errors of the cluster in plane frame
   TVector3           fPositionG;                ///< position of the cluster in tracker frame
   TVector3           fPosErrorG;                ///< position's errors of the cluster in tracker frame
   TClonesArray*      fListOfHits;               ///< list of hits attached to this cluster
   Int_t              fIndexSeed;                ///< index of seed crystal
   Bool_t             fTwMatched;                ///< matched with a rec point of TW
   Float_t            fEnergy;                   ///< sum of energy
   
public:
   TACAcluster();
   TACAcluster(const TACAcluster& cluster);
   ~TACAcluster();
   
   // Set position in local frame
   void               SetPosition(TVector3& pos);
   // Set position error in local frame
   void               SetPosError(TVector3& pos);
   // Set position in global tracker frame
   void               SetPositionG(TVector3& pos);
   
   //! Set crystal seed index
   void               SetIndexSeed(Int_t index)              { fIndexSeed = index;     }
   //! Set sum of pulse height
   void               SetEnergy(Float_t chg)                 { fEnergy = chg;          }
   //! Set match flag with TW point
   void               SetTwMatched(Bool_t m)                 { fTwMatched = m;         }
   
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
   //! Get  match flag with TW point
   Bool_t             IsTwMatched()                   const { return fTwMatched;      }
   //! Get pixel
   TACAhit*           GetHit(Int_t idx);
   
   //! Get list of hits
   TClonesArray*     GetListOfHits()                   const { return fListOfHits;     }
  
   //! Get number of hits
   Int_t             GetHitsN()                        const { return fListOfHits->GetEntriesFast(); }
   
   // reset pixels
   void              ResetHits();
   
   // setup clones
   void              SetupClones();

   // Add pixel to the list
   void              AddHit(TACAhit* hit);
   
   ClassDef(TACAcluster,7)                          // Describes TACAcluster
};


//##############################################################################
/*!
 \class TACAntuCluster
 \brief simple container class for cluster calorimeter
 */
class TACAntuCluster : public TAGdata {
   
protected:
   TClonesArray*      fListOfClusters; ///< List of clusters
   
private:
   static TString    fgkBranchName;    ///< Branch name in TTree
   
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
   //! return branch name
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
   ClassDef(TACAntuCluster,1)
};
#endif

