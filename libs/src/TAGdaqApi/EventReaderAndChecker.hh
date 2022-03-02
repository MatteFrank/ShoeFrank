#ifndef EVENTREADERANDCHECKER_HH
#define EVENTREADERANDCHECKER_HH

/*!
 \file EventReaderAndChecker.hh
 
 \brief Declaration of EventReaderAndChecker
 */


#include <vector>
#include "EventReader.hh"

/*!
 \struct ErrInfo
 \brief  error information
 */
struct ErrInfo {
  u_int counts; ///< counts
  u_int errors; ///< errors
  bool  isFound; ///< found flag
  ErrInfo() : counts(0), errors(0), isFound(false) {};
};


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

  std::map<u_int,ErrInfo> m_infos; ///< information map
  u_int m_fileErrors;              ///< file errors
  u_int m_eventOffset;             ///< event offset
};

#endif
