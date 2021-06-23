/*!
 \file
 \version $Id: TAVTactBaseTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAVTactBaseTrack.
          Base class for VTX and IT
 */
#include "TClonesArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TVector3.h"
#include "TVector2.h"

//BM
#include "TABMntuTrack.hxx"

// First
#include "TAGgeoTrafo.hxx"

//TAG
#include "TAGroot.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparCal.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactBaseTrack.hxx"

#include "TAVTbaseDigitizer.hxx"

/*!
 \class TAVTactBaseTrack 
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAVTactBaseTrack);

Bool_t TAVTactBaseTrack::fgRefit = true;
//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactBaseTrack::TAVTactBaseTrack(const char* name, 
										 TAGdataDsc* pNtuClus,TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig,  
										 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib)
: TAGaction(name, "TAVTactNtuTrack - Base NTuplize Tracker"),
  fpNtuTrack(pNtuTrack),
  fpNtuClus(pNtuClus),
  fpConfig(pConfig),
  fpGeoMap(pGeoMap),
  fpCalib(pCalib),
  fTracksMaximum(100),
  fRequiredClusters(3),
  fSearchClusDistance(100),
  fGraphU(new TGraphErrors()),
  fGraphV(new TGraphErrors())
{
   AddPara(pGeoMap, "TAVTbaseParGeo");
   AddPara(pConfig, "TAVTbaseParConf");
   if (pCalib)
	  AddPara(pCalib, "TAVTbaseParCal");
   
   
   TString tmp(name);
   fPrefix = tmp(0,2);
   
   fTitleDev = fPrefix;
   if (fPrefix.Contains("vt"))
      fTitleDev = "Vertex";
   else if (fPrefix.Contains("it"))
      fTitleDev = "Inner Tracker";
   else if (fPrefix.Contains("ir"))
      fTitleDev = "Interaction Region";
   else
      printf("Wrong prefix for histograms !");

   
   TAVTbaseParConf* config = (TAVTbaseParConf*) fpConfig->Object();
   fRequiredClusters   = config->GetAnalysisPar().PlanesForTrackMinimum;
   fSearchClusDistance = config->GetAnalysisPar().SearchHitDistance;
   SetGeoTrafo(TAGgeoTrafo::GetDefaultActName().Data());

}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseTrack::~TAVTactBaseTrack()
{   
   delete fGraphU;
   delete fGraphV;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseTrack::CreateHistogram()
{
   DeleteHistogram();   
   TAVTbaseParGeo* pGeoMap  = (TAVTbaseParGeo*) fpGeoMap->Object();
   
   fpHisPixelTot = new TH1F(Form("%sTrackedClusPixTot", fPrefix.Data()), Form("%s - Total # pixels per tracked clusters", fTitleDev.Data()), 100, -0.5, 99.5);
   AddHistogram(fpHisPixelTot);
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
     fpHisTrackMap[i] = new TH2F(Form("%sTrackMap%d", fPrefix.Data(), i+1), Form("%s - Tracks map for sensor %d", fTitleDev.Data(), i+1),
                                 100, -pGeoMap->GetPitchY()*pGeoMap->GetPixelsNy()/2., pGeoMap->GetPitchY()*pGeoMap->GetPixelsNy()/2.,
                                 100, -pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2., pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2.);
     fpHisTrackMap[i]->SetMarkerStyle(24);
     fpHisTrackMap[i]->SetMarkerSize(1.);
     fpHisTrackMap[i]->SetMarkerColor(3);
     fpHisTrackMap[i]->SetStats(kFALSE);
     AddHistogram(fpHisTrackMap[i]);
	  
	  fpHisPixel[i] = new TH1F(Form("%sTrackedClusPix%d", fPrefix.Data(), i+1), Form("%s - # pixels per tracked clusters of sensor %d", fTitleDev.Data(), i+1), 100, -0.5, 99.5);
	  AddHistogram(fpHisPixel[i]);
	  
	  fpHisResX[i] = new TH1F(Form("%sResX%d", fPrefix.Data(), i+1), Form("%s - ResidualX of sensor %d", fTitleDev.Data(), i+1), 400, -0.01, 0.01);
	  AddHistogram(fpHisResX[i]);
	  fpHisResY[i] = new TH1F(Form("%sResY%d", fPrefix.Data(), i+1), Form("%s - ResidualY of sensor %d", fTitleDev.Data(), i+1), 400, -0.01, 0.01);
	  AddHistogram(fpHisResY[i]);
   }
   
   fpHisResTotX = new TH1F(Form("%sResTotX", fPrefix.Data()), Form("%s - Total ResidualX", fTitleDev.Data()), 400, -0.01, 0.01);
   fpHisResTotY = new TH1F(Form("%sResTotY", fPrefix.Data()), Form("%s - Total ResidualY", fTitleDev.Data()), 400, -0.01, 0.01);
   AddHistogram(fpHisResTotX);
   AddHistogram(fpHisResTotY);
   
   fpHisChi2TotX = new TH1F(Form("%sChi2TotX", fPrefix.Data()), Form("%s - Total Chi2 X", fTitleDev.Data()), 200, 0, 20);
   fpHisChi2TotY = new TH1F(Form("%sChi2TotY", fPrefix.Data()), Form("%s - Total Chi2 Y", fTitleDev.Data()), 200, 0, 20);
   AddHistogram(fpHisChi2TotX);
   AddHistogram(fpHisChi2TotY);
   
   fpHisTrackEvt = new TH1F(Form("%sTrackEvt", fPrefix.Data()), Form("%s - Number of tracks per event", fTitleDev.Data()), 20, -0.5, 19.5);
   AddHistogram(fpHisTrackEvt);
   
   fpHisTrackClus = new TH1F(Form("%sTrackClus", fPrefix.Data()), Form("%s - Number of clusters per track", fTitleDev.Data()), 9, -0.5, 8.5);
   AddHistogram(fpHisTrackClus);
   
   fpHisClusSensor = new TH1F(Form("%sClusSensor", fPrefix.Data()), Form("%s - Number of tracked clusters per sensor", fTitleDev.Data()), 9, -0.5, 8.5);
   AddHistogram(fpHisClusSensor);
   
   fpHisTheta = new TH1F(Form("%sTrackTheta", fPrefix.Data()), Form("%s - Track polar distribution", fTitleDev.Data()), 500, 0, 90);
   AddHistogram(fpHisTheta);
   
   fpHisPhi = new TH1F(Form("%sTrackPhi", fPrefix.Data()), Form("%s - Track azimutal distribution (#theta > 5)", fTitleDev.Data()), 500, -180, 180);
   AddHistogram(fpHisPhi);
   
   fpHisBeamProf = new TH2F(Form("%sBeamProf", fPrefix.Data()), Form("%s -  Beam Profile", fTitleDev.Data()),
							100, -pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2., pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2., 
							100, -pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2., pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2.);
   fpHisBeamProf->SetStats(kFALSE);
   AddHistogram(fpHisBeamProf);
   
   fpHisMeanPixel = new TH1F(Form("%sMeanClusPix", fPrefix.Data()), Form("%s - mean pixels per tracked clusters", fTitleDev.Data()), 100, -0.5, 99.5);
   AddHistogram(fpHisMeanPixel);
   
   Int_t adc = TAVTbaseDigitizer::GetTotAdcDepth();
   Int_t bin = TMath::Power(2, adc);
   fpHisMeanCharge = new TH1F(Form("%sMeanClusChg", fPrefix.Data()), Form("%s - mean charge per tracked clusters", fTitleDev.Data()), bin/2., 0, bin);
   AddHistogram(fpHisMeanCharge);

   fpHisClusLeft = new TH1F(Form("%sClusLeft", fPrefix.Data()), Form("%s - Clusters left per sensor", fTitleDev.Data()), 8, 1, 8);
   AddHistogram(fpHisClusLeft);
   
   fpHisClusLeftPix = new TH2F(Form("%sClusLeftPix", fPrefix.Data()), Form("%s - Number of pixels for left clusters vs sensor", fTitleDev.Data()),
                               10, 0, 9, 50, 1, 50);
   fpHisClusLeftPix->SetStats(kFALSE);
   AddHistogram(fpHisClusLeftPix);
   
   SetValidHistogram(kTRUE);
   
   return;
}



//_____________________________________________________________________________
//  
Bool_t TAVTactBaseTrack::AppyCuts(TAVTbaseTrack* track)
{
   Bool_t valid = false;  
   
   TAVTbaseParConf* pConfig = (TAVTbaseParConf*) fpConfig->Object();
   if (track->GetClustersN() >= pConfig->GetAnalysisPar().PlanesForTrackMinimum )
  	  valid = true;
   
   return valid;
}

//_____________________________________________________________________________
//  
void TAVTactBaseTrack::UpdateParam(TAVTbaseTrack* track)
{
   TVector3           lineOrigin;  // origin in the tracker system
   TVector3           lineSlope;   // slope along z-axis in tracker system
   
   // init
   lineOrigin.SetXYZ(0.,0.,0.);
   lineSlope.SetXYZ(0.,0.,1.);
   
   TAVTbaseParGeo* pGeoMap = (TAVTbaseParGeo*) fpGeoMap->Object();
   
   Int_t nClusters = track->GetClustersN();
   
   if( nClusters == 2) {
	  TAVTbaseCluster* cluster0 = track->GetCluster(0);
	  TAVTbaseCluster* cluster1 = track->GetCluster(1);
	  
     if(FootDebugLevel(1))
		 printf("TAVTactNtuTrack::Analyse track with %d cluster taking origin from it and slope 0\n", track->GetClustersN());
	  Double_t x0 = cluster0->GetPositionG()[0];
	  Double_t y0 = cluster0->GetPositionG()[1];
	  
	  Double_t x1 = cluster1->GetPositionG()[0];
	  Double_t y1 = cluster1->GetPositionG()[1];
	  
	  Double_t z0 = pGeoMap->GetSensorPosition(cluster0->GetSensorIdx()).Z();
	  Double_t z1 = pGeoMap->GetSensorPosition(cluster1->GetSensorIdx()).Z();
	  
     if(FootDebugLevel(1))
		 printf( "TAVTactNtuTrack::Analyze cluster[pl %d]=(%.2f, %.2f, %.2f) cluster[pl %d]=(%.2f, %.2f, %.2f)\n",
				cluster0->GetSensorIdx(), x0, y0, z0,
				cluster1->GetSensorIdx(), x1, y1, z1);
	  
	  if( z1-z0 != 0.) {
		 lineOrigin(0) = (z1*x0-z0*x1)/(z1-z0);
		 lineOrigin(1) = (z1*y0-z0*y1)/(z1-z0);
		 lineSlope(0)  = (x1-x0)/(z1-z0);
		 lineSlope(1)  = (y1-y0)/(z1-z0);
	  }
   } else {
	  Double_t x, dx;
	  Double_t y, dy;
	  Double_t z, dz;
	  
	  fGraphU->Set(nClusters);
	  fGraphV->Set(nClusters);
	  
	  for (Int_t i = 0; i < nClusters; ++i) {
		 TAVTbaseCluster* cluster = track->GetCluster(i);
		 x  = cluster->GetPositionG()(0);
		 y  = cluster->GetPositionG()(1);
		 z  = cluster->GetPositionG()(2);
		 dx = cluster->GetPosError()(0);
		 dy = cluster->GetPosError()(1);
		 dz = cluster->GetPosError()(2);
		 fGraphU->SetPoint(i, z, x);
		 fGraphU->SetPointError(i, dz, dx);
		 
		 fGraphV->SetPoint(i, z, y);
		 fGraphV->SetPointError(i, dz, dy);
	  }
	  
	  fGraphU->Fit("pol1", "Q");
	  TF1* polyU = fGraphU->GetFunction("pol1");
	  (lineOrigin)(0) = polyU->GetParameter(0);
	  (lineSlope)(0)  = polyU->GetParameter(1);
	  fGraphV->Fit("pol1", "Q");
	  TF1* polyV = fGraphV->GetFunction("pol1");
	  (lineOrigin)(1) = polyV->GetParameter(0);
	  (lineSlope)(1)  = polyV->GetParameter(1);
   }
   
   track->SetLineValue(lineOrigin, lineSlope);
}

//_____________________________________________________________________________
//  
void TAVTactBaseTrack::FillHistogramm(TAVTbaseTrack* track)
{   
   fpHisTheta->Fill(track->GetTheta());
   fpHisPhi->Fill(track->GetPhi());
   
   TAVTbaseParGeo* pGeoMap = (TAVTbaseParGeo*)fpGeoMap->Object();
   
   fpHisTrackClus->Fill(track->GetClustersN());
   for (Int_t i = 0; i < track->GetClustersN(); ++i) {
	  TAVTbaseCluster * cluster = track->GetCluster(i);
	  cluster->SetFound();
	  Int_t idx = cluster->GetSensorIdx();
	  fpHisPixelTot->Fill(cluster->GetPixelsN());
	  fpHisPixel[idx]->Fill(cluster->GetPixelsN());
	  
	  Float_t posZ       = cluster->GetPositionG()[2];
	  TVector3 impact    = track->Intersection(posZ);
   
     TVector3 impactLoc =  pGeoMap->Detector2Sensor(idx, impact);
      
     fpHisTrackMap[idx]->Fill(impactLoc[0], impactLoc[1]);
	  fpHisResTotX->Fill(impact[0]-cluster->GetPositionG()[0]);
	  fpHisResTotY->Fill(impact[1]-cluster->GetPositionG()[1]);
	  fpHisResX[idx]->Fill(impact[0]-cluster->GetPositionG()[0]);
	  fpHisResY[idx]->Fill(impact[1]-cluster->GetPositionG()[1]);
	  fpHisClusSensor->Fill(idx+1);
   }
   fpHisChi2TotX->Fill(track->GetChi2U());
   fpHisChi2TotY->Fill(track->GetChi2V());
   
   TVector3 origin = track->GetOrigin();
   fpHisBeamProf->Fill(origin.X(), origin.Y());
   
   fpHisMeanPixel->Fill(track->GetMeanPixelsN());
   fpHisMeanCharge->Fill(track->GetMeanCharge());
}

//_____________________________________________________________________________
//  
void TAVTactBaseTrack::FillHistogramm()
{
   TAVTbaseParGeo* pGeoMap   = (TAVTbaseParGeo*) fpGeoMap->Object();
   TAVTntuCluster* pNtuClus  = (TAVTntuCluster*) fpNtuClus->Object();
   TAVTntuTrack*   pNtuTrack = (TAVTntuTrack*)   fpNtuTrack->Object();
   
   fpHisTrackEvt->Fill(pNtuTrack->GetTracksN());
   if (pNtuTrack->GetTracksN() == 0)
	  fpHisClusSensor->Fill(0);
   
   for (Int_t iPlane = 0; iPlane < pGeoMap->GetSensorsN(); ++iPlane) {
	  
	  TClonesArray* list = pNtuClus->GetListOfClusters(iPlane);
	  Int_t nClusters = pNtuClus->GetClustersN(iPlane);
	  if ( nClusters == 0) continue;
	  
	  Int_t left = 0;
	  for( Int_t iClus = 0; iClus < nClusters; ++iClus) { 
		 
		 TAVTcluster* cluster = (TAVTcluster*)list->At(iClus);
		 if (!cluster->Found()) {
			fpHisClusLeftPix->Fill(iPlane+1, cluster->GetPixelsN());
			left++;
		 }
	  }
	  static Float_t mean[36];
	  Int_t evtNumber = gTAGroot->CurrentEventNumber();
	  mean[iPlane] = evtNumber/(evtNumber+1.)*mean[iPlane] + 1./(evtNumber+1.)*(left/nClusters);
	  fpHisClusLeft->SetBinContent(iPlane+1, mean[iPlane]*100);
   }   
}

//_____________________________________________________________________________
//
void TAVTactBaseTrack::SetChargeProba()
{
   if (fpCalib == 0x0) return;
   TAVTbaseParCal* pCalib   = (TAVTbaseParCal*) fpCalib->Object();
   
   Int_t nTrack = GetTracksN();
   if (nTrack == 0) return;
   
   for (Int_t iTrack = 0; iTrack < nTrack; ++iTrack) {
      
      Int_t nPixelsTot = 0;
      Float_t nPixelsMean;
      TAVTbaseTrack* track = GetTrack(iTrack);
      Int_t nClus = track->GetClustersN();
      
      for (Int_t i=0;i<nClus;i++) {
         TAVTbaseCluster* clus = track->GetCluster(i);
         nPixelsTot += clus->GetPixelsN();
      }
      nPixelsMean = nPixelsTot/nClus;
      const TArrayF* proba =  pCalib->GetChargeProba(nPixelsMean);
      track->SetChargeProba(proba);
      track->SetChargeMaxProba(pCalib->GetChargeMaxProba());
      track->SetChargeWithMaxProba(pCalib->GetChargeWithMaxProba());
      
      proba =  pCalib->GetChargeProbaNorm(nPixelsMean);
      track->SetChargeProbaNorm(proba);
      track->SetChargeMaxProbaNorm(pCalib->GetChargeMaxProbaNorm());
      track->SetChargeWithMaxProbaNorm(pCalib->GetChargeWithMaxProbaNorm());
   }   
}


//_____________________________________________________________________________
//  
//TAVTbaseCluster* TAVTactBaseTrack::NearestCluster(TAVTbaseTrack *aTrack, Int_t iSensor)
//{   
//   // For a given track, return the index of the nearest hit of a given plane
//   TAVTntuCluster* pNtuClus = (TAVTntuCluster*) fpNtuClus->Object();
//
//   Double_t minDist = 1.e9;
//   Double_t distance;
//   TAVTbaseCluster *tCluster;
//   
//   TAVTbaseCluster *tNearest = 0x0;
//   for (Int_t ht = 0; ht < pNtuClus->GetClustersN(iSensor); ++ht) { // loop on hits
//	  tCluster = pNtuClus->GetCluster(iSensor, ht);
//	  distance = tCluster->Distance( aTrack) ;
//	  if( distance < minDist) {
//		 minDist = distance;
//		 tNearest = tCluster;
//	  }
//   } //end loop on hits
//   
//   return tNearest;
//}

//_____________________________________________________________________________
//  
//TAVTbaseTrack* TAVTactBaseTrack::NearestTrack(TAVTbaseCluster *aCluster) {
//   
//   // For a given track, return the index of the nearest hit of a given plane
//   Double_t minDist = 1.e9;
//   Double_t distance;
//   TAVTtrack *tTrack;
//   
//   TAVTbaseTrack *tNearest = 0x0;
//   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
//
//   for( Int_t it = 0; it < pNtuTrack->GetTracksN(); ++it) { // loop on tracks
//	  tTrack = pNtuTrack->GetTrack(it);
//	  distance = aCluster->Distance( tTrack);
//	  if( distance < minDist) {
//		 minDist = distance;
//		 tNearest = tTrack;
//	  }
//   } // end loop on tracks
//   
//   return tNearest;
//}

//______________________________________________________________________________
//  
void TAVTactBaseTrack::SetGeoTrafo(TString name)
{
   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(name.Data());
   if (!fpFootGeo)
	  Error("SetGeoTrafo()", "No GeoTrafo action %s available yet\n", name.Data());
}
