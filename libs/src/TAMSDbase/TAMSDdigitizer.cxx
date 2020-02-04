#include "TAMSDdigitizer.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGgeoTrafo.hxx"

#include <math.h>
#include "TVector2.h"
#include "TRandom.h"
#include "TList.h"
#include "TF1.h"

ClassImp(TAMSDdigitizer);

using namespace std;

Float_t TAMSDdigitizer::fgChargeGain = 20;

// --------------------------------------------------------------------------------------
TAMSDdigitizer::TAMSDdigitizer(TAMSDparGeo* parGeo)
: TAGbaseDigitizer(),
  fpParGeo(parGeo),
  fPitch(0.),
  fStripsN(-1),
  fView(-1),
  fEtaLimLo(0.01),
  fEtaLimUp(0.99),
  fEtaCst(16.322),
  fEtaErrCst(0.001)
{
  fStripsN = fpParGeo->GetNStrip();
  fPitch   = fpParGeo->GetPitch();

   SetFunctions();
   SetParFunction();
}

// --------------------------------------------------------------------------------------
TAMSDdigitizer::~TAMSDdigitizer() 
{   
}

//------------------------------------------+-----------------------------------
//! fill pixel signal
Bool_t TAMSDdigitizer::Process( Double_t edep, Double_t x0, Double_t y0,  Double_t zin, Double_t zout,  Double_t /*time*/, Int_t sensorId, Int_t /*Z*/)
{
  //   if (fgSmearFlag) {
  //      Float_t dx = gRandom->Gaus(0, fPitchX/2.);
  //      Float_t dy = gRandom->Gaus(0, fPitchX/2.);
  //      x0 += dx;
  //      y0 += dy;
  //   }
  
  Int_t view = fpParGeo->GetSensorPar(sensorId).TypeIdx;
  
  Float_t pos = 0.;
  
  if (view == 0)
    pos = x0;
  else
    pos = y0;
  
  fMap.clear();
  
  SetParFunction();
   
  edep *= TAGgeoTrafo::GevToKev();
  Double_t value    = 0.;
  Int_t    strip    = GetStrip(pos);
  Double_t posStrip = fpParGeo->GetPosition(strip);
  Double_t eta      = GetEta((pos-posStrip)*1e4);
   
  if (eta > 0.5) { // fill 3 strip around seed
     if (strip-1 >= 0) {
        value = edep*fgChargeGain*(1-eta/2.);
        FillMap(strip-1, value);
     }
     
     value = edep*fgChargeGain*eta;
     FillMap(strip, value);
     
     if (strip+1 < fStripsN) {
        value = edep*fgChargeGain*(1-eta/2.);
        FillMap(strip+1, value);
     }
  } else { // fill 3 strip around seed -1
     if (strip-2 >= 0) {
        value = edep*fgChargeGain*eta/2.;
        FillMap(strip-2, value);
     }
     if (strip-1 >= 0) {
        value = edep*fgChargeGain*(1-eta);
        FillMap(strip-1, value);
     }
     if (strip < fStripsN) {
        value = edep*fgChargeGain*eta/2.;
        FillMap(strip, value);
     }
  }
   
  return true;
}


//___________________________________________________________________________________________
Double_t TAMSDdigitizer::GetEta(Double_t pos)
{
   Double_t a   = fEtaLimLo;
   Double_t b   = fEtaLimUp;
   Double_t eta = (b+a)/2. ;
   Double_t fm  = fFuncEta->Eval(eta) ;
   
   static Double_t eps = 1e-2;
   
   while (TMath::Abs(a - b) > eps ) {
     eta = (b+a)/2. ;
     fm  = fFuncEta->Eval(eta);
      
      if (fm > pos )
         b = eta;
      else
         a = eta;
   }
   
   return eta;
}

// --------------------------------------------------------------------------------------
void TAMSDdigitizer::SetFunctions()
{
   // compute X0 vs Eta
   fFuncEta = new TF1("EtaX0", this, &TAMSDdigitizer::EtaX0, fEtaLimLo, fEtaLimUp, 1, "TAMSDdigitizer", "EtaX0");
}

// --------------------------------------------------------------------------------------
void TAMSDdigitizer::SetParFunction()
{
   // position of impact X0 versus Eta
   Double_t cst = fEtaCst + gRandom->Gaus(0, fEtaErrCst);
   fFuncEta->SetParameter(0, cst);
}

// --------------------------------------------------------------------------------------
Double_t TAMSDdigitizer::EtaX0(Double_t* x, Double_t* par)
{
   Float_t xx = x[0];
   Float_t X0 = -par[0]*TMath::Log(1./xx-1.);
   
   return X0;
}

// --------------------------------------------------------------------------------------
void TAMSDdigitizer::FillMap(Int_t strip, Double_t value)
{
  if (strip < 0) return;
  
  fMap[strip] = value;
}

//_____________________________________________________________________________
Int_t TAMSDdigitizer::GetStrip(Float_t pos) const
{
  // equivalent to  floor((-y-ymin)/fPitchV)-1
  Float_t min = -fStripsN*fPitch/2.;
  
  if (pos < min || pos > -min) {
    if (fDebugLevel)
      Warning("GetLine()", "Value of Y: %f out of range +/- %f\n", pos, min);
    return -1;
  }
  
  Int_t strip = floor((pos-min)/fPitch);
  
  return strip;
}

