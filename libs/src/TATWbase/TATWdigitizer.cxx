/*!
 \file TATWdigitizer.cxx
 \brief   Implemetation of TATWdigitizer.
 */

#include "TRandom3.h"
#include "TF1.h"
#include "TMath.h"

#include "TATWdigitizer.hxx"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TATWdigitizer
 --> 09/04/2020 parameters changed by mtoppi using updated values provided by R.Zarrella (Eloss res and TimeTW res) and fitted in F.Murtas thesis and M.Morrocchi (velocity of prop of light in the bars)
 */

//! Class Imp

Double_t TATWdigitizer::fgHfactor = 1.45;

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
   fDeResMin(3.88),  // min rel [%] resolution value from Table 2 Kraan et al
   fDeResMax(4.82),   // max rel [%] resolution value from Table 2 Kraan et al
   fDeResECst(7.033),
   fDeErrResECst(0.05592),
   fDeResEC(-0.02046),
   fDeErrResEC(0.001131),
   // Eloss parameter for intrinsic resolution from MC
   fMClandauFluctuations(false),
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
   // Morrocchi:
   fTofPropAlpha(67.43), // velocity^-1 of propagation of light in TW bar (ps/cm)
   fTofErrPropAlpha(0.09),
   // parameters for position resolution
   fPosCstE(39.33),  // mm
   fPosErrCstE(0.03),  // mm
   fPosLambdaE(-5.665),  // MeV^-1
   fPosErrLambdaE(0.05),  // MeV^-1
   fPosk0E(6.075),  // mm
   fPosErrk0E(0.06),  // mm
   fSlatLength(0),
   fGain(1),
   fElossMeasLimit(90),  // MeV. Is the maximum Eloss measured up to now...resolution beyond this value is not reliable and is set constant
   fEnergyThreshold(0),    // Set energy loss threshold from config file
   fEnergyMax(-1)         // max energy loss in the case of multiple hits
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
   fDeRateShift = new TF1("ElossRateShift", this, &TATWdigitizer::RateSaturation, 0, 200, 3, "TATWdigitizer", "ElossRateShift");  // 0-200 MeV range in energy loss
   fDeResE      = new TF1("ResEnergy", this, &TATWdigitizer::ResLinear, 0, 200, 2, "TATWdigitizer", "ResEnergy");  // 0-200 MeV range in energy loss
   fDeResE_MC   = new TF1("ResEnergy_MC", this, &TATWdigitizer::ResFormula, 0, 200, 3, "TATWdigitizer", "ResEnergy_MC");  // 0-200 MeV range in energy loss
   fTimeTWResE  = new TF1("ResTimeTW", this, &TATWdigitizer::ResFormula, 0, 200, 3, "TATWdigitizer", "ResTimeTW");  // 0-200 MeV range in energy loss
   fPosResE     = new TF1("ResPos", this, &TATWdigitizer::ResFormula, 0, 200, 3, "TATWdigitizer", "ResPos");  // 0-200 MeV range in energy loss
}

// --------------------------------------------------------------------------------------
void  TATWdigitizer::SetInitParFunction()
{
   //all in cm, ps and MeV
   fDeResE->SetParameters(fDeResECst, fDeResEC);
   fDeResE_MC->SetParameters(fEmcA, fEmcB, fEmcC);
   fTimeTWResE->SetParameters(fTimeTW_C, fTimeTW_B, fTimeTW_C);
   fPosResE->SetParameters(fPosCstE, fPosLambdaE, fPosk0E);
   fDeRateShift->SetParameters(fDeRateShiftPar0,fDeRateShiftPar1,fDeRateShiftPar2);
   
}

// --------------------------------------------------------------------------------------
TATWdigitizer::~TATWdigitizer()
{   
   delete fDeResE;
   delete fDeResE_MC;
   delete fTimeTWResE;
   delete fPosResE;
   delete fDeRateShift;
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::ResLinear(Double_t* x, Double_t* par)
{
   Double_t eloss = x[0];
   Double_t q = par[0];
   Double_t m = par[1];
   
   Double_t res = q + m * eloss; 
   
   return res;
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::ResFormula(Double_t* x, Double_t* par)
{
   Double_t eloss = x[0];
   Double_t A = par[0];
   Double_t B = par[1];
   Double_t C = par[2];
   
   Double_t res = A/eloss + B/sqrt(eloss) + C;  // fitting relative energy resolution to smear MC true with experimental timeTW resolution
   
   return res;
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::RateSaturation(Double_t* x, Double_t* par)
{
   Double_t eloss = x[0];
   Double_t A = par[0];
   Double_t B = par[1];
   Double_t C = par[2];
   
   Double_t res = A+B*TMath::Exp(-eloss/C); //to change with the proper model
   
   return res;
}


// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::ResExponential(Double_t* x, Double_t* par)
{
   Double_t deltaE = x[0];
   Double_t res    = par[0]*TMath::Exp(-deltaE*par[1]) + par[2];
   
   return res;
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::GetResEnergyExp(Double_t energy)
{

   // variation of the linear fit parameters
   Double_t q = gRandom->Gaus(fDeResECst, fDeErrResECst);
   Double_t m = gRandom->Gaus(fDeResEC, fDeErrResEC);
   
   fDeResE->SetParameters(q,m);
   
   Double_t relRes;
   if(energy<fElossMeasLimit)
      relRes = fDeResE->Eval(energy); // percentage Eloss resolution
   else
      relRes = 5.2; // percentage Eloss resolution constant at the minimum because beyond Eloss = 90 MeV has not been measured

   // relRes = gRandom->Uniform(fDeResMin,fDeResMax)*sqrt(2);  // to be multiplied by sqrt(2) because in the Kraan paper the rel err is on the sum of the eloss front and rear
   
   return energy*relRes/100.;
   
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::GetElossShiftRate()
{
   
   if(fHisRate==NULL) return 0.0f;
   
   Double_t rate = fHisRate->GetRandom();
   Double_t shift = fDeRateShift->Eval(rate);
   
   if (FootDebugLevel(4)) {
      cout << "Eloss smearing with rate::" << rate << "   shift::" << shift << endl;
   }   
   
   return shift;
   
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::GetResEnergyMC(Double_t energy)
{

   if(!fMClandauFluctuations) return 0.;

   Double_t A = gRandom->Gaus(fEmcA,fEmcErrA);
   Double_t B = gRandom->Gaus(fEmcB,fEmcErrB);
   Double_t C = gRandom->Gaus(fEmcC,fEmcErrC);
   
   fDeResE_MC->SetParameters(A,B,C);
   
   Double_t relRes = fDeResE_MC->Eval(energy); // percentage Eloss resolution
   
   return energy*relRes/100.;
   
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::GetResCharge(Double_t energy)
{
   
   // Double_t relResEloss = GetResEnergyExp(energy);
   Double_t resElossExp = GetResEnergyExp(energy);
   Double_t resElossMC  = GetResEnergyMC(energy);
   Double_t relResEloss = sqrt(pow(resElossExp,2)-pow(resElossMC,2));
   
   return sqrt(2)*relResEloss;
   
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::GetResPos(Double_t edep)
{
   
   Double_t A = gRandom->Gaus(fPosCstE, fPosErrCstE);
   Double_t B = gRandom->Gaus(fPosLambdaE, fPosErrLambdaE);
   Double_t C = gRandom->Gaus(fPosk0E, fPosErrk0E);
   
   fPosResE->SetParameters(A,B,C);
   
   return fPosResE->Eval(edep);
   
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::GetResTimeTW(Double_t edep)
{
   
   Double_t A = gRandom->Gaus(fTimeTW_A, fTimeTWErr_A);
   Double_t B = gRandom->Gaus(fTimeTW_B, fTimeTWErr_B);
   Double_t C = gRandom->Gaus(fTimeTW_C, fTimeTWErr_C);
   
   fTimeTWResE->SetParameters(A,B,C);
   
   return fTimeTWResE->Eval(edep)*sqrt(2);
   
}
// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::GetTimeLeft(Double_t pos, Double_t time, Double_t edep)
{
   
   // Double_t alpha  = gRandom->Gaus(fTofPropAlpha, fTofErrPropAlpha);
   Double_t alpha  = fTofPropAlpha;
   
   // Double_t timeL  = time - pos*alpha;
   Double_t timeL  = time + (fSlatLength/2. - pos) * alpha;  // Left--> positive Pos   L/A <-------- R/B
   
   Double_t resTimeL = GetResTimeTW(edep)*TMath::Sqrt(2.); // Assume same time resolution L/R
   timeL += gRandom->Gaus(0, resTimeL);
   
   return timeL;
}

// --------------------------------------------------------------------------------------
Double_t TATWdigitizer::GetTimeRight(Double_t pos, Double_t time, Double_t edep)
{
   
   // Double_t alpha  = gRandom->Gaus(fTofPropAlpha, fTofErrPropAlpha);
   Double_t alpha  = fTofPropAlpha;
   
   // Double_t timeR  = time + pos*alpha;
   Double_t timeR  = time + (fSlatLength/2. + pos) * alpha;  // Right--> negative Pos L/A <-------- R/B
   
   Double_t resTimeR = GetResTimeTW(edep)*TMath::Sqrt(2.); // Assume same time resolution L/R
   timeR += gRandom->Gaus(0, resTimeR);
   
   return timeR;
}
//-----------------------------------------------------------------------------
//! Method to compute the position along the bar using the time difference btw ChA e Chb for a given bar (as done in data)
void TATWdigitizer::ComputePosDeltaTime(Double_t eloss, Double_t time, Double_t &pos, Double_t &timeA, Double_t &timeB) {

    // Reconstruct the position along the bar as in data using time differences between bar edges
    // define tA and tB using position of the hit along the bar   
    timeA = GetTimeLeft(pos, time, eloss);
    timeB = GetTimeRight(pos, time, eloss);
    
    pos = (timeB-timeA)/(2*fTofPropAlpha);  // cm

    if(FootDebugLevel(4)) {
      printf("time A::%.1f B::%.1f\n", timeA, timeB);
      cout<<endl<<"pos_diff::"<<pos<<endl<<endl;
    }

    return;

}
// --------------------------------------------------------------------------------------
Bool_t TATWdigitizer::IsOverEnergyThreshold(double edep_thr, double edep) {
   
   fEnergyThreshold = edep_thr;
   
   if(edep>fEnergyThreshold)
      return true;
   else
      return false;
   
}

// --------------------------------------------------------------------------------------
void TATWdigitizer::SmearEnergyLoss(Double_t &Eloss) {

   Double_t ElossShiftRate = GetElossShiftRate();
   Double_t resElossExp = GetResEnergyExp(Eloss);  // phys + experimental fluctuation
   Double_t resElossMC = GetResEnergyMC(Eloss);  // physical fluctuaction (landau)
   Eloss += gRandom->Gaus(ElossShiftRate,sqrt(pow(resElossExp,2)-pow(resElossMC,2)));
   
   return;

}

// --------------------------------------------------------------------------------------
void TATWdigitizer::SmearTimeTW(Double_t eloss, Double_t &time) {

   Double_t resTimeTW = GetResTimeTW(eloss);

   time += gRandom->Gaus(0,resTimeTW);
   
   //time should be stored in the hit in ns:
   time  *= TAGgeoTrafo::PsToNs();
   
   return;

}

// --------------------------------------------------------------------------------------
void TATWdigitizer::SmearPosAlongBar(Double_t eloss, Double_t &pos_rec) {

  // position resolution from data
   Double_t resPos = (Double_t)GetResPos(eloss)*TAGgeoTrafo::MmToCm();  //resolution in mm and position in cm!!
   pos_rec += gRandom->Gaus(0,resPos);

   return;
}

// --------------------------------------------------------------------------------------
void TATWdigitizer::SmearTWquantities(Double_t &edep, Double_t &timeTW, Double_t &pos) {

   // smear the timeTW with the experimental resolution
   SmearTimeTW(edep,timeTW);

   // smear the position of the hit along the TW bar with the exp resolution
   SmearPosAlongBar(edep,pos);

   // smear the energy loss with the experimental resolution
   SmearEnergyLoss(edep);
   
   return;
   
}
//-------------------------------------------------------------------------------
void  TATWdigitizer::CheckPUmanaging(Double_t time, Double_t eloss, Double_t pos, Int_t zmc) {
  
         Info("Process","PU case");
         cout<<"current HIT vs previous HIT"<<endl;
         cout<<"timeTW ::"<<time<<"  ::"<<GetCurrentHit()->GetTime()<<endl;
         cout<<"eloss  ::"<<eloss<<"  ::"<<GetCurrentHit()->GetEnergyLoss()<<endl;
         cout<<"pos    ::"<<pos<<"  ::"<<GetCurrentHit()->GetPosition()<<endl;
         cout<<"Z      ::"<<zmc<<"  ::"<<GetCurrentHit()->GetChargeZ()<<endl<<endl;

         return;
}

// -----------------------------------------------------------------------------
//! Reconstruct the MC TW hit, smearing the time, the eloss and the position along the bar according to the respective experimental resolutions. For multi hits in the same bar a single hit is reconstracrted (by default) with a depoited energy given by the sum of the single energies deposited and the TW time, the charge Z and the position are the ones related to the hit with the biggest energy deposit
Bool_t TATWdigitizer::Process(Double_t edep, Double_t posAlongBar, Double_t layer, Double_t barid, Double_t twhitID, Double_t timeTW, Int_t baridA, Int_t Z, Double_t /*px0*/, Double_t /*py0*/, Double_t /*pz0*/){
   
   Double_t recEloss       = edep;
   Double_t recTimeTW      = timeTW;
   Double_t recPosAlongBar = posAlongBar;
   
   Int_t dummyTrigType  = -1000;

   Double_t posDeltaTime(-100), timeA(-1), timeB(-1);
   ComputePosDeltaTime(edep,timeTW,recPosAlongBar,timeA,timeB); 

   //-----Smear Eloss Time and Pos according to experimental resolutions-----//
   SmearTWquantities(recEloss,recTimeTW,recPosAlongBar);


   
   if( IsPileUpOff() ||  fMap[baridA] == 0 )  { // first hit in a given bar
  
       fCurrentHit = (TATWhit*)fpNtuRaw->NewHit((Int_t)layer, (Int_t)barid, recEloss, recTimeTW, recPosAlongBar, posDeltaTime, posAlongBar, edep, timeTW, timeA ,timeB, dummyTrigType); // timeA/B is ps, and timeTW in ns !


       fCurrentHit->SetChargeZ(Z);      //here set true Z charge: the rec charge is set after in TATWactNtuMC.cxx to the final hit (once pile-up has been considered)
       
       fMap[baridA] = fCurrentHit;
         
   } else { // multiple hit in an already hit bar
       
       fCurrentHit =  fMap[baridA];

       if(FootDebugLevel(4)) 
         CheckPUmanaging(recTimeTW,recEloss,recPosAlongBar,Z);
       

       // for multi hit in the same bar take the time, position and Z of the hit with greater Eloss
       if(recEloss>fEnergyMax) {    
         
         fEnergyMax = recEloss;
         
         fCurrentHit->SetTime(recTimeTW);
         fCurrentHit->SetPosition(recPosAlongBar);
         
         fCurrentHit->SetTimeChA(timeA);
         fCurrentHit->SetTimeChB(timeB);
         
         fCurrentHit->SetChargeZ(Z);        // here set true Z charge: the rec charge is set later in TATWactNtuHitMC.cxx to the final hit (once multi hit edep has been added)       

       }
       
       // for multi hit in the same bar sum the Elosses
       fCurrentHit->SetEnergyLoss(fCurrentHit->GetEnergyLoss() + recEloss);
       

       if(FootDebugLevel(4)) 
         CheckPUmanaging(fCurrentHit->GetTime(),fCurrentHit->GetEnergyLoss(),fCurrentHit->GetPosition(),fCurrentHit->GetChargeZ());


   }
   
   return true;
   
}
