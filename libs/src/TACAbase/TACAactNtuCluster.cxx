/*!
 \file TACAactNtuCluster.cxx
 \brief   Implementation of TACAactNtuCluster.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TRandom3.h"

#include "TAGgeoTrafo.hxx"
#include "TACAparGeo.hxx"
#include "TACAparCal.hxx"
#include "TACAntuHit.hxx"
#include "TACAntuCluster.hxx"
#include "TACAactNtuCluster.hxx"

#include "TATWntuPoint.hxx"


Float_t TACAactNtuCluster::fgChargeThreshold = 1;
Bool_t  TACAactNtuCluster::fgThresholdFlag   = true;

/*!
 \class TACAactNtuCluster
 \brief NTuplizer for calorimeter clusters. **
 */

//! Class Imp
ClassImp(TACAactNtuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit input container descriptor
//! \param[out] pNtuClus cluster output container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pTwPoint TW point input container descriptor
TACAactNtuCluster::TACAactNtuCluster(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus, TAGparaDsc* pGeoMap,  TAGparaDsc*  pCalib, TAGparaDsc* pConfig, TAGdataDsc* pTwPoint)
 : TAGactNtuCluster2D(name, "TACAactNtuCluster - NTuplize cluster"),
   fpNtuRaw(pNtuRaw),
   fpNtuClus(pNtuClus),
   fpConfig(pConfig),
   fpParCal(pCalib),
   fpGeoMap(pGeoMap),
   fpNtuTwPoint(pTwPoint),
   fClustersN(0)
{
   AddDataIn(pNtuRaw,   "TACAntuHit");
   AddDataOut(pNtuClus, "TACAntuCluster");
   AddPara(pGeoMap,     "TACAparGeo");
   if (pCalib)
      AddPara(pCalib,     "TACAparCal");
   //AddPara(pConfig, "TACAparConf");

   TACAparGeo* parGeo  = (TACAparGeo*) fpGeoMap->Object();
   fDimX = max(parGeo->GetMaxNumLines(), parGeo->GetMaxNumColumns());
   fDimY = max(parGeo->GetMaxNumLines(), parGeo->GetMaxNumColumns());
   SetupMaps(fDimY*fDimX);
   
   fpNtuHit  = (TACAntuHit*) fpNtuRaw->Object();
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

   fpHisChargeTot = new TH1F("caClusChargeTot", "Calorimeter - Total charge per TW matched cluster", 400, 0., 200);
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
//! Action
Bool_t TACAactNtuCluster::Action()
{
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
//! Fill maps
void TACAactNtuCluster::FillMaps()
{
   // Clear maps
   ClearMaps();

   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();

   // fill maps for cluster
   for (Int_t i = 0; i < fpNtuHit->GetHitsN(); i++) { // loop over hit crystals
      TACAhit* hit = fpNtuHit->GetHit(i);
      if(!hit->IsValid()) continue;
      Int_t id   = hit->GetCrystalId();
      double charge = hit->GetCharge();
      Int_t line = pGeoMap->GetCrystalLine(id);
      Int_t col  = pGeoMap->GetCrystalCol(id);

      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      if( charge < fgChargeThreshold && fgThresholdFlag) continue;

      TAGactNtuCluster2D::FillMaps(line, col, i);
   }
}


//______________________________________________________________________________
//! Found all clusters. Starting seed from local maximum 
void TACAactNtuCluster::SearchCluster()
{
   fClustersN = 0;
   
   TACAparGeo* pGeoMap  = (TACAparGeo*) fpGeoMap->Object();
   TACAntuHit* pNtuHit  = (TACAntuHit*) fpNtuRaw->Object();
   
   // Search for cluster
   for (Int_t i = 0; i < pNtuHit->GetHitsN(); ++i) { // loop over hit crystals
      TACAhit* hit = pNtuHit->GetHit(i);
      
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
   
   if(FootDebugLevel(2))
     cout << "CA - Found : " << fClustersN << " clusters on event: " << gTAGroot->CurrentEventId().EventNumber() << endl;

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
//! Apply cut for a given cluster
//!
//! \param[in] cluster a given cluster
Bool_t TACAactNtuCluster::ApplyCuts(TACAcluster* cluster)
{
  // TACAparConf* pConfig = (TACAparConf*) fpConfig->Object();

  // TClonesArray* list = cluster->GetListOfHits();
  // Int_t  entries = list->GetEntriesFast();

  // cuts on crystals in cluster
  // if(entries < pConfig->GetSensorPar(cluster->GetPlaneNumber()).MinNofCrystalsInCluster ||
  //    entries > pConfig->GetSensorPar(cluster->GetPlaneNumber()).MaxNofCrystalsInCluster)
  //    return kFALSE;

   return kTRUE;
}

//______________________________________________________________________________
//! Create clusters
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
//! \param[in] cluster a given cluster
void TACAactNtuCluster::ComputePosition(TACAcluster* cluster)
{
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

   cluster->SetIndexSeed(iMax);
   cluster->SetPosition(pos);
   cluster->SetPosError(posErr);
   cluster->SetPositionG(pos);
   cluster->SetEnergy(clusterPulseSum);
}

///_____________________________________________________________________________
//! Fill cluster informations
//!
//! \param[in] cluster a given cluster
void TACAactNtuCluster::FillClusterInfo(TACAcluster* cluster)
{
   if (ApplyCuts(cluster)) {
      if (fpNtuTwPoint) {
         ComputePosition(cluster);
         ComputeMinDist(cluster);
         if (fpParCal)
            CalibrateEnergy(cluster);
      }

      ComputePosition(cluster);

      // histograms
      if (ValidHistogram()) {
         if (cluster->GetHitsN() > 0 && cluster->IsTwMatched()) {
            fpHisHitTot->Fill(cluster->GetHitsN());
            fpHisChargeTot->Fill(cluster->GetEnergy());
            fpHisClusMap->Fill(cluster->GetPosition()[0], cluster->GetPosition()[1]);
         }
      }

      cluster->SetValid(true);
   } else {
      cluster->SetValid(false);
   }
}

///_____________________________________________________________________________
//! Compute minimum distance to a cluster
//!
//! \param[in] cluster a given cluster
void TACAactNtuCluster::ComputeMinDist(TACAcluster* cluster)
{
   TAGgeoTrafo* pFootGeo = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));

   TACAparGeo* pGeoMap = (TACAparGeo*) fpGeoMap->Object();
   Float_t width = pGeoMap->GetCrystalWidthFront()*2.;

   TVector3 posG = cluster->GetPositionG();
   Float_t min   = width;
   Int_t imin    = -1;
   fTwPointZ     = -1;
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
      fTwPointZ = point->GetChargeZ();

      if (ValidHistogram()) {
         fpHisResTwMag->Fill(resMin.Mag());
         fpHisHitTwMatch->Fill(1);
         fpHisTwDeCaE->Fill(cluster->GetEnergy(), point->GetEnergyLoss());
      }
      cluster->SetTwMatched(true);
   } else {
      if (ValidHistogram())
         fpHisHitTwMatch->Fill(0);
      cluster->SetTwMatched(false);
   }
}

///_____________________________________________________________________________
//! Compute minimum distance to a cluster
//!
//! \param[in] cluster a given cluster
void TACAactNtuCluster::CalibrateEnergy(TACAcluster* cluster)
{
   Int_t nhits = cluster->GetHitsN();
   
   for (Int_t i = 0; i < nhits; ++i) {
      TACAhit* hit = cluster->GetHit(i);
      Int_t crysId = hit->GetCrystalId();
      Float_t charge = hit->GetCharge();
      Double_t energy = 0;
      if (fTwPointZ != -1)
         energy = GetEnergy(charge, fTwPointZ);
      else
         energy = charge;
      
      hit->SetCharge(energy);
      if(FootDebugLevel(1))
         printf("%d %f\n", fTwPointZ, energy);
   }
}

//------------------------------------------+-----------------------------------
//! Get curve for fragments of different Z obtained as a ratio between the parameter curve for protons and the parameter curve for other He, C, O
//!
//! \param[in] p0 of exponential slope
//! \param[in] p1 of exponential slope
//! \param[in] p2 of exponential slope
//! \param[in] z particle charge (atomic number)
Double_t TACAactNtuCluster::GetZCurve(Double_t p0, Double_t  p1, Double_t p2, Int_t z)
{
   
   return p0 + p1*exp(-(z/p2));
   
   
   //calibration AValetti's analysis 22.12.22
   //return energy from ADC
}

//------------------------------------------+-----------------------------------
//! Get calibrated energy
//!
//! \param[in] rawenergy raw energy
//! \param[in] crysId crystal id
//! \param[in] z particle charge (atomic number)
Double_t TACAactNtuCluster::GetEnergy(Double_t rawenergy, Int_t z)
{
   TACAparCal* p_parcal = (TACAparCal*) fpParCal->Object();
   
   Double_t p0 = p_parcal->GetADC2EnergyParam(0);
   Double_t p1 = p_parcal->GetADC2EnergyParam(1);
   Double_t p2 = p_parcal->GetADC2EnergyParam(2);
   
   Double_t p3 = p_parcal->GetADC2EnergyParam(3);
   Double_t p4 = p_parcal->GetADC2EnergyParam(4);
   Double_t p5 = p_parcal->GetADC2EnergyParam(5);
   
   Double_t p6 = p_parcal->GetADC2EnergyParam(6);
   Double_t p7 = p_parcal->GetADC2EnergyParam(7);
   Double_t p8 = p_parcal->GetADC2EnergyParam(8);
   
   Double_t p9 = p_parcal->GetADC2EnergyParam(9);
   Double_t p10 = p_parcal->GetADC2EnergyParam(10);
   Double_t p11 = p_parcal->GetADC2EnergyParam(11);
   
   p0 = p0*GetZCurve(p3,p4,p5,z);
   p1 = p1*GetZCurve(p6,p7,p8,z);
   p2 = p2*GetZCurve(p9,p10,p11,z);
   
   Double_t ac = p0-p2*rawenergy;
   
   return (p1 * rawenergy + sqrt( p1 * p1 * rawenergy * rawenergy + 4 * ac ))/(2 * ac);
   
   
   //calibration AValetti's analysis 22.12.22
   //return energy from ADC
}
