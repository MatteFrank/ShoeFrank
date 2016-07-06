/*!
  \file
  \version $Id: TAGdata.cxx,v 1.5 2003/06/09 18:37:01 mueller Exp $
  \brief   Implementation of TAGdata.
*/

#include "TClass.h"

#include "TAGdata.hxx"

/*!
  \class TAGdata TAGdata.hxx "TAGdata.hxx"
  \brief Mother of all data objects. **
*/

ClassImp(TAGdata);

//------------------------------------------+-----------------------------------
//! Destructor.

TAGdata::~TAGdata()
{}

//------------------------------------------+-----------------------------------
//! Setup internal TClonesArray's (used by TreeWriter only)

void TAGdata::SetupClones()
{
  return;
}

//------------------------------------------+-----------------------------------
//! Returns \c true if object needs automatic delete when read from tree.

Bool_t TAGdata::NeedAutoDelete() const
{
  return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Invalidate data object.

void TAGdata::Clear(Option_t*)
{
  ResetBit(kFail);
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TAGdata::ToStream(ostream& os, Option_t* option) const
{
  os <<"TAGdata: " << IsA()->GetName();
  if (Fail()) os << "<failed>";
  os << endl;
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TAGdata::Print(Option_t* option) const
{
  ToStream(cout, option);
  return;
}

