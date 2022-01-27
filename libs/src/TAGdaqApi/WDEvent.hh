#ifndef WDEVENT_HH
#define WDEVENT_HH

/*!
 \file WDEvent.hh
 
 \brief Declaration of WDEvent
 */

#include "RemoteEvent.hh"

class WDEvent : public RemoteEvent {

public:
  //firmware data
  u_int detectorHeader;     ///< detector header
  u_int boardHeader;        ///< board header
  u_int numberWords;        ///< number of words
  u_int hardwareEventNumber;///< trigger counter
  u_int BCOofTrigger;       ///< BCO of trigger
  u_int numBoards;          ///< number of boards
  u_int TWChans;            ///< Number of TW channels
  u_int CaloChans;          ///< Number of CAL channels
  u_int trigType;           ///< trigger type
  uint64_t TriggerPattern;  ///< trigger pattern
  uint64_t TriggerGenerationBin[32]; ///< trigger generation bin

  
  virtual ~WDEvent();
  
  virtual void readData(unsigned int **p);
  virtual void printData() const;
   //! Check
  virtual bool check() const{return true;};
  
   //! return class type
  virtual std::string classType() const {return "WDEvent";};
  
};

#endif
