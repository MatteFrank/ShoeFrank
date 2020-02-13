/*!
 \file
 \version $Id: TAVTactNtuClusterF.cxx $
 \brief   Implementation of TAVTactNtuClusterF.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAGgeoTrafo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactNtuClusterF.hxx"

/*!
 \class TAVTactNtuClusterF 
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAVTactNtuClusterF);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactNtuClusterF::TAVTactNtuClusterF(const char* name, 
									 TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus,
									 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
: TAVTactBaseNtuCluster(name, pConfig, pGeoMap),
  fpNtuRaw(pNtuRaw),
  fpNtuClus(pNtuClus)
{
   AddDataIn(pNtuRaw,   "TAVTntuRaw");
   AddDataOut(pNtuClus, "TAVTntuCluster");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuClusterF::~TAVTactNtuClusterF()
{
   
}

//______________________________________________________________________________
//
Bool_t TAVTactNtuClusterF::Action()
{
   TAVTntuRaw* pNtuHit  = (TAVTntuRaw*) fpNtuRaw->Object();
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
Bool_t TAVTactNtuClusterF::FindClusters(Int_t iSensor)
{
   // Algo taking from Virgile BEKAERT (ImaBio @ IPHC-Strasbourg)
   // Look in a iterative way to next neighbour
   
   TAVTntuCluster* pNtuClus = (TAVTntuCluster*) fpNtuClus->Object();

   FillMaps();
   SearchCluster();
 
   return CreateClusters(iSensor, pNtuClus);
}

//______________________________________________________________________________
//
Bool_t TAVTactNtuClusterF::CreateClusters(Int_t iSensor, TAVTntuCluster* pNtuClus)
{
   TAVTparGeo* pGeoMap  = (TAVTparGeo*) fpGeoMap->Object();

   TAVTcluster* cluster = 0x0;

   // create clusters
   for (Int_t i = 0; i< fClustersN; ++i)
      pNtuClus->NewCluster(iSensor);
   
   for (Int_t iPix = 0; iPix < fListOfPixels->GetEntries(); ++iPix) {
      TAVTntuHit* pixel = (TAVTntuHit*)fListOfPixels->At(iPix);
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if(!CheckLine(line)) continue;
      if(!CheckCol(col)) continue;
      
      Int_t clusterN = GetClusterNumber(line,col);
      if ( clusterN != -1 ) {
         cluster = pNtuClus->GetCluster(iSensor, clusterN);
         cluster->AddPixel(pixel);
      }
   }
   
   // Compute position and fill clusters info
   for (Int_t i = 0; i< pNtuClus->GetClustersN(iSensor); ++i) {
      cluster = pNtuClus->GetCluster(iSensor, i);
      cluster->SetPlaneNumber(iSensor);
      fCurListOfPixels = cluster->GetListOfPixels();
      ComputePosition();
      TVector3 posG = *GetCurrentPosition();
      posG = pGeoMap->Sensor2Detector(iSensor, posG);
      cluster->SetPositionG(posG);
      cluster->SetPosition(*GetCurrentPosition());
      cluster->SetPosError(*GetCurrentPosError());
      cluster->SetCharge(GetClusterPulseSum());
      
      if (ApplyCuts(cluster)) {
         // histogramms
         if (ValidHistogram()) {
            if (cluster->GetPixelsN() > 0) {
               fpHisPixelTot->Fill(cluster->GetPixelsN());
               fpHisPixel[iSensor]->Fill(cluster->GetPixelsN());
               // printf("sensor %d %d\n", iSensor, cluster->GetPixelsN());
               if (TAVTparConf::IsMapHistOn()) {
                  fpHisClusMap[iSensor]->Fill(cluster->GetPosition()[0], cluster->GetPosition()[1]);
               }
            }
         }
         cluster->SetValid(true);
      } else {
         cluster->SetValid(false);
      }
   }
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
void TAVTactNtuClusterF::ComputePosition()
{
   TAVTactBaseNtuCluster::ComputePosition();
}
