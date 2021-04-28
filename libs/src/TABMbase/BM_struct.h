#ifndef _BM_STRUCT_H_
#define _BM_STRUCT_H_

using namespace std;

#define MAXHITTDC 200
#define MAXEVTDC 20
#define SCA830MAX 32     //max channel read=32
#define ADC792MAX 32     //max channel read=32

//The BM acquisition software structure
typedef struct BM_struct {

  Int_t evnum;                  //number of event (from global header)
  Int_t words;                  //total number of words
  Int_t tdcev;                  //total number of tdc events
  Int_t tdc_hitnum[MAXEVTDC];   //total number of hits of the tdc for each tdc events
  Int_t tdc_evnum[MAXEVTDC];    //tdc event number for each tdc events
  Int_t tdc_id[MAXHITTDC];      //the tdc channel number measurement
  Int_t tdc_meas[MAXHITTDC];    //measurement value (10^-8 sec.)
  Int_t tdc_sync[MAXHITTDC];    //time of the sync channel -10000=not set, (10^-8 sec.)
  Int_t sca830_meas[SCA830MAX]; //scaler measurement defined as new count - old count for each channel
  Int_t sca830_counts[SCA830MAX]; //scaler raw counts for each channel
  Int_t adc792_meas[ADC792MAX]; //ADC measurement for each channel
  Int_t adc792_over[ADC792MAX]; //ADC overflow cannel, -1000=not set, 1=overflow, 0=ok
  Int_t tot_status;             //global error flag: 0=ok
  Int_t tdc_status;             //tdc error flag: -10000=ok, 0=not set, 1=tdc wrong ev_num, 2=channel out of range, 3=no tdc data
  Int_t sca_status;             //scaler830 error flag: 0=ok
  Int_t adc_status;             //adc792 error flag: 0=ok, 1=wrong words num,
  Int_t time_evtoev;            //time between the previous and this event (microsec)
  Int_t time_read;              //time occurred to read the data (microsec, this time do not consider the vme acess time ~+10msec)
  Int_t time_acq;               //time of the event acquisition
  Int_t tdc_numsync;            //numbero of tdc sync(for multisync events)
} BM_struct;

#endif
