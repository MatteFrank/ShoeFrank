#ifndef _TATWactNtuHit_HXX
#define _TATWactNtuHit_HXX
/*!
  \file
  \version $Id: TATWactNtuHit.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TATWactNtuHit.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>

#include "TH1.h"
#include "TH2.h"

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGparGeo.hxx"

#include "TATWparMap.hxx"
#include "TATWparGeo.hxx"
#include "TATWparCal.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuHit.hxx"

#include "TASTntuHit.hxx"

#include "Parameters.h"

class TATWactNtuHit : public TAGaction {

public:

  explicit        TATWactNtuHit(const char* name=0,
                                TAGdataDsc* p_datraw=0,
                                TAGdataDsc* p_nturaw=0,
                                TAGdataDsc* p_stnturaw=0,
                                TAGparaDsc* p_pargeo=0,
                                TAGparaDsc* p_parmap=0,
                                TAGparaDsc* p_calmap=0,
                                TAGparaDsc* p_pargeo_gl=0);
  virtual         ~TATWactNtuHit();

  virtual Bool_t  Action();

  virtual void    CreateHistogram();
   
  ClassDef(TATWactNtuHit,0);


private:
  
  TAGdataDsc*     fpDatRaw;		    // input data dsc TW
  TAGdataDsc*     fpNtuRaw;		    // output data dsc TW
  TAGdataDsc*     fpSTNtuRaw;		    // output data dsc ST
  TAGparaDsc*     fpParGeo;		    // parameter dsc
  TAGparaDsc*     fpParMap;
  TAGparaDsc*     fpCalPar;
  TAGparaDsc*     fpParGeo_Gl;            // beam parameter dsc

  TAGparGeo*      f_pargeo_gl;
  TAGgeoTrafo*    f_geoTrafo;

  TATWparGeo*     f_pargeo;
  TATWparMap*     f_parmap;
  TATWparCal*     f_parcal;


  Int_t           fZbeam;
  Int_t           fEvtCnt;
  
  Float_t         fTofPropAlpha;    // inverse of light propagation velocity
  Float_t         fTofErrPropAlpha;

  TH1F*           fpHisDeTot;       // Total energy loss
  TH1F*           fpHisTimeTot;     // Total time of flight

  TH1F*           fpHisAmpA[nLayers];     // Total time of flight
  TH1F*           fpHisAmpB[nLayers];     // Total time of flight
  TH2F*           fpHisAmpA_vs_Eloss[nLayers];     // Total time of flight
  TH2F*           fpHisAmpB_vs_Eloss[nLayers];     // Total time of flight

  Bool_t          f_debug;

  
  TH2D*           fpHisElossTof_layer[nLayers];
  vector<TH2D*>   fpHisElossTof_Z;
  vector<TH1D*>   fpHisEloss_Z[nLayers];
  vector<TH1D*>   fpHisTof_Z;

private:
  //
  TATWhit* fCurrentHit;
  //
  Double_t GetRawEnergy(TATWrawHit*a,TATWrawHit*b);
  Double_t GetRawTime(TATWrawHit*a,TATWrawHit*b);
  Double_t GetRawTimeOth(TATWrawHit*a,TATWrawHit*b);

  Double_t GetEnergy(Double_t RawEnergy,Int_t layer,Int_t posId, Int_t barId);
  Double_t GetTime(Double_t Time,Int_t layer, Int_t posId, Int_t barId);
  Double_t GetTimeOth(Double_t Time,Int_t layer, Int_t posId, Int_t barId);

  Double_t GetChargeCenterofMass(TATWrawHit*a,TATWrawHit*b);

  Double_t GetPosition(TATWrawHit*a,TATWrawHit*b);

  Int_t    GetBarCrossId(Int_t layer, Int_t barId, Double_t rawPos);
  Int_t    GetPerpBarId(Int_t layer, Int_t barId, Double_t rawPos);


};

#endif
