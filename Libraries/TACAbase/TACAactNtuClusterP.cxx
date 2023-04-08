/*!
 \file TACAactNtuClusterP.cxx
 \brief   Implementation of TACAactNtuClusterP.
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
#include "TACAactNtuClusterP.hxx"

#include "TATWntuPoint.hxx"

/*!
 \class TACAactNtuClusterP
 \brief NTuplizer for calorimeter clusters. **
 */

//! Class Imp
ClassImp(TACAactNtuClusterP);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit input container descriptor
//! \param[out] pNtuClus cluster output container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pTwPoint TW point input container descriptor
TACAactNtuClusterP::TACAactNtuClusterP(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus, TAGparaDsc* pGeoMap, TAGparaDsc*  pCalib, TAGparaDsc* pConfig, TAGdataDsc* pTwPoint, Bool_t flagMC)
 : TACAactBaseNtuCluster(name, pNtuRaw, pNtuClus, pGeoMap, pCalib, pConfig, pTwPoint, flagMC)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TACAactNtuClusterP::~TACAactNtuClusterP()
{
}


//______________________________________________________________________________
//! Fill maps
void TACAactNtuClusterP::FillMaps()
{
   // Clear maps
   ClearMaps();

   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();

   // fill maps for cluster
   for (Int_t i = 0; i < fpNtuHit->GetHitsN(); i++) { // loop over hit crystals
      TACAhit* hit = fpNtuHit->GetHit(i);
      if(!hit->IsValid()) continue;
      Int_t id   = hit->GetCrystalId();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      TAGactNtuCluster2D::FillMaps(line, col, i);
   }
}

//______________________________________________________________________________
//! Shape cluster by region growing algorithm
//! Condition to grow: hit charge should be less than seed charge
//!
//! \param[in] numClus cluster number
//! \param[in] IndX index in X
//! \param[in] IndY index in Y
//! \param[in] seedCharge charge of previous seed
Bool_t TACAactNtuClusterP::ShapeCluster(Int_t numClus, Int_t IndX, Int_t IndY, double seedCharge)
{
   Int_t idx = IndY*fDimX+IndX;
   if ( fPixelMap.count(idx) == 0 ) return false; // empty place
   if ( fFlagMap[idx] != -1 ) return false; // already flagged

   int index = fIndexMap[idx];
   TAGobject* pixel = GetHitObject(index);
   TACAhit* hit = (TACAhit*)pixel;
   double charge = hit->GetCharge();

   if( charge > seedCharge ) return false;

   fFlagMap[idx] = numClus;
   pixel->SetFound(true);

   
   for(Int_t i = -1; i <= 1 ; ++i)
      if (CheckLine(IndX+i)) {
         ShapeCluster(numClus, IndX+i, IndY, charge);
      }
   
   for(Int_t j = -1; j <= 1 ; ++j)
      if (CheckCol(IndY+j)) {
         ShapeCluster(numClus, IndX, IndY+j, charge);
      }
   
   return true;
}

//______________________________________________________________________________
//! Found all clusters. Starting seed from local maximum 
void TACAactNtuClusterP::SearchCluster()
{
   fClustersN = 0;

   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();

   int nCry = pGeoMap->GetCrystalsN();

   // Search for cluster
   int loopCounter = 0;
   while (true) {
      double maxCharge = 0;
      int indexSeed = -1;
      for (Int_t i = 0; i < fpNtuHit->GetHitsN(); ++i) { // loop over hit crystals
         TACAhit* hit = fpNtuHit->GetHit(i);
         if (!hit->IsValid()) continue;
         if (hit->Found()) continue;
         double charge = hit->GetCharge();
         if (charge > maxCharge) {
            maxCharge = charge;
            indexSeed = i;
         }
      }

      if (indexSeed == -1) break; // maximum not found, not more clusters

      TACAhit* hit = fpNtuHit->GetHit(indexSeed);

      Int_t id   = hit->GetCrystalId();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      // Find cluster by region growing
      if ( ShapeCluster(fClustersN, col, line, hit->GetCharge() + 0.1) ) // +0.1 to meet condition on the seed
         fClustersN++;   
      else 
         break;   

      if ( ++loopCounter > nCry ) { // this should not happen (we can not have more clusters than crystals)
         Error("TACAactNtuClusterP::SearchCluster",
               "We got infinite loop!! Getting out.. num. cluster: %d, Event: %d",
               fClustersN, gTAGroot->CurrentEventId().EventNumber());
         break;
      }
   }

   if(FootDebugLevel(2))
     cout << "CA - Found : " << fClustersN << " clusters on event: " << gTAGroot->CurrentEventId().EventNumber() << endl;
}
