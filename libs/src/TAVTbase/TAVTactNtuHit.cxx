/*!
 \file
 \brief   Implementation of TAVTactNtuHit.
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
 \class TAVTactNtuHit TAVTactNtuHit.hxx "TAVTactNtuHit.hxx"
 \brief Get vertex raw data from DAQ (binary format) **
 */

ClassImp(TAVTactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

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
   static Bool_t first = false;
   
   const DECardEvent* evt = 0x0;
   const DECardEvent* evt0 = 0x0;
   static const DECardEvent* evtp = 0x0;

   static UInt_t prefBco = 0;
   for (Int_t i = 0; i < nFragments; ++i) {
      
       TString type = datDaq->GetClassType(i);
       if (type.Contains("DECardEvent")) {
          evt0  = static_cast<const DECardEvent*> (datDaq->GetFragment(i));
          
          if (fQueueEvtsN == 0)
             evt = evt0;
          else {
             evt = evtp;//fQueueEvt.front();
             //fQueueEvt.push(evt0);
          }
          bcoTrig    = evt->BCOofTrigger;
          fData      = evt->values;
          fEventSize = evt->evtSize;
          fDataLink  = evt->channelID - (dataVTX | 0x30);
          if (fEventSize == 0) continue;
          DecodeEvent();
          
  //        if (fQueueEvtsN > 0)
//              fQueueEvt.pop();
       }
   }
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);
   
   if (!evt) return true;
   
   TrgEvent* trig = datDaq->GetTrgEvent();
   if (!trig) return true;

   if (!first) {
      fFirstBcoTrig = bcoTrig-trig->BCOofTrigger;
      first = true;
   }
   
   Int_t diff    = bcoTrig - trig->BCOofTrigger - fFirstBcoTrig;
   Int_t prediff = prefBco - trig->BCOofTrigger - fFirstBcoTrig;
   fpHisBCOofTrigger->Fill(diff);
   
   if (TMath::Abs(float(diff)) > 3) {
      Warning("Action()", "BCOofTrigger difference higher than 3 (%d) for %d time, resynchronizing", diff, fQueueEvtsN+1);
      
 //     fQueueEvt.push(evtp);
      fQueueEvtsN++;
   }
   if (fQueueEvtsN > 0) {
      if (evtp)
         delete evtp;
      evtp = new DECardEvent(*evt0);
   }

   prefBco = bcoTrig;
   
   return kTRUE;
}
