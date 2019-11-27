#ifndef _TAVTactBaseNtuCluster_HXX
#define _TAVTactBaseNtuCluster_HXX
/*!
 \file
 \version $Id: TAVTactBaseNtuCluster.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAVTactBaseNtuCluster.
 */
/*------------------------------------------+---------------------------------*/

#include "TArrayI.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TString.h"

#include "TAGobject.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

#include "TAGactNtuCluster2D.hxx"

class TAVTbaseCluster;
class TAVTntuHit;
class TAVTbaseParGeo;
class TH1F;
class TH2F;
class TAVTactBaseNtuCluster : public TAGactNtuCluster2D {
   
public:
   explicit  TAVTactBaseNtuCluster(const char* name     =  0,
								           TAGparaDsc* p_config =  0,
								           TAGparaDsc* p_geomap =  0);
   
   virtual ~TAVTactBaseNtuCluster();
   
   //! Action
   virtual  Bool_t Action() { return false; }
   
   //! Apply basic cuts
   virtual Bool_t  ApplyCuts(TAVTbaseCluster* cluster);

   //! Base creation of histogram
   virtual  void   CreateHistogram();
   
    //! Get list of pixels for a given plane
   TClonesArray*   GetListOfPixels()   const { return fListOfPixels;    }
    //! Return seed pixel
   TAVTntuHit*     GetSeedPixel()      const { return fPSeed;           }
   //! Get total charge
   Float_t GetClusterPulseSum()        const { return fClusterPulseSum; }
   
   //! Get position of current cluster
   TVector3*       GetCurrentPosition()      { return fCurrentPosition; }
   //! Get position error of current cluster
   TVector3*       GetCurrentPosError()      { return fCurrentPosError; }

   
   //! Set list of pixels
   void SetListOfPixels(TClonesArray* list)                 { fListOfPixels = list;            }
   //! Set position of current cluster
   void SetCurrentPosition(Float_t u, Float_t v, Float_t z) { fCurrentPosition->SetXYZ(u,v,z); }
   
   //! Compute position
   virtual void ComputePosition();
   
   // Get object in list
   TAGobject*  GetHitObject(Int_t idx) const;
   
protected:
   TAGparaDsc*     fpConfig;		  // config para dsc
   TAGparaDsc*     fpGeoMap;		  // geometry para dsc
   
   TAVTntuHit*    fPSeed;             // seed pixel
   TVector3*      fCurrentPosition;   // pointer to current position
   TVector3*      fCurrentPosError ;  // pointer to current position error
   TClonesArray*  fListOfPixels;      // list of pixels 
   TClonesArray*  fCurListOfPixels;   // list of pixels in current cluster
   Float_t        fClusterPulseSum;   // total charge of cluster
   
   Int_t          fClustersN;     // number of cluster

   TH1F*          fpHisPixelTot;	     // Total number of pixels per cluster
   TH1F*          fpHisPixel[32];	  // number of pixels per cluster per sensor
   TH2F*          fpHisClusMap[32];   // cluster map per sensor
   
   TString        fPrefix;            // prefix of histogram
   TString        fTitleDev;          // device name for histogram title

protected:
   void   SearchCluster();
   void   FillMaps();

   void   ComputeSeedPosition();
   void   ComputeCoGPosition();
   
   ClassDef(TAVTactBaseNtuCluster,0)
};

#endif
