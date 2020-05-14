/*!
 \file
 \version $Id: TAVTactNtuClusterMT.cxx $
 \brief   Implementation of TAVTactNtuClusterMT.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TThread.h"

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
  fpNtuClus(pNtuClus),
  fOk(true)
{
   AddDataIn(pNtuRaw,   "TAVTntuRaw");
   AddDataOut(pNtuClus, "TAVTntuCluster");
   
   for (Int_t i = 0; i < fgMaxThread; ++i)
      fThread[i] = 0x0;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuClusterMT::~TAVTactNtuClusterMT()
{
}

//______________________________________________________________________________
//
void TAVTactNtuClusterMT::Thread0(void* arg)
{
   Int_t thr = 0;
   Int_t sensorId = 0;
   TAVTactNtuClusterMT* action = (TAVTactNtuClusterMT*)arg;
   TThread::Lock();
   TAVTntuRaw* pNtuHit  = (TAVTntuRaw*) action->fpNtuRaw->Object();
   TThread::UnLock();

   TClonesArray* listOfPixels = pNtuHit->GetListOfPixels(sensorId);
   printf("thread0, pixels: %d\n", listOfPixels->GetEntries());
   if (listOfPixels->GetEntries() == 0) return;
   action->fOk += action->FindClusters(sensorId, listOfPixels, thr);

}

//______________________________________________________________________________
//
void TAVTactNtuClusterMT::Thread1(void* arg)
{
   Int_t thr = 1;
   Int_t sensorId = 1;
   TAVTactNtuClusterMT* action = (TAVTactNtuClusterMT*)arg;
   TThread::Lock();
   TAVTntuRaw* pNtuHit  = (TAVTntuRaw*) action->fpNtuRaw->Object();
   TThread::UnLock();
   
   TClonesArray* listOfPixels = pNtuHit->GetListOfPixels(sensorId);
   printf("thread1, pixels: %d\n", listOfPixels->GetEntries());
   if (listOfPixels->GetEntries() == 0) return;
   action->fOk += action->FindClusters(sensorId, listOfPixels, thr);
}

//______________________________________________________________________________
//
void TAVTactNtuClusterMT::Thread2(void* arg)
{
   Int_t thr = 2;
   Int_t sensorId = 2;
   TAVTactNtuClusterMT* action = (TAVTactNtuClusterMT*)arg;
   TThread::Lock();
   TAVTntuRaw* pNtuHit  = (TAVTntuRaw*) action->fpNtuRaw->Object();
   TThread::UnLock();
   
   TClonesArray* listOfPixels = pNtuHit->GetListOfPixels(sensorId);
   printf("thread2, pixels: %d\n", listOfPixels->GetEntries());
   if (listOfPixels->GetEntries() == 0) return;
    action->fOk += action->FindClusters(sensorId, listOfPixels, thr);
}

//______________________________________________________________________________
//
void TAVTactNtuClusterMT::Thread3(void* arg)
{
   Int_t thr = 3;
   Int_t sensorId = 3;
   TAVTactNtuClusterMT* action = (TAVTactNtuClusterMT*)arg;
   TThread::Lock();
   TAVTntuRaw* pNtuHit  = (TAVTntuRaw*) action->fpNtuRaw->Object();
   TThread::UnLock();
   
   TClonesArray* listOfPixels = pNtuHit->GetListOfPixels(sensorId);
   printf("thread3, pixels: %d\n", listOfPixels->GetEntries());
   if (listOfPixels->GetEntries() == 0) return;
    action->fOk += action->FindClusters(sensorId, listOfPixels, thr);
}

//______________________________________________________________________________
//
Bool_t TAVTactNtuClusterMT::ThreadStart()
{
   for (Int_t i = 0; i < fgMaxThread; ++i) {
      if (!fThread[i]) {
         if (i == 0)
            fThread[i] = new TThread(Form("thread%d", i), TAVTactNtuClusterMT::Thread0, (void*)this);
         else if (i == 1)
            fThread[i] = new TThread(Form("thread%d", i), TAVTactNtuClusterMT::Thread1, (void*)this);
         else if (i == 2)
            fThread[i] = new TThread(Form("thread%d", i), TAVTactNtuClusterMT::Thread2, (void*)this);
         else if (i == 3)
            fThread[i] = new TThread(Form("thread%d", i), TAVTactNtuClusterMT::Thread3, (void*)this);
         else
            Error("ThreadStart", "No thread %d!", i);
         fThread[i]->Run();
      } else
         return false;
   }
   
//   TThread::Ps();
   return true;
}

//______________________________________________________________________________
//
Bool_t TAVTactNtuClusterMT::ThreadStop()
{
   for (Int_t i = 0; i < fgMaxThread; ++i) {
      if (fThread[i]) {
        TThread::Delete(fThread[i]);
         delete fThread[i];
      } else
         return false;
   }
   
//   TThread::Ps();
   return true;
}

//______________________________________________________________________________
//
Bool_t TAVTactNtuClusterMT::Action()
{
   ThreadStart();
//   for (Int_t i = 0; i < fgMaxThread; ++i)
//      fThread[i]->Join();
  
   ThreadStop();

   if(fOk)
      fpNtuClus->SetBit(kValid);
   
   return true;
}

//______________________________________________________________________________
//  
Bool_t TAVTactNtuClusterMT::FindClusters(Int_t iSensor, TClonesArray* listOfPixels, Int_t thr)
{
   // Algo taking from Virgile BEKAERT (ImaBio @ IPHC-Strasbourg)
   // Look in a iterative way to next neighbour
   

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
   for (Int_t i = 0; i< fClustersN; ++i)
      pNtuClus->NewCluster(iSensor);
   
   for (Int_t iPix = 0; iPix < listOfPixels->GetEntries(); ++iPix) {
      TAVTntuHit* pixel = (TAVTntuHit*)listOfPixels->At(iPix);
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if(!CheckLine(line)) continue;
      if(!CheckCol(col)) continue;
      
      Int_t clusterN = GetClusterNumber(line,col, thr);
      if ( clusterN != -1 ) {
         TThread::Lock();
         cluster1 = pNtuClus->GetCluster(iSensor, clusterN);
         cluster1->AddPixel(pixel);
         TThread::UnLock();
      }
   }

   // Compute position and fill clusters info
   TAVTbaseCluster* cluster = 0x0;

   for (Int_t i = 0; i< pNtuClus->GetClustersN(iSensor); ++i) {
      TThread::Lock();
      cluster = pNtuClus->GetCluster(iSensor, i);
      FillClusterInfo(iSensor, cluster);
      TThread::UnLock();
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
