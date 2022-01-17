/*!
 \file TAVTactNtuHit.cxx
 \brief   Get vertex raw data from DAQ (binary format)
 */

#include "TH1F.h"
#include "TMath.h"

#include "DECardEvent.hh"
#include "DAQMarkers.hh"

#include "TAGrecoManager.hxx"
#include "TAGdaqEvent.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"

#include "TAVTactNtuHit.hxx"

/*!
 \class TAVTactNtuHit
 \brief Get vertex raw data from DAQ (binary format)
 */

//! Class Imp
ClassImp(TAVTactNtuHit);

UInt_t TAVTactNtuHit::fgTStolerance = 40;

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[out] pNtuRaw hit container descriptor
//! \param[in] pDatDaq daq event container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pParMap mapping parameter descriptor
TAVTactNtuHit::TAVTactNtuHit(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pDatDaq, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAVTactBaseNtuHit(name, pNtuRaw, pGeoMap, pConfig, pParMap),
  fpDatDaq(pDatDaq),
  fFirstBcoTrig(0),
  fQueueEvtsN(0)
{
   AddDataIn(pDatDaq, "TAGdaqEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuHit::~TAVTactNtuHit()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAVTactNtuHit::Action()
{
   TAGdaqEvent* datDaq = (TAGdaqEvent*)  fpDatDaq->Object();
   
   Int_t nFragments = datDaq->GetFragmentsN();
   UInt_t bcoTrig = 0;
   UInt_t evtNumber = 0;
   static Bool_t first = false;
   
   const DECardEvent* evt  = 0x0;
   const DECardEvent* evt0 = 0x0;
   const DECardEvent* evtp = 0x0;

   for (Int_t i = 0; i < nFragments; ++i) {
      
       TString type = datDaq->GetClassType(i);
       if (type.Contains("DECardEvent")) {
          evt0  = static_cast<const DECardEvent*> (datDaq->GetFragment(i));
          
          if (fQueueEvtsN == 0)
             evt = evt0;
          else
             evt = fQueueEvt.front();
          bcoTrig    = evt->BCOofTrigger;
          fData      = evt->values;
          fEventSize = evt->evtSize;
          fDataLink  = evt->channelID - (dataVTX | 0x30);
          if (fEventSize == 0) continue;
          DecodeEvent();
       }
   }
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);
   
   if (!evt) return true;
   
   TrgEvent* trig = datDaq->GetTrgEvent();
   if (!trig) return true;

   InfoEvent* evtInfo = datDaq->GetInfoEvent();
   if (!evtInfo) return true;

   evtNumber = evtInfo->eventNumber;
   
   if (!first) {
      fFirstBcoTrig = bcoTrig-trig->BCOofTrigger;
      first = true;
   }
   
   Int_t diff = bcoTrig - trig->BCOofTrigger - fFirstBcoTrig;
   fpHisBCOofTrigger->Fill(evtNumber, diff);
   
   if (TMath::Abs(float(diff)) > fgTStolerance) {
      Warning("Action()", "BCOofTrigger difference higher than %u (%d) for %d time(s), resynchronizing", fgTStolerance, diff, fQueueEvtsN+1);
      if (diff > 0) // to avoid corrupted timestamp number
         fQueueEvtsN++;
      else
          Warning("Action()", "BCOofTrigger difference negative value, do not resynchronize");
   }
   
   if (fQueueEvtsN > 0) {
      if (fQueueEvtsN - fQueueEvt.size() == 0)
         fQueueEvt.pop();
      evtp = new DECardEvent(*evt0);
      fQueueEvt.push(evtp);
   }
   
   return kTRUE;
}
