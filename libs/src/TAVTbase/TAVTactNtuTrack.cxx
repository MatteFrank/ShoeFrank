/*!
 \file
 \version $Id: TAVTactNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAVTactNtuTrack.
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
#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactNtuTrack.hxx"

/*!
 \class TAVTactNtuTrack 
 \brief NTuplizer for vertex tracks. **
 */

ClassImp(TAVTactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactNtuTrack::TAVTactNtuTrack(const char* name, 
								 TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGdataDsc* pBMntuTrack)
: TAVTactBaseNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib, pBMntuTrack)
{
   TString device(name);
   if (device.Contains("vt")) {
      AddDataIn(pNtuClus,   "TAVTntuCluster");
      AddDataOut(pNtuTrack, "TAVTntuTrack");
   }
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuTrack::~TAVTactNtuTrack()
{

}

//_____________________________________________________________________________
//  
Bool_t TAVTactNtuTrack::FindTiltedTracks()
{
   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   
   TVector3 lineOrigin;    // origin in the tracker system
   TVector3 lineSlope;     // slope along z-axis in tracker system
   TVector3 pos ;
   
   // init
   lineOrigin.SetXYZ(0.,0.,0.);
   lineSlope.SetXYZ(0.,0.,1.);
   
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
	  Int_t nClusters = GetClustersN(curPlane);
	  if ( nClusters == 0) continue;
	  
	  // Loop on all clusters of the last plane
	  for( Int_t iLastClus = 0; iLastClus < nClusters; ++iLastClus) { // loop on cluster of last plane, 
		 
//       if( pNtuTrack->GetTracksN() >= pConfig->GetAnalysisPar().TracksMaximum ) break; // if max track number reach, stop
		 
		 TAGcluster* cluster = GetCluster(curPlane, iLastClus);
		 if (cluster->Found()) continue;
		 TAGbaseTrack*  track  = NewTrack();
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
        
        if (fPrefix.Contains("vt"))
           lineOrigin = fpFootGeo->FromGlobalToVTLocal(lineOrigin); // back to vertex frame
        else if (fPrefix.Contains("it"))
           lineOrigin = fpFootGeo->FromGlobalToITLocal(lineOrigin); // back to IT frame
        else if (fPrefix.Contains("ms"))
           lineOrigin = fpFootGeo->FromGlobalToMSDLocal(lineOrigin); // back to MSD frame
        else
           printf("Wrong prefix !");
        

		 lineSlope  = cluster->GetPositionG() - lineOrigin;
		 lineSlope *= 1/(lineSlope)(2);
      
		 track->SetLineValue(lineOrigin, lineSlope);
		 
		 // Loop on all planes to find a matching cluster in them
		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on planes
			Int_t nClusters1 = GetClustersN(iPlane);
			if (nClusters1 == 0) continue; //empty planes
			
			// loop on all clusters of this plane and keep the nearest one
			minDistance = fSearchClusDistance*(1 + 3.*TMath::Tan(track->GetTheta()*TMath::DegToRad()));
			TAGcluster* bestCluster = 0x0;
			
			for( Int_t iClus = 0; iClus < nClusters1; ++iClus ) { // loop on plane clusters
			   TAGcluster* aCluster = GetCluster(iPlane, iClus );
			   
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
			track->SetTrackIdx(GetTracksN());
			track->MakeChiSquare();
			track->SetType(1);
			AddNewTrack(track);
			if (fBmTrack && fBmTrackOk) {
			   SetBeamPosition(fBmTrackPos);
			} else {
			   TVector3 orig(0,0,0);
			   SetBeamPosition(orig);
			}
			
			if (ValidHistogram()) 
			   FillHistogramm(track);
				  
			delete track;
			
		 } else { // reset clusters
			for (Int_t i = 0; i < array.GetEntries(); ++i) {
			   TAGcluster*  cluster1 = (TAGcluster*)array.At(i);
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

