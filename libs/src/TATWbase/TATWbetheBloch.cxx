#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TMath.h"

#include "TATWbetheBloch.hxx"

ClassImp(TATWbetheBloch);

const Double_t TATWbetheBloch::fgkFactorK      = 0.307075;	 // MeV cm2/g for A = 1 g/mol, so unit = MeV.
const Double_t TATWbetheBloch::fgkMassElectron = 0.5109;     // MeV
const Double_t TATWbetheBloch::fgkIonisation   = 4.7042E-5;	 // Mean excitation energy for scintillator BC408 [MeV] to be changed for EJ232!

//------------------------------------------+-----------------------------------
TATWbetheBloch::TATWbetheBloch(TATWparGeo* parGeo)
 : TAGobject(),
   fpParGeo(parGeo),
   fBarDensity(0.)
{
   fBarDensity       = parGeo->GetBarDensity();
   TString name      = parGeo->GetBarMat();
   TGeoMaterial* mat = gGeoManager->GetMaterial(name.Data());
   if (mat) {
      fBarZ = mat->GetZ();
      fBarA = mat->GetA();
   } else
      Error("TATWbetheBloch()", "Unknown material %s", name.Data());
}

//------------------------------------------+-----------------------------------
TATWbetheBloch::~TATWbetheBloch()
{
}

//------------------------------------------+-----------------------------------
Double_t TATWbetheBloch::GetDeltaE(Double_t deltaX, Double_t beta, Double_t z, Double_t mass)
{
   // mass in MeV, deltaX in cm
   mass         *= TAGgeoTrafo::GevToMev();
   Double_t K    = fgkFactorK;
   Double_t ro   = fBarDensity;
   Double_t Zmed = fBarZ;
   Double_t Amed = fBarA;
   Double_t I    = fgkIonisation;
   Double_t Q    = K*ro*z*z*(Zmed/Amed);		//K*ro*z^2*Z/A
   Double_t me   = fgkMassElectron;
   
   Double_t beta2     = beta*beta;
   Double_t gamma     = TMath::Sqrt(1/(1-beta2));
   Double_t gamma2    = gamma *gamma;
   Double_t Tmax      = 2*gamma2*beta2*me/( 1+(2*gamma*(me/mass)) + (me/mass)*(me/mass) );
   Double_t logarithm = 0.5*TMath::Log(2*me*beta2*gamma2*Tmax/(I*I)) - beta2;
   Double_t dEdX      = Q/beta2*logarithm;
   Double_t dE        = dEdX*deltaX;
  
  return dE;
}
