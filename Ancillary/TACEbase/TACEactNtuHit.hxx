#ifndef _TACEactNtuHit_HXX
#define _TACEactNtuHit_HXX
/*!
 \file
 \brief   Declaration of TACEactNtuHit.
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAPLactBaseNtuHit.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

class TH1F;

class TACEactNtuHit : public TAPLactBaseNtuHit {
public:
   explicit  TACEactNtuHit(const char* name=0,
                           TAGdataDsc* p_nturaw=0,
                           TAGdataDsc* p_datraw=0);
   virtual ~TACEactNtuHit();
   
   //! Base action 
   virtual Bool_t  Action();
   
private:
   TAGdataDsc*     fpNtuRaw;		    // output data dsc
   TAGdataDsc*     fpDatRaw;		    // input data dsc

   ClassDef(TACEactNtuHit,0)
};

#endif
