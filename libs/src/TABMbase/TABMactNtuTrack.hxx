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
#include "TABMntuTrackTr.hxx"
#include "TABMparCon.hxx"
#include "TABMparGeo.hxx"
#include "TAGparGeo.hxx"
#include "TABMntuRaw.hxx"
#include "TABMntuHit.hxx"
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


/* lines defined in the plane by  y=mx+q*/  //used for least chi2 tracking
typedef struct{
  Double_t mm[4];
  Double_t qq[4];
} TG_STRUCT;

class TABMactNtuTrack : public TAGaction {
public:
  explicit        TABMactNtuTrack(const char* name=0,
                                  TAGdataDsc* dscntutrk=0,
                                  TAGdataDsc* dscnturaw=0,
                                  TAGparaDsc* dscbmgeo=0,
                                  TAGparaDsc* dscbmcon=0,
                                  TAGparaDsc* dsctggeo=0);
  virtual         ~TABMactNtuTrack();
  virtual Bool_t  Action();

  virtual  void   CreateHistogram();

  //tracking
  void ChargeLegendrePoly();
  Int_t FindLegendreBestValues();
  void CheckAssHits(const Int_t asshitx, const Int_t asshity);
  void CheckPossibleHits(const Int_t cellplane, Int_t wireplane[], TH2D* legpolsum, Double_t yvalue, const Int_t best_rbin, Double_t diff, Double_t res, Int_t &selview, const Int_t hitnum);
  Double_t EvaluateChi2(const double *params);
  Double_t EvaluateChi2Xview(const double *params);
  Double_t EvaluateChi2Yview(const double *params);
  Double_t EvaluateChi2GSIx(const double *params);
  Double_t EvaluateChi2GSIy(const double *params);
  Int_t NumericalMinimizationDouble();
  Int_t NumericalMinimizationGSI();//same as NumericalMinimizationDouble, but specific for the first FOOT GSI run 2019, with t0 passed as free par
  Bool_t ComputeDataAll();

  ClassDef(TABMactNtuTrack,0)

  private:

  TGeoManager*      f_BMgeoMan;
  TAGdataDsc*       fpNtuTrk;		    // output data dsc
  TAGdataDsc*       fpNtuHit;		    // input data dsc
  TAGparaDsc*       fpBMGeo;		    // input data dsc
  TAGparaDsc*       fpBMCon;		    // input data dsc
  TAGparaDsc*       fpTGgeo;		    // input data dsc

  TABMntuTrackTr *tmp_trackTr;
  //ntu objects
  TABMntuRaw*   p_nturaw;
  TABMntuHit*   p_hit;
  //par objects
  TABMparCon* p_bmcon;
  TABMparGeo* p_bmgeo;
  TAGparGeo*  p_tggeo;

  //Legendre:
  TH2D*    legpolxsum;
  TH2D*    legpolysum;
  Int_t    best_mxbin;
  Int_t    best_mybin;
  Int_t    best_rxbin;
  Int_t    best_rybin;

  //new chi2 ROOT based
  ROOT::Math::Minimizer* minx;
  ROOT::Math::Minimizer* miny;
  ROOT::Math::Functor* functorx;
  ROOT::Math::Functor* functory;
  ROOT::Math::Functor* functorGSIx;
  ROOT::Math::Functor* functorGSIy;

  //histos
  //~ TH2F*            fpHisR02d;
  TH2F*            fpResTot;//
  TH2F*            fpHisMap;
  TH2F*            fpHisMylar12d;
  TH2F*            fpHisMylar22d;
  TH1F*            fpHisChi2Red;
  TH1I*            fpHisNhitTrack;
  TH1I*            fpHisNrejhitTrack;
  TH1I*            fpHisTrackStatus;
  TH1I*            fpHisTrackFakeHit;
  TH1F*            fpHisTrackEff;
  TH1I*            fpHisTrigTime;

};


#endif
