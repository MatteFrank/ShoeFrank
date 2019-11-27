/*!
 \file
 \version $Id: TAITactNtuClusterF.cxx $
 \brief   Implementation of TAITactNtuClusterF.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAITntuRaw.hxx"
#include "TAITntuRaw.hxx"
#include "TAITntuCluster.hxx"
#include "TAITactNtuClusterF.hxx"

/*!
 \class TAITactNtuClusterF 
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAITactNtuClusterF);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAITactNtuClusterF::TAITactNtuClusterF(const char* name, 
									 TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuClus,
									 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
: TAVTactBaseNtuCluster(name, pConfig, pGeoMap),
  fpNtuRaw(pNtuRaw),
  fpNtuClus(pNtuClus)
{
   AddDataIn(pNtuRaw,   "TAITntuRaw");
   AddDataOut(pNtuClus, "TAITntuCluster");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactNtuClusterF::~TAITactNtuClusterF()
{
   
}

//______________________________________________________________________________
//
Bool_t TAITactNtuClusterF::Action()
{
   TAITntuRaw* pNtuHit  = (TAITntuRaw*) fpNtuRaw->Object();
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
Bool_t TAITactNtuClusterF::FindClusters(Int_t iSensor)
{
   // Algo taking from Virgile BEKAERT (ImaBio @ IPHC-Strasbourg)
   // Look in a iterative way to next neighbour
   
   TAITntuCluster* pNtuClus = (TAITntuCluster*) fpNtuClus->Object();

   FillMaps();
   SearchCluster();
 
   return CreateClusters(iSensor, pNtuClus);
}

//______________________________________________________________________________
//
Bool_t TAITactNtuClusterF::CreateClusters(Int_t iSensor, TAITntuCluster* pNtuClus)
{
   TAVTbaseParGeo* pGeoMap  = (TAVTbaseParGeo*)     fpGeoMap->Object();
   TAITcluster* cluster = 0x0;

   // create clusters
   for (Int_t i = 0; i< fClustersN; ++i)
      pNtuClus->NewCluster(iSensor);
   
   for (Int_t iPix = 0; iPix < fListOfPixels->GetEntries(); ++iPix) {
      TAITntuHit* pixel = (TAITntuHit*)fListOfPixels->At(iPix);
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
      cluster->SetPositionG(&posG);
      cluster->SetPosition(GetCurrentPosition());
      cluster->SetPosError(GetCurrentPosError());
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
      } else {
         pNtuClus->GetListOfClusters(iSensor)->Remove(cluster);
         pNtuClus->GetListOfClusters(iSensor)->Compress();
      }
   }
   if (pNtuClus->GetClustersN(iSensor))
      return true;
   
   return false;
}

//______________________________________________________________________________
//
void TAITactNtuClusterF::ComputePosition()
{
   TAVTactBaseNtuCluster::ComputePosition();
}
