#ifndef _TATWdigitizer_HXX
#define _TATWdigitizer_HXX

#include <map>

#include "TAGbaseDigitizer.hxx"

#include "TATWntuRaw.hxx"

/*!
 \file
 \version $Id: TATWdigitizer,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TATWdigitizer.
 
 Parameters taken from M. Morrocchi, E. Ciarrocchi et al., NIM A 911 (2018) 0–8

--> 09/04/2020 parameters changed by mtoppi using updated values provided by R.Zarrella (Eloss res and Tof res) and M.Morrocchi (velocity of prop of light in the bars)
 */
/*------------------------------------------+---------------------------------*/

using namespace std;

class TF1;
// --------------------------------------------------------------------------------------
class TATWdigitizer : public TAGbaseDigitizer {
   
public:
   TATWdigitizer(TATWntuRaw* pNtuRaw);
   ~TATWdigitizer();
   
   void           SetFunctions();
   void           SetInitParFunction();
   
   Bool_t         Process(Double_t edep, Double_t x0, Double_t y0, Double_t zin=0, Double_t timeST=0, Double_t tof = 0, Int_t sensorId = 0, Int_t Z =-99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);
   
   Float_t        GetResCharge(Float_t energy);
   Float_t        GetResEnergyExp(Float_t energy);
   Float_t        GetResEnergyMC(Float_t energy);

   Double_t       ResLinear(Double_t* x, Double_t* par);
   Double_t       ResFormula(Double_t* x, Double_t* par);
   Double_t       ResExponential(Double_t* x, Double_t* par);
   
   Bool_t         IsOverEnergyThreshold(double ethr, double ene);  
   Double_t       GetEnergyThreshold() {return fEnergyThreshold;}

   Bool_t         IsMCtrue() {return fMCtrue;}
   Bool_t         IsPileUpOff() {return fPileUpOff;}

   void           SetMCtrue() {fMCtrue = true;}
   void           SetPileUpOff() {fPileUpOff = true;}
 
   Float_t        GetResPos(Float_t edep);
   Float_t        GetResToF(Float_t edep);
   Float_t        GetResTimeTW(Float_t edep);
   
   Float_t        GetDeAttLeft(Float_t pos, Float_t edep);
   Double_t       DeAttLeft(Double_t* pos, Double_t* par);

   Float_t        GetDeAttRight(Float_t pos, Float_t edep);
   Double_t       DeAttRight(Double_t* pos, Double_t* par);
   
   Float_t        GetTimeLeft(Float_t pos, Float_t time, Float_t edep);
   Float_t        GetTimeRight(Float_t pos, Float_t time, Float_t edep);
   
   void           SetGain(Float_t g)   { fGain = g;          }
   TATWntuHit*    GetCurrentHit()      { return fCurrentHit; }
 
   void           ClearMap()           { fMap.clear();       }
   
private:
   TATWntuRaw*   fpNtuRaw;
   TATWntuHit*   fCurrentHit;
   TATWparGeo*   fpParGeo;
   
   // flags
   Bool_t         fMCtrue;
   Bool_t         fPileUpOff;

   // deltaE
   TF1*          fDeResE;
   Float_t       fDeResECst;
   Float_t       fDeErrResECst;
   Float_t       fDeResEC;
   Float_t       fDeErrResEC;

   TF1*          fDeResE_MC;
   Float_t       fEmcA;  // MeV
   Float_t       fEmcErrA;  // MeV
   Float_t       fEmcB;  // sqrt(MeV)
   Float_t       fEmcErrB;  // sqrt(MeV)
   Float_t       fEmcC;
   Float_t       fEmcErrC;

   TF1*          fDeAttLeft;
   Float_t       fDeAttCstLeft;
   Float_t       fDeErrAttCstLeft; // not used
   Float_t       fDeAttLambdaLeft;
   Float_t       fDeErrAttLambdaLeft;
   
   TF1*          fDeAttRight;
   Float_t       fDeAttCstRight;
   Float_t       fDeErrAttCstRight; // not used
   Float_t       fDeAttLambdaRight;
   Float_t       fDeErrAttLambdaRight;
   
   Float_t       fDeAttAsym;
   Float_t       fDeAttAsymSmear;

   Double_t      fEnergyThreshold;
   Double_t      fElossMeasLimit;

   // position
   TF1*          fPosResE;
   Float_t       fPosCstE;
   Float_t       fPosErrCstE;
   Float_t       fPosLambdaE;
   Float_t       fPosErrLambdaE;
   Float_t       fPosk0E;
   Float_t       fPosErrk0E;

   // TOF
   TF1*          fTimeTWResE;
   Float_t       fTimeTW_A;
   Float_t       fTimeTWErr_A;
   Float_t       fTimeTW_B;
   Float_t       fTimeTWErr_B;
   Float_t       fTimeTW_C;
   Float_t       fTimeTWErr_C;

  // TOF
   TF1*          fTofResE;
   Float_t       fTofCstE;
   Float_t       fTofErrCstE;
   Float_t       fTofLambdaE;
   Float_t       fTofErrLambdaE;
   Float_t       fTofk0E;
   Float_t       fTofErrk0E;

   Float_t       fTofPropAlpha; // inverse of light propagation velocity
   Float_t       fTofErrPropAlpha; 

  // misc
   Float_t       fSlatLength;
   Float_t       fGain;
   
   map<int, TATWntuHit*> fMap; //! map for pilepup
   
   
private:
   static       Float_t fgHfactor; // happy factor for edep

};
#endif

