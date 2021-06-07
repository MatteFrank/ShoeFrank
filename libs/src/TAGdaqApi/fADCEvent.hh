#ifndef FADCEVENT_HH
#define FADCEVENT_HH

#include <vector>
#include "BaseFragment.hh"

class fADCEvent : public BaseFragment {

  public:
    u_int fadc_sec;
    u_int fadc_usec;
    u_int eventNumber;
    u_int eventSize;
    u_int data;
    u_int nchans;
    u_int eventFADCounter;
    u_int triggerTimeTag;
    u_int MSBTimeTag;
    uint64_t extendedTriggerTimeTag;
    std::vector<bool> active; //map of active channel
    std::vector<u_int> channel[8]; //data
 
    virtual ~fADCEvent();
  
    virtual void readData(unsigned int **p);
    virtual void printData() const;
    virtual bool check() const { return true;};

    virtual std::string classType() const {return "fADCEvent";};
 
  private:
  
    void printV ();
    int getWordsChannel( int nchans );
    int getnchans ( int data );
	
};

#endif
