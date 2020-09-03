/*!
 \file
 \version $Id: TACAactNtuCluster.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TACAactNtuCluster.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TACAparGeo.hxx"
//#include "TACAparConf.hxx"
#include "TACAntuRaw.hxx"
#include "TACAntuCluster.hxx"
#include "TACAactNtuCluster.hxx"

/*!
 \class TACAactNtuCluster
 \brief NTuplizer for calorimeter clusters. **
 */

ClassImp(TACAactNtuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAactNtuCluster::TACAactNtuCluster(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig)
 : TAGactNtuCluster2D(name, "TACAactNtuCluster - NTuplize cluster"),
   fpNtuRaw(pNtuRaw),
   fpNtuClus(pNtuClus),
   fpConfig(pConfig),
   fpGeoMap(pGeoMap),
   fCurrentPosition(0., 0., 0.),
   fCurrentPosError(0., 0., 0.),
   fListOfHits(0x0),
   fClustersN(0)

{
   AddDataIn(pNtuRaw,   "TACAntuRaw");
   AddDataOut(pNtuClus, "TACAntuCluster");
   AddPara(pGeoMap, "TACAparGeo");
  // AddPara(pConfig, "TACAparConf");
   
   TString tmp(name);
   fPrefix = tmp(0,2);
   
   fTitleDev = "Calorimeter";

   fDimY = 18;
   fDimX = 18;
   SetupMaps(fDimY*fDimX);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TACAactNtuCluster::~TACAactNtuCluster()
{
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TACAactNtuCluster::CreateHistogram()
{
   DeleteHistogram();
   fpHisHitTot = new TH1F(Form("%sClusHitsTot", fPrefix.Data()), Form("%s - Total # hits per cluster", fTitleDev.Data()), 25, 0., 25);
   AddHistogram(fpHisHitTot);

   fpHisChargeTot = new TH1F(Form("%sClusChargeTot", fPrefix.Data()), Form("%s - Total charge per cluster", fTitleDev.Data()), 1000, 0., 4000);
   AddHistogram(fpHisChargeTot);

   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();
   fpHisClusMap = new TH2F(Form("%sClusMapTot", fPrefix.Data()), Form("%s - clusters map", fTitleDev.Data()),
                           100, -pGeoMap->GetCaloSize()[0]/2., pGeoMap->GetCaloSize()[0]/2.,
                           100, -pGeoMap->GetCaloSize()[1]/2., pGeoMap->GetCaloSize()[1]/2.);
   fpHisClusMap->SetMarkerStyle(20);
   fpHisClusMap->SetMarkerSize(.6);
   fpHisClusMap->SetMarkerColor(1);
   fpHisClusMap->SetStats(kFALSE);
   AddHistogram(fpHisClusMap);
   
   SetValidHistogram(kTRUE);
   
   return;
}

//______________________________________________________________________________
//
Bool_t TACAactNtuCluster::Action()
{
   TACAntuRaw* pNtuHit  = (TACAntuRaw*) fpNtuRaw->Object();

   if (pNtuHit->GetHitsN() == 0) {
      fpNtuClus->SetBit(kValid);
      return true;
   }
   
   FillMaps();
   SearchCluster();
   Bool_t ok = CreateClusters();
   
   fpNtuClus->SetBit(kValid);

   return true;
   
}

//______________________________________________________________________________
//
void TACAactNtuCluster::FillMaps()
{
   // Clear maps
   ClearMaps();
   
   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();
   TACAntuRaw* pNtuHit  = (TACAntuRaw*) fpNtuRaw->Object();

   // fill maps for cluster
   for (Int_t i = 0; i < pNtuHit->GetHitsN(); i++) { // loop over hit crystals
      TACAntuHit* hit = pNtuHit->GetHit(i);
      Int_t id   = hit->GetCrystalId();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;
      
      TAGactNtuCluster2D::FillMaps(line, col, i);
   }
}

//______________________________________________________________________________
//
void TACAactNtuCluster::SearchCluster()
{
   fClustersN = 0;

   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();
   TACAntuRaw* pNtuHit  = (TACAntuRaw*) fpNtuRaw->Object();

   // Search for cluster
   for (Int_t i = 0; i < pNtuHit->GetHitsN(); ++i) { // loop over hit crystals
      TACAntuHit* hit = pNtuHit->GetHit(i);

      if (hit->Found()) continue;
      
      Int_t id   = hit->GetCrystalId();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      // loop over lines & columns
      if ( ShapeCluster(fClustersN, line, col) )
         fClustersN++;
      
   }
}

//______________________________________________________________________________
// Get object in list
TAGobject*  TACAactNtuCluster::GetHitObject(Int_t idx) const
{
   TACAntuRaw* pNtuHit  = (TACAntuRaw*) fpNtuRaw->Object();

   if (idx >= 0 && idx <  pNtuHit->GetHitsN() )
      return (TAGobject*)pNtuHit->GetHit(idx);

   else {
      Error("GetHitObject()", "Error in index %d (max: %d)", idx, pNtuHit->GetHitsN()-1);
      return 0x0;
   }
}


//______________________________________________________________________________
//
Bool_t TACAactNtuCluster::ApplyCuts(TACAcluster* cluster)
{
//   TACAparConf* pConfig = (TACAparConf*) fpConfig->Object();
//
//   TClonesArray* list = cluster->GetListOfHits();
//   Int_t  entries = list->GetEntries();
//
//   // cuts on crystals in cluster
//   if(entries < pConfig->GetSensorPar(cluster->GetPlaneNumber()).MinNofCrystalsInCluster ||
//      entries > pConfig->GetSensorPar(cluster->GetPlaneNumber()).MaxNofCrystalsInCluster)
//      return kFALSE;
   
   return kTRUE;
}

//______________________________________________________________________________
//
Bool_t TACAactNtuCluster::CreateClusters()
{
   TACAntuCluster* pNtuClus = (TACAntuCluster*) fpNtuClus->Object();
   TACAparGeo*     pGeoMap  = (TACAparGeo*)     fpGeoMap->Object();
   TACAntuRaw*     pNtuHit  = (TACAntuRaw*) fpNtuRaw->Object();

   TACAcluster* cluster = 0x0;
   
   // create clusters
   for (Int_t i = 0; i< fClustersN; ++i)
      pNtuClus->NewCluster();
   
   for (Int_t i = 0; i < pNtuHit->GetHitsN(); ++i) {
      TACAntuHit* hit = pNtuHit->GetHit(i);
      if (hit == 0x0) continue;
      Int_t id = hit->GetCrystalId();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);
      if(!CheckLine(line)) continue;
      if(!CheckCol(col)) continue;
      
      Int_t clusterN = GetClusterNumber(line,col);
      if ( clusterN != -1 ) {
         cluster = pNtuClus->GetCluster(clusterN);
         cluster->AddHit(hit);
      }
   }
   
   // Compute position and fill clusters info
   cluster = 0x0;
   
   for (Int_t i = 0; i < pNtuClus->GetClustersN(); ++i) {
      cluster = pNtuClus->GetCluster(i);
      FillClusterInfo(cluster);
   }
   
   // Remove no valid cluster
   for (Int_t i = pNtuClus->GetClustersN()-1; i >= 0; --i) {
      cluster = pNtuClus->GetCluster(i);
      if (!cluster->IsValid())
         pNtuClus->GetListOfClusters()->Remove(cluster);
   }
   
   pNtuClus->GetListOfClusters()->Compress();
   
   if (pNtuClus->GetClustersN())
      return true;
   
   return false;

}

//______________________________________________________________________________
//
void TACAactNtuCluster::ComputePosition()
{
   TACAntuRaw* pNtuHit  = (TACAntuRaw*) fpNtuRaw->Object();

   if (fListOfHits == 0) return;
   
   TVector3 tCorrection, tCorrection2, tCorTemp;
   TVector3 pos, posErr;
   tCorrection.SetXYZ( 0., 0., 0.);
   tCorrection2.SetXYZ( 0., 0., 0.);
   
   fClusterPulseSum = 0.;
   
   
   for (Int_t i = 0; i < fListOfHits->GetEntries(); ++i) {
      TACAntuHit* hit = (TACAntuHit*)fListOfHits->At(i);
      tCorTemp.SetXYZ(hit->GetPosition()(0)*hit->GetCharge(), hit->GetPosition()(1)*hit->GetCharge(), hit->GetPosition()(2)*hit->GetCharge());
      tCorrection  += tCorTemp;
      fClusterPulseSum  += hit->GetCharge();
	  }
   
   pos = tCorrection*(1./fClusterPulseSum);
   
   for (Int_t i = 0; i < fListOfHits->GetEntries(); ++i) {
      TACAntuHit* hit = (TACAntuHit*)fListOfHits->At(i);
	  tCorrection2.SetXYZ(hit->GetCharge()*(hit->GetPosition()(0)-(pos)(0))*(hit->GetPosition()(0)-(pos)(0)),
							hit->GetCharge()*(hit->GetPosition()(1)-(pos)(1))*(hit->GetPosition()(1)-(pos)(1)),
							0);
	  posErr += tCorrection2;
   }
   
   posErr *= 1./fClusterPulseSum;
   
   // for cluster with a single hit
   Float_t lim = 2.5e-7; // in cm !
   if (posErr(0) < lim) posErr(0) = lim; //(20/Sqrt(12)^2
   if (posErr(1) < lim) posErr(1) = lim; //(20/Sqrt(12)^2
   
   fCurrentPosition.SetXYZ(pos[0], pos[1], pos[2]);
   fCurrentPosError.SetXYZ(TMath::Sqrt(posErr[0]), TMath::Sqrt(posErr[1]), 0);
}

//______________________________________________________________________________
//
void TACAactNtuCluster::FillClusterInfo(TACAcluster* cluster)
{
   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();
   
   fListOfHits = cluster->GetListOfHits();
   ComputePosition();
   TVector3 posG = GetCurrentPosition();
   cluster->SetPositionG(posG);
   cluster->SetPosition(GetCurrentPosition());
   cluster->SetPosError(GetCurrentPosError());
   cluster->SetCharge(GetClusterPulseSum());
   
   if (ApplyCuts(cluster)) {
      // histogramms
      if (ValidHistogram()) {
         if (cluster->GetHitsN() > 0) {
            fpHisHitTot->Fill(cluster->GetHitsN());
            fpHisChargeTot->Fill(cluster->GetCharge());
            fpHisClusMap->Fill(cluster->GetPosition()[0], cluster->GetPosition()[1]);
         
         }
      }
      cluster->SetValid(true);
   } else {
      cluster->SetValid(false);
   }
}
