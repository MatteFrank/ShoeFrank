/*!
 \file
 \version $Id: TAMSDactNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMSDactNtuTrack.
 */
#include "TClonesArray.h"
#include "TMath.h"


//TAGroot
#include "TAGroot.hxx"

// Foot trafo
#include "TAGgeoTrafo.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDtrack.hxx"
#include "TAMSDntuTrack.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDactNtuTrack.hxx"

/*!
 \class TAMSDactNtuTrack
 \brief NTuplizer for Inner tracker tracks. **
 */

ClassImp(TAMSDactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMSDactNtuTrack::TAMSDactNtuTrack(const char* name,
								 TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								 TAGparaDsc* pGeoMap)
: TAMSDactBaseNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap)
{
   AddDataIn(pNtuClus,   "TAMSDntuPoint");
   AddDataOut(pNtuTrack, "TAMSDntuTrack");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactNtuTrack::~TAMSDactNtuTrack()
{

}

//_____________________________________________________________________________
//  
Bool_t TAMSDactNtuTrack::FindTiltedTracks()
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

   // loop over last planes
   Int_t nPlane = pGeoMap->GetSensorsN()-1;
   Int_t curPlane = nPlane;
   
   while (curPlane >= fRequiredClusters-1) {
	  // Get the last reference plane
	  curPlane = nPlane--;
	  TClonesArray* list = pNtuClus->GetListOfPoints(curPlane);
	  Int_t nClusters = pNtuClus->GetPointsN(curPlane);
	  if ( nClusters == 0) continue;
	  
	  // Loop on all clusters of the last plane
	  for( Int_t iLastClus = 0; iLastClus < nClusters; ++iLastClus) { // loop on cluster of last plane, 
		 
		 if( pNtuTrack->GetTracksN() >= pConfig->GetAnalysisPar().TracksMaximum ) break; // if max track number reach, stop
		 
		 TAMSDpoint* cluster = (TAMSDpoint*)list->At( iLastClus );
		 if (cluster->Found()) continue;
		 TAMSDtrack*   track   = new TAMSDtrack();
		 array.Clear();
		 track->AddCluster(cluster);
		 array.Add(cluster);
		 
        lineOrigin.SetXYZ(0, 0, 0);
        
       lineOrigin = fpFootGeo->FromGlobalToVTLocal(lineOrigin); // back to vertex frame

		 lineSlope  = cluster->GetPositionG() - lineOrigin;
		 lineSlope *= 1/(lineSlope)(2);
      
		 track->SetLineValue(lineOrigin, lineSlope);
		 
		 // Loop on all planes to find a matching cluster in them
		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on planes
			TClonesArray* list1 = pNtuClus->GetListOfPoints(iPlane);
			Int_t nClusters1 = pNtuClus->GetPointsN(iPlane);
			if (nClusters1 == 0) continue; //empty planes
			
			// loop on all clusters of this plane and keep the nearest one
			minDistance = fSearchClusDistance*(1 + 3.*TMath::Tan(track->GetTheta()*TMath::DegToRad()));
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
		 if (AppyCuts(track)) {
			track->SetTrackIdx(pNtuTrack->GetTracksN());
			track->MakeChiSquare();
			track->SetType(1);
			pNtuTrack->NewTrack(*track);
          TVector3 orig(0,0,0);
          pNtuTrack->SetBeamPosition(orig);
			
			
			if (ValidHistogram()) 
			   FillHistogramm(track);
				  
			delete track;
			
		 } else { // reset clusters
			for (Int_t i = 0; i < array.GetEntries(); ++i) {
			   TAMSDpoint*  cluster1 = (TAMSDpoint*)array.At(i);
			   cluster1->SetFound(false);
			}
			array.Clear();
			delete track;
		 }
		 
	  } // end loop on first clusters
	  
   } // while
      
   return true;
}



