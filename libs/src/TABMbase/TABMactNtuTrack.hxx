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
#include "TABMLegendreFitter.hxx"
#include "TABMMinuitFitter.hxx"
#include "../TAGfoot/ClockCounter.hxx"

#include <TDecompChol.h>

#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TGeoManager.h"

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

  void InvertTracks(vector<TABMtrack> &tracktrvec, Int_t InvertView);  //Invert the BM tracks Y axis parameters (to be consistent with the VTX in GSI2021 campaign)
  void CombineTrack(vector<TABMtrack> &ytracktr, vector<TABMtrack> &xtracktr); //combine the track of both views
  void EvaluateDistRes();                           //Evaluate the resolution distribution

  //for calibration only
  void FitWriteCalib(TF1 *newstrel, TF1 *resofunc, Double_t &meanTimeReso, Double_t &meanDistReso);          //Fit the calibration plots and write the output

  void EarlyReturn( Int_t code);
  void FillTrackHistograms(vector<TABMtrack> &ytracktr, vector<TABMtrack> &xtracktr);
  void FillFitHistograms();

  Int_t CountRemainingHits( Int_t view );

  private:

  TAGdataDsc*       fpNtuTrk;		    // output data dsc
  TAGdataDsc*       fpNtuHit;		    // input data dsc
  TAGparaDsc*       fpParGeo;		    // input data dsc
  TAGparaDsc*       fpParCon;		    // input data dsc
  TAGparaDsc*       fpParCal;		    // input data dsc

  TABMntuTrack* p_ntutrk; 
  TABMntuHit*   p_nturaw;
  Int_t         i_nhit;




  TABMtrack *       fpTmpTrack;     // adopted for reconstruction
  int fTrackNum;
  int fNowView;
  
  TABMLegendreFitter  *legendreFit;
  TABMMinuitFitter    *minuitFit;
  
  //histos
  TH2F*            fpResTot;
  TH2F*            fpResTot1;
  TH2F*            fpHisMap;
  TH2F*            fpHisMapTW;
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

  // chisq minimization
  TH1I*            fpFitIters;
  TH1F*            fpFitQvalue;
  TH1F*            fpFitMvalue;
  TH1F*            fpFitQ;
  TH1F*            fpFitM;
  TH1F*            fpFitQdiff;
  TH1F*            fpFitMdiff;
  TH2F*            fpFitPulls1;
  TH2F*            fpFitPulls2;
  TH2F*            fpFitQFvsQ;
  TH2F*            fpFitMFvsM;
  TH2F*            fpFitMFvsQF;
  TH1I*            fpBadHits;
  TH1I*            fpUnassignedHits;
  TH1I*            fpRebinM;
  TH1I*            fpRebinQ;
  ClockCounter  clk;
  ClassDef(TABMactNtuTrack,0)
};


#endif
