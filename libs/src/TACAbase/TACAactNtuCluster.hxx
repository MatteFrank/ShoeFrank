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

#include "TACAactBaseNtuCluster.hxx"

class TACAcluster;
class TACAparGeo;
class TACAntuHit;
class TH1F;
class TH2F;
class TACAactNtuCluster : public TACAactBaseNtuCluster {
   
public:
   explicit  TACAactNtuCluster(const char* name      = 0,  TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0, TAGparaDsc* p_geomap = 0,
                               TAGparaDsc*  p_calib  = 0,
                               TAGparaDsc*  p_config = 0, TAGdataDsc* p_twpt = 0, Bool_t flagMC = false);
   
   virtual ~TACAactNtuCluster();
   

public:
   static void DisableChargeThres()          { fgThresholdFlag = false;   }
   static void EnableChargeThres()           { fgThresholdFlag = true;    }
   static void SetChargeThreshold(Float_t f) { fgChargeThreshold = f;     }

protected:
   // Search cluster
   void           SearchCluster();
   // Fill maps
   void           FillMaps();

private:
   static Float_t  fgChargeThreshold;
   static Bool_t   fgThresholdFlag;

   ClassDef(TACAactNtuCluster,0)
};

#endif
