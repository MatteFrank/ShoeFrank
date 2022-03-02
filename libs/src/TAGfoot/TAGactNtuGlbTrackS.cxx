/*!
 \file TAGactNtuGlbTrackS.cxx
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

// TGT
#include "TAGparGeo.hxx"

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
#include "TAMSDntuPoint.hxx"

//TW
#include "TATWparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TATWntuPoint.hxx"

//CA
#include "TACAparGeo.hxx"
#include "TACAntuCluster.hxx"

// GLB
#include "TAGntuPoint.hxx"
#include "TAGntuGlbTrack.hxx"
#include "TAGactNtuGlbTrackS.hxx"

// EM
#include "TADItrackEmProperties.hxx"


/*!
 \class TAGactNtuGlbTrackS
 \brief Global straight line tracking
 Combining VTX tracks with IT-MSD & TW clusters (& CAL)
 Computing energy loss and scattering angle
 */

Bool_t  TAGactNtuGlbTrackS::fgBmMatched = false;

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pVtVertex  vertex container descriptor
//! \param[in] pItNtuClus ITR cluster container descriptor
//! \param[in] pMsdNtuClus MSD point container descriptor
//! \param[in] pTwNtuRec TW point container descriptor
//! \param[in] pCaNtuClus CAL cluster container descriptor
//! \param[out] pNtuTrack global track container descriptor
//! \param[in] pVtGeoMap VTX geometry parameter descriptor
//! \param[in] pItGeoMap ITR geometry parameter descriptor
//! \param[in] pMsdGeoMap MSD geometry parameter descriptor
//! \param[in] pTwGeoMap TW geometry parameter descriptor
//! \param[in] pgGeoMap target geometry parameter descriptor
TAGactNtuGlbTrackS::TAGactNtuGlbTrackS(const char* name,
								 TAGdataDsc* pVtVertex, TAGdataDsc* pItNtuClus, TAGdataDsc* pMsdNtuClus, TAGdataDsc* pTwNtuRec, TAGdataDsc* pCaNtuClus, TAGdataDsc* pNtuTrack,
								  TAGparaDsc* pVtGeoMap, TAGparaDsc* pItGeoMap, TAGparaDsc* pMsdGeoMap,TAGparaDsc* pTwGeoMap, TAGparaDsc* pgGeoMap)
:  TAGaction(name, "TAGactNtuGlbTrackS - NTuplize Straight Track"),
   fpVertexVtx(pVtVertex),
   fpNtuClusItr(pItNtuClus),
   fpNtuClusPoint(pMsdNtuClus),
   fpNtuRecTw(pTwNtuRec),
   fpNtuClusCa(pCaNtuClus),
   fpNtuTrack(pNtuTrack),
   fpGeoMapVtx(pVtGeoMap),
   fpGeoMapItr(pItGeoMap),
   fpGeoMapMsd(pMsdGeoMap),
   fpGeoMapTof(pTwGeoMap),
   fpGeoMapG(pgGeoMap),
   fVtVertex(0x0),
   fRequiredClusters(3),
   fSearchClusDistItr(0.06),
   fSearchClusDistMsd(0.08),
   fSearchClusDistTof(2),
   fOffsetVtx(0),
   fOffsetItr(0),
   fOffsetMsd(0),
   fOffsetTof(0),
   fEmProperties(new TADItrackEmProperties)
{
   AddDataIn(pVtVertex,   "TAVTntuVertex");
   
   if (TAGrecoManager::GetPar()->IncludeIT())
      AddDataIn(pItNtuClus,  "TAITntuCluster");
   
   if (TAGrecoManager::GetPar()->IncludeMSD())
      AddDataIn(pMsdNtuClus, "TAMSDntuPoint");
   
   if (TAGrecoManager::GetPar()->IncludeTW())
      AddDataIn(pTwNtuRec,   "TATWntuPoint");
   
   if (TAGrecoManager::GetPar()->IncludeCA())
      AddDataIn(pCaNtuClus, "TACAntuCluster");
   
   AddDataOut(pNtuTrack,  "TAGntuGlbTrack");

   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   if (!fpFootGeo)
      Error("TAGactNtuGlbTrackS()", "No GeoTrafo action available yet\n");
   
   TAGparGeo* pGeoMapG    = (TAGparGeo*)   fpGeoMapG->Object();
   TAVTparGeo* pGeoMapVt  = (TAVTparGeo*)  fpGeoMapVtx->Object();

   fSensorThickVtx = pGeoMapVt->GetTotalSize()[2];
   fLastPlaneVtx   = pGeoMapVt->GetLayerPosZ(pGeoMapVt->GetSensorsN()-1);
   fLastPlaneVtx   = fpFootGeo->FromVTLocalToGlobal(TVector3(0,0,fLastPlaneVtx))[2];
   
   // in case no ITR and  MSD
   fLastPlaneItr = fLastPlaneVtx;
   fLastPlaneMsd = fLastPlaneVtx;
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      TATWparGeo* pGeoMapTw  = (TATWparGeo*)  fpGeoMapTof->Object();
      fWallThickTw = pGeoMapTw->GetBarThick()*2;
   }
   
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      TAITparGeo* pGeoMapIt  = (TAITparGeo*)  fpGeoMapItr->Object();
      fSensorThickItr = pGeoMapIt->GetTotalSize()[2];
      fLastPlaneItr   = pGeoMapIt->GetLayerPosZ(3);
      fLastPlaneItr   = fpFootGeo->FromITLocalToGlobal(TVector3(0,0,fLastPlaneItr))[2];
   } 
   
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      TAMSDparGeo* pGeoMapMs = (TAMSDparGeo*) fpGeoMapMsd->Object();
      fSensorThickMsd = pGeoMapMs->GetTotalSize()[2];
      fLastPlaneMsd   = pGeoMapMs->GetLayerPosZ(pGeoMapMs->GetSensorsN()-1);
      fLastPlaneMsd   = fpFootGeo->FromMSDLocalToGlobal(TVector3(0,0,fLastPlaneMsd))[2];
      
   } else if (TAGrecoManager::GetPar()->IncludeIT())
     fLastPlaneMsd = fSensorThickItr;

   // beam loss in target
   fBeamEnergyTarget = pGeoMapG->GetBeamPar().Energy;
   Float_t bA = pGeoMapG->GetBeamPar().AtomicMass;
   Float_t bz = pGeoMapG->GetBeamPar().AtomicNumber;

   TString matTgt   = pGeoMapG->GetTargetPar().Material;
   Float_t thickTgt = pGeoMapG->GetTargetPar().Size[2]/2.; // in average
   
   fBeamEnergyTarget = fEmProperties->GetEnergyLoss(matTgt, thickTgt, fBeamEnergyTarget, bA, bz);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuGlbTrackS::~TAGactNtuGlbTrackS()
{
   delete fEmProperties;
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
      nHistos += pGeoMapMs->GetStationsN();
   }

   fOffsetTof = nHistos;
   
   for (Int_t i = 0; i < nHistos+1; ++i) {
      fpHisResX[i] = new TH1F(Form("%sResX%d", prefix.Data(), i+1), Form("%s - ResidualX of sensor %d", titleDev.Data(), i+1), 400, -0.2, 0.2);
      AddHistogram(fpHisResX[i]);
      fpHisResY[i] = new TH1F(Form("%sResY%d", prefix.Data(), i+1), Form("%s - ResidualY of sensor %d", titleDev.Data(), i+1), 400, -0.2, 0.2);
      AddHistogram(fpHisResY[i]);
   }
   
   fpHisResTotX = new TH1F(Form("%sResTotX", prefix.Data()), Form("%s - Total ResidualX", titleDev.Data()), 400, -0.2, 0.2);
   fpHisResTotY = new TH1F(Form("%sResTotY", prefix.Data()), Form("%s - Total ResidualY", titleDev.Data()), 400, -0.2, 0.2);
   AddHistogram(fpHisResTotX);
   AddHistogram(fpHisResTotY);

   fpHisResTrkTotX = new TH1F(Form("%sResTrkTotX", prefix.Data()), Form("%s - Total ResidualX for tracker", titleDev.Data()), 400, -0.2, 0.2);
   fpHisResTrkTotY = new TH1F(Form("%sResTrkTotY", prefix.Data()), Form("%s - Total ResidualY for tracker", titleDev.Data()), 400, -0.2, 0.2);
   AddHistogram(fpHisResTrkTotX);
   AddHistogram(fpHisResTrkTotY);

   
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
   
   fpHisTwDeCaE = new TH2F(Form("%sTwDeCaE", prefix.Data()), Form("%s - TW-deltaE vs CA-E", titleDev.Data()), 300, 0, 3000,  100, 0, 200);
   fpHisTwDeCaE->SetStats(kFALSE);
   AddHistogram(fpHisTwDeCaE);
   
   fpHisMsdDeCaE = new TH2F(Form("%sMsdDeCaE", prefix.Data()), Form("%s - MSD-deltaE vs CA-E", titleDev.Data()), 300, 0, 3000,  300, 0, 10000);
   fpHisMsdDeCaE->SetStats(kFALSE);
   AddHistogram(fpHisMsdDeCaE);

   SetValidHistogram(kTRUE);
}

//_____________________________________________________________________________
//! Action
Bool_t TAGactNtuGlbTrackS::Action()
{
   // GLB
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();
   pNtuTrack->Clear();

   if(FootDebugLevel(1))
      printf("\nNext event %d\n", gTAGroot->CurrentEventNumber());
   
   FindTracks();
   
   if(FootDebugLevel(2)) {
      printf(" %d tracks found\n", pNtuTrack->GetTracksN());
      for (Int_t i = 0; i < pNtuTrack->GetTracksN(); ++i) {
         TAGtrack* track = pNtuTrack->GetTrack(i);
      }
   }
   
   fpNtuTrack->SetBit(kValid);
   
   return true;
}

//_____________________________________________________________________________
//! Check Vertex
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
//! Find global tracks
Bool_t TAGactNtuGlbTrackS::FindTracks()
{
   if (!CheckVtx()) return false;
   
   // get containers
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();
   
   Int_t nTracks = fVtVertex->GetTracksN();
   
   for(Int_t i = 0; i < nTracks; ++i) {	  // loop over VTX tracks
      
      if(FootDebugLevel(1))
         printf("Next Track %d\n", i);

      TAVTtrack* vtTrack = fVtVertex->GetTrack(i);
      TAGtrack* track    = FillVtxTracks(vtTrack);
   
      if (TAGrecoManager::GetPar()->IncludeIT())
         FindItrCluster(track);

      if (TAGrecoManager::GetPar()->IncludeMSD())
         FindMsdCluster(track);

      if (TAGrecoManager::GetPar()->IncludeTW())
         FindTwCluster(track);
      
      if (TAGrecoManager::GetPar()->IncludeCA())
         FindCaCluster(track);

      // compute mass
      ComputeMass(track);
      
      // Add track
      track->SetTrackId(pNtuTrack->GetTracksN()-1);
      pNtuTrack->NewTrack(*track);
      if (ValidHistogram())
         FillHistogramm(track);
      delete track;
   }
   
   if (ValidHistogram())
      FillHistogramm();
   
   return true;
}


//_____________________________________________________________________________
//! Fill MC track id
//!
//! \param[in] cluster a given cluster
//! \param[in] point related global point
void TAGactNtuGlbTrackS::FillMcTrackId(TAGcluster* cluster, TAGpoint* point)
{
   for (Int_t k = 0; k < cluster->GetMcTracksN(); ++k) {
      Int_t idx = cluster->GetMcTrackIdx(k);
      point->AddMcTrackIdx(idx);
   }
}

//_____________________________________________________________________________
//! Fill global track with VTX track
TAGtrack* TAGactNtuGlbTrackS::FillVtxTracks(TAVTtrack* vtTrack)
{
   TAGtrack* track   = new TAGtrack();
   Int_t nClus = vtTrack->GetClustersN();
   
   for (Int_t i = 0; i < nClus; ++i) {
      TAVTcluster* cluster = (TAVTcluster*)vtTrack->GetCluster(i);
      TVector3 posG = cluster->GetPositionG();
      TVector3 errG = cluster->GetPosErrorG();
      
      // from VT local to FOOT global
      posG = fpFootGeo->FromVTLocalToGlobal(posG);
      
      TAGpoint* point = track->AddPoint(TAVTparGeo::GetBaseName(), posG, errG);
      point->SetDeviceType(TAGgeoTrafo::GetDeviceType(TAVTparGeo::GetBaseName()));
      point->SetSensorIdx(cluster->GetSensorIdx());
      point->SetClusterIdx(cluster->GetClusterIdx());
      FillMcTrackId(cluster, point);
   }
   
   UpdateParam(track);
   
   // first gues with TW
   TAVTparGeo* pGeoMapVt = (TAVTparGeo*) fpGeoMapVtx->Object();
   TAGparGeo* pGeoMapG   = (TAGparGeo*)  fpGeoMapG->Object();

   if (TAGrecoManager::GetPar()->IncludeTW()) {
      FindTwCluster(track, false);
      fPartZ = track->GetTwChargeZ();
   } else {
      fPartZ = 0;
   }
   
   if (fPartZ == 0) {
      if(FootDebugLevel(1))
         printf("No charge found, set Z = 1\n");
      fPartZ = 1;// taking worse case;
   }
   
   if (fPartZ == 1)
      fPartA = 1.;
   else
      fPartA = 2*fPartZ;
   
   // Compute particle after half target
   TString matTgt   = pGeoMapG->GetTargetPar().Material;
   Float_t thickTgt = pGeoMapG->GetTargetPar().Size[2]/2.; // in average
   fPartEnergy      = fEmProperties->GetEnergyLoss(matTgt, thickTgt, fBeamEnergyTarget, fPartA, fPartZ);
   Float_t thetaTgt = fEmProperties->GetSigmaTheta(matTgt, thickTgt, fBeamEnergyTarget, fPartA, fPartZ);
   
   // Compute particle after vertex
   Float_t thickVtx = fSensorThickVtx*nClus;
   TString matEpi   = pGeoMapVt->GetEpiMaterial();
   Float_t thetaVtx = fEmProperties->GetSigmaTheta(matEpi, thickVtx, fPartEnergy, fPartA, fPartZ);
   fPartEnergy      = fEmProperties->GetEnergyLoss(matEpi, thickVtx, fPartEnergy, fPartA, fPartZ);
   
   fPartSigmaTheta  = TMath::Sqrt(thetaTgt*thetaTgt + thetaVtx*thetaVtx);

   if(FootDebugLevel(1))
   for (Int_t i = 0; i < nClus; ++i) {
      TAVTcluster* cluster = (TAVTcluster*)vtTrack->GetCluster(i);
      printf("VTX Sensor %d charge %.0f\n", cluster->GetSensorIdx(), fPartZ);
   }

   if(FootDebugLevel(2))
      printf("VTX theta %g sensors %d charge %.0f\n", fPartSigmaTheta*TMath::RadToDeg(), nClus, fPartZ);
      
   if(FootDebugLevel(2)) {
      for (Int_t i = 0; i < nClus; ++i) {
         TAVTcluster* cluster = (TAVTcluster*)vtTrack->GetCluster(i);
         TVector3 posG = cluster->GetPositionG();
         TVector3 errG = cluster->GetPosErrorG();
         
         // from VT local to FOOT global
         posG = fpFootGeo->FromVTLocalToGlobal(posG);
         
         printf("VTX Sensor %d\n", cluster->GetSensorIdx());
         posG.Print();
         errG.Print();
         
         TVector3 inter = track->Intersection(posG.Z());
         inter.Print();
      }
   }
   
   return track;
}

//_____________________________________________________________________________
//! Find ITR cluster for a given global track
//!
//! \param[in] track a given track
void TAGactNtuGlbTrackS::FindItrCluster(TAGtrack* track)
{
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();
   TAITntuCluster* pNtuClus  = (TAITntuCluster*) fpNtuClusItr->Object();
   TAITparGeo*     pGeoMap   = (TAITparGeo*)     fpGeoMapItr->Object();

   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   Int_t nSensor = pGeoMap->GetSensorsN();

   Int_t addedCluster = 0;
   // Loop on all sensors to find a matching cluster in them
   for( Int_t iSensor = 0; iSensor < nSensor; ++iSensor) { // loop on sensors
      
      TClonesArray* list = pNtuClus->GetListOfClusters(iSensor);
      Int_t nClusters = pNtuClus->GetClustersN(iSensor);
      if (nClusters == 0) continue; //empty sensors
      
      
      // loop on all clusters of this sensor and keep the nearest one
      minDistance = fSearchClusDistItr;
      TAITcluster* bestCluster = 0x0;
      
      for( Int_t iClus = 0; iClus < nClusters; ++iClus ) { // loop on sensor clusters
         TAITcluster* aCluster = (TAITcluster*)list->At(iClus);
         
         if( aCluster->Found()) continue; // skip cluster already found
         
         // from IT local to FOOT global
         TVector3 posG = aCluster->GetPositionG();
         posG = fpFootGeo->FromITLocalToGlobal(posG);
         
         // track intersection with the sensor
         TVector3 inter = track->Intersection(posG.Z());
         
         // Compute diffusion
         Float_t length = posG.Z() - fLastPlaneVtx;
         Float_t thetaMin = track->GetTgtTheta() - 3.*fPartSigmaTheta; // 3 sigma search
         Float_t thetaMax = track->GetTgtTheta() + 3.*fPartSigmaTheta;
         Float_t disp = TMath::Abs(length*TMath::Tan(thetaMax) - length*TMath::Tan(thetaMin));
         
         minDistance = TMath::Sqrt(fSearchClusDistItr*fSearchClusDistItr + disp*disp);
         
         if(FootDebugLevel(2))
            printf("min distance %0.1f\n", minDistance*TAGgeoTrafo::CmToMu());
         
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
         addedCluster++;
         
         // from IT local to FOOT global
         TVector3 posG = bestCluster->GetPositionG();
         TVector3 errG = bestCluster->GetPosErrorG();

         posG = fpFootGeo->FromITLocalToGlobal(posG);
         
         TAGpoint* point = track->AddPoint(TAITparGeo::GetBaseName(), posG, errG);
         point->SetDeviceType(TAGgeoTrafo::GetDeviceType(TAITparGeo::GetBaseName()));
         point->SetSensorIdx(iSensor);
         point->SetClusterIdx(bestCluster->GetClusterIdx());
         FillMcTrackId(bestCluster, point);

         UpdateParam(track);
         
         if(FootDebugLevel(1))
            printf("ITR Sensor %d\n", iSensor);
      }
      
   } // end loop on sensors
   

   // Compute particle after inner tracker
   if (addedCluster == 0) addedCluster = 1;
   Float_t thick    = fSensorThickItr*addedCluster/TMath::Cos(track->GetTgtTheta());
   TString matEpi   = pGeoMap->GetEpiMaterial();
   Float_t theta    = fEmProperties->GetSigmaTheta(matEpi, thick, fPartEnergy, fPartA, fPartZ);
   fPartEnergy      = fEmProperties->GetEnergyLoss(matEpi, thick, fPartEnergy, fPartA, fPartZ);
   fPartSigmaTheta  = TMath::Sqrt(fPartSigmaTheta*fPartSigmaTheta + theta*theta);

   if(FootDebugLevel(2))
      printf("ITR theta %g sensors %d\n", fPartSigmaTheta*TMath::RadToDeg(), addedCluster);
}

//_____________________________________________________________________________
//! Find MSD cluster for a given global track
//!
//! \param[in] track a given track
void TAGactNtuGlbTrackS::FindMsdCluster(TAGtrack* track)
{
   TAGntuGlbTrack*  pNtuTrack = (TAGntuGlbTrack*)  fpNtuTrack->Object();
   TAMSDntuPoint*   pNtuRec   = (TAMSDntuPoint*)   fpNtuClusPoint->Object();
   TAMSDparGeo*     pGeoMap   = (TAMSDparGeo*)     fpGeoMapMsd->Object();
   
   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   Int_t nSensor = pGeoMap->GetSensorsN()/2.;
   
   TVector3 prePosG;

   // Loop on all sensors to find a matching cluster in them
   for( Int_t iStation = 0; iStation < nSensor; ++iStation) { // loop on sensors
      
      TClonesArray* list = pNtuRec->GetListOfPoints(iStation);
      Int_t nClusters = pNtuRec->GetPointsN(iStation);
      if (nClusters == 0) continue; //empty sensors
      
      
      // loop on all clusters of this sensor and keep the nearest one
      minDistance = fSearchClusDistMsd*4;

      TAMSDpoint* bestCluster = 0x0;
      
      for( Int_t iClus = 0; iClus < nClusters; ++iClus ) { // loop on sensor clusters
         TAMSDpoint* aCluster = (TAMSDpoint*)list->At(iClus);
         
         if( aCluster->Found()) continue; // skip cluster already found
         
         // from IT local to FOOT global
         TVector3 posG = aCluster->GetPositionG();
         posG = fpFootGeo->FromMSDLocalToGlobal(posG);
         
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
         
         // from MSD local to FOOT global
         TVector3 posG = bestCluster->GetPositionG();
         TVector3 errG = bestCluster->GetPosErrorG();
         
         posG = fpFootGeo->FromMSDLocalToGlobal(posG);
         
         TAGpoint* point = track->AddPoint(TAMSDparGeo::GetBaseName(), posG, errG);
         point->SetSensorIdx(iStation);
         point->SetClusterIdx(bestCluster->GetClusterIdx());
         point->SetEnergyLoss(bestCluster->GetEnergyLoss());
         FillMcTrackId(bestCluster, point);

         UpdateParam(track);
         
         // Compute particle after each plane
         Float_t thick    = fSensorThickMsd/TMath::Cos(track->GetTgtTheta());
         TString matEpi   = pGeoMap->GetEpiMaterial();
         Float_t theta    = fEmProperties->GetSigmaTheta(matEpi, thick, fPartEnergy, fPartA, fPartZ);
         fPartEnergy      = fEmProperties->GetEnergyLoss(matEpi, thick, fPartEnergy, fPartA, fPartZ);
         fPartSigmaTheta  = TMath::Sqrt(fPartSigmaTheta*fPartSigmaTheta + theta*theta);
         
         if(FootDebugLevel(2))
            printf("MSD theta %g sensor %d\n", fPartSigmaTheta*TMath::RadToDeg(), iStation);
         
         // Compute diffusion
         Float_t lastPlanePos;
         if (iStation == 0)
            lastPlanePos = fLastPlaneItr;
         else {
            lastPlanePos = pGeoMap->GetLayerPosZ(iStation-1);
            lastPlanePos = fpFootGeo->FromMSDLocalToGlobal(TVector3(0,0,lastPlanePos))[2];
         }
         
         Float_t length = posG.Z() - lastPlanePos;
         Float_t thetaMin = track->GetTgtTheta() - 3.*fPartSigmaTheta;
         Float_t thetaMax = track->GetTgtTheta() + 3.*fPartSigmaTheta;
         Float_t disp = TMath::Abs(length*TMath::Tan(thetaMax) - length*TMath::Tan(thetaMin));
         
         minDistance = TMath::Sqrt(fSearchClusDistMsd*fSearchClusDistMsd + disp*disp);
         
         if(FootDebugLevel(2))
            printf("min distance %0.1f\n", minDistance*TAGgeoTrafo::CmToMu());
         
         prePosG = posG;
      }
      
   } // end loop on sensors
   
}

//_____________________________________________________________________________
//! Find TW cluster for a given global track
//!
//! \param[in] track a given track
void TAGactNtuGlbTrackS::FindTwCluster(TAGtrack* track, Bool_t update)
{
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();
   TATWntuPoint*   pNtuRec   = (TATWntuPoint*)   fpNtuRecTw->Object();
   TATWparGeo*     pGeoMap   = (TATWparGeo*)     fpGeoMapTof->Object();

   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   
   // loop on all clusters and keep the nearest one
   Double_t  searchDist = fSearchClusDistTof;

   if (!update)
      searchDist = fSearchClusDistTof*2;
      
   TATWpoint* bestCluster = 0x0;
   Int_t nClusters = pNtuRec->GetPointsN();

   for( Int_t iClus = 0; iClus < nClusters; ++iClus ) { // loop on sensor clusters
      TATWpoint* aCluster =  pNtuRec->GetPoint(iClus);
      
      if( aCluster->Found()) continue; // skip cluster already found
      
      // from TW local to FOOT global
      TVector3 posG = aCluster->GetPositionG();
      posG = fpFootGeo->FromTWLocalToGlobal(posG);
      
      // Compute diffusion
      Float_t length = posG.Z() - fLastPlaneMsd;
      Float_t thetaMin = track->GetTgtTheta() - 3.*fPartSigmaTheta;
      Float_t thetaMax = track->GetTgtTheta() + 3.*fPartSigmaTheta;
      Float_t disp = TMath::Abs(length*TMath::Tan(thetaMax) - length*TMath::Tan(thetaMin));

      minDistance = TMath::Sqrt(searchDist*searchDist + disp*disp);
      
      if(FootDebugLevel(2))
         printf("min distance %0.1f\n", minDistance*TAGgeoTrafo::CmToMu());
      
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
      
      Float_t Z = bestCluster->GetChargeZ();
      track->SetTwChargeZ(Z);
      
      if (update) {
         // from IT local to FOOT global
         TVector3 posG = bestCluster->GetPositionG();
         TVector3 errG = bestCluster->GetPosErrorG();
      
         posG = fpFootGeo->FromTWLocalToGlobal(posG);
         
         TAGpoint* point = track->AddPoint(TATWparGeo::GetBaseName(), posG, errG);
         point->SetDeviceType(TAGgeoTrafo::GetDeviceType(TATWparGeo::GetBaseName()));
         point->SetSensorIdx(0);
         point->SetClusterIdx(bestCluster->GetClusterIdx());
         point->SetEnergyLoss(bestCluster->GetEnergyLoss());
         FillMcTrackId(bestCluster, point);

         Float_t tof = bestCluster->GetMeanTof();
         track->SetTwTof(tof);
   
         bestCluster->SetFound();
         UpdateParam(track);

         if(FootDebugLevel(1))
            printf("TW\n");
      }
   }
   
   // Compute particle after ToF Wall
   Float_t thick    = fWallThickTw/TMath::Cos(track->GetTgtTheta());
   TString mat      = pGeoMap->GetBarMat();
   Float_t theta    = fEmProperties->GetSigmaTheta(mat, thick, fPartEnergy, fPartA, fPartZ);
   fPartEnergy      = fEmProperties->GetEnergyLoss(mat, thick, fPartEnergy, fPartA, fPartZ);
   fPartSigmaTheta  = TMath::Sqrt(fPartSigmaTheta*fPartSigmaTheta + theta*theta);
}

//_____________________________________________________________________________
//! Find CAL cluster for a given global track
//!
//! \param[in] track a given track
void TAGactNtuGlbTrackS::FindCaCluster(TAGtrack* track)
{
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();
   TACAntuCluster* pNtuClus  = (TACAntuCluster*) fpNtuClusCa->Object();

   Double_t minDistance  = 1.e9;
   Double_t aDistance;
   
   // loop on all clusters and keep the nearest one
   Double_t  searchDist = fSearchClusDistTof*4;
   
   TACAcluster* bestCluster = 0x0;
   Int_t nClusters = pNtuClus->GetClustersN();
   
   for( Int_t iClus = 0; iClus < nClusters; ++iClus ) { // loop on sensor clusters
      TACAcluster* aCluster =  pNtuClus->GetCluster(iClus);
      
      if( aCluster->Found()) continue; // skip cluster already found
      
      // from TW local to FOOT global
      TVector3 posG = aCluster->GetPositionG();
      posG = fpFootGeo->FromCALocalToGlobal(posG);
      
      minDistance = searchDist;
      
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
      
      // from CA local to FOOT global
      TVector3 posG = bestCluster->GetPositionG();
      TVector3 errG = bestCluster->GetPosErrorG();
         
      posG = fpFootGeo->FromCALocalToGlobal(posG);
         
      TAGpoint* point = track->AddPoint(TACAparGeo::GetBaseName(), posG, errG);
      point->SetDeviceType(TAGgeoTrafo::GetDeviceType(TACAparGeo::GetBaseName()));
      point->SetSensorIdx(0);
      point->SetClusterIdx(bestCluster->GetClusterIdx());
      point->SetEnergyLoss(bestCluster->GetEnergy());
      FillMcTrackId(bestCluster, point);

      Float_t Ek = bestCluster->GetEnergy();
      track->SetFitEnergy(Ek*TAGgeoTrafo::MevToGev());
      
      bestCluster->SetFound();
      UpdateParam(track);
         
      if(FootDebugLevel(1))
         printf("CA\n");
   }
}

//------------------------------------------+-----------------------------------
//! Compute mass from track
//!
//! \param[in] track a given track
void TAGactNtuGlbTrackS::ComputeMass(TAGtrack* track)
{
   Float_t Z = track->GetTwChargeZ();
   if (Z < 1) return;

   // compute beta
   Double_t tof    = track->GetTwTof();
   TVector3 posTw  = fpFootGeo->GetTWCenter();
   TVector3 pos    = track->Intersection(posTw[2]);
   
   Double_t length = pos.Mag();
   
   Double_t beta   = length/tof;
   beta /= TAGgeoTrafo::GetLightVelocity(); //cm/ns
   
   // compute mass
   Double_t Ekin = track->GetFitEnergy(); // should come from Calo
  // Ekin += fPartEnergy; // add energy loss (neglecting loss in air)
   Double_t gamma = 1./TMath::Sqrt(1-beta*beta);
   Double_t mass  = Ekin/(gamma-1);
   track->SetMass(mass);
   
   if(FootDebugLevel(1))
      printf("Charge %.0f Mass %f\n", Z, mass);
}

//------------------------------------------+-----------------------------------
//! Fill histogram from track
//!
//! \param[in] track a given track
void TAGactNtuGlbTrackS::FillHistogramm(TAGtrack* track)
{
   Float_t twEnergyLoss  = -1.;
   Float_t msdEnergyLoss =  0.;
   Int_t   msdSensorsN   =  0;
   Float_t caEnergyRes   = -1.;

   fpHisTheta->Fill(track->GetTgtTheta());
   fpHisPhi->Fill(track->GetTgtPhi());
   
   fpHisTrackClus->Fill(track->GetPointsN());
   for (Int_t i = 0; i < track->GetPointsN(); ++i) {
      
      Int_t offset = 0;
      TAGpoint* cluster = track->GetPoint(i);
      Int_t idx = cluster->GetSensorIdx();
      TString devName = cluster->GetDevName();
      
      fpHisClusSensor->Fill(idx+1);
      
      Float_t posZ       = cluster->GetPositionG()[2];
      TVector3 impact    = track->Intersection(posZ);
      
      if (devName.Contains(TAVTparGeo::GetBaseName()))
         offset = 0;
      
      if (devName.Contains(TAITparGeo::GetBaseName()))
         offset = fOffsetItr;;
      
      if (devName.Contains(TAMSDparGeo::GetBaseName())) {
         offset = fOffsetMsd;
         msdEnergyLoss += cluster->GetEnergyLoss();
         msdSensorsN++;
      }
      
      if (devName.Contains(TATWparGeo::GetBaseName())) {
         offset = fOffsetTof;
         twEnergyLoss = cluster->GetEnergyLoss();
      }
      
      if (devName.Contains(TACAparGeo::GetBaseName())) {
         caEnergyRes = cluster->GetEnergyLoss();
      }
      
      if (track->GetPointsN() == 7) {
         fpHisResTrkTotX->Fill(impact[0]-cluster->GetPositionG()[0]);
         fpHisResTrkTotY->Fill(impact[1]-cluster->GetPositionG()[1]);
      }
      fpHisResTotX->Fill(impact[0]-cluster->GetPositionG()[0]);
      fpHisResTotY->Fill(impact[1]-cluster->GetPositionG()[1]);
      fpHisResX[idx+offset]->Fill(impact[0]-cluster->GetPositionG()[0]);
      fpHisResY[idx+offset]->Fill(impact[1]-cluster->GetPositionG()[1]);
   }
   
   if (twEnergyLoss != -1 && caEnergyRes != -1)
      fpHisTwDeCaE->Fill(caEnergyRes, twEnergyLoss);
   
   if (msdSensorsN > 0 && caEnergyRes != -1) {
      msdEnergyLoss /= float(msdSensorsN);
      fpHisMsdDeCaE->Fill(caEnergyRes, msdEnergyLoss);
   }

}

//_____________________________________________________________________________
//! Fill histograms
void TAGactNtuGlbTrackS::FillHistogramm()
{
   TAGntuGlbTrack* pNtuTrack = (TAGntuGlbTrack*) fpNtuTrack->Object();

   fpHisTrackEvt->Fill(pNtuTrack->GetTracksN());
   if (pNtuTrack->GetTracksN() == 0)
      fpHisClusSensor->Fill(0);
}

//------------------------------------------+-----------------------------------
//! Update track parameters
//!
//! \param[in] track a given track
void TAGactNtuGlbTrackS::UpdateParam(TAGtrack* track)
{
   TVector3 lineOrigin = track->GetTgtPosition();  // origin in the tracker system
   TVector3 lineSlope  = track->GetTgtDirection();   // slope along z-axis in tracker system
   
   Int_t nClusters = track->GetPointsN();
   
   Double_t x, dx;
   Double_t y, dy;
   Double_t z;
   
   vector<double> xData, dxData;
   vector<double> yData, dyData;
   vector<double> zxData, zyData;
   vector<double> res;

   for (Int_t i = 0; i < nClusters; ++i) {
      TAGpoint* cluster = track->GetPoint(i);
      x  = cluster->GetMeasPosition()(0);
      y  = cluster->GetMeasPosition()(1);
      z  = cluster->GetMeasPosition()(2);
      dx = cluster->GetMeasPosError()(0);
      dy = cluster->GetMeasPosError()(1);
  
     
      zxData.push_back(z);
      xData.push_back(x);
      if (dx < 1e-4) dx = 1;
      dxData.push_back(dx);
      
      zyData.push_back(z);
      yData.push_back(y);
      if (dy < 1e-4) dy = 1;
      dyData.push_back(dy);
      
   }
   
   res = GetLinearFit(zxData, xData, dxData);
   (lineOrigin)(0) = res[0];
   (lineSlope)(0)  = res[1];

   
   res = GetLinearFit(zyData, yData, dyData);
   (lineOrigin)(1) = res[0];
   (lineSlope)(1)  = res[1];
   
   lineSlope[2] = 1.;
   track->SetTgtPosition(lineOrigin);
   track->SetTgtDirection(lineSlope);
}

//------------------------------------------+-----------------------------------
//! Make a linear fit
//!
//! \param[in] z Z-position vector
//! \param[in] x X-position vector
//! \param[in] dx X-position error vector
//! \return parameter vector (slope/offset)
vector<double> TAGactNtuGlbTrackS::GetLinearFit(const vector<double>& z, const vector<double>& x, const vector<double>& dx)
{
   double zzSum = 0, zxSum = 0, slope, intercept;
   double zMean = 0, xMean = 0, wSum = 0;
   
   for (long i = 0; i < z.size(); i++) {
      double w = 1./(dx[i]*dx[i]);
      zMean += w*z[i];
      xMean += w*x[i];
      wSum += 1./(dx[i]*dx[i]);
   }
   
   zMean /= wSum;
   xMean /= wSum;
   
   for (long i = 0; i < z.size(); i++) {
      double w = 1./(dx[i]*dx[i]);
      zxSum += (x[i] - xMean)*(z[i] - zMean)*w;
      zzSum += (z[i] - zMean)*(z[i] - zMean)*w;
   }
   
   slope = zxSum/zzSum;
   intercept = xMean - slope*zMean;
   
   vector<double> res;
   res.push_back(intercept);
   res.push_back(slope);
   
   return res;
}
