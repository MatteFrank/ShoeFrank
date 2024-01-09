#ifndef _TASTactNtuHit_HXX
#define _TASTactNtuHit_HXX
/*!
  \file TASTactNtuHit.hxx
  \brief   Declaration of TASTactNtuHit.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TASTparGeo.hxx"
#include "TASTntuRaw.hxx"
#include "TASTntuHit.hxx"
#include "TH1.h"
#include "TH2.h"

class TASTactNtuHit : public TAGaction {

public:

  explicit        TASTactNtuHit(const char* name=0,
                                TAGdataDsc* p_datraw=0,
                                TAGdataDsc* p_datdaq=0,
                                TAGparaDsc* p_parmap=0);
  virtual         ~TASTactNtuHit();

  virtual Bool_t  Action();

  virtual void CreateHistogram();
   
private:
  TAGdataDsc*     fpDatRaw;		    // input data dsc
  TAGdataDsc*     fpNtuRaw;		    // output data dsc
  TAGparaDsc*     fpParMap;		    // output data dsc
  Int_t           fEventsN;

  TH1F *hArrivalTime[8];
  TH1F *hCharge[8];
  TH1F *hAmplitude[8];
  TH1F *hChargePedestal[8];
  TH1F *hBaseline[8];
  TH1F *hNoise[8];
  TH1F *hRiseTime[8];

  TH1F *hTotTime;
  TH1F *hTotCharge;
  TH1F *hTotNoise;
  TH1F *hTotAmplitude; 
  TH1F *hTotPedestal;
  TH1F *hTotBaseline;
  TH1F *hTotRiseTime;
  
  TH1F *hEff;
  TH1F *hPileUp;
  TH2F *hChargevsTime[3];
  
   
private:
  void   SavePlot(TGraph g,TF1 f1, TF1 f2, TASTrawHit *hit);

   ClassDef(TASTactNtuHit,0)
};

#endif
