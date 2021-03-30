/*!
 \file
 \brief   Implementation of TAGactNtuEvent.
 */

#include "InfoEvent.hh"
#include "TrgEvent.hh"

#include "GlobalPar.hxx"
#include "TAGdaqEvent.hxx"
#include "TAMCntuEvent.hxx"

#include "TAGactNtuEvent.hxx"

/*!
 \class TAGactNtuEvent TAGactNtuEvent.hxx "TAGactNtuEvent.hxx"
 \brief Get trigger from DAQ **
 */

ClassImp(TAGactNtuEvent);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGactNtuEvent::TAGactNtuEvent(const char* name, TAGdataDsc* pNtuEvt, TAGdataDsc* pDatDaq)
: TAGaction(name, "TAGactNtuEvent - Unpack trigger raw data"),
  fpDatDaq(pDatDaq),
  fpNtuEvt(pNtuEvt)
{
  AddDataIn(pDatDaq, "TAGdaqEvent");
  AddDataOut(pNtuEvt, "TAMCntuEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuEvent::~TAGactNtuEvent()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAGactNtuEvent::Action()
{
  TAGdaqEvent* datDaq = (TAGdaqEvent*)  fpDatDaq->Object();
  TAMCntuEvent* pNtuEvt = (TAMCntuEvent*)  fpNtuEvt->Object();
  
  InfoEvent*  infoEvent = datDaq->GetInfoEvent();
  TrgEvent*   trgEvent  = datDaq->GetTrgEvent();
  
  if (trgEvent->eventNumber != infoEvent->eventNumber)
    Warning("Action()", "Event number different in trigger and info event");
  
  pNtuEvt->SetEventNumber(trgEvent->eventNumber);
  pNtuEvt->SetTriggerNumber(trgEvent->triggerCounter);

  SetBit(kValid);
  fpNtuEvt->SetBit(kValid);
  
  return kTRUE;
}
