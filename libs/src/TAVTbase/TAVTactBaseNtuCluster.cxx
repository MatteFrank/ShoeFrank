/*!
 \file
 \version $Id: TAVTactBaseNtuCluster.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAVTactBaseNtuCluster.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactBaseNtuCluster.hxx"

/*!
 \class TAVTactBaseNtuCluster 
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAVTactBaseNtuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAVTactBaseNtuCluster::TAVTactBaseNtuCluster(const char* name, 
											 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
 : TAGactNtuCluster2D(name, "TAVTactNtuCluster - NTuplize cluster"),
   fpConfig(pConfig),
   fpGeoMap(pGeoMap),
   fCurrentPosition(new TVector3(0., 0., 0.)),
   fCurrentPosError(new TVector3(0., 0., 0.)),
   fListOfPixels(0x0),
   fClustersN(0)

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
   SetupMaps(fDimY*fDimX);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseNtuCluster::~TAVTactBaseNtuCluster()
{
   delete fCurrentPosition;
   delete fCurrentPosError;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseNtuCluster::CreateHistogram()
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
void TAVTactBaseNtuCluster::FillMaps()
{
   // Clear maps
   ClearMaps();
   
   if (fListOfPixels->GetEntries() == 0) return;
   
   // fill maps for cluster
   for (Int_t i = 0; i < fListOfPixels->GetEntries(); i++) { // loop over hit pixels
      
      TAVTbaseNtuHit* pixel = (TAVTbaseNtuHit*)fListOfPixels->At(i);
      if (!pixel->IsValidFrames()) continue;
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      TAGactNtuCluster2D::FillMaps(line, col, i);
   }
}

//______________________________________________________________________________
//
void TAVTactBaseNtuCluster::SearchCluster()
{
   fClustersN = 0;
   // Search for cluster
   
   for (Int_t iPix = 0; iPix < fListOfPixels->GetEntries(); ++iPix) { // loop over hit pixels
      TAVTbaseNtuHit* pixel = (TAVTbaseNtuHit*)fListOfPixels->At(iPix);
      if (pixel->Found()) continue;
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;
      
      // loop over lines & columns
      if ( ShapeCluster(fClustersN, line, col) )
         fClustersN++;
   }
}

//______________________________________________________________________________
// Get object in list
TAGobject*  TAVTactBaseNtuCluster::GetHitObject(Int_t idx) const
{
   if (idx >= 0 && idx < GetListOfPixels()->GetEntries() )
      return (TAGobject*)GetListOfPixels()->At(idx);
   
   else {
      Error("GetHitObject()", "Error in index %d (max: %d)", idx, GetListOfPixels()->GetEntries()-1);
      return 0x0;
   }   
}


//______________________________________________________________________________
//
Bool_t TAVTactBaseNtuCluster::ApplyCuts(TAVTbaseCluster* cluster)
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
void TAVTactBaseNtuCluster::ComputePosition()
{
   ComputeCoGPosition();
}

//______________________________________________________________________________
//
void TAVTactBaseNtuCluster::ComputeSeedPosition()
{
   GetCurrentPosition()->SetXYZ((fPSeed->GetPosition())(0), (fPSeed->GetPosition())(1), 0);   
}

//______________________________________________________________________________
//
void TAVTactBaseNtuCluster::ComputeCoGPosition()
{
   if (!fCurListOfPixels) return;
   
   TVector3 tCorrection, tCorrection2, tCorTemp;
   TVector3 pos, posErr;
   tCorrection.SetXYZ( 0., 0., 0.);
   tCorrection2.SetXYZ( 0., 0., 0.);
   
   Float_t tClusterPulseSum = 0.;
   
   
   for (Int_t i = 0; i < fCurListOfPixels->GetEntries(); ++i) {
      TAVTntuHit* pixel = (TAVTntuHit*)fCurListOfPixels->At(i);
      tCorTemp.SetXYZ(pixel->GetPosition()(0)*pixel->GetPulseHeight(), pixel->GetPosition()(1)*pixel->GetPulseHeight(), pixel->GetPosition()(2));
      tCorrection  += tCorTemp;
      tClusterPulseSum  += pixel->GetPulseHeight();
	  }
   
   pos = tCorrection*(1./tClusterPulseSum);
   
   for (Int_t i = 0; i < fCurListOfPixels->GetEntries(); ++i) {
	  TAVTntuHit* pixel = (TAVTntuHit*)fCurListOfPixels->At(i);
	  tCorrection2.SetXYZ(pixel->GetPulseHeight()*(pixel->GetPosition()(0)-(pos)(0))*(pixel->GetPosition()(0)-(pos)(0)), 
							pixel->GetPulseHeight()*(pixel->GetPosition()(1)-(pos)(1))*(pixel->GetPosition()(1)-(pos)(1)), 
							0);
	  posErr += tCorrection2;
   }
   
   posErr *= 1./tClusterPulseSum;
   
   // for cluster with a single pixel
   Float_t lim = 2.5e-7; // in cm !
   if (posErr(0) < lim) posErr(0) = lim; //(20/Sqrt(12)^2
   if (posErr(1) < lim) posErr(1) = lim; //(20/Sqrt(12)^2
   
   GetCurrentPosition()->SetXYZ((pos)(0), (pos)(1), 0);  
   GetCurrentPosError()->SetXYZ(TMath::Sqrt((posErr)(0)), TMath::Sqrt((posErr)(1)), 0);
}

