/*!
 \file TAVTactNtuTrackF.cxx
  \brief  NTuplizer for VTX tracks using combinatory algorithm
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
 \brief  NTuplizer for VTX tracks using combinatory algorithm
 */

ClassImp(TAVTactNtuTrackF);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuClus cluster container descriptor
//! \param[out] pNtuTrack track container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pCalib calibration parameter descriptor
//! \param[in] pBMntuTrack input BM track container descriptor
TAVTactNtuTrackF::TAVTactNtuTrackF(const char* name, 
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
TAVTactNtuTrackF::~TAVTactNtuTrackF()
{
 
}

//_____________________________________________________________________________
//! Find tilted tracks
Bool_t TAVTactNtuTrackF::FindTiltedTracks()
{
   Double_t minDistance  = 1.e9;
   Double_t aDistance = 0;
   
   TAVTbaseParGeo* pGeoMap   = (TAVTbaseParGeo*) fpGeoMap->Object();
   
   TList array;
   array.SetOwner(false);
   array.Clear();
   
   TAGbaseTrack* track = 0x0;
   Int_t nPlane   = pGeoMap->GetSensorsN()-1;
   Int_t curPlane = nPlane;
   
   fMapClus.clear();
   
   while (curPlane >= fRequiredClusters) {
	  
	  // Get the last plane
	  curPlane = nPlane--;
	  
	  Int_t nLastClusters    = GetClustersN(curPlane);
	  
	  if ( nLastClusters == 0) continue;

	  for( Int_t iLastClus = 0; iLastClus < nLastClusters; ++iLastClus) { // loop on cluster of last plane
		 TAGcluster* lastCluster = GetCluster(curPlane, iLastClus);
		 if (lastCluster->Found()) continue;
		 lastCluster->SetFound();

		 for( Int_t iPlane = curPlane-1; iPlane >= 0; --iPlane) { // loop on next planes
			 
			Int_t nNextClusters    = GetClustersN(iPlane);
			
			for( Int_t iNextClus = 0; iNextClus < nNextClusters; ++iNextClus) { // loop on cluster of next plane
			   TAGcluster* nextCluster = GetCluster(iPlane, iNextClus);
			   if (nextCluster->Found()) continue;
            if (fMapClus[lastCluster] == 1) continue;

            track = NewTrack();
;
			   track->AddCluster(lastCluster);
			   nextCluster->SetFound();
			   track->AddCluster(nextCluster);

			   UpdateParam(track);

			   if (!IsGoodCandidate(track)) {
				  nextCluster->SetFound(false);
               for (Int_t i = 0; i < track->GetClustersN(); i++) {
                 TAGcluster* clus = track->GetCluster(i);
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
				   
				  Int_t nFirstClusters    = GetClustersN(iFirstPlane);
				  
				  minDistance = fSearchClusDistance;
				  TAGcluster* bestCluster = 0x0;
				  for( Int_t iFirstClus = 0; iFirstClus < nFirstClusters; ++iFirstClus) { // loop on cluster of first planes
					 TAGcluster* firstCluster = GetCluster(iFirstPlane, iFirstClus);
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
				  track->SetTrackIdx(GetTracksN());
				  track->MakeChiSquare();
				  track->SetType(1);
				  AddNewTrack(track);
				  
				  if (ValidHistogram()) 
					 FillHistogramm(track);
   
				  delete track;		 				  
			   } else { // reset clusters
				  nextCluster->SetFound(false);
				  for (Int_t i = 0; i < array.GetEntries(); ++i) {
					 TAGcluster*  cluster1 = (TAGcluster*)array.At(i);
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
//! Check if projected track is out of target area
//!
//! \param[in] trk a given track
Bool_t TAVTactNtuTrackF::IsGoodCandidate(TAGbaseTrack* trk)
{
   TAVTtrack* track = static_cast<TAVTtrack*>(trk);

   for (Int_t i = 0; i < track->GetClustersN(); i++) {
      TAVTcluster* clus = (TAVTcluster*)track->GetCluster(i);
      fMapClus[clus] = 1;
   }
   
   TAVTbaseParGeo*  pGeoMap  = (TAVTbaseParGeo*) fpGeoMap->Object();

   Float_t width  = pGeoMap->GetEpiSize()[0];//VTX_WIDTH/2.;
   Float_t height = pGeoMap->GetEpiSize()[1];//VTX_HEIGHT/2.;
   TVector3 vec = track->Intersection(-fpFootGeo->GetVTCenter()[2]);
   
   if (TMath::Abs(vec.X()) > width || TMath::Abs(vec.Y()) > height)
	  return false;
   
   return true;
}


