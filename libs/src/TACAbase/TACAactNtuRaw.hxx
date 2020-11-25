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

#define REF_CLK make_pair(16,27)
#define CHISQUARE_THRESHOLD 1.0
#define AMPLITUDE_THRESHOLD 0.04

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

  Double_t GetTime(Double_t RawTime, Int_t  crysId);
  Double_t GetEnergy(Double_t rawEnergy, Int_t  crysId);

  ClassDef(TACAactNtuRaw,0)
    
private:
  TAGdataDsc*     fpDatRaw;		    // input data dsc
  TAGdataDsc*     fpNtuRaw;		    // output data dsc
  TAGparaDsc*     fpParMap;        // output data dsc
  TAGparaDsc*     fpParCal;        // output data dsc

  TH1F *hArrivalTime[8];
  TH1F *hCharge[8];
  TH1F *hAmplitude[8];
  TH1F *hTrigTime;
  TH1F *hTotCharge;
  TH1F *hEventTime;
  
};

#endif
