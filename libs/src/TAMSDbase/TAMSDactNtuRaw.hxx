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
#include "TAMSDparMap.hxx"
#include "TAMSDparCal.hxx"
#include "TAGdaqEvent.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAGactDaqReader.hxx"

#include "TH2.h"

class TAMSDactNtuRaw : public TAGaction {

public:
    explicit        TAMSDactNtuRaw(const char* name=0,
                                   TAGdataDsc* dscdatraw=0,
                                   TAGdataDsc* dscdatdaq=0,
                                   TAGparaDsc* dscparmap=0,
                                   TAGparaDsc* dscparcal=0,
                                   TAGparaDsc* dscpargeo=0);
    virtual         ~TAMSDactNtuRaw();

    virtual  void   CreateHistogram();
    virtual Bool_t  Action();

    ClassDef(TAMSDactNtuRaw,0)

private:
    TAGdataDsc*     fpDatRaw;		        // output data dsc
    TAGdataDsc*     fpDatDaq;		        // input data dsc
    TAGparaDsc*     fpParMap;		        // parameter dsc
    TAGparaDsc*     fpParCal;		        // parameter dsc
    TAGparaDsc*     fpParGeo;		        // parameter dsc

private:
    Bool_t DecodeHits(const DEMSDEvent* evt);
};

#endif
