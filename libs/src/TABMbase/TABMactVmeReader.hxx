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
#include <fstream>

class TABMactVmeReader : public TAGaction {
  public:

    //~ Bool_t openFile(TABMparCon*);  //load in datastream the data file, provv da modificare cancellare

    explicit        TABMactVmeReader(const char* name=0,
                                  TAGdataDsc* p_datraw=0,
                                  TAGparaDsc* p_parmap=0,
                                  TAGparaDsc* p_parcon=0,
                                  TAGparaDsc* p_pargeo=0,
                                  TAGdataDsc* p_timraw=0);
    virtual         ~TABMactVmeReader();
    virtual Int_t   Open(const TString& name);
    virtual void    Close();
    virtual Bool_t  Process();
    //~ virtual Bool_t  Action();
    
    void clear_bmstruct(Bool_t forced);
    Bool_t read_event(Bool_t evt0);    
    void PrintBMstruct();
    void monitorQDC(vector<Int_t>& adc792_words);

    ClassDef(TABMactVmeReader,0)

  private:
    TAGdataDsc*     fpDatRaw;		    // output data dsc
    TAGdataDsc*     fpTimRaw;		    // output data dsc
    TAGparaDsc*     fpParMap;		    // parameter dsc
    TAGparaDsc*     fpParCon;		    // parameter dsc
    TAGparaDsc*     fpParGeo;		    // parameter dsc
    BM_struct*      fpEvtStruct;    
    ifstream        fbmfile;      //bm raw file

    Int_t track_ok;  //-2=maxnhit_cut; -1=minhit_cut; otherwise=track_status     
    Long64_t data_num_ev; //current number of events
    Long64_t data_sync_num_ev; //current number of events + number of sync

};

#endif
