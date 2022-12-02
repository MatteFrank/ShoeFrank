#ifndef _TACAactNtuHit_HXX
#define _TACAactNtuHit_HXX
/*!
  \file TACAactNtuHit.hxx
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

  explicit  TACAactNtuHit(const char* name=0,
                          TAGdataDsc* p_datraw=0,
                          TAGdataDsc* p_datdaq=0,
                          TAGparaDsc* p_pargeo=0,
                          TAGparaDsc* p_parmap=0,
                          TAGparaDsc* p_parcal=0);
   virtual  ~TACAactNtuHit();

   Bool_t   Action();

   void     CreateHistogram();
   void     SetTemperatureFunctions();
   void     SetParFunction();

   Double_t TemperatureCorrFunction(Double_t* x, Double_t* par);

   Double_t GetTime(Double_t RawTime, Int_t  crysId);
   Double_t GetEnergy(Double_t rawEnergy, Int_t  crysId);
   Double_t GetTemperatureCorrection(Double_t rawEnergy, Double_t temp, Int_t  crysId);
   Double_t GetEqualisationCorrection(Double_t rawEnergy, Int_t  crysId);
   Double_t ADC2Temp(Double_t adc);

   
private:
   TAGdataDsc*     fpDatRaw;		    ///< input raw data
   TAGdataDsc*     fpNtuRaw;		    ///< output calibrated data
   TAGparaDsc*     fpParGeo;         ///< geometry pointer
   TAGparaDsc*     fpParMap;         ///< mapping pointer
   TAGparaDsc*     fpParCal;         ///< calibration pointer

   Double_t        fTcorr1Par1;      ///< Temperature correlation parameter 1
   Double_t        fTcorr1Par0;      ///< Temperature correlation parameter 0
   Double_t        fTcorr2Par1;      ///< Temperature correlation parameter 1
   Double_t        fTcorr2Par0;      ///< Temperature correlation parameter 0
   Double_t        fT1;              ///< Temperature  parameter 1
   Double_t        fT2;              ///< Temperature  parameter 2


   TH1F*           fhArrivalTime[9];  ///< histograms for arrival time
   TH1F*           fhCharge[9];       ///< histograms for charge
   TH1F*           fhAmplitude[9];    ///< histograms for amplitude
   TH1F*           fhTrigTime;        ///< histogram for trigger time
   TH1F*           fhTotCharge;       ///< histogram for total charge
   TH1F*           fhEventTime;       ///< histogram for event type
   TH1F*           fhChannelMap;      ///< histogram for channel map

   TF1*            fTcorr1;           ///< histogram for correlation tempetarure 1
   TF1*            fTcorr2;           ///< histogram for correlation tempetarure 2
  
   ClassDef(TACAactNtuHit, 0)
};

#endif
