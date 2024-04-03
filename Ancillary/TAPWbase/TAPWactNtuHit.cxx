/*!
 \file
 \brief   Implementation of TAPWactNtuHit.
 */
#include "TH2F.h"

#include "TAPWntuRaw.hxx"
#include "TAPWntuHit.hxx"
#include "TAPWactNtuHit.hxx"

/*!
 \class TAPWactNtuHit
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAPWactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAPWactNtuHit::TAPWactNtuHit(const char* name, TAGdataDsc* p_nturaw, TAGdataDsc* p_datraw)
: TAPLactBaseNtuHit(name, "TAPWactNtuHit - NTuplize vertex raw data"),
  fpNtuRaw(p_nturaw),
  fpDatRaw(p_datraw)
{
   AddDataOut(p_nturaw, "TAPWntuHit");
   AddDataIn(p_datraw, "TAPWntuRaw");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAPWactNtuHit::~TAPWactNtuHit()
{}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAPWactNtuHit::CreateHistogram()
{
   TAPLactBaseNtuHit::CreateHistogram();
      
   fhChargeFast = new TH1F(Form("%sChargeFast", fPrefix.Data()), Form("%s - Fast Charge", fTitleDev.Data()), 4096, -100, 5000);
   AddHistogram(fhChargeFast);
   
   fhChargeSlow = new TH1F(Form("%sChargeSlow", fPrefix.Data()), Form("%s - Slow Charge", fTitleDev.Data()), 4096, -100, 5000);
   AddHistogram(fhChargeSlow);
}


//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAPWactNtuHit::Action()
{
   TAPWntuRaw* pDatRaw = (TAPWntuRaw*) fpDatRaw->Object();
   TAPWntuHit* pNtuRaw = (TAPWntuHit*) fpNtuRaw->Object();
   
   TAPWrawHit* rawHit =  pDatRaw->GetHit();
   TAPWhit* hit= pNtuRaw->NewHit(rawHit, true);
   hit->SetChannelId(rawHit->GetChannelId());

   if (ValidHistogram()) {
      fhAmplitude->Fill(hit->GetAmplitude());
      fhCharge->Fill(hit->GetCharge());
      fhChargeFast->Fill(hit->GetChargeFast());
      fhChargeSlow->Fill(hit->GetChargeSlow());
      fhBaseline->Fill(hit->GetBaseline());
      fhRiseTime->Fill(hit->GetRiseTime());
      fhFallTime->Fill(hit->GetFallTime());
      fhTimeLE->Fill(hit->GetTimeLE());
      fhTimeCFD->Fill(hit->GetTimeCFD());
   }
   
   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}
