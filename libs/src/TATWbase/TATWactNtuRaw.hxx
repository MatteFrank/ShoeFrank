#ifndef _TATWactNtuRaw_HXX
#define _TATWactNtuRaw_HXX
/*!
  \file
  \version $Id: TATWactNtuRaw.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TATWactNtuRaw.
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

#include "TATWparGeo.hxx"
#include "TATWparCal.hxx"
#include "TATWdatRaw.hxx"
#include "TATWntuRaw.hxx"

class TATWactNtuRaw : public TAGaction {

public:

  explicit        TATWactNtuRaw(const char* name=0,
				TAGdataDsc* p_datraw=0,
				TAGdataDsc* p_datdaq=0,
				TAGparaDsc* p_pargeo=0,
				TAGparaDsc* p_parmap=0,
				TAGparaDsc* p_calmap=0,
				TAGparaDsc* p_pargeo_gl=0);
  virtual         ~TATWactNtuRaw();

  virtual Bool_t  Action();

  virtual void    CreateHistogram();
   
  ClassDef(TATWactNtuRaw,0);


private:
  
  TAGdataDsc*     fpDatRaw;		    // input data dsc
  TAGdataDsc*     fpNtuRaw;		    // output data dsc
  TAGparaDsc*     fpParGeo;		    // parameter dsc
  TAGparaDsc*     fpParMap;
  TAGparaDsc*     fpCalPar;
  TAGparaDsc*     fpParGeo_Gl;            // beam parameter dsc

  TAGparGeo*      f_pargeo;

  Int_t          Z_beam;
  
  Float_t        fTofPropAlpha;    // inverse of light propagation velocity
  Float_t        fTofErrPropAlpha;

  TH1F*          fpHisDeTot;       // Total energy loss
  TH1F*          fpHisTimeTot;     // Total time of flight
   
  TH2D*          fpHisElossTof_layer[TATWparCal::kLayers];
  vector<TH2D*>  fpHisElossTof_Z;

private:
  //
  TATWntuHit* fCurrentHit;
  //
  Double_t GetRawEnergy(TATWrawHit*a,TATWrawHit*b);
  Double_t GetRawTime(TATWrawHit*a,TATWrawHit*b);
  Double_t GetRawTimeOth(TATWrawHit*a,TATWrawHit*b);
  Double_t GetTime(Double_t Time,Int_t BarId);
  Double_t GetTimeOth(Double_t Time,Int_t BarId);
  Double_t GetEnergy(Double_t RawEnergy,Int_t BarId);
  Double_t GetPosition(TATWrawHit*a,TATWrawHit*b);
  Double_t GetChargeCenterofMass(TATWrawHit*a,TATWrawHit*b);


};

#endif
