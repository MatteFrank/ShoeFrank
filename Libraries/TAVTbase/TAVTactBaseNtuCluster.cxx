/*!
 \file TAVTactBaseNtuCluster.cxx
 \brief   Base class to NTuplize cluster
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TCanvas.h"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactBaseNtuCluster.hxx"

/*!
 \class TAVTactBaseNtuCluster 
 \brief Base class to NTuplize cluster
 */

//! Class imp
ClassImp(TAVTactBaseNtuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
TAVTactBaseNtuCluster::TAVTactBaseNtuCluster(const char* name, 
											 TAGparaDsc* pConfig, TAGparaDsc* pGeoMap)
 : TAGactNtuCluster2D(name, "TAVTactNtuCluster - NTuplize cluster"),
   fpConfig(pConfig),
   fpGeoMap(pGeoMap),
   fCurrentPosition(0., 0., 0.),
   fCurrentPosError(0., 0., 0.),
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
   else if (fPrefix.Contains("mp"))
      fTitleDev = "Monopix2";
   else
      printf("Wrong prefix for histograms !");

   TAVTparGeo* geoMap = (TAVTparGeo*)fpGeoMap->Object();
   fDimY = geoMap->GetPixelsNy()+1;
   fDimX = geoMap->GetPixelsNx()+1;
   SetupMaps(fDimY*fDimX);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseNtuCluster::~TAVTactBaseNtuCluster()
{
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
      fpHisCharge[i] = new TH1F(Form("%sClusCharge%d",fPrefix.Data(), i+1), Form("%s - charge per clusters for sensor %d", fTitleDev.Data(), i+1), 1000, 0., 1000.);
      AddHistogram(fpHisCharge[i]);
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
//! Filling cluster maps.
void TAVTactBaseNtuCluster::FillMaps()
{
   // Clear maps
   ClearMaps();
   
   if (fListOfPixels->GetEntriesFast() == 0) return;
   
   // fill maps for cluster
   for (Int_t i = 0; i < fListOfPixels->GetEntriesFast(); i++) { // loop over hit pixels
      
      TAVTbaseHit* pixel = (TAVTbaseHit*)fListOfPixels->At(i);
      if (!pixel->IsValidFrames()) continue;
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;

      TAGactNtuCluster2D::FillMaps(line, col, i);
   }
}

//______________________________________________________________________________
//! Search for cluster.
void TAVTactBaseNtuCluster::SearchCluster()
{
   fClustersN = 0;
   // Search for cluster
   
   for (Int_t iPix = 0; iPix < fListOfPixels->GetEntriesFast(); ++iPix) { // loop over hit pixels
      TAVTbaseHit* pixel = (TAVTbaseHit*)fListOfPixels->At(iPix);
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
//! Get object from current pixel list
//!
//! \param[in] idx list index
TAGobject*  TAVTactBaseNtuCluster::GetHitObject(Int_t idx) const
{
   if (idx >= 0 && idx < GetListOfPixels()->GetEntriesFast() )
      return (TAGobject*)GetListOfPixels()->At(idx);
   
   else {
      Error("GetHitObject()", "Error in index %d (max: %d)", idx, GetListOfPixels()->GetEntriesFast()-1);
      return 0x0;
   }   
}

//______________________________________________________________________________
//! Apply cuts for a given cluster
//!
//! \param[in] cluster a given cluster
Bool_t TAVTactBaseNtuCluster::ApplyCuts(TAVTbaseCluster* cluster)
{
   TAVTbaseParConf* pConfig = (TAVTbaseParConf*) fpConfig->Object();
   
   TClonesArray* list = cluster->GetListOfPixels();
   Int_t  entries = list->GetEntriesFast();
   
   // cuts on pixels in cluster

  if(entries < pConfig->GetSensorPar(cluster->GetSensorIdx()).MinNofPixelsInCluster ||
      entries > pConfig->GetSensorPar(cluster->GetSensorIdx()).MaxNofPixelsInCluster)
      return kFALSE;
   
   return kTRUE;
}

//______________________________________________________________________________
//! Compute position
void TAVTactBaseNtuCluster::ComputePosition()
{
   ComputeCoGPosition();
}

//______________________________________________________________________________
//! Compute seed position
//!
//! taken position of first pixel
void TAVTactBaseNtuCluster::ComputeSeedPosition()
{
   fCurrentPosition.SetXYZ((fPSeed->GetPosition())(0), (fPSeed->GetPosition())(1), 0);   
}

//______________________________________________________________________________
//! Compute center of gravity position
void TAVTactBaseNtuCluster::ComputeCoGPosition()
{
   if (!fCurListOfPixels) return;
   
   TVector3 tCorrection, tCorrection2, tCorTemp;
   TVector3 pos, posErr;
   tCorrection.SetXYZ( 0., 0., 0.);
   tCorrection2.SetXYZ( 0., 0., 0.);
   
   fClusterPulseSum = 0.;
   
   
   for (Int_t i = 0; i < fCurListOfPixels->GetEntriesFast(); ++i) {
      TAVThit* pixel = (TAVThit*)fCurListOfPixels->At(i);
      tCorTemp.SetXYZ(pixel->GetPosition()(0)*pixel->GetPulseHeight(), pixel->GetPosition()(1)*pixel->GetPulseHeight(), pixel->GetPosition()(2));
      tCorrection  += tCorTemp;
      fClusterPulseSum  += pixel->GetPulseHeight();
	  }
   
   pos = tCorrection*(1./fClusterPulseSum);
   
   for (Int_t i = 0; i < fCurListOfPixels->GetEntriesFast(); ++i) {
	  TAVThit* pixel = (TAVThit*)fCurListOfPixels->At(i);
	  tCorrection2.SetXYZ(pixel->GetPulseHeight()*(pixel->GetPosition()(0)-(pos)(0))*(pixel->GetPosition()(0)-(pos)(0)), 
							pixel->GetPulseHeight()*(pixel->GetPosition()(1)-(pos)(1))*(pixel->GetPosition()(1)-(pos)(1)), 
							0);
	  posErr += tCorrection2;
   }

   posErr *= 1. / (fClusterPulseSum * fClusterPulseSum);   //variance of the mean 
   // for cluster with a single pixel
   Float_t lim = 2.5e-7; // in cm !
   if (posErr(0) < lim) posErr(0) = lim; //(20/Sqrt(12)^2
   if (posErr(1) < lim) posErr(1) = lim; //(20/Sqrt(12)^2

   fCurrentPosition.SetXYZ((pos)(0), (pos)(1), 0);
   fCurrentPosError.SetXYZ(TMath::Sqrt((posErr)(0)) * (1. / TMath::Sqrt(2)), TMath::Sqrt((posErr)(1)) * (1. / TMath::Sqrt(2)), 0); // multiplicative factor to make the pull with a sigma close to 1 according to gsi 2021 mc 400 mev/n
}

//______________________________________________________________________________
//! Compute center of gravity position
void TAVTactBaseNtuCluster::EstimateMedian()
{
   if (!fCurListOfPixels)
     return;

   vector<double> x_distrib,y_distrib;
   vector<double> x_distrib_dev, y_distrib_dev;

   vector<int> x_id, y_id;


   for (Int_t i = 0; i < fCurListOfPixels->GetEntriesFast(); ++i)
   {
     TAVThit *pixel = (TAVThit *)fCurListOfPixels->At(i);
     x_distrib.push_back(pixel->GetPosition()(0));
     y_distrib.push_back(pixel->GetPosition()(1));
     x_id.push_back(pixel->GetPixelColumn());
      }

   sort(x_distrib.begin(), x_distrib.end());
   sort(y_distrib.begin(), y_distrib.end());
   sort(x_id.begin(), x_id.end());
   
   auto x_median{0.0}, y_median{0.0};
   
   if (x_distrib.size() % 2 == 0) //if even
   {
     x_median = (x_distrib[(x_distrib.size() / 2) -1] + x_distrib[(x_distrib.size() / 2)]) /2.;
     y_median = (y_distrib[(y_distrib.size() / 2) - 1] + y_distrib[(y_distrib.size() / 2)]) / 2.;
   } else { //if odd
     x_median = x_distrib[(x_distrib.size() - 1) / 2];
     y_median = y_distrib[(y_distrib.size() - 1) / 2];
   }
   
   double rms_x{0}, rms_y{0};

   for (auto elem_x : x_distrib)
   {
     x_distrib_dev.push_back(abs(elem_x - x_median)); // residuals (deviations) from the data's median
     rms_x += ((elem_x - x_median) * (elem_x - x_median)); //mean square

   }

   for (auto elem_y : y_distrib)
   {
     y_distrib_dev.push_back(abs(elem_y - y_median)); // residuals (deviations) from the data's median
     rms_y += ((elem_y - y_median) * (elem_y - y_median)); // mean square
   }
   

   std::nth_element(x_distrib_dev.begin(), x_distrib_dev.begin() + x_distrib_dev.size() / 2, x_distrib_dev.end()); // sort the deviation
   std::nth_element(y_distrib_dev.begin(), y_distrib_dev.begin() + y_distrib_dev.size() / 2, y_distrib_dev.end()); // sort the deviation

   auto x_mad = x_distrib_dev[x_distrib.size() / 2]; // the median absolute deviation (MAD) is the median of absolute values with the residuals (deviations) from the data's median
   auto y_mad = y_distrib_dev[y_distrib.size() / 2]; // MAD = median(|x_i - X_median|)

   Float_t lim = 207e-7; // in cm !
   if (x_mad < lim) x_mad = lim;
   if (y_mad < lim) y_mad = lim;
   if (rms_x < lim) rms_x = lim;
   if (rms_y < lim) rms_y = lim;
   rms_x = TMath::Sqrt(rms_x / (x_distrib.size() * x_distrib.size())); // root mean square in x
   rms_y = TMath::Sqrt(rms_y / (y_distrib.size() * y_distrib.size())); // root mean square in y
   fCurrentPosition.SetXYZ(x_median, y_median, 0);
   fCurrentPosError.SetXYZ(rms_x, rms_y, 0);

   //cout << "cluster pos: " << x_median << " +- " << rms_x << "; " << y_median << " +- " << rms_y << endl;
}


//______________________________________________________________________________
//! Fill data members of a given cluster
//!
//! \param[in] iSensor index of sensor
//! \param[in] cluster a given cluster
void TAVTactBaseNtuCluster::FillClusterInfo(Int_t iSensor, TAVTbaseCluster* cluster)
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
            fpHisCharge[iSensor]->Fill(cluster->GetCharge());
            fpHisPixel[iSensor]->Fill(cluster->GetPixelsN());
            fpHisClusMap[iSensor]->Fill(cluster->GetPositionG()[0], cluster->GetPositionG()[1]);
         }
      }
      cluster->SetValid(true);
   } else {
      cluster->SetValid(false);
   }
}
