#ifndef _TAGactNtuEvent_HXX
#define _TAGactNtuEvent_HXX

#include "TAWDntuTrigger.hxx"
#include "TAGaction.hxx"
#include <TH1F.h>

/*!
 \file TAGactNtuEvent.hxx
 \brief   Declaration of TAGactNtuEvent.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;

using namespace std;
class TAGactNtuEvent : public TAGaction {
public:
  
  explicit TAGactNtuEvent(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0);
  virtual  ~TAGactNtuEvent();
  
   // Action
  virtual Bool_t  Action();


  //create histograms
  virtual void      CreateHistogram();


protected:
  TAGdataDsc*     fpDatDaq;	///< input data dsc
  TAGdataDsc*     fpNtuEvt;   ///< output data dsc

  TH1F *hDeltaTimeEvents;
  TH1F *hDAQEventsVsTime;

  
  ClassDef(TAGactNtuEvent,1)
};

#endif
