
#include "Riostream.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"

#include "TADItrackDiffusion.hxx"
#include "TAGgeoTrafo.hxx"

//##############################################################################

/*!
 \class TADItrackDiffusion TADItrackDiffusion.hxx "TADItrackDiffusion.hxx"
 \brief  Functions for scattering calculation. **
 */

ClassImp(TADItrackDiffusion);

//_____________________________________________________________________________
//
// Default constructor
TADItrackDiffusion::TADItrackDiffusion()
: TAGpara(),
  fAlpha(0),
  fPfactor(0)
{
   if (gGeoManager == 0x0)
      Error("TADItrackDiffusion()", "gGeoManager not defined");
   
   fFuncSigTheta = new TF1("funcSigTheta", this, &TADItrackDiffusion::SigmaTheta, 0, 500, 5, "TADItrackDiffusion", "SigmaTheta");
}

//_____________________________________________________________________________
//
// Destructor
TADItrackDiffusion::~TADItrackDiffusion()
{
   delete fFuncSigTheta;
}

//_____________________________________________________________________________
//
// Calculation of the WEPL of the material layer
Float_t TADItrackDiffusion::WEPLCalc(const TString& mat, Float_t thickness)
{
   
   Float_t factor = 0;
   Float_t waterEq = 0;
   
   TString material(mat);
   
   material.ToUpper();
   
   if (material == "AIR"){
      factor = 0.001045298;
   } else if (material == "SI"){
      factor = 1.833962703;
   } else if (material == "12C"){
      factor = 1.991445766;
   } else if (material == "PMMA"){
      factor = 0.925313636;
   } else if (material == "H2O" || material == "WATER"){
      factor = 1.;
   } else if (material == "TI"){
   	  factor = 3.136600294;
   } else Warning("WEPLCalc()","Material is not in the list.... Candidates: Air, Si, 12C, PMMA, H2O, Ti");
   
   waterEq = thickness * factor;

   return waterEq;
}

//_____________________________________________________________________________
//
// Calculation of the energy loss in the material layer (WEPL in [cm])
Float_t TADItrackDiffusion::EnergyCalc(Float_t energy, Float_t massNumber, Int_t atomicNumber, Float_t WEPL)
{
   if (energy < 250){
      fAlpha = 0.0022;
      fPfactor = 1.77;
   } else if ((energy >= 250) && (energy < 400)){
      fAlpha = 0.0022;
      fPfactor = 1.76;
   } else {
      fAlpha = 0.0022;
      fPfactor = 1.75;
   }
   
   Float_t range = (massNumber / (atomicNumber*atomicNumber) * fAlpha * pow(energy, fPfactor));
   Float_t path = range - WEPL;
   Float_t dE = pow((path * atomicNumber * atomicNumber / (fAlpha * massNumber)), (1/fPfactor));
   energy = dE;
   if (path < 0) {
      Info("EnergyCalc()","The remaining energy is 0....");
      energy = 0;
   }
   
   return energy;
}

//_____________________________________________________________________________
//
// Calculation of the impact*c [MeV]
Float_t TADItrackDiffusion::PCCalc(Float_t energy, Float_t massNumber)
{
   Float_t massFac = TAGgeoTrafo::GetMassFactorMeV();
   Float_t pc      = sqrt(energy * energy + 2*energy * massFac) * massNumber ;
   
   return pc;
}

//_____________________________________________________________________________
//
// Calculation of beta
Float_t TADItrackDiffusion::BetaCalc(Float_t energy)
{
   Float_t massFac = TAGgeoTrafo::GetMassFactorMeV();
   Float_t beta    = sqrt(1.0 - (1/(energy/massFac +1.0))*(1/(energy/massFac +1.0)));
   
   return beta;
}

// --------------------------------------------------------------------------------------
Double_t TADItrackDiffusion::SigmaTheta(Double_t *x, Double_t *par)
{
   /// Function to modelize the scattering angle of incident ions at a depth x (Highlands)
   /// par[0]=beta of particule
   /// par[1]=momentum MeV/c2
   /// par[2]=z of the beam
   /// par[3]=radiation length of the target material
   /// par[4]=density of target material
   /// x[0] in cm
   
   Double_t radL = x[0]*par[4]/par[3];
   Double_t A    = 14.1*par[2]/(par[0]*par[1]);
   Double_t B    = TMath::Sqrt(radL)*(1+TMath::Log10(radL)/9.);
   
   return A*B*TMath::RadToDeg();
}

// --------------------------------------------------------------------------------------
Float_t TADItrackDiffusion::SigmaThetaCalc(Float_t energy, TString mat, Float_t x, Float_t A, Float_t Z )
{
   Double_t pc   = PCCalc(energy, A);
   Double_t beta = BetaCalc(energy);
   Double_t rho  = GetDensity(mat);        // density of material  [g/cm^3]
   Double_t radL = GetRadLength(mat)*rho;  // Radiation length for material, multiply by density [g/cm^2]

   fFuncSigTheta->SetParameter(0, beta);
   fFuncSigTheta->SetParameter(1, pc);
   fFuncSigTheta->SetParameter(2, Z);
   fFuncSigTheta->SetParameter(3, radL);
   fFuncSigTheta->SetParameter(4, rho);
   
   return  fFuncSigTheta->Eval(x);
}

// --------------------------------------------------------------------------------------
Float_t TADItrackDiffusion::GetRadLength(TString name)
{
   TGeoMaterial* mat = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject(name.Data());
   if (mat == 0x0) {
      Warning("GetRadLength()", "Unknown material %s", name.Data());
      return -1;
   } else
      return mat->GetRadLen();
}

// --------------------------------------------------------------------------------------
Float_t TADItrackDiffusion::GetDensity(TString name)
{
   TGeoMaterial* mat = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject(name.Data());
   if (mat == 0x0) {
      Warning("GetDensity()", "Unknown material %s", name.Data());
      return -1;
   } else
      return mat->GetDensity();
}