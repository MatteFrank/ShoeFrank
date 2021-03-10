
#include "TRandom3.h"
#include "TF1.h"
#include "TMath.h"

#include "TAGgeoTrafo.hxx"
#include "TACAntuHit.hxx"
#include "TACAparGeo.hxx"
#include "TACAdigitizer.hxx"


Float_t TACAdigitizer::fgThreshold = 80; // MeV 

// --------------------------------------------------------------------------------------
TACAdigitizer::TACAdigitizer(TACAntuHit* pNtuRaw)
 : TAGbaseDigitizer(),
   fpNtuRaw(pNtuRaw),
   fGain(1e-4),
   fResPar0(1.03978e+01),
   fResErrPar0(3.28955e-01),
   fResPar1(1.03392e+02),
   fResErrPar1(4.27932e+00),
   fResPar2(3.90060e-01),
   fResErrPar2(4.94583e-03),
   fBirkPar0(9000),
   fBirkPar1(3.679e-3),
   fCalEPar0(-28.42),
   fCalEPar1(1.252)
{
   SetFunctions();
   SetParFunction();
}

// --------------------------------------------------------------------------------------
void  TACAdigitizer::SetFunctions()
{
   // compute birks law for a given Edep
   fFuncBirks = new TF1("PhotonsN", this, &TACAdigitizer::RecPhotonsN, 0, 20, 2, "TACAdigitizer", "RecPhotonsN");
   
   // compute energy resolution
   fDeResE    = new TF1("ResEnergy", this, &TACAdigitizer::ResEnergy, 0, 5000, 3, "TACAdigitizer", "ResEnergy");
}

// --------------------------------------------------------------------------------------
void  TACAdigitizer::SetParFunction()
{
   // photons yield (n/MeV), kB: Birk (mm/MeV)
   fFuncBirks->SetParameters(fBirkPar0, fBirkPar1);
   
   // Resolution parameter
   fDeResE->SetParameters(fResPar0, fResPar1, fResPar2);
}

// --------------------------------------------------------------------------------------
TACAdigitizer::~TACAdigitizer()
{   
   delete fFuncBirks;
   delete fDeResE;
}

// --------------------------------------------------------------------------------------
Double_t TACAdigitizer::RecPhotonsN(Double_t* x, Double_t* par)
{
   Float_t xx = x[0];
   Float_t photonsN = par[0]*xx/(1+par[1]*xx);

   return photonsN;
}

//___________________________________________________________________________________________
Double_t TACAdigitizer::ResEnergy(Double_t* x, Double_t* par)
{
   Float_t energy = x[0];
   Float_t res = TMath::Sqrt(par[0]*par[0]/energy + par[1]*par[1]/(energy*energy) + par[2]*par[2]);
   
   return res/100.;
}
//___________________________________________________________________________________________
Float_t TACAdigitizer::GetPhotonsN(Float_t /*X*/, Float_t /*Y*/, Float_t edep)
{
   return fFuncBirks->Eval(edep);
}

//___________________________________________________________________________________________
Float_t TACAdigitizer::GetResEnergy(Float_t edep)
{
   return fDeResE->Eval(edep)*edep;
}

//___________________________________________________________________________________________
Bool_t TACAdigitizer::Process(Double_t edep, Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/, Double_t time, Int_t id, Int_t /*Z*/, Double_t /*px0*/, Double_t /*py0*/, Double_t /*pz0*/)
{
   if (edep < fgThreshold) {
      fCurrentHit = 0x0;
      return true;
   }
   

   // Float_t resEnergy = 0.02*edep;
   Float_t resEnergy = GetResEnergy(edep);
   Float_t s = gRandom->Gaus(0, resEnergy);
   edep+=s;
   if ((edep<0.) || (TMath::Abs(edep - edep+s)>7.)) edep = 0.;

   // edep  = fCalEPar1*edep + fCalEPar0; // rough calibration
   
  // Float_t photonsN = GetPhotonsN(x0, y0, edep)*fGain; // skip Birks law should be included in resolution

   if (fMap[id] == 0) { //if in the Map of Hits the element id-esimo was empty fill with a NewHit(...)
      fCurrentHit = (TACAhit*)fpNtuRaw->NewHit(id, edep, time);
      fMap[id] = fCurrentHit;
   } else {  //if the element "fMap[id]" was not empty then assign to the fCurrentHit the previous Hit, but sum the two energies
      fCurrentHit = fMap[id];
      
      //Add charge to current hit
      fCurrentHit->SetCharge(fCurrentHit->GetCharge()+edep);
   }
   
   return true;
}


