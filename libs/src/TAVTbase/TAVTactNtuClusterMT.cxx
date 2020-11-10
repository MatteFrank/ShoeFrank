/*!
 \file
 \version $Id: TAVTactNtuClusterMT.cxx $
 \brief   Implementation of TAVTactNtuClusterMT.
 */

#include <pthread.h>

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactNtuClusterMT.hxx"

/*!
 \class TAVTactNtuClusterMT
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAVTactNtuClusterMT);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactNtuClusterMT::TAVTactNtuClusterMT(const char* name,
									 TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus,
									 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
: TAVTactBaseNtuClusterMT(name, pConfig, pGeoMap),
  fpNtuRaw(pNtuRaw),
  fpNtuClus(pNtuClus)
{
   AddDataIn(pNtuRaw,   "TAVTntuRaw");
   AddDataOut(pNtuClus, "TAVTntuCluster");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuClusterMT::~TAVTactNtuClusterMT()
{
}


//______________________________________________________________________________
//
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
//  
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
//
Bool_t TAVTactNtuClusterMT::CreateClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr)
{
   TAVTntuCluster* pNtuClus = (TAVTntuCluster*) fpNtuClus->Object();
   TAVTcluster* cluster1    = 0x0;

   // create clusters
   for (Int_t i = 0; i< fClustersN[thr]; ++i)
      pNtuClus->NewCluster(iSensor);
   
   for (Int_t iPix = 0; iPix < listOfPixels->GetEntries(); ++iPix) {
      TAVTntuHit* pixel = (TAVTntuHit*)listOfPixels->At(iPix);
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
//
TClonesArray* TAVTactNtuClusterMT::GetListOfPixels(Int_t sensorId)
{
   TAVTntuRaw* pNtuHit  = (TAVTntuRaw*) fpNtuRaw->Object();

   return pNtuHit->GetListOfPixels(sensorId);
}
