#ifndef INFOEVENT_HH
#define INFOEVENT_HH

/*!
 \file InfoEvent.hh
 
 \brief Declaration of InfoEvent
 */

#include "BaseFragment.hh"

class InfoEvent : public BaseFragment {

public:
  u_int time_sec;    ///< time in second
  u_int time_nsec;   ///< time in microsecond
  u_int runType;     ///< run type
  u_int runNumber;   ///< run number
  u_int eventNumber; ///< event number

  //! Constructor
  InfoEvent(){};
  virtual ~InfoEvent();
  
  virtual void readData(unsigned int **p);
  virtual void printData() const;
  //! Check (dummy)
  virtual bool check() const { return true;};

  //! Return Class tyoe
  virtual std::string classType() const {return "InfoEvent";};
};

#endif
