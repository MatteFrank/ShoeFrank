/*!
 \file TACAactNtuCluster.cxx
 \brief   Implementation of TACAactNtuCluster.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TRandom3.h"

#include "TAGgeoTrafo.hxx"
#include "TACAparGeo.hxx"
#include "TACAparCal.hxx"
#include "TACAntuHit.hxx"
#include "TACAntuCluster.hxx"
#include "TACAactNtuCluster.hxx"

#include "TATWntuPoint.hxx"


Float_t TACAactNtuCluster::fgChargeThreshold = 1;
Bool_t  TACAactNtuCluster::fgThresholdFlag   = true;

/*!
 \class TACAactNtuCluster
 \brief NTuplizer for calorimeter clusters. **
 */

//! Class Imp
ClassImp(TACAactNtuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit input container descriptor
//! \param[out] pNtuClus cluster output container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pTwPoint TW point input container descriptor
TACAactNtuCluster::TACAactNtuCluster(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus, TAGparaDsc* pGeoMap,  TAGparaDsc*  pCalib, TAGparaDsc* pConfig, TAGdataDsc* pTwPoint, Bool_t flagMC)
 : TACAactBaseNtuCluster(name, pNtuRaw, pNtuClus, pGeoMap, pCalib, pConfig, pTwPoint, flagMC)
{
   
}

//------------------------------------------+-----------------------------------
//! Destructor.
TACAactNtuCluster::~TACAactNtuCluster()
{
}

//______________________________________________________________________________
//! Fill maps
void TACAactNtuCluster::FillMaps()
{
   // Clear maps
   ClearMaps();

   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();

   // fill maps for cluster
   for (Int_t i = 0; i < fpNtuHit->GetHitsN(); i++) { // loop over hit crystals
      TACAhit* hit = fpNtuHit->GetHit(i);
      if(!hit->IsValid()) continue;
      Int_t id   = hit->GetCrystalId();
      double charge = hit->GetCharge();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);

      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      if( charge < fgChargeThreshold && !fFlagMC) continue;

      TAGactNtuCluster2D::FillMaps(line, col, i);
   }
}


//______________________________________________________________________________
//! Found all clusters. Starting seed from local maximum 
void TACAactNtuCluster::SearchCluster()
{
   fClustersN = 0;
   
   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();
   TACAntuHit* pNtuHit  = (TACAntuHit*) fpNtuRaw->Object();
   
   // Search for cluster
   for (Int_t i = 0; i < pNtuHit->GetHitsN(); ++i) { // loop over hit crystals
      TACAhit* hit = pNtuHit->GetHit(i);
      
      if (hit->Found()) continue;
      if (!hit->IsValid()) continue;
            
      Int_t id   = hit->GetCrystalId();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;
      
      // loop over lines & columns
      if ( ShapeCluster(fClustersN, line, col) )
         fClustersN++;
   }
   
   if(FootDebugLevel(2))
     cout << "CA - Found : " << fClustersN << " clusters on event: " << gTAGroot->CurrentEventId().EventNumber() << endl;

}
