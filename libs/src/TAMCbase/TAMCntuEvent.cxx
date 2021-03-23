#include "TAMCntuEvent.hxx"

ClassImp(TAMCntuEvent);

TString  TAMCntuEvent::fgkBranchName = "mcevt.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMCntuEvent::TAMCntuEvent()
: TAGdata(),
  fEventNumber(-1)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMCntuEvent::~TAMCntuEvent()
{
}

//------------------------------------------+-----------------------------------
//! Clear.
void TAMCntuEvent::Clear(Option_t*)
{
  fEventNumber = -1;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAMCntuEvent::ToStream(ostream& os, Option_t* option) const
{
  os << "TAMCntuEvent"
  << Form("  event number=%d", GetEventNumber())
  << endl;
}

