#ifndef DEMSDEVENT_HH
#define DEMSDEVENT_HH

/*!
 \file DEMSDEvent.hh
 
 \brief Declaration of DEMSDEvent
 */


#include "DECardEvent.hh"
#include <vector>

class DEMSDEvent : public DECardEvent {

  public:
    virtual ~DEMSDEvent();
  
    virtual void readData(unsigned int **p);
    virtual void printData() const;
    virtual bool check() const;

    //! return class type
    virtual std::string classType() const {return "DEMSDEvent";};

    u_int FirstPlane[640];   ///< First Plane (closer to the TG) ADC information
    u_int SecondPlane[640];  ///< Second Plane ADC information

    void fillStrip();
};

#endif
