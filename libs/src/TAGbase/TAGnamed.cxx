/*!
  \file TAGnamed.cxx
  \brief   Implementation of TAGnamed.
*/

#include "TClass.h"
#include "TAGnamed.hxx"

/*!
  \class TAGnamed 
  \brief Mother of all named TAG objects. **
*/

//! Class Imp
ClassImp(TAGnamed);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGnamed::TAGnamed()
: TNamed()
{}

//------------------------------------------+-----------------------------------
//! Construct with a name and a title.
//!
//! \param[in] name name
//! \param[in] title title
TAGnamed::TAGnamed(const char* name, const char* title)
: TNamed(name, title)
{}

//------------------------------------------+-----------------------------------
//! Construct with a name and a title.
//!
//! \param[in] name name
//! \param[in] title title
TAGnamed::TAGnamed(const TString& name, const TString& title)
: TNamed(name, title)
{}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGnamed::~TAGnamed()
{}

//------------------------------------------+-----------------------------------
//! operator<<
//!
//! \param[in] os output stream
//! \param[in] obj object name to printout
ostream& operator<<(ostream& os, const TAGnamed& obj)
{
  obj.ToStream(os);
  return os;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAGnamed::ToStream(ostream& os, Option_t* option) const
{
  os <<"TAGnamed: " << IsA()->GetName() << "  " << GetName() 
     << "  " << GetTitle() << endl;
  return;
}

//------------------------------------------+---------------------------------
//! Print
//!
//! \param[in] option option for printout
void TAGnamed::Print(Option_t* option) const
{
  ToStream(cout, option);
  return;
}

