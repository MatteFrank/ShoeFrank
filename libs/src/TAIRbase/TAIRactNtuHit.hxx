#ifndef _TAIRactNtuHit_HXX
#define _TAIRactNtuHit_HXX

#include "TAGaction.hxx"

/*!
 \file
 \version $Id: TAIRactNtuHit.hxx $
 \brief   Declaration of TAIRactNtuHit.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;

using namespace std;
class TAIRactNtuHit : public TAGaction {
public:
  
  explicit TAIRactNtuHit(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0);
  virtual  ~TAIRactNtuHit();
  
  virtual Bool_t  Action();
  
protected:
  TAGdataDsc*     fpDatDaq;		    // input data dsc
  TAGdataDsc*     fpNtuEvt;         // output data dsc
  
  ClassDef(TAIRactNtuHit,0)
};

#endif
