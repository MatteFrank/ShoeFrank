#ifndef REMOTEEVENT_HH
#define REMOTEEVENT_HH

/*!
 \file RemoteEvent.hh
 
 \brief Declaration of RemoteEvent
 */

#include "BaseFragment.hh"
#include <vector>

class RemoteEvent : public BaseFragment {

  public:
    u_int time_sec;            ///< time in second
    u_int time_usec;           ///< time in microsecond
    u_int eventNumber;         ///< event number

    u_int evtSize;             ///< event size
    std::vector<u_int> values; ///< value vector

    virtual ~RemoteEvent();
  
   RemoteEvent();
   RemoteEvent(const RemoteEvent &right);
   const RemoteEvent& operator=(const RemoteEvent &right);
   
    virtual void readData(unsigned int **p);
    virtual void printData() const;
    virtual bool check() const { return true;};

    //! Return Class type
    virtual std::string classType() const {return "RemoteEvent";};
  
};

#endif
