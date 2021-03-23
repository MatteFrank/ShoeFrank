/*!
  \file
  \version $Id: TAGobject.cxx,v 1.4 2003/06/09 18:40:09 mueller Exp $
  \brief   Implementation of TAGobject.
*/

#include "TClass.h"

#include "TAGobject.hxx"

/*!
  \class TAGobject TAGobject.hxx "TAGobject.hxx"
  \brief Mother of most unnamed TAG objects. **
*/

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

void TAGobject::ToStream(ostream& os, Option_t* option) const
{
  os <<"TAGobject: " << IsA()->GetName() << "  " << GetName() 
     << "  " << GetTitle() << endl;
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TAGobject::Print(Option_t* option) const
{
  ToStream(cout, option);
  return;
}

//------------------------------------------+-----------------------------------
/*!
 \relates TAGobject
 \brief ostream insertion operator.
 */

inline ostream& operator<<(ostream& os, const TAGobject& obj)
{
  obj.ToStream(os);
  return os;
}
