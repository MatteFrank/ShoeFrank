/*!
 \file TAGntuEvent.cxx
 \brief Simple container event informations from DAQ
 */
/*------------------------------------------+---------------------------------*/

#include <sys/time.h>
#include <math.h>

#include "TAGntuEvent.hxx"

/*!
 \class TAGntuEvent
 \brief Simple container event informations from DAQ
 */
/*------------------------------------------+---------------------------------*/

//! Class Imp
ClassImp(TAGntuEvent);

TString  TAGntuEvent::fgkBranchName = "evt.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGntuEvent::TAGntuEvent()
: TAGdata(),
  fTimeSec(0),
  fTimeUsec(0),
  fEventNumber(0),
  fLiveTime(0),
  fTimeSinceLastTrigger(0),
  fClockCounter(0),
  fTriggerCounter(0),
  fBCOofTrigger(0),
  fSpillNrAndTrgFineDelay(0),
  fPMTsAndBusy(0),
  fTriggerID(-1)  
{
  for(int i=0;i<NMAXTRIG;i++)fTriggersStatus[i]=0;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGntuEvent::~TAGntuEvent()
{
}

//------------------------------------------+-----------------------------------
//! Clear.
void TAGntuEvent::Clear(Option_t*)
{
  fTimeSec                = 0;
  fTimeUsec               = 0;
  fEventNumber            = 0;
  fLiveTime               = 0;
  fTimeSinceLastTrigger   = 0;
  fClockCounter           = 0;
  fTriggerCounter         = 0;
  fBCOofTrigger           = 0;
  fSpillNrAndTrgFineDelay = 0;
  fPMTsAndBusy            = 0;
  fTriggerID              =-1;
  for(int i=0;i<NMAXTRIG;i++)fTriggersStatus[i]=0;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAGntuEvent::ToStream(ostream& os, Option_t* option) const
{
  os << "TAGntuEvent"
  << Form("  event number=%u\n", fEventNumber)
  << Form("  trigger number=%u\n", fTriggerCounter)
  << Form("  time (mus)=%ud\n", fTimeUsec)
  << endl;
}

//------------------------------------------+-----------------------------------
//! Convert a double to a timestamp.
//!
//! \param[in] time time to convert
void TAGntuEvent::TimeFromDouble(Double_t time)
{
  Double_t sec  = floor(time);
  Double_t usec = floor(1.e6*(time - sec));
  if (usec == 1.e6) {
    sec  += 1.;
    usec  = 0.;
  }
  fTimeSec  = (UInt_t) sec;
  fTimeUsec = (UInt_t) usec;
  return;
}

//------------------------------------------+-----------------------------------
//! Set to current system time.
void TAGntuEvent::SetCurrentTime()
{
  struct timeval  tv;
  struct timezone tz;
  
  gettimeofday(&tv, &tz);
  fTimeSec  = (UInt_t) tv.tv_sec;
  fTimeUsec = (UInt_t) tv.tv_usec;
  return;
}

//------------------------------------------+-----------------------------------
//! Add deltatime seconds to the timestamp.
//!
//! \param[in] deltatime time to add
TAGntuEvent& TAGntuEvent::operator+(Double_t deltatime)
{
  TimeFromDouble(TimeToDouble() + deltatime);
  return *this;
}

//------------------------------------------+-----------------------------------
//! Subtract deltatime seconds from the timestamp.
//!
//! \param[in] deltatime time to substract
TAGntuEvent& TAGntuEvent::operator-(Double_t deltatime)
{
  TimeFromDouble(TimeToDouble() - deltatime);
  return *this;
}

//------------------------------------------+-----------------------------------
//! Returns the time elapsed from timestamp rhs to timestamp lhs in seconds.
//!
//! \param[in] lhs time left
//! \param[in] rhs time right
Double_t operator-(const TAGntuEvent& lhs, const TAGntuEvent& rhs)
{
  return lhs.TimeToDouble() - rhs.TimeToDouble();
}
