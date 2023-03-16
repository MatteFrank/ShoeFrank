#ifndef _TAVTactBaseClusterMT_HXX
#define _TAVTactBaseClusterMT_HXX
/*!
 \file TAVTactBaseClusterMT.hxx
 \brief   Declaration of TAVTactBaseClusterMT.
 */
/*------------------------------------------+---------------------------------*/

#include <pthread.h>

#include "TArrayI.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TString.h"

#include "TAGobject.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

#include "TAGactNtuClusterMT.hxx"

class TAVTbaseCluster;
class TAVThit;
class TAVTbaseParGeo;
class TH1F;
class TH2F;
class TAVTactBaseClusterMT : public TAGactNtuClusterMT {
   
public:
   explicit  TAVTactBaseClusterMT(const char* name     =  0,
								           TAGparaDsc* p_config =  0,
								           TAGparaDsc* p_geomap =  0);
   
   virtual ~TAVTactBaseClusterMT();
   
   //! Action
   virtual  Bool_t Action() { return false; }
   
   // Apply basic cuts
   virtual Bool_t  ApplyCuts(TAVTbaseCluster* cluster);

   // Base creation of histogram
   virtual  void   CreateHistogram();
   
   //! Return seed pixel
   TAVThit*     GetSeedPixel()         const { return fPSeed;           }
   //! Get total charge
   Float_t GetClusterPulseSum()        const { return fClusterPulseSum; }
   
   //! Get position of current cluster
   TVector3&       GetCurrentPosition()      { return fCurrentPosition; }
   //! Get position error of current cluster
   TVector3&       GetCurrentPosError()      { return fCurrentPosError; }

   
   //! Set position of current cluster
   void SetCurrentPosition(Float_t u, Float_t v, Float_t z) { fCurrentPosition.SetXYZ(u,v,z);  }
   
   // Compute position
   virtual void ComputePosition();
   
   // Get object in list
   TAGobject*  GetHitObject(Int_t idx, TClonesArray* listOfPixels) const;
   
protected:
   TAGparaDsc*     fpConfig;		     ///< config para dsc
   TAGparaDsc*     fpGeoMap;		     ///< geometry para dsc
   
   TAVThit*       fPSeed;             ///< seed pixel
   TVector3       fCurrentPosition;   ///< pointer to current position
   TVector3       fCurrentPosError ;  ///< pointer to current position error
   TClonesArray*  fCurListOfPixels;   ///< list of pixels in current cluster
   Float_t        fClusterPulseSum;   ///< total charge of cluster
   
   Int_t          fClustersN[4];      ///< number of cluster

   TH1F*          fpHisPixelTot;	     ///< Total number of pixels per cluster
   TH1F*          fpHisPixel[32];	  ///< number of pixels per cluster per sensor
   TH2F*          fpHisClusMap[32];   ///< cluster map per sensor
   
   TString        fPrefix;            ///< prefix of histogram
   TString        fTitleDev;          ///< device name for histogram title

protected:
   // Search cluster per thread
   void   SearchCluster(TClonesArray* listOfPixels, Int_t thr);
   // Fill map per thread
   void   FillMaps(TClonesArray* listOfPixels, Int_t thr);
   // Compute Seed position
   void   ComputeSeedPosition();
   // Compute CoG position
   void   ComputeCoGPosition();
   // Fill cluster info
   void   FillClusterInfo(Int_t iSensor, TAVTbaseCluster* cluster);
   
protected:
   static   pthread_mutex_t fLock;   ///< mutex lock

   ClassDef(TAVTactBaseClusterMT,0)
};

#endif
