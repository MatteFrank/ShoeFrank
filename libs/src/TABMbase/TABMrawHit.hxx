#ifndef _TABMrawHit_HXX
#define _TABMrawHit_HXX
/*!
  \file    TABMrawHit.hxx
  \brief   Declaration of TABMrawHit, the class for the BM raw hits
*/
/*------------------------------------------+---------------------------------*/
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
    TABMrawHit(Int_t id, Int_t plane, Int_t view, Int_t cell, Double_t time);
    virtual         ~TABMrawHit();

    void            SetData(Int_t id, Int_t plane, Int_t view, Int_t cell, Double_t time);

    //! Returns cell number [0-2]
    Int_t           GetCell()   const { return fiCell;   }
    //! Returns plane number [0-5]
    Int_t           GetPlane()  const { return fiPlane;  }
    //! Returns view number [0-1]
    Int_t           GetView()   const { return fiView;   }
    //! Returns time (ns)
    Double_t        GetTime()   const { return ftdTtime; }
    //! Returns idCell number [0-35]
    Int_t           GetIdCell() const { return fidCell;  }

  private:
    Int_t    fiView;        ///< Hit view [0-1]
    Int_t    fiPlane;       ///< Hit plane [0-5]
    Int_t    fiCell;        ///< Hit cell [0-2]
    Int_t    fidCell;       ///< Hit idcell [0-35]
    Double_t ftdTtime;      ///< Hit raw tdc time measurement in ns

   ClassDef(TABMrawHit,1)
};

#endif
