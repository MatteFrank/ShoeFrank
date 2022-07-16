#ifndef _TACAactNtuCluster_HXX
#define _TACAactNtuCluster_HXX
/*!
 \file
 \version $Id: TACAactNtuCluster.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
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
   
   //! Get total charge
   //Float_t         GetClusterPulseSum()          const { return fClusterPulseSum; }
   
   //! Get position of current cluster
   //TVector3&       GetCurrentPosition()        { return fCurrentPosition; }
   
   //! Get position error of current cluster
   //TVector3&       GetCurrentPosError()        { return fCurrentPosError; }
   
   //! Set position of current cluster
   //void            SetCurrentPosition(Float_t u, Float_t v, Float_t z) { fCurrentPosition.SetXYZ(u,v,z);  }
   
   //! Compute position
   virtual void    ComputePosition(TACAcluster* cluster);
   
   void            DrawEventClusters(int ievent, Option_t *opt);
   
   //! Get object in list
   TAGobject*      GetHitObject(Int_t idx) const;
   
protected:
   TAGdataDsc*     fpNtuRaw;         ///< input data dsc
   TAGdataDsc*     fpNtuClus;        ///< output data dsc
   TAGdataDsc*     fpNtuTwPoint;     ///< input data dsc

   TACAntuHit*     fpNtuHit;         ///< list of Hits


   TAGparaDsc*     fpConfig;		    ///< config para dsc
   TAGparaDsc*     fpGeoMap;		    ///< geometry para dsc
   
   //TVector3       fCurrentPosition;  ///< pointer to current position
   //TVector3       fCurrentPosError ; ///< pointer to current position error
   //Float_t        fClusterPulseSum;  ///< total charge of cluster
   
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
   Bool_t         CreateClusters();
   void           SearchCluster();
   Bool_t         ShapeCluster(Int_t numClus, Int_t IndX, Int_t IndY, double seedCharge);
   void           FillMaps();
   void           FillClusterInfo(TACAcluster* cluster);
   void           ComputeMinDist(TACAcluster* cluster);

   ClassDef(TACAactNtuCluster,0)
};

#endif
