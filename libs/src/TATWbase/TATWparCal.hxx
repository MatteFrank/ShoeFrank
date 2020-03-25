#ifndef _TATWparCal_HXX
#define _TATWparCal_HXX
/*!
  \file
  \version $Id: TATWparCal.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
  \brief   Declaration of TATWparCal.
*/
/*------------------------------------------+---------------------------------*/
#include <vector>

#include "Riostream.h"

#include "TObject.h"
#include "TArrayF.h"
#include "TF1.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "TAGparTools.hxx"
#include "TAGparGeo.hxx"
#include "TAGgeoTrafo.hxx"

#include "CCalibrationMap.hxx"


//##############################################################################

class TATWparCal : public TAGparTools {
      
public:

  enum{kLayers=2,kSlats=20};  // TW quantities
  
private:

  struct ChargeParameter_t : public  TObject {
    vector<Int_t>   Layer;     // TW layer
    vector<Int_t>   Charge;    // charge
    vector<Float_t> Norm_BB;   // par0 BB
    vector<Float_t> Const_BB;  // par1 BB
    vector<Float_t> CutLow;    // Lower Tof cut
    vector<Float_t> CutUp;     // Upper Tof cut
  };
   

  ChargeParameter_t fChargeParameter;
  
  static TString fgkBBparamName;    // default BBparameters for Z identification with TW
  static TString fgkDefaultCalName; // default detector charge calibration file
  CCalibrationMap *cMapCal;
  
  TAGparGeo*      m_parGeo;
  TAGgeoTrafo*    m_geoTrafo;
  
  Int_t      Z_beam;
  TString    ion_name;
  Int_t      A_beam;
  Float_t    kinE_beam;
  
  Float_t    z_SC;
  Float_t    z_TW;
  Float_t    Lmin;  
  Double_t   C_speed;
  Double_t   Tof_min;
  
  TDatabasePDG db;
  Double_t     mass_p;
  Double_t     mass_n;
  
  Double_t binding_energy; //MeV/u
  enum {H=1,He4=4,C12=12,O16=16};
  
  Double_t Mass_beam;
  Double_t Energy_beam;
  Double_t Beta_beam;
  Double_t Tof_beam;
  Double_t Tof_max;
  
  void       RetrieveBeamQuantities();

  Double_t   fBBparametrized(double tof, int chg, double par0, double par1);
  Double_t   fBB_prime_parametrized(double tof, int chg, double par0, double par1);
  Float_t    fDist(double tof, double eloss, double x, double fBB);
  Double_t   fDistPrime(double tof, double eloss, double x, double fBB, double fBB_prime, double dist);
  
  int     Zraw;
  float   dist_min_Z;

  vector<float> dist_Z;

public:

  TATWparCal();
  virtual ~TATWparCal();
  
  // Calibration
  inline CCalibrationMap* getCalibrationMap() {return cMapCal;}
  //
  //! Read from file
  Bool_t          FromFile(const TString& name = "");
  Bool_t          FromFile( Int_t isZbeam, const TString& name = "");

  void            ComputeBBDistance(double edep, double tof, int tw_layer);
  //
  //! Get Methods
  Int_t           GetChargeZ(Float_t edep, Float_t tof, Int_t layer); //const;
  Int_t           GetBisecChargeZ() const {return Zraw;}
  Float_t         GetDistBB(int ichg) const { return dist_Z[ichg-1];}
  //
  //! Set Methods
  void            SetBisecChargeZ(int chg) {Zraw = chg;}
  void            SetDistBB(int ichg, float dist) {dist_Z[ichg-1] = dist;}
  //
  //! Clear
  virtual void       Clear(Option_t* opt="");

  //! Stream output
  virtual void       ToStream(ostream& os = cout, Option_t* option = "") const;

  //   //! Get charge parameter
  //   ChargeParameter_t& GetChargePar(Int_t idx) {return fChargeParameter[idx];}
  //
  //   Int_t              GetCharge(Int_t idx) const { return fChargeParameter[idx].Charge; }
  //   Float_t            GetCutLow(Int_t idx) const { return fChargeParameter[idx].CutLow; }
  //   Float_t            GetCutUp(Int_t idx)  const { return fChargeParameter[idx].CutUp; }

public:
  static const Char_t* GetDefaultCalName()      { return fgkDefaultCalName.Data(); }

  ClassDef(TATWparCal,1)
};

#endif
