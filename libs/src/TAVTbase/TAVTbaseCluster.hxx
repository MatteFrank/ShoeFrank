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
#include "TAVTntuHit.hxx"


class TAVTparGeo;

/** TAVTbaseCluster class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TAVTbaseCluster : public TAGcluster {
   
protected:
   TClonesArray*      fListOfPixels;             // list of pixel attached to this cluster
   
   Float_t            fCharge;                   // sum of pulseheight
   
public:
   TAVTbaseCluster(); 
   TAVTbaseCluster(const TAVTbaseCluster& cluster);
   ~TAVTbaseCluster();
   
   //! Set position in global tracker frame
   void               SetPositionG(TVector3& pos);
   
   //! Set sum of pulse height
   void               SetCharge(Float_t chg)                 { fCharge = chg;          }
  
   // Compute size
   TVector2           ComputeSize();
   
   //! Get U position in local frame
   Float_t            GetPositionU()                   const { return fPosition[0];    }
   //! Get V position in local frame
   Float_t            GetPositionV()                   const { return fPosition[1];    }
   //! Get Pixel list
   TClonesArray*      GetListOfPixels()                const { return fListOfPixels;   }
   
   //! Get index for a given pixel
   Int_t              GetIndex(Int_t tSk)              const { return ((TAVThit*)fListOfPixels->At(tSk))->GetPixelIndex();  } 
   //! Get pulse height for a given pixel
   Float_t            GetPulseHeight(Int_t tSk)        const { return ((TAVThit*)fListOfPixels->At(tSk))->GetPulseHeight(); } 
   //! Get number of pixels in this clusters
   Int_t              GetPixelsN()                     const { return  fListOfPixels->GetEntries(); }
   //! Get sum of pulse height
   Float_t            GetCharge()                      const { return fCharge;  }
   //! Get pixel
   TAVThit*           GetPixel(Int_t idx);
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
   
   ClassDef(TAVTbaseCluster,8)                          // Describes TAVTbaseCluster
};


#endif

