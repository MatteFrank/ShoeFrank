#ifndef _TABMactNtuRaw_HXX
#define _TABMactNtuRaw_HXX
/*!
  \file    TABMactNtuRaw.hxx
  \brief   Declaration of TABMactNtuRaw, the class that convert the FOOT DAQ input to create a BM raw hit object (TABMntuRaw)
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

  private:
    TAGdataDsc*     fpDatRaw;		        ///< output BM raw data dsc (TABMntuRaw)
    TAGdataDsc*     fpDatDaq;		        ///< input DAQ data dsc (TAGdaqEvent)
    TAGdataDsc*     fpTimRaw;		        ///< input Start Counter data dsc (TASTntuHit)
    TAGparaDsc*     fpParMap;		        ///< TABMparMap parameter dsc
    TAGparaDsc*     fpParCal;		        ///< TABMparCal parameter dsc
    TAGparaDsc*     fpParGeo;		        ///< TABMparGeo parameter dsc

    Bool_t DecodeHits(const TDCEvent* evt, const double sttrigger);

    //histos
    TH1I*            fpRawTdcChannel;      ///< tdc channel distribution
    TH1F*            fpRawTrigTrigger;     ///< Used Trigger time
    TH1F*            fpRawSTFitTrigger;    ///< Start Counter post analysis fitted trigger Time
    TH1F*            fpRawMagorTrigger;    ///< Daq trigger Time
    TH1F*            fpRawTdcTrigger;      ///< Tdc trigger Time

    TH1F*            fpRawbmTdcLessSTFITLessMagorTrigger;   ///< Tdc - ST - Majority trigger time
    TH1F*            fpRawbmTdcLessSTFITPlusMagorTrigger;   ///< Tdc - ST + Majority trigger time
    TH1F*            fpRawbmTdcPlusSTFITPlusMagorTrigger;   ///< Tdc + ST + Majority trigger time
    TH1F*            fpRawbmTdcPlusSTFITLessMagorTrigger;   ///< Tdc + ST - Majority trigger time
    TH1F*            fpRawbmTdcLessSTFITTrigger;   ///< Tdc - ST trigger time
    TH1F*            fpRawbmTdcPlusSTFITTrigger;   ///< Tdc + trigger time

    TH1I*            fpRawMagorDouble;    ///< Number of majority signals in the +- 10 ns window
    std::vector<TH1F*> fpRawTdcMeas;      ///< vector of tdc channel raw measurements
    std::vector<TH1F*> fpRawTdcLessSync;  ///< vector of tdc channel meas - used_trigger

    //channel 1 trigger plots
    TH1F*            fpRawCh1NoTrig;    ///< BM channel 1 without trigger subtraction
    TH1F*            fpRawCh1LessTdcTr; ///< BM channel 1 - TDC Trigger time
    TH1F*            fpRawCh1LessSTFit; ///< BM channel 1 - TDC trigger - STfit time
    TH1F*            fpRawCh1PlusSTFit; ///< BM channel 1 - TDC trigger + STfit time
    TH1F*            fpRawCh1LessMagorTr; ///< BM channel 1 - TDC trigger - Daq trigger
    TH1F*            fpRawCh1LessSTFitLessMagor; ///< BM channel 1 - Tdc trigger - STfit - Daq trigger
    TH1F*            fpRawCh1PlusSTFitLessMagor; ///< BM channel 1 - Tdc trigger + STfit - Daq trigger

   ClassDef(TABMactNtuRaw,0)
};

#endif
