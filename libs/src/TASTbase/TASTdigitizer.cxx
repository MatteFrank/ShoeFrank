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

#include "TASTparGeo.hxx"
#include "TASTdigitizer.hxx"

// Compute the number of photons for 5 mm slice width

// --------------------------------------------------------------------------------------
TASTdigitizer::TASTdigitizer(TASTntuRaw* pNtuRaw)
 : TAGbaseDigitizer(),
   fpNtuRaw(pNtuRaw),
   fGain(5.e-4),
   fResTime(80) // in ps second, to be checked
{
   SetFunctions();
   SetParFunction();
}

// --------------------------------------------------------------------------------------
void  TASTdigitizer::SetFunctions()
{
   // compute birks law for a given Edep
   fFuncBirks = new TF1("PhotonsTR", this, &TASTdigitizer::RecPhotonsN, 0, 20, 3, "TASTdigitizer", "RecPhotonsN");
}

// --------------------------------------------------------------------------------------
void  TASTdigitizer::SetParFunction()
{
   //all in mm
   // !!! WRONG number for Birks coefficient  and light collection efficiency put 0.9!!!
   fFuncBirks->SetParameters(10400, 0.0126, 0.9); // photons yield (Nphoton/MeV), kB: birks (mm/MeV), collection efficiency (0.4%) for EJ-202
}

// --------------------------------------------------------------------------------------
TASTdigitizer::~TASTdigitizer()
{   
   delete fFuncBirks;
}

// --------------------------------------------------------------------------------------
Double_t TASTdigitizer::RecPhotonsN(Double_t* x, Double_t* par)
{
   Float_t xx = x[0];
   Float_t photonsN = par[0]*xx/(1+par[1]*xx)*par[2];

   return photonsN;
}

//___________________________________________________________________________________________
Float_t TASTdigitizer::GetPhotonsN(Float_t /*X*/, Float_t /*Y*/, Float_t edep)
{
   return fFuncBirks->Eval(edep);
}

//___________________________________________________________________________________________
Bool_t TASTdigitizer::Process(Double_t edep, Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/, Double_t time, Int_t id, Int_t /*Z*/)
{
   time += gRandom->Gaus(0, fResTime);
   Float_t charge = GetPhotonsN(x0, y0, edep)*fGain;
   //time should be stored in the hit in ns:
   time /= 1000.;
   
   if (fMap[id] == 0) {
      fCurrentHit = (TASTntuHit*)fpNtuRaw->NewHit(id, charge, time);
      fMap[id] = fCurrentHit;
   } else {
      fCurrentHit = fMap[id];
      
      //Add charge to current hit
      fCurrentHit->SetCharge(fCurrentHit->GetCharge()+charge);
      
      // take the shortest time
      if (time < fCurrentHit->GetTime())
         fCurrentHit->SetTime(time);
   }
   
   return true;
}


