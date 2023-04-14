#ifndef FADCEVENT_HH
#define FADCEVENT_HH

/*!
 \file fADCEvent.hh
 
 \brief Declaration of fADCEvent
 */

#include <vector>
#include "BaseFragment.hh"

class fADCEvent : public BaseFragment {

  public:
    u_int fadc_sec;                 ///< fadc in second
    u_int fadc_usec;                ///< fadc in microsecond
    u_int eventNumber;              ///< event number
    u_int eventSize;                ///< event size
    u_int data;                     ///< data
    u_int nchans;                   ///< number of channels
    u_int eventFADCounter;          ///< event fadc counter
    u_int triggerTimeTag;           ///< trigger time tag
    u_int MSBTimeTag;               ///< MSB time tag
    uint64_t extendedTriggerTimeTag;///< extented trigger time tag
   
    std::vector<bool> active;       ///< map of active channel
    std::vector<u_int> channel[8];  ///< data
 
    virtual ~fADCEvent();
  
    virtual void readData(unsigned int **p);
    virtual void printData() const;
   //! Check (dummy)
    virtual bool check() const { return true;};

    //! Return Class type
    virtual std::string classType() const {return "fADCEvent";};
 
  private:
  
    void printV ();
    int getWordsChannel( int nchans );
    int getnchans ( int data );
	
};

#endif
