#ifndef _TAMSDcluster_HXX
#define _TAMSDcluster_HXX

// ROOT classes
#include "TVector3.h"
#include "TClonesArray.h"

#include "TAGobject.hxx"


/** TAMSDcluster class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TAMSDntuHit;

class TAMSDcluster : public TAGobject {
   
private:
   Float_t          fPosition;                 // position of the cluster in plane frame
   Float_t          fPosError;                 // position's errors of the cluster in plane frame
   TVector3*        fPositionG;                // position of the clus in tracker frame
   TClonesArray*    fListOfStrips;             // list of strips attached to this cluster
   
   Int_t              fNumber;                   // number
   Int_t              fPlaneNumber;              // plane number
   Bool_t             fFound;                    // kTRUE is associated to a track
   Bool_t             fIsValid;                 //! validity flag

public:
   TAMSDcluster(); 
   TAMSDcluster(const TAMSDcluster& cluster);
   ~TAMSDcluster();
   
   //! Set position in local frame
   void               SetPosition(Float_t pos)               { fPosition = pos; }
   //! Set position error in local frame
   void               SetPosError(Float_t pos)               { fPosError = pos; }
   //! Set position in global tracker frame
   void               SetPositionG(TVector3* pos);
   //! Found flag for this cluster
   void               SetFound(Bool_t flag = true)           { fFound = flag;          }
   //! Set cluster number
   void               SetNumber(Int_t nb)                    { fNumber = nb;           }
   //! Set plane number
   void               SetPlaneNumber(Int_t nb)               { fPlaneNumber = nb;      }
   //! Set validy
   void               SetValid(Bool_t v = true)              { fIsValid = v;           }
   // Compute size
   void               ComputeSize();
   
   //! Get position in local frame
   Float_t           GetPosition()                     const { return fPosition;      }
   //! Get position error in local frame
   Float_t           GetPosError()                     const { return fPosError;      }
   //! Get position in global tracker frame
   TVector3&           GetPositionG()                  const { return *fPositionG ;    }
   //! Get Pixel list
   TClonesArray*      GetListOfStrips()                const { return fListOfStrips;   }
   //! Get cluster number
   Int_t              GetNumber()                      const { return fNumber;         }
   //! Get cluster number
   Int_t              GetPlaneNumber()                 const { return fPlaneNumber;    }
   //! Get found flag this cluster
   Bool_t             GetFound()                       const { return fFound;          }
   
   //! Get validity
   Bool_t             IsValid()                        const { return fIsValid;        }
   //! Get number of pixels in this clusters
   Int_t              GetStripsN()                     const { return  fListOfStrips->GetEntries(); }
   //! Get pixel
   TAMSDntuHit*        GetStrip(Int_t idx);
      
   //! Compute distance from another cluster
   Float_t            Distance(TAMSDcluster *aClus);
   //! reset pixels
   void               ResetStrips();

   //! Set clones
   virtual void       SetupClones();

   //! Add pixel to the list
   void               AddPixel(TAMSDntuHit* pixel);

   
   ClassDef(TAMSDcluster,2)                          // Describes TAMSDcluster
};


#endif

