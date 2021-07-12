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
    TH1F*            fpRawTrigTrigger;     //Used Trigger time
    TH1F*            fpRawSTFitTrigger;    //Start Counter post analysis fitted trigger Time
    TH1F*            fpRawDaqTrigger;      //Daq trigger Time
    TH1F*            fpRawTdcTrigger;   //Tdc trigger Time
    std::vector<TH1F*> fpRawTdcMeas;    //vector of tdc channel raw measurements
    std::vector<TH1F*> fpRawTdcLessSync;//vector of tdc channel meas - used_trigger

    //channel 1 trigger plots
    TH1F*            fpRawCh1NoTrig;    //BM channel 1 without trigger subtraction
    TH1F*            fpRawCh1LessTdcTr; //BM channel 1 - TDC Trigger time
    TH1F*            fpRawCh1LessSTFit; //BM channel 1 - TDC trigger - STfit time
    TH1F*            fpRawCh1PlusSTFit; //BM channel 1 - TDC trigger + STfit time
    TH1F*            fpRawCh1LessDaqTr; //BM channel 1 - TDC trigger - Daq trigger
    TH1F*            fpRawCh1LessSTFitLessDaq; //BM channel 1 - Tdc trigger - STfit - Daq trigger
    TH1F*            fpRawCh1PlusSTFitLessDaq; //BM channel 1 - Tdc trigger + STfit - Daq trigger
};

#endif
