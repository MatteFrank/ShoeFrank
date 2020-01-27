#ifndef _TAVTbaseCluster_HXX
#define _TAVTbaseCluster_HXX

// ROOT classes
#include "TList.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TArrayI.h"

#include "TAGcluster.hxx"
#include "TAGdata.hxx"
#include "TAVTntuRaw.hxx"


class TAVTparGeo;

/** TAVTbaseCluster class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TAVTbaseCluster : public TAGcluster {
   
protected:
   TVector3*          fPosition;                 // position of the cluster in plane frame
   TVector3*          fPosError;                 // position's errors of the cluster in plane frame 
   TVector3*          fPositionG;                // position of the clus in tracker frame
   TClonesArray*      fListOfPixels;             // list of pixel attached to this cluster
   
   Int_t              fNumber;                   // number
   Int_t              fPlaneNumber;              // plane number
   Float_t            fCharge;                   // sum of pulseheight
   Bool_t             fFoundXZ;					    //! kTRUE is associated to a track in XZ Projection
   Bool_t             fFoundYZ;					    //! kTRUE is associated to a track in YZ Projection
   Bool_t             fIsValid;                  // validity flag
   
   TArrayI            fMcTrackIdx;               // Idx of the track created in the simulation
   std::map<int, int> fMcTrackMap;               // Map of MC track Id
   
public:
   TAVTbaseCluster(); 
   TAVTbaseCluster(const TAVTbaseCluster& cluster);
   ~TAVTbaseCluster();
   
   //! Set position in local frame
   void               SetPosition(TVector3* pos); 
   //! Set position error in local frame
   void               SetPosError(TVector3* pos); 
   void               SetPosition(Float_t u, Float_t v, Float_t z) { fPosition->SetXYZ(u,v,z); }
   //! Set position in global tracker frame
   void               SetPositionG(TVector3* pos); 
   //! Found flag for this cluster (Hough Transform XZ)
   void               SetFoundXZ(Bool_t flag = true)         { fFoundXZ = flag;        }
   //! Found flag for this cluster ((Hough Transform YZ)
   void               SetFoundYZ(Bool_t flag = true)         { fFoundYZ = flag;        }
   //! Set cluster number
   void               SetNumber(Int_t nb)                    { fNumber = nb;           }
   //! Set plane number
   void               SetPlaneNumber(Int_t nb)               { fPlaneNumber = nb;      }
   //! Set sum of pulse height
   void               SetCharge(Float_t chg)                 { fCharge = chg;          }
   //! Set validy
   void               SetValid(Bool_t v = true)              { fIsValid = v;           }
   // Compute size
   TVector2           ComputeSize();
   
   //! Get position in local frame
   const TVector3&     GetPosition()                   const { return *fPosition;      }
   //! Get position error in local frame
   const TVector3&     GetPosError()                   const { return *fPosError;      }
   //! Get position in global tracker frame
   TVector3&           GetPositionG()                  const { return *fPositionG ;    }
   //! Get U position in local frame
   Float_t            GetPositionU()                   const { return (*fPosition)(0); }
   //! Get V position in local frame
   Float_t            GetPositionV()                   const { return (*fPosition)(1); }
   //! Get Pixel list
   TClonesArray*      GetListOfPixels()                const { return fListOfPixels;   }
   //! Get cluster number
 //  Int_t              GetNumber()                      const { return fNumber;         }
   //! Get cluster number
   Int_t              GetPlaneNumber()                 const { return fPlaneNumber;    }
   //! Get found flag this cluster
   Bool_t             GetFoundXZ()                     const { return fFoundXZ;        }
   //! Get found flag this cluster
   Bool_t             GetFoundYZ()                     const { return fFoundYZ;        }
   
   //! Get validity
   Bool_t             IsValid()                        const { return fIsValid;        }
   //! Get index for a given pixel
   Int_t              GetIndex(Int_t tSk)              const { return ((TAVTntuHit*)fListOfPixels->At(tSk))->GetPixelIndex();  } 
   //! Get pulse height for a given pixel
   Float_t            GetPulseHeight(Int_t tSk)        const { return ((TAVTntuHit*)fListOfPixels->At(tSk))->GetPulseHeight(); } 
   //! Get number of pixels in this clusters
   Int_t              GetPixelsN()                     const { return  fListOfPixels->GetEntries(); }
   //! Get sum of pulse height
   Float_t            GetCharge()                      const { return fCharge;  }
   //! Get pixel
   TAVTntuHit*        GetPixel(Int_t idx);
   //! Get position of seed pixel
   Float_t            GetSeedU()                       const;
   Float_t            GetSeedV()                       const;
   
   //! Get distance from cluster position to a given pixel
   Float_t            GetPixelDistanceU(Int_t index)   const;
   Float_t            GetPixelDistanceV(Int_t index)   const;
   
   //! Compute distance from another cluster
   Float_t            Distance(TAVTbaseCluster *aClus);
   //! reset pixels
   void               ResetPixels();
   
   // Add MC track Idx
   void               AddMcTrackIdx(Int_t trackIdx);

   // Get MC info
   Int_t              GetMcTrackIdx(Int_t index)      const   { return fMcTrackIdx[index];    }
   Int_t              GetMcTracksN()                  const   { return fMcTrackIdx.GetSize(); }

   ClassDef(TAVTbaseCluster,2)                          // Describes TAVTbaseCluster
};


#endif

