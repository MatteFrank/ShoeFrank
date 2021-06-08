#ifndef _TAMSDactNtuRaw_HXX
#define _TAMSDactNtuRaw_HXX
/*!
  \file
  \version $Id: TAMSDactNtuRaw.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TAMSDactNtuRaw.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "DEMSDEvent.hh"
#include "TAMSDparMap.hxx"
#include "TAMSDparCal.hxx"
#include "TAGdaqEvent.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAGactDaqReader.hxx"

#include "TH2.h"
#include <fstream>
#include <iomanip>


class DEMSDEvent;

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

    Bool_t DecodeHits(const DEMSDEvent* evt);

};

#endif
