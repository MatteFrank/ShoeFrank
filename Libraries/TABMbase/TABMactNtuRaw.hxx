#ifndef _TABMactNtuRaw_HXX
#define _TABMactNtuRaw_HXX
/*!
  \file    TABMactNtuRaw.hxx
  \brief   Declaration of TABMactNtuRaw, the class that convert the FOOT DAQ input to create a BM raw hit object (TABMntuRaw)
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAWDntuTrigger.hxx"
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
                                  TAGdataDsc* dsctimraw=0,
                                  TAGdataDsc* dsctrigraw=0);
    virtual         ~TABMactNtuRaw();

    virtual  void   CreateHistogram();
    virtual Bool_t  Action();
    void EvaluateT0time();

  private:
    TAGdataDsc*     fpDatRaw;		        ///< output BM raw data dsc (TABMntuRaw)
    TAGdataDsc*     fpDatDaq;		        ///< input DAQ data dsc (TAGdaqEvent)
    TAGdataDsc*     fpTimRaw;		        ///< input Start Counter data dsc (TASTntuHit)
    TAGdataDsc*     fpTrigRaw;	        ///< input Trigger data dsc (TAWDntuTrigger)
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
    std::vector<TH1F*> fpRawTdcFragLessSync;  ///< vector of tdc channel meas - used_trigger for fragmentation events

    //channel 1 trigger plots
    TH1F*            fpRawCh1NoTrig;    ///< BM channel 1 without trigger subtraction

    TH1F*            fpRawCh1LessTdcTr; ///< BM channel 1 - TDC Trigger time for all the events
    TH1F*            fpRawCh1LessSTFit; ///< BM channel 1 - TDC trigger - STfit time for all events
    TH1F*            fpRawCh1PlusSTFit; ///< BM channel 1 - TDC trigger + STfit time for all events
    TH1F*            fpRawCh1LessMagorTr; ///< BM channel 1 - TDC trigger - Daq trigger for all events
    TH1F*            fpRawCh1LessSTFitLessMagor; ///< BM channel 1 - Tdc trigger - STfit - Daq trigger for all events
    TH1F*            fpRawCh1PlusSTFitLessMagor; ///< BM channel 1 - Tdc trigger + STfit - Daq trigger for all events

    TH1F*            fpRawCh1LessTdcTrFrag; ///< BM channel 1 - TDC Trigger time for fragmentation trigger events
    TH1F*            fpRawCh1LessSTFitFrag; ///< BM channel 1 - TDC trigger - STfit time for fragmentation trigger events
    TH1F*            fpRawCh1PlusSTFitFrag; ///< BM channel 1 - TDC trigger + STfit time for fragmentation trigger events
    TH1F*            fpRawCh1LessMagorTrFrag; ///< BM channel 1 - TDC trigger - Daq trigger for fragmentation trigger events
    TH1F*            fpRawCh1LessSTFitLessMagorFrag; ///< BM channel 1 - Tdc trigger - STfit - Daq trigger for fragmentation trigger events
    TH1F*            fpRawCh1PlusSTFitLessMagorFrag; ///< BM channel 1 - Tdc trigger + STfit - Daq trigger for fragmentation trigger events

    TH1F*            fpRawCh1LessTdcTrMaj; ///< BM channel 1 - TDC Trigger time for Majority events
    TH1F*            fpRawCh1LessSTFitMaj; ///< BM channel 1 - TDC trigger - STfit time for Majority events
    TH1F*            fpRawCh1PlusSTFitMaj; ///< BM channel 1 - TDC trigger + STfit time for Majority events
    TH1F*            fpRawCh1LessMagorTrMaj; ///< BM channel 1 - TDC trigger - Daq trigger for Majority events
    TH1F*            fpRawCh1LessSTFitLessMagorMaj; ///< BM channel 1 - Tdc trigger - STfit - Daq trigger for Majority events
    TH1F*            fpRawCh1PlusSTFitLessMagorMaj; ///< BM channel 1 - Tdc trigger + STfit - Daq trigger for Majority events

   ClassDef(TABMactNtuRaw,0)
};

#endif
