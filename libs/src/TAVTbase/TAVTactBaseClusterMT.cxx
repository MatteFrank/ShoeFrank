/*!
 \file
 \version $Id: TAVTactBaseClusterMT.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAVTactBaseClusterMT.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TThread.h"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactBaseClusterMT.hxx"

/*!
 \class TAVTactBaseClusterMT
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAVTactBaseClusterMT);

pthread_mutex_t TAVTactBaseClusterMT::fLock;

//------------------------------------------+-----------------------------------
//! Default constructor.

TAVTactBaseClusterMT::TAVTactBaseClusterMT(const char* name,
											 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
 : TAGactNtuClusterMT(name, "TAVTactNtuCluster - NTuplize cluster"),
   fpConfig(pConfig),
   fpGeoMap(pGeoMap),
   fCurrentPosition(0., 0., 0.),
   fCurrentPosError(0., 0., 0.),
   fCurListOfPixels(0x0)
{
   AddPara(pGeoMap, "TAVTbaseParGeo");
   AddPara(pConfig, "TAVTbaseParConf");
   
   TString tmp(name);
   fPrefix = tmp(0,2);
   
   fTitleDev = fPrefix;
   if (fPrefix.Contains("vt"))
      fTitleDev = "Vertex";
   else if (fPrefix.Contains("it"))
      fTitleDev = "Inner Tracker";
   else
      printf("Wrong prefix for histograms !");

   TAVTparGeo* geoMap = (TAVTparGeo*)fpGeoMap->Object();
   fDimY = geoMap->GetPixelsNy()+1;
   fDimX = geoMap->GetPixelsNx()+1;
   
   // max threads
   fThreadsN = fgMaxThreadsN;
   if (fThreadsN > geoMap->GetSensorsN())
      fThreadsN =  geoMap->GetSensorsN();
   
   for (Int_t i = 0; i < fThreadsN; ++i) {
      SetupMaps(fDimY*fDimX, i);
      fClustersN[i] = 0;
   }
   
   if (pthread_mutex_init(&fLock, NULL) != 0)
      Error("TAVTactNtuClusterMT()", "Mutex Init failed");
   
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseClusterMT::~TAVTactBaseClusterMT()
{
   pthread_mutex_destroy(&fLock);
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseClusterMT::CreateHistogram()
{
   DeleteHistogram();
   fpHisPixelTot = new TH1F(Form("%sClusPixelTot", fPrefix.Data()), Form("%s - Total # pixels per clusters", fTitleDev.Data()), 100, 0., 100.);
   AddHistogram(fpHisPixelTot);
   
   TAVTbaseParGeo* pGeoMap  = (TAVTbaseParGeo*) fpGeoMap->Object();
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
	  fpHisPixel[i] = new TH1F(Form("%sClusPixel%d",fPrefix.Data(), i+1), Form("%s - # pixels per clusters for sensor %d", fTitleDev.Data(), i+1), 100, 0., 100.);
	  AddHistogram(fpHisPixel[i]);
   }
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
		 fpHisClusMap[i] = new TH2F(Form("%sClusMap%d", fPrefix.Data(), i+1), Form("%s - clusters map for sensor %d", fTitleDev.Data(), i+1),
									100, -pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx()/2., pGeoMap->GetPitchY()*pGeoMap->GetPixelsNx()/2.,
									100, -pGeoMap->GetPitchY()*pGeoMap->GetPixelsNx()/2., pGeoMap->GetPitchY()*pGeoMap->GetPixelsNx()/2.);
		 fpHisClusMap[i]->SetMarkerStyle(20);
		 fpHisClusMap[i]->SetMarkerSize(.6);
		 fpHisClusMap[i]->SetMarkerColor(1);
		 fpHisClusMap[i]->SetStats(kFALSE);
		 AddHistogram(fpHisClusMap[i]);
   }
   
   SetValidHistogram(kTRUE);
   
   return;
}

//______________________________________________________________________________
//
void TAVTactBaseClusterMT::FillMaps(TClonesArray* listOfPixels, Int_t thr)
{
   // Clear maps
   ClearMaps(thr);
   
   if (listOfPixels->GetEntries() == 0) return;
   
   // fill maps for cluster
   for (Int_t i = 0; i < listOfPixels->GetEntries(); i++) { // loop over hit pixels
      
      TAVTbaseHit* pixel = (TAVTbaseHit*)listOfPixels->At(i);
      if (!pixel->IsValidFrames()) continue;
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      TAGactNtuClusterMT::FillMaps(line, col, i, thr);
   }
}

//______________________________________________________________________________
//
void TAVTactBaseClusterMT::SearchCluster(TClonesArray* listOfPixels, Int_t thr)
{
   fClustersN[thr] = 0;
   // Search for cluster
   
   Int_t pixelsN =  listOfPixels->GetEntries();
   
   for (Int_t iPix = 0; iPix < pixelsN; ++iPix) { // loop over hit pixels

      pthread_mutex_lock(&fLock);
      TAVTbaseHit* pixel = (TAVTbaseHit*)listOfPixels->At(iPix);
      pthread_mutex_unlock(&fLock);

      if (pixel->Found()) continue;
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;
      
      // loop over lines & columns
      if ( ShapeCluster(fClustersN[thr], line, col, listOfPixels, thr) )
         fClustersN[thr]++;

   }
}

//______________________________________________________________________________
// Get object in list
TAGobject*  TAVTactBaseClusterMT::GetHitObject(Int_t idx, TClonesArray* listOfPixels) const
{
   if (idx >= 0 && idx < listOfPixels->GetEntries() )
      return (TAGobject*)listOfPixels->At(idx);
   
   else {
      Error("GetHitObject()", "Error in index %d (max: %d)", idx, listOfPixels->GetEntries()-1);
      return 0x0;
   }   
}


//______________________________________________________________________________
//
Bool_t TAVTactBaseClusterMT::ApplyCuts(TAVTbaseCluster* cluster)
{
   TAVTbaseParConf* pConfig = (TAVTbaseParConf*) fpConfig->Object();
   
   TClonesArray* list = cluster->GetListOfPixels();
   Int_t  entries = list->GetEntries();
   
   // cuts on pixels in cluster
   if(entries < pConfig->GetSensorPar(cluster->GetSensorIdx()).MinNofPixelsInCluster ||
      entries > pConfig->GetSensorPar(cluster->GetSensorIdx()).MaxNofPixelsInCluster)
      return kFALSE;
   
   return kTRUE;
}

//______________________________________________________________________________
//
void TAVTactBaseClusterMT::ComputePosition()
{
   ComputeCoGPosition();
}

//______________________________________________________________________________
//
void TAVTactBaseClusterMT::ComputeSeedPosition()
{
   fCurrentPosition.SetXYZ((fPSeed->GetPosition())(0), (fPSeed->GetPosition())(1), 0);   
}

//______________________________________________________________________________
//
void TAVTactBaseClusterMT::ComputeCoGPosition()
{
   if (!fCurListOfPixels) return;
   
   TVector3 tCorrection, tCorrection2, tCorTemp;
   TVector3 pos, posErr;
   tCorrection.SetXYZ( 0., 0., 0.);
   tCorrection2.SetXYZ( 0., 0., 0.);
   
   fClusterPulseSum = 0.;
   
   
   for (Int_t i = 0; i < fCurListOfPixels->GetEntries(); ++i) {
      TAVThit* pixel = (TAVThit*)fCurListOfPixels->At(i);
      tCorTemp.SetXYZ(pixel->GetPosition()(0)*pixel->GetPulseHeight(), pixel->GetPosition()(1)*pixel->GetPulseHeight(), pixel->GetPosition()(2));
      tCorrection  += tCorTemp;
      fClusterPulseSum  += pixel->GetPulseHeight();
	  }
   
   pos = tCorrection*(1./fClusterPulseSum);
   
   for (Int_t i = 0; i < fCurListOfPixels->GetEntries(); ++i) {
	  TAVThit* pixel = (TAVThit*)fCurListOfPixels->At(i);
	  tCorrection2.SetXYZ(pixel->GetPulseHeight()*(pixel->GetPosition()(0)-(pos)(0))*(pixel->GetPosition()(0)-(pos)(0)), 
							pixel->GetPulseHeight()*(pixel->GetPosition()(1)-(pos)(1))*(pixel->GetPosition()(1)-(pos)(1)), 
							0);
	  posErr += tCorrection2;
   }
   
   posErr *= 1./fClusterPulseSum;
   
   // for cluster with a single pixel
   Float_t lim = 2.5e-7; // in cm !
   if (posErr(0) < lim) posErr(0) = lim; //(20/Sqrt(12)^2
   if (posErr(1) < lim) posErr(1) = lim; //(20/Sqrt(12)^2
   
   fCurrentPosition.SetXYZ((pos)(0), (pos)(1), 0);
   fCurrentPosError.SetXYZ(TMath::Sqrt((posErr)(0)), TMath::Sqrt((posErr)(1)), 0);
}

//______________________________________________________________________________
//
void TAVTactBaseClusterMT::FillClusterInfo(Int_t iSensor, TAVTbaseCluster* cluster)
{
   TAVTbaseParGeo* pGeoMap  = (TAVTbaseParGeo*) fpGeoMap->Object();
   
   cluster->SetSensorIdx(iSensor);
   fCurListOfPixels = cluster->GetListOfPixels();
   ComputePosition();
   cluster->SetPosition(GetCurrentPosition());
   cluster->SetPosError(GetCurrentPosError());
   TVector3 posG = GetCurrentPosition();
   posG = pGeoMap->Sensor2Detector(iSensor, posG);
   cluster->SetPositionG(posG);
   cluster->SetCharge(GetClusterPulseSum());
   
   if (ApplyCuts(cluster)) {
      // histogramms
      if (ValidHistogram()) {
         if (cluster->GetPixelsN() > 0) {
            fpHisPixelTot->Fill(cluster->GetPixelsN());
            fpHisPixel[iSensor]->Fill(cluster->GetPixelsN());
            // printf("sensor %d %d\n", iSensor, cluster->GetPixelsN());
               fpHisClusMap[iSensor]->Fill(cluster->GetPosition()[0], cluster->GetPosition()[1]);
         }
      }
      cluster->SetValid(true);
   } else {
      cluster->SetValid(false);
   }
}
