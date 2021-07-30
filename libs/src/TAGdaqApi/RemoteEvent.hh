#ifndef REMOTEEVENT_HH
#define REMOTEEVENT_HH

#include "BaseFragment.hh"
#include <vector>

class RemoteEvent : public BaseFragment {

  public:
    u_int time_sec;
    u_int time_usec;
    u_int eventNumber;

    u_int evtSize;
    std::vector<u_int> values;

    virtual ~RemoteEvent();
  
   RemoteEvent();
   RemoteEvent(const RemoteEvent &right);
   const RemoteEvent& operator=(const RemoteEvent &right);
   
    virtual void readData(unsigned int **p);
    virtual void printData() const;
    virtual bool check() const { return true;};

    virtual std::string classType() const {return "RemoteEvent";};
  
};

#endif
