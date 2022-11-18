/*!
 \file
 \brief   Implementation of TACEactNtuHit.
 */
#include "TH2F.h"

#include "TACEntuRaw.hxx"
#include "TACEntuHit.hxx"
#include "TACEactNtuHit.hxx"

/*!
 \class TACEactNtuHit
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TACEactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACEactNtuHit::TACEactNtuHit(const char* name, TAGdataDsc* p_nturaw, TAGdataDsc* p_datraw)
: TAPLactBaseNtuHit(name, "TACEactNtuHit - NTuplize vertex raw data"),
  fpNtuRaw(p_nturaw),
  fpDatRaw(p_datraw)
{
   AddDataOut(p_nturaw, "TACEntuHit");
   AddDataIn(p_datraw, "TACEntuRaw");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACEactNtuHit::~TACEactNtuHit()
{}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TACEactNtuHit::Action()
{
   TACEntuRaw* pDatRaw = (TACEntuRaw*) fpDatRaw->Object();
   TACEntuHit* pNtuRaw = (TACEntuHit*) fpNtuRaw->Object();
   
   TACErawHit* rawHit =  pDatRaw->GetHit();
   TACEhit* hit= pNtuRaw->NewHit(rawHit);
   hit->SetChannelId(rawHit->GetChannelId());

   if (ValidHistogram()) {
      fhAmplitude->Fill(hit->GetAmplitude());
      fhCharge->Fill(hit->GetCharge());
      fhBaseline->Fill(hit->GetBaseline());
      fhRiseTime->Fill(hit->GetRiseTime());
      fhFallTime->Fill(hit->GetFallTime());
      fhTimeLE->Fill(hit->GetTimeLE());
      fhTimeCFD->Fill(hit->GetTimeCFD());
   }
   
   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}
