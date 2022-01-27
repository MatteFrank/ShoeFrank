/*!
 \file TAMSDactNtuClusterDT.cxx
 \brief   Implementation of TAMSDactNtuClusterDT.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDactNtuClusterDT.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TAMSDactNtuClusterDT
 \brief NTuplizer for MSD cluster. **
 */

//! Class Imp
ClassImp(TAMSDactNtuClusterDT);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit container descriptor
//! \param[out] pNtuClus cluster container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
TAMSDactNtuClusterDT::TAMSDactNtuClusterDT(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus, TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
  : TAGaction(name, "TAMSDactNtuClusterDT - NTuplize cluster"),
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
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAMSDactNtuClusterDT::CreateHistogram()
{
  DeleteHistogram();
  
  TString prefix = "ms";
  TString titleDev = "Micro Strip Detector";
  
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
     fpHisClusCharge[i] = new TH1F(Form("%sClusCharge%d",prefix.Data(), i+1), Form("%s - cluster charge for sensor %d", titleDev.Data(), i+1), 1000, 0., 5000);
     AddHistogram(fpHisClusCharge[i]);
  }
   
  fpHisClusChargeTot = new TH1F(Form("%sClusChargeTot",prefix.Data()), Form("%s - total cluster charge", titleDev.Data()), 1000, 0., 5000);
  AddHistogram(fpHisClusChargeTot);

  SetValidHistogram(kTRUE);
  return;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactNtuClusterDT::~TAMSDactNtuClusterDT()
{
}

//______________________________________________________________________________
//! Action
Bool_t TAMSDactNtuClusterDT::Action()
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

//------------------------------------------+-----------------------------------
//! Find clusters
//!
//! \param[in] iSensor sensor index
Bool_t TAMSDactNtuClusterDT::FindClusters(Int_t iSensor)
{
  return true;
}

//------------------------------------------+-----------------------------------
//! Compute position for a clusters
//!
//! \param[in] cluster a given cluster
void TAMSDactNtuClusterDT::ComputePosition(TAMSDcluster* cluster)
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

//------------------------------------------+-----------------------------------
//! Applay cuts for a clusters
//!
//! \param[in] cluster a given cluster
Bool_t TAMSDactNtuClusterDT::ApplyCuts(TAMSDcluster* cluster)
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
