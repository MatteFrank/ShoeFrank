/*!
 \file TAMCntuEvent.cxx
 \brief   Implementation of TAMCntuEvent.
 */
/*------------------------------------------+---------------------------------*/

#include "TAMCntuEvent.hxx"

/*!
 \class TAMCntuEvent
 \brief MC event container
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
ClassImp(TAMCntuEvent);

TString  TAMCntuEvent::fgkBranchName = "mcevt.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMCntuEvent::TAMCntuEvent()
: TAGdata(),
  fEventNumber(-1),
  fTriggerNumber(-1),
  fTimeStamp(0)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMCntuEvent::~TAMCntuEvent()
{
}

//------------------------------------------+-----------------------------------
//! Clear event.
//!
//! \param[in] opt option for clearing (not used)
void TAMCntuEvent::Clear(Option_t*)
{
  fEventNumber = -1;
  fTriggerNumber = -1;
  fTimeStamp = 0;
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAMCntuEvent::ToStream(ostream& os, Option_t* option) const
{
  os << "TAMCntuEvent"
  << Form("  event number=%d\n", GetEventNumber())
  << Form("  trigger number=%d\n", GetTriggerNumber())
  << Form("  time stamp=%lld\n", GetTimeStamp())
  << endl;
}

