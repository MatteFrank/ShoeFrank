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
//! operator<<
//!
//! \param[in] os output stream
//! \param[in] obj object to stream
ostream& operator<<(ostream& os, const TAGdata& obj)
{
  obj.ToStream(os);
  return os;
}

//! Class Imp
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
//! Returns true if object needs automatic delete when read from tree.
Bool_t TAGdata::NeedAutoDelete() const
{
  return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Clear
//!
//! \param[in] opt clear options
void TAGdata::Clear(Option_t*)
{
  ResetBit(kFail);
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option printout option
void TAGdata::ToStream(ostream& os, Option_t* option) const
{
  os <<"TAGdata: " << IsA()->GetName();
  if (Fail()) os << "<failed>";
  os << endl;
  return;
}

/*------------------------------------------+---------------------------------*/
//! Print
//!
//! \param[in] option printout option
void TAGdata::Print(Option_t* option) const
{
  ToStream(cout, option);
  return;
}
