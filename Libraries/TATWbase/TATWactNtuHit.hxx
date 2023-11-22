#ifndef _TATWactNtuHit_HXX
#define _TATWactNtuHit_HXX
/*!
  \file TATWactNtuHit.hxx
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

#include "TATWparameters.hxx"

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
  
  TH1F*           fpHisPos[nLayers];      // Position along the bar for all bars
  TH1F*           fpHisRawTof[nLayers];   // Raw time of flight for all bars
  TH1F*           fpHisCharge[nLayers];
  
  TH1F*           fpHisChargeCentralBar[nLayers];
  TH1F*           fpHisRawTofCentralBar[nLayers];
  TH1F*           fpHisAmpA[nLayers];    
  TH1F*           fpHisAmpB[nLayers];    
  TH2F*           fpHisAmpA_vs_Eloss[nLayers];
  TH2F*           fpHisAmpB_vs_Eloss[nLayers];

  Bool_t          f_debug;

  
  TH1I*           fpHisBarsID[nLayers];
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

  Double_t GetEnergy(Double_t RawEnergy,Int_t layer,Int_t posId, Int_t barId);
  Double_t GetToF(Double_t Time,Int_t layer, Int_t posId, Int_t barId);

  Double_t GetChargeCenterofMass(TATWrawHit*a,TATWrawHit*b);

  Double_t GetPosition(TATWrawHit*a,TATWrawHit*b,Int_t layer,Int_t bar);

  Int_t    GetBarCrossId(Int_t layer, Int_t barId, Double_t rawPos);
  Int_t    GetPerpBarId(Int_t layer, Int_t barId, Double_t rawPos);


};

#endif
