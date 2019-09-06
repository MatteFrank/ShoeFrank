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
#include "TAMSDntuRaw.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDactNtuCluster.hxx"

/*!
 \class TAMSDactNtuCluster 
 \brief NTuplizer for vertex raw hits. **
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
  AddDataIn(pNtuRaw,   "TAMSDntuRaw");
  AddDataOut(pNtuClus, "TAMSDntuCluster");
  
  TAMSDparGeo* geoMap = (TAMSDparGeo*)fpGeoMap->Object();
  fDimX = geoMap->GetNStrip()+1;
  SetupMaps(fDimX);
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAMSDactNtuCluster::CreateHistogram()
{
  DeleteHistogram();
  
  TString prefix = "ms";
  TString titleDev = "Multi Strip Detector";
  
  fpHisStripTot = new TH1F(Form("%sClusPixelTot", prefix.Data()), Form("%s - Total # strips per clusters", titleDev.Data()), 100, 0., 100.);
  AddHistogram(fpHisStripTot);
  
  TAMSDparGeo* pGeoMap  = (TAMSDparGeo*) fpGeoMap->Object();
  
  for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
    fpHisStrip[i] = new TH1F(Form("%sClusPixel%d",prefix.Data(), i+1), Form("%s - # strips per clusters for sensor %d", titleDev.Data(), i+1), 100, 0., 100.);
    AddHistogram(fpHisStrip[i]);
  }
  
  for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
    fpHisClusMap[i] = new TH1F(Form("%sClusMap%d", prefix.Data(), i+1), Form("%s - clusters map for sensor %d", titleDev.Data(), i+1),
			       pGeoMap->GetNStrip(), -pGeoMap->GetPitch()*pGeoMap->GetNStrip()/2., pGeoMap->GetPitch()*pGeoMap->GetNStrip()/2.);
    
    AddHistogram(fpHisClusMap[i]);
  }
  
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
  TAMSDntuRaw* pNtuHit  = (TAMSDntuRaw*) fpNtuRaw->Object();
  TAMSDparGeo* pGeoMap  = (TAMSDparGeo*) fpGeoMap->Object();
  Bool_t ok = true;
  
  for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
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
  
  TAMSDntuCluster* pNtuClus = (TAMSDntuCluster*) fpNtuClus->Object();
  TAMSDparGeo* pGeoMap  = (TAMSDparGeo*)     fpGeoMap->Object();
  
  FillMaps();
  SearchCluster();
  
  return CreateClusters(iSensor, pNtuClus, pGeoMap);
}

//______________________________________________________________________________
//
void TAMSDactNtuCluster::SearchCluster()
{
  fClustersN = 0;
  // Search for cluster
  
  for (Int_t iPix = 0; iPix < fListOfStrips->GetEntries(); ++iPix) { // loop over hit strips
    TAMSDntuHit* strip = (TAMSDntuHit*)fListOfStrips->At(iPix);
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
    
    TAMSDntuHit* strip = (TAMSDntuHit*)fListOfStrips->At(i);
    Int_t stripId  = strip->GetStrip();
    if (!CheckLine(stripId)) continue;
    
    TAGactNtuCluster1D::FillMaps(stripId, i);
  }
}

//______________________________________________________________________________
//
Bool_t TAMSDactNtuCluster::CreateClusters(Int_t iSensor, TAMSDntuCluster* pNtuClus, TAMSDparGeo* pGeoMap)
{
  TAMSDcluster* cluster = 0x0;
  
  // create clusters
  for (Int_t i = 0; i< fClustersN; ++i)
    pNtuClus->NewCluster(iSensor);
  
  for (Int_t iPix = 0; iPix < fListOfStrips->GetEntries(); ++iPix) {
    TAMSDntuHit* strip = (TAMSDntuHit*)fListOfStrips->At(iPix);
    Int_t stripId = strip->GetStrip();
    if(!CheckLine(stripId)) continue;
    
    Int_t clusterN = GetClusterNumber(stripId);
    if ( clusterN != -1 ) {
      cluster = pNtuClus->GetCluster(iSensor, clusterN);
      cluster->AddPixel(strip);
    }
  }
  
  // Compute position and fill clusters info
  for (Int_t i = 0; i< pNtuClus->GetClustersN(iSensor); ++i) {
    cluster = pNtuClus->GetCluster(iSensor, i);
    cluster->SetPlaneNumber(iSensor);
    fCurListOfStrips = cluster->GetListOfStrips();
    ComputePosition();
    
    TVector3 posG(GetCurrentPosition(), 0, 0);
    posG = pGeoMap->Sensor2Detector(iSensor, posG);
    //  pGeoMap->Local2Global(&posG);
    cluster->SetPositionG(&posG);
    cluster->SetPosition(GetCurrentPosition());
    cluster->SetPosError(GetCurrentPosError());
    
    if (ApplyCuts(cluster)) {
      // histogramms
      if (ValidHistogram()) {
	if (cluster->GetStripsN() > 0) {
	  fpHisStripTot->Fill(cluster->GetStripsN());
	  fpHisStrip[iSensor]->Fill(cluster->GetStripsN());
	  // printf("sensor %d %d\n", iSensor, cluster->GetPixelsN());
	  if (TAMSDparConf::IsMapHistOn()) {
	    fpHisClusMap[iSensor]->Fill(cluster->GetPosition());
	  }
	}
      }
    }
    if (pNtuClus->GetClustersN(iSensor))
      return true;
    
    return false;
    
  }
}
  
//______________________________________________________________________________
//
void TAMSDactNtuCluster::ComputePosition()
{
  if (!fCurListOfStrips) return;
  
  Float_t tCorrection, tCorrection2, tCorTemp;
  Float_t pos, posErr;
  tCorrection = 0.;
  tCorrection2 = 0.;
   
  Float_t tClusterPulseSum = 0.;
  
  for (Int_t i = 0; i < fCurListOfStrips->GetEntries(); ++i) {
    TAMSDntuHit* strip = (TAMSDntuHit*)fCurListOfStrips->At(i);
    tCorTemp = strip->GetPosition()*strip->GetValue();
    tCorrection  += tCorTemp;
    tClusterPulseSum  += strip->GetValue();
  }
  
  pos = tCorrection*(1./tClusterPulseSum);
  
  for (Int_t i = 0; i < fCurListOfStrips->GetEntries(); ++i) {
    TAMSDntuHit* strip = (TAMSDntuHit*)fCurListOfStrips->At(i);
    tCorrection2 = strip->GetValue()*(strip->GetPosition()-pos)*(strip->GetPosition()-pos);
    posErr += tCorrection2;
  }
  
  posErr *= 1./tClusterPulseSum;
  
  // for cluster with a single strip
  Float_t lim = 2.5e-3; // in cm !
  if (posErr < lim) posErr = lim; //(20/Sqrt(12)^2
  
  fCurrentPosition = pos;
  fCurrentPosError = posErr;
}
