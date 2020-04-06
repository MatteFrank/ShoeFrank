#ifndef _TABMrawHit_HXX
#define _TABMrawHit_HXX

#include "TAGdata.hxx"

#include <vector>
#include <string.h>
#include <fstream>
#include <bitset>
#include <algorithm>

#include "TString.h"

using namespace std;

class TABMrawHit {
  public:
    TABMrawHit();
    virtual         ~TABMrawHit();

    void            SetData(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time);
    Int_t           GetCell() const;
    Int_t           GetPlane() const;
    Int_t           GetView() const;
    Double_t        GetTime() const;
    Int_t           GetIdcell() const;

    ClassDef(TABMrawHit,1)

  private:
    Int_t    fiView;    
    Int_t    fiLayer;
    Int_t    fiCell;
    Int_t    fidCell;
    Double_t ftdTtime;//ns
};

#include "TABMrawHit.icc"

#endif
