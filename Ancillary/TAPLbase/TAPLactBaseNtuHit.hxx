#ifndef _TAPLactBaseNtuHit_HXX
#define _TAPLactBaseNtuHit_HXX
/*!
 \file
 \brief   Declaration of TAPLactBaseNtuHit.
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

class TH1F;

class TAPLactBaseNtuHit : public TAGaction {
public:
   explicit  TAPLactBaseNtuHit(const char* name=0, const char* title=0);
   virtual ~TAPLactBaseNtuHit();
   
   
   //! Base creation of histogram
   virtual  void   CreateHistogram();
   
protected:
   Int_t           fDebugLevel;         // debug level
   
   TString         fPrefix;
   TString         fTitleDev;        

   TH1F*           fhAmplitude;
   TH1F*           fhCharge;
   TH1F*           fhBaseline;
   TH1F*           fhRiseTime;
   TH1F*           fhFallTime;
   TH1F*           fhTimeLE;
   TH1F*           fhTimeCFD;

   ClassDef(TAPLactBaseNtuHit,0)
};

#endif
