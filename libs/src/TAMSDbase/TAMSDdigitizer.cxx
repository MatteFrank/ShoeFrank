#include "TAMSDdigitizer.hxx"
#include "TAMSDparGeo.hxx"

#include <math.h>
#include "TVector2.h"
#include "TRandom.h"
#include "TList.h"

ClassImp(TAMSDdigitizer);

using namespace std;

Float_t TAMSDdigitizer::fgChargeGain      = 2000;
Float_t TAMSDdigitizer::fgChargeFac       = 0.8;


// --------------------------------------------------------------------------------------
TAMSDdigitizer::TAMSDdigitizer(TAMSDparGeo* parGeo)
: TAGbaseDigitizer(),
  fpParGeo(parGeo),
  fPitch(0),
  fStripsN(-1),
  fView(-1)
{
  
  fStripsN = fpParGeo->GetNStrip();
  fPitch = fpParGeo->GetPitch();
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
  
  Double_t value = 0.;
  
  Int_t strip = GetStrip(pos);
  
  if (strip-1 >= 0) {
    value = edep*fgChargeGain*(1-fgChargeFac/2.);
    FillMap(strip-1, value);
  }
  
  value = edep*fgChargeGain*fgChargeFac;
  FillMap(strip, value);
  
  if (strip+1 < fStripsN) {
    value = edep*fgChargeGain*(1-fgChargeFac/2.);
    FillMap(strip+1, value);
  }
  
  return true;
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

