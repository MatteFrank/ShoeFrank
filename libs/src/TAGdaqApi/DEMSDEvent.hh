#ifndef DEMSDEVENT_HH
#define DEMSDEVENT_HH

#include "DECardEvent.hh"
#include <vector>

class DEMSDEvent : public DECardEvent {

  public:
    virtual ~DEMSDEvent();
  
    virtual void readData(unsigned int **p);
    virtual void printData() const;
    virtual bool check() const;

    virtual std::string classType() const {return "DEMSDEvent";};

    u_int Xplane[640];
    u_int Yplane[640];

    void fillStrip();
};

#endif
