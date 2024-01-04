/*!
 \file TATWactNtuPoint.cxx
 \brief   Implementation of TATWactNtuPoint.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAGroot.hxx"

#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"

#include "TAGnameManager.hxx"

#include "TATWactNtuPoint.hxx"

#include "TATWparameters.hxx"
/*!
 \class TATWactNtuPoint 
 \brief NTuplizer for TW reconstructed points. **
 */

//! Class Imp
ClassImp(TATWactNtuPoint);

//------------------------------------------+-----------------------------------
//! Default constructor.
TATWactNtuPoint::TATWactNtuPoint(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuPoint, TAGparaDsc* pGeoMap, TAGparaDsc* pConfMap , TAGparaDsc* pCalMap)
 : TAGaction(name, "TATWactNtuCluster - NTuplize cluster"),
   fpNtuRaw(pNtuRaw),
   fpNtuPoint(pNtuPoint),
   fpGeoMap(pGeoMap),
   fpConfMap(pConfMap),
   fpCalMap(pCalMap),
   fDefPosErr(0),
   fIsZmatch(false),
   fIsZMCtrue(false)
{
   AddDataIn(pNtuRaw,   "TATWntuHit");
   AddDataOut(pNtuPoint, "TATWntuPoint");

   AddPara(pGeoMap, "TATWparGeo");
   AddPara(pConfMap, "TATWparConf");
   AddPara(pCalMap, "TATWparCal");

   fparGeoTW = (TATWparGeo*) fpGeoMap->Object();

   Float_t barWidth = fparGeoTW->GetBarWidth();
   fDefPosErr = barWidth/TMath::Sqrt(12.);

   fparCal = (TATWparCal*) fpCalMap->Object();

   fgeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   fparGeo = (TAGparGeo*)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object();
   fZbeam = fparGeo->GetBeamPar().AtomicNumber;

   TATWparConf* parConf = (TATWparConf*) fpConfMap->Object();
   fIsZmatch = parConf->IsZmatching();
   fIsZMCtrue = parConf->IsZmc();

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
   
   TATWparGeo* pGeoMap  = (TATWparGeo*) fpGeoMap->Object();
  
   fpHisPointMap = new TH2F("twPointMap", "TW - Point map",
                            pGeoMap->GetNBars(), -21., 19.,
                            pGeoMap->GetNBars(), -21., 19.);
   // pGeoMap->GetNBars(), -pGeoMap->GetDimension()[1]/2., pGeoMap->GetDimension()[1]/2.);       
   AddHistogram(fpHisPointMap);
  
   fpHisElossTof = new TH2F("twPoint_Eloss_vs_Tof", "twPoint_Eloss_vs_Tof",3000,0.,30.,480,0.,120.);
   AddHistogram(fpHisElossTof);

   fpHisElossTofMean = new TH2F("twPoint_Eloss_vs_Tof_mean", "twPoint_Eloss_vs_Tof_mean",3000,0.,30.,480,0.,120.);
   AddHistogram(fpHisElossTofMean);

   for(int iZ=1; iZ < fZbeam+1; iZ++) {
     fpHisDist.push_back(new TH1F(Form("twMinDist_Z%d",iZ), Form("TW - Minimal distance between clusterized hits - Z%d",iZ), 100, 0., 10));
     AddHistogram(fpHisDist[iZ-1]);
  
     fpHisDeltaE.push_back(new TH1F(Form("twDeltaE_Z%d",iZ),Form("DeltaE_Z%d",iZ),1000,-25.,25.));
     AddHistogram(fpHisDeltaE[iZ-1]);
     
     fpHisDeltaTof.push_back( new TH1F(Form("twDeltaTof_Z%d",iZ),Form("DeltaTof_Z%d",iZ),1000,-5.,5) );
     AddHistogram(fpHisDeltaTof[iZ-1]);

     fpHisDeltaPosX.push_back( new TH1F(Form("twDeltaPosX_Z%d",iZ),Form("DeltaPosX_Z%d",iZ),400,-20.,20) );
     AddHistogram(fpHisDeltaPosX[iZ-1]);

     fpHisDeltaPosY.push_back( new TH1F(Form("twDeltaPosY_Z%d",iZ),Form("DeltaPosY_Z%d",iZ),400,-20.,20) );
     AddHistogram(fpHisDeltaPosY[iZ-1]);

     fpHisElossMean.push_back( new TH1F(Form("twElossMean_Z%d",iZ),Form("ElossMean_Z%d",iZ),480,0,120) );
     AddHistogram(fpHisElossMean[iZ-1]);
     
     fpHisTofMean.push_back( new TH1F(Form("twTofMean_Z%d",iZ),Form("TofMean_Z%d",iZ),1400,6,20) );
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
   TATWntuHit* pNtuHit      = (TATWntuHit*) fpNtuRaw->Object();
   TATWntuPoint* pNtuPoint  = (TATWntuPoint*) fpNtuPoint->Object();
   
   Double_t minDist = 99999;
   Int_t minId = -1;
   Bool_t best = false;
   
   Int_t nHitsX = pNtuHit->GetHitN(LayerX);
   Int_t nHitsY = pNtuHit->GetHitN(LayerY);
   
   if(FootDebugLevel(4))
     cout<<endl<<"Number of Hits   X:: "<<nHitsX<<" Y::"<<nHitsY<<endl;
   
   if(FootDebugLevel(2) && nHitsX==0 && nHitsY==0)
     Info("FindPoints()","no TW point can be found: 0 X and Y TW hits!!");
   
   Int_t nHitsX_good(0);
   Int_t nHitsY_good(0);
   Int_t totChargeX(0);
   Int_t totChargeY(0);
   
   fmapMultHit.clear();
   for (Int_t idx = 0; idx < nHitsX; ++idx) {  // loop over front TW hits
      
      TATWhit* hitX = pNtuHit->GetHit(idx, LayerX);
      
      if(FootDebugLevel(4))
         cout<<"Z_x::"<<hitX->GetChargeZ()<<"  bar_x::"<<hitX->GetBar()<<endl;
      
      if(!hitX) continue;
      if(!hitX->IsValid()) continue;
      if(fIsZMCtrue && hitX->GetEnergyLoss()<0) continue; // remove hit under threshold in MC with Z true-->cut included in IsValid (redundant)
      if((Int_t)hitX->GetChargeZ()<=0) continue; //exclude neutrons and hits with Z charge < 0
                                                 // remove multiple hit in the same layer of the same track when Z = Ztrue
      if(fIsZMCtrue && IsMultHit(hitX) ) continue;  // only for algorithm debug purpose: study points with Z true only in the case of one track matched per bar
      
      nHitsX_good++;
      totChargeX+=hitX->GetChargeZ();
      
      fmapHitX[idx] = hitX;
      
      if(FootDebugLevel(4))
         cout<<"X  eloss::"<<hitX->GetEnergyLoss()<<" tof::"<<hitX->GetToF()<<" Z::"<<hitX->GetChargeZ()<<" x::"<<hitX->GetPosition()<<" y::"<<fparGeoTW->GetBarPosition(LayerX, hitX->GetBar())[1]<<endl;
      
   }
   
   fmapMultHit.clear();
   for (Int_t idy = 0; idy < nHitsY; ++idy) {  // loop over front TW hits
      
      TATWhit* hitY = pNtuHit->GetHit(idy, LayerY);
      
      if(FootDebugLevel(4))
         cout<<"Z_y::"<<hitY->GetChargeZ()<<"  bar_y::"<<hitY->GetBar()<<endl;
      
      if(!hitY) continue;
      if(!hitY->IsValid()) continue;
      if(fIsZMCtrue && hitY->GetEnergyLoss()<0) continue; // remove hit under threshold in MC with Z true-->cut included in IsValid (redundant)
      if((Int_t)hitY->GetChargeZ()<=0) continue; //exclude neutrons and hits with Z charge < 0
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
   
   if(FootDebugLevel(2) && fmapLessHits.empty() && fmapMoreHits.empty())
     Info("FindPoints()","map with More and Less hits both empty...no way to make a TW points");
   

   // TW clustering
      
   for(auto it1=fmapMoreHits.begin(); it1!=fmapMoreHits.end(); ++it1) {
     
         Int_t id1 = it1->first;
         TATWhit * hit1 = it1->second;
         Int_t layer1 = hit1->GetLayer();
         Int_t bar1 = hit1->GetBar();
         
         minDist = fparGeoTW->GetBarHeight() - 2*fparGeoTW->GetBarWidth(); // borders have no overlapp (44 cm -2*2cm)
         vector<Double_t> v_dist;
         v_dist.clear();
         
         // use the time difference to compute position for the layer with more hits (and less Pile-up probability)  --> really used only in the case with fmapLessHits.size() > 1
         Double_t pos1 = GetPositionFromDeltaTime(layer1,bar1,hit1);
         
         best = false;
         
         if(FootDebugLevel(4))
            cout<<"MORE  eloss::"<<hit1->GetEnergyLoss()<<" tof::"<<hit1->GetToF()<<" Z::"<<hit1->GetChargeZ()<<endl;
         
         if(fmapLessHits.empty()) {
           Bool_t isSingleBarIncluded = false;

           if(isSingleBarIncluded) {
             if(IsInsideTwOverlappedXYBars(pos1,minDist))
               point = SetTWPoint(pNtuPoint,layer1,hit1,hit1,GetLocalPosSingleBar(pos1,hit1));
           }

           if(FootDebugLevel(2)) {
             if(!point)
               Info("FindPoints()","no TWpoint has been found in the case of one only bar in the %s",LayerName[(TLayer)layer1].data());
             else {
               Info("FindPoints()","TWpoint done with only one bar in the %s",LayerName[(TLayer)layer1].data());
               cout<<"recPosAlongBar::"<<pos1<<"  truePosAlongBar::"<<hit1->GetChargeChB()<<" dE::"<<hit1->GetEnergyLoss()<<" Zrec::"<<hit1->GetChargeZ()<<endl;
             }
           }

         } else if(fmapLessHits.size()==1) {

           best = true;
           minDist = 0;
           
         } else {  // more than one bar in the other layer: try to disentangle from ghosts using position along the bar with deltaTime
           
           for(auto it2=fmapLessHits.begin(); it2!=fmapLessHits.end(); ++it2) {
             
             Int_t id2 = it2->first;
             TATWhit * hit2 = it2->second;
             Int_t layer2 = hit2->GetLayer();
             Int_t bar2 = hit2->GetBar();
             
             Double_t pos2  = GetPositionFromBarCenter(layer2,bar2,hit2);
             
             Double_t dist = TMath::Abs(pos2 - pos1);
             v_dist.push_back(dist);
             
             if (dist < minDist) {
               minDist = dist;
               minId   = id2;
               best    = true;
             }
             
           }

         }  //  fmapLessHits.size()>1
         
           
         if (best) {
             
           TATWhit* hitmin;

           if(fmapLessHits.size()==1)  //only one bar-->take first (and only) entry in fmapLesshits
             hitmin = fmapLessHits[fmapLessHits.begin()->first];
           else
             hitmin = fmapLessHits[minId];
           
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
               Warning("FindPoints()","no TW point has been assigned for reasons to understand: hitsX: %d hitsY: %d totZx: %d totZy: %d",nHitsX_good,nHitsY_good,totChargeX,totChargeY);
             continue; 
           }
           
           if(FootDebugLevel(4)) {
             cout<<"fIsZmatch::"<<fIsZmatch<<"  hit1_Z::"<<hit1->GetChargeZ()<<" hitmin_Z::"<<hitmin->GetChargeZ()<<" point_Z::"<<point->GetChargeZ()<<endl;
             cout<<"point ID::"<<point->GetPointMatchMCtrkID()<<" Z::"<<point->GetChargeZ()<<" barID::"<<hit1->GetBar()<<endl;
             cout<<"Bars::  "<<bar1<<"  "<<bar_min<<"  MainLayer::"<<point->GetMainLayer()<<endl;
             cout<<"Eloss1::"<<hit1->GetEnergyLoss()<<"  Eloss2::"<<hitmin->GetEnergyLoss()<<endl;
             cout<<"Eloss1::"<<point->GetEnergyLoss1()<<"  Eloss2::"<<point->GetEnergyLoss2()<<endl;
             cout<<"hits  Tof1::"<<hit1->GetToF()<<"  Tof2::"<<hitmin->GetToF()<<endl;
             cout<<"hits  Time1::"<<hit1->GetTime()<<"  Time2::"<<hitmin->GetTime()<<endl;
             cout<<"point Tof1::"<<point->GetTof1()<<"  Tof2::"<<point->GetTof2()<<endl;
             cout<<"Z::"<<point->GetChargeZ()<<"  "<<hitmin->GetChargeZ()<<" "<<endl;
             
           }
           
         } // if(best)
         
         if(!point) {
           if(FootDebugLevel(1))
             Warning("FindPoints()","no TW point has been assigned: hitsX: %d hitsY: %d totZx: %d totZy: %d",nHitsX_good,nHitsY_good,totChargeX,totChargeY);
             
           if(FootDebugLevel(2)) {
             cout<<"recPosAlongBar::"<<pos1<<"  truePosAlongBar::"<<hit1->GetChargeChB()<<" dE::"<<hit1->GetEnergyLoss()<<" Zrec::"<<hit1->GetChargeZ()<<endl;
             for (auto it_v=v_dist.begin();it_v!=v_dist.end();++it_v)
               cout<<"distances found between TW hits::"<< *it_v<<endl<<endl;
           }
           continue; 
         }
         
         Int_t Z = point->GetChargeZ();
         TVector3 posLoc = fgeoTrafo->FromGlobalToTWLocal(point->GetPositionGlb());
         
         if (ValidHistogram()) {
           
           if( Z>0 && Z < fZbeam+1 ) {
             
             fpHisDist[Z-1]->Fill(minDist);
             fpHisElossMean[Z-1]->Fill(point->GetEnergyLoss()/2.);
             fpHisTofMean[Z-1]->Fill(point->GetMeanTof());
	     fpHisElossTof->Fill(point->GetToF(),point->GetMainEloss());
	     fpHisElossTofMean->Fill(point->GetMeanTof(),point->GetEnergyLoss()/2.);
             fpHisDeltaE[Z-1]->Fill(point->GetColumnHit()->GetEnergyLoss()-point->GetRowHit()->GetEnergyLoss());
             fpHisDeltaTof[Z-1]->Fill(point->GetColumnHit()->GetTime()-point->GetRowHit()->GetTime());

             // difference between the reconstructed TWpoint position and the true MC position along the bar in X and Y
             fpHisDeltaPosX[Z-1]->Fill( posLoc.X() - point->GetRowHit()->GetPosition() );
             fpHisDeltaPosY[Z-1]->Fill( posLoc.Y() - point->GetColumnHit()->GetPosition() );

           }
           
         }
           
   }  // close moreHits loop
      

   
   if(point==nullptr) { //continue;
     if(FootDebugLevel(1))
       Warning("FindPoints()","no TW point has been assigned: hitsX: %d hitsY: %d totZx: %d totZy: %d",nHitsX_good,nHitsY_good,totChargeX,totChargeY);
   }
   
   if (ValidHistogram()) {
     
     Int_t nPoints = pNtuPoint->GetPointsN();
     for( Int_t i = 0; i < nPoints; ++i ) {
       TATWpoint* aPoint =  pNtuPoint->GetPoint(i);
       TVector3 posGlb = aPoint->GetPositionGlb();
       TVector3 posLoc = fgeoTrafo->FromGlobalToTWLocal(posGlb);
       
       fpHisPointMap->Fill(posLoc[0], posLoc[1]);
     }
   }
   
   return true;
}

//___________________________________________
Bool_t TATWactNtuPoint::IsMultHit(TATWhit *hit)
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
Double_t TATWactNtuPoint::GetPositionFromDeltaTime(Int_t layer, Int_t bar, TATWhit* hit) {

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

Double_t TATWactNtuPoint::GetPositionFromBarCenter(Int_t layer, Int_t bar, TATWhit* hit) {

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
TVector3 TATWactNtuPoint::GetLocalPosSingleBar(Int_t posAlongBar, TATWhit *hit)
{

    TVector3 posLoc(-99,-99,-99);

    Int_t layer = hit->GetLayer();
    Int_t bar   = hit->GetBar();
    
    if(layer==(Int_t)LayerX) {
      posLoc.SetX( posAlongBar );
      posLoc.SetY( fparGeoTW->GetBarPosition(LayerX, bar )[1]);
    }
    else if(layer==(Int_t)LayerY) {
      posLoc.SetX( fparGeoTW->GetBarPosition(LayerY, bar )[0] );
      posLoc.SetY( posAlongBar );
    }
    
    double zloc = fgeoTrafo->FromGlobalToTWLocal(fgeoTrafo->GetTWCenter()).z(); 
    posLoc.SetZ(zloc);
    
    return posLoc;

}

//___________________________________________________//

TATWpoint* TATWactNtuPoint::SetTWPoint(TATWntuPoint* pNtuPoint, Int_t layer1, TATWhit* hit1, TATWhit* hitmin, TVector3 posLoc)
{

  TATWpoint *point;    
  Int_t mainLayer = hit1->GetLayer();
  
  if(layer1==(Int_t)LayerX) {
    point = pNtuPoint->NewPoint(posLoc.X(), fDefPosErr, hit1, posLoc.Y(), fDefPosErr, hitmin, mainLayer);
  }
  else if(layer1==(Int_t)LayerY) {
    point = pNtuPoint->NewPoint(posLoc.X(), fDefPosErr, hitmin, posLoc.Y(), fDefPosErr, hit1, mainLayer);
  }

  // set point global position and sensor Id
  TVector3 posGlb = fgeoTrafo->FromTWLocalToGlobal(posLoc);
  Float_t barThick = fparGeoTW->GetBarThick();
  // the point z position is the entrance of the LayerY. Modify for LayerX
  if(layer1==(Int_t)LayerX)
    posGlb.SetXYZ(posGlb.X(),posGlb.Y(),posGlb.Z()-barThick);
    
  point->SetPositionGlb(posGlb);
  point->SetSensorIdx(0);
  point->SetDeviceType(TAGgeoTrafo::GetDeviceType(FootBaseName("TATWparGeo")));

  // set point charge Z
  point->SetChargeZ(hit1->GetChargeZ());

  // set point Tof
  point->SetToF(hit1->GetToF());

  // set point main Eloss 
  point->SetMainEloss(hit1->GetEnergyLoss());

  // assign to the point the matched MC track id if no multi track hitting same bar, otherwise assign -1
  if(hit1->GetMcTracksN()==1)
    point->SetPointMatchMCtrkID(hit1->GetMcTrackIdx(0));
  else // pile-up
    point->SetPointMatchMCtrkID(-1);

  // cout<<"NmctracksY::"<<fColumnHit->GetMcTracksN()<<endl;

  if(FootDebugLevel(4)) {
    cout<<"X::"<<posLoc.X()<<"  Y::"<<posLoc.Y()<<"  Z::"<<posLoc.Z()<<endl;
    cout<<"X::"<<posGlb.X()<<"  Y::"<<posGlb.Y()<<"  Z::"<<posGlb.Z()<<endl;
  }
  
  
  return point;

}

//_____________________________________________________________________//
Bool_t TATWactNtuPoint::IsPointWithMatchedZ(TATWhit* hit1, TATWhit* hitmin)
{

  Int_t Z1 = hit1->GetChargeZ();
  Int_t Zmin = hitmin->GetChargeZ();

  if(Z1 == Zmin)

    return true;

  else

    return false;
  
}

//_____________________________________________________________________//
Bool_t  TATWactNtuPoint::IsInsideTwOverlappedXYBars( Double_t posAlongBar , Double_t  halfBarLength) {

  halfBarLength /= 2.; // hal bar length excluding borders (which have no overlapp (44 cm -2*2cm)/2)

  if(TMath::Abs(posAlongBar)<halfBarLength) {

    cout<<"| "<<posAlongBar<<" |  < "<<halfBarLength<<endl;
    return true;

  }  else {

    cout<<"| "<<posAlongBar<<" |  > "<<halfBarLength<<endl;
    return false;

  }
  
}
