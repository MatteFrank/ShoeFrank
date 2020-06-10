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

class TABMrawHit : public TAGdata {
  public:
    TABMrawHit();
    TABMrawHit(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time);
    virtual         ~TABMrawHit();

    void            SetData(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time);
   
    //! Returns cell number.
    Int_t           GetCell()   const { return fiCell;   }
    //! Returns plane number.
    Int_t           GetPlane()  const { return fiLayer;  }
    //! Returns view number.
    Int_t           GetView()   const { return fiView;   }
    //! Returns time.
    Double_t        GetTime()   const { return ftdTtime; }
    //! Returns Cell id number [0-35]
    Int_t           GetIdcell() const { return fidCell;  }
   
  private:
    Int_t    fiView;    
    Int_t    fiLayer;
    Int_t    fiCell;
    Int_t    fidCell;
    Double_t ftdTtime;//ns
   
   ClassDef(TABMrawHit,1)
};

#endif
