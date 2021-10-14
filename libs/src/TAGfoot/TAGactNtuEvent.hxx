#ifndef _TAGactNtuEvent_HXX
#define _TAGactNtuEvent_HXX

#include "TAGWDtrigInfo.hxx"
#include "TAGaction.hxx"

/*!
 \file
 \version $Id: TAGactNtuEvent.hxx $
 \brief   Declaration of TAGactNtuEvent.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;

using namespace std;
class TAGactNtuEvent : public TAGaction {
public:
  
  //explicit TAGactNtuEvent(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0, TAGdataDsc* p_WDtrigInfo=0);
  explicit TAGactNtuEvent(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0);
  virtual  ~TAGactNtuEvent();
  
  virtual Bool_t  Action();
  
protected:
  TAGdataDsc*     fpDatDaq;		    // input data dsc
  TAGdataDsc*     fpNtuEvt;         // output data dsc
  
  ClassDef(TAGactNtuEvent,1)
};

#endif
