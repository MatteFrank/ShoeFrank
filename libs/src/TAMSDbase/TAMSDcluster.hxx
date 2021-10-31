#ifndef _TAMSDcluster_HXX
#define _TAMSDcluster_HXX

#include <map>
// ROOT classes
#include "TVector3.h"
#include "TClonesArray.h"
#include "TAGcluster.hxx"


/** TAMSDcluster class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TAMSDhit;

class TAMSDcluster : public TAGcluster {
   
private:
   Float_t            fPositionF;                // position of the cluster in plane frame
   Float_t            fPosErrorF;                // position's errors of the cluster in plane frame
   Float_t            fCog;                      // center of gravity of the cluster in term of strip number
   TVector3           fCurPosition;              // current position of the cluster in plane frame
   TClonesArray*      fListOfStrips;             // list of strips attached to this cluster
   Int_t              fPlaneView;                // plane view = 0 for X and = 1 for Y plane
   Float_t            fEnergyLoss;               // loss of energy
   
public:
   TAMSDcluster(); 
   TAMSDcluster(const TAMSDcluster& cluster);
   ~TAMSDcluster();
   
   //! Set position in local frame
   void               SetPositionF(Float_t pos);
   //! Set position error in local frame
   void               SetPosErrorF(Float_t pos);
   //! Set cluster center of gravity
   void               SetCog(Float_t pos);
   //! Set position in global tracker frame
   void               SetPositionG(TVector3& pos);
   //! Set view
   void               SetPlaneView(Int_t v);
   //! Set energy loss
   void               SetEnergyLoss(Float_t chg)             { fEnergyLoss = chg;      }
   
   //! Get position in local frame
   Float_t            GetPositionF()                   const { return fPositionF;      }
   //! Get position error in local frame
   Float_t            GetPosErrorF()                   const { return fPosErrorF;      }
   //! Get center of gravity
   Float_t            GetCog()                         const { return fCog;            }
   //! Get vector position in local frame
   const TVector3&    GetPosition()                    const { return fCurPosition;    }
   //! Get vector position error in local frame
   const TVector3&    GetPosError()                    const { return fPosErrorG;      }
   
   //! Get Pixel list
   TClonesArray*      GetListOfStrips()                const { return fListOfStrips;   }
   
   //! Get view
   Int_t              GetPlaneView()                   const { return fPlaneView;      }
   //! Get energy loss
   Float_t            GetEnergyLoss()                  const { return fEnergyLoss;     }
   //! Get energy loss with eta correction
   Float_t            GetEnergyLossCorr()                    { return ComputeEtaCorrection(fCog) * fEnergyLoss;}
   //! Get 'Eta' value for the cluster 
   Float_t            GetEta()                               { return ComputeEta(fCog); }

   //! Get number of pixels in this clusters
   Int_t              GetStripsN()                     const { return  fListOfStrips->GetEntries(); }
   //! Get pixel
   TAMSDhit*          GetStrip(Int_t idx);
      
   //! Compute distance from another cluster
   Float_t            Distance(TAMSDcluster *aClus);
   
   //! Compute 'Eta' value for the cluster
   Float_t            ComputeEta(Float_t cog);
   //! Compute corection factor for the energy loss wrt position
   Float_t            ComputeEtaCorrection(Float_t cog);
   
   //! reset pixels
   void               ResetStrips();

   //! Set clones
   virtual void       SetupClones();

   //! Add pixel to the list
   void               AddStrip(TAMSDhit* strip);


   ClassDef(TAMSDcluster,5)                          // Describes TAMSDcluster
};


#endif

