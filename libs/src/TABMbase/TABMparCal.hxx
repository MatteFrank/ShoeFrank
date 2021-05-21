#ifndef _TABMparCal_HXX
#define _TABMparCal_HXX
/*!
  \file
  \version $Id: TABMparCal.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
  \brief   Declaration of TABMparCal.
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
                    TABMparCal();
    virtual         ~TABMparCal();

    //setters
    void SetResoFunc(TF1* inreso);
    void SetSTrelFunc(TF1* instrel);

    //getters
    TF1*  GetResoFunc()                 const{return fpResoFunc;};
    TF1*  GetSTrelFunc()                const{return fpSTrel;};

    //T0
    void        PrintT0s(TString , TString, Long64_t);
    void        SetT0s(vector<Float_t> t0s);
    void        SetT0(Int_t cha, Float_t t0in);
    Float_t     GetT0(Int_t view, Int_t plane, Int_t cell){return GetT0(cell+view*3+plane*6);};
    Float_t     GetT0(Int_t index_in){return (index_in<36 && index_in>-1) ? fT0Vec[index_in]:-1000;};
    void        CoutT0();
    void        ResetT0(){fill(fT0Vec.begin(), fT0Vec.end(),-10000);};

    //Adc pedestals
    void        PrintAdc(TString , TString, Long64_t);
    Bool_t      LoadAdc(TString inputname, TABMparMap *p_bmmap);
    void        ResetAdc(Int_t size);
    void        SetAdc(Int_t cha, Float_t ped, Float_t dev){fAdcPedVec.at(cha)=make_pair(ped,dev);};
    Float_t     GetAdcPed(Int_t cha){return fAdcPedVec.at(cha).first;};
    Float_t     GetAdcDevStd(Int_t cha){return fAdcPedVec.at(cha).second;};
    void        CoutAdc();

    //others
    Bool_t    FromFile(const TString& name);//read the calibration file
    Double_t  STrelEval(Float_t tdrift) {return fpSTrel->Eval(tdrift);};
    Float_t   ResoEval(Float_t dist){return (dist>0 && dist<0.8) ? fpResoFunc->Eval(dist) : 0.15;};
    Double_t  GetTimeFromRDrift(Double_t rdrift){return fpSTrel->GetX(rdrift);};
    void      ResetStrelFunc();
    void      PrintResoStrel(TString output_filename);

    //default
    virtual void    Clear(Option_t* opt="");
    virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
    TString         fkDefaultParName;  // default par file name

    ClassDef(TABMparCal,1)

  private:
    vector<Float_t>  fT0Vec;                   //T0 in ns
    vector<pair<Float_t,Float_t>>  fAdcPedVec;  //Adc pedestals, adc devstd
    TF1             *fpResoFunc;               //space resolution function
    TF1             *fpSTrel;                  //Space time relation
};

#endif
