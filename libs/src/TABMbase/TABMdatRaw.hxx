#ifndef _TABMdatRaw_HXX
#define _TABMdatRaw_HXX
/*!
  \file
  \version $Id: TABMdatRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TABMdatRaw.
*/

#include "TAGdata.hxx"
#include "TABMrawHit.hxx"

#include <vector>
#include <string.h>
#include <fstream>
#include <bitset>
#include <algorithm>

#include "TString.h"

using namespace std; 

class TABMdatRaw : public TAGdata {
  public:

                    TABMdatRaw();
    virtual         ~TABMdatRaw();

    void            SetHitData(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time);
    void            SetCounter(Int_t i_ntdc, Int_t i_ndrop);
    void            AddDischarged();
    static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
    Double_t        GetTrigtime(){return trigtime;};
    void            SetTrigtime(Double_t trigin){trigtime=trigin;};
  
    Int_t           NHit() const;
    const TABMrawHit& Hit(Int_t i_ind) const;

    //~ Int_t           NTdc() const;
    Int_t           NDrop() const;

    virtual void    Clear(Option_t* opt="");

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    ClassDef(TABMdatRaw,1)

  private:
     static TString fgkBranchName;    // Branch name in TTree
    //~ Int_t           fiNTdc;		      //number of accepted tdc values
    Int_t           fiNDrop;		    //number of discharged tdc values
    vector<TABMrawHit> fHitList;		//list of TABMrawHit ATTENZIONE!! NON È UN TCLONESARRAY COME GLI ALTRI!!!
    Double_t        trigtime;
};

#include "TABMdatRaw.icc"

#endif
