/*!
  \file TAVTbaseParCal.cxx
  \brief  Charge (raw) calibration for vertex.
*/

#include <Riostream.h>

#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"

#include "TAGrecoManager.hxx"
#include "TAVTparMap.hxx"
#include "TAVTbaseParCal.hxx"

//##############################################################################

/*!
  \class TAVTbaseParCal 
  \brief Charge (raw) calibration for vertex
*/

//! Class Imp
ClassImp(TAVTbaseParCal);

Bool_t TAVTbaseParCal::fgkLandauOn = false;

//------------------------------------------+-----------------------------------
//! Standard constructor
TAVTbaseParCal::TAVTbaseParCal()
: TAGparTools(),
  fChargeProba(new TArrayF(8)),
  fChargeProbaNorm(new TArrayF(8)),
  fChargeWithMaxProba(0),
  fChargeMaxProba(0.),
  fkDefaultCalName("")
{
   // set default wise to 1
   for (Int_t p = 0; p < 32; p++) {
      fEffParameter[p].SensorId   = p;
      for (Int_t k = 0; k < 4; k++) 
        fEffParameter[p].QuadEff[k] = 1.;
   }
}

//------------------------------------------+-----------------------------------
//! Destructor
TAVTbaseParCal::~TAVTbaseParCal()
{
   delete fChargeProba;
   delete fChargeProbaNorm;
   if (fgkLandauOn) {
      for (Int_t p = 0; p < fChargesN; p++) {
         delete fLandau[p];
         delete fLandauNorm[p];
      }
      delete fLandauTot;
   }
}

//------------------------------------------+-----------------------------------
//! Read from file
//!
//! \param[in] name file name
Bool_t TAVTbaseParCal::FromFile(const TString& name) 
{
   // Reading calibration file
   TString nameExp;
   
   if (name.IsNull())
     nameExp = fkDefaultCalName;
   else 
     nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   Info("FromFile()", "Open file %s for calibration\n", name.Data());

   ReadItem(fChargesN);
   if(FootDebugLevel(1))
      printf("ChargesN: %d\n", fChargesN);
   
   Double_t* tmp = new Double_t[4];
   for (Int_t p = 0; p < fChargesN; p++) { // Loop on each charge
     
	  // read parameters
	  ReadItem(tmp, 4, ' ');
	  fLandauParameter[p].Charge   = p+1;
	  fLandauParameter[p].Constant = tmp[0];
	  fLandauParameter[p].MPV      = tmp[1];
	  fLandauParameter[p].Sigma    = tmp[2];
	  fLandauParameter[p].Quench   = tmp[3];

	  if(FootDebugLevel(1))
        cout << endl << " Landau Parameter: "
		      << Form("%d %f %f %f %f", fLandauParameter[p].Charge, fLandauParameter[p].Constant, 
				 fLandauParameter[p].MPV, fLandauParameter[p].Sigma, fLandauParameter[p].Quench) << endl;
   }
   delete[] tmp;

   if (!Eof()) {
      ReadItem(fSensorsN);
      if(FootDebugLevel(1))
         printf("SensorsN: %d\n", fSensorsN);
      
      Double_t* tmp = new Double_t[4];
      for (Int_t p = 0; p < fSensorsN; p++) { // Loop on each charge
         
         // read parameters
         ReadItem(tmp, 4, ' ');
         
         fEffParameter[p].SensorId = p;
         fEffParameter[p].QuadEff[0] = tmp[0];
         fEffParameter[p].QuadEff[1] = tmp[1];
         fEffParameter[p].QuadEff[2] = tmp[2];
         fEffParameter[p].QuadEff[3] = tmp[3];
         
         if(FootDebugLevel(1))
            cout << endl << " Efficieny Parameter: "
            << Form("%d %f %f %f %f", fEffParameter[p].SensorId, fEffParameter[p].QuadEff[0],
                    fEffParameter[p].QuadEff[1], fEffParameter[p].QuadEff[2], fEffParameter[p].QuadEff[3]) << endl;
      }
      delete[] tmp;
   }
   
   if (fgkLandauOn)
      SetFunction();
   
   return true;
}

//------------------------------------------+-----------------------------------
//! Get charge probability for a given number of pixels in cluster
//!
//! \param[in] pixelsN number of pixels
const TArrayF* TAVTbaseParCal::GetChargeProba(Float_t pixelsN)
{
   Float_t value[fChargesN];
   Float_t fac = 0.;
   Float_t maxProba = 0.;
   
   for (Int_t p = 0; p < fChargesN; p++) { // Loop on each charge
	  value[p] = fLandau[p]->Eval(pixelsN);
	  fac += value[p];
   }
   
   for (Int_t p = 0; p < fChargesN; p++) { // Loop on each charge
	  Float_t ratio = value[p]/fac*100;
	  fChargeProba->AddAt(ratio, p);
	  if (ratio > maxProba) {
		 maxProba = ratio;
		 fChargeWithMaxProba = p+1;
		 fChargeMaxProba = ratio;
	  }
   }
   
   return fChargeProba;
}

//------------------------------------------+-----------------------------------
//! Get charge normalized probability (integral = 1) for a given number of pixels in cluster
//!
//! \param[in] pixelsN number of pixels
const TArrayF* TAVTbaseParCal::GetChargeProbaNorm(Float_t pixelsN)
{
   Float_t value[fChargesN];
   Float_t fac = 0.;
   Float_t maxProba = 0.;
   
   for (Int_t p = 0; p < fChargesN; p++) { // Loop on each charge
	  value[p] = fLandauNorm[p]->Eval(pixelsN);
      fac += value[p];
   }
   
   for (Int_t p = 0; p < fChargesN; p++) { // Loop on each charge
	  Float_t ratio = value[p]/fac*100;
	  fChargeProbaNorm->AddAt(ratio, p);
	  if (ratio > maxProba) {
		 maxProba = ratio;
		 fChargeWithMaxProbaNorm = p+1;
		 fChargeMaxProbaNorm = ratio;
	  }
   }
   
   return fChargeProbaNorm;
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.
void TAVTbaseParCal::Clear(Option_t*)
{
  return;
}

//------------------------------------------+-----------------------------------
//! ostream insertion.
//!
//! \param[out] os stream output
//! \param[in] option option for printout
void TAVTbaseParCal::ToStream(ostream& os, Option_t*) const
{
//  os << "TAVTbaseParCal " << GetName() << endl;
//  os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
//     << "   ver_x   ver_y   ver_z  width" << endl;

  return;
}

//------------------------------------------+-----------------------------------
//! Set up quenched Landau parameters
void TAVTbaseParCal::SetFunction()
{
   for (Int_t p = 0; p < fChargesN; p++) { // Loop on each charge
	  fLandau[p] = new TF1(Form("landau%d", p), this, &TAVTbaseParCal::QLandau, 0, 22, 4, "TAVTbaseParCal", "QLandau");
	  fLandau[p]->SetParameters(fLandauParameter[p].Constant, fLandauParameter[p].MPV, fLandauParameter[p].Sigma, 
								fLandauParameter[p].Quench);
	  fLandau[p]->SetParNames("Constant", "MPV", "Sigma", "Quench");
   }
   
   Float_t integral0 = 0.;
   Float_t integral1 = 0.;
   for (Int_t p = 0; p < fChargesN; p++) { // Loop on each charge
      fLandauNorm[p] = new TF1(Form("landauNorm%d", p), this, &TAVTbaseParCal::QLandauNorm, 0, 22, 4, "TAVTbaseParCal", "QLandauNorm");
      fLandauNorm[p]->SetParameters(1, fLandauParameter[p].MPV, fLandauParameter[p].Sigma, fLandauParameter[p].Quench);
      fLandauNorm[p]->SetParNames("Constant", "MPV", "Sigma", "Quench");

      integral0 = fLandauNorm[0]->Integral(0,22);
      integral1 = fLandauNorm[p]->Integral(0,22);
      
      Float_t ratio = integral0/integral1;
      fLandauNorm[p]->SetParameter(0, ratio);
   }
   
   fLandauTot = new TF1("landauTot", this, &TAVTbaseParCal::QLandauTot, 0, 22, 0, "TAVTbaseParCal", "QLandauTot");
}

//------------------------------------------+-----------------------------------
//! Quenched Landau
//!
//! \param[in] x charge
//! \param[in] par parameters vector
Double_t TAVTbaseParCal::QLandau(Double_t* x, Double_t* par)
{
   Float_t xx = (x[0]-par[1])/par[2];
   Double_t f = 1./TMath::Sqrt(2*TMath::Pi())*par[0]*TMath::Exp(-0.5*(xx + par[3]*TMath::Exp(-xx)));
   return f;   
}

//------------------------------------------+-----------------------------------
//! Quenched normalized Landau
//!
//! \param[in] x charge
//! \param[in] par parameters vector
Double_t TAVTbaseParCal::QLandauNorm(Double_t* x, Double_t* par)
{
   Float_t xx = (x[0]-par[1])/par[2];
   Double_t f = 1./TMath::Sqrt(2*TMath::Pi())*par[0]*TMath::Exp(-0.5*(xx + par[3]*TMath::Exp(-xx)));
   return f;
}

//------------------------------------------+-----------------------------------
//! Total quenched Landau
//!
//! \param[in] x charge
//! \param[in] par parameters vector
Double_t TAVTbaseParCal::QLandauTot(Double_t* x, Double_t* /*par*/)
{
   Float_t xx = x[0];
   Double_t f = 0;
   
   for (Int_t i = 0; i < fChargesN-1; ++i) {
	  f += fLandau[i]->Eval(xx);
   }
   
   return f;   
}


