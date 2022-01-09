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

ClassImp(TAGnamed);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGnamed::TAGnamed()
: TNamed()
{}

//------------------------------------------+-----------------------------------
//! Construct with \a name and \a title.
TAGnamed::TAGnamed(const char* name, const char* title)
: TNamed(name, title)
{}

//------------------------------------------+-----------------------------------
//! Construct with \a name and \a title.
TAGnamed::TAGnamed(const TString& name, const TString& title)
: TNamed(name, title)
{}


//------------------------------------------+-----------------------------------
/*!
 \relates TAGnamed
 \brief ostream insertion operator.
 */
ostream& operator<<(ostream& os, const TAGnamed& obj)
{
  obj.ToStream(os);
  return os;
}
//------------------------------------------+-----------------------------------
//! Destructor.
TAGnamed::~TAGnamed()
{}

/*------------------------------------------+---------------------------------*/
// ostream insertion.
void TAGnamed::ToStream(ostream& os, Option_t* option) const
{
  os <<"TAGnamed: " << IsA()->GetName() << "  " << GetName() 
     << "  " << GetTitle() << endl;
  return;
}

/*------------------------------------------+---------------------------------*/
// ostream insertion.
void TAGnamed::Print(Option_t* option) const
{
  ToStream(cout, option);
  return;
}

