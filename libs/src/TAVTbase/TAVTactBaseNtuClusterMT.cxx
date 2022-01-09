/*!
 \file TAVTactBaseNtuClusterMT.cxx
 \brief  Base action for MT clustering
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
#include "TAVTactBaseNtuClusterMT.hxx"

/*!
 \class TAVTactBaseNtuClusterMT
 \brief Base action for MT clustering
 */

ClassImp(TAVTactBaseNtuClusterMT);

Int_t TAVTactBaseNtuClusterMT::fgSensorOff = 0;

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactBaseNtuClusterMT::TAVTactBaseNtuClusterMT(const char* name,
									 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
: TAVTactBaseClusterMT(name, pConfig, pGeoMap),
  fOk(true)
{
   
   for (Int_t i = 0; i < fThreadsN; ++i)
      fThread[i] = 0x0;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseNtuClusterMT::~TAVTactBaseNtuClusterMT()
{
}

//______________________________________________________________________________
//
void* TAVTactBaseNtuClusterMT::Thread0(void* arg)
{
   Int_t thr = 0;
   Int_t sensorId = fgSensorOff+thr;
   TAVTactBaseNtuClusterMT* action = (TAVTactBaseNtuClusterMT*)arg;
   pthread_mutex_lock(&fLock);
   TClonesArray* listOfPixels = action->GetListOfPixels(sensorId);
   pthread_mutex_unlock(&fLock);

   if (listOfPixels->GetEntries() == 0) return 0x0;
   action->fOk += action->FindClusters(sensorId, listOfPixels, thr);

   return 0x0;
}

//______________________________________________________________________________
//
void* TAVTactBaseNtuClusterMT::Thread1(void* arg)
{
   Int_t thr = 1;
   Int_t sensorId = fgSensorOff+thr;
   TAVTactBaseNtuClusterMT* action = (TAVTactBaseNtuClusterMT*)arg;
   pthread_mutex_lock(&fLock);
   TClonesArray* listOfPixels = action->GetListOfPixels(sensorId);
   pthread_mutex_unlock(&fLock);

   if (listOfPixels->GetEntries() == 0) return 0x0;
   action->fOk += action->FindClusters(sensorId, listOfPixels, thr);
   
   return 0x0;
}

//______________________________________________________________________________
//
void* TAVTactBaseNtuClusterMT::Thread2(void* arg)
{
   Int_t thr = 2;
   Int_t sensorId = fgSensorOff+thr;
   TAVTactBaseNtuClusterMT* action = (TAVTactBaseNtuClusterMT*)arg;
   pthread_mutex_lock(&fLock);
   TClonesArray* listOfPixels = action->GetListOfPixels(sensorId);
   pthread_mutex_unlock(&fLock);

   if (listOfPixels->GetEntries() == 0) return 0x0;
   action->fOk += action->FindClusters(sensorId, listOfPixels, thr);
   
   return 0x0;
}

//______________________________________________________________________________
//
void* TAVTactBaseNtuClusterMT::Thread3(void* arg)
{
   Int_t thr = 3;
   Int_t sensorId = fgSensorOff+thr;
   TAVTactBaseNtuClusterMT* action = (TAVTactBaseNtuClusterMT*)arg;
   pthread_mutex_lock(&fLock);
   TClonesArray* listOfPixels = action->GetListOfPixels(sensorId);
   pthread_mutex_unlock(&fLock);

   if (listOfPixels->GetEntries() == 0) return 0x0;
   action->fOk += action->FindClusters(sensorId, listOfPixels, thr);
   
   return 0x0;
}

//______________________________________________________________________________
//
void* TAVTactBaseNtuClusterMT::Thread4(void* arg)
{
   Int_t thr = 4;
   Int_t sensorId = fgSensorOff+thr;
   TAVTactBaseNtuClusterMT* action = (TAVTactBaseNtuClusterMT*)arg;
   pthread_mutex_lock(&fLock);
   TClonesArray* listOfPixels = action->GetListOfPixels(sensorId);
   pthread_mutex_unlock(&fLock);
   
   if (listOfPixels->GetEntries() == 0) return 0x0;
   action->fOk += action->FindClusters(sensorId, listOfPixels, thr);
   
   return 0x0;
}

//______________________________________________________________________________
//
void* TAVTactBaseNtuClusterMT::Thread5(void* arg)
{
   Int_t thr = 5;
   Int_t sensorId = fgSensorOff+thr;
   TAVTactBaseNtuClusterMT* action = (TAVTactBaseNtuClusterMT*)arg;
   pthread_mutex_lock(&fLock);
   TClonesArray* listOfPixels = action->GetListOfPixels(sensorId);
   pthread_mutex_unlock(&fLock);
   
   if (listOfPixels->GetEntries() == 0) return 0x0;
   action->fOk += action->FindClusters(sensorId, listOfPixels, thr);
   
   return 0x0;
}

//______________________________________________________________________________
//
void* TAVTactBaseNtuClusterMT::Thread6(void* arg)
{
   Int_t thr = 6;
   Int_t sensorId = fgSensorOff+thr;
   TAVTactBaseNtuClusterMT* action = (TAVTactBaseNtuClusterMT*)arg;
   pthread_mutex_lock(&fLock);
   TClonesArray* listOfPixels = action->GetListOfPixels(sensorId);
   pthread_mutex_unlock(&fLock);
   
   if (listOfPixels->GetEntries() == 0) return 0x0;
   action->fOk += action->FindClusters(sensorId, listOfPixels, thr);
   
   return 0x0;
}

//______________________________________________________________________________
//
void* TAVTactBaseNtuClusterMT::Thread7(void* arg)
{
   Int_t thr = 7;
   Int_t sensorId = fgSensorOff+thr;
   TAVTactBaseNtuClusterMT* action = (TAVTactBaseNtuClusterMT*)arg;
   pthread_mutex_lock(&fLock);
   TClonesArray* listOfPixels = action->GetListOfPixels(sensorId);
   pthread_mutex_unlock(&fLock);
   
   if (listOfPixels->GetEntries() == 0) return 0x0;
   action->fOk += action->FindClusters(sensorId, listOfPixels, thr);
   
   return 0x0;
}

//______________________________________________________________________________
//
Bool_t TAVTactBaseNtuClusterMT::ThreadStart()
{
   for (Int_t i = 0; i < fThreadsN; ++i) {
      if (!fThread[i]) {
         fThread[i] = new pthread_t;
         if (i == 0)
            pthread_create(fThread[i], 0x0, Thread0, (void*)this);
         else if (i == 1)
            pthread_create(fThread[i], 0x0, Thread1, (void*)this);
         else if (i == 2)
            pthread_create(fThread[i], 0x0, Thread2, (void*)this);
         else if (i == 3)
            pthread_create(fThread[i], 0x0, Thread3, (void*)this);
         else if (i == 4)
            pthread_create(fThread[i], 0x0, Thread4, (void*)this);
         else if (i == 5)
            pthread_create(fThread[i], 0x0, Thread5, (void*)this);
         else if (i == 6)
            pthread_create(fThread[i], 0x0, Thread6, (void*)this);
         else if (i == 7)
            pthread_create(fThread[i], 0x0, Thread7, (void*)this);
         else
            Error("ThreadStart", "No thread %d!", i);
      } else
         return false;
   }
   
   return true;
}

//______________________________________________________________________________
//
Bool_t TAVTactBaseNtuClusterMT::ThreadStop()
{
   for (Int_t i = 0; i < fThreadsN; ++i) {
      if (fThread[i]) {
         delete fThread[i];
         fThread[i] = 0x0;
      } else
         return false;
   }
   
   return true;
}

//______________________________________________________________________________
//
Bool_t TAVTactBaseNtuClusterMT::ThreadJoin()
{
   for (Int_t i = 0; i < fThreadsN; ++i) {
      if (fThread[i]) {
            pthread_join(*fThread[i], 0x0);
      } else
         return false;
   }
   
   return true;
}
