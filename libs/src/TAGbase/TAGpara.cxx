/*!
  \file TAGpara.cxx
  \brief   Implementation of TAGpara.
*/

#include "TClass.h"
#include "TAGpara.hxx"

/*!
  \class TAGpara 
  \brief Mother of all parameter objects. **
*/

//! Class Imp
ClassImp(TAGpara);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGpara::TAGpara()
: TObject()
{}


//------------------------------------------+-----------------------------------
//! Destructor.
TAGpara::~TAGpara()
{}

//------------------------------------------+-----------------------------------
//! Clear
void TAGpara::Clear(Option_t*)
{
  ResetBit(kFail);
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAGpara::ToStream(ostream& os, Option_t* option) const
{
  os <<"TAGpara: " << IsA()->GetName();
  if (Fail()) os << "<failed>";
  os << endl;
  return;
}

/*------------------------------------------+---------------------------------*/
//! Print
//!
//! \param[in] option option for printout
void TAGpara::Print(Option_t* option) const
{
  ToStream(cout, option);
  return;
}

//------------------------------------------+-----------------------------------
//! operator<<
//!
//! \param[in] os output stream
//! \param[in] obj object to printout
inline ostream& operator<<(ostream& os, const TAGpara& obj)
{
  obj.ToStream(os);
  return os;
}
