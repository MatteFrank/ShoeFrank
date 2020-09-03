
#include "TRandom3.h"
#include "TF1.h"
#include "TMath.h"

#include "TAGgeoTrafo.hxx"
#include "TACAntuRaw.hxx"
#include "TACAparGeo.hxx"
#include "TACAdigitizer.hxx"


Float_t TACAdigitizer::fgThreshold = 25; // MeV

// --------------------------------------------------------------------------------------
TACAdigitizer::TACAdigitizer(TACAntuRaw* pNtuRaw)
 : TAGbaseDigitizer(),
   fpNtuRaw(pNtuRaw),
   fGain(10/3.),
   fResPar0(1.21),
   fResErrPar0(7e-3),
   fResPar1(-1.37),
   fResErrPar1(0.58),
   fResPar2(5.07e-2),
   fResErrPar2(2.12e-4)
{
   SetFunctions();
   SetParFunction();
}

// --------------------------------------------------------------------------------------
void  TACAdigitizer::SetFunctions()
{
   // compute birks law for a given Edep
   fFuncBirks = new TF1("PhotonsN", this, &TACAdigitizer::RecPhotonsN, 0, 20, 3, "TACAdigitizer", "RecPhotonsN");
   
   fDeResE    = new TF1("ResEnergy", this, &TACAdigitizer::ResEnergy, 50, 5000, 3, "TACAdigitizer", "ResEnergy");
}

// --------------------------------------------------------------------------------------
void  TACAdigitizer::SetParFunction()
{
   // photons yield (n/MeV), kB: birks (mm/MeV), collection efficiency (0.4%) for BGO
   fFuncBirks->SetParameters(9000, 3.679e-3, 1./240.);
   fDeResE   ->SetParameters(fResPar0, fResPar1, fResPar2);
}

// --------------------------------------------------------------------------------------
TACAdigitizer::~TACAdigitizer()
{   
   delete fFuncBirks;
}

// --------------------------------------------------------------------------------------
Double_t TACAdigitizer::RecPhotonsN(Double_t* x, Double_t* par)
{
   Float_t xx = x[0];
   Float_t photonsN = par[0]*xx/(1+par[1]*xx)*par[2];

   return photonsN;
}

//___________________________________________________________________________________________
Double_t TACAdigitizer::ResEnergy(Double_t* x, Double_t* par)
{
   Float_t energy = x[0];
   Float_t res = (par[0]*par[0]/energy + par[1]*par[1]/(energy*energy) + par[2]*par[2]);
   
   return res;
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
Bool_t TACAdigitizer::Process(Double_t edep, Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/, Double_t time, Int_t id, Int_t /*Z*/)
{
   if (edep < fgThreshold) {
      fCurrentHit = 0x0;
      return true;
   }
   
   Float_t resEnergy = GetResEnergy(edep);
   edep += gRandom->Gaus(0, resEnergy);
   edep = 1.252*edep - 28.42; // rough calibration
   
   Float_t photonsN = edep; //GetPhotonsN(x0, y0, edep)*fGain; // skip Birks law for the moment

   if (fMap[id] == 0) {
      fCurrentHit = (TACAntuHit*)fpNtuRaw->NewHit(id, photonsN, time);
      fMap[id] = fCurrentHit;
   } else {
      fCurrentHit = fMap[id];
      
      //Add charge to current hit
      fCurrentHit->SetCharge(fCurrentHit->GetCharge()+photonsN);
   }
   
   return true;
}


