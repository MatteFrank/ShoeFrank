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

//VTX
#include "TAVTntuVertex.hxx"

#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"
#include "TAVTtrack.hxx"
#include "TAITntuTrack.hxx"
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
 : TAITactBaseNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib),
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
Bool_t TAIRactNtuTrack::FindTiltedTracks()
{
   if (!CheckVtx()) return false;
   
   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   
   // get containers
   TAITntuCluster* pNtuClus  = (TAITntuCluster*) fpNtuClus->Object();
   TAITntuTrack*   pNtuTrack = (TAITntuTrack*)   fpNtuTrack->Object();
   TAITparGeo*     pGeoMap   = (TAITparGeo*)     fpGeoMap->Object();
   TAITparConf*    pConfig   = (TAITparConf*)    fpConfig->Object();
   
   TList array;
   array.SetOwner(false);
   array.Clear();

   Int_t nSensor = pGeoMap->GetSensorsN();
   
   Int_t nTracks = fVtVertex->GetTracksN();
   
   for(Int_t i = 0; i < nTracks; ++i) {	  // loop over VTX tracks
      
      TAVTtrack* vtTrack = fVtVertex->GetTrack(i);
      TAITtrack* track   = new TAITtrack(*vtTrack);
   
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
            
            // put IT cluster pos in VT frame
            TVector3 posG = aCluster->GetPositionG();
            // from IT local to FOOT global
            posG = fpFootGeo->FromITLocalToGlobal(posG);
            // from FOOT global to VT local
            posG = fpFootGeo->FromGlobalToVTLocal(posG);
   
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
            track->AddCluster(bestCluster);
            array.Add(bestCluster);
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
         
      } else { // reset clusters
         for (Int_t i = 0; i < array.GetEntries(); ++i) {
            TAITcluster*  cluster1 = (TAITcluster*)array.At(i);
            cluster1->SetFound(false);
         }
         array.Clear();
         delete track;
      }
   }

   return true;
}



