#ifndef TDCEVENT_HH
#define TDCEVENT_HH
#include "BaseFragment.hh"

#include <vector>

class TDCEvent : public BaseFragment {

public:
  u_int tdc_sec;
  u_int tdc_usec;
  u_int eventNumber;
  u_int globalHeader;
  std::vector<u_int> tdcHeader;
  std::vector<u_int> measurement;
  std::vector<u_int> tdcTrailer;
  u_int extendedTimeTag;
  u_int globalTrailer;

  virtual ~TDCEvent();

  virtual void readData(unsigned int **p);
  virtual void printData() const;
  virtual bool check() const{return true;};

  virtual std::string classType() const {return "TDCEvent";};
};

#endif
