/*!
 \file
 \brief   Implementation of TACAactNtuCluster.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TRandom3.h"

#include "TAGgeoTrafo.hxx"
#include "TACAparGeo.hxx"
#include "TACAparGeo.hxx"
#include "TACAntuHit.hxx"
#include "TACAntuCluster.hxx"
#include "TACAactNtuCluster.hxx"

#include "TATWntuPoint.hxx"

/*!
 \class TACAactNtuCluster
 \brief NTuplizer for calorimeter clusters. **
 */

ClassImp(TACAactNtuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAactNtuCluster::TACAactNtuCluster(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGdataDsc* pTwPoint)
 : TAGactNtuCluster2D(name, "TACAactNtuCluster - NTuplize cluster"),
   fpNtuRaw(pNtuRaw),
   fpNtuClus(pNtuClus),
   fpConfig(pConfig),
   fpGeoMap(pGeoMap),
   fpNtuTwPoint(pTwPoint),
//   fCurrentPosition(0., 0., 0.),
//   fCurrentPosError(0., 0., 0.),
   fClustersN(0)
{
   AddDataIn(pNtuRaw,   "TACAntuHit");
   AddDataOut(pNtuClus, "TACAntuCluster");
   AddPara(pGeoMap,     "TACAparGeo");
   //AddPara(pConfig, "TACAparConf");

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
   fpHisHitTot = new TH1F("caClusHitsTot", "Calorimeter - Total # hits per cluster", 25, 0., 25);
   AddHistogram(fpHisHitTot);

   fpHisChargeTot = new TH1F("caClusChargeTot", "Calorimeter - Total charge per cluster", 1000, 0., 4000);
   AddHistogram(fpHisChargeTot);

   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();
   fpHisClusMap = new TH2F("caClusMapTot", "Calorimeter - clusters map",
                           100, -pGeoMap->GetCaloSize()[0]/2., pGeoMap->GetCaloSize()[0]/2.,
                           100, -pGeoMap->GetCaloSize()[1]/2., pGeoMap->GetCaloSize()[1]/2.);
   fpHisClusMap->SetMarkerStyle(20);
   fpHisClusMap->SetMarkerSize(.6);
   fpHisClusMap->SetMarkerColor(1);
   fpHisClusMap->SetStats(kFALSE);
   AddHistogram(fpHisClusMap);

   fpHisResTwMag = new TH1F("caResTwMag", "Calorimeter - Minimal distance with TW points",
                          200, -pGeoMap->GetCrystalWidthFront()*2., pGeoMap->GetCrystalWidthFront()*4.);
   AddHistogram(fpHisResTwMag);

   fpHisHitTwMatch = new TH1F("caTwMatch", "Calorimeter - Number of matched hits with TW points", 2, 0, 2);
   AddHistogram(fpHisHitTwMatch);

   fpHisTwDeCaE = new TH2F("caTwDeCaE", "Calorimeter TW-deltaE vs CA-E", 300, 0, 3000,  100, 0, 200);
   fpHisClusMap->SetStats(kFALSE);
   AddHistogram(fpHisTwDeCaE);

   SetValidHistogram(kTRUE);

   return;
}

//______________________________________________________________________________
//
Bool_t TACAactNtuCluster::Action()
{

   fpNtuHit  = (TACAntuHit*) fpNtuRaw->Object();

   if (fpNtuHit->GetHitsN() == 0) {
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
   //TACAntuHit* pNtuHit  = (TACAntuHit*) fpNtuRaw->Object();

   // fill maps for cluster
   for (Int_t i = 0; i < fpNtuHit->GetHitsN(); i++) { // loop over hit crystals
      TACAhit* hit = fpNtuHit->GetHit(i);
      if(!hit->IsValid()) continue;
      Int_t id   = hit->GetCrystalId();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      TAGactNtuCluster2D::FillMaps(line, col, i);
   }
}

//______________________________________________________________________________
//! Shape cluster by region growing algorithm
//! Condition to grow: hit charge should be less than seed charge
//!
//! \param[in] numClus cluster number
//! \param[in] IndX index in X
//! \param[in] IndY index in Y
//! \param[in] seedCharge charge of previous seed
Bool_t TACAactNtuCluster::ShapeCluster(Int_t numClus, Int_t IndX, Int_t IndY, double seedCharge)
{
   Int_t idx = IndY*fDimX+IndX;
   if ( fPixelMap.count(idx) == 0 ) return false; // empty place
   if ( fFlagMap[idx] != -1 ) return false; // already flagged

   int index = fIndexMap[idx];
   TAGobject* pixel = GetHitObject(index);
   TACAhit* hit = (TACAhit*)pixel;
   double charge = hit->GetCharge();

   //if( charge < fChargeThreshold ) return false;

   //if( charge > fFactor * seedCharge ) return false;
   if( charge > seedCharge ) return false;

   fFlagMap[idx] = numClus;
   pixel->SetFound(true);

   
   for(Int_t i = -1; i <= 1 ; ++i)
      if (CheckLine(IndX+i)) {
         ShapeCluster(numClus, IndX+i, IndY, charge);
      }
   
   for(Int_t j = -1; j <= 1 ; ++j)
      if (CheckCol(IndY+j)) {
         ShapeCluster(numClus, IndX, IndY+j, charge);
      }
   
   return true;
}

//______________________________________________________________________________
//! Found all clusters. Starting seed from local maximum 
void TACAactNtuCluster::SearchCluster()
{
   fClustersN = 0;

   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();

   int nCry = pGeoMap->GetCrystalsN();

   // Search for cluster
   int loopCounter = 0;
   while (true) {
      double maxCharge = 0;
      int indexSeed = -1;
      for (Int_t i = 0; i < fpNtuHit->GetHitsN(); ++i) { // loop over hit crystals
         TACAhit* hit = fpNtuHit->GetHit(i);
         if (!hit->IsValid()) continue;
         if (hit->Found()) continue;
         double charge = hit->GetCharge();
         if (charge > maxCharge) {
            maxCharge = charge;
            indexSeed = i;
         }
      }

      if (indexSeed == -1) break; // maximum not found, not more clusters

      TACAhit* hit = fpNtuHit->GetHit(indexSeed);

      Int_t id   = hit->GetCrystalId();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      // Find cluster by region growing
      if ( ShapeCluster(fClustersN, col, line, hit->GetCharge() + 0.1) ) // +0.1 to meet condition on the seed
         fClustersN++;   
      else 
         break;   

      if ( ++loopCounter > nCry ) { // this should not happen (we can not have more clusters than crystals)
         Error("TACAactNtuCluster::SearchCluster", 
               "We got infinite loop!! Getting out.. num. cluster: %d, Event: %d",
               fClustersN, gTAGroot->CurrentEventId().EventNumber());
         break;
      }
   }

   if(FootDebugLevel(2))
     cout << "CA - Found : " << fClustersN << " clusters on event: " << gTAGroot->CurrentEventId().EventNumber() << endl;

   /*
   for (Int_t i = 0; i < fpNtuHit->GetHitsN(); ++i) { // loop over hit crystals
      TACAhit* hit = fpNtuHit->GetHit(i);

      if (hit->Found()) continue;
      if (!hit->IsValid()) continue;

      Int_t id   = hit->GetCrystalId();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      // loop over lines & columns
      if ( ShapeCluster(fClustersN, line, col) )
         fClustersN++;

   }
   */
}

//______________________________________________________________________________
//! Get Hit object in list
//!
//! \param[in] idx index on Hit list
TAGobject*  TACAactNtuCluster::GetHitObject(Int_t idx) const
{
   if (idx >= 0 && idx <  fpNtuHit->GetHitsN() )
      return (TAGobject*)fpNtuHit->GetHit(idx);
   else {
      Error("GetHitObject()", "Error in index %d (max: %d)", idx, fpNtuHit->GetHitsN()-1);
      return 0x0;
   }
}


//______________________________________________________________________________
//
Bool_t TACAactNtuCluster::ApplyCuts(TACAcluster* cluster)
{
  // TACAparConf* pConfig = (TACAparConf*) fpConfig->Object();

  // TClonesArray* list = cluster->GetListOfHits();
  // Int_t  entries = list->GetEntries();

  // cuts on crystals in cluster
  // if(entries < pConfig->GetSensorPar(cluster->GetPlaneNumber()).MinNofCrystalsInCluster ||
  //    entries > pConfig->GetSensorPar(cluster->GetPlaneNumber()).MaxNofCrystalsInCluster)
  //    return kFALSE;

   return kTRUE;
}

//______________________________________________________________________________
//
Bool_t TACAactNtuCluster::CreateClusters()
{
   TACAntuCluster* pNtuClus = (TACAntuCluster*) fpNtuClus->Object();
   TACAparGeo*     pGeoMap  = (TACAparGeo*)     fpGeoMap->Object();

   TACAcluster* cluster = 0x0;

   // create clusters
   for (Int_t i = 0; i< fClustersN; ++i)
      pNtuClus->NewCluster();  //create a TClonesArray of clusters

   for (Int_t i = 0; i < fpNtuHit->GetHitsN(); ++i) {
      TACAhit* hit = fpNtuHit->GetHit(i);
      if (hit == 0x0) continue;
      if (!hit->IsValid()) continue;
      Int_t id = hit->GetCrystalId();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);
      if(!CheckLine(line)) continue;
      if(!CheckCol(col)) continue;

      Int_t clusterN = GetClusterNumber(line,col);  //cluster ID
      if ( clusterN != -1 ) {  //if cluster exist do:
         cluster = pNtuClus->GetCluster(clusterN);  //get empty cluster from TClonesArray with "clusterN" ID
         cluster->AddHit(hit); //fill that element of TClonesArray with "hit"
      }
   }

   // Compute position and fill clusters info
   cluster = 0x0;

   //Fill histograms
   for (Int_t i = 0; i < pNtuClus->GetClustersN(); ++i) {
      cluster = pNtuClus->GetCluster(i);
      FillClusterInfo(cluster);
   }

   // Remove no valid cluster
   for (Int_t i = pNtuClus->GetClustersN()-1; i >= 0; --i) {
      cluster = pNtuClus->GetCluster(i);
      cluster->SetDeviceType(TAGgeoTrafo::GetDeviceType(TACAparGeo::GetBaseName()));
      if (!cluster->IsValid())
         pNtuClus->GetListOfClusters()->Remove(cluster);
   }

   pNtuClus->GetListOfClusters()->Compress();

   if (pNtuClus->GetClustersN())
      return true;

   return false;

}

///_____________________________________________________________________________
//! Compute the cluster centroid position
//!
//! \param[in] cluster 
void TACAactNtuCluster::ComputePosition(TACAcluster* cluster)
{
   //TACAntuHit* pNtuHit  = (TACAntuHit*) fpNtuRaw->Object();

   if (cluster->GetListOfHits() == 0) return;

   TVector3 posWeightSum, posWeightSum2, posWeight;
   TVector3 pos, posErr;
   posWeightSum.SetXYZ( 0., 0., 0.);
   posWeightSum2.SetXYZ( 0., 0., 0.);

   double clusterPulseSum = 0.;
   Float_t maxCharge = 0;
   Int_t iMax = -1;

   for (Int_t i = 0; i < cluster->GetHitsN(); ++i) {
      TACAhit* hit = cluster->GetHit(i);
      posWeight.SetXYZ(hit->GetPosition()(0)*hit->GetCharge(), hit->GetPosition()(1)*hit->GetCharge(), hit->GetPosition()(2)*hit->GetCharge());
      posWeightSum  += posWeight;
      clusterPulseSum  += hit->GetCharge();

      if (hit->GetCharge() > maxCharge) {
         maxCharge = hit->GetCharge();
         iMax      = i;
      }
   }
   pos = posWeightSum*(1./clusterPulseSum);

   for (Int_t i = 0; i < cluster->GetHitsN(); ++i) {
     TACAhit* hit = cluster->GetHit(i);
     posWeightSum2.SetXYZ(
         hit->GetCharge()*(hit->GetPosition()(0)-(pos)(0))*(hit->GetPosition()(0)-(pos)(0)),
         hit->GetCharge()*(hit->GetPosition()(1)-(pos)(1))*(hit->GetPosition()(1)-(pos)(1)),
         0);
     posErr += posWeightSum2;
   }
   
   posErr *= 1./clusterPulseSum;

   //fCurrentPosition.SetXYZ(pos[0], pos[1], pos[2]);
   //fCurrentPosError.SetXYZ(TMath::Sqrt(posErr[0]), TMath::Sqrt(posErr[1]), 0);

   cluster->SetIndexSeed(iMax);
   cluster->SetPosition(pos);
   cluster->SetPosError(posErr);
   cluster->SetPositionG(pos);
   cluster->SetEnergy(clusterPulseSum);
}

//______________________________________________________________________________
//
void TACAactNtuCluster::FillClusterInfo(TACAcluster* cluster)
{
   ComputePosition(cluster);

   //if (ApplyCuts(cluster)) {
      if (fpNtuTwPoint)
         ComputeMinDist(cluster);

      // histograms
      if (ValidHistogram()) {
         if (cluster->GetHitsN() > 0) {
            fpHisHitTot->Fill(cluster->GetHitsN());
            fpHisChargeTot->Fill(cluster->GetEnergy());
            fpHisClusMap->Fill(cluster->GetPosition()[0], cluster->GetPosition()[1]);
         }
      }

      cluster->SetValid(true);
   //} else {
   //   cluster->SetValid(false);
   //}
}

//______________________________________________________________________________
//
void TACAactNtuCluster::ComputeMinDist(TACAcluster* cluster)
{
   TAGgeoTrafo* pFootGeo = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));

   TACAparGeo* pGeoMap = (TACAparGeo*) fpGeoMap->Object();
   Float_t width = pGeoMap->GetCrystalWidthFront()*2.;

   TVector3 posG = cluster->GetPositionG();
   Float_t min   = width;
   Int_t imin    = -1;
   TVector3 resMin;

   TATWntuPoint* pNtuPoint = (TATWntuPoint*) fpNtuTwPoint->Object();
   Int_t nPoints = pNtuPoint->GetPointsN();

   for (Int_t iPoint = 0; iPoint < nPoints; ++iPoint) {

      TATWpoint *point = pNtuPoint->GetPoint(iPoint);
      TVector3 posGtw = point->GetPosition();

      posGtw = pFootGeo->FromTWLocalToGlobal(posGtw);
      posG   = pFootGeo->FromCALocalToGlobal(posG);
      posGtw[2] = posG[2] = 0.;
      TVector3 res = posG-posGtw;
      Float_t diff = res.Mag();

      if (diff < min) {
         min = diff;
         imin = iPoint;
         resMin = res;
      }
   } //end loop on points

   if (imin != -1) {
      resMin[0]  += gRandom->Uniform(-1, 1);
      resMin[1]  += gRandom->Uniform(-1, 1);
      TATWpoint *point = pNtuPoint->GetPoint(imin);
      point->SetMatchCalIdx(cluster->GetClusterIdx());

      if (ValidHistogram()) {
         fpHisResTwMag->Fill(resMin.Mag());
         fpHisHitTwMatch->Fill(1);
         fpHisTwDeCaE->Fill(cluster->GetEnergy(), point->GetEnergyLoss());
      }
   } else
      if (ValidHistogram())
         fpHisHitTwMatch->Fill(0);

}
