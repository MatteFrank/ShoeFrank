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
#include "TRandom.h"
#include <TCanvas.h>
#include "TATWdigitizer.hxx"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

Float_t TATWdigitizer::fgHfactor = 1.45;

// --------------------------------------------------------------------------------------
TATWdigitizer::TATWdigitizer(TATWntuHit* pNtuRaw ,TAGparaDsc* pParGeo, TAGparaDsc* pParCal)
 : TAGbaseDigitizer(),
   fpNtuRaw(pNtuRaw),
   fpParGeo(pParGeo),
   fpParCal(pParCal),
   fCurrentHit(0x0),
   fMCtrue(true),
   fPileUpOff(false),
   fMCRateSmearing(false),
   // Rate saturation effect
   fHisRate(NULL),
   fDeRateShiftPar0(0.0),
   fDeRateShiftPar1(0.0),
   fDeRateShiftPar2(1.0),
   // Eloss parameter for Experimental Resolution eith ResLinear
   fDeResECst(7.033),
   fDeErrResECst(0.05592),
   fDeResEC(-0.02046),
   fDeErrResEC(0.001131),
   fDeAttCstLeft(10),
   fDeErrAttCstLeft(0.2),
   // parameters for attenuation: not used
   fDeAttLambdaLeft(12.1),
   fDeErrAttLambdaLeft(0.5),
   fDeAttCstRight(10),
   fDeErrAttCstRight(0.2),
   fDeAttLambdaRight(10.7),
   fDeErrAttLambdaRight(0.3),
   fDeAttAsym(0.09),
   fDeAttAsymSmear(0.),
   // Eloss parameter for intrinsic resolution from MC
   fEmcA(-0.057),  // MeV
   fEmcErrA(0.002),  // MeV
   fEmcB(0.293),  // sqrt(MeV)
   fEmcErrB(0.002),  // sqrt(MeV)
   fEmcC(-0.0098), 
   fEmcErrC(0.0003),
   // timeTW parameter for resolution with ResFormula
   fTimeTW_A(155.9),  // ps*MeV 
   fTimeTWErr_A(5.544),  // ps*MeV
   fTimeTW_B(38.99),  // ps*MeV^1/2
   fTimeTWErr_B(3.852),  // ps*MeV^1/2
   fTimeTW_C(13.36),  // ps
   fTimeTWErr_C(0.4432),  // ps
   // tof parameter for tof resolutionwith ResFormula
   fTofCstE(297.5),  // ps*MeV 
   fTofErrCstE(9.2),  // ps*MeV
   fTofLambdaE(230.9),  // ps*MeV^1/2
   fTofErrLambdaE(5.7),  // ps*MeV^1/2
   fTofk0E(24.9),  // ps
   fTofErrk0E(0.6),  // ps
   // tof parameter for tof resolutionwith ResExponential
   // fTofCstE(241),  // ps 
   // fTofErrCstE(3),  // ps
   // fTofLambdaE(0.08),  // MeV^-1
   // fTofErrLambdaE(0.001),  // MeV^-1
   // fTofk0E(60.2),  // ps
   // fTofErrk0E(0.3),  // ps
   // GSI:
   // fTofPropAlpha(66.), // velocity^-1 of propagation of light in TW bar (ps/cm)
   // fTofErrPropAlpha(2),
   // Morrocchi:
   fTofPropAlpha(67.43), // velocity^-1 of propagation of light in TW bar (ps/cm)
   fTofErrPropAlpha(0.09),
   // parameters for position resolution in mm with ResExponential
   fPosCstE(39.33),  // mm 
   fPosErrCstE(0.03),  // mm
   fPosLambdaE(-5.665),  // MeV^-1
   fPosErrLambdaE(0.05),  // MeV^-1
   fPosk0E(6.075),  // mm
   fPosErrk0E(0.06),  // mm
   // fPosCstE(11.5),  // cm 
   // fPosErrCstE(1.281),  // cm
   // fPosLambdaE(0.09154),  // MeV^-1
   // fPosErrLambdaE(0.03703),  // MeV^-1
   // fPosk0E(5.8),  // cm
   // fPosErrk0E(0.3917),  // cm
   fSlatLength(0),
   fGain(1),
   fElossMeasLimit(90),  // MeV. Is the maximum Eloss measured up to now...resolution beyond this value is not reliable and is set constant
   fEnergyThreshold(0)
{
   SetFunctions();
   SetInitParFunction();
   
   twParGeo =   (TATWparGeo*) gTAGroot->FindParaDsc(TATWparGeo::GetDefParaName(), "TATWparGeo")->Object();
   twParCal = (TATWparCal*)fpParCal->Object();

   fSlatLength = twParGeo->GetBarDimension().Y();
   fHisRate = twParCal->GetRate();
         
   fMap.clear();
}

// --------------------------------------------------------------------------------------
void  TATWdigitizer::SetFunctions()
{

   fDeRateShift     = new TF1("ElossRateShift", this, &TATWdigitizer::RateSaturation, 0, 200, 3, "TATWdigitizer", "ElossRateShift");  // 0-200 MeV range in energy loss
  
   fDeResE     = new TF1("ResEnergy", this, &TATWdigitizer::ResLinear, 0, 200, 2, "TATWdigitizer", "ResEnergy");  // 0-200 MeV range in energy loss

   fDeResE_MC    = new TF1("ResEnergy_MC", this, &TATWdigitizer::ResFormula, 0, 200, 3, "TATWdigitizer", "ResEnergy_MC");  // 0-200 MeV range in energy loss

   fTimeTWResE    = new TF1("ResTimeTW", this, &TATWdigitizer::ResFormula, 0, 200, 3, "TATWdigitizer", "ResTimeTW");  // 0-200 MeV range in energy loss

   fTofResE    = new TF1("ResTof", this, &TATWdigitizer::ResFormula, 0, 200, 3, "TATWdigitizer", "ResToF");  // 0-200 MeV range in energy loss
   // fTofResE    = new TF1("ResTof", this, &TATWdigitizer::ResExponential, 0, 200, 3, "TATWdigitizer", "ResToF");  // 0-200 MeV range in energy loss

   // fTofResE_MC    = new TF1("ResTof_MC", this, &TATWdigitizer::ResToF_MC, 0, 200, 3, "TATWdigitizer", "ResToF_MC");  // 0-200 MeV range in energy loss

   // fTofResE    = new TF1("ResDiffTime", this, &TATWdigitizer::ResToF, 0, 200, 3, "TATWdigitizer", "ResToF");  // 0-200 MeV range in energy loss

   fPosResE    = new TF1("ResPos", this, &TATWdigitizer::ResFormula, 0, 200, 3, "TATWdigitizer", "ResPos");  // 0-200 MeV range in energy loss

      // fPosResE    = new TF1("ResPos", this, &TATWdigitizer::ResExponential, 0, 200, 3, "TATWdigitizer", "ResPos");  // 0-200 MeV range in energy loss

   fDeAttLeft  = new TF1("DeAttLeft", this, &TATWdigitizer::DeAttLeft,   -fSlatLength/2, fSlatLength/2., 3, "TATWdigitizer", "DeAttLeft");

   fDeAttRight = new TF1("DeAttRight", this, &TATWdigitizer::DeAttRight, -fSlatLength/2, fSlatLength/2., 3, "TATWdigitizer", "DeAttRight");

}

// --------------------------------------------------------------------------------------
void  TATWdigitizer::SetInitParFunction()
{
   //all in cm, ps and MeV
   fDeResE->SetParameters(fDeResECst, fDeResEC); // fig9 fitted with lines
   fDeResE_MC->SetParameters(fEmcA, fEmcB, fEmcC);
   fTimeTWResE->SetParameters(fTimeTW_C, fTimeTW_B, fTimeTW_C); // 
   fTofResE->SetParameters(fTofCstE, fTofLambdaE, fTofk0E); // fig7
   fPosResE->SetParameters(fPosCstE, fPosLambdaE, fPosk0E); // fit from data in table IV

   fDeRateShift->SetParameters(fDeRateShiftPar0,fDeRateShiftPar1,fDeRateShiftPar2);

   fDeAttLeft->SetParameters(fDeAttCstLeft, fDeAttLambdaLeft, fSlatLength/2.); // fig8
   fDeAttRight->SetParameters(fDeAttCstRight, fDeAttLambdaRight, fSlatLength/2.); // fig8
}

// --------------------------------------------------------------------------------------
TATWdigitizer::~TATWdigitizer()
{   
   delete fDeResE;
   delete fDeResE_MC;
   delete fTimeTWResE;
   delete fTofResE;
   delete fPosResE;
   delete fDeAttLeft;
   delete fDeAttRight;
   delete fDeRateShift;
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::DeAttLeft(Double_t* x, Double_t* par)
{
   Float_t xx  = x[0];
   Float_t att = par[0]*TMath::Exp(-(par[2]+xx)/par[1]);
   
   return att;
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::DeAttRight(Double_t* x, Double_t* par)
{
   Float_t xx  = x[0];
   Float_t att = par[0]*TMath::Exp(-(par[2]-xx)/par[1]);
   
   return att;
}


// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::ResLinear(Double_t* x, Double_t* par)
{
   Float_t eloss = x[0];
   Float_t q = par[0];
   Float_t m = par[1];
   
   Float_t res = q + m * eloss;  // fitting relative energy resolution in fig. 9 with lines
   
   // Float_t res = TMath::Sqrt(par[0]/energy + par[1]*par[1]);
   // res /= 100*TMath::Sqrt(2.); // %<-
   
   return res;
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::ResFormula(Double_t* x, Double_t* par)
{
   Float_t eloss = x[0];
   Float_t A = par[0];
   Float_t B = par[1];
   Float_t C = par[2];
   
   Float_t res = A/eloss + B/sqrt(eloss) + C;  // fitting relative energy resolution in MC true to extract intrinsic physics fluctuations
   
   return res;
}


// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::RateSaturation(Double_t* x, Double_t* par)
{
   Float_t eloss = x[0];
   Float_t A = par[0];
   Float_t B = par[1];
   Float_t C = par[2];
   
   Float_t res = A+B*TMath::Exp(-eloss/C);//to change with the proper model
   
   return res;
}


// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::ResExponential(Double_t* x, Double_t* par)
{
   Float_t deltaE = x[0];
   Float_t res    = par[0]*TMath::Exp(-deltaE*par[1]) + par[2];

   return res;
}

//___________________________________________________________________________________________
Float_t TATWdigitizer::GetDeAttLeft(Float_t pos, Float_t edep)
{
   Float_t energy = edep*fGain; // share equitable
   energy *= (1-fDeAttAsymSmear);
   
   Float_t lambda = gRandom->Gaus(fDeAttLambdaLeft, fDeErrAttLambdaLeft);
   
   fDeAttLeft->SetParameter(0, energy);
   fDeAttLeft->SetParameter(1, lambda);
   
   return fDeAttLeft->Eval(pos);
}

//___________________________________________________________________________________________
Float_t TATWdigitizer::GetDeAttRight(Float_t pos, Float_t edep)
{
   Float_t energy = edep*fGain; // share equitable
   energy *= (1+fDeAttAsymSmear);
   Float_t lambda = gRandom->Gaus(fDeAttLambdaRight, fDeErrAttLambdaRight);
   
   fDeAttRight->SetParameter(0, energy);
   fDeAttRight->SetParameter(1, lambda);
   
   return fDeAttRight->Eval(pos);
}

//___________________________________________________________________________________________
Float_t TATWdigitizer::GetResEnergyExp(Float_t energy)
{

   // Float_t q = gRandom->Gaus(fDeResECst, fDeErrResECst);
   // Float_t m = gRandom->Gaus(fDeResEC, fDeErrResEC);

   // fDeResE->SetParameters(q,m);

   Float_t relRes;
   if(energy<fElossMeasLimit)
     relRes = fDeResE->Eval(energy); // percentage Eloss resolution
   else
     relRes = 5.2; // percentage Eloss resolution constant at the minimum because beyond Eloss = 90 MeV has not been measured
     
   return energy*relRes/100.;

}


//___________________________________________________________________________________________
Float_t TATWdigitizer::GetElossShiftRate()
{

  if(fHisRate==NULL) return 0.0f;
  
  Float_t rate = fHisRate->GetRandom();
  Float_t shift = fDeRateShift->Eval(rate);

  if (FootDebugLevel(1)) {
    cout << "Eloss smearing with rate::" << rate << "   shift::" << shift << endl;
  }

  
  return shift;

}



//___________________________________________________________________________________________
Float_t TATWdigitizer::GetResEnergyMC(Float_t energy)
{

   // Float_t A = gRandom->Gaus(fEmcA,fEmcErrA);
   // Float_t B = gRandom->Gaus(fEmcB,fEmcErrB);
   // Float_t C = gRandom->Gaus(fEmcC,fEmcErrC);

   // fDeResE_MC->SetParameters(A,B,C);

   Float_t relRes = fDeResE_MC->Eval(energy); // percentage Eloss resolution
     
   return energy*relRes/100.;

}

//___________________________________________________________________________________________
Float_t TATWdigitizer::GetResCharge(Float_t energy)
{
  
  // Float_t relResEloss = GetResEnergyExp(energy);
  Float_t resElossExp = GetResEnergyExp(energy);
  Float_t resElossMC  = GetResEnergyMC(energy);
  Float_t relResEloss = sqrt(pow(resElossExp,2)-pow(resElossMC,2));

  return sqrt(2)*relResEloss;

}

//___________________________________________________________________________________________
Float_t TATWdigitizer::GetResPos(Float_t edep)
{

   // Float_t A = gRandom->Gaus(fPosCstE, fPosErrCstE);
   // Float_t B = gRandom->Gaus(fPosLambdaE, fPosErrLambdaE);
   // Float_t C = gRandom->Gaus(fPosk0E, fPosErrk0E);

   // fPosResE->SetParameters(A,B,C);

   return fPosResE->Eval(edep);

}

//___________________________________________________________________________________________
Float_t TATWdigitizer::GetResTimeTW(Float_t edep)
{

   // Float_t A = gRandom->Gaus(fTimeTW_A, fTimeTWErr_A);
   // Float_t B = gRandom->Gaus(fTimeTW_B, fTimeTWErr_B);
   // Float_t C = gRandom->Gaus(fTimeTW_C, fTimeTWErr_C);

   // fTimeTWResE->SetParameters(A,B,C);

   return fTimeTWResE->Eval(edep);

}


//___________________________________________________________________________________________
Float_t TATWdigitizer::GetResToF(Float_t edep)
{

   // Float_t A = gRandom->Gaus(fTofCstE, fTofErrCstE);
   // Float_t B = gRandom->Gaus(fTofLambdaE, fTofErrLambdaE);
   // Float_t C = gRandom->Gaus(fTofk0E, fTofErrk0E);

   // fTofResE->SetParameters(A,B,C);

   return fTofResE->Eval(edep);

}

//___________________________________________________________________________________________
Float_t TATWdigitizer::GetTimeLeft(Float_t pos, Float_t time, Float_t edep)
{

   // Float_t alpha  = gRandom->Gaus(fTofPropAlpha, fTofErrPropAlpha);
   Float_t alpha  = fTofPropAlpha;

   // Float_t timeL  = time - pos*alpha;
   Float_t timeL  = time + (fSlatLength/2. - pos) * alpha;  // Left--> positive Pos   L/A <-------- R/B

   Float_t resTofL = GetResTimeTW(edep)*TMath::Sqrt(2.); // Assume same time resolution L/R
   timeL += gRandom->Gaus(0, resTofL);
   
   return timeL;
}

//___________________________________________________________________________________________
Float_t TATWdigitizer::GetTimeRight(Float_t pos, Float_t time, Float_t edep)
{

   // Float_t alpha  = gRandom->Gaus(fTofPropAlpha, fTofErrPropAlpha);
   Float_t alpha  = fTofPropAlpha;

   // Float_t timeR  = time + pos*alpha;
   Float_t timeR  = time + (fSlatLength/2. + pos) * alpha;  // Right--> negative Pos L/A <-------- R/B

   Float_t resTofR = GetResTimeTW(edep)*TMath::Sqrt(2.); // Assume same time resolution L/R
   timeR += gRandom->Gaus(0, resTofR);
   
   return timeR;
}

//___________________________________________________//
Bool_t TATWdigitizer::IsOverEnergyThreshold(double edep_thr, double edep) {

  fEnergyThreshold = edep_thr;
  
  if(edep>fEnergyThreshold)
    return true;
  else
    return false;
  
}
//___________________________________________________________________________________________
Bool_t TATWdigitizer::Process(Double_t edep, Double_t x0, Double_t y0, Double_t /*zin*/, Double_t timeST, Double_t time, Int_t barid, Int_t Z, Double_t /*px0*/, Double_t /*py0*/, Double_t /*pz0*/){

	
  Int_t dummyTrigType=-1000;
  Bool_t roughDig = true;
  // hit position, bars and layer
   Double_t pos     = 0;
   Int_t baridA = barid;
   Int_t layer = -1;
   if (barid < TATWparGeo::GetLayerOffset()) {  // vertical bar --> layer0
      pos = y0;
      layer = (Int_t)LayerY;
   } else {   // horizontal bars --> layer1
      pos = x0;
      layer = (Int_t)LayerX;
      barid -= TATWparGeo::GetLayerOffset();
   }

   // energy resolution
   Float_t Eloss = edep;
   Float_t ElossShiftRate = GetElossShiftRate();
   Float_t resElossExp = GetResEnergyExp(Eloss);  // phys + experimental fluctuation
   Float_t resElossMC = GetResEnergyMC(Eloss);  // physical fluctuaction (landau)

   //   Eloss += gRandom->Gaus(0,sqrt(pow(resElossExp,2)-pow(resElossMC,2)));
   Eloss += gRandom->Gaus(ElossShiftRate,sqrt(pow(resElossExp,2)-pow(resElossMC,2)));
   /*
  
   fDeAttAsymSmear = gRandom->Uniform(-fDeAttAsym, +fDeAttAsym); // asymmetry btw left/right ends
   
   if (FootDebugLevel(1)) {
      printf("asym %4.2f\n", fDeAttAsymSmear);
      printf("edep %f\n", edep);
   }
  
   Float_t chargeA = GetDeAttLeft(pos, edep);
   Float_t chargeB = GetDeAttRight(pos, edep);
   Float_t chargeA = edep;
   Float_t chargeB = edep;
   
   Float_t resChargeA = GetResEnergy(chargeA,layer);
   chargeA += gRandom->Gaus(0, resChargeA);
   
   Float_t resChargeB = GetResEnergy(chargeB,layer);
   chargeB += gRandom->Gaus(0, resChargeB);

   */
   // fake amplitude A and B--> assign edep
   Double_t amplitudeA = edep;
   Double_t amplitudeB = edep;
   
   // charge supposing chargeA = chargeB 
   Float_t chargeA = edep;
   Float_t chargeB = edep;

   Float_t resChargeA = GetResCharge(chargeA);
   chargeA += gRandom->Gaus(0, resChargeA);
   
   Float_t resChargeB = GetResCharge(chargeB);
   chargeB += gRandom->Gaus(0, resChargeB);

   Double_t energy = TMath::Sqrt(chargeA*chargeB);
   Double_t chargeCOM = TMath::Log(chargeA/chargeB);

   if (FootDebugLevel(4)) {
      printf("pos %.1f\n", pos);
      printf("energy %.1f %.1f\n", chargeA, chargeB);
      printf("Res %.3f %.3f\n", resChargeA*100, resChargeB*100);
   }
   
   // tof resolution from calibration data
   Double_t tof = time;
   Double_t resTof = (Double_t)GetResToF(edep);
   tof += gRandom->Gaus(0,resTof);
   if(FootDebugLevel(4))
     cout<<"smear tof::"<<tof<<endl;
   //Store Tof in ns
   tof *= TAGgeoTrafo::PsToNs(); 

   if(FootDebugLevel(4)) printf("true pos %.1f\n", pos);


   // position resolution from data
   Double_t pos_rec = pos;
   Double_t resPos = (Double_t)GetResPos(edep)*TAGgeoTrafo::MmToCm();  //resolution in mm and position in cm!!
   pos_rec += gRandom->Gaus(0,resPos);
   if(FootDebugLevel(4)) {
     // printf("edep %.1f  resPos %.2f\n", edep, resPos);
     printf("pos %.3f\n\n", pos_rec);
   }

   
   // define tA e tB using position of the hit along the bar
   Double_t timeTW = time + timeST;

   if(FootDebugLevel(4))
     cout<<timeTW<<" "<<time<<" "<<timeST<<endl;
   
   Float_t timeA = GetTimeLeft(pos, timeTW, edep);
   Float_t timeB = GetTimeRight(pos, timeTW, edep);

   if (FootDebugLevel(4)) {
     printf("time %.1f\n", time);
     printf("time A::%.1f B::%.1f\n", timeA, timeB);
   }
   
   Double_t tof_sum = (timeA+timeB)/2. - fSlatLength/2. * fTofPropAlpha - timeST;

   if(FootDebugLevel(4))
     cout<<"tof_sum::"<<tof_sum<<endl;
   Float_t resTimeTW = GetResTimeTW(edep);
   tof_sum += gRandom->Gaus(0,sqrt(pow(resTof,2)-pow(resTimeTW,2))); // add smearing of timeST
   if(FootDebugLevel(4))
     cout<<"tof_sum smear::"<<tof_sum<<endl;

   //Store Tof in ns
   tof_sum *= TAGgeoTrafo::PsToNs(); 

   Double_t pos_diff = (timeB-timeA)/(2*fTofPropAlpha);  // cm   
   if(FootDebugLevel(4))
     cout<<endl<<"pos_diff::"<<pos_diff<<endl<<endl;
   
   if( IsPileUpOff() ) {     

     if(roughDig) {

       fCurrentHit = (TATWhit*)fpNtuRaw->NewHit(layer, barid, energy, tof_sum, tof_sum, pos_diff, chargeCOM, chargeA ,chargeB, amplitudeA ,amplitudeB, timeA, timeB, timeA, timeB,dummyTrigType); // timeA/B is ps, and tof in ns !     
       
       fCurrentHit->SetToF(tof_sum);     
       
       if(FootDebugLevel(4))
	 cout<<"PU off    time::"<<tof_sum<<"  tof::"<<fCurrentHit->GetToF()<<endl;
       
     } else {
       
       fCurrentHit = (TATWhit*)fpNtuRaw->NewHit(layer, barid, Eloss, tof, tof, pos_rec, chargeCOM, chargeA ,chargeB, amplitudeA ,amplitudeB, timeA, timeB, timeA, timeB,dummyTrigType); // timeA/B is ps, and tof in ns !

       fCurrentHit->SetToF(tof);     

       if(FootDebugLevel(4))
	 cout<<"PU off    time::"<<tof<<"  tof::"<<fCurrentHit->GetToF()<<endl;
     }

     //here set true Z charge: the rec charge is set after in TATWactNtuMC.cxx to the final hit (once pile-up has been considered)
     fCurrentHit->SetChargeZ(Z);
	 
   }
   
   else {

     if (fMap[baridA] == 0) {
       
       if(roughDig) {
	 
	 fCurrentHit = (TATWhit*)fpNtuRaw->NewHit(layer, barid, energy, tof_sum, tof_sum, pos_diff, chargeCOM, chargeA ,chargeB, amplitudeA ,amplitudeB, timeA, timeB, timeA, timeB, dummyTrigType); // timeA/B is ps, and tof in ns !     
	 
	 fCurrentHit->SetToF(tof_sum);     
	 
	 if(FootDebugLevel(4))
	   cout<<"PU off    time::"<<tof_sum<<"  tof::"<<fCurrentHit->GetToF()<<endl;
	 
       } else {
       
	 fCurrentHit = (TATWhit*)fpNtuRaw->NewHit(layer, barid, Eloss, tof, tof, pos_rec, chargeCOM, chargeA ,chargeB, amplitudeA ,amplitudeB, timeA, timeB, timeA, timeB, dummyTrigType);
	 
	 fCurrentHit->SetToF(tof);     
	 
	 if(FootDebugLevel(4))
	   cout<<"PU off    time::"<<tof<<"  tof::"<<fCurrentHit->GetToF()<<endl;
       }
       
       //here set true Z charge: the rec charge is set after in TATWactNtuMC.cxx to the final hit (once pile-up has been considered)
       fCurrentHit->SetChargeZ(Z);
       
       fMap[baridA] = fCurrentHit;
       
     } else {  // PILE-UP

       fCurrentHit =  fMap[baridA];


       //Add charge to current hit
       fCurrentHit->SetChargeChA(fCurrentHit->GetChargeChA()+chargeA);
       fCurrentHit->SetChargeChB(fCurrentHit->GetChargeChB()+chargeB);
       
       // take the shortest time
       if (timeA < fCurrentHit->GetChargeTimeA())
         fCurrentHit->SetChargeTimeA(timeA);
       
       if (timeB < fCurrentHit->GetChargeTimeB())
         fCurrentHit->SetChargeTimeB(timeB);


       // recompute 
       energy    = TMath::Sqrt(fCurrentHit->GetChargeChA()*fCurrentHit->GetChargeChB());

       tof_sum   = (fCurrentHit->GetChargeTimeA()+fCurrentHit->GetChargeTimeB())/2  - fSlatLength/2. * fTofPropAlpha - timeST;
       tof_sum  += gRandom->Gaus(0,sqrt(pow(resTof,2)-pow(resTimeTW,2))); // add smearing of timeST
       tof_sum  *= TAGgeoTrafo::PsToNs();

       pos_diff       = (fCurrentHit->GetChargeTimeB()-fCurrentHit->GetChargeTimeA())/(2*fTofPropAlpha);
       chargeCOM = TMath::Log(fCurrentHit->GetChargeChA()/fCurrentHit->GetChargeChB());

       
       if(roughDig) {
	 fCurrentHit->SetEnergyLoss(energy);
	 fCurrentHit->SetToF(tof_sum);
       }
       else {  // for pile-up events take the Eloss Sum of the PU hits and the TOF the average TOF
	 fCurrentHit->SetEnergyLoss(fCurrentHit->GetEnergyLoss()+Eloss);       
	 fCurrentHit->SetToF((fCurrentHit->GetToF()+tof)/2.);
       }

       fCurrentHit->SetTime(fCurrentHit->GetToF());
       fCurrentHit->SetPosition(pos_diff);
       fCurrentHit->SetCOM(chargeCOM);
       
       if(FootDebugLevel(4))
	 cout<<"PU case    time::"<<tof_sum<<"  tof::"<<fCurrentHit->GetToF()<<endl;
       
       // rough: set the "true" Z charge of the PU hit as the charge of the fragment with higher Z (it doesn't work for multiple light ions)
       // The rec charge is set later in TATWactNtuMC.cxx to the final hit (once pile-up has been considered)
       
       if(Z > fCurrentHit->GetChargeZ())  // case of fragmentation inside TW
	 fCurrentHit->SetChargeZ(Z);
       
     }
   }
     
   return true;

}

