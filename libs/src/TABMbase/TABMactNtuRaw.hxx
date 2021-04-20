#ifndef _TABMactNtuRaw_HXX
#define _TABMactNtuRaw_HXX
/*!
  \file
  \version $Id: TABMactNtuRaw.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TABMactNtuRaw.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TDCEvent.hh"
#include "TABMparMap.hxx"
#include "TABMparCal.hxx"
#include "TAGdaqEvent.hxx"
#include "TABMntuRaw.hxx"
#include "TABMrawHit.hxx"
#include "TASTntuHit.hxx"
#include "TAGactDaqReader.hxx"

#include "TH2.h"
#include <fstream>
#include <iomanip>


class TDCEvent;

class TABMactNtuRaw : public TAGaction {
  public:

    explicit        TABMactNtuRaw(const char* name=0,
                                  TAGdataDsc* dscdatraw=0,
                                  TAGdataDsc* dscdatdaq=0,
                                  TAGparaDsc* dscparmap=0,
                                  TAGparaDsc* dscparcal=0,
                                  TAGparaDsc* dscpargeo=0,
                                  TAGdataDsc* dsctimraw=0);
    virtual         ~TABMactNtuRaw();

    virtual  void   CreateHistogram();
    virtual Bool_t  Action();
    void EvaluateT0time();

    ClassDef(TABMactNtuRaw,0)

  private:
    TAGdataDsc*     fpDatRaw;		        // output data dsc
    TAGdataDsc*     fpDatDaq;		        // input data dsc
    TAGdataDsc*     fpTimRaw;		        // input data dsc
    TAGparaDsc*     fpParMap;		        // parameter dsc
    TAGparaDsc*     fpParCal;		        // parameter dsc
    TAGparaDsc*     fpParGeo;		        // parameter dsc

    Bool_t DecodeHits(const TDCEvent* evt, const double sttrigger);

    //histos
    TH1I*            fpRawTdcChannel;   //tdc channel distribution
    TH1I*            fpRawTrigTime;     //Used Trigger time
    TH1I*            fpRawSTFitTime;    //Start Counter post analysis fitted trigger Time
    TH1I*            fpRawSTTdcTime;    //Start Counter Time from TDC
    TH1I*            fpRawDAQTdcTime;   //DAQ trigger time from TDC
    std::vector<TH1F*> fpRawTdcMeas;    //vector of tdc channel raw measurements
    std::vector<TH1F*> fpRawTdcLessSync;//vector of tdc channel meas - sync meas

};

#endif
