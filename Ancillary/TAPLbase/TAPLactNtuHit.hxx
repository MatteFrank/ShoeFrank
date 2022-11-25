#ifndef _TAPLactNtuHit_HXX
#define _TAPLactNtuHit_HXX
/*!
 \file
 \brief   Declaration of TAPLactNtuHit.
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAPLactBaseNtuHit.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

class TH1F;

class TAPLactNtuHit : public TAPLactBaseNtuHit {
public:
   explicit  TAPLactNtuHit(const char* name=0,
                           TAGdataDsc* p_nturaw=0,
                           TAGdataDsc* p_datraw=0);
   virtual ~TAPLactNtuHit();
   
   //! Base action 
   virtual Bool_t  Action();
   
private:
   TAGdataDsc*     fpNtuRaw;		    // output data dsc
   TAGdataDsc*     fpDatRaw;		    // input data dsc
   
   ClassDef(TAPLactNtuHit,0)
};

#endif
