/*!
 \file TACAdigitizer.cxx
 \brief   Implementation of TACAdigitizer.
 */

#include "TRandom3.h"
#include "TF1.h"
#include "TMath.h"

#include "TAGgeoTrafo.hxx"
#include "TACAntuHit.hxx"
#include "TACAparGeo.hxx"
#include "TACAdigitizer.hxx"

/*!
 \class TACAdigitizer
 \brief digitizer for calorimeter hits **
 */

Float_t TACAdigitizer::fgThreshold = 80; // MeV

// --------------------------------------------------------------------------------------
//! Constructor
TACAdigitizer::TACAdigitizer(TACAntuHit* pNtuRaw)
 : TAGbaseDigitizer(),
   fpNtuRaw(pNtuRaw),
   fResPar0(1.03978e+01),
   fResErrPar0(3.28955e-01),
   fResPar1(1.03392e+02),
   fResErrPar1(4.27932e+00),
   fResPar2(3.90060e-01),
   fResErrPar2(4.94583e-03)
{
   SetFunctions();
   SetParFunction();
}

// --------------------------------------------------------------------------------------
//! Set functions
void  TACAdigitizer::SetFunctions()
{
   // compute energy resolution
   fDeResE = new TF1("ResEnergy", this, &TACAdigitizer::ResEnergy, 0, 5000, 3, "TACAdigitizer", "ResEnergy");
}

// --------------------------------------------------------------------------------------
//! Set parameter function
void  TACAdigitizer::SetParFunction()
{
   // Resolution parameter
   fDeResE->SetParameters(fResPar0, fResPar1, fResPar2);
}

// --------------------------------------------------------------------------------------
//! Destructor
TACAdigitizer::~TACAdigitizer()
{
   delete fDeResE;
}

// --------------------------------------------------------------------------------------
//! Resolution energy function
//!
//! \param[in] x position vector
//! \param[in] par parameters vector
Double_t TACAdigitizer::ResEnergy(Double_t* x, Double_t* par)
{
   Float_t energy = x[0];
   Float_t res = TMath::Sqrt(par[0]*par[0]/energy + par[1]*par[1]/(energy*energy) + par[2]*par[2]);

   return res/100.;
}

//___________________________________________________________________________________________
//! Get resolution energy
//!
//! \param[in] edep energy loss
Float_t TACAdigitizer::GetResEnergy(Float_t edep)
{
   return fDeResE->Eval(edep)*edep;
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
//! \param[in] id crystal id
//! \param[in] Z atomic charge of the particle
//! \param[in] px0 momentum in X direction
//! \param[in] py0 momentum in Y direction
//! \param[in] pz0 momentum in z direction
Bool_t TACAdigitizer::Process(Double_t edep, Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/, Double_t time, Int_t id, Int_t /*Z*/, Double_t /*px0*/, Double_t /*py0*/, Double_t /*pz0*/)
{

 //  if ((edep<0.) || (TMath::Abs(edep - edep+s)>7.)) edep = 0.; // what for ?
   if (edep<0.) edep = 0.;

   if (fMap[id] == 0) { //if in the Map of Hits the element id-esimo was empty fill with a NewHit(...)
      fCurrentHit = fpNtuRaw->NewHit(id, edep, time);
      fMap[id] = fCurrentHit;
   } else {  //if the element "fMap[id]" was not empty then assign to the fCurrentHit the previous Hit, but sum the two energies
      fCurrentHit = fMap[id];

      //Add charge to current hit
      fCurrentHit->SetCharge(fCurrentHit->GetCharge()+edep);
   }

   return true;
}

//___________________________________________________________________________________________
//! Smearing energy
//!
//! \param[in] parCal calibration parameter
void TACAdigitizer::SmearEnergy(TACAparCal* parCal)
{
  for (auto const& element : fMap) {
    Double_t charge = element.second->GetCharge();
    Double_t thres = parCal->GetCrystalThres(element.second->GetCrystalId());
    Double_t newcharge = gRandom->Gaus(charge, GetResEnergy(charge));
    if (newcharge < thres){
      element.second->SetValid(false); // to be check if it can work or not...
      fpNtuRaw->AddNDrop();
    }else{
      element.second->SetCharge(newcharge);
      element.second->SetValid(true);
    }
  }

  return;
}
