
/*!
 \file TADItrackEmProperties.cxx
 \brief  Implemention of TADItrackEmProperties.cxx
 */

#include "Riostream.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TAGionisMaterials.hxx"

#include "TADItrackEmProperties.hxx"
#include "TAGgeoTrafo.hxx"

//##############################################################################

/*!
 \class TADItrackEmProperties 
 \brief  Functions for EM track properties **
 */

//! Class Imp
ClassImp(TADItrackEmProperties);

Float_t TADItrackEmProperties::fgkX0w    = 36.08;
Float_t TADItrackEmProperties::fgkElossK = 0.307075;  // MeV cm2/g for A = 1 g/mol, in  [MeV].

//_____________________________________________________________________________
//! Default constructor
TADItrackEmProperties::TADItrackEmProperties()
: TAGpara()
{
   if (gGeoManager == 0x0)
      Error("TADItrackEmProperties()", "gGeoManager not defined");
   
   fFuncSigTheta = new TF1("funcSigTheta", this, &TADItrackEmProperties::SigmaTheta, 0, 500, 4, "TADItrackEmProperties", "SigmaTheta");
}

//_____________________________________________________________________________
//! Destructor
TADItrackEmProperties::~TADItrackEmProperties()
{
   delete fFuncSigTheta;
}

//_____________________________________________________________________________
//! Calculation of the WEPL of the material layer
//!
//! \param[in] mat material name
Float_t TADItrackEmProperties::GetFacWEPL(const TString& mat)
{
   Float_t factor = 0;
  
   TString material(mat);
   
   material.ToUpper();
   
   if (material == "AIR"){
      factor = 0.001045298;
   } else if (material == "SI"){
      factor = 1.833962703;
   } else if (material == "C"){
      factor = 1.991445766;
   } else if (material == "PMMA"){
      factor = 1.1635;
   } else if (material.Contains("EJ")){
      factor = 1.0259;
   } else if (material == "H2O" || material == "WATER"){
      factor = 1.;
   } else if (material == "TI"){
      factor = 3.136600294;
   } else Warning("GetWEPL()","Material is not in the list.... Candidates: Air, Si, C, PMMA, H2O, Ti");
   

   return factor;
}

//_____________________________________________________________________________
//! Calculation of the energy loss in the material layer (WEPL in [cm])
//!
//! \param[in] energy beam energy (MeV)
//! \param[in] massNumber mass number of beam
//! \param[in] atomicNumber atomic number of beam
//! \param[in] WEPL Water equivalent path length of target
Float_t TADItrackEmProperties::GetEnergyLoss(Float_t energy, Float_t massNumber, Int_t atomicNumber, Float_t WEPL)
{
   // rage formula Bortfeld et al, PMB 41 (1996)
   // R = alpha*Energy^(pFactor)
   energy *= TAGgeoTrafo::GevToMev();
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
   
   return dE*TAGgeoTrafo::MevToGev();
}

//_____________________________________________________________________________
//! Calculation of the energy loss in the material layer (thickness in [cm])
//!
//! \param[in] mat material name of target
//! \param[in] thickness thickness of target
//! \param[in] energy beam energy
//! \param[in] massNumber mass number of beam
//! \param[in] atomicNumber atomic number of beam
Float_t TADItrackEmProperties::GetEnergyLoss(const TString& mat, Float_t thickness, Float_t energy, Float_t massNumber, Int_t atomicNumber)
{
   // rage formula Bortfeld et al, PMB 41 (1996)
   // R = alpha*Energy^(pFactor)
   Float_t WEPL = GetFacWEPL(mat)*thickness;
   
   return GetEnergyLoss(energy, massNumber, atomicNumber, WEPL);
}

//_____________________________________________________________________________
//! Calculation of the energy loss in the material layer (thickness in [cm])
//!
//! \param[in] mat material name of target
//! \param[in] energy beam energy (MeV)
//! \param[in] Abeam mass number of beam
//! \param[in] Zbeam atomic number of beam
Float_t TADItrackEmProperties::GetRange(const TString& mat, Float_t energy, Float_t Abeam, Int_t Zbeam)
{
  // rage formula Bortfeld et al, PMB 41 (1996)
  // R = alpha*Energy^(pFactor)
  Float_t alpha   = 0.0022;
  Float_t pfactor = 1.;
  Float_t facWEPL = GetFacWEPL(mat);

  
  if (energy < 250){
    pfactor = 1.77;
  } else if ((energy >= 250) && (energy < 400)){
    pfactor = 1.76;
  } else {
    pfactor = 1.75;
  }
  
  Float_t rangeW = (Abeam / (Zbeam*Zbeam) * alpha * TMath::Power(energy, pfactor));

  
  return rangeW*facWEPL;
}

//_____________________________________________________________________________
//! Calculation of the energy loss per range with Bethe-Bloch [MeV.cm^2/g]
//!
//! \param[in] mat material name of target
//! \param[in] beta beta of beam
//! \param[in] zBeam atomic number of beam
Float_t TADItrackEmProperties::GetdEdX(const TString& mat, Double_t beta,  Double_t zBeam)
{
   // Inspired from Alessio's ChargeBetheBloch formula
   Double_t K       = fgkElossK;
   Double_t rho     = GetDensity(mat);     // mean density g/cm3

   Double_t Zmed    = GetZ(mat); // raw number not effective A
   Double_t Amed    = GetA(mat); // raw number not effective Z
   
   Double_t I       = GetMeanExcitationEnergy(mat);

   Double_t Q       = K*zBeam*zBeam*Zmed/Amed;      //Kz^2xZ/A
   Double_t me      = TAGgeoTrafo::GetElectronMassMeV();
   
   // calculating the gamma and Tmax
   Double_t massFac = TAGgeoTrafo::GetMassFactorMeV();
   Double_t mass    = Amed*massFac;

   Double_t beta2   = beta*beta;
   Double_t gamma   = TMath::Sqrt(1/(1-beta2));
   Double_t gamma2  = gamma*gamma;
   Double_t Tmax    = 2*gamma2*beta2*me/(1+(2*gamma*(me/mass))+me*me/(mass*mass));
   Double_t logar   = 0.5*TMath::Log(2*me*beta2*gamma2*Tmax/(I*I)) - beta2;
   Double_t dEdX    = Q/beta2*logar;
   
   return dEdX;
}

//_____________________________________________________________________________
//! Calculation of the impact*c [MeV]
//!
//! \param[in] energy energy of beam (MeV)
//! \param[in] massNumber mass number of beam
Float_t TADItrackEmProperties::GetPCC(Float_t energy, Float_t massNumber)
{
   Float_t massFac = TAGgeoTrafo::GetMassFactorMeV();
   Float_t pc      = TMath::Sqrt(energy * energy + 2*energy * massFac) * massNumber ;
   
   return pc;
}

//_____________________________________________________________________________
//! Calculation of beta
//!
//! \param[in] energy energy of beam (MeV)
Float_t TADItrackEmProperties::GetBeta(Float_t energy)
{
   Float_t massFac = TAGgeoTrafo::GetMassFactorMeV();
   Float_t gamma   = 1.0 + energy/massFac;
   Float_t beta    = TMath::Sqrt(1.0 - 1/(gamma*gamma));
  
   return beta;
}

// --------------------------------------------------------------------------------------
//! Function to modelize the scattering angle of incident ions at a depth x (Highland)
//! (14.1MeV/p*beta)*Z*sqrt(x/Lr)*[1 +1/9log(x/Lr)]
//!
//! \param[in] x position vector
//! \param[in] par parameters vector
//! \return angle in rad
//!
//! par[0]=beta of particule (beta)
//! par[1]=momentum MeV/c (pc)
//! par[2]=z of the beam (Z)
//! par[3]=radiation length of the target material (Lr)
//! x[0] in cm
Double_t TADItrackEmProperties::SigmaTheta(Double_t *x, Double_t *par)
{
   Double_t radL = x[0]/par[3];
   Double_t A    = 14.1*par[2]/(par[0]*par[1]);
   Double_t B    = TMath::Sqrt(radL)*(1+TMath::Log10(radL)/9.);
   
   return A*B;
}

//_____________________________________________________________________________
//! Get the scattering angle of incident ions at a depth x (Highland)
//!
//! \param[in] matTarget material name of target
//! \param[in] x thickness of target
//! \param[in] energy energy of beam
//! \param[in] bA mass number of beam
//! \param[in] bZ atomic number of beam
Float_t TADItrackEmProperties::GetSigmaTheta(const TString& matTarget, Float_t x, Float_t energy, Float_t bA, Float_t bZ)
{
   energy       *= TAGgeoTrafo::GevToMev();
   Double_t pc   = GetPCC(energy, bA);
   Double_t beta = GetBeta(energy);
   Double_t radL = GetRadLength(matTarget);  // Radiation length for material

   fFuncSigTheta->SetParameter(0, beta);
   fFuncSigTheta->SetParameter(1, pc);
   fFuncSigTheta->SetParameter(2, bZ);
   fFuncSigTheta->SetParameter(3, radL);
   
   return  fFuncSigTheta->Eval(x);
}

//_____________________________________________________________________________
//! Get the scattering angle of incident ions at a depth x (Highland)
//!
//! \param[in] matTarget material name of target
//! \param[in] x thickness of target
//! \param[in] energy energy of beam
//! \param[in] beam beam materrial name
Float_t TADItrackEmProperties::GetSigmaTheta(const TString& matTarget, Float_t x, Float_t energy, const TString& beam)
{
   Double_t bA   = GetA(beam);
   Double_t bZ   = GetZ(beam);
  
  return GetSigmaTheta(matTarget, bA, bZ, x, energy);
}

//_____________________________________________________________________________
//! Get radiation length for a given material
//!
//! \param[in] name material name
Float_t TADItrackEmProperties::GetRadLength(TString name)
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

//_____________________________________________________________________________
//! Get density for a given material
//!
//! \param[in] name material name
Float_t TADItrackEmProperties::GetDensity(TString name)
{
   TGeoMaterial* mat = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject(name.Data());
   if (mat == 0x0) {
      Warning("GetDensity()", "Unknown material %s", name.Data());
      return -1;
   } else
      return mat->GetDensity();
}
                  
//_____________________________________________________________________________
//! Get mass number for a given material
//!
//! \param[in] name material name
//! \param[in] eff flag for effective mass number (average of A isotopes)
Float_t TADItrackEmProperties::GetA(TString name, Bool_t eff)
{
   TGeoMaterial* mat = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject(name.Data());
   if (mat == 0x0) {
      Warning("GetA()", "Unknown material %s", name.Data());
      return -1;
   } else {
      if (mat->IsMixture() && !eff)
         return GetA((TGeoMixture*)mat);
      else
         return mat->GetA();
   }
}

//_____________________________________________________________________________
//! Get atomic number for a given material
//!
//! \param[in] name material name
//! \param[in] eff flag for effective atomic number (average of Z isotopes)
Float_t TADItrackEmProperties::GetZ(TString name, Bool_t eff)
{
   TGeoMaterial* mat = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject(name.Data());
   if (mat == 0x0) {
      Warning("GetZ()", "Unknown material %s", name.Data());
      return -1;
   } else {
      if (mat->IsMixture() && !eff)
         return GetZ((TGeoMixture*)mat);
      else
         return mat->GetZ();
   }
}

//_____________________________________________________________________________
//! Get mass number for a given mixture
//!
//! \param[in] mix a given mixture
Float_t TADItrackEmProperties::GetA(TGeoMixture* mix)
{
   if (mix == 0x0) {
      Warning("GetA()", "Unknown material");
      return -1;
   } else {
      Float_t A = 0;
      for(Int_t i = 0; i < mix->GetNelements(); ++i)
         A += mix->GetAmixt()[i]*mix->GetNmixt()[i];
      return A;
   }
}

//_____________________________________________________________________________
//! Get atomic number for a given mixture
//!
//! \param[in] mix a given mixture
Float_t TADItrackEmProperties::GetZ(TGeoMixture* mix)
{
   if (mix == 0x0) {
      Warning("GetZ()", "Unknown material");
      return -1;
   } else {
      Float_t Z = 0;
      for(Int_t i = 0; i < mix->GetNelements(); ++i)
         Z += mix->GetZmixt()[i]*mix->GetNmixt()[i];
      return Z;
   }
}

//_____________________________________________________________________________
//! Get mean excitation energy for a given material
//!
//! \param[in] name material name
Float_t TADItrackEmProperties::GetMeanExcitationEnergy(TString name)
{
   TGeoMaterial* mat = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject(name.Data());
   if (mat == 0x0) {
      Warning("GetDensity()", "Unknown material %s", name.Data());
      return -1;
   } else
      return  mat->GetConstProperty(TAGionisMaterials::GetMeanExcitationEnergyName());
}
