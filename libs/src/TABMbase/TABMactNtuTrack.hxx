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
   
  //new tracking
  void Print_matrix(vector<vector<int>>& vec);
  void ChargePrunedTrack(const Int_t prunedhit, Int_t &firedUview, Int_t &firedVview, vector< vector<Int_t> > &hitxtrack, const Int_t index);  
  Int_t EstimateFIRSTTrackPar(vector<Int_t> &singlehittrack, Int_t &firedSingleUview, Int_t &firedSingleVview);
  TG_STRUCT Circles2Tangents(Double_t xc1, Double_t yc1, Double_t r1, Double_t xc2, Double_t yc2, Double_t r2);
  void Chi2Fit(vector<Int_t> &singlehittrack, Int_t &prunedhit,Bool_t converged);
  void ComputeDy(vector<Int_t> &singlehittrack, TVectorD &Dy);
  void ComputeVV(vector<Int_t> &singlehittrack, TMatrixD &VV);
  Double_t ComputeChiQua(TVectorD &Dy, TMatrixD &VV);
  void ComputeAA(vector<Int_t> &singlehittrack, TVectorD &alpha, TMatrixD &AA);
  void ComputeDataAll(vector<Int_t> &singlehittrack);
  Int_t Mini(Int_t nmeas,TMatrixD &AA, TMatrixD &VV, TVectorD &Dy, TVectorD &Eta);
  void Update(vector<Int_t> &singlehittrack, TVectorD Dy, TVectorD& alpha, TVectorD &Eta, TMatrixD &VV, Int_t &worst_hit);  
  Int_t SortFirstDoubleHits(vector< vector<Int_t> > &hitxplane, vector< vector<Int_t> > &hitxtrack);
  void ChargeAllTracks(vector< vector<Int_t> > &hitxtrack,vector< vector<Int_t> > &hitxplane, Int_t tracknum, Int_t firedPlane);
  
  
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
   
  //histos 
  //~ TH2F*            fpHisR02d;
  TH2F*            fpHisMap;
  TH2F*            fpResTot;
  TH2F*            fpHisMylar12d;
  TH2F*            fpHisMylar22d;
  TH1I*            fpHisTrackStatus;
  TH1I*            fpHisPrefitStatus;
  TH1I*            fpHisNhitTrack;
  TH1I*            fpHisNite;
  TH1F*            fpHisChi2Red;
  TH1F*            fpHisTrackEff;

};


#endif
