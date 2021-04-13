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
   TVector3           fCurPosition;              // current position of the cluster in plane frame
   TClonesArray*      fListOfStrips;             // list of strips attached to this cluster
   
   Int_t              fPlaneView;                // plane view = 0 for X and = 1 for Y plane
   Bool_t             fIsValid;                  // validity flag
   
public:
   TAMSDcluster(); 
   TAMSDcluster(const TAMSDcluster& cluster);
   ~TAMSDcluster();
   
   //! Set position in local frame
   void               SetPositionF(Float_t pos);
   //! Set position error in local frame
   void               SetPosErrorF(Float_t pos);
   //! Set position in global tracker frame
   void               SetPositionG(TVector3& pos);
   //! Set view
   void               SetPlaneView(Int_t v)                  { fPlaneView = v;         }
   //! Set validy
   void               SetValid(Bool_t v = true)              { fIsValid = v;           }
   
   //! Get position in local frame
   Float_t            GetPositionF()                   const { return fPositionF;      }
   //! Get position error in local frame
   Float_t            GetPosErrorF()                   const { return fPosErrorF;      }
   
   //! Get vector position in local frame
   const TVector3&    GetPosition()                    const { return fCurPosition;    }
   //! Get vector position error in local frame
   const TVector3&    GetPosError()                    const { return fPosErrorG;      }
   
   //! Get Pixel list
   TClonesArray*      GetListOfStrips()                const { return fListOfStrips;   }
   
   //! Get view
   Int_t              GetPlaneView()                   const { return fPlaneView;      }
   //! Get validity
   Bool_t             IsValid()                        const { return fIsValid;        }
   //! Get number of pixels in this clusters
   Int_t              GetStripsN()                     const { return  fListOfStrips->GetEntries(); }
   //! Get pixel
   TAMSDhit*          GetStrip(Int_t idx);
      
   //! Compute distance from another cluster
   Float_t            Distance(TAMSDcluster *aClus);
   //! reset pixels
   void               ResetStrips();

   //! Set clones
   virtual void       SetupClones();

   //! Add pixel to the list
   void               AddStrip(TAMSDhit* strip);


   ClassDef(TAMSDcluster,3)                          // Describes TAMSDcluster
};


#endif

