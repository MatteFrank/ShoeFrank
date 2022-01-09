#ifndef _TACAactNtuHit_HXX
#define _TACAactNtuHit_HXX
/*!
  \file
  \version $Id: TACAactNtuHit.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TACAactNtuHit.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TACAparGeo.hxx"
#include "TACAntuRaw.hxx"
#include "TACAntuHit.hxx"
#include "TACAparCal.hxx"
#include "TACAcalibrationMap.hxx"

class TACAactNtuHit : public TAGaction {

public:

  explicit        TACAactNtuHit(const char* name=0,
                                TAGdataDsc* p_datraw=0,
                                TAGdataDsc* p_datdaq=0,
                                TAGparaDsc* p_parmap=0,
                                TAGparaDsc* p_parcal=0);
  virtual         ~TACAactNtuHit();

  Bool_t   Action();

  void     CreateHistogram();
  void     SetTemperatureFunctions();
  void     SetParFunction();

  Double_t TemperatureCorrFunction(Double_t* x, Double_t* par);

  Double_t GetTime(Double_t RawTime, Int_t  crysId);
  Double_t GetEnergy(Double_t rawEnergy, Int_t  crysId);
  Double_t GetTemperatureCorrection(Double_t rawEnergy, Int_t  crysId);
  Double_t GetEqualisationCorrection(Double_t rawEnergy, Int_t  crysId);
   
private:
  TAGdataDsc*     fpDatRaw;		    // input data dsc
  TAGdataDsc*     fpNtuRaw;		    // output data dsc
  TAGparaDsc*     fpParMap;        // output data dsc
  TAGparaDsc*     fpParCal;        // output data dsc

  Double_t        fTcorr1Par1;
  Double_t        fTcorr1Par0;
  Double_t        fTcorr2Par1;
  Double_t        fTcorr2Par0;
  Double_t        fT1;
  Double_t        fT2;


  TH1F*           fhArrivalTime[8];
  TH1F*           fhCharge[8];
  TH1F*           fhAmplitude[8];
  TH1F*           fhTrigTime;
  TH1F*           fhTotCharge;
  TH1F*           fhEventTime;
  TH1F*           fhChannelMap;

  TF1*            fTcorr1;
  TF1*            fTcorr2;
  
   ClassDef(TACAactNtuHit,0)
};

#endif
