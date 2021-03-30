/*!
 \file
 \brief   Implementation of TAGactNtuHit.
 */

#include "InfoEvent.hh"
#include "TrgEvent.hh"

#include "GlobalPar.hxx"
#include "TAGdaqEvent.hxx"
#include "TAMCntuEvent.hxx"

#include "TAGactNtuHit.hxx"

/*!
 \class TAGactNtuHit TAGactNtuHit.hxx "TAGactNtuHit.hxx"
 \brief Get trigger from DAQ **
 */

ClassImp(TAGactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGactNtuHit::TAGactNtuHit(const char* name, TAGdataDsc* pNtuEvt, TAGdataDsc* pDatDaq)
: TAGaction(name, "TAGactNtuHit - Unpack trigger raw data"),
  fpDatDaq(pDatDaq),
  fpNtuEvt(pNtuEvt)
{
  AddDataIn(pDatDaq, "TAGdaqEvent");
  AddDataOut(pNtuEvt, "TAMCntuEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuHit::~TAGactNtuHit()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAGactNtuHit::Action()
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
