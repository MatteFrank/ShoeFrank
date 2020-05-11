#include "TRandom3.h"
#include "TF1.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TMath.h"

#include "TACAntuRaw.hxx"
#include "TAGroot.hxx"
#include "TACAparGeo.hxx"
#include "TACAdigitizer.hxx"

#include "GlobalPar.hxx"
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
   gRandom->SetSeed(0);
}

// --------------------------------------------------------------------------------------
void  TACAdigitizer::SetFunctions()
{
   // compute birks law for a given Edep
   fFuncBirks = new TF1("PhotonsN" , this, &TACAdigitizer::RecPhotonsN, 0, 20, 3, "TACAdigitizer", "RecPhotonsN");
   fDeResE    = new TF1("ResEnergy", this, &TACAdigitizer::ResEnergy, 50, 5000, 3, "TACAdigitizer", "ResEnergy");

}

// --------------------------------------------------------------------------------------
void  TACAdigitizer::SetParFunction()
{
   // photons yield (n/MeV), kB: birks (mm/MeV), collection efficiency (0.4%) for BGO
   fFuncBirks->SetParameters(9000, 3.679e-3, 1./240.); 
   fDeResE   ->SetParameters(fResPar0, fResPar1, fResPar2);
   // fDeResE   ->SetParameters(fResPar0, fResPar2);

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
   Double_t xx = x[0];
   Double_t photonsN = par[0]*xx/(1+par[1]*xx)*par[2];

   return photonsN;
}

//___________________________________________________________________________________________
Double_t TACAdigitizer::ResEnergy(Double_t* x, Double_t* par)
{
   Float_t energy = x[0];
   Float_t res = (par[0]*par[0]/energy + par[1]*par[1]/(energy*energy) + par[2]*par[2]);
   // Float_t res = (par[0]*par[0]/energy + par[1]*par[1]);

   // res /= 100;

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
  if (FootDebugLevel(1)) {
      printf("Energy before digitization: %f\n", edep);
   }

   //get number of photons
   Float_t photonsN = GetPhotonsN(x0, y0, edep)*fGain;

   //apply smearing to the energy
   edep *= 1000; //Convert MC energy from GeV to MeV

   Float_t resEnergy = GetResEnergy(edep);
   edep += gRandom->Gaus(0, resEnergy);

   if (FootDebugLevel(1)) {
      printf("Resolution: %.3f \n", resEnergy*100);
      printf("Energy after digitization: %.1f \n", edep);
   }

   edep /= 1000;  //Convert energy from MeV to GeV

   if (fMap[id] == 0) {
     fCurrentHit = (TACAntuHit*)fpNtuRaw->NewHit(id, photonsN, edep, time);
     fMap[id] = fCurrentHit;
   } else {
     fCurrentHit = fMap[id];
     //AS:: need to fix the energy as well
     
     //Add charge to current hit
     fCurrentHit->SetCharge(fCurrentHit->GetCharge()+photonsN);
   }
   
   return true;
}











