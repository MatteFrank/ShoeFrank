/*!
 \file
 \version $Id: TATWactNtuPoint.cxx $
 \brief   Implementation of TATWactNtuPoint.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

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
TATWactNtuPoint::TATWactNtuPoint(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuPoint, TAGparaDsc* pGeoMap, TAGparaDsc* pCalMap, Bool_t isZmatch, Bool_t isZmc)
 : TAGaction(name, "TATWactNtuCluster - NTuplize cluster"),
   fpNtuRaw(pNtuRaw),
   fpNtuPoint(pNtuPoint),
   fpGeoMap(pGeoMap),
   fpCalMap(pCalMap),
   fDefPosErr(0),
   fIsZmatch(isZmatch),
   fIsZMCtrue(isZmc)
{
   AddDataIn(pNtuRaw,   "TATWntuRaw");
   AddDataOut(pNtuPoint, "TATWntuPoint");

   AddPara(pGeoMap, "TATWparGeo");
   AddPara(pCalMap, "TATWparCal");

   fparGeoTW = (TATWparGeo*) fpGeoMap->Object();

   Float_t barWidth = fparGeoTW->GetBarWidth();
   fDefPosErr = barWidth/TMath::Sqrt(12.);

   fparCal = (TATWparCal*) fpCalMap->Object();

   fgeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   fparGeo = (TAGparGeo*)gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object();
   fZbeam = fparGeo->GetBeamPar().AtomicNumber;

   fmapMultHit.clear();

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
   
   for(int iZ=1; iZ < fZbeam+1; iZ++) {
     fpHisDist.push_back(new TH1F(Form("minDist_Z%d",iZ), Form("TW - Minimal distance between clusterized hits - Z%d",iZ), 1000, 0., 10));
     AddHistogram(fpHisDist[iZ-1]);
  

   fpHisDeltaE.push_back(new TH1F(Form("DeltaE_Z%d",iZ),Form("DeltaE_Z%d",iZ),1000,-25.,25.));
     
     AddHistogram(fpHisDeltaE[iZ-1]);
     
     fpHisDeltaTof.push_back( new TH1F(Form("DeltaTof_Z%d",iZ),Form("DeltaTof_Z%d",iZ),1000,-5.,5) );
     AddHistogram(fpHisDeltaTof[iZ-1]);

     fpHisElossMean.push_back( new TH1F(Form("ElossMean_Z%d",iZ),Form("ElossMean_Z%d",iZ),480,0,120) );
     AddHistogram(fpHisElossMean[iZ-1]);
     
     fpHisTofMean.push_back( new TH1F(Form("TofMean_Z%d",iZ),Form("TofMean_Z%d",iZ),1400,6,20) );
     AddHistogram(fpHisTofMean[iZ-1]);
     
   }
   
   SetValidHistogram(kTRUE);
}

//______________________________________________________________________________
//
Bool_t TATWactNtuPoint::Action()
{
   fmapHitX.clear();
   fmapHitY.clear();
   fmapMoreHits.clear();
   fmapLessHits.clear();

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

   Bool_t isClustering=true;

   Float_t minDist = 99999;
   Int_t minId = -1;
   Bool_t best = false;
   
   Int_t nHitsX = pNtuHit->GetHitN(LayerX);
   Int_t nHitsY = pNtuHit->GetHitN(LayerY);

   if(FootDebugLevel(4)) {
     cout<<""<<endl;
     cout<<"Hits::  "<<nHitsX<<" "<<nHitsY<<endl;
   }

   Int_t nHitsX_good(0); 
   Int_t nHitsY_good(0); 
   Int_t totChargeX(0); 
   Int_t totChargeY(0); 

   fmapMultHit.clear();
   for (Int_t idx = 0; idx < nHitsX; ++idx) {  // loop over front TW hits

     TATWntuHit* hitX = pNtuHit->GetHit(idx, LayerX);

     if(FootDebugLevel(4))
       cout<<"Z_x::"<<hitX->GetChargeZ()<<"  bar_x::"<<hitX->GetBar()<<endl;
     
     if(!hitX) continue;
     if(!hitX->IsValid()) continue;
     if(fIsZMCtrue && hitX->GetEnergyLoss()<0) continue; // remove hit under threshold in MC with Z true
     if((Int_t)hitX->GetChargeZ()<=0) continue; //exclude neutrons and hits with Z charge < 0
     // if((Int_t)hitX->GetChargeZ()<0) continue; //exclude neutrons and hits with Z charge < 0
     // remove double hit in the same layer of the same track when Z = Ztrue
     if(fIsZMCtrue && IsMultHit(hitX) ) continue;
     
     nHitsX_good++;
     totChargeX+=hitX->GetChargeZ();

     fmapHitX[idx] = hitX;

     if(FootDebugLevel(4))
       cout<<"X  eloss::"<<hitX->GetEnergyLoss()<<" tof::"<<hitX->GetToF()<<" Z::"<<hitX->GetChargeZ()<<" x::"<<hitX->GetPosition()<<" y::"<<fparGeoTW->GetBarPosition(LayerX, hitX->GetBar())[1]<<endl;      	 

   }
       
   fmapMultHit.clear();
   for (Int_t idy = 0; idy < nHitsY; ++idy) {  // loop over front TW hits

     TATWntuHit* hitY = pNtuHit->GetHit(idy, LayerY);
     
     if(FootDebugLevel(4))
       cout<<"Z_y::"<<hitY->GetChargeZ()<<"  bar_y::"<<hitY->GetBar()<<endl;
     
     if(!hitY) continue;
     if(!hitY->IsValid()) continue;
     if(fIsZMCtrue && hitY->GetEnergyLoss()<0) continue; // remove hit under threshold in MC with Z true
     if((Int_t)hitY->GetChargeZ()<=0) continue; //exclude neutrons and hits with Z charge < 0
     // if((Int_t)hitY->GetChargeZ()<0) continue; //exclude neutrons and hits with Z charge < 0
     // remove double hit in the same layer of the same track when Z = Ztrue
     if(fIsZMCtrue && IsMultHit(hitY) ) continue;
     
     nHitsY_good++;
     totChargeY+=hitY->GetChargeZ();

     fmapHitY[idy] = hitY;

     if(FootDebugLevel(4))
       cout<<"Y  eloss::"<<hitY->GetEnergyLoss()<<" tof::"<<hitY->GetToF()<<" Z::"<<hitY->GetChargeZ()<<" x::"<<fparGeoTW->GetBarPosition(LayerY, hitY->GetBar())[0]<<" y::"<<hitY->GetPosition()<<endl;
   }

   TATWpoint *point = nullptr;

   if( nHitsX_good >= nHitsY_good ) {

     fmapMoreHits = fmapHitX;
     fmapLessHits = fmapHitY;
     
     if(fIsZMCtrue) { // remove double hit in the same layer of the same track when Zrec is not active
       if(totChargeX>fZbeam && totChargeY<fZbeam+1) { 
	 if(FootDebugLevel(1))
	   Warning("FindPoints()"," Zbeam: %d hitsX: %d hitsY: %d totZx: %d totZy: %d",fZbeam,nHitsX_good,nHitsY_good,totChargeX,totChargeY);
	 fmapMoreHits = fmapHitY;
	 fmapLessHits = fmapHitX;
       }
       else if(totChargeX>fZbeam && totChargeY>fZbeam) { 
	 if(FootDebugLevel(1))
	   Warning("FindPoints()","no TW point has been assigned: Zbeam: %d hitsX: %d hitsY: %d totZx: %d totZy: %d",fZbeam,nHitsX_good,nHitsY_good,totChargeX,totChargeY);
	 return true;
       }
     }

   } else { //if( nHitsY_good > nHitsX_good )
     

     fmapMoreHits = fmapHitY;
     fmapLessHits = fmapHitX;

     if(fIsZMCtrue) { // remove double hit in the same layer of the same track when Zrec is not active
       if(totChargeY>fZbeam && totChargeX<fZbeam+1) {  // remove double hit in the same layer of the same track when Zrec is not active
	 if(FootDebugLevel(1))
	   Warning("FindPoints()"," Zbeam: %d hitsX: %d hitsY: %d totZx: %d totZy: %d",fZbeam,nHitsX_good,nHitsY_good,totChargeX,totChargeY);
	 fmapMoreHits = fmapHitX;
	 fmapLessHits = fmapHitY;
       }
       else if(totChargeX>fZbeam && totChargeY>fZbeam) { 
	 if(FootDebugLevel(1))
	   Warning("FindPoints()","no TW point has been assigned: Zbeam: %d hitsX: %d hitsY: %d totZx: %d totZy: %d",fZbeam,nHitsX_good,nHitsY_good,totChargeX,totChargeY);
	 return true;
       }
     }

   }

   
   if( nHitsX_good==0 || nHitsY_good==0 ) isClustering = false;
   
   if(!isClustering) {
     
     for (Int_t ihit = 0; ihit < pNtuHit->GetHitN(); ++ihit) {  // loop over all TW hits
       
       TATWntuHit *hit = pNtuHit->GetHit(ihit);
       Int_t layer = hit->GetLayer();
       
       if(!hit) continue;
       if(!hit->IsValid()) continue;
       if(fIsZMCtrue && hit->GetEnergyLoss()<0) continue; // remove hit under threshold in MC with Z true
       if((Int_t)hit->GetChargeZ()<=0) continue; //exclude neutrons and hits with Z charge < 0
       
       if(nHitsX_good >= nHitsY_good) {
	 
	 if(layer==(Int_t)LayerY) continue; // skip for rear hits
	 point = pNtuPoint->NewPoint(hit->GetPosition(), fDefPosErr, hit, fparGeoTW->GetBarPosition(LayerX, hit->GetBar()).y(), fDefPosErr, hit, hit->GetLayer());

       } else {	 // if(nHitsX_good < nHitsY_good)
	   
	 if(layer==(Int_t)LayerX) continue;  // skip for front hits
	 point = pNtuPoint->NewPoint(fparGeoTW->GetBarPosition(LayerY,hit->GetBar()).x(), fDefPosErr, hit, hit->GetPosition(), fDefPosErr, hit, hit->GetLayer());
	 
       }  
   
       if(point==nullptr) { //continue;

       if(FootDebugLevel(1))

	 Warning("FindPoints()","no TW point has been assigned: hitsX: %d hitsY: %d totZx: %d totZy: %d",nHitsX_good,nHitsY_good,totChargeX,totChargeY);
	 continue;  // no TW point can be assigned...
       }
       
       point->SetChargeZ(hit->GetChargeZ());
       
       double z = fgeoTrafo->FromGlobalToTWLocal(fgeoTrafo->GetTWCenter()).z(); 
       TVector3 posLoc(point->GetPosition().x(),point->GetPosition().y(), z);
       TVector3 posGlb = fgeoTrafo->FromTWLocalToGlobal(posLoc);
       point->SetPositionGlb(posGlb);
       
       if(FootDebugLevel(4))
	 cout<<"point ID::"<<point->GetPointMatchMCtrkID()<<" Z::"<<point->GetChargeZ()<<" barID::"<<hit->GetBar()<<endl;
       
       if(FootDebugLevel(4)) {
	 cout<<"Layer::  "<<layer<<"  position:: x::"<<posLoc.x()<<" y::"<<posLoc.y()<<" z::"<<posLoc.z()<<endl;
	 cout<<"global::"<<posGlb.x()<<"  "<<posGlb.y()<<" "<<posGlb.z()<<endl;
	 cout<<"Z::"<<hit->GetChargeZ()<<" "<<endl;
	 
       }     
     }
   }
   
   
   else {  // TW clustering

     
     for(auto it1=fmapMoreHits.begin(); it1!=fmapMoreHits.end(); ++it1) {
       
       Int_t id1 = it1->first;
       TATWntuHit * hit1 = it1->second;
       Int_t layer1 = hit1->GetLayer();
       Int_t bar1 = hit1->GetBar();
       
       minDist = fparGeoTW->GetBarHeight() - 2*fparGeoTW->GetBarWidth(); // borders have no overlapp (44 cm -2*2cm)
       
       // use the time difference to compute position for the layer with more hits (and less Pile-up probability)
       Double_t pos1 = GetPositionFromDeltaTime(layer1,bar1,hit1);
       
       best = false;
       
       if(FootDebugLevel(4))
	 cout<<"MORE  eloss::"<<hit1->GetEnergyLoss()<<" tof::"<<hit1->GetToF()<<" Z::"<<hit1->GetChargeZ()<<endl;
       
       for(auto it2=fmapLessHits.begin(); it2!=fmapLessHits.end(); ++it2) {
	 
     	 Int_t id2 = it2->first;
     	 TATWntuHit * hit2 = it2->second;
     	 Int_t layer2 = hit2->GetLayer();
     	 Int_t bar2 = hit2->GetBar();
	 
     	 Double_t pos2  = GetPositionFromBarCenter(layer2,bar2,hit2);
	 
     	 Double_t dist = TMath::Abs(pos2 - pos1);
         
     	 if (dist < minDist) {
     	   minDist = dist;
     	   minId   = id2;
     	   best    = true;
     	 }

       }
       
       if (best) {
	 
       	 TATWntuHit* hitmin = fmapLessHits[minId];
       	 if(!hitmin) continue;

	 if(FootDebugLevel(4))
	   cout<<"MIN  eloss::"<<hitmin->GetEnergyLoss()<<" tof::"<<hitmin->GetToF()<<" Z::"<<hitmin->GetChargeZ()<<endl;

	 Int_t bar_min   = hitmin->GetBar();
	 
	 TVector3 posLoc = GetLocalPointPosition(layer1,pos1,bar1,bar_min);

	 if( fIsZmatch && !IsPointWithMatchedZ(hit1,hitmin) ) {
	   if(FootDebugLevel(1))
	     Warning("FindPoints()","no TW point has been assigned: mismatched Z Z1: %d Z2: %d",hit1->GetChargeZ(),hitmin->GetChargeZ());
	   continue;
	 }
	 
	 point = SetTWPoint(pNtuPoint,layer1,hit1,hitmin,posLoc);

	 if(!point) {
	   if(FootDebugLevel(1))
	     Warning("FindPoints()","no TW point has been assigned: hitsX: %d hitsY: %d totZx: %d totZy: %d",nHitsX_good,nHitsY_good,totChargeX,totChargeY);
	   
	   return true;
	 }

	 TVector3 posGlb = fgeoTrafo->FromTWLocalToGlobal(posLoc);
	 point->SetPositionGlb(posGlb);
	 
	 Int_t Z = hit1->GetChargeZ();
	 point->SetChargeZ(Z);

	 if(FootDebugLevel(4)) {
	   // if(hit1->GetMcTracksN()>1) {
	   cout<<endl<<"N_MCtracks::"<<hit1->GetMcTracksN()<<endl;
	   cout<<"fIsZmatch::"<<fIsZmatch<<"  hit1_Z::"<<hit1->GetChargeZ()<<" hitmin_Z::"<<hitmin->GetChargeZ()<<" point_Z::"<<point->GetChargeZ()<<endl;
	   cout<<"point ID::"<<point->GetPointMatchMCtrkID()<<" Z::"<<point->GetChargeZ()<<" barID::"<<hit1->GetBar()<<endl;
	   //   getchar();
	   // }
	   
	   cout<<"Bars::  "<<bar1<<"  "<<bar_min<<"  MainLayer::"<<point->GetMainLayer()<<endl;
	   cout<<"Eloss1::"<<hit1->GetEnergyLoss()<<"  Eloss2::"<<hitmin->GetEnergyLoss()<<endl;
	   cout<<"Eloss1::"<<point->GetEnergyLoss1()<<"  Eloss2::"<<point->GetEnergyLoss2()<<endl;
	   cout<<"hits  Tof1::"<<hit1->GetToF()<<"  Tof2::"<<hitmin->GetToF()<<endl;
	   cout<<"hits  Time1::"<<hit1->GetTime()<<"  Time2::"<<hitmin->GetTime()<<endl;
	   cout<<"point Tof1::"<<point->GetTof1()<<"  Tof2::"<<point->GetTof2()<<endl;
	   cout<<"Z::"<<Z<<"  "<<hitmin->GetChargeZ()<<" "<<endl;
	   cout<<"X::"<<posLoc.X()<<"  Y::"<<posLoc.Y()<<"  Z::"<<posLoc.Z()<<endl;
	   cout<<"X::"<<posGlb.X()<<"  Y::"<<posGlb.Y()<<"  Z::"<<posGlb.Z()<<endl;
	 }
	 
	 if (ValidHistogram()) {
	   
	   if( Z>0 && Z < fZbeam+1 ) {

	     fpHisDist[Z-1]->Fill(minDist);
	     fpHisElossMean[Z-1]->Fill(point->GetEnergyLoss()/2.);
	     fpHisTofMean[Z-1]->Fill(point->GetMeanTof());
	     fpHisDeltaE[Z-1]->Fill(point->GetColumnHit()->GetEnergyLoss()-point->GetRowHit()->GetEnergyLoss());
	     fpHisDeltaTof[Z-1]->Fill(point->GetColumnHit()->GetTime()-point->GetRowHit()->GetTime());
	     
	   }
	 }

       }  // if(best)

     }  // close moreHits loop

   }  // close clustering

   if(point==nullptr) { //continue;
     if(FootDebugLevel(1))
       Warning("FindPoints()","no TW point has been assigned: hitsX: %d hitsY: %d totZx: %d totZy: %d",nHitsX_good,nHitsY_good,totChargeX,totChargeY);
   }

   
   return true;
      
}


//___________________________________________//
Bool_t TATWactNtuPoint::IsMultHit(TATWntuHit *hit)
{

  for (Int_t j = 0; j < hit->GetMcTracksN(); ++j) {

    int trk_id = hit->GetMcTrackIdx(j);
    fmapMultHit[trk_id].push_back(hit->GetBar());

    if( fmapMultHit[trk_id].size()>1 ) {

      // cout<<fmapMultHit[trk_id].size()<<endl;
      return true;

    }

  }   
  
  return false;
}
//___________________________________________//
Double_t TATWactNtuPoint::GetPositionFromDeltaTime(Int_t layer, Int_t bar, TATWntuHit* hit) {

     Double_t posAlongBar(-99), posPerpendicular(-99);

     if(layer==(Int_t)LayerX) {
       posAlongBar  =  hit->GetPosition();
       posPerpendicular  = fparGeoTW->GetBarPosition(LayerX, bar)[1];
     }
     else if(layer==(Int_t)LayerY) {
       posPerpendicular  = fparGeoTW->GetBarPosition(LayerY, bar)[0];
       posAlongBar  = hit->GetPosition();
     }
     else
       Error("FindPoints","TW Layer %d doesn't exist...check what's going wrong...",layer);

     if(FootDebugLevel(4)) {
       cout<<"posHit1::"<<posAlongBar<<" "<<posPerpendicular<<endl;
       cout<<"Z::"<<hit->GetChargeZ()<<"  Eloss::"<<hit->GetEnergyLoss()<<"  ToF::"<<hit->GetToF()<<endl;
     }

     return posAlongBar;

}

//___________________________________________//

Double_t TATWactNtuPoint::GetPositionFromBarCenter(Int_t layer, Int_t bar, TATWntuHit* hit) {

  Double_t posAlongBar(-99), posPerpendicular(-99);


     posAlongBar  =  hit->GetPosition();

     if(layer==(Int_t)LayerX)
       posPerpendicular  = fparGeoTW->GetBarPosition(LayerX, bar)[1];  // get y bar position
     else if(layer==(Int_t)LayerY) 
       posPerpendicular  = fparGeoTW->GetBarPosition(LayerY, bar)[0];  // get x bar position
     else
       Error("FindPoints","TW Layer %d doesn't exist...check what's going wrong...",layer);

     if(FootDebugLevel(4)) {
       cout<<"posAlongBar::"<<posAlongBar<<"  posPerp::"<<posPerpendicular<<endl;
       cout<<"Z::"<<hit->GetChargeZ()<<"  Eloss::"<<hit->GetEnergyLoss()<<"  ToF::"<<hit->GetToF()<<endl;
     }

     return posPerpendicular;

}

//____________________________________________________//

TVector3 TATWactNtuPoint::GetLocalPointPosition(Int_t layer1, Double_t pos1, Int_t bar1, Int_t bar2)
{

    TVector3 posLoc(-99,-99,-99);
	
    if(layer1==(Int_t)LayerX) {
      // posLoc.SetX( pos1 );
      posLoc.SetX( fparGeoTW->GetBarPosition(LayerY, bar2 )[0]);
      posLoc.SetY( fparGeoTW->GetBarPosition(LayerX, bar1 )[1]);
    }
    else if(layer1==(Int_t)LayerY) {
      posLoc.SetX( fparGeoTW->GetBarPosition(LayerY, bar1 )[0] );
      posLoc.SetY( fparGeoTW->GetBarPosition(LayerX, bar2 )[1] );
    }
    
    double zloc = fgeoTrafo->FromGlobalToTWLocal(fgeoTrafo->GetTWCenter()).z(); 
    posLoc.SetZ(zloc);
    
    return posLoc;
    
}

//___________________________________________________//

TATWpoint* TATWactNtuPoint::SetTWPoint(TATWntuPoint* pNtuPoint, Int_t layer1, TATWntuHit* hit1, TATWntuHit* hitmin, TVector3 posLoc)
{

  TATWpoint *point;    
  Int_t mainLayer = hit1->GetLayer();
  
  if(layer1==(Int_t)LayerX) {
    point = pNtuPoint->NewPoint(posLoc.X(), fDefPosErr, hit1, posLoc.Y(), fDefPosErr, hitmin, mainLayer);
  }
  else if(layer1==(Int_t)LayerY) {
    point = pNtuPoint->NewPoint(posLoc.X(), fDefPosErr, hitmin, posLoc.Y(), fDefPosErr, hit1, mainLayer);
  }
  
  return point;

}

//_____________________________________________________________________//
Bool_t TATWactNtuPoint::IsPointWithMatchedZ(TATWntuHit* hit1, TATWntuHit* hitmin)
{

  Int_t Z1 = hit1->GetChargeZ();
  Int_t Zmin = hitmin->GetChargeZ();

  if(Z1 == Zmin)

    return true;

  else

    return false;
  
}
