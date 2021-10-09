/*!
 \file
 \version $Id: TAMSDactBaseNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMSDactBaseNtuTrack.
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

// Foot
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"

//TAG
#include "TAGroot.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparCal.hxx"
#include "TAMSDtrack.hxx"
#include "TAMSDntuTrack.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDactBaseNtuTrack.hxx"

/*!
 \class TAMSDactBaseNtuTrack
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAMSDactBaseNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMSDactBaseNtuTrack::TAMSDactBaseNtuTrack(const char* name,
										 TAGdataDsc* pNtuClus,TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig,  
										 TAGparaDsc* pGeoMap)
: TAVTactBaseTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactBaseNtuTrack::~TAMSDactBaseNtuTrack()
{   
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAMSDactBaseNtuTrack::CreateHistogram()
{
   TAVTactBaseTrack::CreateHistogram();
   
   TAVTbaseParGeo* pGeoMap  = (TAVTbaseParGeo*) fpGeoMap->Object();
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      fpHisStrip[i] = new TH1F(Form("%sTrackedPointStrip%d", fPrefix.Data(), i+1), Form("%s - # strips per tracked points of sensor %d", fTitleDev.Data(), i+1), 100, -0.5, 99.5);
      AddHistogram(fpHisStrip[i]);
   }
   
   fpHisStripTot = new TH1F(Form("%sTrackedPointStripTot", fPrefix.Data()), Form("%s - Total # strips per tracked points", fTitleDev.Data()), 100, -0.5, 99.5);
   AddHistogram(fpHisStripTot);
   
   
   fpHisResX = new TH1F(Form("%sResX", fPrefix.Data()), Form("%s -  position resisdualX BM/VT", fTitleDev.Data()), 500, -5, 5);
   AddHistogram(fpHisResX);
   
   fpHisResY = new TH1F(Form("%sResY", fPrefix.Data()), Form("%s -  position resisdualY BM/VT", fTitleDev.Data()), 500, -5, 5);
   AddHistogram(fpHisResY);
   
   
   fpHisPointLeft = new TH1F(Form("%sPointLeft", fPrefix.Data()), Form("%s -  point left per sensor", fTitleDev.Data()), 8, 1, 8);
   AddHistogram(fpHisPointLeft);
   
   SetValidHistogram(kTRUE);
   return;
}
//_____________________________________________________________________________
//  
Bool_t TAMSDactBaseNtuTrack::Action()
{   
   // VTX
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
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
		 TAMSDtrack* track = pNtuTrack->GetTrack(i);
		 printf("   with # clusters %d\n", track->GetClustersN());
	  }
   }
   

   if (ValidHistogram())
	  FillHistogramm();
   
   fpNtuTrack->SetBit(kValid);
   return true;
}

//_____________________________________________________________________________
//  
Bool_t TAMSDactBaseNtuTrack::FindStraightTracks()
{
   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   
   TVector3 lineOrigin;    // origin in the tracker system
   TVector3 lineSlope;     // slope along z-axis in tracker system
   TVector3 pos ;
   
   // init
   lineOrigin.SetXYZ(0.,0.,0.);
   lineSlope.SetXYZ(0.,0.,1.);
   
   TAMSDntuPoint*   pNtuClus  = (TAMSDntuPoint*)   fpNtuClus->Object();
   TAMSDntuTrack*   pNtuTrack = (TAMSDntuTrack*)   fpNtuTrack->Object();
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
	  TClonesArray* list = pNtuClus->GetListOfPoints(curPlane);
	  Int_t nClusters = pNtuClus->GetPointsN(curPlane);
	  if ( nClusters == 0) continue;
	  
	  // Loop on all clusters of the first plane
	  // and try to make a track out of each one
	  for( Int_t iLastClus = 0; iLastClus < nClusters; ++iLastClus) { // loop on cluster of last plane, 
		 		 
		 TAMSDpoint* cluster = (TAMSDpoint*)list->At( iLastClus );
		 if (cluster->Found()) continue;
		 TAMSDtrack*   track   = new TAMSDtrack();
		 array.Clear();
		 track->AddCluster(cluster);
		 array.Add(cluster);
		 
		 lineOrigin.SetXYZ(cluster->GetPosition()[0], cluster->GetPosition()[1], 0); // parallel lines
		 lineOrigin = pGeoMap->Sensor2Detector(curPlane, lineOrigin);
		 lineSlope.SetXYZ(0, 0, 1);
		 
		 track->SetLineValue(lineOrigin, lineSlope);
		 
		 // Loop on all planes to find a matching cluster in them
		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on planes
			TClonesArray* list1 = pNtuClus->GetListOfPoints(iPlane);
			Int_t nClusters1 = pNtuClus->GetPointsN(iPlane);
			if (nClusters1 == 0) continue; //empty planes
			
			// loop on all clusters of this plane and keep the nearest one
			minDistance = fSearchClusDistance;
			TAMSDpoint* bestCluster = 0x0;
			
			for( Int_t iClus = 0; iClus < nClusters1; ++iClus ) { // loop on plane clusters
			   TAMSDpoint* aCluster = (TAMSDpoint*)list1->At( iClus );
			   
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
			   TAMSDpoint*  cluster1 = (TAMSDpoint*)array.At(i);
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
void TAMSDactBaseNtuTrack::FillHistogramm(TAVTbaseTrack* track)
{
   TAVTactBaseTrack::FillHistogramm(track);
   
   TAVTbaseParGeo* pGeoMap = (TAVTbaseParGeo*)fpGeoMap->Object();
   
   fpHisTrackClus->Fill(track->GetClustersN());
   for (Int_t i = 0; i < track->GetClustersN(); ++i) {
      TAMSDpoint* cluster = (TAMSDpoint*)track->GetCluster(i);
      Int_t idx = cluster->GetSensorIdx();
      fpHisStripTot->Fill(cluster->GetElementsN());
      fpHisStrip[idx]->Fill(cluster->GetElementsN());
   }
   
   fpHisChi2TotX->Fill(track->GetChi2U());
   fpHisChi2TotY->Fill(track->GetChi2V());
   
   fpHisMeanCharge->Fill(track->GetMeanCharge());
}

//_____________________________________________________________________________
//
void TAMSDactBaseNtuTrack::FillHistogramm()
{
   TAVTbaseParGeo* pGeoMap   = (TAVTbaseParGeo*) fpGeoMap->Object();
   TAMSDntuPoint*  pNtuClus  = (TAMSDntuPoint*)  fpNtuClus->Object();
   TAMSDntuTrack*  pNtuTrack = (TAMSDntuTrack*)  fpNtuTrack->Object();
   
   fpHisTrackEvt->Fill(pNtuTrack->GetTracksN());
   if (pNtuTrack->GetTracksN() == 0)
      fpHisClusSensor->Fill(0);
   
   for (Int_t iPlane = 0; iPlane < pGeoMap->GetSensorsN(); ++iPlane) {
      
      TClonesArray* list = pNtuClus->GetListOfPoints(iPlane);
      Int_t nClusters = pNtuClus->GetPointsN(iPlane);
      if ( nClusters == 0) continue;
      
      Int_t left = 0;
      for( Int_t iClus = 0; iClus < nClusters; ++iClus) {
         
         TAMSDpoint* cluster = (TAMSDpoint*)list->At(iClus);
         if (!cluster->Found()) {
            fpHisPointLeft->Fill(iPlane+1, cluster->GetElementsN());
            left++;
         }
      }
      static Float_t mean[36];
      Int_t evtNumber = gTAGroot->CurrentEventNumber();
      mean[iPlane] = evtNumber/(evtNumber+1.)*mean[iPlane] + 1./(evtNumber+1.)*(left/nClusters);
      fpHisPointLeft->SetBinContent(iPlane+1, mean[iPlane]*100);
   }
}

//_____________________________________________________________________________
//
TAVTbaseTrack* TAMSDactBaseNtuTrack::GetTrack(Int_t idx)
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();
   TAVTbaseTrack* track  = pNtuTrack->GetTrack(idx);

   return track;
}

//_____________________________________________________________________________
//
Int_t TAMSDactBaseNtuTrack::GetTracksN() const
{
   TAMSDntuTrack* pNtuTrack = (TAMSDntuTrack*) fpNtuTrack->Object();

   return pNtuTrack->GetTracksN();
}

