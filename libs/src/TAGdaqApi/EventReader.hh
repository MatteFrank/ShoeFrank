//
//
#ifndef EVENTREADER_HH
#define EVENTREADER_HH

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <stdint.h>

typedef unsigned int u_int;

#include "DAQFileHeader.hh"
#include "DAQMarkers.hh"
#include "InfoEvent.hh"
#include "TrgEvent.hh"


class EventReader {

public:
  // default constructor
  EventReader(int debugLevel=0);
  // default destructor
  ~EventReader();

  // it returns false if unable to open file
  bool openFile( std::string filename );

  // simple close file method
  void closeFile();
  //
  bool good() const { return (!m_errorOnRead); };
  bool getIsOpened() const {return m_isOpened;};

  void readFileHeader();
  void getNextEvent();
  bool endOfFileReached() const { return m_errorOnRead;};
  unsigned int getEventsRead() const { return m_eventsRead;};

  void printData();
  bool check();  // return true if all OK

  DAQFileHeader* getFileHeader() const {return m_head;};
  InfoEvent*     getInfoEvent() const {return m_info;};
  TrgEvent*      getTriggerEvent() const {return m_trg;};

  const std::map<u_int, BaseFragment*> & getFragments() const { return m_fragments;};
  const BaseFragment* getFragmentID(u_int fragID) const { 
     std::map<u_int, BaseFragment*>::const_iterator iternew = m_fragments.find(fragID); 
     const BaseFragment* bp=NULL;
     if( iternew != m_fragments.end() )
       bp = iternew->second;
     return bp;};

protected:

  // filename and ancillary data
  std::ifstream* m_file;
  bool m_isOpened;
  bool m_errorOnRead;
  bool m_isFirstFile;

  // main fragment storage
  std::map<u_int, BaseFragment*> m_fragments;
  
  // pointer to interesting fragments
  DAQFileHeader* m_head;    // file header
  InfoEvent* m_info;        // event information
  TrgEvent* m_trg;          // trigger fragment

  int m_debugLevel;
  unsigned int m_eventsRead;

private:

  void preEvent();
  char* readInEvent();
  unsigned int getROSInformation(unsigned int **p1);
  void readInfoEvent(unsigned int ** p1);

  // low level functions
  unsigned int readWord();
  std::string readString();
  std::string copyString(unsigned int nwords);

  // working area for input event
  char* m_buffer;
};

#endif
