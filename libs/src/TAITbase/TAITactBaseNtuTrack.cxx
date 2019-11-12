/*!
 \file
 \version $Id: TAITactBaseNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAITactBaseNtuTrack.
 */

#include "GlobalPar.hxx"

#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparCal.hxx"
#include "TAITtrack.hxx"
#include "TAITntuTrack.hxx"
#include "TAITntuCluster.hxx"

#include "TAITactBaseNtuTrack.hxx"

/*!
 \class TAITactBaseNtuTrack 
 \brief NTuplizer for Inner Tracker tracking **
 */

ClassImp(TAITactBaseNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAITactBaseNtuTrack::TAITactBaseNtuTrack(const char* name, 
										 TAGdataDsc* pNtuClus,TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig,  
										 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib)
: TAVTactBaseTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactBaseNtuTrack::~TAITactBaseNtuTrack()
{   
}

//_____________________________________________________________________________
//  
Bool_t TAITactBaseNtuTrack::Action()
{
   // IT
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

