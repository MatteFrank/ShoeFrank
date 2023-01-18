#ifndef _TACAactNtuCluster_HXX
#define _TACAactNtuCluster_HXX
/*!
 \file TACAactNtuCluster.hxx
 \brief   Declaration of TACAactNtuCluster.
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

class TACAcluster;
class TACAparGeo;
class TACAntuHit;
class TH1F;
class TH2F;
class TACAactNtuCluster : public TAGactNtuCluster2D {
   
public:
   explicit  TACAactNtuCluster(const char* name     = 0,  TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0, TAGparaDsc* p_geomap = 0,
                               TAGparaDsc*  p_config = 0, TAGdataDsc* p_twpt = 0);
   
   virtual ~TACAactNtuCluster();
   
   //! Action
   virtual  Bool_t Action();
   
   //! Apply basic cuts
   virtual Bool_t  ApplyCuts(TACAcluster* cluster);

   //! Base creation of histogram
   virtual  void   CreateHistogram();
   
   //! Compute position
   virtual void    ComputePosition(TACAcluster* cluster);
   
   //! Get object in list
   TAGobject*      GetHitObject(Int_t idx) const;

public:
   static void DisableChargeThres()         { fgThresholdFlag = false;   }
   static void EnableChargeThres()          { fgThresholdFlag = true;    }
   static void SetChargeThreshold(Bool_t f) { fgChargeThreshold = true;  }

protected:
   TAGdataDsc*     fpNtuRaw;         ///< input data dsc
   TAGdataDsc*     fpNtuClus;        ///< output data dsc
   TAGdataDsc*     fpNtuTwPoint;     ///< input data dsc

   TACAntuHit*     fpNtuHit;         ///< list of Hits


   TAGparaDsc*     fpConfig;		    ///< config para dsc
   TAGparaDsc*     fpGeoMap;		    ///< geometry para dsc
   
   Int_t          fClustersN;        ///< number of cluster

   TH1F*          fpHisHitTot;       ///< Total number of hits per cluster
   TH1F*          fpHisChargeTot;    ///< Total charge per cluster
   TH2F*          fpHisClusMap;      ///< cluster map per sensor
   
   TH1F*          fpHisHitTwMatch;   ///< Number of hit that match a TW point
   TH1F*          fpHisResTwMag;     ///< Minimal distance in position with TW
   TH2F*          fpHisTwDeCaE;      ///< TW-deltaE vs CA-E

   TString        fPrefix;           ///< prefix of histogram
   TString        fTitleDev;         ///< device name for histogram title

protected:
   // Create clusters
   Bool_t         CreateClusters();
   // Search cluster
   void           SearchCluster();
   // Shape cluster
   Bool_t         ShapeCluster(Int_t numClus, Int_t IndX, Int_t IndY);
   // Fill maps
   void           FillMaps();
   // Fill cluster information
   void           FillClusterInfo(TACAcluster* cluster);
   // Compute minimum distance to a cluster
   void           ComputeMinDist(TACAcluster* cluster);
   
private:
   static Float_t  fgChargeThreshold;
   static Bool_t   fgThresholdFlag;

   ClassDef(TACAactNtuCluster,0)
};

#endif
