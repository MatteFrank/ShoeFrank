/*!
 \file
 \version $Id: TAIRactNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAIRactNtuTrack.
 */
#include "TClonesArray.h"
#include "TMath.h"

//TAGroot
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "GlobalPar.hxx"

//VTX
#include "TAVTntuVertex.hxx"

#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"
#include "TAVTtrack.hxx"
#include "TAIRtrack.hxx"
#include "TAIRntuTrack.hxx"
#include "TAITntuCluster.hxx"
#include "TAIRactNtuTrack.hxx"


/*!
 \class TAIRactNtuTrack
 \brief NTuplizer for Inner tracker tracks. **
 Combining VTX tracks with IT clusters
 */

ClassImp(TAIRactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAIRactNtuTrack::TAIRactNtuTrack(const char* name,
								 TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGdataDsc* pVtVertex)
 : TAVTactBaseTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib),
   fpVtVertex(pVtVertex),
   fVtVertex(0x0)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAIRactNtuTrack::~TAIRactNtuTrack()
{

}

//_____________________________________________________________________________
//
Bool_t TAIRactNtuTrack::Action()
{
   // IT
   TAIRntuTrack* pNtuTrack = (TAIRntuTrack*) fpNtuTrack->Object();
   pNtuTrack->Clear();
   
   
   
   // looking inclined line
   if (!FindTracks()){
      if (ValidHistogram())
         FillHistogramm();
      fpNtuTrack->SetBit(kValid);
      return true;
   }
   
   if(FootDebugLevel(1)) {
      printf(" %d tracks found\n", pNtuTrack->GetTracksN());
      for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i) {
         TAIRtrack* track = pNtuTrack->GetTrack(i);
         printf("   with # clusters %d\n", track->GetClustersN());
      }
   }
   
   if (ValidHistogram())
      FillHistogramm();
   
   fpNtuTrack->SetBit(kValid);
   return true;
}

//_____________________________________________________________________________
//
Bool_t TAIRactNtuTrack::CheckVtx()
{
   // VTX info
   Bool_t vtVertexOk = false;
   TAVTntuVertex* pNtuVertex  = (TAVTntuVertex*) fpVtVertex->Object();
   
   Int_t nVertex = pNtuVertex->GetVertexN();
   
   for (Int_t i = 0; i < nVertex; ++i) {
      
      TAVTvertex* vtx = pNtuVertex->GetVertex(i);
      if (vtx->IsBmMatched()) {
         vtVertexOk     = true;
         fVtVertex       = vtx;
      }
   }
   
   return vtVertexOk;
}

//_____________________________________________________________________________
//
TAIRtrack* TAIRactNtuTrack::FillTracks(TAVTtrack* vtTrack)
{
   TAIRtrack* track   = new TAIRtrack();
   Int_t nClus = vtTrack->GetClustersN();
   
   for (Int_t i = 0; i < nClus; ++i) {
      TAVTbaseCluster* cluster = vtTrack->GetCluster(i);
      TVector3 posG = cluster->GetPositionG();
      
      // from VT local to FOOT global
      posG = fpFootGeo->FromVTLocalToGlobal(posG);
      cluster->SetPositionG(&posG);
      
      track->AddCluster(cluster);
   }
   
   UpdateParam(track);
   
   return track;
}

//_____________________________________________________________________________
//  
Bool_t TAIRactNtuTrack::FindTracks()
{
   if (!CheckVtx()) return false;
   
   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   
   // get containers
   TAITntuCluster* pNtuClus  = (TAITntuCluster*) fpNtuClus->Object();
   TAIRntuTrack*   pNtuTrack = (TAIRntuTrack*)   fpNtuTrack->Object();
   TAITparGeo*     pGeoMap   = (TAITparGeo*)     fpGeoMap->Object();
   TAITparConf*    pConfig   = (TAITparConf*)    fpConfig->Object();
   
   Int_t nSensor = pGeoMap->GetSensorsN();
   
   Int_t nTracks = fVtVertex->GetTracksN();
   
   for(Int_t i = 0; i < nTracks; ++i) {	  // loop over VTX tracks
      
      TAVTtrack* vtTrack = fVtVertex->GetTrack(i);
      TAIRtrack* track   = FillTracks(vtTrack);
   
      // Loop on all sensors to find a matching cluster in them
      for( Int_t iSensor = 0; iSensor < nSensor; ++iSensor) { // loop on sensors
         
         TClonesArray* list = pNtuClus->GetListOfClusters(iSensor);
         Int_t nClusters = pNtuClus->GetClustersN(iSensor);
         if (nClusters == 0) continue; //empty sensors
         
         if( pNtuTrack->GetTracksN() >= pConfig->GetAnalysisPar().TracksMaximum ) break; // if max track number reach, stop
         
         // loop on all clusters of this sensor and keep the nearest one
         minDistance = fSearchClusDistance*(1 + 2.*TMath::Tan(track->GetTheta()*TMath::DegToRad()));
         TAITcluster* bestCluster = 0x0;
         
         for( Int_t iClus = 0; iClus < nClusters; ++iClus ) { // loop on sensor clusters
            TAITcluster* aCluster = (TAITcluster*)list->At(iClus);
            
            if( aCluster->Found()) continue; // skip cluster already found
            
            // from IT local to FOOT global
            TVector3 posG = aCluster->GetPositionG();
            posG = fpFootGeo->FromITLocalToGlobal(posG);
   
            // track intersection with the sensor
            TVector3 inter = track->Intersection(posG.Z());
            
            // compute distance
            posG[2] = 0.; // do not considered Z axis
            aDistance = (inter-posG).Mag();
            
            if( aDistance < minDistance ) {
               minDistance = aDistance;
               bestCluster = aCluster;
            }
         } // end loop on sensor clusters
         
         // if a cluster has been found, add the cluster
         if( bestCluster ) {
            bestCluster->SetFound();
            // from IT local to FOOT global
            TVector3 posG = bestCluster->GetPositionG();
            posG = fpFootGeo->FromITLocalToGlobal(posG);
            bestCluster->SetPositionG(&posG);
            
            track->AddCluster(bestCluster);
            if (fgRefit)
               UpdateParam(track);
         }
         
      } // end loop on sensors
      
      // Apply cuts
      if (AppyCuts(track)) {
         track->SetNumber(pNtuTrack->GetTracksN());
         track->MakeChiSquare();
         track->SetType(1);
         pNtuTrack->NewTrack(*track);
         
         if (ValidHistogram())
            FillHistogramm(track);
         
         delete track;
         
      } else {
         delete track;
      }
   }

   return true;
}



