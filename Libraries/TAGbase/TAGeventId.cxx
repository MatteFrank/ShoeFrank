/*!
  \file TAGeventId.cxx
  \brief   Implementation of TAGeventId.
*/

#include "TBuffer.h"

#include "TAGeventId.hxx"

/*!
  \class TAGeventId 
  \brief DAQ raw event info. **
*/

//! Class Imp
ClassImp(TAGeventId);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGeventId::TAGeventId()
: fiCam(-1),
  fiRun(-1),
  fiEvt(-1)
{}

//------------------------------------------+-----------------------------------
//! Construct with campaign, run, and event number.
//!
//! \param[in] i_cam campaign number
//! \param[in] i_run run number
//! \param[in] i_evt event number
TAGeventId::TAGeventId(Short_t i_cam, Short_t i_run, Int_t i_evt)
: fiCam(i_cam),
  fiRun(i_run),
  fiEvt(i_evt)
{}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGeventId::~TAGeventId()
{}

//------------------------------------------+-----------------------------------
//! Clear event id.
void TAGeventId::Clear()
{
  fiCam = -1;
  fiRun = -1;
  fiEvt = -1;
  return;
}

//------------------------------------------+-----------------------------------
//! operator =
//!
//! \param[in] right event to equal
const TAGeventId& TAGeventId::operator=(const TAGeventId& right)
{
   fiCam = right.fiCam;          // campaign number
   fiRun = right.fiRun;          // run number
   fiEvt = right.fiEvt;
  
  return *this;
}

/*------------------------------------------+---------------------------------*/
// Custom streamer.
//
// \param[in] R__b buffer
void TAGeventId::Streamer(TBuffer& R__b)
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

//------------------------------------------+-----------------------------------
//!
//! Returns true of event id's \a lhs and \a rhs are equal
//!
//! \param[in] lhs left to compare
//! \param[in] rhs right to compare
bool operator==(const TAGeventId& lhs, const TAGeventId& rhs)
{
  return lhs.fiCam==rhs.fiCam && lhs.fiRun==rhs.fiRun && lhs.fiEvt==rhs.fiEvt;
}

