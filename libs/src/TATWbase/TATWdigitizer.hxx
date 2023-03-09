#ifndef _TATWdigitizer_HXX
#define _TATWdigitizer_HXX

/*!
 \file TATWdigitizer.hxx
 \brief   Declaration of TATWdigitizer.
 */
/*------------------------------------------+---------------------------------*/


#include <map>

#include "TAGbaseDigitizer.hxx"
#include "TATWparCal.hxx"
#include "TATWparGeo.hxx"
#include "TATWntuHit.hxx"

using namespace std;

class TF1;
// --------------------------------------------------------------------------------------
class TATWdigitizer : public TAGbaseDigitizer {
   
public:
  TATWdigitizer(TATWntuHit* pNtuRaw, TAGparaDsc* pParGeo, TAGparaDsc* pParCal);
   ~TATWdigitizer();
   
   void           SetFunctions();
   void           SetInitParFunction();
   void           SmearTWquantities(Double_t &energy, Double_t &time, Double_t &pos);
   void           SmearEnergyLoss(Double_t &energy);
   void           SmearTimeTW(Double_t energy,Double_t &time);
   void           SmearPosAlongBar(Double_t energy,Double_t &pos);
   void           ComputePosDeltaTime(Double_t eloss, Double_t time, Double_t &pos, Double_t &timea, Double_t &timeb);

   
   Bool_t         Process(Double_t edep, Double_t posalongbar, Double_t layer, Double_t barId, Double_t hitId, Double_t time = 0, Int_t sensorId = 0, Int_t Z =-99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);
   
   Double_t       GetResCharge(Double_t energy);
   Double_t       GetResEnergyExp(Double_t energy);
   Double_t       GetResEnergyMC(Double_t energy);
   Double_t       GetElossShiftRate();

   TH1D*          GetRate() {return fHisRate;}
  
   Double_t       ResLinear(Double_t* x, Double_t* par);
   Double_t       ResFormula(Double_t* x, Double_t* par);
   Double_t       ResExponential(Double_t* x, Double_t* par);
   Double_t       RateSaturation(Double_t* x, Double_t* par);
   
   Bool_t         IsOverEnergyThreshold(double ethr, double ene);  
   Double_t       GetEnergyThreshold() {return fEnergyThreshold;}

   Bool_t         IsMCtrue() {return fMCtrue;}
   Bool_t         IsPileUpOff() {return fPileUpOff;}

   void           SetMCtrue() {fMCtrue = true;}
   void           SetPileUpOff() {fPileUpOff = true;}
   void           SetRateSmearing() {fMCRateSmearing = true;}
  
   Double_t       GetResPos(Double_t edep);
   Double_t       GetResTimeTW(Double_t edep);
   
   Double_t       GetTimeLeft(Double_t pos, Double_t time, Double_t edep);
   Double_t       GetTimeRight(Double_t pos, Double_t time, Double_t edep);
   
   void           SetGain(Double_t g)   { fGain = g;          }
   void           CheckPUmanaging(Double_t time, Double_t edep, Double_t pos, Int_t chargeZ);
   TATWhit*       GetCurrentHit()      { return fCurrentHit; }
 
   void           ClearMap()           { fMap.clear();       }
   
private:
   TATWntuHit*   fpNtuRaw;
   TATWhit*      fCurrentHit;
   TAGparaDsc*   fpParGeo;
   TAGparaDsc*   fpParCal;

   TATWparCal*   twParCal;
   TATWparGeo*   twParGeo;
   
   // flags
   Bool_t        fMCtrue;
   Bool_t        fPileUpOff;  ///< flag to switch off the PU: when true, hits dopositing energy in the same bar are stored as indipendent hit. When false (defult) multiple hits in the same bar are reconstructed like a single hit
   Bool_t        fMClandauFluctuations;
   Bool_t        fMCRateSmearing;

   // deltaE
   
   TF1*          fDeResE;   ///< function to smear the trueMC energy deposited in TW with experimental TW energy loss resolution and parameters in the following
   Double_t      fDeResMin;
   Double_t      fDeResMax;
   Double_t      fDeResECst;
   Double_t      fDeErrResECst;
   Double_t      fDeResEC;
   Double_t      fDeErrResEC;

  //rate saturation effect
   TH1D*         fHisRate;
   TF1*          fDeRateShift;
   Double_t      fDeRateShiftPar0;
   Double_t      fDeRateShiftPar1;
   Double_t      fDeRateShiftPar2;
  
   TF1*          fDeResE_MC; ///< function to take care of MC fluctuations and parameters in the following-->not used
   Double_t      fEmcA;  // MeV
   Double_t      fEmcErrA;  // MeV
   Double_t      fEmcB;  // sqrt(MeV)
   Double_t      fEmcErrB;  // sqrt(MeV)
   Double_t      fEmcC;
   Double_t      fEmcErrC;

   Double_t      fElossMeasLimit;
   Double_t      fEnergyThreshold;
   Double_t      fEnergyMax;

   // position
   TF1*          fPosResE;  ///< function to smear trueMC position along the bar with experimental position resolution and parameters in the following
   Double_t      fPosCstE;
   Double_t      fPosErrCstE;
   Double_t      fPosLambdaE;
   Double_t      fPosErrLambdaE;
   Double_t      fPosk0E;
   Double_t      fPosErrk0E;

   // TimeTW
   TF1*          fTimeTWResE;  ///< function to smear the trueMC TWtime with experimental TW time resolution and parameters in the following
   Double_t      fTimeTW_A;
   Double_t      fTimeTWErr_A;
   Double_t      fTimeTW_B;
   Double_t      fTimeTWErr_B;
   Double_t      fTimeTW_C;
   Double_t      fTimeTWErr_C;

   // inverse of light propagation velocity
   Double_t      fTofPropAlpha;
   Double_t      fTofErrPropAlpha; 

  // misc
   Double_t      fSlatLength;
   Double_t      fGain;
   
   map<int, TATWhit*> fMap; //! map for pilepup
   
   
private:
   static       Double_t fgHfactor; // happy factor for edep

};
#endif

