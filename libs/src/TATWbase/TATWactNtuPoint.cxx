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
TATWactNtuPoint::TATWactNtuPoint(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuPoint, TAGparaDsc* pGeoMap, TAGparaDsc* pCalMap)
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

   fparGeoTW = (TATWparGeo*) fpGeoMap->Object();

   Float_t barWidth = fparGeoTW->GetBarWidth();
   fDefPosErr = barWidth/TMath::Sqrt(12.);

   fparCal = (TATWparCal*) fpCalMap->Object();

   fgeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   fparGeo = (TAGparGeo*)gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object();
   fZbeam = fparGeo->GetBeamPar().AtomicNumber;

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
   
   fpHisDist = new TH1F("twDist", "TW - Minimal distance between clusterized hits", 100, 0., 10);
   AddHistogram(fpHisDist);
  
   for(int iZ=1; iZ < fZbeam+1; iZ++) {
     fpHisDeltaE.push_back(new TH1F(Form("DeltaE_Z%d",iZ),Form("DeltaE_Z%d",iZ),1000,-25.,25.));
     
     AddHistogram(fpHisDeltaE[iZ-1]);
     
     fpHisDeltaTof.push_back( new TH1F(Form("DeltaTof_Z%d",iZ),Form("DeltaTof_Z%d",iZ),10000,-5.,5) );
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
   mapHitX.clear();
   mapHitY.clear();
   mapMoreHits.clear();
   mapLessHits.clear();

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
   // if (nHitsX == 0) return false;
   
   Int_t nHitsY = pNtuHit->GetHitN(LayerY);
   // if (nHitsY == 0) return false;

   if(FootDebugLevel(1)) {
     cout<<""<<endl;
     cout<<"Hits::  "<<nHitsX<<" "<<nHitsY<<endl;
   }

   Int_t nHitsX_good(0); 
   Int_t nHitsY_good(0); 
   Int_t totChargeX(0); 
   Int_t totChargeY(0); 

   for (Int_t idx = 0; idx < nHitsX; ++idx) {  // loop over front TW hits

     TATWntuHit* hitX = pNtuHit->GetHit(idx, LayerX);

     if(FootDebugLevel(1))
       cout<<"Z_x::"<<hitX->GetChargeZ()<<"  bar_x::"<<hitX->GetBar()<<endl;
     
     if(!hitX) continue;
     if((Int_t)hitX->GetChargeZ()<=0) continue; //exclude neutrons and hits with Z charge < 0
     nHitsX_good++;
     totChargeX+=hitX->GetChargeZ();

     mapHitX[idx] = hitX;

     if(FootDebugLevel(1))
       cout<<"X  eloss::"<<hitX->GetEnergyLoss()<<" tof::"<<hitX->GetToF()<<" Z::"<<hitX->GetChargeZ()<<" x::"<<hitX->GetPosition()<<" y::"<<fparGeoTW->GetBarPosition(LayerX, hitX->GetBar())[1]<<endl;      	 

   }
       
   for (Int_t idy = 0; idy < nHitsY; ++idy) {  // loop over front TW hits

     TATWntuHit* hitY = pNtuHit->GetHit(idy, LayerY);
     
     if(FootDebugLevel(1))
       cout<<"Z_y::"<<hitY->GetChargeZ()<<"  bar_y::"<<hitY->GetBar()<<endl;
     
     if(!hitY) continue;
     if((Int_t)hitY->GetChargeZ()<=0) continue; //exclude neutrons and hits with Z charge < 0
     nHitsY_good++;
     totChargeY+=hitY->GetChargeZ();

     mapHitY[idy] = hitY;

     if(FootDebugLevel(1))
       cout<<"Y  eloss::"<<hitY->GetEnergyLoss()<<" tof::"<<hitY->GetToF()<<" Z::"<<hitY->GetChargeZ()<<" x::"<<fparGeoTW->GetBarPosition(LayerY, hitY->GetBar())[0]<<" y::"<<hitY->GetPosition()<<endl;
   }

   TATWpoint *point = nullptr;

   if( nHitsX_good >= nHitsY_good ) {
     
     mapMoreHits = mapHitX;
     mapLessHits = mapHitY;

     // if(totChargeX>fZbeam) isClustering=false;
       
   } else {
     
     mapMoreHits = mapHitY;
     mapLessHits = mapHitX;
     
     // if(totChargeY>fZbeam) isClustering=false;
     
   }

   
   if( nHitsX_good==0 || nHitsY_good==0 ) isClustering = false;
   
   if(!isClustering) {
     
     for (Int_t ihit = 0; ihit < pNtuHit->GetHitN(); ++ihit) {  // loop over all TW hits
       
       TATWntuHit *hit = pNtuHit->GetHit(ihit);
       Int_t layer = hit->GetLayer();
       
       if(!hit) continue;
       if((Int_t)hit->GetChargeZ()<=0) continue; //exclude neutrons and hits with Z charge < 0
       
       if(nHitsX_good >= nHitsY_good) {
	 
	 // if(totChargeX <= fZbeam) {  // take front hits as TW points
	   
	 if(layer==(Int_t)LayerY) continue; // skip for rear hits
	 point = pNtuPoint->NewPoint(hit->GetPosition(), fDefPosErr, hit, fparGeoTW->GetBarPosition(LayerX, hit->GetBar()).y(), fDefPosErr, hit, hit->GetLayer());
	   /*	   
	 }
	 else {
	   
	   // if(totChargeY <= fZbeam) {   // take rear hits as TW points
	     
	   //   if(layer==(Int_t)LayerX) continue;  // skip for front hits
	   //   point = pNtuPoint->NewPoint(fparGeoTW->GetBarPosition(LayerY,hit->GetBar()).x(), fDefPosErr, hit, hit->GetPosition(), fDefPosErr, hit);
	     
	   // }
	   // else {
	     Info("FindPoints()",Form("no TW point has been assigned: hitsX: %d hitsY: %d totZx: %d totZy: %d",nHitsX_good,nHitsY_good,totChargeX,totChargeY));
	     continue;  // no TW point can be assigned...
	 }
	   */
       }
       // }
       
       else {
	 
	 if(nHitsX_good < nHitsY_good) {
	   
	   // if(totChargeY <= fZbeam) {   // take rear hits as TW points
	     
	     if(layer==(Int_t)LayerX) continue;  // skip for front hits
	     point = pNtuPoint->NewPoint(fparGeoTW->GetBarPosition(LayerY,hit->GetBar()).x(), fDefPosErr, hit, hit->GetPosition(), fDefPosErr, hit, hit->GetLayer());
	     /*     
	   }
	   else {
	     
	     // if(totChargeX <= fZbeam) {  // take front hits as TW points
	       
	     //   if(layer==(Int_t)LayerY) continue; // skip for rear hits
	     //   point = pNtuPoint->NewPoint(hit->GetPosition(), fDefPosErr, hit, fparGeoTW->GetBarPosition(LayerX, hit->GetBar()).y(), fDefPosErr, hit);
	       
	     // }
	     
	     // else {
	       Info("FindPoints()",Form("no TW point has been assigned: hitsX: %d hitsY: %d totZx: %d totZy: %d",nHitsX_good,nHitsY_good,totChargeX,totChargeY));
	       continue;  // no TW point can be assigned...
	   }
	     */
	 }
       // }

       }  
   
       if(point==nullptr) { //continue;
	 Warning("FindPoints()",Form("no TW point has been assigned: hitsX: %d hitsY: %d totZx: %d totZy: %d",nHitsX_good,nHitsY_good,totChargeX,totChargeY));
	 continue;  // no TW point can be assigned...
       }
       
       point->SetChargeZ(hit->GetChargeZ());
       
       double z = fgeoTrafo->FromGlobalToTWLocal(fgeoTrafo->GetTWCenter()).z(); 
       TVector3 posLoc(point->GetPosition().x(),point->GetPosition().y(), z);
       TVector3 posG = fgeoTrafo->FromTWLocalToGlobal(posLoc);
       point->SetPositionG(posG);
       
	 if(FootDebugLevel(1))
	   cout<<"point ID::"<<point->GetPointID()<<" Z::"<<point->GetChargeZ()<<" barID::"<<hit->GetBar()<<endl;

	 // Info("Action()",Form("Z::%d",point->GetChargeZ()));
       if(FootDebugLevel(1)) {
	 cout<<"Layer::  "<<layer<<"  position:: x::"<<posLoc.x()<<" y::"<<posLoc.y()<<" z::"<<posLoc.z()<<endl;
	 cout<<"global::"<<posG.x()<<"  "<<posG.y()<<" "<<posG.z()<<endl;
	 cout<<"Z::"<<hit->GetChargeZ()<<" "<<endl;
	 
       }     
     }
   }

   
   else {  // TW clustering

     
     for(auto it1=mapMoreHits.begin(); it1!=mapMoreHits.end(); ++it1) {
       
       Int_t id1 = it1->first;
       TATWntuHit * hit1 = it1->second;
       Int_t layer1 = hit1->GetLayer();
       Int_t bar1 = hit1->GetBar();
       
       minDist = fparGeoTW->GetBarHeight() - 2*fparGeoTW->GetBarWidth(); // borders have no overlapp (44 cm -2*2cm)
       
       // use the time difference to compute position for the layer with more hits (and less Pile-up probability in order to disentangle ghosts)
       Double_t pos1 = GetPositionFromDeltaTime(layer1,bar1,hit1);
       
       best = false;
       
       if(FootDebugLevel(1))
	 cout<<"MORE  eloss::"<<hit1->GetEnergyLoss()<<" tof::"<<hit1->GetToF()<<" Z::"<<hit1->GetChargeZ()<<endl;
       
       for(auto it2=mapLessHits.begin(); it2!=mapLessHits.end(); ++it2) {
	 
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
	 
       	 TATWntuHit* hitmin = mapLessHits[minId];
       	 if(!hitmin) continue;

	 if(FootDebugLevel(1))
	   cout<<"MIN  eloss::"<<hitmin->GetEnergyLoss()<<" tof::"<<hitmin->GetToF()<<" Z::"<<hitmin->GetChargeZ()<<endl;

	 Int_t bar_min   = hitmin->GetBar();
	 
	 TVector3 posLoc = GetLocalPointPosition(layer1,pos1,bar_min);
	 
	 point = SetTWPoint(pNtuPoint,layer1,hit1,hitmin,posLoc);
	 if(!point)
	   return true;
	   
	 TVector3 posGlb = fgeoTrafo->FromTWLocalToGlobal(posLoc);
	 point->SetPositionG(posGlb);
	 
	 Int_t Z = hit1->GetChargeZ();
	 point->SetChargeZ(Z);
	 
	 if(FootDebugLevel(1))
	   cout<<"point ID::"<<point->GetPointID()<<" Z::"<<point->GetChargeZ()<<" barID::"<<hit1->GetBar()<<endl;
	 
	 if(FootDebugLevel(1)) {
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
	   fpHisDist->Fill(minDist);
	   
	   if( Z>0 && Z < fZbeam+1 ) {

	     // golden cut utile per un clustering pulito anche se ammazza la statistica
	     // if(Z==hitmin->GetChargeZ()) {
	       fpHisElossMean[Z-1]->Fill(point->GetEnergyLoss()/2.);
	       fpHisTofMean[Z-1]->Fill(point->GetMeanTof());
	       fpHisDeltaE[Z-1]->Fill(point->GetColumnHit()->GetEnergyLoss()-point->GetRowHit()->GetEnergyLoss());
	       fpHisDeltaTof[Z-1]->Fill(point->GetColumnHit()->GetTime()-point->GetRowHit()->GetTime());
	     // }
	     
	   }
	 }

       }  // if(best)

     }  // close moreHits loop

   }  // close clustering

   if(point==nullptr) { //continue;
     Warning("FindPoints()",Form("no TW point has been assigned: hitsX: %d hitsY: %d totZx: %d totZy: %d",nHitsX_good,nHitsY_good,totChargeX,totChargeY));
   }


   
   return true;
      
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
       Error("FindPoints",Form("TW Layer %d doesn't exist...check what's going wrong...",layer));

     if(FootDebugLevel(1)) {
       cout<<"posHit1::"<<posAlongBar<<" "<<posPerpendicular<<endl;
       cout<<"Z::"<<hit->GetChargeZ()<<"  Eloss::"<<hit->GetEnergyLoss()<<"  ToF::"<<hit->GetToF()<<endl;
     }

     return posAlongBar;

}

//___________________________________________//

Double_t TATWactNtuPoint::GetPositionFromBarCenter(Int_t layer, Int_t bar, TATWntuHit* hit) {

  Double_t posAlongBar1(-99), posAlongBar2(-99), posPerpendicular(-99);


     posAlongBar1  =  hit->GetPosition();

     if(layer==(Int_t)LayerX) {
       posAlongBar2  = fparGeoTW->GetBarPosition(LayerX, bar)[0];
       posPerpendicular  = fparGeoTW->GetBarPosition(LayerX, bar)[1];
     }
     else if(layer==(Int_t)LayerY) {
       posPerpendicular  = fparGeoTW->GetBarPosition(LayerY, bar)[0];
       posAlongBar2  = fparGeoTW->GetBarPosition(LayerY, bar)[1];
     }
     else
       Error("FindPoints",Form("TW Layer %d doesn't exist...check what's going wrong...",layer));

     if(FootDebugLevel(1)) {
       cout<<"posAlongBar   pos1::"<<posAlongBar1<<"  pos2::"<<posAlongBar2<<" delta::"<<TMath::Abs(posAlongBar2-posAlongBar1)<<"  posPerp::"<<posPerpendicular<<endl;
       cout<<"Z::"<<hit->GetChargeZ()<<"  Eloss::"<<hit->GetEnergyLoss()<<"  ToF::"<<hit->GetToF()<<endl;
     }

     return posPerpendicular;

}

//____________________________________________________//

TVector3 TATWactNtuPoint::GetLocalPointPosition(Int_t layer1, Double_t pos1, Int_t bar2)
{

    TVector3 posLoc(-99,-99,-99);
	
    if(layer1==(Int_t)LayerX) {
      posLoc.SetX( pos1 );
      posLoc.SetY( fparGeoTW->GetBarPosition(LayerY, bar2 )[0]);
    }
    else if(layer1==(Int_t)LayerY) {
      posLoc.SetX( fparGeoTW->GetBarPosition(LayerY, bar2 )[1] );
      posLoc.SetY( pos1 );
    }
    
    double zloc = fgeoTrafo->FromGlobalToTWLocal(fgeoTrafo->GetTWCenter()).z(); 
    posLoc.SetZ(zloc);
    // posLoc.SetZ( (fparGeoTW->GetBarPosition(layer1, bar1)[2] + fparGeoTW->GetBarPosition(hitmin->GetLayer(), bar_min)[2])/ 2.);
    
    return posLoc;
    
}

//___________________________________________________//

TATWpoint* TATWactNtuPoint::SetTWPoint(TATWntuPoint* pNtuPoint, Int_t layer1, TATWntuHit* hit1, TATWntuHit* hitmin, TVector3 posLoc)
{

  TATWpoint *point;    
  Int_t mainLayer = hit1->GetLayer();
  
  // if(hit1->GetChargeZ()==hitmin->GetChargeZ()) {
    // point = pNtuPoint->NewPoint(posLoc.X(), fDefPosErr, hit1, posLoc.Y(), fDefPosErr, hitmin);

    if(layer1==(Int_t)LayerX) {
      point = pNtuPoint->NewPoint(posLoc.X(), fDefPosErr, hit1, posLoc.Y(), fDefPosErr, hitmin, mainLayer);
    }
    else if(layer1==(Int_t)LayerY) {
      point = pNtuPoint->NewPoint(posLoc.X(), fDefPosErr, hitmin, posLoc.Y(), fDefPosErr, hit1, mainLayer);
    }

  // }
  // else
  //   point = nullptr;
  
     return point;

}
