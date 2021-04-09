/*!
 \file
 \version $Id: TAGactNtuGlbTrackS.cxx
 \brief   Implementation of TAGactNtuGlbTrackS.
 */
#include "TClonesArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"

//TAGroot
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"

//VTX
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"

//ITR
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"
#include "TAITntuCluster.hxx"

//MSD
#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDparMap.hxx"
#include "TAMSDntuCluster.hxx"

//TW
#include "TATWntuPoint.hxx"

// IR
#include "TAGntuPoint.hxx"
#include "TAGntuGlbTrack.hxx"
#include "TAGactNtuGlbTrackS.hxx"


/*!
 \class TAGactNtuGlbTrackS
 \brief NTuplizer for Inner tracker tracks. **
 Combining VTX tracks with IT-MSD & TW clusters
 */

Bool_t  TAGactNtuGlbTrackS::fgBmMatched = false;

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGactNtuGlbTrackS::TAGactNtuGlbTrackS(const char* name,
								 TAGdataDsc* pVtVertex, TAGdataDsc* pItNtuClus, TAGdataDsc* pMsdNtuClus, TAGdataDsc* pTwNtuRec, TAGdataDsc* pNtuTrack,
								  TAGparaDsc* pVtGeoMap, TAGparaDsc* pItGeoMap, TAGparaDsc* pItConfig,   TAGparaDsc* pMsdGeoMap)
:  TAGaction(name, "TAGactNtuGlbTrackS - NTuplize Straight Track"),
   fpVertexVtx(pVtVertex),
   fpNtuClusItr(pItNtuClus),
   fpNtuClusMsd(pMsdNtuClus),
   fpNtuTrack(pNtuTrack),
   fpGeoMapVtx(pVtGeoMap),
   fpGeoMapItr(pItGeoMap),
   fpConfigItr(pItConfig),
   fpGeoMapMsd(pMsdGeoMap),
   fpNtuRecTw(pTwNtuRec),
   fVtVertex(0x0),
   fRequiredClusters(3),
   fSearchClusDistance(100),
   fGraphU(new TGraphErrors()),
   fGraphV(new TGraphErrors()),
   fOffsetVtx(0),
   fOffsetItr(0),
   fOffsetMsd(0),
   fOffsetTof(0)
{
   AddDataIn(pVtVertex,   "TAVTntuVertex");
   
   if (TAGrecoManager::GetPar()->IncludeIT())
      AddDataIn(pItNtuClus,  "TAITntuCluster");
   
   if (TAGrecoManager::GetPar()->IncludeMSD())
      AddDataIn(pMsdNtuClus, "TAMSDntuCluster");
   
   if (TAGrecoManager::GetPar()->IncludeTW())
      AddDataIn(pTwNtuRec,   "TATWntuPoint");
   
   AddDataOut(pNtuTrack,  "TAGntuGlbTrack");

   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   if (!fpFootGeo)
      Error("TAGactNtuGlbTrackS()", "No GeoTrafo action available yet\n");
   
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuGlbTrackS::~TAGactNtuGlbTrackS()
{
   delete fGraphU;
   delete fGraphV;

}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAGactNtuGlbTrackS::CreateHistogram()
{
   TString prefix("glb");
   TString titleDev = prefix;

   DeleteHistogram();
   TAVTparGeo* pGeoMapVt   = (TAVTparGeo*) fpGeoMapVtx->Object();
   Int_t nHistos = pGeoMapVt->GetSensorsN() ;
   fOffsetVtx = 0;
   
   TAITparGeo* pGeoMapIt = 0x0;
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      pGeoMapIt   = (TAITparGeo*) fpGeoMapItr->Object();
      fOffsetItr = nHistos;
      nHistos += pGeoMapIt->GetSensorsN();
   }
   
   TAMSDparGeo* pGeoMapMs = 0x0;
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      pGeoMapMs  = (TAMSDparGeo*) fpGeoMapMsd->Object();
      fOffsetMsd = nHistos;
      nHistos += pGeoMapMs->GetSensorsN();
   }

   fOffsetTof = nHistos+1;
   
   for (Int_t i = 0; i < nHistos+1; ++i) {
      
      fpHisResX[i] = new TH1F(Form("%sResX%d", prefix.Data(), i+1), Form("%s - ResidualX of sensor %d", titleDev.Data(), i+1), 400, -0.01, 0.01);
      AddHistogram(fpHisResX[i]);
      fpHisResY[i] = new TH1F(Form("%sResY%d", prefix.Data(), i+1), Form("%s - ResidualY of sensor %d", titleDev.Data(), i+1), 400, -0.01, 0.01);
      AddHistogram(fpHisResY[i]);
   }
   
   fpHisResTotX = new TH1F(Form("%sResTotX", prefix.Data()), Form("%s - Total ResidualX", titleDev.Data()), 400, -0.01, 0.01);
   fpHisResTotY = new TH1F(Form("%sResTotY", prefix.Data()), Form("%s - Total ResidualY", titleDev.Data()), 400, -0.01, 0.01);
   AddHistogram(fpHisResTotX);
   AddHistogram(fpHisResTotY);
   
   fpHisTrackEvt = new TH1F(Form("%sTrackEvt", prefix.Data()), Form("%s - Number of tracks per event", titleDev.Data()), 20, -0.5, 19.5);
   AddHistogram(fpHisTrackEvt);
   
   fpHisTrackClus = new TH1F(Form("%sTrackClus", prefix.Data()), Form("%s - Number of clusters per track", titleDev.Data()), 16, -0.5, 15.5);
   AddHistogram(fpHisTrackClus);
   
   fpHisClusSensor = new TH1F(Form("%sClusSensor", prefix.Data()), Form("%s - Number of tracked clusters per sensor", titleDev.Data()), 16, -0.5, 15.5);
   AddHistogram(fpHisClusSensor);
   
   fpHisTheta = new TH1F(Form("%sTrackTheta", prefix.Data()), Form("%s - Track polar distribution", titleDev.Data()), 500, 0, 90);
   AddHistogram(fpHisTheta);
   
   fpHisPhi = new TH1F(Form("%sTrackPhi", prefix.Data()), Form("%s - Track azimutal distribution (#theta > 5)", titleDev.Data()), 500, -180, 180);
   AddHistogram(fpHisPhi);
   
   fpHisBeamProf = new TH2F(Form("%sBeamProf", prefix.Data()), Form("%s -  Beam Profile", titleDev.Data()),
                            100, -pGeoMapIt->GetPitchX()*pGeoMapIt->GetPixelsNx()/2., pGeoMapIt->GetPitchX()*pGeoMapIt->GetPixelsNx()/2.,
                            100, -pGeoMapIt->GetPitchX()*pGeoMapIt->GetPixelsNx()/2., pGeoMapIt->GetPitchX()*pGeoMapIt->GetPixelsNx()/2.);
   fpHisBeamProf->SetStats(kFALSE);
   AddHistogram(fpHisBeamProf);
   
   SetValidHistogram(kTRUE);
   
   return;
}

//_____________________________________________________________________________
//
Bool_t TAGactNtuGlbTrackS::Action()
{
   // IT
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();
   pNtuTrack->Clear();

   FindTracks();
   
   if(FootDebugLevel(1)) {
      printf(" %d tracks found\n", pNtuTrack->GetTracksN());
      for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i) {
         TAGtrack* track = pNtuTrack->GetTrack(i);
      }
   }
   
   fpNtuTrack->SetBit(kValid);
   
   return true;
}

//_____________________________________________________________________________
//
Bool_t TAGactNtuGlbTrackS::CheckVtx()
{
   // VTX info
   Bool_t vtVertexOk = false;
   TAVTntuVertex* pNtuVertex  = (TAVTntuVertex*) fpVertexVtx->Object();
   
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
Bool_t TAGactNtuGlbTrackS::AppyCuts(TAGtrack* track)
{
   Bool_t valid = false;
   
   TAITparConf* pConfig = (TAITparConf*) fpConfigItr->Object();
   if (track->GetMeasPointsN() >= pConfig->GetAnalysisPar().PlanesForTrackMinimum )
      valid = true;
   
   return valid;
}

//_____________________________________________________________________________
//  
Bool_t TAGactNtuGlbTrackS::FindTracks()
{
   if (!CheckVtx()) return false;
   
   // get containers
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();
   
   Int_t nTracks = fVtVertex->GetTracksN();
   
   for(Int_t i = 0; i < nTracks; ++i) {	  // loop over VTX tracks
      
      TAVTtrack* vtTrack = fVtVertex->GetTrack(i);
      TAGtrack* track    = FillTracks(vtTrack);
   
      if (TAGrecoManager::GetPar()->IncludeIT())
         FindItrCluster(track);

      if (TAGrecoManager::GetPar()->IncludeMSD())
         FindMsdCluster(track);
      
      if (TAGrecoManager::GetPar()->IncludeTW())
         FindTwCluster(track);

      // Apply cuts
      if (AppyCuts(track)) {
         track->SetTrackId(pNtuTrack->GetTracksN());
         pNtuTrack->NewTrack(*track);
         if (ValidHistogram())
            FillHistogramm(track);
         delete track;
         
      } else
         delete track;
   }

   return true;
}


//_____________________________________________________________________________
//
TAGtrack* TAGactNtuGlbTrackS::FillTracks(TAVTtrack* vtTrack)
{
   TAGtrack* track   = new TAGtrack();
   Int_t nClus = vtTrack->GetClustersN();
   
   for (Int_t i = 0; i < nClus; ++i) {
      TAVTcluster* cluster = (TAVTcluster*)vtTrack->GetCluster(i);
      TVector3 posG = cluster->GetPositionG();
      TVector3 errG = cluster->GetPosErrorG();
      
      // from VT local to FOOT global
      posG = fpFootGeo->FromVTLocalToGlobal(posG);
      errG = fpFootGeo->FromVTLocalToGlobal(errG);
      
      TAGpoint* point = track->AddMeasPoint(TAVTparGeo::GetBaseName(), posG, errG);
      point->SetSensorIdx(cluster->GetSensorIdx());
   }
   
   UpdateParam(track);
   
   if (ValidHistogram())
      FillHistogramm();
   
   return track;
}

//_____________________________________________________________________________
//
void TAGactNtuGlbTrackS::FindItrCluster(TAGtrack* track)
{
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();
   TAITntuCluster* pNtuClus  = (TAITntuCluster*) fpNtuClusItr->Object();
   TAITparGeo*     pGeoMap   = (TAITparGeo*)     fpGeoMapItr->Object();

   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   Int_t nSensor = pGeoMap->GetSensorsN();

   // Loop on all sensors to find a matching cluster in them
   for( Int_t iSensor = 0; iSensor < nSensor; ++iSensor) { // loop on sensors
      
      TClonesArray* list = pNtuClus->GetListOfClusters(iSensor);
      Int_t nClusters = pNtuClus->GetClustersN(iSensor);
      if (nClusters == 0) continue; //empty sensors
      
      
      // loop on all clusters of this sensor and keep the nearest one
      minDistance = fSearchClusDistance*(1 + 2.*TMath::Tan(track->GetTgtTheta()*TMath::DegToRad()));
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
         
         // from IT local to FOOT global
         TVector3 posG = bestCluster->GetPositionG();
         TVector3 errG = bestCluster->GetPosErrorG();

         posG = fpFootGeo->FromITLocalToGlobal(posG);
         errG = fpFootGeo->FromITLocalToGlobal(errG);

         TAGpoint* point = track->AddMeasPoint(TAITparGeo::GetBaseName(), posG, errG);
         point->SetSensorIdx(iSensor);

         UpdateParam(track);
      }
      
   } // end loop on sensors
}

//_____________________________________________________________________________
//
void TAGactNtuGlbTrackS::FindMsdCluster(TAGtrack* track)
{
   TAGntuGlbTrack*  pNtuTrack = (TAGntuGlbTrack*)  fpNtuTrack->Object();
   TAMSDntuCluster* pNtuClus  = (TAMSDntuCluster*) fpNtuClusMsd->Object();
   TAMSDparGeo*     pGeoMap   = (TAMSDparGeo*)     fpGeoMapMsd->Object();
   
   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   Int_t nSensor = pGeoMap->GetSensorsN();
   
   // Loop on all sensors to find a matching cluster in them
   for( Int_t iSensor = 0; iSensor < nSensor; ++iSensor) { // loop on sensors
      
      TClonesArray* list = pNtuClus->GetListOfClusters(iSensor);
      Int_t nClusters = pNtuClus->GetClustersN(iSensor);
      if (nClusters == 0) continue; //empty sensors
      
      
      // loop on all clusters of this sensor and keep the nearest one
      minDistance = fSearchClusDistance*(1 + 2.*TMath::Tan(track->GetTgtTheta()*TMath::DegToRad()));
      TAMSDcluster* bestCluster = 0x0;
      
      for( Int_t iClus = 0; iClus < nClusters; ++iClus ) { // loop on sensor clusters
         TAMSDcluster* aCluster = (TAMSDcluster*)list->At(iClus);
         
         if( aCluster->Found()) continue; // skip cluster already found
         
         // from IT local to FOOT global
         TVector3 posG = aCluster->GetPositionG();
         posG = fpFootGeo->FromITLocalToGlobal(posG);
         
         // track intersection with the sensor
         TVector3 inter = track->Intersection(posG.Z());
         
         // compute distance
         if (aCluster->GetPlaneView() == 0)
            aDistance = TMath::Abs(inter[0]-posG[0]);
         else
            aDistance = TMath::Abs(inter[1]-posG[1]);
         
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
         TVector3 errG = bestCluster->GetPosErrorG();
         
         posG = fpFootGeo->FromMSDLocalToGlobal(posG);
         errG = fpFootGeo->FromMSDLocalToGlobal(errG);
         
         TAGpoint* point = track->AddMeasPoint(TAMSDparGeo::GetBaseName(), posG, errG);
         point->SetSensorIdx(iSensor);
         
         UpdateParam(track);
      }
      
   } // end loop on sensors
}

//_____________________________________________________________________________
//
void TAGactNtuGlbTrackS::FindTwCluster(TAGtrack* track)
{
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();
   TATWntuPoint*   pNtuClus  = (TATWntuPoint*)   fpNtuRecTw->Object();
   
   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   
   // loop on all clusters and keep the nearest one
   minDistance = fSearchClusDistance*(1 + 2.*TMath::Tan(track->GetTgtTheta()*TMath::DegToRad()));
   TATWpoint* bestCluster = 0x0;
   Int_t nClusters = pNtuClus->GetPointN();
   
   for( Int_t iClus = 0; iClus < nClusters; ++iClus ) { // loop on sensor clusters
      TATWpoint* aCluster =  pNtuClus->GetPoint(iClus);
      
      if( aCluster->Found()) continue; // skip cluster already found
      
      // from IT local to FOOT global
      TVector3 posG = aCluster->GetPositionG();
      posG = fpFootGeo->FromTWLocalToGlobal(posG);
      
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
      
      // from IT local to FOOT global
      TVector3 posG = bestCluster->GetPositionG();
      TVector3 errG = bestCluster->GetPosErrorG();
      
      posG = fpFootGeo->FromTWLocalToGlobal(posG);
      errG = fpFootGeo->FromTWLocalToGlobal(errG);
      
      TAGpoint* point = track->AddMeasPoint(TATWparGeo::GetBaseName(), posG, errG);
      point->SetSensorIdx(12);
      
      UpdateParam(track);
   }
      
}
//_____________________________________________________________________________
//
void TAGactNtuGlbTrackS::FillHistogramm(TAGtrack* track)
{
   fpHisTheta->Fill(track->GetTgtTheta());
   fpHisPhi->Fill(track->GetTgtPhi());
   
   TAITparGeo* pGeoMapIt = (TAITparGeo*)fpGeoMapItr->Object();
   TAVTparGeo* pGeoMapVt = (TAVTparGeo*)fpGeoMapVtx->Object();

   fpHisTrackClus->Fill(track->GetMeasPointsN());
   for (Int_t i = 0; i < track->GetMeasPointsN(); ++i) {
      
      Int_t offset = 0;
      TAGpoint* cluster = track->GetMeasPoint(i);
      Int_t idx = cluster->GetSensorIdx();
      TString devName = cluster->GetDevName();
      
      fpHisClusSensor->Fill(idx+1);
      
      Float_t posZ       = cluster->GetPositionG()[2];
      TVector3 impact    = track->Intersection(posZ);
      
      TVector3 impactLoc;
      
      if (devName.Contains(TAVTparGeo::GetBaseName())) {
         impactLoc = pGeoMapVt->Detector2Sensor(idx, impact);
         offset = 0;
      }
      
      if (devName.Contains(TAITparGeo::GetBaseName())) {
         impactLoc = pGeoMapIt->Detector2Sensor(idx, impact);
         offset = fOffsetItr;;
      }
      
      if (devName.Contains(TAMSDparGeo::GetBaseName())) {
         impactLoc = pGeoMapIt->Detector2Sensor(idx, impact);
         offset = fOffsetMsd;
      }
      
      if (devName.Contains(TAMSDparGeo::GetBaseName())) {
         impactLoc = pGeoMapIt->Detector2Sensor(idx, impact);
         offset = fOffsetTof;
      }
      
      
      fpHisResTotX->Fill(impact[0]-cluster->GetPositionG()[0]);
      fpHisResTotY->Fill(impact[1]-cluster->GetPositionG()[1]);
      fpHisResX[idx+offset]->Fill(impact[0]-cluster->GetPositionG()[0]);
      fpHisResY[idx+offset]->Fill(impact[1]-cluster->GetPositionG()[1]);
   }
   
   TVector3 origin = track->GetTgtPosition();
   fpHisBeamProf->Fill(origin.X(), origin.Y());
}

//_____________________________________________________________________________
//
void TAGactNtuGlbTrackS::FillHistogramm()
{
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();

   fpHisTrackEvt->Fill(pNtuTrack->GetTracksN());
   if (pNtuTrack->GetTracksN() == 0)
      fpHisClusSensor->Fill(0);
}

//_____________________________________________________________________________
//
void TAGactNtuGlbTrackS::UpdateParam(TAGtrack* track)
{
   TVector3           lineOrigin;  // origin in the tracker system
   TVector3           lineSlope;   // slope along z-axis in tracker system
   
   // init
   lineOrigin.SetXYZ(0.,0.,0.);
   lineSlope.SetXYZ(0.,0.,1.);
   
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMapItr->Object();
   
   Int_t nClusters = track->GetMeasPointsN();
   
   Double_t x, dx;
   Double_t y, dy;
   Double_t z, dz;
   
   fGraphU->Set(nClusters);
   fGraphV->Set(nClusters);
   
   for (Int_t i = 0; i < nClusters; ++i) {
      TAGpoint* cluster = track->GetMeasPoint(i);
      x  = cluster->GetPositionG()(0);
      y  = cluster->GetPositionG()(1);
      z  = cluster->GetPositionG()(2);
      dx = cluster->GetPosErrorG()(0);
      dy = cluster->GetPosErrorG()(1);
      dz = cluster->GetPosErrorG()(2);
      fGraphU->SetPoint(i, z, x);
      fGraphU->SetPointError(i, dz, dx);
      
      fGraphV->SetPoint(i, z, y);
      fGraphV->SetPointError(i, dz, dy);
   }
   
   fGraphU->Fit("pol1", "Q");
   TF1* polyU = fGraphU->GetFunction("pol1");
   (lineOrigin)(0) = polyU->GetParameter(0);
   (lineSlope)(0)  = polyU->GetParameter(1);
   fGraphV->Fit("pol1", "Q");
   TF1* polyV = fGraphV->GetFunction("pol1");
   (lineOrigin)(1) = polyV->GetParameter(0);
   (lineSlope)(1)  = polyV->GetParameter(1);
   
   
   track->SetTgtPosition(lineOrigin);
   track->SetTgtDirection(lineSlope);
}
