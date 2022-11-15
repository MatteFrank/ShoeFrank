/*!
 \file
 \brief   Implementation of TAPLactNtuHit.
 */
#include "TH2F.h"

#include "TAPLntuRaw.hxx"
#include "TAPLntuHit.hxx"
#include "TAPLactNtuHit.hxx"

/*!
 \class TAPLactNtuHit
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAPLactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAPLactNtuHit::TAPLactNtuHit(const char* name, TAGdataDsc* p_nturaw, TAGdataDsc* p_datraw)
: TAPLactBaseNtuHit(name, "TAPLactNtuHit - NTuplize vertex raw data"),
  fpNtuRaw(p_nturaw),
  fpDatRaw(p_datraw)
{
   AddDataOut(p_nturaw, "TAPLntuHit");
   AddDataIn(p_datraw, "TAPLntuRaw");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAPLactNtuHit::~TAPLactNtuHit()
{}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAPLactNtuHit::Action()
{
   TAPLntuRaw* pDatRaw = (TAPLntuRaw*) fpDatRaw->Object();
   TAPLntuHit* pNtuRaw = (TAPLntuHit*) fpNtuRaw->Object();
   
   TAPLrawHit* rawHit =  pDatRaw->GetHit();
   TAPLhit* hit= pNtuRaw->NewHit(rawHit);
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
