/*!
 \file
 \version $Id: TAVTactBaseNtuClusterMT.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAVTactBaseNtuClusterMT.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TThread.h"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactBaseNtuClusterMT.hxx"

/*!
 \class TAVTactBaseNtuClusterMT
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAVTactBaseNtuClusterMT);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAVTactBaseNtuClusterMT::TAVTactBaseNtuClusterMT(const char* name,
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
   fDimY = geoMap->GetNPixelY()+1;
   fDimX = geoMap->GetNPixelX()+1;
   
   for (Int_t i = 0; i < fgMaxThread; ++i) {
      SetupMaps(fDimY*fDimX, i);
      fClustersN[i] = 0;
   }
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseNtuClusterMT::~TAVTactBaseNtuClusterMT()
{
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseNtuClusterMT::CreateHistogram()
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
	  if (TAVTparConf::IsMapHistOn()) {
		 fpHisClusMap[i] = new TH2F(Form("%sClusMap%d", fPrefix.Data(), i+1), Form("%s - clusters map for sensor %d", fTitleDev.Data(), i+1),
									100, -pGeoMap->GetPitchX()*pGeoMap->GetNPixelX()/2., pGeoMap->GetPitchY()*pGeoMap->GetNPixelX()/2.,
									100, -pGeoMap->GetPitchY()*pGeoMap->GetNPixelX()/2., pGeoMap->GetPitchY()*pGeoMap->GetNPixelX()/2.);
		 fpHisClusMap[i]->SetMarkerStyle(20);
		 fpHisClusMap[i]->SetMarkerSize(.6);
		 fpHisClusMap[i]->SetMarkerColor(1);
		 fpHisClusMap[i]->SetStats(kFALSE);
		 AddHistogram(fpHisClusMap[i]);
	  }
   }
   
   SetValidHistogram(kTRUE);
   
   return;
}

//______________________________________________________________________________
//
void TAVTactBaseNtuClusterMT::FillMaps(TClonesArray* listOfPixels, Int_t thr)
{
   // Clear maps
   ClearMaps(thr);
   
   if (listOfPixels->GetEntries() == 0) return;
   
   // fill maps for cluster
   for (Int_t i = 0; i < listOfPixels->GetEntries(); i++) { // loop over hit pixels
      
      TAVTbaseNtuHit* pixel = (TAVTbaseNtuHit*)listOfPixels->At(i);
      if (!pixel->IsValidFrames()) continue;
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      TThread::Lock();
      TAGactNtuClusterMT::FillMaps(line, col, i, thr);
      TThread::UnLock();
   }
}

//______________________________________________________________________________
//
void TAVTactBaseNtuClusterMT::SearchCluster(TClonesArray* listOfPixels, Int_t thr)
{
   fClustersN[thr] = 0;
   // Search for cluster
   
   Int_t pixelsN =  listOfPixels->GetEntries();
   
   printf("titi %d\n", pixelsN);


   for (Int_t iPix = 0; iPix < pixelsN; ++iPix) { // loop over hit pixels

      TThread::Lock();
      TAVTbaseNtuHit* pixel = (TAVTbaseNtuHit*)listOfPixels->At(iPix);
      TThread::UnLock();
      
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
TAGobject*  TAVTactBaseNtuClusterMT::GetHitObject(Int_t idx, TClonesArray* listOfPixels) const
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
Bool_t TAVTactBaseNtuClusterMT::ApplyCuts(TAVTbaseCluster* cluster)
{
   TAVTbaseParConf* pConfig = (TAVTbaseParConf*) fpConfig->Object();
   
   TClonesArray* list = cluster->GetListOfPixels();
   Int_t  entries = list->GetEntries();
   
   // cuts on pixels in cluster
   if(entries < pConfig->GetSensorPar(cluster->GetPlaneNumber()).MinNofPixelsInCluster ||
      entries > pConfig->GetSensorPar(cluster->GetPlaneNumber()).MaxNofPixelsInCluster)
      return kFALSE;
   
   return kTRUE;
}

//______________________________________________________________________________
//
void TAVTactBaseNtuClusterMT::ComputePosition()
{
   ComputeCoGPosition();
}

//______________________________________________________________________________
//
void TAVTactBaseNtuClusterMT::ComputeSeedPosition()
{
   fCurrentPosition.SetXYZ((fPSeed->GetPosition())(0), (fPSeed->GetPosition())(1), 0);   
}

//______________________________________________________________________________
//
void TAVTactBaseNtuClusterMT::ComputeCoGPosition()
{
   if (!fCurListOfPixels) return;
   
   TVector3 tCorrection, tCorrection2, tCorTemp;
   TVector3 pos, posErr;
   tCorrection.SetXYZ( 0., 0., 0.);
   tCorrection2.SetXYZ( 0., 0., 0.);
   
   fClusterPulseSum = 0.;
   
   
   for (Int_t i = 0; i < fCurListOfPixels->GetEntries(); ++i) {
      TAVTntuHit* pixel = (TAVTntuHit*)fCurListOfPixels->At(i);
      tCorTemp.SetXYZ(pixel->GetPosition()(0)*pixel->GetPulseHeight(), pixel->GetPosition()(1)*pixel->GetPulseHeight(), pixel->GetPosition()(2));
      tCorrection  += tCorTemp;
      fClusterPulseSum  += pixel->GetPulseHeight();
	  }
   
   pos = tCorrection*(1./fClusterPulseSum);
   
   for (Int_t i = 0; i < fCurListOfPixels->GetEntries(); ++i) {
	  TAVTntuHit* pixel = (TAVTntuHit*)fCurListOfPixels->At(i);
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
void TAVTactBaseNtuClusterMT::FillClusterInfo(Int_t iSensor, TAVTbaseCluster* cluster)
{
   TAVTbaseParGeo* pGeoMap  = (TAVTbaseParGeo*) fpGeoMap->Object();
   
   cluster->SetPlaneNumber(iSensor);
   fCurListOfPixels = cluster->GetListOfPixels();
   ComputePosition();
   TVector3 posG = GetCurrentPosition();
   posG = pGeoMap->Sensor2Detector(iSensor, posG);
   cluster->SetPositionG(posG);
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
      cluster->SetValid(true);
   } else {
      cluster->SetValid(false);
   }
}
