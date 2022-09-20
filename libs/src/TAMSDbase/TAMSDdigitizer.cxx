/*!
 \file TAMSDdigitizer.cxx
 \brief   Implementation of TAMSDdigitizer.
 */

#include "TAMSDdigitizer.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"

#include <math.h>
#include "TVector2.h"
#include "TRandom.h"
#include "TList.h"
#include "TF1.h"

using namespace std;

Float_t TAMSDdigitizer::fgChargeGain  = 1.;
Bool_t  TAMSDdigitizer::fgSmearFlag   = false;
Float_t TAMSDdigitizer::fgDefSmearPos = 70;    // in micron

/*!
 \class TAMSDdigitizer
 \brief digitizer for MSD
 */

// --------------------------------------------------------------------------------------
//! Constructor
//!
//! \param[in] parGeo geometry parameter
TAMSDdigitizer::TAMSDdigitizer(TAMSDparGeo* parGeo)
: TAGbaseDigitizer(),
  fpParGeo(parGeo),
  fPitch(0.),
  fStripsN(-1),
  fView(-1),
  fEtaLimLo(0.01),
  fEtaLimUp(0.99),
  fEtaCst(16.322),
  fEtaErrCst(0.001),
  fResPar0(0),
  fResErrPar0(0),
  fResPar1(12.),
  fResErrPar1(5.),
  fResPar2(0),
  fResErrPar2(0)
{
  fStripsN = fpParGeo->GetStripsN();
  fPitch   = fpParGeo->GetPitch();

   SetFunctions();
   SetParFunction();
}

// --------------------------------------------------------------------------------------
//! Destructor
TAMSDdigitizer::~TAMSDdigitizer() 
{   
}

//------------------------------------------+-----------------------------------
//! Fill hit signal
//!
//! \param[in] edep energy loss
//! \param[in] x0  impact position in X direction
//! \param[in] y0  impact position in Y direction
//! \param[in] zin impact position in z direction
//! \param[in] zout outgoing position in z direction
//! \param[in] time impact time
//! \param[in] sensorId plane id
//! \param[in] Z atomic charge of the particle
//! \param[in] px0 momentum in X direction
//! \param[in] py0 momentum in Y direction
//! \param[in] pz0 momentum in z direction
Bool_t TAMSDdigitizer::Process( Double_t edep, Double_t x0, Double_t y0,  Double_t zin, Double_t zout,  Double_t /*time*/, Int_t sensorId, Int_t /*Z*/, Double_t /*px0*/, Double_t /*py0*/, Double_t /*pz0*/)
{
  if (fgSmearFlag) {
     Float_t dx = gRandom->Gaus(0, fgDefSmearPos*TAGgeoTrafo::MuToCm());
     Float_t dy = gRandom->Gaus(0, fgDefSmearPos*TAGgeoTrafo::MuToCm());
     x0 += dx;
     y0 += dy;
  }
  
  Int_t view = fpParGeo->GetSensorPar(sensorId).TypeIdx;
  
  Float_t pos = 0.;
  
  if (view == 0)
    pos = x0;
  else
    pos = y0;
  
  fMap.clear();
  
  SetParFunction();
   
  edep *= TAGgeoTrafo::GevToKev();
  Double_t value     = 0.;
  Double_t resEnergy = GetResEnergy(edep);
  Int_t    strip     = GetStrip(pos);
  Double_t posStrip  = fpParGeo->GetPosition(strip);
  Double_t eta       = GetEta((pos-posStrip)*TAGgeoTrafo::CmToMu());
   
  if (eta > 0.6 && eta < 0.9) { // fill 3 strip around seed
     if (strip-1 >= 0) {
        edep += gRandom->Gaus(0, resEnergy);
        value = edep*fgChargeGain*(1-eta/2.);
        resEnergy = GetResEnergy(edep);
        
        FillMap(strip-1, value);
     }
     
     edep += gRandom->Gaus(0, resEnergy);
     value = edep*fgChargeGain*eta;
     FillMap(strip, value);
     
     if (strip+1 < fStripsN) {
        edep += gRandom->Gaus(0, resEnergy);
        value = edep*fgChargeGain*(1-eta/2.);
        FillMap(strip+1, value);
     }
  } else if (eta > 0.4 && eta <= 0.6) { // fill only 2 strips
     
     edep += gRandom->Gaus(0, resEnergy);
     value = edep*fgChargeGain*eta;
     FillMap(strip, value);
     
     if (strip-1 >= 0) {
        edep += gRandom->Gaus(0, resEnergy);
        value = edep*fgChargeGain*(1-eta);
        FillMap(strip-1, value);
     }
     
  } else if (eta >= 0.9) { // only one strip
     edep += gRandom->Gaus(0, resEnergy);
     value = edep*fgChargeGain*eta;
     FillMap(strip, value);
     
  } else if (eta <= 0.1) { // only one strip-1
     if (strip-1 >= 0) {
        edep += gRandom->Gaus(0, resEnergy);
        value = edep*fgChargeGain*(1-eta);
        FillMap(strip-1, value);
     }
     
  } else { // fill 3 strip around seed -1
     if (strip-2 >= 0) {
        edep += gRandom->Gaus(0, resEnergy);
        value = edep*fgChargeGain*eta/2.;
        FillMap(strip-2, value);
     }
     if (strip-1 >= 0) {
        edep += gRandom->Gaus(0, resEnergy);
        value = edep*fgChargeGain*(1-eta);
        FillMap(strip-1, value);
     }
     if (strip < fStripsN) {
        edep += gRandom->Gaus(0, resEnergy);
        value = edep*fgChargeGain*eta/2.;
        FillMap(strip, value);
     }
  }
   
  return true;
}

//___________________________________________________________________________________________
//! Get eta for a given position
//!
//! \param[in] pos a given position
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
//! Set functions
void TAMSDdigitizer::SetFunctions()
{
   // compute X0 vs Eta
   fFuncEta = new TF1("EtaX0", this, &TAMSDdigitizer::EtaX0, fEtaLimLo, fEtaLimUp, 1, "TAMSDdigitizer", "EtaX0");
   
   // compute energy resolution
   fDeResE = new TF1("ResEnergy", this, &TAMSDdigitizer::ResEnergy, 0, 5000, 3, "TAMSDdigitizer", "ResEnergy");
}

// --------------------------------------------------------------------------------------
//! Set parameter function
void TAMSDdigitizer::SetParFunction()
{
   // position of impact X0 versus Eta
   Double_t cst = fEtaCst + gRandom->Gaus(0, fEtaErrCst);
   fFuncEta->SetParameter(0, cst);
   
   // Resolution parameter
   fDeResE->SetParameters(fResPar0, fResPar1, fResPar2);
}

// --------------------------------------------------------------------------------------
//! Eta function
//!
//! \param[in] x position vector
//! \param[in] par parameters vector
Double_t TAMSDdigitizer::EtaX0(Double_t* x, Double_t* par)
{
   Float_t xx = x[0];
   Float_t X0 = -par[0]*TMath::Log(1./xx-1.);
   
   return X0;
}

// --------------------------------------------------------------------------------------
//! Resolution energy function
//!
//! \param[in] x position vector
//! \param[in] par parameters vector
Double_t TAMSDdigitizer::ResEnergy(Double_t* x, Double_t* par)
{
   Float_t energy = x[0];
   Float_t res = TMath::Sqrt(par[0]*par[0]/energy + par[1]*par[1]/(energy*energy) + par[2]*par[2]);
   
   return res;
}

// --------------------------------------------------------------------------------------
//! Get resolution energy
//!
//! \param[in] edep energy loss
Double_t TAMSDdigitizer::GetResEnergy(Float_t edep)
{
   return fDeResE->Eval(edep)*edep;
}

// --------------------------------------------------------------------------------------
//! Fill strips
//!
//! \param[in] strip strip id
//! \param[in] value strip value
void TAMSDdigitizer::FillMap(Int_t strip, Double_t value)
{
  if (strip < 0) return;
  
   if (fMap[strip] < 0.0001)
      fMap[strip] = value;
   else
      fMap[strip] = fMap[strip]+value;
}

// --------------------------------------------------------------------------------------
//! Get strip number from position
//!
//! \param[in] pos a given position
Int_t TAMSDdigitizer::GetStrip(Float_t pos) const
{
  // equivalent to  floor((-y-ymin)/fPitchV)-1
  Float_t min = -fStripsN*fPitch/2.;
  
  if (pos < min || pos > -min) {
    if(FootDebugLevel(1))
      Warning("GetLine()", "Value of Y: %f out of range +/- %f\n", pos, min);
    return -1;
  }
  
  Int_t strip = floor((pos-min)/fPitch);
  
  return strip;
}

