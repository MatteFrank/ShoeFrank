#ifndef _TABMparCal_HXX
#define _TABMparCal_HXX
/*!
  \file TABMparCal.hxx
  \brief Declaration of TABMparCal, the BM calibration class
*/
/*------------------------------------------+---------------------------------*/

#include "TString.h"
#include "TVector3.h"
#include "TSpline.h"
#include "TF1.h"
#include "TH1.h"

#include <vector>

#include "TAGpara.hxx"
#include "TAGparTools.hxx"
#include "TAGrecoManager.hxx"
#include "TABMparMap.hxx"

//##############################################################################

class TABMparCal : public TAGparTools {

  public:
    //default functions
                    TABMparCal();
    virtual         ~TABMparCal();
    virtual void    Clear(Option_t* opt="");
    virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;

    //getters
    //! Get the BM resolution function expressed as resolution [cm] as a function of drift distance [cm]
    TF1*  GetResoFunc()                 const{return fpResoFunc;};
    //! Get the BM space time relations expressed as space [cm] as a function of drift time [ns]
    TF1*  GetSTrelFunc()                const{return fpSTrel;};
    //! Get the drift distance [cm] given the drift time [tdrift in ns]
    Double_t  STrelEval(Float_t tdrift) {return fpSTrel->Eval(tdrift);};
    //! Get the detector resolution [cm] given the drift distance [dist in cm]
    Float_t   ResoEval(Float_t dist){return (dist>0 && dist<0.8) ? fpResoFunc->Eval(dist) : 0.15;};

    //Setters
    void SetResoFunc(TF1* inreso);
    void SetSTrelFunc(TF1* instrel);

    //T0 functions
    void        PrintT0s(TString , TString, Long64_t);
    void        SetT0s(vector<Float_t> t0s);
    void        SetT0(Int_t cha, Float_t t0in);
    Float_t     GetT0(Int_t view, Int_t plane, Int_t cell){return GetT0(cell+view*3+plane*6);};
    Float_t     GetT0(Int_t index_in){return (index_in<36 && index_in>-1) ? fT0Vec[index_in]:-1000;};
    void        CoutT0();
    void        ResetT0(){fill(fT0Vec.begin(), fT0Vec.end(),-10000);};

    //ADC functions
    void        PrintAdc(TString , TString, Long64_t);
    Bool_t      LoadAdc(TString inputname, TABMparMap *p_bmmap);
    void        ResetAdc(Int_t size);
    void        SetAdc(Int_t cha, Float_t ped, Float_t dev){fAdcPedVec.at(cha)=make_pair(ped,dev);};
    Float_t     GetAdcPed(Int_t cha){return fAdcPedVec.at(cha).first;};
    Float_t     GetAdcDevStd(Int_t cha){return fAdcPedVec.at(cha).second;};
    void        CoutAdc();

    //Other functions
    Bool_t    FromFile(const TString& name);//read the calibration file
    Double_t  GetTimeFromRDrift(Double_t rdrift){return fpSTrel->GetX(rdrift);};
    void      ResetStrelFunc();
    void      PrintResoStrel(TString output_filename);

    TString         fkDefaultParName;  ///< default par file name

  private:
    vector<Float_t>  fT0Vec;                    ///< vector of Beam Monitor T0 [ns], the vector position corresponds to the cellid index [0-35]
    vector<pair<Float_t,Float_t>>  fAdcPedVec;  ///< vector of ADC pedestal values (first) and devstd (second)
    TF1             *fpResoFunc;                ///< Space resolution [cm] function as a function of drift distance [cm]
    TF1             *fpSTrel;                   ///< Space time relation fuction expressed as space [cm] as a function of drift time [ns]

   ClassDef(TABMparCal,1)
};

#endif
