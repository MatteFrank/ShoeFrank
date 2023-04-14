/*!
  \file TAGobject.cxx
  \brief   Implementation of TAGobject.
*/

#include "TClass.h"

#include "TAGobject.hxx"

/*!
  \class TAGobject 
  \brief Mother of most unnamed TAG objects. **
*/

//! Class Imp
ClassImp(TAGobject);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGobject::TAGobject()
: fFound(false)
{}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGobject::~TAGobject()
{}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAGobject::ToStream(ostream& os, Option_t* option) const
{
  os <<"TAGobject: " << IsA()->GetName() << "  " << GetName() 
     << "  " << GetTitle() << endl;
  return;
}

/*------------------------------------------+---------------------------------*/
//! Print
//!
//! \param[in] option option for printout
void TAGobject::Print(Option_t* option) const
{
  ToStream(cout, option);
  return;
}

//------------------------------------------+-----------------------------------
//! operator<<
//!
//! \param[in] os output stream
//! \param[in] obj object to printout
inline ostream& operator<<(ostream& os, const TAGobject& obj)
{
  obj.ToStream(os);
  return os;
}
