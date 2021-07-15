/*!
 \file
 \version $Id: TAMSDactNtuCluster.cxx $
 \brief   Implementation of TAMSDactNtuCluster.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDactNtuCluster.hxx"

/*!
 \class TAMSDactNtuCluster 
 \brief NTuplizer for MSD cluster. **
 */

ClassImp(TAMSDactNtuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMSDactNtuCluster::TAMSDactNtuCluster(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus, TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
  : TAGactNtuCluster1D(name, "TAMSDactNtuCluster - NTuplize cluster"),
    fpNtuRaw(pNtuRaw),
    fpNtuClus(pNtuClus),
    fpConfig(pConfig),
    fpGeoMap(pGeoMap),
    fCurrentPosition(0.),
    fCurrentPosError(0.),
    fListOfStrips(0x0),
    fClustersN(0)
{
  AddPara(pGeoMap, "TAMSDparGeo");
  AddPara(pConfig, "TAMSDparConf");
  AddDataIn(pNtuRaw,   "TAMSDntuHit");
  AddDataOut(pNtuClus, "TAMSDntuCluster");
  
  TAMSDparGeo* geoMap = (TAMSDparGeo*)fpGeoMap->Object();
  fDimX = geoMap->GetStripsN()+1;
  SetupMaps(fDimX);
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAMSDactNtuCluster::CreateHistogram()
{
  DeleteHistogram();
  
  TString prefix = "ms";
  TString titleDev = "Mirco Strip Detector";
  
  fpHisStripTot = new TH1F(Form("%sClusStripTot", prefix.Data()), Form("%s - Total # strips per clusters", titleDev.Data()), 25, 0., 25.);
  AddHistogram(fpHisStripTot);
  
  TAMSDparGeo* pGeoMap  = (TAMSDparGeo*) fpGeoMap->Object();
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisStrip[i] = new TH1F(Form("%sClusStrip%d",prefix.Data(), i+1), Form("%s - # strips per clusters for sensor %d", titleDev.Data(), i+1), 25, 0., 25);
    AddHistogram(fpHisStrip[i]);
  }
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisClusMap[i] = new TH1F(Form("%sClusMap%d", prefix.Data(), i+1), Form("%s - clusters map for sensor %d", titleDev.Data(), i+1),
			       pGeoMap->GetStripsN(), -pGeoMap->GetPitch()*pGeoMap->GetStripsN()/2., pGeoMap->GetPitch()*pGeoMap->GetStripsN()/2.);
    
    AddHistogram(fpHisClusMap[i]);
  }
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
     fpHisClusCharge[i] = new TH1F(Form("%sClusCharge%d",prefix.Data(), i+1), Form("%s - cluster charge for sensor %d", titleDev.Data(), i+1), 100, 0., 500);
     AddHistogram(fpHisClusCharge[i]);
  }
   
  fpHisClusChargeTot = new TH1F(Form("%sClusChargeTot",prefix.Data()), Form("%s - total cluster charge", titleDev.Data()), 100, 0., 500);
  AddHistogram(fpHisClusChargeTot);

  SetValidHistogram(kTRUE);
  return;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactNtuCluster::~TAMSDactNtuCluster()
{
}

//______________________________________________________________________________
//
Bool_t TAMSDactNtuCluster::Action()
{
  TAMSDntuHit* pNtuHit  = (TAMSDntuHit*) fpNtuRaw->Object();
  TAMSDparGeo* pGeoMap  = (TAMSDparGeo*) fpGeoMap->Object();
  Bool_t ok = true;
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fListOfStrips = pNtuHit->GetListOfStrips(i);
    if (fListOfStrips->GetEntries() == 0) continue;
    ok += FindClusters(i);
  }
  
  if(ok)
    fpNtuClus->SetBit(kValid);
  
  return ok;
}

//______________________________________________________________________________
//  
Bool_t TAMSDactNtuCluster::FindClusters(Int_t iSensor)
{
  // Algo taking from Virgile BEKAERT (ImaBio @ IPHC-Strasbourg)
  // Look in a iterative way to next neighbour
  
  FillMaps();
  SearchCluster();
  
  return CreateClusters(iSensor);
}

//______________________________________________________________________________
//
void TAMSDactNtuCluster::SearchCluster()
{
  fClustersN = 0;
  // Search for cluster
  
  for (Int_t iPix = 0; iPix < fListOfStrips->GetEntries(); ++iPix) { // loop over hit strips
    TAMSDhit* strip = (TAMSDhit*)fListOfStrips->At(iPix);
    if (strip->Found()) continue;
    
    Int_t stripId  = strip->GetStrip();
    if (!CheckLine(stripId)) continue;
    
    // loop over lines & columns
    if ( ShapeCluster(fClustersN, stripId) )
      fClustersN++;
  }
}

//______________________________________________________________________________
// Get object in list
TAGobject*  TAMSDactNtuCluster::GetHitObject(Int_t idx) const
{
  if (idx >= 0 && idx < GetListOfStrips()->GetEntries() )
    return (TAGobject*)GetListOfStrips()->At(idx);
  
  else {
    Error("GetHitObject()", "Error in index %d (max: %d)", idx, GetListOfStrips()->GetEntries()-1);
    return 0x0;
  }
}

//______________________________________________________________________________
//
void TAMSDactNtuCluster::FillMaps()
{
  
  // Clear maps
  ClearMaps();
  
  if (fListOfStrips->GetEntries() == 0) return;
  
  // fill maps for cluster
  for (Int_t i = 0; i < fListOfStrips->GetEntries(); i++) { // loop over hit strips
    
    TAMSDhit* strip = (TAMSDhit*)fListOfStrips->At(i);
    Int_t stripId  = strip->GetStrip();
    if (!CheckLine(stripId)) continue;
    
    TAGactNtuCluster1D::FillMaps(stripId, i);
  }
}

//______________________________________________________________________________
//
Bool_t TAMSDactNtuCluster::CreateClusters(Int_t iSensor)
{
   TAMSDntuCluster* pNtuClus = (TAMSDntuCluster*) fpNtuClus->Object();
   TAMSDparGeo* pGeoMap  = (TAMSDparGeo*)     fpGeoMap->Object();

  TAMSDcluster* cluster = 0x0;
  
  // create clusters
  for (Int_t i = 0; i< fClustersN; ++i)
    pNtuClus->NewCluster(iSensor);
  
  for (Int_t iPix = 0; iPix < fListOfStrips->GetEntries(); ++iPix) {
    TAMSDhit* strip = (TAMSDhit*)fListOfStrips->At(iPix);
    Int_t stripId = strip->GetStrip();
    if(!CheckLine(stripId)) continue;
    
    Int_t clusterN = GetClusterNumber(stripId);
    if ( clusterN != -1 ) {
      cluster = pNtuClus->GetCluster(iSensor, clusterN);
      cluster->AddStrip(strip);
      cluster->SetPlaneView(strip->GetView());
    }
  }

  
  // Compute position and fill clusters info
  for (Int_t i = 0; i< pNtuClus->GetClustersN(iSensor); ++i) {
    cluster = pNtuClus->GetCluster(iSensor, i);

    cluster->SetSensorIdx(iSensor);
    fCurListOfStrips = cluster->GetListOfStrips();
    ComputePosition(cluster);
    
    TVector3 posG(GetCurrentPosition(), 0, 0);
    posG = pGeoMap->Sensor2Detector(iSensor, posG);
    cluster->SetPlaneView(pGeoMap->GetSensorPar(iSensor).TypeIdx);
    cluster->SetPositionG(posG);
    
     if (ApplyCuts(cluster)) {
        // histogramms
        cluster->SetValid();
        if (ValidHistogram()) {
           if (cluster->GetStripsN() > 0) {
              fpHisStripTot->Fill(cluster->GetStripsN());
              fpHisStrip[iSensor]->Fill(cluster->GetStripsN());
              fpHisClusCharge[iSensor]->Fill(cluster->GetEnergyLoss());
              fpHisClusChargeTot->Fill(cluster->GetEnergyLoss());
              fpHisClusMap[iSensor]->Fill(cluster->GetPositionF());
           }
        }
     }
  }
   
  if (pNtuClus->GetClustersN(iSensor))
     return true;
  
   return false;
}
  
//______________________________________________________________________________
//
void TAMSDactNtuCluster::ComputePosition(TAMSDcluster* cluster)
{
  if (!fCurListOfStrips) return;
    
  Float_t tCorrection, tCorrection2, tCorTemp;
  Float_t pos, posErr = 0;
  tCorrection = 0.;
  tCorrection2 = 0.;
   
  Float_t tClusterPulseSum = 0.;
  
  for (Int_t i = 0; i < fCurListOfStrips->GetEntries(); ++i) {
    TAMSDhit* strip = (TAMSDhit*)fCurListOfStrips->At(i);
    tCorTemp = strip->GetPosition()*strip->GetEnergyLoss();
    tCorrection  += tCorTemp;
    tClusterPulseSum  += strip->GetEnergyLoss();
  }
  
  pos = tCorrection*(1./tClusterPulseSum);
  
  for (Int_t i = 0; i < fCurListOfStrips->GetEntries(); ++i) {
    TAMSDhit* strip = (TAMSDhit*)fCurListOfStrips->At(i);
    tCorrection2 = strip->GetEnergyLoss()*(strip->GetPosition()-pos)*(strip->GetPosition()-pos);
    posErr += tCorrection2;
  }
  
  posErr *= 1./tClusterPulseSum;
  
  // for cluster with a single strip
  Float_t lim = 1.875e-5; // in cm !
  if (posErr < lim) posErr = lim; //(150/Sqrt(12)^2
  
  fCurrentPosition = pos;
  fCurrentPosError = TMath::Sqrt(posErr);
   
  cluster->SetPositionF(fCurrentPosition);
  cluster->SetPosErrorF(fCurrentPosError);
  cluster->SetEnergyLoss(tClusterPulseSum);
}

//______________________________________________________________________________
//
Bool_t TAMSDactNtuCluster::ApplyCuts(TAMSDcluster* cluster)
{
   TAMSDparConf* pConfig = (TAMSDparConf*) fpConfig->Object();
   
   TClonesArray* list = cluster->GetListOfStrips();
   Int_t  entries = list->GetEntries();
   
   // cuts on strips in cluster
   if(entries < pConfig->GetSensorPar(cluster->GetSensorIdx()).MinNofStripsInCluster ||
      entries > pConfig->GetSensorPar(cluster->GetSensorIdx()).MaxNofStripsInCluster)
      return false;
   
   return true;
}
