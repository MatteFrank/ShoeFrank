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

#include "TATWntuRaw.hxx"
#include "TATWntuPoint.hxx"

#include "TATWactNtuPoint.hxx"

#include "Parameters.h"
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

   AddPara(pGeoMap, "TATWparGeo");
   AddPara(pCalMap, "TATWparCal");

   fparGeo = (TATWparGeo*) fpGeoMap->Object();
   Float_t barWidth = fparGeo->GetBarWidth();
   fDefPosErr = barWidth/TMath::Sqrt(12.);

   fparCal = (TATWparCal*) fpCalMap->Object();
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

   Float_t minDist = 99999;
   Int_t minId = -1;
   Bool_t best = false;
   
   Int_t nHitsX = pNtuHit->GetHitN(LayerX);
   if (nHitsX == 0) return false;
   
   Int_t nHitsY = pNtuHit->GetHitN(LayerY);
   if (nHitsY == 0) return false;

   if(FootDebugLevel(1)) {
     cout<<""<<endl;
     cout<<"Hits::  "<<nHitsX<<" "<<nHitsY<<endl;
   }
   
   for (Int_t i = 0; i < nHitsX; ++i) {  // loop over front TW hits
      minDist = fparGeo->GetBarHeight() - fparGeo->GetBarWidth(); // borders have no overlapp
      
      TATWntuHit* hitX = pNtuHit->GetHit(i, LayerX);

      if(!hitX) continue;
      if((Int_t)hitX->GetChargeZ()<=0) continue; //exclude neutrons and hits with Z charge not assigned (automatically exclude hit with Eloss and ToF < 0)
      
      Int_t barX = hitX->GetBar();
      Float_t y  = fparGeo->GetBarPosition(LayerX, barX)[1];
      Float_t x  = hitX->GetPosition();
      if(FootDebugLevel(1)) {
	cout<<"posHitX::"<<x<<" "<<y<<endl;
	cout<<"ToF::"<<hitX->GetToF()<<"  Z::"<<hitX->GetChargeZ()<<endl;
      }

      TVector2 pos1(x, y);

      best = false;
      for (Int_t j = 0; j < nHitsY; ++j) {  // loop over rear TW hits
         
         TATWntuHit* hitY = pNtuHit->GetHit(j, LayerY);

         if(!hitY) continue;
	 if((Int_t)hitY->GetChargeZ()<=0) continue; //exclude neutrons and hits with Z charge not assigned (automatically exclude hit with Eloss and ToF < 0)
	 
         Int_t barY = hitY->GetBar();
	 Float_t x  = fparGeo->GetBarPosition(LayerY, barY)[0];
	 Float_t y  = hitY->GetPosition();         
	 if(FootDebugLevel(1)) {
	   cout<<"posHitY::"<<x<<" "<<y<<endl;
	   cout<<"ToF::"<<hitY->GetToF()<<"  Z::"<<hitY->GetChargeZ()<<endl;
	 }
	 
	 TVector2 pos2(x, y);
         Float_t dist = (pos2 - pos1).Mod();
         
         if (dist < minDist) {
            minDist = dist;
            minId   = j;
            best    = true;
         }
      }
      
      if (best) {
         TATWntuHit* hitmin = pNtuHit->GetHit(minId, LayerY);
         if(!hitmin) continue;
	 
         Int_t barY   = hitmin->GetBar();
         Float_t xmin = fparGeo->GetBarPosition(LayerY, barY)[0];
         Float_t ymin = fparGeo->GetBarPosition(LayerX, barX)[1];
         Float_t zmin = (fparGeo->GetBarPosition(LayerX, barX)[2] + fparGeo->GetBarPosition(LayerY, barY)[2])/2.;

         TATWpoint* point = pNtuPoint->NewPoint(xmin, fDefPosErr, hitX, ymin, fDefPosErr, hitmin);
         TVector3 posG(xmin, ymin, zmin);
         point->SetPositionG(posG);

         Int_t Z = hitX->GetChargeZ();
         point->SetChargeZ(Z);

	 if(FootDebugLevel(1)) {
	   cout<<"Bars::  "<<barX<<"  "<<barY<<endl;
	   cout<<"Z::"<<Z<<"  "<<hitmin->GetChargeZ()<<" "<<endl;
	   cout<<"Posmin::"<<xmin<<"  "<<ymin<<" "<<zmin<<endl;
	   cout<<"Posmin::"<<x<<"  "<<hitmin->GetPosition()<<endl;
	 }
	 
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





