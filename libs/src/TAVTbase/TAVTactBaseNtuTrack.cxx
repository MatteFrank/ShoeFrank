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

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactBaseNtuTrack::TAVTactBaseNtuTrack(const char* name, 
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
TAVTactBaseNtuTrack::~TAVTactBaseNtuTrack()
{   
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseNtuTrack::CreateHistogram()
{   
   TAVTactBaseTrack::CreateHistogram();
   
   TAVTbaseParGeo* pGeoMap  = (TAVTbaseParGeo*) fpGeoMap->Object();

   // TG
   fpHiVtxTgResX = new TH1F(Form("%sTgResX", fPrefix.Data()), Form("%s - Resolution at target in X", fTitleDev.Data()), 500, -0.02, 0.02);
   AddHistogram(fpHiVtxTgResX);
   
   fpHiVtxTgResY = new TH1F(Form("%sTgResY", fPrefix.Data()), Form("%s - Resolution at target in Y", fTitleDev.Data()), 500, -0.02, 0.02);
   AddHistogram(fpHiVtxTgResY);

   // BM
   fpHisBmBeamProf = new TH2F(Form("%sbmBeamProf", fPrefix.Data()), Form("%s - BM Beam Profile", fTitleDev.Data()),
							  100, -pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., 
							  100, -pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2.);
   fpHisBmBeamProf->SetStats(kFALSE);
   AddHistogram(fpHisBmBeamProf);
   
   fpHisVtxResX = new TH1F(Form("%sBmResX", fPrefix.Data()), Form("%s - Vertex position resisdualX BM/VT", fTitleDev.Data()), 500, -5, 5);
   AddHistogram(fpHisVtxResX);
   
   fpHisVtxResY = new TH1F(Form("%sBmResY", fPrefix.Data()), Form("%s - Vertex position resisdualY BM/VT", fTitleDev.Data()), 500, -5, 5);
   AddHistogram(fpHisVtxResY);
   
   fpHisBmChi2 = new TH1F(Form("%sbmChi2", fPrefix.Data()), Form("%s - BM Chi2 of tracks", fTitleDev.Data()), 200, 0, 1000);
   AddHistogram(fpHisBmChi2);
   
   
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
   
   TAVTntuCluster*  pNtuClus  = (TAVTntuCluster*)  fpNtuClus->Object();
   TAVTntuTrack*    pNtuTrack = (TAVTntuTrack*)    fpNtuTrack->Object();
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
TAVTbaseTrack* TAVTactBaseNtuTrack::GetTrack(Int_t idx)
{
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   TAVTbaseTrack* track  = pNtuTrack->GetTrack(idx);

   return track;
}

//_____________________________________________________________________________
//
Int_t TAVTactBaseNtuTrack::GetTracksN() const
{
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();

   return pNtuTrack->GetTracksN();
}

