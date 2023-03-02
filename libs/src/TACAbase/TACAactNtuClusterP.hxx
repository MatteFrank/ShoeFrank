#ifndef _TACAactNtuClusterP_HXX
#define _TACAactNtuClusterP_HXX
/*!
 \file TACAactNtuClusterP.hxx
 \brief   Declaration of TACAactNtuClusterP.
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
class TACAactNtuClusterP : public TACAactBaseNtuCluster {
   
public:
   explicit  TACAactNtuClusterP(const char* name     = 0,  TAGdataDsc* p_nturaw  = 0,
                               TAGdataDsc* p_ntuclus = 0, TAGparaDsc* p_geomap = 0,
                               TAGparaDsc*  p_calib  = 0,
                               TAGparaDsc*  p_config = 0, TAGdataDsc* p_twpt = 0);
   
   virtual ~TACAactNtuClusterP();

protected:
   // Search cluster
   void           SearchCluster();
   // Shape cluster
   Bool_t         ShapeCluster(Int_t numClus, Int_t IndX, Int_t IndY, double seedCharge);
   // Fill maps
   void           FillMaps();
   
   ClassDef(TACAactNtuClusterP,0)
};

#endif
