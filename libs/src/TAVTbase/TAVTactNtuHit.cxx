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
  fFirstBcoTrig(0)
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
   
   for (Int_t i = 0; i < nFragments; ++i) {
      
       TString type = datDaq->GetClassType(i);
       if (type.Contains("DECardEvent")) {
          const DECardEvent* evt = static_cast<const DECardEvent*> (datDaq->GetFragment(i));
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
   
   TrgEvent* trig = datDaq->GetTrgEvent();
   if (!trig) return true;

   if (!first) {
      fFirstBcoTrig = bcoTrig-trig->BCOofTrigger;
      first = true;
   }
   
   Int_t diff = bcoTrig-trig->BCOofTrigger-fFirstBcoTrig;
   fpHisBCOofTrigger->Fill(diff);
   if (TMath::Abs(float(diff)) > 3)
      Warning("Action()", "BCOofTrigger difference higher than 3 (%d)", diff);
   
   return kTRUE;
}
