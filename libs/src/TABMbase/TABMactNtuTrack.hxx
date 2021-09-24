#ifndef _TABMactNtuTrack_HXX
#define _TABMactNtuTrack_HXX
/*!
  \file
  \version $Id: TABMactNtuTrack.hxx,v 1.3 2003/06/29 21:52:35 mueller Exp $
  \brief   Declaration of TABMactNtuTrack.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TAGroot.hxx"
#include "TABMntuTrack.hxx"
#include "TABMtrack.hxx"
#include "TABMparConf.hxx"
#include "TABMparGeo.hxx"
#include "TABMparCal.hxx"
#include "TAGparGeo.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"
#include "TABMntuTrack.hxx"

#include "FieldManager.h"
#include "MaterialEffects.h"
#include "ConstField.h"
#include "Track.h"
#include "WireMeasurement.h"
#include "EventDisplay.h"
#include "TDatabasePDG.h"
#include "DAF.h"
#include <TDecompChol.h>

#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TGeoManager.h"
#include "TGeoMaterialInterface.h"

#include "Math/Minimizer.h"
#include "TFitter.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TRandom2.h"
#include "TError.h"

#include <math.h>
#include <iostream>


class TABMactNtuTrack : public TAGaction {
public:

 //default
  explicit        TABMactNtuTrack(const char* name=0,
                                  TAGdataDsc* dscntutrk=0,
                                  TAGdataDsc* dscnturaw=0,
                                  TAGparaDsc* dscbmgeo=0,
                                  TAGparaDsc* dscbmcon=0,
                                  TAGparaDsc* dscbmcal=0);
  virtual         ~TABMactNtuTrack();
  virtual Bool_t  Action();
  virtual  void   CreateHistogram();

  //reco methods (ordered)
  void ChargeLegendrePoly();                        //fill the fLegPolSum for the legendre polynomy
  Int_t FindLegendreBestValues();                   //find in fLegPolSum the hghest bin with the first estimate of the track parameters
  Int_t CheckAssHits(const Float_t asshiterror, const Float_t minMerr);    //fill the fSelMap with the associated hits
  void CheckPossibleHits(Int_t wireplane[], Float_t yvalue, Float_t diff, Float_t res, Int_t &selview, const Int_t hitnum, TABMhit* p_hit);//adopted inside checkasshits to check if a hit is associated to the track or not
  Int_t NumericalMinimizationDouble();              //use minuit2 to refine the track parameters
  Double_t EvaluateChi2(const double *params);      //adopted in minuit2 to calculate the track chi2 with the selected hits
  Bool_t ComputeDataAll();                          //after the reconstruction, calculate the residuals, chi2 for all the hits
  void InvertTracks(vector<TABMtrack> &tracktrvec, Int_t InvertView);  //Invert the BM tracks Y axis parameters (to be consistent with the VTX in GSI2021 campaign)
  void CombineTrack(vector<TABMtrack> &ytracktr, vector<TABMtrack> &xtracktr, TABMntuTrack* p_ntutrk); //combine the track of both views
  void EvaluateDistRes();                           //Evaluate the resolution distribution

  //for calibration only
  void FitWriteCalib(TF1 *newstrel, TF1 *resofunc, Double_t &meanTimeReso, Double_t &meanDistReso);          //Fit the calibration plots and writhe the output

  //not used methods
  void SaveLegpol();                                // extra method adopted to save fLegPolSum in a different file,

  ClassDef(TABMactNtuTrack,0)

  private:

  TAGdataDsc*       fpNtuTrk;		    // output data dsc
  TAGdataDsc*       fpNtuHit;		    // input data dsc
  TAGparaDsc*       fpParGeo;		    // input data dsc
  TAGparaDsc*       fpParCon;		    // input data dsc
  TAGparaDsc*       fpParCal;		    // input data dsc

  TABMtrack *       fpTmpTrack;     // adopted for reconstruction

  //Legendre:
  TH2F*    fLegPolSum;
  Int_t    fBestMBin;
  Int_t    fBestRBin;
  Int_t    fTrackNum;
  Int_t    fNowView;

  //chi2 minimization ROOT minuit2 based
  ROOT::Math::Minimizer* fpMinimizer;
  ROOT::Math::Functor* fpFunctor;

  //histos
  TH2F*            fpResTot;
  TH2F*            fpHisMap;
  TH2F*            fpHisMylar12d;

  // TH2F*            fpHis0MSD;
  // TH2F*            fpHis1MSD;
  // TH2F*            fpHis2MSD;

  TH1F*            fpHisAngleX;
  TH1F*            fpHisAngleY;
  TH1F*            fpHisChi2Red;
  TH1F*            fpHisChi2XZ;
  TH1F*            fpHisChi2YZ;
  TH1I*            fpHisNhitTotTrack;
  TH1I*            fpHisNhitXTrack;
  TH1I*            fpHisNhitYTrack;
  TH1I*            fpHisNrejhitTrack;
  TH1I*            fpHisTrackStatus;
  TH1I*            fpHisTrackFakeHit;
  TH1I*            fpNtotTrack;
  TH1I*            fpNXtrack;
  TH1I*            fpNYtrack;
  TH1F*            fpTrackAngles;
  TH1F*            fpTrackSep;
  TH1F*            fpParRes;
  TH1F*            fpParSTrel;

  //for bm calibration
  TH2F*            fpResTimeTot;
  TH1F*            fpParNewSTrel;
  TH1F*            fpParNewTimeRes;
  TH1F*            fpNewStrelDiff;
  TH1F*            fpNewResoDistDiff;
  TH1D*            fpParNewDistRes;
  std::vector<TH1F*> fpResTimeBin; //for the STREL calibration
  std::vector<TH1F*> fpResDistBin; //for the STREL calibration

};


#endif
