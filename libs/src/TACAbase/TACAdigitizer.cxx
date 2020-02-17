
#include "TRandom3.h"
#include "TF1.h"
#include "TMath.h"

#include "TACAntuRaw.hxx"
#include "TACAparGeo.hxx"
#include "TACAdigitizer.hxx"


// --------------------------------------------------------------------------------------
TACAdigitizer::TACAdigitizer(TACAntuRaw* pNtuRaw)
 : TAGbaseDigitizer(),
   fpNtuRaw(pNtuRaw),
   fGain(10/3.)
{
   SetFunctions();
   SetParFunction();
}

// --------------------------------------------------------------------------------------
void  TACAdigitizer::SetFunctions()
{
   // compute birks law for a given Edep
   fFuncBirks = new TF1("PhotonsN", this, &TACAdigitizer::RecPhotonsN, 0, 20, 3, "TACAdigitizer", "RecPhotonsN");
}

// --------------------------------------------------------------------------------------
void  TACAdigitizer::SetParFunction()
{
   // photons yield (n/MeV), kB: birks (mm/MeV), collection efficiency (0.4%) for BGO
   fFuncBirks->SetParameters(9000, 3.679e-3, 1./240.); 
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
Float_t TACAdigitizer::GetPhotonsN(Float_t /*X*/, Float_t /*Y*/, Float_t edep)
{
   return fFuncBirks->Eval(edep);
}

//___________________________________________________________________________________________
Bool_t TACAdigitizer::Process(Double_t edep, Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/, Double_t time, Int_t id, Int_t /*Z*/)
{
   Float_t photonsN = GetPhotonsN(x0, y0, edep)*fGain;
   
   if (fMap[id] == 0) {
      fCurrentHit = (TACAntuHit*)fpNtuRaw->NewHit(id, photonsN, time);
      fMap[id] = fCurrentHit;
   } else
      fCurrentHit = fMap[id];

   return true;
}


