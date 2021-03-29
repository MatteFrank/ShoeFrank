/*!
 \file
 \brief   Implementation of TAIRactNtuHit.
 */

#include "InfoEvent.hh"
#include "TrgEvent.hh"

#include "GlobalPar.hxx"
#include "TAGdaqEvent.hxx"
#include "TAMCntuEvent.hxx"

#include "TAIRactNtuHit.hxx"

/*!
 \class TAIRactNtuHit TAIRactNtuHit.hxx "TAIRactNtuHit.hxx"
 \brief Get vertex raw data from DAQ (binary format) **
 */

ClassImp(TAIRactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAIRactNtuHit::TAIRactNtuHit(const char* name, TAGdataDsc* pNtuEvt, TAGdataDsc* pDatDaq)
: TAGaction(name, "TAIRactNtuHit - Unpack trigger raw data"),
  fpDatDaq(pDatDaq),
  fpNtuEvt(pNtuEvt)
{
  AddDataIn(pDatDaq, "TAGdaqEvent");
  AddDataOut(pNtuEvt, "TAMCntuEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAIRactNtuHit::~TAIRactNtuHit()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAIRactNtuHit::Action()
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
