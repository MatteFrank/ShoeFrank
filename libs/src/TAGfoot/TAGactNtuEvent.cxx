/*!
 \file TAGactNtuEvent.cxx
 \brief   Implementation of TAGactNtuEvent.
 */

#include "InfoEvent.hh"
#include "TrgEvent.hh"

#include "TAGrecoManager.hxx"
#include "TAGdaqEvent.hxx"
#include "TAGntuEvent.hxx"

#include "TAGactNtuEvent.hxx"

/*!
 \class TAGactNtuEvent
 \brief Get trigger event from DAQ **
 */

//! Class Imp
ClassImp(TAGactNtuEvent);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[out] pNtuEvt  event container descriptor
//! \param[in]  pDatDaq  daq event descriptor
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
  TAGdaqEvent* datDaq  = (TAGdaqEvent*) fpDatDaq->Object();
  TAGntuEvent* pNtuEvt = (TAGntuEvent*) fpNtuEvt->Object();
  
  InfoEvent* infoEvent = datDaq->GetInfoEvent();
  TrgEvent*  trgEvent  = datDaq->GetTrgEvent();
   
  if (infoEvent && trgEvent) {
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


     hDeltaTimeEvents->Fill(trgEvent->timeSinceLastTrigger*1E-3);
     Double_t tottime = trgEvent->time_sec+1E-6*trgEvent->time_usec;
     hDAQEventsVsTime->Fill(tottime);
     
     
     
  }
  SetBit(kValid);
  fpNtuEvt->SetBit(kValid);
  
  return kTRUE;
}


void TAGactNtuEvent::CreateHistogram(){

  DeleteHistogram();
  
  char histoname[100]="";
  if(FootDebugLevel(1))
     cout<<"I have created the DAQ histo "<<endl;

  
  strcpy(histoname,"DeltaTimeEvents");
  hDeltaTimeEvents = new TH1F(histoname, histoname, 20000, 0., 200.);
  AddHistogram(hDeltaTimeEvents);

  strcpy(histoname,"DAQEventsVsTime");
  hDAQEventsVsTime = new TH1F(histoname, histoname, 300000, 0., 300.);
  AddHistogram(hDAQEventsVsTime);

 

  SetValidHistogram(kTRUE);

}
