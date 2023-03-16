#ifndef _BM_STRUCT_H_
#define _BM_STRUCT_H_

using namespace std;
//The BM acquisition software structure
typedef struct BM_struct {

  Int_t evnum;                  //number of event (from global header)
  Int_t words;                  //total number of words
  Int_t tot_status;             //global error flag: 0=ok
  Int_t tdc_status;             //tdc error flag: 0=ok, -1=not set, 1=tdc wrong ev_num, 2=channel out of range, 3=no tdc data
  Int_t sca_status;             //scaler830 error flag: 0=ok
  Int_t adc_status;             //adc792 error flag: 0=ok, 1=wrong words num,
  Int_t time_evtoev;            //time between the previous and this event (microsec)
  Int_t time_read;              //time occurred to read the data (microsec, this time do not consider the vme acess time ~+10msec)
  Int_t time_acq;               //time of the event acquisition
} BM_struct;

#endif
