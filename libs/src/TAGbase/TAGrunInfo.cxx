/*!
  \file
  \version $Id: TAGrunInfo.cxx
  \brief   Implementation of TAGrunInfo.
*/

#include "TBuffer.h"

#include "TAGrunInfo.hxx"

/*!
  \class TAGrunInfo TAGrunInfo.hxx "TAGrunInfo.hxx"
  \brief Run number info. **
*/

ClassImp(TAGrunInfo);

TString TAGrunInfo::fgkObjectName = "runinfo";

//------------------------------------------+-----------------------------------
//! Destructor.

TAGrunInfo::~TAGrunInfo()
{}

//------------------------------------------+-----------------------------------
//! Clear run info.

void TAGrunInfo::Clear(Option_t*)
{
  fsCam = "";
  fiCam = -1;
  fiRun = -1;
  return;
}

//------------------------------------------+-----------------------------------
//! ostream insertion.

void TAGrunInfo::ToStream(ostream& os, Option_t* option) const
{
  os << "TAGrunInfo:     "
     << Form("  cam: %s", fsCam.Data())
     << Form("  cam = %4d", fiCam)
     << Form("  run = %4d", fiRun)
     << endl;
}
