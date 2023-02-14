#ifndef _TATWparConf_HXX
#define _TATWparConf_HXX
/*!
  \file   TATWparConf.hxx
  \brief   Declaration of TATWparConf, the Beam Monitor configuration class
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


//##############################################################################
class TATWparConf : public TAGparTools
{
  public:
                    TATWparConf();
    virtual         ~TATWparConf();

   //! Is Z from MC
   Bool_t    IsTWZmc()    const { return fEnableTWZmc;    }
   //! Is no pileup flag
   Bool_t    IsTWnoPU()   const { return fEnableTWnoPU;   }
   //! Is Z matching
   Bool_t    IsTWZmatch() const { return fEnableTWZmatch; }
   //! Is Calibration per bar
   Bool_t    IsTWCalBar() const { return fEnableTWCalBar; }
   //! Number of bars
   Int_t     GetBarsN()   const { return fBarsN;          }

   Bool_t    FromFile(const TString& name);

   void      Clear(Option_t* opt="");
   void      ToStream(ostream& os = cout, Option_t* option = "") const;

private:
   TString   fkDefaultParName;  ///< default parconf file name ("./config/TATWdetector.cfg")
   Bool_t    fEnableTWZmc;     ///< Enable Z from MC
   Bool_t    fEnableTWnoPU;    ///< Enable no pileup flag
   Bool_t    fEnableTWZmatch;  ///< Z matching
   Bool_t    fEnableTWCalBar;  ///< Calibration per bar
   Int_t     fBarsN;           ///< Number of bars

   ClassDef(TATWparConf,1)
};

#endif
