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

#include "TAVTparGeo.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTactNtuTrackF.hxx"


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
   AddDataIn(pNtuClus,   "TAVTntuCluster");
   AddDataOut(pNtuTrack, "TAVTntuTrack");
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
   TAVTbaseParGeo* pGeoMap   = (TAVTbaseParGeo*) fpGeoMap->Object();
   
   TList array;
   array.SetOwner(false);
   array.Clear();
   
   TAVTtrack* track = 0x0;
   Int_t nPlane   = pGeoMap->GetSensorsN()-1;
   Int_t curPlane = nPlane;
   
   fMapClus.clear();
   
   while (curPlane >= fRequiredClusters) {
	  
	  // Get the last plane
	  curPlane = nPlane--;
	  
	  TClonesArray* lastList = pNtuClus->GetListOfClusters(curPlane);
	  Int_t nLastClusters    = pNtuClus->GetClustersN(curPlane);
	  
	  if ( nLastClusters == 0) continue;

	  for( Int_t iLastClus = 0; iLastClus < nLastClusters; ++iLastClus) { // loop on cluster of last plane
		 TAVTcluster* lastCluster = (TAVTcluster*)lastList->At(iLastClus);
		 if (lastCluster->Found()) continue;
		 lastCluster->SetFound();

		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on next planes
			 
			TClonesArray* nextList = pNtuClus->GetListOfClusters(iPlane);
			Int_t nNextClusters    = pNtuClus->GetClustersN(iPlane);
			
			for( Int_t iNextClus = 0; iNextClus < nNextClusters; ++iNextClus) { // loop on cluster of next plane
			   TAVTcluster* nextCluster = (TAVTcluster*)nextList->At(iNextClus);
			   if (nextCluster->Found()) continue;
            if (fMapClus[lastCluster] == 1) continue;

			   track = new TAVTtrack();
			   track->AddCluster(lastCluster);
			   nextCluster->SetFound();
			   track->AddCluster(nextCluster);

			   UpdateParam(track);

			   if (!IsGoodCandidate(track)) {
				  nextCluster->SetFound(false);
               for (Int_t i = 0; i < track->GetClustersN(); i++) {
                 TAVTcluster* clus = (TAVTcluster*)track->GetCluster(i);
                 fMapClus[clus] = 0;
              }
               if (curPlane == pGeoMap->GetSensorsN()-1)
                  fMapClus[lastCluster] = 1;
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
   
   if (ValidHistogram())
      if (fBmTrackOk)
         FillBmHistogramm(fBmTrackPos);

   return true;
}

//_____________________________________________________________________________
//  
Bool_t TAVTactNtuTrackF::IsGoodCandidate(TAVTtrack* track)
{
   for (Int_t i = 0; i < track->GetClustersN(); i++) {
      TAVTcluster* clus = (TAVTcluster*)track->GetCluster(i);
      fMapClus[clus] = 1;
   }
   
   TAVTbaseParGeo*  pGeoMap  = (TAVTbaseParGeo*) fpGeoMap->Object();
   TAGgeoTrafo* pFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   Float_t width  = pGeoMap->GetEpiSize()[0];//VTX_WIDTH/2.;
   Float_t height = pGeoMap->GetEpiSize()[1];//VTX_HEIGHT/2.;
   TVector3 vec = track->Intersection(-pFootGeo->GetVTCenter()[2]);
   
   if (TMath::Abs(vec.X()) > width || TMath::Abs(vec.Y()) > height)
	  return false;
   
   return true;
}


