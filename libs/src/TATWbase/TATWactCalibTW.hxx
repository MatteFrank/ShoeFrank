#ifndef _TATWactCalibTW_HXX
#define _TATWactCalibTW_HXX
/*!
  \file
  \version $Id: TATWactCalibTW.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TATWactCalibTW.
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
#include "TATWdatRaw.hxx"
#include "TATWntuHit.hxx"

#include "TASTntuRaw.hxx"

#include "Parameters.h"

class TATWactCalibTW : public TAGaction {

public:

  explicit        TATWactCalibTW(const char* name=0,
				TAGdataDsc* p_datraw=0,
				TAGdataDsc* p_nturaw=0,
				TAGdataDsc* p_stnturaw=0,
				TAGparaDsc* p_pargeo=0,
				TAGparaDsc* p_parmap=0,
				TAGparaDsc* p_calmap=0,
				TAGparaDsc* p_pargeo_gl=0);
  virtual         ~TATWactCalibTW();

  virtual Bool_t  Action();

  virtual void    CreateHistogram();

  ClassDef(TATWactCalibTW,0);


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

  ParticleType    fParticleID;
  Int_t           fEvtCnt;

  Float_t         fTofPropAlpha;    // inverse of light propagation velocity
  Float_t         fTofErrPropAlpha;

  //Raw Q and TOF histograms
  std::vector<TH1D*> _hQLayerX;
  std::vector<TH1D*> _hQLayerY;
  std::vector<TH1D*> _hTOFLayerX;
  std::vector<TH1D*> _hTOFLayerY;

private:
  //
  TATWhit* fCurrentHit;
  //
  Double_t GetRawEnergy(TATWrawHit*a,TATWrawHit*b);
  Double_t GetRawTime(TATWrawHit*a,TATWrawHit*b);

  Double_t GetPosition(TATWrawHit*a,TATWrawHit*b);

  Int_t    GetBarCrossId(Int_t layer, Int_t barId, Double_t rawPos);
  Int_t    GetPerpBarId(Int_t layer, Int_t barId, Double_t rawPos);

  ParticleType  ZbeamToParticleID(Int_t Zbeam);
};

#endif
