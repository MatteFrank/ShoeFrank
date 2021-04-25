/*!
 \file
 \version $Id: TAVTactNtuCluster.cxx $
 \brief   Implementation of TAVTactNtuCluster.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAGgeoTrafo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactNtuCluster.hxx"

/*!
 \class TAVTactNtuCluster 
 \brief NTuplizer for vertex clusters. **
 */

ClassImp(TAVTactNtuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactNtuCluster::TAVTactNtuCluster(const char* name, 
									 TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus,
									 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
: TAVTactBaseNtuCluster(name, pConfig, pGeoMap),
  fpNtuRaw(pNtuRaw),
  fpNtuClus(pNtuClus)
{
   AddDataIn(pNtuRaw,   "TAVTntuHit");
   AddDataOut(pNtuClus, "TAVTntuCluster");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuCluster::~TAVTactNtuCluster()
{
   
}

//______________________________________________________________________________
//
Bool_t TAVTactNtuCluster::Action()
{
   TAVTntuHit* pNtuHit  = (TAVTntuHit*) fpNtuRaw->Object();
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   Bool_t ok = true;
   
   for (Int_t i = 0; i < pConfig->GetSensorsN(); ++i) {
      fListOfPixels = pNtuHit->GetListOfPixels(i);
      if (fListOfPixels->GetEntries() == 0) continue;
      ok += FindClusters(i);
   }
   
   if(ok)
      fpNtuClus->SetBit(kValid);
   
   return ok;
}

//______________________________________________________________________________
//  
Bool_t TAVTactNtuCluster::FindClusters(Int_t iSensor)
{
   // Algo taking from Virgile BEKAERT (ImaBio @ IPHC-Strasbourg)
   // Look in a iterative way to next neighbour
   

   FillMaps();
   SearchCluster();
 
   return CreateClusters(iSensor);
}

//______________________________________________________________________________
//
Bool_t TAVTactNtuCluster::CreateClusters(Int_t iSensor)
{
   TAVTntuCluster* pNtuClus = (TAVTntuCluster*) fpNtuClus->Object();
   TAVTcluster* cluster1    = 0x0;

   // create clusters
   for (Int_t i = 0; i< fClustersN; ++i)
      pNtuClus->NewCluster(iSensor);
   
   for (Int_t iPix = 0; iPix < fListOfPixels->GetEntries(); ++iPix) {
      TAVThit* pixel = (TAVThit*)fListOfPixels->At(iPix);
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
