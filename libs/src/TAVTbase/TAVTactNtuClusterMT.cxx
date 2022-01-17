/*!
 \file TAVTactNtuClusterMT.cxx
 \brief   Implementation of TAVTactNtuClusterMT.
 */

#include <pthread.h>

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactNtuClusterMT.hxx"

/*!
 \class TAVTactNtuClusterMT
 \brief NTuplizer for VTX cluster in Multi-thread mode
 */

ClassImp(TAVTactNtuClusterMT);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit input container descriptor
//! \param[out] pNtuClus cluster output container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
TAVTactNtuClusterMT::TAVTactNtuClusterMT(const char* name,
									 TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus,
									 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
: TAVTactBaseNtuClusterMT(name, pConfig, pGeoMap),
  fpNtuRaw(pNtuRaw),
  fpNtuClus(pNtuClus)
{
   AddDataIn(pNtuRaw,   "TAVTntuHit");
   AddDataOut(pNtuClus, "TAVTntuCluster");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuClusterMT::~TAVTactNtuClusterMT()
{
}


//______________________________________________________________________________
//! Action
Bool_t TAVTactNtuClusterMT::Action()
{
   fgSensorOff = 0;
   
   ThreadStart();
   ThreadJoin();
   ThreadStop();
   
   fpNtuClus->SetBit(kValid);
   
   return true;
}

//______________________________________________________________________________
//! Find clusters for a given sensor and thread
//!
//! \param[in] iSensor index of sensor
//! \param[in] listOfPixels list of pixels
//! \param[in] thr index of thread
Bool_t TAVTactNtuClusterMT::FindClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr)
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
Bool_t TAVTactNtuClusterMT::CreateClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr)
{
   TAVTntuCluster* pNtuClus = (TAVTntuCluster*) fpNtuClus->Object();
   TAVTcluster* cluster1    = 0x0;

   // create clusters
   for (Int_t i = 0; i< fClustersN[thr]; ++i)
      pNtuClus->NewCluster(iSensor);
   
   for (Int_t iPix = 0; iPix < listOfPixels->GetEntries(); ++iPix) {
      TAVThit* pixel = (TAVThit*)listOfPixels->At(iPix);
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

   pNtuClus->GetListOfClusters(iSensor)->Compress();

   if (pNtuClus->GetClustersN(iSensor))
      return true;
   
   return false;
}

//______________________________________________________________________________
//! Get list of pixels for a given sensor
//!
//! \param[in] sensorId index of sensor
TClonesArray* TAVTactNtuClusterMT::GetListOfPixels(Int_t sensorId)
{
   TAVTntuHit* pNtuHit  = (TAVTntuHit*) fpNtuRaw->Object();

   return pNtuHit->GetListOfPixels(sensorId);
}
