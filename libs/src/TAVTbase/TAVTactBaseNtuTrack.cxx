/*!
 \file
 \version $Id: TAVTactBaseNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAVTactBaseNtuTrack.
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
#include "TAVTactBaseNtuTrack.hxx"

/*!
 \class TAVTactBaseNtuTrack 
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAVTactBaseNtuTrack);

Bool_t TAVTactBaseNtuTrack::fgRefit = true;
//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactBaseNtuTrack::TAVTactBaseNtuTrack(const char* name, 
										 TAGdataDsc* pNtuClus,TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig,  
										 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGdataDsc* pBMntuTrack)
: TAGaction(name, "TAVTactNtuTrack - Base NTuplize Tracker"),
  fpNtuTrack(pNtuTrack),
  fpNtuClus(pNtuClus),
  fpConfig(pConfig),
  fpGeoMap(pGeoMap),
  fpCalib(pCalib),
  fpBMntuTrack(pBMntuTrack),
  fTracksMaximum(100),
  fRequiredClusters(3),
  fSearchClusDistance(100),
  fGraphU(new TGraphErrors()),
  fGraphV(new TGraphErrors()),
  fBmTrackOk(false),
  fBmTrack(0x0),
  fBmTrackPos(0,0,0)
{
   AddDataIn(pNtuClus,   "TAVTntuCluster");
   AddDataOut(pNtuTrack, "TAVTntuTrack");
   AddPara(pGeoMap, "TAVTparGeo");
   AddPara(pConfig, "TAVTparConf");
   if (pCalib)
	  AddPara(pCalib, "TAVTparCal");
   
   TAVTparConf* config = (TAVTparConf*) fpConfig->Object();
   fRequiredClusters   = config->GetAnalysisPar().PlanesForTrackMinimum;
   fSearchClusDistance = config->GetAnalysisPar().SearchHitDistance;
   SetGeoTrafo(TAGgeoTrafo::GetDefaultActName().Data());

}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseNtuTrack::~TAVTactBaseNtuTrack()
{   
   delete fGraphU;
   delete fGraphV;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseNtuTrack::CreateHistogram()
{
   DeleteHistogram();   
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   TAVTparGeo* pGeoMap  = (TAVTparGeo*) fpGeoMap->Object();
   
   fpHisPixelTot = new TH1F("vtTrackedClusPixTot", "Total # pixels per tracked clusters", 100, -0.5, 99.5);
   AddHistogram(fpHisPixelTot);
   
   fpHiVtxTgResX = new TH1F("vtTgResX", "Resolution at target in X", 500, -0.02, 0.02);
   AddHistogram(fpHiVtxTgResX);
   
   fpHiVtxTgResY = new TH1F("vtTgResY", "Resolution at target in Y", 500, -0.02, 0.02);
   AddHistogram(fpHiVtxTgResY);
   
   for (Int_t i = 0; i < pConfig->GetSensorsN(); ++i) {
	  if (TAVTparConf::IsMapHistOn()) {
		 fpHisTrackMap[i] = new TH2F(Form("vtTrackMap%d", i+1), Form("Vertex - Tracks map for sensor %d", i+1), 
									 100, -pGeoMap->GetPitchY()*pGeoMap->GetNPixelY()/2., pGeoMap->GetPitchY()*pGeoMap->GetNPixelY()/2., 
									 100, -pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2.);
		 fpHisTrackMap[i]->SetMarkerStyle(24);
		 fpHisTrackMap[i]->SetMarkerSize(1.);
		 fpHisTrackMap[i]->SetMarkerColor(3);
		 fpHisTrackMap[i]->SetStats(kFALSE);
		 AddHistogram(fpHisTrackMap[i]);
	  }
	  
	  fpHisPixel[i] = new TH1F(Form("vtTrackedClusPix%d", i+1), Form("# pixels per tracked clusters of sensor %d", i+1), 100, -0.5, 99.5);
	  AddHistogram(fpHisPixel[i]);
	  
	  fpHisResX[i] = new TH1F(Form("vtResX%d", i+1), Form("ResidualX of sensor %d", i+1), 400, -0.01, 0.01);
	  AddHistogram(fpHisResX[i]);
	  fpHisResY[i] = new TH1F(Form("vtResY%d", i+1), Form("ResidualY of sensor %d", i+1), 400, -0.01, 0.01);
	  AddHistogram(fpHisResY[i]);
   }
   
   fpHisResTotX = new TH1F("vtResTotX", "Total ResidualX", 400, -0.01, 0.01);
   fpHisResTotY = new TH1F("vtResTotY", "Total ResidualY", 400, -0.01, 0.01);
   AddHistogram(fpHisResTotX);
   AddHistogram(fpHisResTotY);
   
   fpHisChi2TotX = new TH1F("vtChi2TotX", "Total Chi2 X", 200, 0, 20);
   fpHisChi2TotY = new TH1F("vtChi2TotY", "Total Chi2 Y", 200, 0, 20);
   AddHistogram(fpHisChi2TotX);
   AddHistogram(fpHisChi2TotY);
   
   fpHisTrackEvt = new TH1F("vtTrackEvt", "Number of tracks per event", 20, -0.5, 19.5);
   AddHistogram(fpHisTrackEvt);
   
   fpHisTrackClus = new TH1F("vtTrackClus", "Number of clusters per track", 9, -0.5, 8.5);
   AddHistogram(fpHisTrackClus);
   
   fpHisClusSensor = new TH1F("vtClusSensor", "Number of tracked clusters per sensor", 9, -0.5, 8.5);
   AddHistogram(fpHisClusSensor);
   
   fpHisTheta = new TH1F("vtTrackTheta", "Track polar distribution", 500, 0, 90);
   AddHistogram(fpHisTheta);
   
   fpHisPhi = new TH1F("vtTrackPhi", "Track azimutal distribution (#theta > 5)", 500, -180, 180);
   AddHistogram(fpHisPhi);
   
   fpHisBeamProf = new TH2F("vtBeamProf", "Vertex Beam Profile", 
							100, -pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., 
							100, -pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2.);
   fpHisBeamProf->SetStats(kFALSE);
   AddHistogram(fpHisBeamProf);
   
   fpHisBmBeamProf = new TH2F("bmBeamProf", "BM Beam Profile", 
							  100, -pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., 
							  100, -pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2.);
   fpHisBmBeamProf->SetStats(kFALSE);
   AddHistogram(fpHisBmBeamProf);
   
   fpHisVtxResX = new TH1F("vtBmResX", "Vertex position resisdualX BM/VT",  500, -0.5, 0.5);
   AddHistogram(fpHisVtxResX);
   
   fpHisVtxResY = new TH1F("vtBmResY", "Vertex position resisdualY BM/VT",  500, -0.5, 0.5);
   AddHistogram(fpHisVtxResY);
   
   fpHisBmChi2 = new TH1F("bmChi2", "BM chi2 of tracks", 200, 0, 1000);
   AddHistogram(fpHisBmChi2);
   
   fpHisClusLeft = new TH1F("vtClusLeft", "Clusters left per sensor", 8, 1, 8);
   AddHistogram(fpHisClusLeft);
   
   fpHisClusLeftPix = new TH2F("vtClusLeftPix", "Number of pixels for left clusters vs sensor", 
							   10, 0, 9, 50, 1, 50);
   fpHisClusLeftPix->SetStats(kFALSE);
   AddHistogram(fpHisClusLeftPix);
   
   SetValidHistogram(kTRUE);
   return;
}

//_____________________________________________________________________________
//  
Bool_t TAVTactBaseNtuTrack::Action()
{
   
   // BM tracks
   if (fpBMntuTrack) 
	  CheckBM();
   
   
   // VTX
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   pNtuTrack->Clear();
   
   // looking straight
   FindStraightTracks();
   
   // looking inclined line
   if (!FindTiltedTracks()){
	  if (ValidHistogram())
		 FillHistogramm();
	  fpNtuTrack->SetBit(kValid);
	  return true;
   }
   
   if (FindVertices())
	  FindTiltedTracks();
   
   if(FootDebugLevel(1)) {
	  printf(" %d tracks found\n", pNtuTrack->GetTracksN());
	  for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i) {
		 TAVTtrack* track = pNtuTrack->GetTrack(i);
		 printf("   with # clusters %d\n", track->GetClustersN());
	  }
   }
   

   if (ValidHistogram())
	  FillHistogramm();
   
   // Set charge probability
   SetChargeProba();
   
   fpNtuTrack->SetBit(kValid);
   return true;
}


//_____________________________________________________________________________
//  
void TAVTactBaseNtuTrack::CheckBM()
{   
   // BM info
   TAVTparConf*  pConfig  = (TAVTparConf*) fpConfig->Object();
   
   Float_t zDiff = 0;
   
   if (fpFootGeo) {
	  TVector3 bmPos = fpFootGeo->GetBMCenter();
	  TVector3 vtPos = fpFootGeo->GetVTCenter();
	  zDiff  = vtPos.Z() - bmPos.Z();	  
   }
   
   TABMntuTrack* pBMtrack = 0x0;
   fBmTrackOk    = false;
   pBMtrack = (TABMntuTrack*) fpBMntuTrack->Object();
   if (pBMtrack->GetTracksN() > 0)
	  fBmTrack = pBMtrack->Track(0);
   
   if (fBmTrack) {
	  fBmTrackPos  = fBmTrack->PointAtLocalZ(zDiff);
	  Float_t chi2 = fBmTrack->GetChi2Red();
	  if (ValidHistogram())
		 fpHisBmChi2->Fill(chi2);
	  
	//  if (chi2 < pConfig->GetAnalysisPar().BmTrackChi2Limit && chi2 > 0) // for the moment
		 fBmTrackOk = true;
   }   
}

//_____________________________________________________________________________
//  
void TAVTactBaseNtuTrack::SetChargeProba()
{
   if (fpCalib == 0x0) return;
   TAVTntuTrack* ntuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   TAVTparCal*    pCalib  = (TAVTparCal*) fpCalib->Object();
   
   Int_t nTrack = ntuTrack->GetTracksN();
   if (nTrack == 0) return;
   
   for (Int_t iTrack = 0; iTrack < nTrack; ++iTrack) {
	  
	  Int_t nPixelsTot = 0;
	  Float_t nPixelsMean;
	  TAVTtrack* track = ntuTrack->GetTrack(iTrack);
	  Int_t nClus = track->GetClustersN();
	  
	  for (Int_t i=0;i<nClus;i++) {
		 TAVTcluster* clus = track->GetCluster(i);
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
Bool_t TAVTactBaseNtuTrack::FindStraightTracks()
{
   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   
   TVector3 lineOrigin;    // origin in the tracker system
   TVector3 lineSlope;     // slope along z-axis in tracker system
   TVector3 pos ;
   
   // init
   lineOrigin.SetXYZ(0.,0.,0.);
   lineSlope.SetXYZ(0.,0.,1.);
   
   TAVTntuCluster* pNtuClus  = (TAVTntuCluster*) fpNtuClus->Object();
   TAVTntuTrack*   pNtuTrack = (TAVTntuTrack*)   fpNtuTrack->Object();
   TAVTparGeo*     pGeoMap   = (TAVTparGeo*)     fpGeoMap->Object();
   TAVTparConf*    pConfig   = (TAVTparConf*)    fpConfig->Object();
   
   TList array;
   array.SetOwner(false);
   array.Clear();
   
   Int_t nPlane = pGeoMap->GetNSensors()-1;
   Int_t curPlane = nPlane;
   
   while (curPlane >= fRequiredClusters-1) {
	  // Get the last reference plane
	  curPlane = nPlane--;
	  TClonesArray* list = pNtuClus->GetListOfClusters(curPlane);
	  Int_t nClusters = pNtuClus->GetClustersN(curPlane);
	  if ( nClusters == 0) continue;
	  
	  // Loop on all clusters of the first plane
	  // and try to make a track out of each one
	  for( Int_t iLastClus = 0; iLastClus < nClusters; ++iLastClus) { // loop on cluster of last plane, 
		 
		 if( pNtuTrack->GetTracksN() >= pConfig->GetAnalysisPar().TracksMaximum ) break; // if max track number reach, stop
		 
		 TAVTcluster* cluster = (TAVTcluster*)list->At( iLastClus );
		 if (cluster->Found()) continue;
		 TAVTtrack*   track   = new TAVTtrack();
		 array.Clear();
		 track->AddCluster(cluster);
		 array.Add(cluster);
		 
		 lineOrigin.SetXYZ(cluster->GetPositionU(), cluster->GetPositionV(), 0); // parallel lines
		 lineOrigin = pGeoMap->Sensor2Detector(curPlane, lineOrigin);
		 lineSlope.SetXYZ(0, 0, 1);
		 
		 track->SetLineValue(lineOrigin, lineSlope);
		 
		 // Loop on all planes to find a matching cluster in them
		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on planes
			TClonesArray* list1 = pNtuClus->GetListOfClusters(iPlane);
			Int_t nClusters1 = pNtuClus->GetClustersN(iPlane);
			if (nClusters1 == 0) continue; //empty planes
			
			// loop on all clusters of this plane and keep the nearest one
			minDistance = fSearchClusDistance;
			TAVTcluster* bestCluster = 0x0;
			
			for( Int_t iClus = 0; iClus < nClusters1; ++iClus ) { // loop on plane clusters
			   TAVTcluster* aCluster = (TAVTcluster*)list1->At( iClus );
			   
			   if( aCluster->Found()) continue; // skip cluster already found
			   
			   aDistance = aCluster->Distance(track);
			   
			   if( aDistance < minDistance ) {
				  minDistance = aDistance;
				  bestCluster = aCluster;
			   }
			} // end loop on plane clusters
			
			// if a cluster has been found, add the cluster
			if( bestCluster ) {	
			   bestCluster->SetFound();
			   cluster->SetFound();
			   track->AddCluster(bestCluster);
			   array.Add(bestCluster);
			   if (fgRefit)
				  UpdateParam(track);
			}
			
		 } // end loop on planes
		 
		 // Apply cuts
		 if (AppyCuts(track)) {
			track->SetNumber(pNtuTrack->GetTracksN());
			track->MakeChiSquare();
			track->SetType(0);
         pNtuTrack->NewTrack(*track);
			TVector3 orig(0,0,0);
			pNtuTrack->SetBeamPosition(orig);
			
			if (ValidHistogram()) {
			   FillHistogramm(track);
			}
			
			delete track;
		 } else { // reset clusters
			for (Int_t i = 0; i < array.GetEntries(); ++i) {
			   TAVTcluster*  cluster1 = (TAVTcluster*)array.At(i);
			   cluster1->SetFound(false);
			}
			delete track;
			array.Clear();
		 }
		 
	  } // end loop on first clusters
	  
   } // while
   
   
   return true;
}

//_____________________________________________________________________________
//  
Bool_t TAVTactBaseNtuTrack::AppyCuts(TAVTtrack* track)
{
   Bool_t valid = false;  
   
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   if (track->GetClustersN() >= pConfig->GetAnalysisPar().PlanesForTrackMinimum )
  	  valid = true;
   
   return valid;
}

//_____________________________________________________________________________
//  
void TAVTactBaseNtuTrack::UpdateParam(TAVTtrack* track)
{
   TVector3           lineOrigin;  // origin in the tracker system
   TVector3           lineSlope;   // slope along z-axis in tracker system
   
   // init
   lineOrigin.SetXYZ(0.,0.,0.);
   lineSlope.SetXYZ(0.,0.,1.);
   
   TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
   
   Int_t nClusters = track->GetClustersN();
   
   if( nClusters == 2) {
	  TAVTcluster* cluster0 = track->GetCluster(0);
	  TAVTcluster* cluster1 = track->GetCluster(1);
	  
     if(FootDebugLevel(1))
		 printf("TAVTactNtuTrack::Analyse track with %d cluster taking origin from it and slope 0\n", track->GetClustersN());
	  Double_t x0 = cluster0->GetPositionG()[0];
	  Double_t y0 = cluster0->GetPositionG()[1];
	  
	  Double_t x1 = cluster1->GetPositionG()[0];
	  Double_t y1 = cluster1->GetPositionG()[1];
	  
	  Double_t z0 = pGeoMap->GetSensorPosition(cluster0->GetPlaneNumber()).Z();
	  Double_t z1 = pGeoMap->GetSensorPosition(cluster1->GetPlaneNumber()).Z();
	  
     if(FootDebugLevel(1))
		 printf( "TAVTactNtuTrack::Analyze cluster[pl %d]=(%.2f, %.2f, %.2f) cluster[pl %d]=(%.2f, %.2f, %.2f)\n",
				cluster0->GetPlaneNumber(), x0, y0, z0, 
				cluster1->GetPlaneNumber(), x1, y1, z1);
	  
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
		 TAVTcluster* cluster = track->GetCluster(i);
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
void TAVTactBaseNtuTrack::FillHistogramm(TAVTtrack* track)
{   
   fpHisTheta->Fill(track->GetTheta());
   fpHisPhi->Fill(track->GetPhi());
   
   TAVTparGeo* pGeoMap = (TAVTparGeo*)fpGeoMap->Object();
   
   fpHisTrackClus->Fill(track->GetClustersN());
   for (Int_t i = 0; i < track->GetClustersN(); ++i) {
	  TAVTcluster * cluster = track->GetCluster(i);
	  cluster->SetFound();
	  Int_t idx = cluster->GetPlaneNumber();
	  fpHisPixelTot->Fill(cluster->GetPixelsN());
	  fpHisPixel[idx]->Fill(cluster->GetPixelsN());
	  
	  Float_t posZ       = cluster->GetPositionG()[2];
	  TVector3 impact    = track->Intersection(posZ);
   
     TVector3 impactLoc =  pGeoMap->Detector2Sensor(idx, impact);
      
	  if (TAVTparConf::IsMapHistOn()) 
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
}

//_____________________________________________________________________________
//  
void TAVTactBaseNtuTrack::FillHistogramm()
{
   TAVTparGeo*     pGeoMap   = (TAVTparGeo*)     fpGeoMap->Object();
   TAVTntuCluster* pNtuClus  = (TAVTntuCluster*) fpNtuClus->Object();
   TAVTntuTrack*   pNtuTrack = (TAVTntuTrack*)   fpNtuTrack->Object();
   
   fpHisTrackEvt->Fill(pNtuTrack->GetTracksN());
   if (pNtuTrack->GetTracksN() == 0)
	  fpHisClusSensor->Fill(0);
   
   for (Int_t iPlane = 0; iPlane < pGeoMap->GetNSensors(); ++iPlane) {
	  
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
	  static Float_t mean[8];
	  Int_t evtNumber = gTAGroot->CurrentEventNumber();
	  mean[iPlane] = evtNumber/(evtNumber+1.)*mean[iPlane] + 1./(evtNumber+1.)*(left/nClusters);
	  fpHisClusLeft->SetBinContent(iPlane+1, mean[iPlane]*100);
   }   
}

//_____________________________________________________________________________
//  
void TAVTactBaseNtuTrack::FillBmHistogramm(TVector3 bmTrackPos)
{
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   bmTrackPos  = fpFootGeo->FromBMLocalToGlobal(bmTrackPos);
   fpHisBmBeamProf->Fill(bmTrackPos.X(), bmTrackPos.Y());
   
   Float_t posZtg = fpFootGeo->FromTGLocalToGlobal(TVector3(0,0,0)).Z();
   posZtg = fpFootGeo->FromGlobalToVTLocal(TVector3(0, 0, posZtg)).Z();

   for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i) {
	  TAVTtrack* track  = pNtuTrack->GetTrack(i);
     TVector3   origin = track->Intersection(posZtg);
	  
	  origin  = fpFootGeo->FromVTLocalToGlobal(origin);
	  TVector3 res = origin - bmTrackPos;
	  fpHisVtxResX->Fill(res.X());
	  fpHisVtxResY->Fill(res.Y());
   }   
}

//_____________________________________________________________________________
//  
TAVTcluster* TAVTactBaseNtuTrack::NearestCluster(TAVTtrack *aTrack, Int_t iSensor)
{   
   // For a given track, return the index of the nearest hit of a given plane
   TAVTntuCluster* pNtuClus = (TAVTntuCluster*) fpNtuClus->Object();

   Double_t minDist = 1.e9;
   Double_t distance;
   TAVTcluster *tCluster;
   
   TAVTcluster *tNearest = 0x0;
   for (Int_t ht = 0; ht < pNtuClus->GetClustersN(iSensor); ++ht) { // loop on hits
	  tCluster = pNtuClus->GetCluster(iSensor, ht);
	  distance = tCluster->Distance( aTrack) ;
	  if( distance < minDist) {
		 minDist = distance;
		 tNearest = tCluster;
	  }
   } //end loop on hits
   
   return tNearest;
}

//_____________________________________________________________________________
//  
TAVTtrack* TAVTactBaseNtuTrack::NearestTrack(TAVTcluster *aCluster) {
   
   // For a given track, return the index of the nearest hit of a given plane
   Double_t minDist = 1.e9;
   Double_t distance;
   TAVTtrack *tTrack;
   
   TAVTtrack *tNearest = 0x0;
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();

   for( Int_t it = 0; it < pNtuTrack->GetTracksN(); ++it) { // loop on tracks
	  tTrack = pNtuTrack->GetTrack(it);
	  distance = aCluster->Distance( tTrack);
	  if( distance < minDist) {
		 minDist = distance;
		 tNearest = tTrack;
	  }
   } // end loop on tracks
   
   return tNearest;
}

//______________________________________________________________________________
//  
void TAVTactBaseNtuTrack::SetGeoTrafo(TString name)
{
   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(name.Data());
   if (!fpFootGeo)
	  printf("No GeoTrafo action %s available yet\n", name.Data());
   else 
	  printf("GeoTrafo action %s found\n", name.Data());
}
