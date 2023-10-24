/*!
 \file TAMSDactNtuCluster.cxx
 \brief   Implementation of TAMSDactNtuCluster.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAMSDparGeo.hxx"
#include "TAMSDparCal.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDactNtuCluster.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TAMSDactNtuCluster 
 \brief NTuplizer for MSD cluster.
 */

//! Class imp
ClassImp(TAMSDactNtuCluster);

Bool_t TAMSDactNtuCluster::fgSeedAlgo = true;

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit input container descriptor
//! \param[out] pNtuClus cluster output container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pCalib calibration parameter descriptor
TAMSDactNtuCluster::TAMSDactNtuCluster(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus, TAGparaDsc* pConfig, TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, Bool_t flagMC)
  : TAGactNtuCluster1D(name, "TAMSDactNtuCluster - NTuplize cluster"),
    fpNtuRaw(pNtuRaw),
    fpNtuClus(pNtuClus),
    fpConfig(pConfig),
    fpGeoMap(pGeoMap),
    fpCalib(pCalib),
    fFlagMC(flagMC),
    fCurrentPosition(0.),
    fCurrentPosError(0.),
    fListOfStrips(0x0),
    fClustersN(0)
{
  AddPara(pGeoMap, "TAMSDparGeo");
  AddPara(pConfig, "TAMSDparConf");
   if (pCalib)
      AddPara(pCalib, "TAMSDparCal");
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
  
  fpHisNClusTot = new TH1F(Form("%sNClusTot", prefix.Data()), Form("%s - total # cluster ", titleDev.Data()), 5, 0.5,5.5 );
  AddHistogram(fpHisNClusTot);

  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisNClus[i] = new TH1F(Form("%sNClus%d", prefix.Data(), i+1), Form("%s - # of cluster for sensor %d", titleDev.Data(), i+1), 5, 0.5,5.5);  
    AddHistogram(fpHisNClus[i]);
  }
  
  fpHisEta = new TH1F(Form("%sClusEtaTot",prefix.Data()), Form("%s - total cluster eta ", titleDev.Data()), 100, 0., 1.);
  AddHistogram(fpHisEta);

  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisSignalMap[i]= new TH2F(Form("%sSignalMap%d", prefix.Data(),i+1), Form("%s - map signal-eta for sensor %d", titleDev.Data(),i+1),40, 0., 1., 100, 0., 100 );
    AddHistogram(fpHisSignalMap[i]);
  }
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisSignalMap_corr[i]= new TH2F(Form("%sSignalMap_corr%d", prefix.Data(),i+1), Form("%s - corrected map signal-eta for sensor %d", titleDev.Data(),i+1),40, 0., 1., 100, 0., 100 );
    AddHistogram(fpHisSignalMap_corr[i]);
  }

  fpHisClusChargeTotGain = new TH1F(Form("%sClusChargeTotGain",prefix.Data()), Form("%s - total cluster charge with gain", titleDev.Data()), 1000, 0., 5000);
  AddHistogram(fpHisClusChargeTotGain);

  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
     fpHisRootADC[i] = new TH1F(Form("%sfpHisRootADC%d",prefix.Data(), i+1), Form("%s - sqrt of cluster charge for sensor %d", titleDev.Data(), i+1), 700, 0., 70);
     AddHistogram(fpHisRootADC[i]);
  }

  fpHisRootADCTot = new TH1F(Form("%sfpHisRootADCTot",prefix.Data()), Form("%s - sqrt of cluster adc Tot", titleDev.Data()), 700, 0., 70);
  AddHistogram(fpHisRootADCTot); 
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
     fpHisRootADC_corr[i] = new TH1F(Form("%sfpHisRootADC_corr%d",prefix.Data(), i+1), Form("%s - sqrt of cluster charge corrected for sensor  %d", titleDev.Data(), i+1), 700, 0., 70);
     AddHistogram(fpHisRootADC_corr[i]);
  }
  
  fpHisRootADCTot_corr = new TH1F(Form("%sfpHisRootADCTot_corr",prefix.Data()), Form("%s - total sqrt of cluster adc corrected ", titleDev.Data()), 700, 0., 70);
  AddHistogram(fpHisRootADCTot_corr); 

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
TAMSDactNtuCluster::~TAMSDactNtuCluster()
{
}

//______________________________________________________________________________
//! Action
Bool_t TAMSDactNtuCluster::Action()
{
  TAMSDntuHit* pNtuHit  = (TAMSDntuHit*) fpNtuRaw->Object();
  TAMSDparGeo* pGeoMap  = (TAMSDparGeo*) fpGeoMap->Object();
  Bool_t ok = true;
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fListOfStrips = pNtuHit->GetListOfStrips(i);

    if (fListOfStrips->GetEntriesFast() == 0) continue;
    ok += FindClusters(i);
   //TODO: add check for clusters containing noisy strips to manipulate them (noisy strips are now simply skipped in the clusterization)
    CorrectCLuster(i);
  }
  
  if(ok)
    fpNtuClus->SetBit(kValid);
  
  return ok;
}

//______________________________________________________________________________
//! Correct signal cluster
//!
//! \param[in] iSensor plane id
void TAMSDactNtuCluster::CorrectCLuster(Int_t iSensor){

  TAMSDntuCluster* pNtuClus = (TAMSDntuCluster*) fpNtuClus->Object();
  int Ncluster=pNtuClus->GetClustersN(iSensor);

  //loop in clusters
  for (Int_t i = 0; i< pNtuClus->GetClustersN(iSensor); ++i) {

    TAMSDcluster* cluster = pNtuClus->GetCluster(iSensor, i);
    int strips= cluster->GetStripsN(); //get number  strips per cluster           
    Float_t eta = cluster->GetEta();
    Float_t e_loss=cluster->GetEnergyLoss();

    //fill clusters info
    if (ValidHistogram()) {
      if (cluster->GetStripsN()> 0) {
        if(strips>1) fpHisEta->Fill(eta);
        fpHisNClusTot->Fill(Ncluster); //total number of cluster
        fpHisNClus[iSensor]->Fill(Ncluster); //number of cluster per sensor
      }
    }

    //Gain factors of VA Signal// for eta map   
    ComputeClusterGain(cluster, iSensor);
    fpHisClusChargeTotGain->Fill(cluster->GetEnergyLoss());
    
    //Correct cluster energy
    ComputePosition(cluster);
    ComputeEta(cluster);
    if (fpCalib && !fFlagMC)
      ComputeCorrEnergy2D(cluster);
    if (ApplyCuts(cluster)){
        // histogramms
      cluster->SetValid();
      if(ValidHistogram()){
        if (cluster->GetStripsN() > 0) {
        //map adc-signal for eta
        Float_t eta = cluster->GetEta();
        SignalMap[eta] = sqrt(cluster->GetEnergyLoss());
        fpHisSignalMap[iSensor]->Fill(eta,sqrt(cluster->GetEnergyLoss()));
        SignalMap_corr[eta] = sqrt(cluster->GetEnergyLossCorr());
        fpHisSignalMap_corr[iSensor]->Fill(eta,sqrt(cluster->GetEnergyLossCorr()));
        fpHisRootADCTot_corr->Fill(sqrt(cluster->GetEnergyLossCorr()));
        fpHisRootADC_corr[iSensor]->Fill(sqrt(cluster->GetEnergyLossCorr()));
        }
      }
    }     
  }
}
//______________________________________________________________________________
//! Find Clusters
//!
//! \param[in] iSensor plane id
Bool_t TAMSDactNtuCluster::FindClusters(Int_t iSensor)
{
  // Algo taking from Virgile BEKAERT (ImaBio @ IPHC-Strasbourg)
  // Look in an iterative way to next neighbour
  
  FillMaps();
  SearchCluster();
  
  return CreateClusters(iSensor);
}

//______________________________________________________________________________
//! Found all clusters.
void TAMSDactNtuCluster::SearchCluster()
{
  fClustersN = 0;
  // Search for cluster
  for (Int_t iStrip = 0; iStrip < fListOfStrips->GetEntriesFast(); ++iStrip) { // loop over seed strips
    TAMSDhit* strip = (TAMSDhit*)fListOfStrips->At(iStrip);
    if (!strip->IsSeed() && fgSeedAlgo) continue;
    if (strip->Found()) continue;
    
    Int_t stripId  = strip->GetStrip();
    if (!CheckLine(stripId)) continue;
    
    // loop over lines & columns
    if ( ShapeCluster(fClustersN, stripId) )
      fClustersN++;
  }
}

//______________________________________________________________________________
//! Get Hit object in list
//!
//! \param[in] idx index on Hit list
TAGobject*  TAMSDactNtuCluster::GetHitObject(Int_t idx) const
{
  if (idx >= 0 && idx < GetListOfStrips()->GetEntriesFast() )
    return (TAGobject*)GetListOfStrips()->At(idx);
  
  else {
    Error("GetHitObject()", "Error in index %d (max: %d)", idx, GetListOfStrips()->GetEntriesFast()-1);
    return 0x0;
  }
}

//______________________________________________________________________________
//! Fill maps
void TAMSDactNtuCluster::FillMaps()
{
  
  // Clear maps
  ClearMaps();
  
  if (fListOfStrips->GetEntriesFast() == 0) return;
  
  Int_t map_idx = 0;
  // fill maps for cluster
  for (Int_t i = 0; i < fListOfStrips->GetEntriesFast(); i++) { // loop over hit strips
    
    TAMSDhit* strip = (TAMSDhit*)fListOfStrips->At(i);
    Int_t stripId  = strip->GetStrip();
    if (!CheckLine(stripId)) continue;
    
    bool free_noise=true;
    if(!free_noise){
      if (strip->IsNoisy()) {
      continue;
      }
    }
 
    TAGactNtuCluster1D::FillMaps(stripId, map_idx);
    map_idx++;
  }
}

//______________________________________________________________________________
//! Create clusters for a given plane
//!
//! \param[in] iSensor plane id
Bool_t TAMSDactNtuCluster::CreateClusters(Int_t iSensor)
{
   TAMSDntuCluster* pNtuClus = (TAMSDntuCluster*) fpNtuClus->Object();
   TAMSDparGeo* pGeoMap  = (TAMSDparGeo*)     fpGeoMap->Object();
   TAMSDparCal* pCalib   = 0x0;
   
   if (fpCalib)
      pCalib = (TAMSDparCal*) fpCalib->Object();

  TAMSDcluster* cluster = 0x0;
  
  // create clusters
  for (Int_t i = 0; i< fClustersN; ++i)
    pNtuClus->NewCluster(iSensor);
  
  for (Int_t iPix = 0; iPix < fListOfStrips->GetEntriesFast(); ++iPix) {
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
    cluster->SetPlaneView(pGeoMap->GetSensorPar(iSensor).TypeIdx);
    fCurListOfStrips = cluster->GetListOfStrips();
    ComputePosition(cluster);
    ComputeEta(cluster);
    if (fpCalib && !fFlagMC)
       ComputeCorrEnergy(cluster);
    
    TVector3 posG(GetCurrentPosition(), 0, 0);
    posG = pGeoMap->Sensor2Detector(iSensor, posG);    
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
              fpHisRootADCTot->Fill(sqrt(cluster->GetEnergyLoss()));
              fpHisRootADC[iSensor]->Fill(sqrt(cluster->GetEnergyLoss()));
           }
        }
     }
  }
   
  if (pNtuClus->GetClustersN(iSensor))
     return true;
  
   return false;
}
  
//_____________________________________________________________________________
//! Compute the cluster centroid position
//!
//! \param[in] cluster a given cluster
void TAMSDactNtuCluster::ComputePosition(TAMSDcluster* cluster)
{
  if (!fCurListOfStrips) return;
    
  Float_t tCorrection  = 0.;
  Float_t tCorrection2 = 0;
  Float_t pos          = 0.;
  Float_t cog          = 0.;
  Float_t posErr       = 0;
  Float_t tStrip       = 0.;

  Float_t tClusterPulseSum = 0.;
  
  for (Int_t i = 0; i < fCurListOfStrips->GetEntriesFast(); ++i) {
    TAMSDhit* strip   = (TAMSDhit*)fCurListOfStrips->At(i);
    if(strip->IsNoisy()) continue;  //skip noisy strips in position calculation
    tCorrection      += strip->GetPosition()*strip->GetEnergyLoss();
    tStrip           += strip->GetStrip()*strip->GetEnergyLoss();
    tClusterPulseSum += strip->GetEnergyLoss();
  }
  
  pos = tCorrection/tClusterPulseSum;
  cog = tStrip/tClusterPulseSum;
   
  for (Int_t i = 0; i < fCurListOfStrips->GetEntriesFast(); ++i) {
    TAMSDhit* strip = (TAMSDhit*)fCurListOfStrips->At(i);
    if(strip->IsNoisy()) continue;  //skip noisy strips in position calculation
    tCorrection2 = strip->GetEnergyLoss()*(strip->GetPosition()-pos)*(strip->GetPosition()-pos);
    posErr += tCorrection2;
  }
  
  posErr *= 1./tClusterPulseSum;
  
  // for cluster with a single strip
  Float_t lim = 1.875e-5; // in cm !
  if (posErr < lim) posErr = lim; //(150/Sqrt(12)^2
  
  fCurrentPosition = pos;
  fCurrentPosError = TMath::Sqrt(posErr);
  fCurrentCog      = cog;
  fCurrentEnergy   = tClusterPulseSum;
  
  cluster->SetPositionF(fCurrentPosition);
  cluster->SetPosErrorF(fCurrentPosError);
  cluster->SetCog(fCurrentCog);
  cluster->SetEnergyLoss(fCurrentEnergy);
}

//_____________________________________________________________________________
//! Compute the cluster eta
//!
//! \param[in] cluster a given cluster
void TAMSDactNtuCluster::ComputeEta(TAMSDcluster* cluster)
{
  if (!fCurListOfStrips) return;
  Int_t nstrips = fCurListOfStrips->GetEntriesFast();

  Float_t eta     = 0.;
  TAMSDhit* strip = 0x0;
  Float_t max_adc = -1;
  Int_t max_pos   = 0;

  if (nstrips == 1) {
    eta = 1.0;
  }  else {
    for (int i = 0; i < nstrips; i++) {
      strip = (TAMSDhit*)fCurListOfStrips->At(i);
      if(strip->GetEnergyLoss() > max_adc) {
         max_adc = strip->GetEnergyLoss();
         max_pos = i;
      }
    }

    if (max_pos == 0) {
      eta = ((TAMSDhit*)fCurListOfStrips->At(0))->GetEnergyLoss() / ( ((TAMSDhit*)fCurListOfStrips->At(0))->GetEnergyLoss() + ((TAMSDhit*)fCurListOfStrips->At(1))->GetEnergyLoss() );
    } else if(max_pos == nstrips - 1) {
      eta = ((TAMSDhit*)fCurListOfStrips->At(max_pos - 1))->GetEnergyLoss() / ( ((TAMSDhit*)fCurListOfStrips->At(max_pos - 1))->GetEnergyLoss() + ((TAMSDhit*)fCurListOfStrips->At(max_pos))->GetEnergyLoss() );
    } else {
      if( ((TAMSDhit*)fCurListOfStrips->At(max_pos - 1))->GetEnergyLoss() >  ((TAMSDhit*)fCurListOfStrips->At(max_pos + 1))->GetEnergyLoss() ) {
         eta = ((TAMSDhit*)fCurListOfStrips->At(max_pos - 1))->GetEnergyLoss() / ( ((TAMSDhit*)fCurListOfStrips->At(max_pos - 1))->GetEnergyLoss() + ((TAMSDhit*)fCurListOfStrips->At(max_pos))->GetEnergyLoss() );
      } else {
         eta = ((TAMSDhit*)fCurListOfStrips->At(max_pos))->GetEnergyLoss() / ( ((TAMSDhit*)fCurListOfStrips->At(max_pos))->GetEnergyLoss() + ((TAMSDhit*)fCurListOfStrips->At(max_pos + 1))->GetEnergyLoss() );
      }
    }
  }

  fCurrentEta = eta;
  cluster->SetEta(fCurrentEta);
}

//_____________________________________________________________________________
//! Compute the cluster energy correction
//!
//! \param[in] cluster a given cluster
void TAMSDactNtuCluster::ComputeCorrEnergy(TAMSDcluster* cluster)
{
  Float_t eCorrection = 1.;

  TAMSDparCal* p_parcal = (TAMSDparCal*) fpCalib->Object();

  eCorrection = p_parcal->GetElossParam(fCurrentEta);

  cluster->SetEnergyLossCorr(fCurrentEnergy / eCorrection);
}

//_____________________________________________________________________________
//! Compute cluster energy gain
//!
//! \param[in] cluster a given cluster
void TAMSDactNtuCluster::ComputeClusterGain(TAMSDcluster* cluster, Int_t iSensor)
{
  vector<double> sensors;
  vector<double> VAS;
  vector<double> gains;

  //read gain from cal
  ifstream filedati("../../build/Reconstruction/gfile/FOOT_VA_GAIN.txt", ios::in);
  //gain = p_parcal->GetElossParam2D(fCurrentEta, fCurrentEnergy);  
    
  string Detector, VA, Correction;
  double sensor,vas,gain;
  filedati>> Detector>>VA>>Correction;
  while(filedati>> sensor>>vas>>gain){ 
    sensors.push_back(sensor);
    VAS.push_back(vas);
    gains.push_back(gain);
  }
  
  for (int i_vec=0;i_vec<sensors.size();i_vec+=10){
    
    if(iSensor == sensors[i_vec]){
      //loop in strips in cluster
      for (Int_t i_strip = 0; i_strip < 1; i_strip++){
        TAMSDhit* strip = (TAMSDhit*)fCurListOfStrips->At(i_strip);
        Int_t stripId = strip->GetStrip();
        int count = 0;
        int min_strip = 0;
        int max_strip = min_strip + CN_CH;
        while(min_strip<641){
          if(stripId>min_strip && stripId<max_strip){
            cluster->SetEnergyLoss(cluster->GetEnergyLoss()/gains[i_vec+count]);
            break;
          }  
          min_strip += CN_CH;
          max_strip += CN_CH;
          count++;
        }
      }
    }
  }
}

//_____________________________________________________________________________
//! Compute the cluster energy correction using eta map
//!
//! \param[in] cluster a given cluster
void TAMSDactNtuCluster::ComputeCorrEnergy2D(TAMSDcluster* cluster)
{
  Float_t eCorrection = 1.;

  TAMSDparCal* p_parcal = (TAMSDparCal*) fpCalib->Object();

  eCorrection = p_parcal->GetElossParam2D(fCurrentEta, fCurrentEnergy);

  cluster->SetEnergyLossCorr(fCurrentEnergy*(pow(eCorrection,2)));  
}

//_____________________________________________________________________________
//! Apply cluster cuts
//!
//! \param[in] cluster a given cluster
Bool_t TAMSDactNtuCluster::ApplyCuts(TAMSDcluster* cluster)
{
   TAMSDparConf* pConfig = (TAMSDparConf*) fpConfig->Object();
   
   TClonesArray* list = cluster->GetListOfStrips();
   Int_t  entries = list->GetEntriesFast();
   
   // cuts on strips in cluster
   if(entries < pConfig->GetSensorPar(cluster->GetSensorIdx()).MinNofStripsInCluster ||
      entries > pConfig->GetSensorPar(cluster->GetSensorIdx()).MaxNofStripsInCluster)
      return false;
   
   return true;
}
