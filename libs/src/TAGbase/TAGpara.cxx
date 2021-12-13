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
// Invalidate parameter object.
void TAGpara::Clear(Option_t*)
{
  ResetBit(kFail);
  return;
}

/*------------------------------------------+---------------------------------*/
// ostream insertion.
void TAGpara::ToStream(ostream& os, Option_t* option) const
{
  os <<"TAGpara: " << IsA()->GetName();
  if (Fail()) os << "<failed>";
  os << endl;
  return;
}

/*------------------------------------------+---------------------------------*/
// ostream insertion.
void TAGpara::Print(Option_t* option) const
{
  ToStream(cout, option);
  return;
}

//------------------------------------------+-----------------------------------
/*!
 \relates TAGpara
 \brief ostream insertion operator.
 */

inline ostream& operator<<(ostream& os, const TAGpara& obj)
{
  obj.ToStream(os);
  return os;
}
