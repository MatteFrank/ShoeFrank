/*!
 \file
 \version $Id: TAVTactNtuTrackF.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAVTactNtuTrackF.
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

#include "TAVTdatRaw.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparMap.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactNtuTrackF.hxx"

#include "foot_geo.h"

/*!
 \class TAVTactNtuTrackF 
 \brief NTuplizer for vertex tracks. **
 */

ClassImp(TAVTactNtuTrackF);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactNtuTrackF::TAVTactNtuTrackF(const char* name, 
								   TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								   TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGdataDsc* pBMntuTrack)
: TAVTactBaseNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib, pBMntuTrack)
{

}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuTrackF::~TAVTactNtuTrackF()
{
 
}

//_____________________________________________________________________________
//  
Bool_t TAVTactNtuTrackF::FindTiltedTracks()
{
   Double_t minDistance  = 1.e9;
   Double_t aDistance = 0;
   
   TAVTntuCluster* pNtuClus  = (TAVTntuCluster*) fpNtuClus->Object();
   TAVTntuTrack*   pNtuTrack = (TAVTntuTrack*)   fpNtuTrack->Object();
   TAVTparGeo*     pGeoMap   = (TAVTparGeo*)     fpGeoMap->Object();
   
   TList array;
   array.SetOwner(false);
   array.Clear();
   
   TAVTtrack* track = 0x0;
   Int_t nPlane   = pGeoMap->GetNSensors()-1;
   Int_t curPlane = nPlane;
   
   while (curPlane >= fRequiredClusters-1) {
	  
	  // Get the last plane
	  curPlane = nPlane--;
	  
	  TClonesArray* lastList = pNtuClus->GetListOfClusters(curPlane);
	  Int_t nLastClusters    = pNtuClus->GetClustersN(curPlane);
	  
	  if ( nLastClusters == 0) continue;

	  for( Int_t iLastClus = 0; iLastClus < nLastClusters; ++iLastClus) { // loop on cluster of last plane
		 TAVTcluster* lastCluster = (TAVTcluster*)lastList->At(iLastClus);
		 if (lastCluster->GetFound()) continue;
		 if (lastCluster->GetPixelsN() <= 1) continue;
		 lastCluster->SetFound();

		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on next planes
			 
			TClonesArray* nextList = pNtuClus->GetListOfClusters(iPlane);
			Int_t nNextClusters    = pNtuClus->GetClustersN(iPlane);
			
			for( Int_t iNextClus = 0; iNextClus < nNextClusters; ++iNextClus) { // loop on cluster of next plane
			   TAVTcluster* nextCluster = (TAVTcluster*)nextList->At(iNextClus);
			   if (nextCluster->GetFound()) continue;
			   if (nextCluster->GetPixelsN() <= 1) continue;
			   
			   track = new TAVTtrack();
			   track->AddCluster(lastCluster);
			   nextCluster->SetFound();
			   track->AddCluster(nextCluster);

			   UpdateParam(track);

			   if (!IsGoodCandidate(track)) {
				  nextCluster->SetFound(false);
				  delete track;
				  continue;
			   }

			   array.Clear();
			   for( Int_t iFirstPlane = iPlane-1; iFirstPlane >= 0; --iFirstPlane) { // loop on first planes
				  
				  if (track->GetClustersN() + iFirstPlane < fRequiredClusters-1)
					  break;
				   
				  TClonesArray* firstList = pNtuClus->GetListOfClusters(iFirstPlane);
				  Int_t nFirstClusters    = pNtuClus->GetClustersN(iFirstPlane);
				  
				  minDistance = fSearchClusDistance;
				  TAVTcluster* bestCluster = 0x0;
				  for( Int_t iFirstClus = 0; iFirstClus < nFirstClusters; ++iFirstClus) { // loop on cluster of first planes
					 TAVTcluster* firstCluster = (TAVTcluster*)firstList->At(iFirstClus);
					 if (firstCluster->GetPixelsN() <= 1) continue;
					 if( firstCluster->GetFound()) continue; // skip cluster already found
					 
					 aDistance = firstCluster->Distance(track);
					 if( aDistance < minDistance ) {
						minDistance = aDistance;
						bestCluster = firstCluster;
					 }
				  } // end loop on cluster of first planes
				  
				  // if a cluster has been found, add the cluster
				  if( bestCluster ) {	
					 bestCluster->SetFound();
					 track->AddCluster(bestCluster);
					 array.Add(bestCluster);
					 UpdateParam(track);
				  } 
				  
			   } // end loop on first planes

			   // apply cut
			   if (AppyCuts(track)) {
				  track->SetNumber(pNtuTrack->GetTracksN());
				  track->MakeChiSquare();
				  track->SetType(1);
				  pNtuTrack->NewTrack(*track);
				  
				  if (ValidHistogram()) 
					 FillHistogramm(track);
   
				  delete track;		 				  
			   } else { // reset clusters
				  nextCluster->SetFound(false);
				  for (Int_t i = 0; i < array.GetEntries(); ++i) {
					 TAVTcluster*  cluster1 = (TAVTcluster*)array.At(i);
					 cluster1->SetFound(false);
				  }
				  delete track;
				  array.Clear();
			   }
			   
			} // end cluster of next plane
		 } // end last-1 plane
	  } // end loop on last clusters
	  
   } // while
   
   return true;
}

//_____________________________________________________________________________
//  
Bool_t TAVTactNtuTrackF::IsGoodCandidate(TAVTtrack* track)
{
   Float_t width = TG_THICK/2.;
   TVector3 vec = track->Intersection(0.);
   if (TMath::Abs(vec.X()) > width || TMath::Abs(vec.Y()) > width)
	  return false;
   
   return true;
}


