/*!
 \file
 \brief   Implementation of TAPLactBaseNtuHit.
 */
#include "TH2F.h"

#include "TAPLntuRaw.hxx"
#include "TAPLntuHit.hxx"
#include "TAPLactBaseNtuHit.hxx"

/*!
 \class TAPLactBaseNtuHit
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAPLactBaseNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAPLactBaseNtuHit::TAPLactBaseNtuHit(const char* name, const char* title)
: TAGaction(name, title)
{
   TString tmp(name);
   fPrefix = tmp(0,2);
   
   fTitleDev = fPrefix;
   if (fPrefix.Contains("st"))
      fTitleDev = "STC";
   else if (fPrefix.Contains("tw"))
      fTitleDev = "TOF";
   else
      printf("Wrong prefix for histograms !");

}

//------------------------------------------+-----------------------------------
//! Destructor.

TAPLactBaseNtuHit::~TAPLactBaseNtuHit()
{}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAPLactBaseNtuHit::CreateHistogram()
{
   DeleteHistogram();
   
   fhAmplitude = new TH1F(Form("%sAmplitude", fPrefix.Data()), Form("%s - Amplitude", fTitleDev.Data()), 4096, 0, 500);
   AddHistogram(fhAmplitude);

   fhCharge    = new TH1F(Form("%sCharge", fPrefix.Data()), Form("%s - Charge", fTitleDev.Data()), 4096, -100, 5000);
   AddHistogram(fhCharge);

   fhBaseline  = new TH1F(Form("%sBaseline", fPrefix.Data()), Form("%s - Baseline", fTitleDev.Data()), 1000, -2, 2);
   AddHistogram(fhBaseline);

   fhRiseTime  = new TH1F(Form("%sRiseTime", fPrefix.Data()), Form("%s - RiseTime", fTitleDev.Data()), 80, 0, 50);
   AddHistogram(fhRiseTime);

   fhFallTime  = new TH1F(Form("%sFallTime ", fPrefix.Data()), Form("%s - FallTime ", fTitleDev.Data()), 400, 0, 250);
   AddHistogram(fhFallTime);

   fhTimeLE    = new TH1F(Form("%sTimeLE", fPrefix.Data()), Form("%s - Time LE ", fTitleDev.Data()), 512, 0, 200);
   AddHistogram(fhTimeLE);

   fhTimeCFD   = new TH1F(Form("%sTimeCFD ", fPrefix.Data()), Form("%s - Time CFD", fTitleDev.Data()), 512, 0, 200);
   AddHistogram(fhTimeCFD);

   SetValidHistogram(kTRUE);
   return;
}
