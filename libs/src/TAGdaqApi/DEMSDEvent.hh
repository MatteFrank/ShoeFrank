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

    u_int Xplane[640];  ///< Xplane information
    u_int Yplane[640];  ///< Yplane information

    void fillStrip();
};

#endif
