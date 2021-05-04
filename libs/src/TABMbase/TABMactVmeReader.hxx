#ifndef _TABMactVmeReader_HXX
#define _TABMactVmeReader_HXX
/*!
  \file
  \version $Id: TABMactVmeReader.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TABMactVmeReader.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "BM_struct.h"
#include "TH2.h"
#include <fstream>

class TABMactVmeReader : public TAGaction {
  public:

    explicit        TABMactVmeReader(const char* name=0,
                                  TAGdataDsc* dscdatraw=0,
                                  TAGparaDsc* dscbmmap=0,
                                  TAGparaDsc* dscbmcal=0,
                                  TAGparaDsc* dscbmgeo=0);
                                  // TAGdataDsc* p_timraw=0);
    virtual         ~TABMactVmeReader();
    virtual Int_t   Open(const TString& name);
    virtual void    Close();
    virtual Bool_t  Process();
    virtual  void   CreateHistogram();
    //~ virtual Bool_t  Action();

    //used in CalibrateBm
    void EvaluateT0time();
    void EvaluateAdcPedestals();

    //setters
    void SetAdcSize(Int_t adc){adc792.resize(adc,make_pair(-1000,-1000));return;};
    void SetScaSize(Int_t sca){sca830.resize(sca,make_pair(0,0));return;};

    //getters
    BM_struct*  GetEvtStruct(){return fpEvtStruct;};
    Int_t  GetSizeTdcEv(){return tdc_event.size();};
    Int_t  GetSizeTdcSync(){return tdc_sync.size();};
    Int_t  GetSizeTdcMeas(){return tdc_meas.size();};
    Int_t  GetSizeSca(){return sca830.size();};
    Int_t  GetSizeAdc(){return adc792.size();};
    pair<Int_t,Int_t>  GetTdcEv(Int_t i){return tdc_event.at(i);};
    Int_t  GetTdcSync(Int_t i){return tdc_sync.at(i);};
    pair<Int_t,Int_t>  GetTdcMeas(Int_t i){return tdc_meas.at(i);};
    pair<Int_t,Int_t>  GetScaMeas(Int_t i){return sca830.at(i);};
    pair<Int_t,Int_t>  GetAdcMeas(Int_t i){return adc792.at(i);};

    vector<TH1F*> GetRawTdc(){return fpRawTdcMeas;};
    vector<TH1F*> GetTdcLessSync(){return fpRawTdcLessSync;};
    vector<TH1I*> GetRawSca(){return fpRawScaMeas;};
    vector<TH1I*> GetRawAdc(){return fpRawAdc;};
    vector<TH1F*> GetAdcLessPed(){return fpAdcLessPed;};

    //BM standalone reader methods
    void ClearBmstruct(Bool_t forced);
    Bool_t ReadEvent();
    void MonitorQDC(vector<Int_t>& adc792_words);
    void PrintBMstruct();

    ClassDef(TABMactVmeReader,0)

  private:
    TAGdataDsc*     fpDatRaw;		    // output data dsc
    // TAGdataDsc*     fpTimRaw;		    // output data dsc
    TAGparaDsc*     fpParMap;		    // parameter dsc
    TAGparaDsc*     fpParCal;		    // parameter dsc
    TAGparaDsc*     fpParGeo;		    // parameter dsc
    ifstream        fbmfile;        //bm raw file

    //data reading structure
    BM_struct*      fpEvtStruct;               //integer t0values
    vector<pair<Int_t,Int_t>>   tdc_event;     //first=total number of hits of the tdc, second=tdc event number
    vector<Int_t>   tdc_sync;                  //time of the sync channel (10^-8 sec.)
    vector<pair<Int_t,Int_t>>   tdc_meas;      //first=tdc channel, second=measurement value (10^-8 sec.)
    vector<pair<Int_t,Int_t>>   sca830;        //first=scaler measurements defined as new count - old count, second=scaler counts
    vector<pair<Int_t,Int_t>>   adc792;        //ADC measurement for each channel, second=ADC overflow cannel, -1000=not set, 1=overflow, 0=ok

    Long64_t fDataNumEv;            //current number of events
    Long64_t fDataSyncNumEv;        //current number of events + number of sync

    //histos
    //BM struct histos
    TH1I*            fpTotStatus;
    TH1I*            fpTdcStatus;
    TH1I*            fpScaStatus;
    TH1I*            fpScaNegative;    //Sometime the scaler can wrongly read a negative measurement
    TH1I*            fpAdcStatus;
    TH1F*            fpTimeEvtoev;
    TH1I*            fpTimeRead;

    TH1I*            fpRawError;        //BM hit channel error
    TH1I*            fpRawTrigTime;     //Trigger time
    TH1I*            fpRawTdcChannel;   //TDC signal distribution
    TH1I*            fpRawAdcAccDisc;   //Adc overflow check
    vector<TH1F*> fpRawTdcMeas;         //vector of tdc channel raw measurements
    vector<TH1F*> fpRawTdcLessSync;     //vector of tdc channel meas -  sync meas
    vector<TH1I*> fpRawScaMeas;         //vector of scaler channel measurements
    vector<TH1I*> fpRawAdc;             //vector of adc  channel measurements
    vector<TH1F*> fpAdcLessPed;         //vector of adc  channel - pedestals
};

#endif
