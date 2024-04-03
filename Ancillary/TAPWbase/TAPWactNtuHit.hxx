#ifndef _TAPWactNtuHit_HXX
#define _TAPWactNtuHit_HXX
/*!
 \file
 \brief   Declaration of TAPWactNtuHit.
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAPLactBaseNtuHit.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

class TH1F;

class TAPWactNtuHit : public TAPLactBaseNtuHit {
public:
   explicit  TAPWactNtuHit(const char* name=0,
                           TAGdataDsc* p_nturaw=0,
                           TAGdataDsc* p_datraw=0);
   virtual ~TAPWactNtuHit();
   
   //! Base action 
   virtual Bool_t  Action();
   
   //! Base creation of histogram
   virtual  void   CreateHistogram();

private:
   TAGdataDsc*     fpNtuRaw;		    // output data dsc
   TAGdataDsc*     fpDatRaw;		    // input data dsc
   
   TH1F*           fhChargeFast;
   TH1F*           fhChargeSlow;

   ClassDef(TAPWactNtuHit,0)
};

#endif
