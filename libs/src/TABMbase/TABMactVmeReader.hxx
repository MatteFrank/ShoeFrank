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

    void EvaluateT0time();

    //BM standalone reader methods
    void ClearBmstruct(Bool_t forced);
    Bool_t ReadEvent(Bool_t evt0);
    void PrintBMstruct();
    void MonitorQDC(vector<Int_t>& adc792_words);



    ClassDef(TABMactVmeReader,0)

  private:
    TAGdataDsc*     fpDatRaw;		    // output data dsc
    // TAGdataDsc*     fpTimRaw;		    // output data dsc
    TAGparaDsc*     fpParMap;		    // parameter dsc
    TAGparaDsc*     fpParCal;		    // parameter dsc
    TAGparaDsc*     fpParGeo;		    // parameter dsc
    BM_struct*      fpEvtStruct;
    ifstream        fbmfile;        //bm raw file

    Long64_t fDataNumEv;            //current number of events
    Long64_t fDataSyncNumEv;        //current number of events + number of sync

    //histos
    TH1I*            fpRawError;        //BM hit channel error
    TH1I*            fpRawTdcChannel;   //TDC signal distribution
    TH1I*            fpRawTrigTime;     //Trigger time
    std::vector<TH1F*> fpRawTdcMeas;    //vector of tdc channel measurements
    std::vector<TH1F*> fpRawTdcLessSync;    //vector of tdc channel measurements
};

#endif
