//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

#include "TRandom3.h"
#include "TF1.h"
#include "TMath.h"

#include "TAPLparGeo.hxx"
#include "TAPLdigitizer.hxx"

// Compute the number of photons for 5 mm slice width

// --------------------------------------------------------------------------------------
TAPLdigitizer::TAPLdigitizer(TAPLntuHit* pNtuRaw)
 : TAGobject(),
   fpNtuRaw(pNtuRaw),
   fGain(5.e4),
   fEnergyResCst(-0.02455),
   fEnergyResSqrt(0.2322),
   fEnergyResSqrt2(-0.4943),
   fResTime(0.200) // in ns second, to be checked
{
   SetFunctions();
   SetParFunction();
}

// --------------------------------------------------------------------------------------
void  TAPLdigitizer::SetFunctions()
{
   // compute birks law for a given Edep
   fFuncEnergyRes = new TF1("PhotonsTR", this, &TAPLdigitizer::RecEnergyRes, 0, 20, 3, "TAPLdigitizer", "RecEnergyRes");
}

// --------------------------------------------------------------------------------------
void  TAPLdigitizer::SetParFunction()
{
   //all in mm
   fFuncEnergyRes->SetParameters(fEnergyResCst, fEnergyResSqrt);
}

// --------------------------------------------------------------------------------------
TAPLdigitizer::~TAPLdigitizer()
{   
   delete fFuncEnergyRes;
}

// --------------------------------------------------------------------------------------
Double_t TAPLdigitizer::RecEnergyRes(Double_t* x, Double_t* par)
{
   Float_t en  = x[0];
   Float_t res = par[0] + par[1]*TMath::Sqrt(en+par[2]*en*en);
   
   return res;
}

//___________________________________________________________________________________________
Float_t TAPLdigitizer::GetEnergyRes(Float_t /*X*/, Float_t /*Y*/, Float_t edep)
{
   return fFuncEnergyRes->Eval(edep);
}

//___________________________________________________________________________________________
Bool_t TAPLdigitizer::Process(Double_t edep, Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/, Double_t time, Int_t id, Int_t /*Z*/, Double_t /*px0*/, Double_t /*py0*/, Double_t /*pz0*/)
{
   time += gRandom->Gaus(0, fResTime);
   Float_t fac = 2*TMath::Sqrt(2*TMath::Log(2));
   Float_t resEnergy = GetEnergyRes(x0, y0, edep)/fac;
   Float_t charge    = gRandom->Gaus(edep, resEnergy);
   
   //time should be stored in the hit in ns:
   //time /= 1000.;
   
   if (fMap[id] == 0) {
     fCurrentHit = (TAPLhit*)fpNtuRaw->NewHit();
      fCurrentHit->SetCharge(charge);
      fCurrentHit->SetTimeLE(time);
      fMap[id] = fCurrentHit;
   } else {
      fCurrentHit = fMap[id];
      
      //Add charge to current hit
      fCurrentHit->SetCharge(fCurrentHit->GetCharge()+charge);
      
      // take the shortest time
      if (time < fCurrentHit->GetTimeLE())
         fCurrentHit->SetTimeLE(time);
   }
   
   return true;
}


