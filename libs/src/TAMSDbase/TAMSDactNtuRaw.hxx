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
   static void EnablePedSub()  { fgPedestalSub = true;  }
   static void DisablePedSub() { fgPedestalSub = false; }
   static void EnableCNSub()  { fgCommonModeSub = true;  }
   static void DisableCNSub() { fgCommonModeSub = false; }

private:
    TAGdataDsc*     fpDatRaw;		        // output data dsc
    TAGdataDsc*     fpDatDaq;		        // input data dsc
    TAGparaDsc*     fpParMap;            // parameter dsc
    TAGparaDsc*     fpParCal;            // parameter dsc
    TAGparaDsc*     fpParGeo;            // parameter dsc
    TAGparaDsc*     fpParConf;           // parameter dsc

    TH1F*           fpHisSeedMap[16];
    TH1F*           fpHisStripMap[16];
    TH1F*           fpHisCommonMode[16];
   
private:
    Bool_t DecodeHits(const DEMSDEvent* evt);
    Double_t ComputeCN(Int_t strip, Double_t *VaContent, Int_t type);

private:
   static UInt_t fkgThreshold;
   static Bool_t fgPedestalSub;
   static Bool_t fgCommonModeSub;
};

#endif
