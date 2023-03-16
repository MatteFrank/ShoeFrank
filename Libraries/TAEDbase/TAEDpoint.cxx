
/*!
 \file TAEDpoint.cxx
 \brief  Class to display point on event.
 */

#include "TClass.h"
#include "TAEDpoint.hxx"

/*!
 \class TAEDpoint
 \brief  Class to display point on event.
 */


//__________________________________________________________
//! default constructor
//!
//! \param[in] name point list name
TAEDpoint::TAEDpoint(const Text_t* name)
  : TEvePointSet(name)
{
}

//__________________________________________________________
//! default destructor
TAEDpoint::~TAEDpoint()
{
}

//__________________________________________________________
//! Add  point
//!
//! \param[in] x position in X-direction
//! \param[in] y position in Y-direction
//! \param[in] z position in Z-direction
void TAEDpoint::AddPoint(Float_t x, Float_t y, Float_t z)
{
   SetPoint(Size(), x, y, z);
}

//__________________________________________________________
//! Reset points
void TAEDpoint::ResetPoints()
{
   Reset();
}

//__________________________________________________________
//! Selected point
//!
//! \param[in] idx point id
void TAEDpoint::PointSelected(Int_t idx)
{
   SecSelected(idx);
   fSelectedIdx = idx;
}

//__________________________________________________________
//! Second selected point
//!
//! \param[in] idx point id
void TAEDpoint::SecSelected(Int_t idx)
{
   Long_t args[1];
   args[0] = (Long_t) idx;
   
   Emit("PointSelected(Int_t)", args);
}
