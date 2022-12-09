#ifndef _TAMSDactNtuRaw_HXX
#define _TAMSDactNtuRaw_HXX
/*!
  \file
  \version $Id: TAMSDactNtuRaw.hxx
  \brief   Declaration of TAMSDactNtuRaw.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"

#include "DEMSDEvent.hh"

#include "TAMSDparameters.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDparMap.hxx"
#include "TAMSDparCal.hxx"
#include "TAGdaqEvent.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAGactDaqReader.hxx"

#include "TH1F.h"

class TAMSDactNtuRaw : public TAGaction {

public:
    explicit        TAMSDactNtuRaw(const char* name=0,
                                   TAGdataDsc* dscdatraw=0,
                                   TAGdataDsc* dscdatdaq=0,
                                   TAGparaDsc* dscparmap=0,
                                   TAGparaDsc* dscparcal=0,
                                   TAGparaDsc* dscpargeo=0,
                                   TAGparaDsc* dscparcon=0);
    virtual         ~TAMSDactNtuRaw();

    virtual  void   CreateHistogram();
    virtual Bool_t  Action();

    ClassDef(TAMSDactNtuRaw,0)
   
public:
   //! Enable pedestal subtraction
   static void EnablePedSub()  { fgPedestalSub = true;    }
   //! Disable pedestal subtraction
   static void DisablePedSub() { fgPedestalSub = false;   }
   //! Enable common mode subtraction
   static void EnableCNSub()   { fgCommonModeSub = true;  }
   //! Disable common mode subtraction
   static void DisableCNSub()  { fgCommonModeSub = false; }
   //! Enable pedestal run
   static void EnablePedRun()  { fgPedestal = true;       }
   //! Disable pedestal run
   static void DisablePedRun() { fgPedestal = false;      }

private:
    TAGdataDsc*     fpDatRaw;		        ///< output data dsc
    TAGdataDsc*     fpDatDaq;		        ///< input data dsc
    TAGparaDsc*     fpParMap;            ///< mapping parameter dsc
    TAGparaDsc*     fpParCal;            ///< calibration parameter dsc
    TAGparaDsc*     fpParGeo;            ///< geometry parameter dsc
    TAGparaDsc*     fpParConf;           ///< configuration parameter dsc

    TH1F*           fpHisSeedMap[MaxPlane];    ///< seed map
    TH1F*           fpHisStripMap[MaxPlane];   ///< strip map
    TH1F*           fpHisCommonMode[MaxPlane]; ///< commom mode
   
private:
    Bool_t   DecodeHits(const DEMSDEvent* evt);
    Double_t ComputeCN(Int_t strip, Double_t *VaContent, Int_t type);

private:
   static UInt_t fkgThreshold;           ///< charge threshold
   static Bool_t fgPedestalSub;          ///< pedestal substraction flag
   static Bool_t fgCommonModeSub;        ///< common noise substraction flag
   static Bool_t fgPedestal;             ///< Flag for pedestal runs
};

#endif
