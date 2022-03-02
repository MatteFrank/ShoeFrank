/*!
 \file
 \version $Id: TAITactNtuClusterMT.cxx
 \brief   Implementation of TAITactNtuClusterMT.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAGrecoManager.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAITntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAITntuCluster.hxx"
#include "TAITactNtuClusterMT.hxx"

/*!
 \class TAITactNtuClusterMT
 \brief NTuplizer for ITR cluster with multi-threading
 */

//! Class Imp
ClassImp(TAITactNtuClusterMT);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit input container descriptor
//! \param[out] pNtuClus cluster output container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
TAITactNtuClusterMT::TAITactNtuClusterMT(const char* name,
									 TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus,
									 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
: TAVTactBaseNtuClusterMT(name, pConfig, pGeoMap),
  fpNtuRaw(pNtuRaw),
  fpNtuClus(pNtuClus)
{
   AddDataIn(pNtuRaw,   "TAITntuHit");
   AddDataOut(pNtuClus, "TAITntuCluster");
   
   if (fThreadsN > fgkLimThreadsN)
      fThreadsN = fgkLimThreadsN;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactNtuClusterMT::~TAITactNtuClusterMT()
{
   
}

//______________________________________________________________________________
//! Action
Bool_t TAITactNtuClusterMT::Action()
{
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   for (Int_t i = 0; i < pConfig->GetSensorsN(); i+=fThreadsN) {
      fgSensorOff = i;
      ThreadStart();
      ThreadJoin();
      ThreadStop();
   }
   
   fpNtuClus->SetBit(kValid);
   
   return true;
}

//______________________________________________________________________________
//! Find clusters for a given sensor and thread
//!
//! \param[in] iSensor index of sensor
//! \param[in] listOfPixels list of pixels
//! \param[in] thr index of thread
Bool_t TAITactNtuClusterMT::FindClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr)
{
   // Algo taking from Virgile BEKAERT (ImaBio @ IPHC-Strasbourg)
   // Look in a iterative way to next neighbour
   
   if (FootDebugLevel(1))
      printf("Thread %d with %d pixels\n", thr, listOfPixels->GetEntries());
   
   FillMaps(listOfPixels, thr);
   SearchCluster(listOfPixels, thr);
 
   return CreateClusters(iSensor, listOfPixels, thr);
}

//______________________________________________________________________________
//! Create clusters for a given sensor, pixel list and thread
//!
//! \param[in] iSensor index of sensor
//! \param[in] listOfPixels list of pixels
//! \param[in] thr index of thread
Bool_t TAITactNtuClusterMT::CreateClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr)
{
   TAITntuCluster* pNtuClus = (TAITntuCluster*) fpNtuClus->Object();
   TAITcluster* cluster1    = 0x0;

   // create clusters
   for (Int_t i = 0; i< fClustersN[thr]; ++i)
      pNtuClus->NewCluster(iSensor);
   
   for (Int_t iPix = 0; iPix < listOfPixels->GetEntries(); ++iPix) {
      TAIThit* pixel = (TAIThit*)listOfPixels->At(iPix);
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if(!CheckLine(line)) continue;
      if(!CheckCol(col)) continue;
      
      Int_t clusterN = GetClusterNumber(line, col, thr);
      if ( clusterN != -1 ) {
         pthread_mutex_lock(&fLock);
         cluster1 = pNtuClus->GetCluster(iSensor, clusterN);
         cluster1->AddPixel(pixel);
         pthread_mutex_unlock(&fLock);
      }
   }
   
   // Compute position and fill clusters info
   TAVTbaseCluster* cluster = 0x0;
   
   for (Int_t i = 0; i< pNtuClus->GetClustersN(iSensor); ++i) {
      pthread_mutex_lock(&fLock);
      cluster = pNtuClus->GetCluster(iSensor, i);
      FillClusterInfo(iSensor, cluster);
      pthread_mutex_unlock(&fLock);
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

//______________________________________________________________________________
//! Get list of pixels for a given sensor
//!
//! \param[in] sensorId index of sensor
TClonesArray* TAITactNtuClusterMT::GetListOfPixels(Int_t sensorId)
{
   TAITntuHit* pNtuHit  = (TAITntuHit*) fpNtuRaw->Object();
   
   return pNtuHit->GetListOfPixels(sensorId);
}
