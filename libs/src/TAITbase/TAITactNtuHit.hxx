#ifndef _TAITactNtuHit_HXX
#define _TAITactNtuHit_HXX

/*!
 \file TAITactNtuHit.hxx
 \brief   Declaration of TAITactNtuHit.
 */

/*------------------------------------------+---------------------------------*/

#include "TAITactBaseNtuHit.hxx"

class TAGdataDsc;
class DEITREvent;

using namespace std;
class TAITactNtuHit : public TAITactBaseNtuHit {
public:
   
   explicit TAITactNtuHit(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAITactNtuHit();
   
   virtual Bool_t  Action();
   
private:
   TAGdataDsc*     fpDatDaq;		    /// input data dsc
   
   ClassDef(TAITactNtuHit,0)
};

#endif
