
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

Float_t TADItrackDiffusion::fgkX0w = 36.08;
//_____________________________________________________________________________
//
// Default constructor
TADItrackDiffusion::TADItrackDiffusion()
: TAGpara()
{
   if (gGeoManager == 0x0)
      Error("TADItrackDiffusion()", "gGeoManager not defined");
   
   fFuncSigTheta = new TF1("funcSigTheta", this, &TADItrackDiffusion::SigmaTheta, 0, 500, 4, "TADItrackDiffusion", "SigmaTheta");
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
Float_t TADItrackDiffusion::GetWEPL(const TString& mat, Float_t thickness)
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
   } else Warning("GetWEPL()","Material is not in the list.... Candidates: Air, Si, 12C, PMMA, H2O, Ti");
   
   waterEq = thickness * factor;

   return waterEq;
}

//_____________________________________________________________________________
//
// Calculation of the energy loss in the material layer (WEPL in [cm])
Float_t TADItrackDiffusion::GetEnergyLoss(Float_t energy, Float_t massNumber, Int_t atomicNumber, Float_t WEPL)
{
   // rage formula Bortfeld et al, PMB 41 (1996)
   // R = alpha*Energy^(pFactor)
   
   Float_t alpha   = 0.0022;
   Float_t pfactor = 1.;

   if (energy < 250){
      pfactor = 1.77;
   } else if ((energy >= 250) && (energy < 400)){
      pfactor = 1.76;
   } else {
      pfactor = 1.75;
   }
   
   Float_t rangeW = (massNumber / (atomicNumber*atomicNumber) * alpha * TMath::Power(energy, pfactor));
   Float_t path   = rangeW - WEPL;
   Float_t dE     = TMath::Power((path * atomicNumber * atomicNumber / (alpha * massNumber)), (1/pfactor));

   if (path < 0) {
      Info("GetEnergyLoss()","The remaining energy is 0....");
      dE = 0;
   }
   
   return dE;
}

//_____________________________________________________________________________
//
// Calculation of the impact*c [MeV]
Float_t TADItrackDiffusion::GetPCC(Float_t energy, Float_t massNumber)
{
   Float_t massFac = TAGgeoTrafo::GetMassFactorMeV();
   Float_t pc      = sqrt(energy * energy + 2*energy * massFac) * massNumber ;
   
   return pc;
}

//_____________________________________________________________________________
//
// Calculation of beta
Float_t TADItrackDiffusion::GetBeta(Float_t energy)
{
   Float_t massFac = TAGgeoTrafo::GetMassFactorMeV();
   Float_t beta    = sqrt(1.0 - (1/(energy/massFac +1.0))*(1/(energy/massFac +1.0)));
   
   return beta;
}

// --------------------------------------------------------------------------------------
Double_t TADItrackDiffusion::SigmaTheta(Double_t *x, Double_t *par)
{
   /// Function to modelize the scattering angle of incident ions at a depth x (Highland)
   /// (14.1MeV/p*beta)*Z*sqrt(x/Lr)*[1 +1/9log(x/Lr)]
   /// par[0]=beta of particule (beta)
   /// par[1]=momentum MeV/c (pc)
   /// par[2]=z of the beam (Z)
   /// par[3]=radiation length of the target material (Lr)
   /// x[0] in cm
   
   Double_t radL = x[0]/par[3];
   Double_t A    = 14.1*par[2]/(par[0]*par[1]);
   Double_t B    = TMath::Sqrt(radL)*(1+TMath::Log10(radL)/9.);
   
   return A*B*TMath::RadToDeg();
}

// --------------------------------------------------------------------------------------
Float_t TADItrackDiffusion::GetSigmaTheta(Float_t energy, TString mat, Float_t x, Float_t A, Float_t Z )
{
   Double_t pc   = GetPCC(energy, A);
   Double_t beta = GetBeta(energy);
   Double_t radL = GetRadLength(mat);  // Radiation length for material

   fFuncSigTheta->SetParameter(0, beta);
   fFuncSigTheta->SetParameter(1, pc);
   fFuncSigTheta->SetParameter(2, Z);
   fFuncSigTheta->SetParameter(3, radL);
   
   return  fFuncSigTheta->Eval(x);
}

// --------------------------------------------------------------------------------------
Float_t TADItrackDiffusion::GetRadLength(TString name)
{
   TString tmp(name);
   tmp.ToUpper();
   if (tmp == "H20" || tmp == "WATER")
      return fgkX0w;
      
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
