#ifndef _TACAactNtuRaw_HXX
#define _TACAactNtuRaw_HXX
/*!
  \file
  \version $Id: TACAactNtuRaw.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TACAactNtuRaw.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TACAparGeo.hxx"
#include "TACAdatRaw.hxx"
#include "TACAntuRaw.hxx"
#include "TACAparCal.hxx"
#include "TACAcalibrationMap.hxx"

class TACAactNtuRaw : public TAGaction {

public:

  explicit        TACAactNtuRaw(const char* name=0,
                                TAGdataDsc* p_datraw=0,
                                TAGdataDsc* p_datdaq=0,
                                TAGparaDsc* p_parmap=0,
                                TAGparaDsc* p_parcal=0);
  virtual         ~TACAactNtuRaw();

  Bool_t   Action();

  void     CreateHistogram();
  void     SetTemperatureFunctions();
  void     SetParFunction();

  Double_t TemperatureCorrFunction(Double_t* x, Double_t* par);

  Double_t GetTime(Double_t RawTime, Int_t  crysId);
  Double_t GetEnergy(Double_t rawEnergy, Int_t  crysId);
  Double_t GetTemperatureCorrection(Double_t rawEnergy, Int_t  crysId);
  Double_t GetEqualisationCorrection(Double_t rawEnergy, Int_t  crysId);


  ClassDef(TACAactNtuRaw,0)
    
private:
  TAGdataDsc*     fpDatRaw;		    // input data dsc
  TAGdataDsc*     fpNtuRaw;		    // output data dsc
  TAGparaDsc*     fpParMap;        // output data dsc
  TAGparaDsc*     fpParCal;        // output data dsc

  TACAparMap*     f_parmap;
  TACAparCal*     f_parcal;

  Double_t        fTcorr1Par1;
  Double_t        fTcorr1Par0;
  Double_t        fTcorr2Par1;
  Double_t        fTcorr2Par0;
  Double_t        T1;
  Double_t        T2;

  TH1F *hArrivalTime[8];
  TH1F *hCharge[8];
  TH1F *hAmplitude[8];
  TH1F *hTrigTime;
  TH1F *hTotCharge;
  TH1F *hEventTime;

  TF1 *fTcorr1;
  TF1 *fTcorr2;
  
};

#endif
