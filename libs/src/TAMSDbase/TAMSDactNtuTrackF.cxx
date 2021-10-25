/*!
 \file
 \version $Id: TAMSDactNtuTrackF.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMSDactNtuTrackF.
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

//First
#include "TAGgeoTrafo.hxx"

#include "TAGparGeo.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDparMap.hxx"
#include "TAMSDntuTrack.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDactNtuTrackF.hxx"

/*!
 \class TAMSDactNtuTrackF
 \brief NTuplizer for vertex tracks. **
 */

ClassImp(TAMSDactNtuTrackF);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMSDactNtuTrackF::TAMSDactNtuTrackF(const char* name,
								   TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								   TAGparaDsc* pGeoMap, TAGparaDsc* p_geo_g)
: TAMSDactBaseNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap),
  fpGeoMapG(p_geo_g)
{
   AddDataIn(pNtuClus,   "TAMSDntuPoint");
   AddDataOut(pNtuTrack, "TAMSDntuTrack");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactNtuTrackF::~TAMSDactNtuTrackF()
{
 
}

//_____________________________________________________________________________
//  
Bool_t TAMSDactNtuTrackF::FindTiltedTracks()
{
   Double_t minDistance  = 1.e9;
   Double_t aDistance = 0;
   
   TAMSDntuPoint* pNtuClus  = (TAMSDntuPoint*) fpNtuClus->Object();
   TAMSDntuTrack*   pNtuTrack = (TAMSDntuTrack*)   fpNtuTrack->Object();
   TAMSDparGeo*     pGeoMap   = (TAMSDparGeo*)     fpGeoMap->Object();
   
   TList array;
   array.SetOwner(false);
   array.Clear();
   
   TAMSDtrack* track = 0x0;
   Int_t nPlane   = pGeoMap->GetSensorsN()-1;
   Int_t curPlane = nPlane;
   
   while (curPlane >= fRequiredClusters-1) {
	  
	  // Get the last plane
	  curPlane = nPlane--;
	  
	  TClonesArray* lastList = pNtuClus->GetListOfPoints(curPlane);
	  Int_t nLastClusters    = pNtuClus->GetPointsN(curPlane);
	  
	  if ( nLastClusters == 0) continue;

	  for( Int_t iLastClus = 0; iLastClus < nLastClusters; ++iLastClus) { // loop on cluster of last plane
		 TAMSDpoint* lastCluster = (TAMSDpoint*)lastList->At(iLastClus);
		 if (lastCluster->Found()) continue;
		 if (lastCluster->GetElementsN() <= 2) continue;
		 lastCluster->SetFound();

		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on next planes
			 
			TClonesArray* nextList = pNtuClus->GetListOfPoints(iPlane);
			Int_t nNextClusters    = pNtuClus->GetPointsN(iPlane);
			
			for( Int_t iNextClus = 0; iNextClus < nNextClusters; ++iNextClus) { // loop on cluster of next plane
			   TAMSDpoint* nextCluster = (TAMSDpoint*)nextList->At(iNextClus);
			   if (nextCluster->Found()) continue;
			   if (nextCluster->GetElementsN() <= 2) continue;
			   
			   track = new TAMSDtrack();
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
				  
//				  if (track->GetClustersN() + iFirstPlane < fRequiredClusters-1)
//					  break;
				   
				  TClonesArray* firstList = pNtuClus->GetListOfPoints(iFirstPlane);
				  Int_t nFirstClusters    = pNtuClus->GetPointsN(iFirstPlane);
				  
				  minDistance = fSearchClusDistance;
				  TAMSDpoint* bestCluster = 0x0;
				  for( Int_t iFirstClus = 0; iFirstClus < nFirstClusters; ++iFirstClus) { // loop on cluster of first planes
					 TAMSDpoint* firstCluster = (TAMSDpoint*)firstList->At(iFirstClus);
					 if (firstCluster->GetElementsN() <= 1) continue;
					 if( firstCluster->Found()) continue; // skip cluster already found
					 
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
				  track->SetTrackIdx(pNtuTrack->GetTracksN());
				  track->MakeChiSquare();
				  track->SetType(1);
				  pNtuTrack->NewTrack(*track);
				  
				  if (ValidHistogram())
					 FillHistogramm(track);
   
				  delete track;
			   } else { // reset clusters
				  nextCluster->SetFound(false);
				  for (Int_t i = 0; i < array.GetEntries(); ++i) {
					 TAMSDpoint*  cluster1 = (TAMSDpoint*)array.At(i);
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
Bool_t TAMSDactNtuTrackF::IsGoodCandidate(TAMSDtrack* track)
{
   TAGparGeo* pGeoMap = (TAGparGeo*)fpGeoMapG->Object();

   Float_t width  = pGeoMap->GetTargetPar().Size[0];
   Float_t height = pGeoMap->GetTargetPar().Size[1];
   TVector3 vec = track->Intersection(-fpFootGeo->GetMSDCenter()[2]);

   if (TMath::Abs(vec.X()) > width || TMath::Abs(vec.Y()) > height)
	  return false;
   
   return true;
}


