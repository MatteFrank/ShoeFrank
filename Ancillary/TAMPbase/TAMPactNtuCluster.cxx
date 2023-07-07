/*!
 \file
 \version $Id: TAMPactNtuCluster.cxx
 \brief   NTuplizer for ITR raw hits
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAGgeoTrafo.hxx"
#include "TAMPparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAMPntuHit.hxx"
#include "TAMPntuHit.hxx"
#include "TAMPntuCluster.hxx"
#include "TAMPactNtuCluster.hxx"

#include "TAGnameManager.hxx"

/*!
 \class TAMPactNtuCluster
 \brief NTuplizer for ITR raw hits
 */

//! Class imp
ClassImp(TAMPactNtuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit input container descriptor
//! \param[out] pNtuClus cluster output container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
TAMPactNtuCluster::TAMPactNtuCluster(const char* name,
									 TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus,
									 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
: TAVTactBaseNtuCluster(name, pConfig, pGeoMap),
  fpNtuRaw(pNtuRaw),
  fpNtuClus(pNtuClus)
{
   AddDataIn(pNtuRaw,   "TAMPntuHit");
   AddDataOut(pNtuClus, "TAMPntuCluster");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMPactNtuCluster::~TAMPactNtuCluster()
{
   
}

//______________________________________________________________________________
//! Action
Bool_t TAMPactNtuCluster::Action()
{
   TAMPntuHit* pNtuHit  = (TAMPntuHit*) fpNtuRaw->Object();
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   Bool_t ok = true;
   
   for (Int_t i = 0; i < pConfig->GetSensorsN(); ++i) {
      fListOfPixels = pNtuHit->GetListOfPixels(i);
      if (fListOfPixels->GetEntriesFast() == 0) continue;
      ok += FindClusters(i);
   }
   
   if(ok)
      fpNtuClus->SetBit(kValid);
   return ok;
}

//______________________________________________________________________________
//! Find clusters for a given sensor
//!
//! \param[in] iSensor index of sensor
Bool_t TAMPactNtuCluster::FindClusters(Int_t iSensor)
{
   // Algo taking from Virgile BEKAERT (ImaBio @ IPHC-Strasbourg)
   // Look in a iterative way to next neighbour
   
   FillMaps();
   SearchCluster();
 
   return CreateClusters(iSensor);
}

//______________________________________________________________________________
//! Create clusters for a given sensor
//!
//! \param[in] iSensor index of sensor
Bool_t TAMPactNtuCluster::CreateClusters(Int_t iSensor)
{
   TAMPntuCluster* pNtuClus = (TAMPntuCluster*) fpNtuClus->Object();
   TAMPcluster* cluster1    = 0x0;

   // create clusters
   for (Int_t i = 0; i< fClustersN; ++i)
      pNtuClus->NewCluster(iSensor);
   
   for (Int_t iPix = 0; iPix < fListOfPixels->GetEntriesFast(); ++iPix) {
      TAMPhit* pixel = (TAMPhit*)fListOfPixels->At(iPix);
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if(!CheckLine(line)) continue;
      if(!CheckCol(col)) continue;
      
      Int_t clusterN = GetClusterNumber(line,col);
      if ( clusterN != -1 ) {
         cluster1 = pNtuClus->GetCluster(iSensor, clusterN);
         cluster1->AddPixel(pixel);
      }
   }
   
   // Compute position and fill clusters info
   TAVTbaseCluster* cluster = 0x0;
   
   for (Int_t i = 0; i< pNtuClus->GetClustersN(iSensor); ++i) {
      cluster = pNtuClus->GetCluster(iSensor, i);
      cluster->SetDeviceType(TAGgeoTrafo::GetDeviceType(FootBaseName("TAMPparGeo")));
      FillClusterInfo(iSensor, cluster);
   }
   
   // Remove no valid cluster
   for (Int_t i = pNtuClus->GetClustersN(iSensor)-1; i >= 0; --i) {
      cluster = pNtuClus->GetCluster(iSensor, i);
      if (!cluster->IsValid())
         pNtuClus->GetListOfClusters(iSensor)->Remove(cluster);
   }
   
   if (pNtuClus->GetClustersN(iSensor))
      return true;
   
   return false;
}
