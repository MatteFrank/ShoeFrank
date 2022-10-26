/*!
 \file TAITactNtuHit.cxx
 \brief   Ntuplizer for ITR raw data
 */

#include "TH2F.h"

#include "DEITREvent.hh"
#include "DAQMarkers.hh"

#include "TAGrecoManager.hxx"
#include "TAGdaqEvent.hxx"
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"

#include "TAITactNtuHit.hxx"

/*!
 \class TAITactNtuHit
 \brief Ntuplizer for ITR raw data
 */

//! Class Imp
ClassImp(TAITactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[out] pNtuRaw hit container descriptor
//! \param[in] pDatDaq daq event container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pParMap mapping parameter descriptor
TAITactNtuHit::TAITactNtuHit(const char* name, TAGdataDsc* pNtuRaw, TAGdataDsc* pDatDaq, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAITactBaseNtuHit(name, pNtuRaw, pGeoMap, pConfig, pParMap),
  fpDatDaq(pDatDaq)
{
   AddDataIn(pDatDaq, "TAGdaqEvent");
   AddDataOut(pNtuRaw, "TAITntuHit");
   AddPara(pGeoMap, "TAITparGeo");
   AddPara(pConfig, "TAITparConf");
   
   TAITparGeo* parGeo = (TAITparGeo*) fpGeoMap->Object();
   fSensorsN = parGeo->GetSensorsN();
   
   for (Int_t i = 0; i < fSensorsN; ++i) {
      fPrevEventNumber[i]   = 0;
      fPrevTriggerNumber[i] = 0;
      fPrevTimeStamp[i]     = 0;
   }
   
   Int_t size = parGeo->GetSensorsN()*sizeof(MI26_FrameRaw)*4;
   fData.resize(size);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactNtuHit::~TAITactNtuHit()
{   
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAITactNtuHit::Action()
{
   
   TAGdaqEvent* datDaq = (TAGdaqEvent*)  fpDatDaq->Object();
   
   Int_t nFragments = datDaq->GetFragmentsN();
   
   for (Int_t i = 0; i < nFragments; ++i) {
      
       TString type = datDaq->GetClassType(i);
       if (type.Contains("DEITREvent")) {
          const DEITREvent* evt = static_cast<const DEITREvent*> (datDaq->GetFragment(i));
          fData      = evt->values;
          fEventSize = evt->evtSize;
          fDataLink  = evt->channelID - (dataITR | 0x30);
          if (fEventSize == 0) continue;
          DecodeEvent();
       }
   }
   
   SetBit(kValid);
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}

