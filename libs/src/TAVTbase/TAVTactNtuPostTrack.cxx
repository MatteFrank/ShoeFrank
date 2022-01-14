/*!
 \file TAVTactNtuPostTrack.cxx
 \brief   Class for post tracking refiting including vertex
 */
#include "TClonesArray.h"
#include "TF1.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TVector3.h"
#include "TVector2.h"

//TAGroot
#include "TAGroot.hxx"

//BM
#include "TABMntuTrack.hxx"

//First
#include "TAGgeoTrafo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuVertex.hxx"

#include "TAVTactNtuPostTrack.hxx"

/*!
 \class TAVTactNtuPostTrack 
 \brief Class for post tracking refiting including vertex
 */

ClassImp(TAVTactNtuPostTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuClus cluster container descriptor
//! \param[out] pNtuTrack track container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pNtuVertex input vertex container descriptor
TAVTactNtuPostTrack::TAVTactNtuPostTrack(const char* name,
								 TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, 
								 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap, TAGdataDsc* pNtuVertex)
: TAVTactBaseNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap),
  fpNtuVertex(pNtuVertex)
{
  // AddDataIn(pNtuVertex, "TAVTntuVertex");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuPostTrack::~TAVTactNtuPostTrack()
{
 
}

//_____________________________________________________________________________
//! Action
Bool_t TAVTactNtuPostTrack::Action()
{
   // VTX
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   pNtuTrack->Clear();

   // looking inclined line
   FindTiltedTracks();
   
   fpNtuTrack->SetBit(kValid);
   
   return true;
}

//_____________________________________________________________________________
//! Find tilted tracks
Bool_t TAVTactNtuPostTrack::FindTiltedTracks()
{
   Double_t minDistance  = 1.e9;
   Double_t aDistance = 0;
   
   TAVTntuCluster* pNtuClus   = (TAVTntuCluster*) fpNtuClus->Object();
   TAVTntuTrack*   pNtuTrack  = (TAVTntuTrack*)   fpNtuTrack->Object();
   TAVTntuVertex*  pNtuVertex = (TAVTntuVertex*)  fpNtuVertex->Object();
   TAVTparConf*    pConfig    = (TAVTparConf*)    fpConfig->Object();
   TAVTparGeo*     pGeoMap    = (TAVTparGeo*)     fpGeoMap->Object();
   
   Int_t nVertice = pNtuVertex->GetVertexN();
   if (nVertice <= 0) return true;
   
   TAVTvertex* vertex = 0x0;
   Bool_t isVtxFound = false;
   for (Int_t i = 0; i < nVertice; ++i) {
	  vertex = pNtuVertex->GetVertex(i);
	  if (vertex->GetValidity() && vertex->GetTracksN() > 1) {
		 isVtxFound = true;
		 break;
	  }
   }
					 
   TList array;
   array.SetOwner(false);
   array.Clear();
   TVector3 posG;
   TAVTcluster* vtxClus = new TAVTcluster();
   
   if (isVtxFound) {
	  posG = vertex->GetPosition();
	  vtxClus->SetPositionG(&posG);
	  vtxClus->SetPlaneNumber(-1);
   } else {
	  return true;
   }

   // init
   TVector3 lineOrigin;    // origin in the tracker system
   TVector3 lineSlope;     // slope along z-axis in tracker system
   lineOrigin.SetXYZ(0.,0.,0.);
   lineSlope.SetXYZ(0.,0.,1.);
   
   // loop over last planes
   Int_t nPlane = pGeoMap->GetSensorsN();
   Int_t curPlane = -1;
   
   while (++curPlane < nPlane) {
	  // Get the last reference plane
	  TClonesArray* list = pNtuClus->GetListOfClusters(curPlane);
	  Int_t nClusters = pNtuClus->GetClustersN(curPlane);
	  if ( nClusters == 0) continue;

	  // Loop on all clusters of the last plane
	  for( Int_t iFirstClus = 0; iFirstClus < nClusters; ++iFirstClus) { // loop on cluster of last plane, 
		 
		 if( pNtuTrack->GetTracksN() >= pConfig->GetAnalysisPar().TracksMaximum ) break; // if max track number reach, stop
		 
		 TAVTcluster* cluster = (TAVTcluster*)list->At( iFirstClus );
		 if (cluster->GetFound()) continue;
		 TAVTtrack* track = new TAVTtrack();
		 array.Clear();
		 track->AddCluster(vtxClus);
		 track->AddCluster(cluster);
		 array.Add(cluster);
		 
		
		 lineOrigin = posG; //in microns for us 
		 lineSlope  = cluster->GetPositionG() - lineOrigin;
		 lineSlope *= 1/(lineSlope)(2);
		 
		 track->SetLineValue(lineOrigin, lineSlope);
		 
		 // Loop on all planes to find a matching cluster in them
		 for( Int_t iPlane = curPlane+1; iPlane < nPlane; ++iPlane) { // loop on planes
			TClonesArray* list1 = pNtuClus->GetListOfClusters(iPlane);
			Int_t nClusters1 = pNtuClus->GetClustersN(iPlane);
			if (nClusters1 == 0) continue; //empty planes
			
			// loop on all clusters of this plane and keep the nearest one
			minDistance = fSearchClusDistance*(1 + 2.*TMath::Tan(track->GetTrackLine().GetTheta()*TMath::DegToRad()));
			TAVTcluster* bestCluster = 0x0;
			
			for( Int_t iClus = 0; iClus < nClusters1; ++iClus ) { // loop on plane clusters
			   TAVTcluster* aCluster = (TAVTcluster*)list1->At(iClus);
			   
			   if( aCluster->GetFound()) continue; // skip cluster already found
			   
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
			track->SetVertexZ(posG.Z());
			vertex->AddTrack(track);
			pNtuTrack->NewTrack(*track);
			delete track;
			
		 } else { // reset clusters
			for (Int_t i = 0; i < array.GetEntries(); ++i) {
			   TAVTcluster*  cluster1 = (TAVTcluster*)array.At(i);
			   cluster1->SetFound(false);
			}
			array.Clear();
			delete track;
		 }
		 
	  } // end loop on first clusters
	  
   } // while

   delete vtxClus;
   
   return true;
}

//_____________________________________________________________________________
//! Apply cut on a given track
//!
//! \param[in] track a given track
Bool_t TAVTactNtuPostTrack::AppyCuts(TAVTtrack* track)
{
   if (!TAVTactBaseNtuTrack::AppyCuts(track))
	  return false;

   Float_t minDistance = fSearchClusDistance*(1 + 2.*TMath::Tan(track->GetTrackLine().GetTheta()*TMath::DegToRad()));
   
   TAVTcluster* aCluster = track->GetCluster(0);
   
   if (aCluster->Distance(track) < minDistance)
	  return true;
   
   return false;
}

