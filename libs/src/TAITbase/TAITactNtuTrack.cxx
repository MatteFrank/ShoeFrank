/*!
 \file
 \version $Id: TAITactNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAITactNtuTrack.
 */
#include "TClonesArray.h"
#include "TMath.h"


//TAGroot
#include "TAGroot.hxx"

// Foot trafo
#include "TAGgeoTrafo.hxx"

//BM
#include "TABMntuTrack.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAITtrack.hxx"
#include "TAITntuTrack.hxx"
#include "TAITntuCluster.hxx"
#include "TAITactNtuTrack.hxx"

/*!
 \class TAITactNtuTrack
 \brief NTuplizer for Inner tracker tracks. **
 */

//------------------------------------------+-----------------------------------
//! Default constructor.
TAITactNtuTrack::TAITactNtuTrack(const char* name,
								 TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGdataDsc* pBMntuTrack)
: TAITactBaseNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib, pBMntuTrack)
{
   AddDataIn(pNtuClus,   "TAITntuCluster");
   AddDataOut(pNtuTrack, "TAITntuTrack");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactNtuTrack::~TAITactNtuTrack()
{

}

//_____________________________________________________________________________
//  
Bool_t TAITactNtuTrack::FindTiltedTracks()
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

   // loop over last planes
   Int_t nPlane = pGeoMap->GetSensorsN()-1;
   Int_t curPlane = nPlane;
   
   while (curPlane >= fRequiredClusters-1) {
	  // Get the last reference plane
	  curPlane = nPlane--;
	  TClonesArray* list = pNtuClus->GetListOfClusters(curPlane);
	  Int_t nClusters = pNtuClus->GetClustersN(curPlane);
	  if ( nClusters == 0) continue;
	  
	  // Loop on all clusters of the last plane
	  for( Int_t iLastClus = 0; iLastClus < nClusters; ++iLastClus) { // loop on cluster of last plane, 
		 
		 if( pNtuTrack->GetTracksN() >= pConfig->GetAnalysisPar().TracksMaximum ) break; // if max track number reach, stop
		 
		 TAITcluster* cluster = (TAITcluster*)list->At( iLastClus );
		 if (cluster->Found()) continue;
		 TAITtrack*   track   = new TAITtrack();
		 array.Clear();
		 track->AddCluster(cluster);
		 array.Add(cluster);
		 
       lineOrigin[2] = 0;

		 if (fpBMntuTrack == 0x0 || fpFootGeo == 0x0) { // no BM info available
          lineOrigin.SetXYZ(0, 0, 0);
		 } else {
			if (fBmTrack) {
            if (fBmTrackOk) {
               lineOrigin.SetXYZ(fBmTrackPos.X(), fBmTrackPos.Y(), 0);
               lineOrigin = fpFootGeo->FromBMLocalToGlobal(lineOrigin);// go to FOOT global
            } else
				  lineOrigin.SetXYZ(0, 0, 0); 
			} else {
			   return true;  // think about it !
			}
		 }
        
       lineOrigin = fpFootGeo->FromGlobalToVTLocal(lineOrigin); // back to vertex frame

		 lineSlope  = cluster->GetPositionG() - lineOrigin;
		 lineSlope *= 1/(lineSlope)(2);
      
		 track->SetLineValue(lineOrigin, lineSlope);
		 
		 // Loop on all planes to find a matching cluster in them
		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on planes
			TClonesArray* list1 = pNtuClus->GetListOfClusters(iPlane);
			Int_t nClusters1 = pNtuClus->GetClustersN(iPlane);
			if (nClusters1 == 0) continue; //empty planes
			
			// loop on all clusters of this plane and keep the nearest one
			minDistance = fSearchClusDistance*(1 + 3.*TMath::Tan(track->GetTheta()*TMath::DegToRad()));
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
			track->SetType(1);
			pNtuTrack->NewTrack(*track);
			if (fBmTrack && fBmTrackOk) {
			   pNtuTrack->SetBeamPosition(fBmTrackPos);
			} else {
			   TVector3 orig(0,0,0);
			   pNtuTrack->SetBeamPosition(orig);
			}
			
			if (ValidHistogram()) 
			   FillHistogramm(track);
				  
			delete track;
			
		 } else { // reset clusters
			for (Int_t i = 0; i < array.GetEntries(); ++i) {
			   TAITcluster*  cluster1 = (TAITcluster*)array.At(i);
			   cluster1->SetFound(false);
			}
			array.Clear();
			delete track;
		 }
		 
	  } // end loop on first clusters
	  
   } // while
   
   if (ValidHistogram()) 
	  if (fBmTrackOk) 
		 FillBmHistogramm(fBmTrackPos);
   
   return true;
}



