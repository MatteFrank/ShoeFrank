/*!
 \file TAVTactBaseTrack.cxx
 \brief    Base class to NTuplizer VTX-IT-MSD track
 */
#include "TClonesArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TVector3.h"
#include "TVector2.h"

//BM
#include "TABMntuTrack.hxx"

// First
#include "TAGgeoTrafo.hxx"

//TAG
#include "TAGroot.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparCal.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactBaseTrack.hxx"

#include "TAVTbaseDigitizer.hxx"

//Fit
#include "TFitResult.h"

/*!
 \class TAVTactBaseTrack 
 \brief Base class to NTuplizer VTX-IT-MSD track
 */

//! Class Imp
ClassImp(TAVTactBaseTrack);

Bool_t TAVTactBaseTrack::fgRefit = true;

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuClus  cluster container descriptor
//! \param[out] pNtuTrack  track container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pCalib calibration parameter descriptor
TAVTactBaseTrack::TAVTactBaseTrack(const char* name,
										 TAGdataDsc* pNtuClus,TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig,  
										 TAGparaDsc* pGeoMap, TAGparaDsc* pCalib)
: TAGaction(name, "TAVTactNtuTrack - Base NTuplize Tracker"),
  fpNtuTrack(pNtuTrack),
  fpNtuClus(pNtuClus),
  fpConfig(pConfig),
  fpGeoMap(pGeoMap),
  fpCalib(pCalib),
  fTracksMaximum(100),
  fRequiredClusters(4),
  fSearchClusDistance(100),
  fGraphU(new TGraphErrors()),
  fGraphV(new TGraphErrors())
{
   AddPara(pGeoMap, "TAVTbaseParGeo");
   AddPara(pConfig, "TAVTbaseParConf");
   if (pCalib)
	  AddPara(pCalib, "TAVTbaseParCal");
   
   
   TString tmp(name);
   fPrefix = tmp(0,2);
   
   fTitleDev = fPrefix;
   if (fPrefix.Contains("vt"))
      fTitleDev = "Vertex";
   else if (fPrefix.Contains("it"))
      fTitleDev = "Inner Tracker";
   else if (fPrefix.Contains("ir"))
      fTitleDev = "Interaction Region";
   else if (fPrefix.Contains("ms"))
      fTitleDev = "Micro Strip Detector";
   else
      printf("Wrong prefix for histograms !");


   SetGeoTrafo(TAGgeoTrafo::GetDefaultActName().Data());
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseTrack::~TAVTactBaseTrack()
{   
   delete fGraphU;
   delete fGraphV;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseTrack::CreateHistogram()
{
   DeleteHistogram();   
   TAVTbaseParGeo* pGeoMap = GetParGeo();
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
	  
      fpHisTrackMap[i] = new TH2F(Form("%sTrackMap%d", fPrefix.Data(), i+1), Form("%s - Tracks map for sensor %d", fTitleDev.Data(), i+1),
                                  100, -pGeoMap->GetEpiSize()[0]/2., pGeoMap->GetEpiSize()[0]/2.,
                                  100, -pGeoMap->GetEpiSize()[1]/2., pGeoMap->GetEpiSize()[1]/2.);
      fpHisTrackMap[i]->SetMarkerStyle(24);
      fpHisTrackMap[i]->SetMarkerSize(1.);
      fpHisTrackMap[i]->SetMarkerColor(3);
      fpHisTrackMap[i]->SetStats(kFALSE);
      AddHistogram(fpHisTrackMap[i]);

      fpHisResX[i] = new TH1F(Form("%sResX%d", fPrefix.Data(), i + 1), Form("%s - ResidualX of sensor %d;[cm]; Counts", fTitleDev.Data(), i + 1), 1000, -0.01, 0.01);
      AddHistogram(fpHisResX[i]);
      fpHisResY[i] = new TH1F(Form("%sResY%d", fPrefix.Data(), i + 1), Form("%s - ResidualY of sensor %d;[cm]; Counts", fTitleDev.Data(), i + 1), 1000, -0.01, 0.01);
      AddHistogram(fpHisResY[i]);

      fpHisTrackClusSize[i] = new TH1F(Form("%sTrackClusSize%d", fPrefix.Data(), i + 1), Form("%s - Total # pixels per cluster of a track in sensor %d for specific tracks", fTitleDev.Data(), i + 1), 100, 0., 100.);
      AddHistogram(fpHisTrackClusSize[i]);
      fpHisTrackClusSizeAll[i] = new TH1F(Form("%sTrackClusSizeAll%d", fPrefix.Data(), i + 1), Form("%s - Total # pixels per cluster of a track in sensor %d", fTitleDev.Data(), i + 1), 100, 0., 100.);
      AddHistogram(fpHisTrackClusSizeAll[i]);

      // pull
      fpHisPullX[i] = new TH1F(Form("%sPullX%d", fPrefix.Data(), i + 1), Form("%s - PullX of sensor %d", fTitleDev.Data(), i + 1), 100, -5, 5);
      AddHistogram(fpHisPullX[i]);
      fpHisPullXcut[i] = new TH1F(Form("%sPullXcut%d", fPrefix.Data(), i + 1), Form("%s - cut PullX of sensor %d", fTitleDev.Data(), i + 1), 100, -5, 5);
      AddHistogram(fpHisPullXcut[i]);
      fpHisPullY[i] = new TH1F(Form("%sPullY%d", fPrefix.Data(), i + 1), Form("%s - pullY of sensor %d", fTitleDev.Data(), i + 1), 100, -5, 5); // ! range
      AddHistogram(fpHisPullY[i]);
      fpHisPullYcut[i] = new TH1F(Form("%sPullYcut%d", fPrefix.Data(), i + 1), Form("%s - cut pullY of sensor %d", fTitleDev.Data(), i + 1), 100, -5, 5); // ! range
      AddHistogram(fpHisPullYcut[i]);

      fpHisPullYvsChiY[i] = new TH2F(Form("%sPullYvsChiY%d", fPrefix.Data(), i + 1), Form("%s - pullY of sensor %d vs reduced chi2 in y; pull; chi2 ", fTitleDev.Data(), i + 1), 100, -5, 5, 1000, 0, 20); // ! range
      AddHistogram(fpHisPullYvsChiY[i]);
      fpHisPullXvsChiX[i] = new TH2F(Form("%sPullXvsChiX%d", fPrefix.Data(), i + 1), Form("%s - pullX of sensor %d vs reduced chi2 in X; pull; chi2 ", fTitleDev.Data(), i + 1), 100, -5, 5, 1000, 0, 20); // ! range
      AddHistogram(fpHisPullXvsChiX[i]);
      fpHisPullYvsPvalue[i] = new TH2F(Form("%sPullYvsP%d", fPrefix.Data(), i + 1), Form("%s - pullY of sensor %d vs pvalue in y; pull; pvalue ", fTitleDev.Data(), i + 1), 100, -5, 5, 1000, 0, 1); // ! range
      AddHistogram(fpHisPullYvsPvalue[i]);
      fpHisPullXvsPvalue[i] = new TH2F(Form("%sPullXvsP%d", fPrefix.Data(), i + 1), Form("%s - pullX of sensor %d vs pvalue in X; pull; pvalue ", fTitleDev.Data(), i + 1), 100, -5, 5, 1000, 0, 1); // ! range
      AddHistogram(fpHisPullXvsPvalue[i]);

      // meas position
      fpHisTrackClustPosX[i] = new TH1F(Form("%sTrackClusPosX%d", fPrefix.Data(), i + 1), Form("%s - Clus X position in sensor %d;[cm]; Counts", fTitleDev.Data(), i + 1), 1000, -5, 5);
      AddHistogram(fpHisTrackClustPosX[i]);
      fpHisTrackClustPosY[i] = new TH1F(Form("%sTrackClusPosY%d", fPrefix.Data(), i + 1), Form("%s - Clus Y position in sensor %d;[cm]; Counts", fTitleDev.Data(), i + 1), 1000, -5, 5); // ! range
      AddHistogram(fpHisTrackClustPosY[i]);

      // meas position resolution
      fpHisTrackClustPosErrX[i] = new TH1F(Form("%sTrackClusPosErrX%d", fPrefix.Data(), i + 1), Form("%s - Clus X position error in sensor %d;[cm]; Counts", fTitleDev.Data(), i + 1), 1000, 0., 0.002);
      AddHistogram(fpHisTrackClustPosErrX[i]);
      fpHisTrackClustPosErrY[i] = new TH1F(Form("%sTrackClusPosErrY%d", fPrefix.Data(), i + 1), Form("%s - Clus Y position error in sensor %d;[cm]; Counts", fTitleDev.Data(), i + 1), 1000, 0., 0.002); // ! range
      AddHistogram(fpHisTrackClustPosErrY[i]);
      fpHisTrackClustPosErrXvsPixel[i] = new TH2F(Form("%sTrackClusPosErrXvsPixel%d", fPrefix.Data(), i + 1), Form("%s - Clus X position error vs n pixels in sensor %d;n pixels; cm", fTitleDev.Data(), i + 1),100,0,100, 1000, 0., 0.002);
      AddHistogram(fpHisTrackClustPosErrXvsPixel[i]);
      fpHisTrackClustPosErrYvsPixel[i] = new TH2F(Form("%sTrackClusPosErrYvsPixel%d", fPrefix.Data(), i + 1), Form("%s - Clus Y position error vs n pixels in sensor %d;n pixels; cm", fTitleDev.Data(), i + 1),100,0,100, 1000, 0., 0.002);
      AddHistogram(fpHisTrackClustPosErrYvsPixel[i]);
      

      // fit position
      fpHisFitClustPosX[i] = new TH1F(Form("%sFitClusPosX%d", fPrefix.Data(), i + 1), Form("%s - Clus X position by fit in sensor %d;[cm]; Counts", fTitleDev.Data(), i + 1), 1000, -5, 5);
      AddHistogram(fpHisFitClustPosX[i]);
      fpHisFitClustPosY[i] = new TH1F(Form("%sFitClusPosY%d", fPrefix.Data(), i + 1), Form("%s - Clus Y position by fit in sensor %d;[cm]; Counts", fTitleDev.Data(), i + 1), 1000, -5, 5); // ! range
      AddHistogram(fpHisFitClustPosY[i]);

      // fit resolution
      fpHisFitClustPosErrX[i] = new TH1F(Form("%sFitClusPosErrX%d", fPrefix.Data(), i + 1), Form("%s - Clus X position error by fit in sensor %d;[cm]; Counts", fTitleDev.Data(), i + 1), 1000, 0., 0.002);
      AddHistogram(fpHisFitClustPosErrX[i]);
      fpHisFitClustPosErrY[i] = new TH1F(Form("%sFitClusPosErrY%d", fPrefix.Data(), i + 1), Form("%s - Clus Y position error by fit  in sensor %d;[cm]; Counts", fTitleDev.Data(), i + 1), 1000, 0., 0.002); // ! range
      AddHistogram(fpHisFitClustPosErrY[i]);

   }
   
   fpHisMeanPixel = new TH1F(Form("%sMeanClusPix", fPrefix.Data()), Form("%s - mean pixels per tracked clusters", fTitleDev.Data()), 100, -0.5, 99.5);
   AddHistogram(fpHisMeanPixel);
   
   fpHisResTotX = new TH1F(Form("%sResTotX", fPrefix.Data()), Form("%s - Total ResidualX", fTitleDev.Data()), 400, -0.01, 0.01);
   fpHisResTotY = new TH1F(Form("%sResTotY", fPrefix.Data()), Form("%s - Total ResidualY", fTitleDev.Data()), 400, -0.01, 0.01);
   AddHistogram(fpHisResTotX);
   AddHistogram(fpHisResTotY);
   
   fpHisChi2RedX = new TH1F(Form("%sChi2RedX", fPrefix.Data()), Form("%s - Reduced Chi2 X", fTitleDev.Data()), 1000, 0, 50);
   fpHisChi2RedY = new TH1F(Form("%sChi2RedY", fPrefix.Data()), Form("%s - Reduce Chi2 Y", fTitleDev.Data()), 1000, 0, 50);
   fpHisChi2TotX = new TH1F(Form("%sChi2TotX", fPrefix.Data()), Form("%s - Tot Chi2 X", fTitleDev.Data()), 1000, 0, 50);
   fpHisChi2XvsTheta = new TH2F(Form("%sChi2TotXvsTheta", fPrefix.Data()), Form("%s - Tot Chi2 X vs theta of the tack; Chi2; theta (deg)", fTitleDev.Data()), 1000, 0, 50,1000,0,30);
   fpHisChi2YvsTheta = new TH2F(Form("%sChi2TotYvsTheta", fPrefix.Data()), Form("%s - Tot Chi2 Y vs theta of the tack; Chi2; theta (deg)", fTitleDev.Data()), 1000, 0, 50, 1000, 0, 30);
   fpHisChi2TotY = new TH1F(Form("%sChi2TotY", fPrefix.Data()), Form("%s - Tot Chi2 Y", fTitleDev.Data()), 1000, 0, 50);
   fpHisChi2RedTot = new TH1F(Form("%sChi2RedTot", fPrefix.Data()), Form("%s - Reduced Chi2 Total", fTitleDev.Data()), 1000, 0, 50);

   fpHisChi2probX = new TH1F(Form("%sChi2pvalueX", fPrefix.Data()), Form("%s - p value of the fit (chi2 probability) X", fTitleDev.Data()), 1000, 0, 1);
   fpHisChi2probY = new TH1F(Form("%sChi2pvalueY", fPrefix.Data()), Form("%s - p value of the fit (chi2 probability) Y", fTitleDev.Data()), 1000, 0, 1);
   AddHistogram(fpHisChi2RedX);
   AddHistogram(fpHisChi2RedY);
   AddHistogram(fpHisChi2TotX);
   AddHistogram(fpHisChi2XvsTheta);
   AddHistogram(fpHisChi2YvsTheta);
   AddHistogram(fpHisChi2TotY);
   AddHistogram(fpHisChi2RedTot);
   AddHistogram(fpHisChi2probX);
   AddHistogram(fpHisChi2probY);
   
   fpHisTrackEvt = new TH1F(Form("%sTrackEvt", fPrefix.Data()), Form("%s - Number of tracks per event", fTitleDev.Data()), 20, -0.5, 19.5);
   AddHistogram(fpHisTrackEvt);
   
   fpHisTrackClus = new TH1F(Form("%sTrackClus", fPrefix.Data()), Form("%s - Number of clusters per track", fTitleDev.Data()), 9, -0.5, 8.5);
   AddHistogram(fpHisTrackClus);
   
   fpHisClusSensor = new TH1F(Form("%sClusSensor", fPrefix.Data()), Form("%s - Number of tracked clusters per sensor", fTitleDev.Data()), 9, -0.5, 8.5);
   AddHistogram(fpHisClusSensor);
   
   fpHisTheta = new TH1F(Form("%sTrackTheta", fPrefix.Data()), Form("%s - Track polar distribution", fTitleDev.Data()), 500, 0, 90);
   AddHistogram(fpHisTheta);
   
   fpHisPhi = new TH1F(Form("%sTrackPhi", fPrefix.Data()), Form("%s - Track azimutal distribution", fTitleDev.Data()), 500, -180, 180);
   AddHistogram(fpHisPhi);
   
   fpHisBeamProf = new TH2F(Form("%sBeamProf", fPrefix.Data()), Form("%s -  Beam Profile", fTitleDev.Data()),
                            100, -pGeoMap->GetEpiSize()[0]/2., pGeoMap->GetEpiSize()[0]/2.,
                            100, -pGeoMap->GetEpiSize()[1]/2., pGeoMap->GetEpiSize()[1]/2.);
   fpHisBeamProf->SetStats(kFALSE);
   AddHistogram(fpHisBeamProf);

   fpHisPosCompareX = new TH2F(Form("%sPosCompareX", fPrefix.Data()), "Comparison of position X; x_{fit} - x_{c} [cm]; x_{meas} - x_{c} [cm];",
                              100, -0.004, 0.004,
                              100, -0.002, 0.002);
   AddHistogram(fpHisPosCompareX);

   fpHisPosCompareY = new TH2F(Form("%sPosCompareY", fPrefix.Data()), "Comparison of position Y; y_{fit} - y_{c} [cm]; y_{meas} - y_{c} [cm];",
                               100, -0.004, 0.004,
                               100, -0.002, 0.002);
   AddHistogram(fpHisPosCompareY);

   Int_t adc = TAVTbaseDigitizer::GetTotAdcDepth();
   Int_t bin = TMath::Power(2, adc);
   fpHisMeanCharge = new TH1F(Form("%sMeanClusChg", fPrefix.Data()), Form("%s - mean charge per tracked clusters", fTitleDev.Data()), bin/2., 0, bin);
   AddHistogram(fpHisMeanCharge);
   
   SetValidHistogram(kTRUE);
   
   return;
}

//_____________________________________________________________________________
//! Apply cuts for a given track
//!
//! \param[in] track a given track
Bool_t TAVTactBaseTrack::AppyCuts(TAGbaseTrack* track)
{
   Bool_t valid = false;  
   
   TAVTbaseParConf* pConfig = GetParConf();
   if (track->GetClustersN() >= fRequiredClusters)
  	  valid = true;

//cut in n pixels
   for (int i = 0; i < track->GetClustersN(); i++){
     TAGcluster *cluster = (TAGcluster *)track->GetCluster(i);
     if (cluster->GetElementsN() < 10) valid = false;
   }

     return valid;
}

//_____________________________________________________________________________
//! Update track parameters
//!
//! \param[in] track a given track
void TAVTactBaseTrack::UpdateParam(TAGbaseTrack* track)
{
   TVector3  origin;  // origin in the tracker system
   TVector3  slope;   // slope along z-axis in tracker system
   TVector3  originErr;  // origin error in the tracker system
   TVector3  slopeErr;   // slope error along z-axis in tracker system
   TMatrixDSym covMatrixU(1);
   TMatrixDSym covMatrixV(1);


   // init
   origin.SetXYZ(0., 0., 0.);
   slope.SetXYZ(0.,0.,1.);

   originErr.SetXYZ(0.,0.,0.);
   slopeErr.SetXYZ(0.,0.,0.);

   covMatrixU(0,0) =0.;
   covMatrixV(0,0) = 0.;

   // values from covariance matrix
   //  std::map<std::string, vector<Double_t>> variance;
   //  variance["dm"] = {0.,0.};
   //  variance["dq"]= {0., 0.};
   //  variance["dmdq"]={0., 0.};   //covariance between origin and slope

   Int_t nClusters = track->GetClustersN();

   if( nClusters == 2) {
	  TAGcluster* cluster0 = track->GetCluster(0);
	  TAGcluster* cluster1 = track->GetCluster(1);
	  
     if(FootDebugLevel(1))
		 printf("TAVTactNtuTrack::Analyse track with %d cluster taking origin from it and slope 0\n", track->GetClustersN());
      
	   Double_t x0 = cluster0->GetPositionG()[0];
      Double_t y0 = cluster0->GetPositionG()[1];
      Double_t z0 = cluster0->GetPositionG()[2];

  	   Double_t x1 = cluster1->GetPositionG()[0];
      Double_t y1 = cluster1->GetPositionG()[1];
      Double_t z1 = cluster1->GetPositionG()[2];
	  
     if(FootDebugLevel(1))
		 printf( "TAVTactNtuTrack::Analyze cluster[pl %d]=(%.2f, %.2f, %.2f) cluster[pl %d]=(%.2f, %.2f, %.2f)\n",
				cluster0->GetSensorIdx(), x0, y0, z0,
				cluster1->GetSensorIdx(), x1, y1, z1);
	  
	  if( z1-z0 != 0.) {
		 origin[0] = (z1*x0-z0*x1)/(z1-z0);
		 origin[1] = (z1*y0-z0*y1)/(z1-z0);
		 slope[0]  = (x1-x0)/(z1-z0);
		 slope[1]  = (y1-y0)/(z1-z0);
	  }
   } else {
	  Double_t x, dx;
	  Double_t y, dy;
	  Double_t z, dz;
	  
	  fGraphU->Set(nClusters);
	  fGraphV->Set(nClusters);
	  
	  for (Int_t i = 0; i < nClusters; ++i) {
		 TAGcluster* cluster = track->GetCluster(i);
		 x  = cluster->GetPositionG()(0);
		 y  = cluster->GetPositionG()(1);
		 z  = cluster->GetPositionG()(2);
		 dx = cluster->GetPosError()(0);
		 dy = cluster->GetPosError()(1);
		 dz = cluster->GetPosError()(2);
		 fGraphU->SetPoint(i, z, x);
		 fGraphU->SetPointError(i, dz, dx);
		 
		 fGraphV->SetPoint(i, z, y);
		 fGraphV->SetPointError(i, dz, dy);
	  }

     auto fitResultU = fGraphU->Fit("pol1", "SQEX0");
     covMatrixU.ResizeTo(fitResultU->GetCovarianceMatrix());
     covMatrixU = fitResultU->GetCovarianceMatrix();

     TF1 *polyU = fGraphU->GetFunction("pol1");
     origin[0]    = polyU->GetParameter(0);
	  slope[0]     = polyU->GetParameter(1);
     originErr[0] = polyU->GetParError(0);
     slopeErr[0]  = polyU->GetParError(1);
   //   variance["dq"][0] = covMatrixU(0, 0);
   //   variance["dm"][0] = covMatrixU(1, 1);
   //   variance["dmdq"][0] = covMatrixU(0, 1);

     auto fitResultV = fGraphV->Fit("pol1", "SQEX0");
     covMatrixV.ResizeTo(fitResultV->GetCovarianceMatrix());
     covMatrixV = fitResultV->GetCovarianceMatrix();
     TF1* polyV   = fGraphV->GetFunction("pol1");
	  origin[1]    = polyV->GetParameter(0);
	  slope[1]     = polyV->GetParameter(1);
     originErr[1] = polyV->GetParError(0);
     slopeErr[1]  = polyV->GetParError(1);
   //   variance["dq"][1] = covMatrixV(0, 0);
   //   variance["dm"][1] = covMatrixV(1, 1);
   //   variance["dmdq"][1] = covMatrixV(0, 1);
      //cout <<"fit result: "<< fitResultU->Chi2() << " ndf: "<< fitResultU->Ndf() << endl;
     track->SetChi2RedU(fitResultU->Chi2() /  fitResultU->Ndf());
     track->SetChi2RedV(fitResultV->Chi2()/  fitResultV->Ndf());
     track->SetChi2U(fitResultU->Chi2());
     track->SetChi2V(fitResultV->Chi2());
     //cout << fitResultU->Ndf() << endl;
     track->SetChi2((fitResultV->Chi2() + fitResultU->Chi2()) / (fitResultV->Ndf() + fitResultU->Ndf()));
     track->SetPvalueU(fitResultU->Prob());
     track->SetPvalueV(fitResultV->Prob());
   }
   track->SetCovarianceMatrix(covMatrixU, covMatrixV);
   track->SetLineValue(origin, slope);
   track->SetLineErrorValue(originErr, slopeErr);
   
}

//_____________________________________________________________________________
//! Fill histograms per track
//! \param[in] track a given track
void TAVTactBaseTrack::FillHistogramm(TAGbaseTrack* track)
{
   TAVTbaseParGeo* pGeoMap  = GetParGeo();
   
   fpHisTheta->Fill(track->GetTheta());
   fpHisPhi->Fill(track->GetPhi());
   
   if (GetTracksN() == 0)
      fpHisClusSensor->Fill(0);
   
   fpHisTrackClus->Fill(track->GetClustersN());
   for (Int_t i = 0; i < track->GetClustersN(); ++i) {
	  TAGcluster * cluster = track->GetCluster(i);
     cluster->SetFound();
     Int_t idx = cluster->GetSensorIdx();
     Float_t posZ       = cluster->GetPositionG()[2];
	  TVector3 impact    = track->Intersection(posZ);
     TVector3 impactLoc =  pGeoMap->Detector2Sensor(idx, impact);
     TVector3 impactGlb = fpFootGeo->FromVTLocalToGlobal(impact);

     //------- pull X
     Double_t dxOverdm = posZ;
     //Double_t dxOverdq = 1.;
     Double_t fitErrorX2 = dxOverdm * dxOverdm* track->GetCovMatrixU()(1,1) + track->GetCovMatrixU()(0,0) + 2.*dxOverdm*track->GetCovMatrixU()(0,1);
     Double_t fitErrorX = sqrt(fitErrorX2);
     Double_t resX = cluster->GetPositionG()(0) - track->Intersection(posZ).X();
     
       if (fitErrorX2 < (cluster->GetPosError().X() * cluster->GetPosError().X()))
       {
          Double_t pullX = resX / sqrt(cluster->GetPosError().X() * cluster->GetPosError().X() - fitErrorX2);
          if (track->GetClustersN() == 4) {
            fpHisPullX[idx]->Fill(pullX);
            fpHisPullXvsChiX[idx]->Fill(pullX, track->GetChi2redU());
            fpHisPullXvsPvalue[idx]->Fill(pullX, track->GetPvalueU());
            if (track->GetPvalueU()>0.001) fpHisPullXcut[idx]->Fill(pullX);
          }
       }   
       else
          cout << "negative difference between sigma in cluster measurement in x!" << endl;

       //------- pull Y
       Double_t dyOverdm = posZ;
       // Double_t dyOverdq = 1.;
       Double_t fitErrorY2 = dyOverdm * dyOverdm * track->GetCovMatrixV()(1, 1) + track->GetCovMatrixV()(0, 0) + 2. * dyOverdm * track->GetCovMatrixV()(0, 1);
       Double_t fitErrorY = sqrt(fitErrorY2);
       Double_t resY = cluster->GetPositionG()(1) - track->Intersection(posZ).Y();

       if (fitErrorY2 < (cluster->GetPosError().Y() * cluster->GetPosError().Y()))
       {
          Double_t pullY = resY / sqrt(cluster->GetPosError().Y() * cluster->GetPosError().Y() - fitErrorY2);
          if (track->GetClustersN() == 4) {
            fpHisPullY[idx]->Fill(pullY);
            fpHisPullYvsChiY[idx]->Fill(pullY, track->GetChi2redV());
            fpHisPullYvsPvalue[idx]->Fill(pullY, track->GetPvalueV());
            if (track->GetPvalueV()>0.001) fpHisPullYcut[idx]->Fill(pullY);

            if (idx == 3){
            if (track->GetPvalueV() < 0.001 && ( pullY >3 || pullY<-3  ))
            {  
               for (int j =0; j<4; j++){
                  fpHisTrackClusSize[j]->Fill((static_cast<TAVTbaseCluster *>(track->GetCluster(j)))->GetPixelsN());
                  //cout << static_cast<TAVTbaseCluster *>(track->GetCluster(j))->GetPixelsN() << endl;
               }
               // cout << "pvalue x: " << track->GetPvalueU() << " pvalue y: " << track->GetPvalueV() << endl;
            }
            }
            fpHisTrackClusSizeAll[idx]->Fill(static_cast<TAVTbaseCluster *>(cluster)->GetPixelsN());
          
          }
      }
       else
          cout << "negative difference between sigma in cluster measurement in y!" << endl;
     

//------------  fitted position vs measured position

TAVTcluster *cluster_vt = static_cast<TAVTcluster *>(cluster);
int id_sensor = cluster_vt ->GetSensorIdx();

Double_t min_dist_meas_x = 0.00207;
Int_t central_pixel_id_x = -1;
Double_t min_dist_meas_y = 0.00207;
Int_t central_pixel_id_y = -1;

for (int i = 0; i < cluster_vt->GetPixelsN(); i++)
{
       TAVThit *pixel = cluster_vt->GetPixel(i);
       Double_t curr_dist_x = cluster_vt ->GetPositionG()(0) - GetParGeo()->Sensor2Detector(cluster_vt->GetSensorIdx(), pixel->GetPosition())(0);
       Double_t curr_dist_y = cluster_vt->GetPositionG()(1) - GetParGeo()->Sensor2Detector(cluster_vt->GetSensorIdx(), pixel->GetPosition())(1);

       if (abs(curr_dist_x) <= abs(min_dist_meas_x))
       {
          min_dist_meas_x = curr_dist_x;
          central_pixel_id_x = i;
       }

       if (abs(curr_dist_y) <= abs(min_dist_meas_y))
       {
          min_dist_meas_y = curr_dist_y;
          central_pixel_id_y = i;
       }
     }


     Double_t min_dist_fit_x = track->Intersection(posZ).X() - GetParGeo()->Sensor2Detector(cluster_vt->GetSensorIdx(), cluster_vt->GetPixel(central_pixel_id_x)->GetPosition())(0);
     Double_t min_dist_fit_y = track->Intersection(posZ).Y() - GetParGeo()->Sensor2Detector(cluster_vt->GetSensorIdx(), cluster_vt->GetPixel(central_pixel_id_y)->GetPosition())(1);
     if (track->GetClustersN() == 4)
     {
       fpHisPosCompareX->Fill(min_dist_fit_x, min_dist_meas_x);
       fpHisPosCompareY->Fill(min_dist_fit_y, min_dist_meas_y);

       //---------position of clusters in plane

       fpHisTrackClustPosX[idx]->Fill(cluster->GetPositionG()[0]);
       fpHisTrackClustPosY[idx]->Fill(cluster->GetPositionG()[1]);
       fpHisTrackClustPosErrX[idx]->Fill(cluster->GetPosError().X());
       fpHisTrackClustPosErrXvsPixel[idx]->Fill(cluster_vt->GetPixelsN(),cluster->GetPosError().X() );
       fpHisTrackClustPosErrY[idx]->Fill(cluster->GetPosError().Y());
       fpHisTrackClustPosErrYvsPixel[idx]->Fill(cluster_vt->GetPixelsN(),cluster->GetPosError().Y() );
       fpHisFitClustPosX[idx]->Fill(track->Intersection(posZ).X());
       fpHisFitClustPosY[idx]->Fill(track->Intersection(posZ).Y());
       fpHisFitClustPosErrX[idx]->Fill(fitErrorX);
       fpHisFitClustPosErrY[idx]->Fill(fitErrorY);
       //fpHisTrackMap[idx]->Fill(impactLoc[0], impactLoc[1]);
       fpHisTrackMap[idx]->Fill(impactGlb[0], impactGlb[1]);
       fpHisResTotX->Fill(impact[0] - cluster->GetPositionG()[0]);
       fpHisResTotY->Fill(impact[1] - cluster->GetPositionG()[1]);
       fpHisResX[idx]->Fill(resX);
       fpHisResY[idx]->Fill(resY);
       fpHisClusSensor->Fill(idx + 1);
     }
   }
   
   if (track->GetClustersN() ==4){
   fpHisChi2RedX->Fill(track->GetChi2redU());
   fpHisChi2RedY->Fill(track->GetChi2redV());
   fpHisChi2TotX->Fill(track->GetChi2U());
   fpHisChi2XvsTheta->Fill(track->GetChi2U(), track->GetTheta() );
   fpHisChi2YvsTheta->Fill(track->GetChi2V(), track->GetTheta() );
   fpHisChi2TotY->Fill(track->GetChi2V());
   fpHisChi2RedTot->Fill(track->GetChi2());
   fpHisChi2probX->Fill(track->GetPvalueU());
   fpHisChi2probY->Fill(track->GetPvalueV());

   // if (track->GetPvalueV() < 0.001 ) {
   //    for (int i =0; i< 4; i++){
   //        //cout << "clus " << i << "pos X: " << track->GetCluster(i)->GetPositionG()[0] << " pos y: " << track->GetCluster(i)->GetPositionG()[1] << " n pixel: " << (static_cast<TAVTbaseCluster*>(track->GetCluster(i)))->GetPixelsN() << endl;
   //        fpHisTrackClusSize[i]->Fill((static_cast<TAVTbaseCluster *>(track->GetCluster(i)))->GetPixelsN());
   //    }
      
   //    //cout << "pvalue x: " << track->GetPvalueU() << " pvalue y: " << track->GetPvalueV() << endl;
   // }

   }
   TVector3 origin = track->GetOrigin();
   fpHisBeamProf->Fill(origin.X(), origin.Y());
   
   fpHisMeanPixel->Fill(track->GetMeanEltsN());
   fpHisMeanCharge->Fill(track->GetMeanCharge());
   
}

//_____________________________________________________________________________
//  
//TAVTbaseCluster* TAVTactBaseTrack::NearestCluster(TAGbaseTrack *aTrack, Int_t iSensor)
//{   
//   // For a given track, return the index of the nearest hit of a given plane
//   TAVTntuCluster* pNtuClus = (TAVTntuCluster*) fpNtuClus->Object();
//
//   Double_t minDist = 1.e9;
//   Double_t distance;
//   TAVTbaseCluster *tCluster;
//   
//   TAVTbaseCluster *tNearest = 0x0;
//   for (Int_t ht = 0; ht < pNtuClus->GetClustersN(iSensor); ++ht) { // loop on hits
//	  tCluster = pNtuClus->GetCluster(iSensor, ht);
//	  distance = tCluster->Distance( aTrack) ;
//	  if( distance < minDist) {
//		 minDist = distance;
//		 tNearest = tCluster;
//	  }
//   } //end loop on hits
//   
//   return tNearest;
//}

//_____________________________________________________________________________
//  
//TAGbaseTrack* TAVTactBaseTrack::NearestTrack(TAVTbaseCluster *aCluster) {
//   
//   // For a given track, return the index of the nearest hit of a given plane
//   Double_t minDist = 1.e9;
//   Double_t distance;
//   TAVTtrack *tTrack;
//   
//   TAGbaseTrack *tNearest = 0x0;
//   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
//
//   for( Int_t it = 0; it < pNtuTrack->GetTracksN(); ++it) { // loop on tracks
//	  tTrack = pNtuTrack->GetTrack(it);
//	  distance = aCluster->Distance( tTrack);
//	  if( distance < minDist) {
//		 minDist = distance;
//		 tNearest = tTrack;
//	  }
//   } // end loop on tracks
//   
//   return tNearest;
//}

//______________________________________________________________________________
//! Set geo trafo with name
//!
//! \param[in] name geo trafo action name
void TAVTactBaseTrack::SetGeoTrafo(TString name)
{
   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(name.Data());
   if (!fpFootGeo)
	  Error("SetGeoTrafo()", "No GeoTrafo action %s available yet\n", name.Data());
}

//_____________________________________________________________________________
//! Get number of clusters for a given sensor
//!
//! \param[in] iSensor sensor index
Int_t TAVTactBaseTrack::GetClustersN(Int_t iSensor)
{
   TAVTntuCluster*  pNtuClus  = (TAVTntuCluster*) fpNtuClus->Object();
   return pNtuClus->GetClustersN(iSensor);
}

//_____________________________________________________________________________
//! Get cluster for a given sensor and a given index
//!
//! \param[in] iSensor sensor index
//! \param[in] iClus cluster index
TAGcluster* TAVTactBaseTrack::GetCluster(Int_t iSensor, Int_t iClus)
{
   TAVTntuCluster*  pNtuClus  = (TAVTntuCluster*)  fpNtuClus->Object();
   TAVTcluster* cluster = pNtuClus->GetCluster(iSensor, iClus);
   
   return cluster;
}

//_____________________________________________________________________________
//! Get number of tracks
Int_t TAVTactBaseTrack::GetTracksN()
{
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   return pNtuTrack->GetTracksN();
}

//_____________________________________________________________________________
//! Get number tracks const
Int_t TAVTactBaseTrack::GetTracksN() const
{
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   
   return pNtuTrack->GetTracksN();
}

//_____________________________________________________________________________
//! Add new track to container using copt cstr
//!
//! \param[in] trk a given track
void TAVTactBaseTrack::AddNewTrack(TAGbaseTrack* trk)
{
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   TAVTtrack* track = static_cast<TAVTtrack*>(trk);
   pNtuTrack->NewTrack(*track);
}

//_____________________________________________________________________________
//! Get new track
TAGbaseTrack* TAVTactBaseTrack::NewTrack()
{
   return new TAVTtrack();
}

//_____________________________________________________________________________
//! Get track from container
//!
//! \param[in] idx track index
TAGbaseTrack* TAVTactBaseTrack::GetTrack(Int_t idx)
{
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   TAGbaseTrack* track  = pNtuTrack->GetTrack(idx);
   
   return track;
}

//_____________________________________________________________________________
//! Set beam position
//!
//! \param[in] pos position of beam
void TAVTactBaseTrack::SetBeamPosition(TVector3 pos)
{
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   pNtuTrack->SetBeamPosition(pos);
}

//_____________________________________________________________________________
//! Get geometry parameters
TAVTbaseParGeo* TAVTactBaseTrack::GetParGeo()
{
   TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
   
   return pGeoMap;
}

//_____________________________________________________________________________
//! Get configuration parameters
TAVTbaseParConf* TAVTactBaseTrack::GetParConf()
{
   TAVTparConf* pConfig = (TAVTparConf*) fpConfig->Object();
   
   return pConfig;
}
