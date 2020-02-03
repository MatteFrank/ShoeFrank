/*!
 \file
 \version $Id: TATWactNtuPoint.cxx $
 \brief   Implementation of TATWactNtuPoint.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "TATWparGeo.hxx"
#include "TATWparCal.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuPoint.hxx"

#include "TATWactNtuPoint.hxx"

/*!
 \class TATWactNtuPoint 
 \brief NTuplizer for TW reconstructed points. **
 */

ClassImp(TATWactNtuPoint);

//------------------------------------------+-----------------------------------
//! Default constructor.
TATWactNtuPoint::TATWactNtuPoint(const char* name, 
									 TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuPoint, TAGparaDsc* pGeoMap, TAGparaDsc* pCalMap)
 : TAGaction(name, "TATWactNtuCluster - NTuplize cluster"),
   fpNtuRaw(pNtuRaw),
   fpNtuPoint(pNtuPoint),
   fpGeoMap(pGeoMap),
   fpCalMap(pCalMap),
   fDefPosErr(0)
{
   AddDataIn(pNtuRaw,   "TATWntuRaw");
   AddDataOut(pNtuPoint, "TATWntuPoint");
   
   TATWparGeo* pGeo = (TATWparGeo*) fpGeoMap->Object();
   Float_t barWidth = pGeo->GetBarWidth();
   fDefPosErr = barWidth/TMath::Sqrt(12.);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TATWactNtuPoint::~TATWactNtuPoint()
{
   
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TATWactNtuPoint::CreateHistogram()
{
   DeleteHistogram();
   
   fpHisDist = new TH1F("twDist", "ToF Wall - Minimal distance between planes", 200, 0., 100);
   AddHistogram(fpHisDist);
  
   fpHisCharge1 = new TH1F("twCharge1", "ToF Wall - Charge layer 1", 1000, 0., 5e6);
   AddHistogram(fpHisCharge1);
   
   fpHisCharge2 = new TH1F("twCharge2", "ToF Wall - Charge layer 2", 1000, 0., 5e6);
   AddHistogram(fpHisCharge2);
   
   fpHisChargeTot = new TH1F("twChargeTot", "ToF Wall - Total charge", 1000, 0., 5e6);
   AddHistogram(fpHisChargeTot);
  
   SetValidHistogram(kTRUE);
}

//______________________________________________________________________________
//
Bool_t TATWactNtuPoint::Action()
{
   Bool_t ok = FindPoints();

   fpNtuPoint->SetBit(kValid);
   
   return true;
}

//______________________________________________________________________________
//  
Bool_t TATWactNtuPoint::FindPoints()
{
   TATWntuRaw* pNtuHit      = (TATWntuRaw*) fpNtuRaw->Object();
   TATWntuPoint* pNtuPoint  = (TATWntuPoint*) fpNtuPoint->Object();
   TATWparGeo* pGeoMap      = (TATWparGeo*) fpGeoMap->Object();
   TATWparCal* pCalMap      = (TATWparCal*) fpCalMap->Object();

   Float_t minDist = 99999;
   Int_t minId = -1;
   Bool_t best = false;
   
   Int_t layer1 = 0;
   Int_t nHits1 = pNtuHit->GetHitN(layer1);
   if (nHits1 == 0) return false;
   
   Int_t layer2 = 1;
   Int_t nHits2 = pNtuHit->GetHitN(layer2);
   if (nHits2 == 0) return false;

   for (Int_t i = 0; i < nHits1; ++i) {
      minDist = 99999; // should put a given value (2*BarWidth ?)
      
      TATWntuHit* hit1 = pNtuHit->GetHit(i, layer1);

      if(!hit1) continue;
      
      Int_t bar1 = hit1->GetBar();
      Float_t x  = pGeoMap->GetBarPosition(layer1, bar1)[0];
      Float_t y  = hit1->GetPosition();
      TVector2 pos1(x, y);
      
      best = false;
      for (Int_t j = 0; j < nHits2; ++j) {
         
         TATWntuHit* hit2 = pNtuHit->GetHit(j, layer2);

         if(!hit2) continue;
	 
         Int_t bar2 = hit2->GetBar();
         Float_t y  = pGeoMap->GetBarPosition(layer2, bar2)[1];
         Float_t x  = hit2->GetPosition();
         
         TVector2 pos2(x, y);
         Float_t dist = (pos2 - pos1).Mod();
         
         if (dist < minDist) {
            minDist = dist;
            minId   = j;
            best    = true;
         }
      }
      
      if (best) {
         TATWntuHit* hitmin = pNtuHit->GetHit(minId, 1);

         if(!hitmin) continue;
	 
         Int_t bar2   = hitmin->GetBar();
         Float_t xmin = pGeoMap->GetBarPosition(0, bar1)[0];
         Float_t ymin = pGeoMap->GetBarPosition(1, bar2)[1];
         Float_t zmin = (pGeoMap->GetBarPosition(0, bar1)[2] + pGeoMap->GetBarPosition(1, bar2)[2])/2.;

         TATWpoint* point = pNtuPoint->NewPoint(xmin, fDefPosErr, hit1, ymin, fDefPosErr, hitmin);
         TVector3 posG(xmin, ymin, zmin);
         point->SetPositionG(posG);

         Int_t Z = hit1->GetChargeZ(); // taking MC Z from hit since no reconstruction available
         point->SetChargeZ(Z);
	 
         if (ValidHistogram()) {
            fpHisDist->Fill(minDist);
            fpHisCharge1->Fill(point->GetEnergyLoss1());
            fpHisCharge2->Fill(point->GetEnergyLoss2());
            fpHisChargeTot->Fill(point->GetEnergyLoss());
         }
      }
   }

   return true;
}





