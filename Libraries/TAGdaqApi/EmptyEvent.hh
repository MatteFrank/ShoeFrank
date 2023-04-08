#ifndef EMPTYEVENT_HH
#define EMPTYEVENT_HH

/*!
 \file EmptyEvent.hh
 
 \brief Declaration of EmptyEvent
 */

#include "BaseFragment.hh"

class EmptyEvent : public BaseFragment {

  public:
    u_int time_sec;    ///< time in second
    u_int time_usec;   ///< time in microsecond
    u_int lumiBlock;   ///< lumi block
    u_int eventNumber; ///< event number

    virtual ~EmptyEvent();
  
    virtual void readData(unsigned int **p);
    virtual void printData() const;
    //! Check (dummy)
    virtual bool check() const { return true;};

    //! return class type
    virtual std::string classType() const {return "EmptyEvent";};

};

#endif
