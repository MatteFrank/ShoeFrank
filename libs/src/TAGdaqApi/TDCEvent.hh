#ifndef TDCEVENT_HH
#define TDCEVENT_HH
#include "BaseFragment.hh"

/*!
 \file TDCEvent.hh
 
 \brief Declaration of TDCEvent
 */


#include <vector>

class TDCEvent : public BaseFragment {

public:
  u_int tdc_sec;                 ///< tdc time in second
  u_int tdc_usec;                ///< tdc time in microsecond
  u_int eventNumber;             ///< event number
  u_int globalHeader;            ///< global header
  std::vector<u_int> tdcHeader;  ///< tdc header vector
  std::vector<u_int> measurement;///< tdc measurement vector
  std::vector<u_int> tdcTrailer; ///< tdc trailer vector
  u_int extendedTimeTag;         ///< extended time tag
  u_int globalTrailer;           ///< global trailer

  virtual ~TDCEvent();

  virtual void readData(unsigned int **p);
  virtual void printData() const;
  //! Check (dummy)
  virtual bool check() const{return true;};

  //! Return Class type
  virtual std::string classType() const {return "TDCEvent";};
};

#endif
