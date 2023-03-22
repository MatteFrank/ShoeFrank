#ifndef DAQFILEHEADER_HH
#define DAQFILEHEADER_HH

/*!
 \file DAQFileHeader.hh
 
 \brief Declaration of DAQFileHeader
 */

#include "BaseFragment.hh"

class DAQFileHeader {

public:
  std::string writer;   ///< writer
  std::string filename; ///< file name
  std::string guid;     ///< gu id
  std::string stream;   ///< stream
  std::string project;  ///< project name
  std::string lumiblock;///< lumi block
  u_int fileNumber;     ///< file number

  void printData () const;

};

#endif
