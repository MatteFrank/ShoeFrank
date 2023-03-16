#ifndef ARDUINOEVENT_HH
#define ARDUINOEVENT_HH

#include "RemoteEvent.hh"
#include <vector>

class ArduinoEvent : public RemoteEvent {

  public:
  u_int mux0[16];
  u_int mux1[16];
  u_int mux2[16];
  u_int mux3[16];
  u_int mux4[16];


  virtual ~ArduinoEvent();
  
  virtual void readData(unsigned int **p);
  virtual void printData() const;
  virtual bool check() const;
  
  virtual std::string classType() const {return "ArduinoEvent";};
  
};

#endif
