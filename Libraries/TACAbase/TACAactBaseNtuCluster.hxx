#ifndef _TACAactBaseNtuCluster_HXX
#define _TACAactBaseNtuCluster_HXX
/*!
 \file TACAactBaseNtuCluster.hxx
 \brief   Declaration of TACAactBaseNtuCluster.
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
class TACAactBaseNtuCluster : public TAGactNtuCluster2D {

public:
   explicit  TACAactBaseNtuCluster(const char* name      = 0, TAGdataDsc* p_nturaw = 0,
                                   TAGdataDsc* p_ntuclus = 0, TAGparaDsc* p_geomap = 0,
                                   TAGparaDsc*  p_calib  = 0,
                                   TAGparaDsc*  p_config = 0, TAGdataDsc* p_twpt = 0, Bool_t flagMC = false);

   virtual ~TACAactBaseNtuCluster();

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

protected:
   TAGdataDsc*     fpNtuRaw;         ///< input data dsc
   TAGdataDsc*     fpNtuClus;        ///< output data dsc
   TAGdataDsc*     fpNtuTwPoint;     ///< input data dsc

   TACAntuHit*     fpNtuHit;         ///< list of Hits


   TAGparaDsc*     fpConfig;         ///< config para dsc
   TAGparaDsc*     fpParCal;         ///< calibration para dsc
   TAGparaDsc*     fpGeoMap;		    ///< geometry para dsc

   Int_t          fClustersN;        ///< number of cluster
   Int_t          fTwPointZ;         ///< Atomic number from TW rec point

   Bool_t         fFlagMC;           ///< MC flag

   TH1F*          fpHisHitTot;       ///< Total number of hits per cluster
   TH1F*          fpHisChargeTot;    ///< Total charge per cluster
   TH2F*          fpHisClusMap;      ///< cluster map per sensor

   TH1F*          fpHisHitTwMatch;   ///< Number of hit that match a TW point
   TH1F*          fpHisChargeTwMatch; ///< Total charge per cluster matched with TW
   TH2F*          fpHisClusMapTwMatch;      ///< cluster map per sensor matched with TW

   TH1F*          fpHisResTwMag;     ///< Minimal distance in position with TW
   TH2F*          fpHisTwDeCaE;      ///< TW-deltaE vs CA-E

   TString        fPrefix;           ///< prefix of histogram
   TString        fTitleDev;         ///< device name for histogram title

protected:
   //! Fill maps
   virtual void           FillMaps()      { return; }
   //! Search cluster
   virtual void           SearchCluster() { return; }
   // Create clusters
   Bool_t         CreateClusters();
   // Fill cluster information
   void           FillClusterInfo(TACAcluster* cluster);
   // Compute minimum distance to a cluster
   void           ComputeMinDist(TACAcluster* cluster);
   // Calibrate crystal energy inside a cluster
   void           CalibrateEnergy(TACAcluster* cluster);
   // Get correcton curve for energy calibration as function of z
   Double_t       GetZCurve(Double_t p0, Double_t  p1, Double_t p2, Int_t z);
   // Get energy calibration as function of z
   Double_t       GetEnergy(Double_t rawEnergy, Int_t z, Int_t  crysId);

   ClassDef(TACAactBaseNtuCluster,0)
};

#endif
