/*!
 \file
 \version $Id: TAITactNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAITactNtuTrack.
 */
#include "TClonesArray.h"
#include "TMath.h"


//TAGroot
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

//VTX
#include "TAVTntuVertex.hxx"

#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"
#include "TAITntuTrack.hxx"
#include "TAITntuCluster.hxx"
#include "TAITactNtuTrack.hxx"

/*!
 \class TAITactNtuTrack 
 \brief NTuplizer for Inner tracker tracks. **
 */

ClassImp(TAITactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAITactNtuTrack::TAITactNtuTrack(const char* name, 
								 TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGdataDsc* pVtVertex)
 : TAITactBaseNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib),
   fpVtVertex(pVtVertex),
   fVtVertex(0x0),
   fVtVertexPos(),
   fVtVertexOk(false)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactNtuTrack::~TAITactNtuTrack()
{

}

//_____________________________________________________________________________
//
void TAITactNtuTrack::CheckVtx()
{
   // VTX info
   fVtVertexOk = false;
   TAVTntuVertex* pNtuVertex  = (TAVTntuVertex*) fpVtVertex->Object();
   
   Int_t nVertex = pNtuVertex->GetVertexN();
   
   for (Int_t i = 0; i < nVertex; ++i) {
      
      TAVTvertex* vtx = pNtuVertex->GetVertex(i);
      if (vtx->IsBmMatched()) {
         fVtVertexPos.SetXYZ(vtx->GetVertexPosition().X(), vtx->GetVertexPosition().Y(), vtx->GetVertexPosition().Z());
         TVector3 posVtG = fpFootGeo->FromVTLocalToGlobal(fVtVertexPos);
         fVtVertexPos    = fpFootGeo->FromGlobalToITLocal(posVtG);
         fVtVertexOk     = true;
      }
   }
}

//_____________________________________________________________________________
//  
Bool_t TAITactNtuTrack::FindTiltedTracks()
{
   CheckVtx();
   
   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   
   TVector3 lineOrigin;    // origin in the tracker system
   TVector3 lineSlope;     // slope along z-axis in tracker system
   TVector3 pos ;
   
   // init
   lineOrigin.SetXYZ(0.,0.,0.);
   lineSlope.SetXYZ(0.,0.,1.);
   
   TAITntuCluster* pNtuClus  = (TAITntuCluster*) fpNtuClus->Object();
   TAITntuTrack*   pNtuTrack = (TAITntuTrack*)   fpNtuTrack->Object();
   TAITparGeo*     pGeoMap   = (TAITparGeo*)     fpGeoMap->Object();
   TAITparConf*    pConfig   = (TAITparConf*)    fpConfig->Object();
   
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
		 
		 if (fpVtVertex == 0x0 || fpFootGeo == 0x0) { // no vertex info available
			lineOrigin.SetXYZ(0, 0, 0);  
		 } else {
			if (fpVtVertex) {
			   if (fVtVertexOk)
				  lineOrigin.SetXYZ(fVtVertexPos.X(), fVtVertexPos.Y(), fVtVertexPos.Z());
			   else 
				  lineOrigin.SetXYZ(0, 0, 0); 
			} else {
			   return true;  // think about it !
			}
		 }
		 lineOrigin[2] = 0;
		 lineSlope     = cluster->GetPositionG() - lineOrigin;
		 lineSlope    *= 1/(lineSlope)(2);
		 
		 track->SetLineValue(lineOrigin, lineSlope);
		 
		 // Loop on all planes to find a matching cluster in them
		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on planes
			TClonesArray* list1 = pNtuClus->GetListOfClusters(iPlane);
			Int_t nClusters1 = pNtuClus->GetClustersN(iPlane);
			if (nClusters1 == 0) continue; //empty planes
			
			// loop on all clusters of this plane and keep the nearest one
			minDistance = fSearchClusDistance*(1 + 2.*TMath::Tan(track->GetTheta()*TMath::DegToRad()));
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
      
   return true;
}



