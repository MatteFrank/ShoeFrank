/*!
 \file
 \version $Id: TAIRactNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAIRactNtuTrack.
 */
#include "TClonesArray.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"

//TAGroot
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"

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

Bool_t  TAIRactNtuTrack::fgBmMatched = false;

//------------------------------------------+-----------------------------------
//! Default constructor.
TAIRactNtuTrack::TAIRactNtuTrack(const char* name,
								 TAGdataDsc* pNtuClus, TAGdataDsc* pVtVertex, TAGdataDsc* pNtuTrack,
								 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap, TAGparaDsc* pvtGeoMap, TAGparaDsc* pCalib)
 : TAVTactBaseTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib),
   fpVtVertex(pVtVertex),
   fVtVertex(0x0),
   fpVtGeoMap(pvtGeoMap)
{
   AddDataOut(pNtuTrack, "TAIRntuTrack");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAIRactNtuTrack::~TAIRactNtuTrack()
{

}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAIRactNtuTrack::CreateHistogram()
{
   DeleteHistogram();
   TAVTbaseParGeo* pGeoMapIt  = (TAVTbaseParGeo*) fpGeoMap->Object();
   TAVTbaseParGeo* pGeoMapVt  = (TAVTbaseParGeo*) fpVtGeoMap->Object();

   fpHisPixelTot = new TH1F(Form("%sTrackedClusPixTot", fPrefix.Data()), Form("%s - Total # pixels per tracked clusters", fTitleDev.Data()), 100, -0.5, 99.5);
   AddHistogram(fpHisPixelTot);
   
   for (Int_t i = 0; i < pGeoMapVt->GetSensorsN() + pGeoMapIt->GetSensorsN(); ++i) {
      
      fpHisPixel[i] = new TH1F(Form("%sTrackedClusPix%d", fPrefix.Data(), i+1), Form("%s - # pixels per tracked clusters of sensor %d", fTitleDev.Data(), i+1), 100, -0.5, 99.5);
      AddHistogram(fpHisPixel[i]);
      
      fpHisResX[i] = new TH1F(Form("%sResX%d", fPrefix.Data(), i+1), Form("%s - ResidualX of sensor %d", fTitleDev.Data(), i+1), 400, -0.01, 0.01);
      AddHistogram(fpHisResX[i]);
      fpHisResY[i] = new TH1F(Form("%sResY%d", fPrefix.Data(), i+1), Form("%s - ResidualY of sensor %d", fTitleDev.Data(), i+1), 400, -0.01, 0.01);
      AddHistogram(fpHisResY[i]);
   }
   
   fpHisResTotX = new TH1F(Form("%sResTotX", fPrefix.Data()), Form("%s - Total ResidualX", fTitleDev.Data()), 400, -0.01, 0.01);
   fpHisResTotY = new TH1F(Form("%sResTotY", fPrefix.Data()), Form("%s - Total ResidualY", fTitleDev.Data()), 400, -0.01, 0.01);
   AddHistogram(fpHisResTotX);
   AddHistogram(fpHisResTotY);
   
   fpHisChi2TotX = new TH1F(Form("%sChi2TotX", fPrefix.Data()), Form("%s - Total Chi2 X", fTitleDev.Data()), 200, 0, 20);
   fpHisChi2TotY = new TH1F(Form("%sChi2TotY", fPrefix.Data()), Form("%s - Total Chi2 Y", fTitleDev.Data()), 200, 0, 20);
   AddHistogram(fpHisChi2TotX);
   AddHistogram(fpHisChi2TotY);
   
   fpHisTrackEvt = new TH1F(Form("%sTrackEvt", fPrefix.Data()), Form("%s - Number of tracks per event", fTitleDev.Data()), 20, -0.5, 19.5);
   AddHistogram(fpHisTrackEvt);
   
   fpHisTrackClus = new TH1F(Form("%sTrackClus", fPrefix.Data()), Form("%s - Number of clusters per track", fTitleDev.Data()), 9, -0.5, 8.5);
   AddHistogram(fpHisTrackClus);
   
   fpHisClusSensor = new TH1F(Form("%sClusSensor", fPrefix.Data()), Form("%s - Number of tracked clusters per sensor", fTitleDev.Data()), 9, -0.5, 8.5);
   AddHistogram(fpHisClusSensor);
   
   fpHisTheta = new TH1F(Form("%sTrackTheta", fPrefix.Data()), Form("%s - Track polar distribution", fTitleDev.Data()), 500, 0, 90);
   AddHistogram(fpHisTheta);
   
   fpHisPhi = new TH1F(Form("%sTrackPhi", fPrefix.Data()), Form("%s - Track azimutal distribution (#theta > 5)", fTitleDev.Data()), 500, -180, 180);
   AddHistogram(fpHisPhi);
   
   fpHisBeamProf = new TH2F(Form("%sBeamProf", fPrefix.Data()), Form("%s -  Beam Profile", fTitleDev.Data()),
                            100, -pGeoMapIt->GetPitchX()*pGeoMapIt->GetPixelsNx()/2., pGeoMapIt->GetPitchX()*pGeoMapIt->GetPixelsNx()/2.,
                            100, -pGeoMapIt->GetPitchX()*pGeoMapIt->GetPixelsNx()/2., pGeoMapIt->GetPitchX()*pGeoMapIt->GetPixelsNx()/2.);
   fpHisBeamProf->SetStats(kFALSE);
   AddHistogram(fpHisBeamProf);
   
   SetValidHistogram(kTRUE);
   
   return;
}

//_____________________________________________________________________________
//
Bool_t TAIRactNtuTrack::Action()
{
   // IT
   TAIRntuTrack* pNtuTrack = (TAIRntuTrack*) fpNtuTrack->Object();
   pNtuTrack->Clear();
   
   
   FindTracks();
   
   if(FootDebugLevel(1)) {
      printf(" %d tracks found\n", pNtuTrack->GetTracksN());
      for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i) {
         TAIRtrack* track = pNtuTrack->GetTrack(i);
         printf("   with # clusters %d\n", track->GetClustersN());
      }
   }
   
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
      if (!vtx->IsBmMatched() && fgBmMatched) continue; // only for pileup
      vtVertexOk     = true;
      fVtVertex       = vtx;
      
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
      cluster->SetPositionG(posG);
      
      track->AddCluster(cluster);
   }
   
   UpdateParam(track);
   
   if (ValidHistogram())
      FillHistogramm();

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
   TAIRntuTrack*   pNtuTrack = (TAIRntuTrack*)   fpNtuTrack->Object();
   TAITntuCluster* pNtuClus  = (TAITntuCluster*) fpNtuClus->Object();
   TAITparGeo*     pGeoMap   = (TAITparGeo*)     fpGeoMap->Object();
   TAITparConf*    pConfig   = (TAITparConf*)    fpConfig->Object();
   TAVTbaseParGeo* pGeoMapVt = (TAVTbaseParGeo*) fpVtGeoMap->Object();

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
            
            // from IT local to FOOT global
            TVector3 posG = bestCluster->GetPositionG();
            posG = fpFootGeo->FromITLocalToGlobal(posG);
            TAIRcluster* last = track->GetLastCluster();
            last->SetPositionG(posG);
            last->SetSensorIdx(last->GetSensorIdx()+pGeoMapVt->GetSensorsN());

            if (fgRefit)
               UpdateParam(track);
         }
         
      } // end loop on sensors
      
      // Apply cuts
      if (AppyCuts(track)) {
         track->SetTrackIdx(pNtuTrack->GetTracksN());
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

//_____________________________________________________________________________
//
void TAIRactNtuTrack::FillHistogramm(TAGbaseTrack* track)
{
   fpHisTheta->Fill(track->GetTheta());
   fpHisPhi->Fill(track->GetPhi());
   
   TAVTbaseParGeo* pGeoMapIt = (TAVTbaseParGeo*)fpGeoMap->Object();
   TAVTbaseParGeo* pGeoMapVt = (TAVTbaseParGeo*)fpVtGeoMap->Object();

   fpHisTrackClus->Fill(track->GetClustersN());
   for (Int_t i = 0; i < track->GetClustersN(); ++i) {
      TAIRcluster * cluster = (TAIRcluster*)track->GetCluster(i);
      cluster->SetFound();
      Int_t idx = cluster->GetSensorIdx();
      fpHisClusSensor->Fill(idx+1);
      fpHisPixelTot->Fill(cluster->GetPixelsN());
      fpHisPixel[idx]->Fill(cluster->GetPixelsN());
      
      Float_t posZ       = cluster->GetPositionG()[2];
      TVector3 impact    = track->Intersection(posZ);
      
      TVector3 impactLoc;
      
      if (idx < pGeoMapVt->GetSensorsN())
         impactLoc = pGeoMapVt->Detector2Sensor(idx, impact);
      else
         impactLoc = pGeoMapIt->Detector2Sensor(idx-pGeoMapVt->GetSensorsN(), impact);

      fpHisResTotX->Fill(impact[0]-cluster->GetPositionG()[0]);
      fpHisResTotY->Fill(impact[1]-cluster->GetPositionG()[1]);
      fpHisResX[idx]->Fill(impact[0]-cluster->GetPositionG()[0]);
      fpHisResY[idx]->Fill(impact[1]-cluster->GetPositionG()[1]);
   }
   fpHisChi2TotX->Fill(track->GetChi2U());
   fpHisChi2TotY->Fill(track->GetChi2V());
   
   TVector3 origin = track->GetOrigin();
   fpHisBeamProf->Fill(origin.X(), origin.Y());
}

//_____________________________________________________________________________
//
void TAIRactNtuTrack::FillHistogramm()
{
   TAIRntuTrack*   pNtuTrack = (TAIRntuTrack*)   fpNtuTrack->Object();
   
   fpHisTrackEvt->Fill(pNtuTrack->GetTracksN());
   if (pNtuTrack->GetTracksN() == 0)
      fpHisClusSensor->Fill(0);
}

