#ifndef WDEVENT_HH
#define WDEVENT_HH

#include "RemoteEvent.hh"

class WDEvent : public RemoteEvent {

public:
  //firmware data
  u_int detectorHeader;
  u_int boardHeader;
  u_int numberWords;
  u_int hardwareEventNumber;
  u_int BCOofTrigger;
  u_int numBoards;
  u_int TWChans;
  u_int CaloChans;
  u_int trigType;
  uint64_t TriggerPattern;
  uint64_t TriggerGenerationBin[32];

  
  virtual ~WDEvent();
  
  virtual void readData(unsigned int **p);
  virtual void printData() const;
  virtual bool check() const{return true;};
  
  virtual std::string classType() const {return "WDEvent";};
  
};

#endif
