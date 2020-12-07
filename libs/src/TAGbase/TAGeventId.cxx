/*!
  \file
  \version $Id: TAGeventId.cxx,v 1.1 2001/12/10 21:28:24 mueller Exp $
  \brief   Implementation of TAGeventId.
*/

#include "TBuffer.h"

#include "TAGeventId.hxx"

/*!
  \class TAGeventId TAGeventId.hxx "TAGeventId.hxx"
  \brief MBS raw event info. **
*/

ClassImp(TAGeventId);

//------------------------------------------+-----------------------------------
//! Destructor.

TAGeventId::~TAGeventId()
{}

//------------------------------------------+-----------------------------------
//! operator =
const TAGeventId& TAGeventId::operator=(const TAGeventId &right)
{
   fiCam = right.fiCam;          // campaign number
   fiRun = right.fiRun;          // run number
   fiEvt = right.fiEvt;
  
  return *this;
}

/*------------------------------------------+---------------------------------*/
//! Custom streamer.

void TAGeventId::Streamer(TBuffer &R__b)
{
  UInt_t R__s, R__c;

  if (R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c); 
    if (R__v) { }
    R__b >> fiCam;
    R__b >> fiRun;
    R__b >> fiEvt;

  } else {
    R__c = R__b.WriteVersion(TAGeventId::IsA(), kFALSE);
    R__b << fiCam;
    R__b << fiRun;
    R__b << fiEvt;
  }

  return;
}
