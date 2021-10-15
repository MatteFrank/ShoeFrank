/*!
 \file
 \version $Id: TAITactBaseNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAITactBaseNtuTrack.
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
#include "TAITtrack.hxx"
#include "TAITntuTrack.hxx"
#include "TAITntuCluster.hxx"
#include "TAITactBaseNtuTrack.hxx"

/*!
 \class TAITactBaseNtuTrack
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAITactBaseNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAITactBaseNtuTrack::TAITactBaseNtuTrack(const char* name,
										 TAGdataDsc* pNtuClus,TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig,  
										 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGdataDsc* pBMntuTrack)
: TAVTactBaseTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib),
  fpBMntuTrack(pBMntuTrack),
  fBmTrackOk(false),
  fBmTrack(0x0),
  fBmTrackPos(0,0,0)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactBaseNtuTrack::~TAITactBaseNtuTrack()
{   
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAITactBaseNtuTrack::CreateHistogram()
{   
   TAVTactBaseTrack::CreateHistogram();
   
   TAVTbaseParGeo* pGeoMap  = (TAVTbaseParGeo*) fpGeoMap->Object();

   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      fpHisPixel[i] = new TH1F(Form("%sTrackedClusPix%d", fPrefix.Data(), i+1), Form("%s - # pixels per tracked clusters of sensor %d", fTitleDev.Data(), i+1), 100, -0.5, 99.5);
      AddHistogram(fpHisPixel[i]);
   }
   
   fpHisPixelTot = new TH1F(Form("%sTrackedClusPixTot", fPrefix.Data()), Form("%s - Total # pixels per tracked clusters", fTitleDev.Data()), 100, -0.5, 99.5);
   AddHistogram(fpHisPixelTot);
   
   fpHisClusLeftPix = new TH2F(Form("%sClusLeftPix", fPrefix.Data()), Form("%s - Number of pixels for left clusters vs sensor", fTitleDev.Data()),
                               10, 0, 9, 50, 1, 50);
   fpHisClusLeftPix->SetStats(kFALSE);
   AddHistogram(fpHisClusLeftPix);
   
   // TG
   fpHiVtxTgResX = new TH1F(Form("%sTgResX", fPrefix.Data()), Form("%s - Resolution at target in X", fTitleDev.Data()), 500, -0.02, 0.02);
   AddHistogram(fpHiVtxTgResX);
   
   fpHiVtxTgResY = new TH1F(Form("%sTgResY", fPrefix.Data()), Form("%s - Resolution at target in Y", fTitleDev.Data()), 500, -0.02, 0.02);
   AddHistogram(fpHiVtxTgResY);

   // BM
   fpHisBmBeamProf = new TH2F(Form("%sbmBeamProf", fPrefix.Data()), Form("%s - BM Beam Profile", fTitleDev.Data()),
							  100, -pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2., pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2., 
							  100, -pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2., pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2.);
   fpHisBmBeamProf->SetStats(kFALSE);
   AddHistogram(fpHisBmBeamProf);
   
   fpHisVtxResX = new TH1F(Form("%sBmResX", fPrefix.Data()), Form("%s - Vertex position resisdualX BM/VT", fTitleDev.Data()), 500, -5, 5);
   AddHistogram(fpHisVtxResX);
   
   fpHisVtxResY = new TH1F(Form("%sBmResY", fPrefix.Data()), Form("%s - Vertex position resisdualY BM/VT", fTitleDev.Data()), 500, -5, 5);
   AddHistogram(fpHisVtxResY);
   
   fpHisBmChi2 = new TH1F(Form("%sbmChi2", fPrefix.Data()), Form("%s - BM Chi2 of tracks", fTitleDev.Data()), 200, 0, 1000);
   AddHistogram(fpHisBmChi2);
   
   fpHisClusLeft = new TH1F(Form("%sClusLeft", fPrefix.Data()), Form("%s - Clusters left per sensor", fTitleDev.Data()), 8, 1, 8);
   AddHistogram(fpHisClusLeft);

   SetValidHistogram(kTRUE);
   return;
}

//_____________________________________________________________________________
//  
Bool_t TAITactBaseNtuTrack::Action()
{
   
   // BM tracks
   if (fpBMntuTrack) 
	  CheckBM();
   
   
   // VTX
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
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
		 TAITtrack* track = pNtuTrack->GetTrack(i);
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
void TAITactBaseNtuTrack::CheckBM()
{   
   // BM info
   TAVTbaseParConf*  pConfig  = (TAVTbaseParConf*) fpConfig->Object();
   
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
	  fBmTrack = pBMtrack->GetTrack(0);
   
   if (fBmTrack) {
	  fBmTrackPos  = fBmTrack->PointAtLocalZ(zDiff);
	  Float_t chi2 = fBmTrack->GetChiSquare();
	  if (ValidHistogram())
		 fpHisBmChi2->Fill(chi2);
	  
	//  if (chi2 < pConfig->GetAnalysisPar().BmTrackChi2Limit && chi2 > 0) // for the moment
		 fBmTrackOk = true;
   }   
}

//_____________________________________________________________________________
//  
Bool_t TAITactBaseNtuTrack::FindStraightTracks()
{
   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   
   TVector3 lineOrigin;    // origin in the tracker system
   TVector3 lineSlope;     // slope along z-axis in tracker system
   TVector3 pos ;
   
   // init
   lineOrigin.SetXYZ(0.,0.,0.);
   lineSlope.SetXYZ(0.,0.,1.);
   
   TAITntuCluster*  pNtuClus  = (TAITntuCluster*)  fpNtuClus->Object();
   TAITntuTrack*    pNtuTrack = (TAITntuTrack*)    fpNtuTrack->Object();
   TAVTbaseParGeo*  pGeoMap   = (TAVTbaseParGeo*)  fpGeoMap->Object();
   TAVTbaseParConf* pConfig   = (TAVTbaseParConf*) fpConfig->Object();
   
   TList array;
   array.SetOwner(false);
   array.Clear();
   
   Int_t nPlane = pGeoMap->GetSensorsN()-1;
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
		 
		 TAITcluster* cluster = (TAITcluster*)list->At( iLastClus );
		 if (cluster->Found()) continue;
		 TAITtrack*   track   = new TAITtrack();
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
			TAITcluster* bestCluster = 0x0;
			
			for( Int_t iClus = 0; iClus < nClusters1; ++iClus ) { // loop on plane clusters
			   TAITcluster* aCluster = (TAITcluster*)list1->At( iClus );
			   
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
		 if (AppyCuts(track) && IsGoodCandidate(track)) {
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
			   TAITcluster*  cluster1 = (TAITcluster*)array.At(i);
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
void TAITactBaseNtuTrack::SetChargeProba()
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
         TAITcluster* clus = (TAITcluster*)track->GetCluster(i);
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
void TAITactBaseNtuTrack::FillHistogramm(TAVTbaseTrack* track)
{
   TAVTactBaseTrack::FillHistogramm(track);
   
   TAVTbaseParGeo* pGeoMap = (TAVTbaseParGeo*)fpGeoMap->Object();
   
   fpHisTrackClus->Fill(track->GetClustersN());
   for (Int_t i = 0; i < track->GetClustersN(); ++i) {
      TAVTcluster* cluster = (TAVTcluster*)track->GetCluster(i);
      Int_t idx = cluster->GetSensorIdx();
      fpHisPixelTot->Fill(cluster->GetPixelsN());
      fpHisPixel[idx]->Fill(cluster->GetPixelsN());
   }
   
   fpHisChi2TotX->Fill(track->GetChi2U());
   fpHisChi2TotY->Fill(track->GetChi2V());
   
   fpHisMeanPixel->Fill(track->GetMeanPixelsN());
   fpHisMeanCharge->Fill(track->GetMeanCharge());
}

//_____________________________________________________________________________
//
void TAITactBaseNtuTrack::FillHistogramm()
{
   TAVTbaseParGeo* pGeoMap   = (TAVTbaseParGeo*) fpGeoMap->Object();
   TAVTntuCluster* pNtuClus  = (TAVTntuCluster*) fpNtuClus->Object();
   TAITntuTrack*   pNtuTrack = (TAITntuTrack*)   fpNtuTrack->Object();
   
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
void TAITactBaseNtuTrack::FillBmHistogramm(TVector3 bmTrackPos)
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   bmTrackPos  = fpFootGeo->FromBMLocalToGlobal(bmTrackPos);
   fpHisBmBeamProf->Fill(bmTrackPos.X(), bmTrackPos.Y());
   
   Float_t posZtg = fpFootGeo->FromTGLocalToGlobal(TVector3(0,0,0)).Z();
   posZtg = fpFootGeo->FromGlobalToVTLocal(TVector3(0, 0, posZtg)).Z();

   for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i) {
	  TAITtrack* track  = pNtuTrack->GetTrack(i);
     TVector3   origin = track->Intersection(posZtg);
	  
	  origin  = fpFootGeo->FromVTLocalToGlobal(origin);
	  TVector3 res = origin - bmTrackPos;
	  fpHisVtxResX->Fill(res.X());
	  fpHisVtxResY->Fill(res.Y());
   }   
}


//_____________________________________________________________________________
//
TAVTbaseTrack* TAITactBaseNtuTrack::GetTrack(Int_t idx)
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();
   TAVTbaseTrack* track  = pNtuTrack->GetTrack(idx);

   return track;
}

//_____________________________________________________________________________
//
Int_t TAITactBaseNtuTrack::GetTracksN() const
{
   TAITntuTrack* pNtuTrack = (TAITntuTrack*) fpNtuTrack->Object();

   return pNtuTrack->GetTracksN();
}

