#ifndef _TABMactBaseNtuTrack_HXX
#define _TABMactBaseNtuTrack_HXX
/*!
  \file
  \version $Id: TABMactBaseNtuTrack.hxx,v 1.3 2003/06/29 21:52:35 mueller Exp $
  \brief   Declaration of TABMactBaseNtuTrack.
*/
/*------------------------------------------+---------------------------------*/

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

#include <vector>
#include <TDecompChol.h>

#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TGeoManager.h"

#include "TFitter.h"
#include "TRandom2.h"
#include "TError.h"

#include <math.h>
#include <iostream>

using namespace std;

class TABMactBaseNtuTrack : public TAGaction {
public:

 //default
  explicit        TABMactBaseNtuTrack(const char* name=0,
                                  TAGdataDsc* dscntutrk=0,
                                  TAGdataDsc* dscnturaw=0,
                                  TAGparaDsc* dscbmgeo=0,
                                  TAGparaDsc* dscbmcon=0,
                                  TAGparaDsc* dscbmcal=0);
  virtual         ~TABMactBaseNtuTrack();
  virtual Bool_t  LegendreFit();
  virtual  void   CreateHistogram();

  //reco methods (ordered)
  Bool_t ApplyBMCuts();                             //Apply BM cuts before the reconstruction
  void ChargeLegendrePoly();                        //fill the fLegPolSum for the legendre polynomy
  Int_t FindLegendreBestValues();                   //find in fLegPolSum the hghest bin with the first estimate of the track parameters
  Int_t CheckAssHits(const Float_t asshiterror, const Float_t minMerr);    //fill the fSelMap with the associated hits
  void CheckPossibleHits(Int_t wireplane[], Float_t yvalue, Float_t diff, Float_t res, Int_t &selview, const Int_t hitnum, TABMhit* p_hit);//adopted inside checkasshits to check if a hit is associated to the track or not

  void CombineTrack(vector<TABMtrack> &ytracktr, vector<TABMtrack> &xtracktr); //combine the track of both views

  //not used methods
  void SaveLegpol(TString addtxt);                                // extra method adopted to save fLegPolSum in a different file,

  protected:

  TAGdataDsc*       fpNtuTrk;		    // output data dsc
  TAGdataDsc*       fpNtuHit;		    // input data dsc
  TAGparaDsc*       fpParGeo;		    // input data dsc
  TAGparaDsc*       fpParCon;		    // input data dsc
  TAGparaDsc*       fpParCal;		    // input data dsc

  TABMtrack *       fpTmpTrack;     // adopted for reconstruction

  //Legendre:
  TH2I*    fLegPolSum;
  Double_t fBestM;
  Double_t fBestR;
  Int_t    fTrackNum;
  Int_t    fNowView;
  vector<Float_t> fbincenterX;
  vector<Float_t> fbincenterY;
  vector<Float_t> flegfixvalue;


  //histos
  TH1I*            fpHisSelhHitDrift;
  TH1I*            fpHisSelhHitTime;
  TH1I*            fpHisDischHitDrift;
  TH1I*            fpHisDischHitTime;
  TH1I*            fpHisNhitTotTrack;
  TH1I*            fpHisNhitXTrack;
  TH1I*            fpHisNhitYTrack;
  TH1I*            fpHisNrejhitTrack;
  TH1I*            fpHisTrackStatus;
  TH1I*            fpHisTrackFakeHit;
  TH1I*            fpNtotTrack;
  TH1I*            fpNXtrack;
  TH1I*            fpNYtrack;
  TH1D*            fpParRes;
  TH1F*            fpParSTrel;

   ClassDef(TABMactBaseNtuTrack,0)
};


#endif
