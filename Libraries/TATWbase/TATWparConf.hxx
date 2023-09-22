#ifndef _TATWparConf_HXX
#define _TATWparConf_HXX
/*!
  \file   TATWparConf.hxx
  \brief   Declaration of TATWparConf, the Tof-Wall configuration class
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
   Bool_t    IsZmc()         const { return fEnableZmc;         }
   //! Is no pileup flag
   Bool_t    IsNoPileUp()    const { return fEnableNoPileUp;    }
   //! Is Z matching
   Bool_t    IsZmatching()   const { return fEnableZmatching;   }
   //! Is Calibration per bar
   Bool_t    IsCalibBar()    const { return fEnableCalibBar;    }
   //! Is rate smearing in MC
   Bool_t    IsRateSmearMc() const { return fEnableRateSmearMc; }
   //! Number of bars
   Int_t     GetBarsN()      const { return fBarsN;             }

   Bool_t    FromFile(const TString& name);

   void      Clear(Option_t* opt="");
   void      ToStream(ostream& os = cout, Option_t* option = "") const;

private:
   TString   fkDefaultParName;   ///< default parconf file name ("./config/TATWdetector.cfg")
   Bool_t    fEnableZmc;         ///< Enable Z from MC
   Bool_t    fEnableNoPileUp;    ///< Enable no pileup flag
   Bool_t    fEnableZmatching;   ///< Z matching
   Bool_t    fEnableCalibBar;    ///< Calibration per bar
   Bool_t    fEnableRateSmearMc; ///< MC rate smearing
   Int_t     fBarsN;             ///< Number of bars

   ClassDef(TATWparConf,1)
};

#endif
