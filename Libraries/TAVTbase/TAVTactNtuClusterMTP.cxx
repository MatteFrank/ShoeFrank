/*!
 \file TAVTactNtuClusterMTP.cxx
 \brief   Implementation of TAVTactNtuClusterMTP.
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
#include "TAVTactNtuClusterMTP.hxx"

/*!
 \class TAVTactNtuClusterMTP
 \brief NTuplizer for VTX cluster in Multi-thread mode
 */

Int_t TAVTactNtuClusterMTP::fgMaxThreadsN  = 32;
Int_t TAVTactNtuClusterMTP::fgkLimThreadsN = 8;

//! Class Imp
ClassImp(TAVTactNtuClusterMTP);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuRaw hit input container descriptor
//! \param[out] pNtuClus cluster output container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
TAVTactNtuClusterMTP::TAVTactNtuClusterMTP(const char* name,
									 TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus,
									 TAGparaDsc* config, TAGparaDsc* pGeoMap)
 : TAVTactBaseNtuCluster(name, config, pGeoMap),
   fpNtuRaw(pNtuRaw),
   fpNtuClus(pNtuClus),
   fThreadsN(fgMaxThreadsN),
   fThreadPool(new TThreadPool<TAVTtaskClusterMT, EProc>(fgMaxThreadsN))
{
   AddDataIn(pNtuRaw,   "TAVTntuHit");
   AddDataOut(pNtuClus, "TAVTntuCluster");
   
   TAVTparGeo* geoMap = (TAVTparGeo*)fpGeoMap->Object();
   fDimY = geoMap->GetPixelsNy()+1;
   fDimX = geoMap->GetPixelsNx()+1;
   SetupMaps(fDimY*fDimX);
   
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   fClusterVect = new Int_t[pConfig->GetSensorsN()];
   
   for (Int_t i = 0; i < min(pConfig->GetSensorsN(), fThreadsN); ++i)
      fThreads.push_back(TAVTtaskClusterMT(GetListOfPixels(i), fFlagMapVect[i], fDimX, fDimY, &fClusterVect[i]));
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuClusterMTP::~TAVTactNtuClusterMTP()
{
}

//______________________________________________________________________________
//! Action
Bool_t TAVTactNtuClusterMTP::Action()
{
   for (auto& t: fThreads)
      fThreadPool->PushTask(t, start);

   fThreadPool->Stop();
   
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   for (Int_t i = 0; i < pConfig->GetSensorsN(); ++i) {
      CreateClusters(i, GetListOfPixels(i));
   }
      
   fpNtuClus->SetBit(kValid);
   
   return true;
}

//______________________________________________________________________________
//! Create clusters for a given sensor, pixel list and thread
//!
//! \param[in] iSensor index of sensor
//! \param[in] listOfPixels list of pixels
//! \param[in] thr index of thread
Bool_t TAVTactNtuClusterMTP::CreateClusters(Int_t iSensor, TClonesArray* listOfPixels)
{
   TAVTntuCluster* pNtuClus = (TAVTntuCluster*) fpNtuClus->Object();
   TAVTcluster* cluster1    = 0x0;

   printf("cluster %d\n", fClusterVect[iSensor]);
   // create clusters
   for (Int_t i = 0; i< fClusterVect[iSensor]; ++i)
      pNtuClus->NewCluster(iSensor);

   for (Int_t iPix = 0; iPix < listOfPixels->GetEntriesFast(); ++iPix) {
      TAVThit* pixel = (TAVThit*)listOfPixels->At(iPix);
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();

      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      Int_t clusterN = GetClusterNumber(iSensor, line, col);
      if ( clusterN != -1 ) {
         cluster1 = pNtuClus->GetCluster(iSensor, clusterN);
         cluster1->AddPixel(pixel);
      }
   }

   // Compute position and fill clusters info
   TAVTbaseCluster* cluster = 0x0;

   for (Int_t i = 0; i< pNtuClus->GetClustersN(iSensor); ++i) {
      cluster = pNtuClus->GetCluster(iSensor, i);
      FillClusterInfo(iSensor, cluster);
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
TClonesArray* TAVTactNtuClusterMTP::GetListOfPixels(Int_t sensorId)
{
   TAVTntuHit* pNtuHit  = (TAVTntuHit*) fpNtuRaw->Object();

   return pNtuHit->GetListOfPixels(sensorId);
}

//______________________________________________________________________________
//! Set up map size
//!
//! \param[in] size map size
void TAVTactNtuClusterMTP::SetupMaps(Int_t size)
{
   fFlagSize = size;
   for (Int_t i = 0; i < fgMaxThreadsN; ++i) {
      fFlagMapVect.push_back(new Int_t[size]);
   }
}
