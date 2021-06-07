//
//
#ifndef EVENTREADERANDCHECKER_HH
#define EVENTREADERANDCHECKER_HH

#include <vector>
#include "EventReader.hh"

struct ErrInfo {
  u_int counts;
  u_int errors;
  bool  isFound;
  ErrInfo() : counts(0), errors(0), isFound(false) {};
};

// this class is an EventReader that is able to perform all
// the possible checks on the data

class EventReaderAndChecker : public EventReader {

public:
  // default constructor
  EventReaderAndChecker(int debugLevel=0);
  // default destructor
  ~EventReaderAndChecker();

  bool check();

  void printStatistics();

  u_int getBCOdiff() const;

private: 

  std::map<u_int,ErrInfo> m_infos;
  u_int m_fileErrors;
  u_int m_eventOffset;
};

#endif
