#ifndef EVENTREADER_HH
#define EVENTREADER_HH

/*!
 \file EventReader.hh
 
 \brief Declaration of EventReader
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <stdint.h>

typedef unsigned int u_int; ///< type def

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
  //! return good event (no error)
  bool good() const { return (!m_errorOnRead); };
   //! Check if file open
  bool getIsOpened() const {return m_isOpened;};

  void readFileHeader();
  void getNextEvent();
  void skipEvent();
   //! Check EOF
  bool endOfFileReached() const { return m_errorOnRead;};
   //! Get number of events read
  unsigned int getEventsRead() const { return m_eventsRead;};

  void printData();
  bool check();  // return true if all OK

   //! Get DAQ header file
  DAQFileHeader* getFileHeader() const {return m_head;};
   //! Get info event
  InfoEvent*     getInfoEvent() const {return m_info;};
   //! Get trigger event
  TrgEvent*      getTriggerEvent() const {return m_trg;};

   //! Get fragment map
  const std::map<u_int, BaseFragment*> & getFragments() const { return m_fragments;};
   //! Get fragment for a given id
   //! \param[in] fragID fragment id
  const BaseFragment* getFragmentID(u_int fragID) const { 
     std::map<u_int, BaseFragment*>::const_iterator iternew = m_fragments.find(fragID); 
     const BaseFragment* bp=NULL;
     if( iternew != m_fragments.end() )
       bp = iternew->second;
     return bp;};

protected:

  // filename and ancillary data
  std::ifstream* m_file;  ///< input stream
  bool m_isOpened;        ///< open file flag
  bool m_errorOnRead;     ///< error flag
  bool m_isFirstFile;     ///< first file flag

  // main fragment storage
  std::map<u_int, BaseFragment*> m_fragments; ///< main fragment storage
  
  // pointer to interesting fragments
  DAQFileHeader* m_head;    ///< file header
  InfoEvent* m_info;        ///< event information
  TrgEvent* m_trg;          ///< trigger fragment

  int m_debugLevel;         ///< debug level
  unsigned int m_eventsRead;///< number of events read

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
  char* m_buffer;  ///< buffer
};

#endif
