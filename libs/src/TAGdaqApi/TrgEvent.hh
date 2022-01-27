#ifndef TRGEVENT_HH
#define TRGEVENT_HH
#include "BaseFragment.hh"

class TrgEvent : public BaseFragment {

public:
  u_int time_sec;              ///< tdc time in second
  u_int time_usec;             ///< tdc time in microseco
  u_int eventNumber;           ///< event number
  // firmware data
  u_int liveTime;              ///< live time
  u_int timeSinceLastTrigger;  ///< time since last trigger
  u_int clockCounter;          ///< clock counter
  u_int triggerCounter;        ///< trigger counter
  u_int BCOofTrigger;          ///< BCO of trigger
  u_int spillNrAndTrgFineDelay; ///< spill number and trigger fine delay
  u_int PMTsAndBusy;            ///< PMT and busy

  TrgEvent(){};
  virtual ~TrgEvent();
  
  virtual void readData(unsigned int **p);
  virtual void printData()const;
  //! Check
  virtual bool check() const { return eventNumber==triggerCounter;};

  //! Return Class type
  virtual std::string classType() const {return "TrgEvent";};
};

#endif
