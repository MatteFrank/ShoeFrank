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
#include "GlobalPar.hxx"


//##############################################################################
class TABMparCal : public TAGparTools {
  public:
                    TABMparCal();
    virtual         ~TABMparCal();

    //setters
    void SetT0choice(Int_t in)                {fT0Choice=in;};
    void SetResoFunc(TF1* inreso);
    void SetSTrelFunc(TF1* instrel);

    //getters
    Int_t GetT0Choice()                 const{return fT0Choice;};
    TF1*  GetResoFunc()                 const{return fpResoFunc;};
    TF1*  GetSTrelFunc()                const{return fpSTrel;};

    //T0
    void        PrintT0s(TString , TString, Long64_t);
    void        SetT0s(vector<Float_t> t0s);
    void        SetT0(Int_t cha, Float_t t0in);
    Float_t     GetT0(Int_t view, Int_t plane, Int_t cell){return GetT0(cell+view*3+plane*6);};
    Float_t     GetT0(Int_t index_in){return (index_in<36 && index_in>-1) ? fT0Vec[index_in]:-1000;};
    void        CoutT0();


    //others
    Bool_t    FromFile(const TString& name);//read the calibration file
    Double_t  STrelEval(Float_t tdrift) {return fpSTrel->Eval(tdrift);};
    Float_t   ResoEval(Float_t dist){return (dist>0 && dist<0.8) ? fpResoFunc->Eval(dist) : 0.15;};
    Double_t  GetTimeFromRDrift(Double_t rdrift);
    void      ResetStrelFunc();
    //default
    virtual void    Clear(Option_t* opt="");
    virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
    TString         fkDefaultParName;  // default par file name

    ClassDef(TABMparCal,1)

  private:

    Int_t            fT0Choice;          //0=meas-internal, 1=meas-st, 2=meas-internal-st, 3=meas-internal+st
    vector<Float_t>  fT0Vec;             //T0 in ns
    TF1             *fpResoFunc;         //space resolution function
    TF1             *fpSTrel;            //Space time relation
};

#endif
