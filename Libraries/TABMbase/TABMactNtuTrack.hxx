#ifndef _TABMactNtuTrack_HXX
#define _TABMactNtuTrack_HXX
/*!
  \file
  \version $Id: TABMactNtuTrack.hxx, $
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
#include "TABMactBaseNtuTrack.hxx"

#include <TDecompChol.h>

#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TGeoManager.h"

#include "Math/Minimizer.h"
#include "TFitter.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TRandom2.h"
#include "TError.h"

#include <math.h>
#include <iostream>


class TABMactNtuTrack : public TABMactBaseNtuTrack {
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

  //reco methods
  Int_t NumericalMinimizationDouble();              //use minuit2 to refine the track parameters
  Double_t EvaluateChi2(const double *params);      //adopted in minuit2 to calculate the track chi2 with the selected hits
  void EvaluateDistRes();                           //Evaluate the resolution distribution
  void EvaluatePulls();
  void EvaluateMauroPulls();

  void ToStream(ostream& os=cout, Option_t* option="") const;
  
  //for calibration only
  void FitWriteCalib(TF1 *newstrel, TF1 *resofunc, Double_t &meanTimeReso, Double_t &meanDistReso);          //Fit the calibration plots and writhe the output

  private:

  //chi2 minimization ROOT minuit2 based
  ROOT::Math::Minimizer* fpMinimizer;
  ROOT::Math::Functor* fpFunctor;

  //histos
  TH2F*            fpResTot;
  TH2F*            fpHisMap;
  TH2F*            fpHisMapTW;
  TH2F*            fpHisMylar12d;

  TH1F*            fpHisAngleX;
  TH1F*            fpHisAngleY;
  TH1F*            fpHisChi2Red;
  TH1F*            fpHisChi2XZ;
  TH1F*            fpHisChi2YZ;
  TH1F*            fpTrackAngles;
  TH1F*            fpTrackSep;
  //for bm calibration
  TH2F*            fpResTimeTot;
  TH1F*            fpParNewSTrel;
  TH1F*            fpParNewTimeRes;
  TH1F*            fpNewStrelDiff;
  TH1F*            fpNewResoDistDiff;
  TH1D*            fpParNewDistRes;
  std::vector<TH1F*> fpResTimeBin; //for the STREL calibration
  std::vector<TH1F*> fpResDistBin; //for the STREL calibration

  //fitting parameters
  TH1I*            fpFitIters;
  TH1F*            fpFitQinitial;
  TH1F*            fpFitMinitial;
  TH1F*            fpFitQ;
  TH1F*            fpFitM;
  TH1F*            fpFitQdiff;
  TH1F*            fpFitMdiff;
  TH2F*            fpFitPulls;
  TH2F*            fpFitQFvsQ;
  TH2F*            fpFitMFvsM;
  TH2F*            fpFitMFvsQF;

  ClassDef(TABMactNtuTrack,0)
};


#endif
