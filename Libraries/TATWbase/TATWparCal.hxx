#ifndef _TATWparCal_HXX
#define _TATWparCal_HXX
/*!
 \file TATWparCal.hxx
 \brief   Declaration of TATWparCal.
 */
/*------------------------------------------+---------------------------------*/
#include <vector>

#include "Riostream.h"

#include "TObject.h"
#include "TArrayF.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "TAGparTools.hxx"
#include "TAGparGeo.hxx"
#include "TAGgeoTrafo.hxx"

#include "TATWcalibrationMap.hxx"

//##############################################################################

typedef std::pair<Int_t,Int_t> TPairId;
typedef std::tuple<Float_t,Int_t> TBarsTuple;
typedef std::map<TPairId,TBarsTuple> TMapInfoBar;
typedef std::tuple<Double_t,Double_t> TTupleDeltaT;
typedef std::map<TPairId,TTupleDeltaT> TMapDeltaT;

enum {parNumberZIDcalibFile=6, parNumberBarStatusFile=4, parNumberDeltaTfile=4}; 

class TATWparCal : public TAGparTools {
   
   
private:
   struct ChargeParameter_t : public  TObject {

     map<Int_t,vector<Int_t>>   Layer;      // TW layer
     map<Int_t,vector<Int_t>>   Charge;     // charge
     map<Int_t,vector<Float_t>> Norm_BB;    // par0 BB
     map<Int_t,vector<Float_t>> Const_BB;   // par1 BB
     map<Int_t,vector<Float_t>> CutLow;     // Lower Tof cut
     map<Int_t,vector<Float_t>> CutUp;      // Upper Tof cut
     map<Int_t,vector<Float_t>> distMean;   // <E_meas - E_BB>
     map<Int_t,vector<Float_t>> distSigma;  // sigma(E_meas - E_BB)

   };
   
   ChargeParameter_t fChargeParameter;
   
   struct BarsParameter_t : public  TObject {

     vector<Int_t> LayerId;  // layer Id
      vector<Int_t> BarId;  // bar id in shoe notatio (0,...,19 bar id for each layer)
      vector<Int_t> ActiveBar;  // active/dead bars
      vector<Float_t> ElossThr;   // eloss thresholds per TW bar

   };
   
   BarsParameter_t fBarsParameter;
   
   TPairId     fPairId;
   TBarsTuple  fBarsTuple;
   TMapInfoBar fMapInfoBar;
   TMapDeltaT  fMapDeltaTime;
   
   TATWcalibrationMap *fMapCal;
   
   Bool_t f_isElossTuningON;
   Bool_t f_isPosCalibration; // default one from Pisa group
   Bool_t f_isBarCalibration;
   
   TAGparGeo*    fParGeo;
   TAGgeoTrafo*  fGeoTrafo;
   TATWparGeo*   fParGeoTw;
  
   Int_t         fZbeam;
   Double_t      fTof_beam;
   Double_t      fTof_min;
   Double_t      fTof_max;
   Double_t      fAvgBeamEnergyLossInBar;

   int           fZraw;
   float         f_dist_min_Z;
   vector<float> f_dist_Z;

   TString       fIonBeamName;
   TH1D*         fHisRate;
  
private:
   static TString fgkBBparamName;  // default BBparameters for Z identification with TW
   static TString fgkBarStatus;    // bar status file
   static TString fgkDeltaTime;    // delta Time and offset calibration file

private:
   void       RetrieveBeamQuantities();
   void       ComputeAvgBeamEnergyLossInTwBar(Int_t Z, Double_t Beta );

   
   Double_t   fBBparametrized(double tof, int chg, double par0, double par1);
   Double_t   fBB_prime_parametrized(double tof, int chg, double par0, double par1);
   Float_t    fDist(double tof, double eloss, double x, double fBB);
   Double_t   fDistPrime(double tof, double eloss, double x, double fBB, double fBB_prime, double dist);
   
   Int_t      SelectProtonsFromNeutrons(float distance_Z1, int layer);
   void       ComputeBBDistance(double edep, double tof, int tw_layer);
   
public:
   
   TATWparCal();
   virtual ~TATWparCal();
   
   // Calibration
   inline TATWcalibrationMap* getCalibrationMap() {return fMapCal;}
   //
   //! Read from file
   Bool_t          FromCalibFile(const TString& name = "", Bool_t isTofcal = false, Bool_t Corr = false);
   Bool_t          FromRateFile(const TString& name = "", Int_t initRun=-1, Int_t endRun=-1);
   Bool_t          FromElossTuningFile(const TString& name = "");
   Bool_t          FromFileZID(const TString& name = "", Int_t zbeam=-1);
   Bool_t          FromDeltaTimeFile(const TString& name = "");
   Bool_t          FromBarStatusFile(const TString& name = "");
   Bool_t          IsElossTuningON()  {return f_isElossTuningON;}
   Bool_t          IsPosCalibration() {return f_isPosCalibration;}
   Bool_t          IsBarCalibration() {return f_isBarCalibration;}
   Bool_t          IsTWbarActive(Int_t layer, Int_t bar);
   //
   //! Get Methods
   Int_t           GetChargeZ(Float_t edep, Float_t tof, Int_t layer); //const;
   Int_t           GetBisecChargeZ() const {return fZraw;}
   Float_t         GetDistBB(int ichg) const { return f_dist_Z[ichg-1];}
   // Get Light speed per bar in [cm/ns]
   Double_t        GetBarLightSpeed(Int_t ilayer, Int_t ibar);
   // Get offset per bar in [cm] for position from delta Time
   Double_t        GetDeltaTimePosOffset(Int_t ilayer, Int_t ibar);
   Double_t        GetElossThreshold(Int_t ilayer, Int_t ibar);

   TString         GetIonBeamName() { return fIonBeamName;}
   Double_t        GetTofMin()      { return (Int_t)fTof_min; }
   Double_t        GetTofMax()      { return (Int_t)fTof_max; }
   Double_t        GetMaxEloss()    { return (Int_t)fAvgBeamEnergyLossInBar; }
   TH1D*           GetRate()        { return fHisRate;}
   
   //! Set Methods
   void            SetBisecChargeZ(int chg) {fZraw = chg;}
   void            SetDistBB(int ichg, float dist) {f_dist_Z[ichg-1] = dist;}
   //
   //! Clear
   virtual void    Clear(Option_t* opt="");
   
   //! Stream output
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
   ClassDef(TATWparCal,1)
};

#endif
