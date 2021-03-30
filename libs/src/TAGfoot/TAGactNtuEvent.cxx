/*!
 \file
 \brief   Implementation of TAGactNtuEvent.
 */

#include "InfoEvent.hh"
#include "TrgEvent.hh"

#include "GlobalPar.hxx"
#include "TAGdaqEvent.hxx"
#include "TAGntuEvent.hxx"

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
  AddDataOut(pNtuEvt, "TAGntuEvent");
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
  TAGntuEvent* pNtuEvt = (TAGntuEvent*)  fpNtuEvt->Object();
  
  InfoEvent*  infoEvent = datDaq->GetInfoEvent();
  TrgEvent*   trgEvent  = datDaq->GetTrgEvent();
  
  if (trgEvent->eventNumber != infoEvent->eventNumber)
    Warning("Action()", "Event number different in trigger and info event");
  
  pNtuEvt->SetTimeSec(trgEvent->time_sec);
  pNtuEvt->SetTimeUSec(trgEvent->time_usec);
  pNtuEvt->SetEventNumber(trgEvent->eventNumber);
  pNtuEvt->SetLiveTime(trgEvent->liveTime);
  pNtuEvt->SetTimeSinceLastTrigger(trgEvent->timeSinceLastTrigger);
  pNtuEvt->SetClockCounter(trgEvent->clockCounter);
  pNtuEvt->SetTriggerCounter(trgEvent->triggerCounter);
  pNtuEvt->SetBCOofTrigger(trgEvent->BCOofTrigger);
  pNtuEvt->SetSpillNrAndTrgFineDelay(trgEvent->spillNrAndTrgFineDelay);
  pNtuEvt->SetPMTsAndBusy(trgEvent->PMTsAndBusy);

  SetBit(kValid);
  fpNtuEvt->SetBit(kValid);
  
  return kTRUE;
}
