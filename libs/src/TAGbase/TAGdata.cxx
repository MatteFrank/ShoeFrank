/*!
  \file TAGdata.cxx
  \brief   Implementation of TAGdata.
*/

#include "TClass.h"

#include "TAGdata.hxx"

/*!
  \class TAGdata 
  \brief Mother of all data objects. **
*/

//------------------------------------------+-----------------------------------
/*!
 \relates TAGdata
 \brief ostream insertion operator.
 */

ostream& operator<<(ostream& os, const TAGdata& obj)
{
  obj.ToStream(os);
  return os;
}

ClassImp(TAGdata);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGdata::TAGdata()
{}

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
