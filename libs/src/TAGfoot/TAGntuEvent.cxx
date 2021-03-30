#include "TAGntuEvent.hxx"

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
  fPMTsAndBusy(0)
{
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
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAGntuEvent::ToStream(ostream& os, Option_t* option) const
{
  os << "TAGntuEvent"
  << Form("  event number=%u\n", fEventNumber)
  << Form("  trigger number=%u\n", fTriggerCounter)
  << Form("  time (mus)=%ud\n", fTimeUsec)
  << endl;
}

