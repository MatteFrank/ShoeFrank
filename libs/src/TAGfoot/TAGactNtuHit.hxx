#ifndef _TAGactNtuHit_HXX
#define _TAGactNtuHit_HXX

#include "TAGaction.hxx"

/*!
 \file
 \version $Id: TAGactNtuHit.hxx $
 \brief   Declaration of TAGactNtuHit.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;

using namespace std;
class TAGactNtuHit : public TAGaction {
public:
  
  explicit TAGactNtuHit(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0);
  virtual  ~TAGactNtuHit();
  
  virtual Bool_t  Action();
  
protected:
  TAGdataDsc*     fpDatDaq;		    // input data dsc
  TAGdataDsc*     fpNtuEvt;         // output data dsc
  
  ClassDef(TAGactNtuHit,0)
};

#endif
