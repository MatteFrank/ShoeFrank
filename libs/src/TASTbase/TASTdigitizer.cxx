/*!
 \file TASTdigitizer.cxx
 \brief   Implementation of TASTdigitizer.
 */


#include "TRandom3.h"
#include "TF1.h"
#include "TMath.h"

#include "TASTdigitizer.hxx"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TASTdigitizer
 --> 21/02/2023 parameters changed by mtoppi using updated Start Counter time resolution provided by G.Traini in https://agenda.infn.it/event/33527/contributions/186569/attachments/100236/139440/FOOTgeneralPG.pdf
 */

//! Class Imp

// --------------------------------------------------------------------------------------
TASTdigitizer::TASTdigitizer(TASTntuHit* pNtuRaw)
 : TAGbaseDigitizer(),
   fpNtuRaw(pNtuRaw),
   fGain(5.e-4),
   fTimeST_A(4.e-2),
   fTimeST_p0(387.7),
   fTimeSTErr_p0(2.6),
   fTimeST_p1(6.5e-3),
   fTimeSTErr_p1(4e-4),
   fEnergyThreshold(0.01),  // Set minimum SC threshold to 0.01 MeV
   fEnergyMax(-1)           // max energy loss in the case of multiple hits
{
   SetFunctions();
   SetInitParFunction();
}

// --------------------------------------------------------------------------------------
void  TASTdigitizer::SetFunctions()
{
   // compute birks law for a given Edep
   fFuncBirks = new TF1("PhotonsTR", this, &TASTdigitizer::RecPhotonsN, 0, 20, 3, "TASTdigitizer", "RecPhotonsN");
   fTimeSTResE  = new TF1("ResTimeST", this, &TASTdigitizer::ResTimeFormula, 0, 200, 3, "TASTdigitizer", "ResTimeST");  // 0-200 MeV range in energy loss
}

// --------------------------------------------------------------------------------------
void  TASTdigitizer::SetInitParFunction()
{
   //all in mm
   // !!! WRONG number for Birks coefficient  and light collection efficiency put 0.9!!!
   fFuncBirks->SetParameters(10400, 0.0126, 0.9); // photons yield (Nphoton/MeV), kB: birks (mm/MeV), collection efficiency (0.4%) for EJ-202
   fTimeSTResE->SetParameters(fTimeST_p0, fTimeST_p1, fTimeST_A);
}

// --------------------------------------------------------------------------------------
TASTdigitizer::~TASTdigitizer()
{   
   delete fFuncBirks;
   delete fTimeSTResE;
}

// --------------------------------------------------------------------------------------
Double_t TASTdigitizer::ResTimeFormula(Double_t* x, Double_t* par)
{
   Double_t eloss = x[0];
   Double_t p0 = par[0];
   Double_t p1 = par[1];
   Double_t A = par[2];
   
   return  p0*sqrt(A/eloss + p1); 
   
}
// --------------------------------------------------------------------------------------
Double_t TASTdigitizer::RecPhotonsN(Double_t* x, Double_t* par)
{
   Double_t xx = x[0];
   Double_t photonsN = par[0]*xx/(1+par[1]*xx)*par[2];

   return photonsN;
}

//___________________________________________________________________________________________

Double_t TASTdigitizer::GetPhotonsN(Double_t /*X*/, Double_t /*Y*/, Double_t edep)
{
   return fFuncBirks->Eval(edep);
}

//___________________________________________________//
Bool_t TASTdigitizer::IsOverEnergyThreshold(double edep_thr, double edep) {
   
   fEnergyThreshold = edep_thr;
   
   if(edep>fEnergyThreshold)
      return true;
   else
      return false;
   
}
//___________________________________________________________________________________________
Double_t TASTdigitizer::GetResTimeST(Double_t edep)
{
   
   Double_t p0 = gRandom->Gaus(fTimeST_p0, fTimeSTErr_p0);
   Double_t p1 = gRandom->Gaus(fTimeST_p1, fTimeSTErr_p1);
   
   fTimeSTResE->SetParameters(p0,p1,fTimeST_A);
   
   return fTimeSTResE->Eval(edep);
   
}

//___________________________________________________________________________________________

void TASTdigitizer::SmearTimeST(Double_t eloss, Double_t &time) {

   Double_t resTimeST = GetResTimeST(eloss);

   time += gRandom->Gaus(0,resTimeST);

   //time should be stored in the hit in ns:
   time  *= TAGgeoTrafo::PsToNs();
   
   return;

}
//___________________________________________________________________________________________

Bool_t TASTdigitizer::Process(Double_t edep, Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/, Double_t time, Int_t id, Int_t /*Z*/, Double_t /*px0*/, Double_t /*py0*/, Double_t /*pz0*/)
{

    // Double_t charge = GetPhotonsN(x0, y0, edep)*fGain;
   if(FootDebugLevel(4))
     Info("Process"," edep::%.2f  timeST::%.2f",edep,time);
  
   SmearTimeST(edep,time);

   if(FootDebugLevel(4))
     Info("Process","Smear ->  edep::%.2f  timeST::%.2f",edep,time);
   
   if (fMap[id] == 0) {
     fCurrentHit = (TASThit*)fpNtuRaw->NewHit(edep, edep, time); 
      fMap[id] = fCurrentHit;
   } else {
      fCurrentHit = fMap[id];
      
      if(FootDebugLevel(4))
        Info("Process"," timeST  current::%.2f  previous::%2.f",time,fCurrentHit->GetTime());

       // for multi hit take the time of the hit with the largest Eloss
      if(edep>fEnergyMax) {
         
        fEnergyMax = edep;
        fCurrentHit->SetTime(time);

       }
       
       // for multi hit sum the Elosses
       fCurrentHit->SetDe(fCurrentHit->GetDe() + edep);
       if(FootDebugLevel(4))
         Info("Process"," eloss  current::%.2f  total::%.2f",edep,fCurrentHit->GetDe());

   }
   
   return true;
}


