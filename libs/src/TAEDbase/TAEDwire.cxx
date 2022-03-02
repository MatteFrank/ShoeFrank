/*!
 \file TAEDwire.cxx
 \brief Class to display wire on event
 */
/*------------------------------------------+---------------------------------*/

#ifndef _TAEDwire_HXX_
#include "TAEDwire.hxx"
#endif

#ifndef ROOT_TEveChunkManager
#include "TEveChunkManager.h"
#endif

/*!
 \class TAEDwire
 \brief Class to display wire on event
 */


//__________________________________________________________
//! default constructor
//!
//! \param[in] name wire list name
TAEDwire::TAEDwire(const Text_t* name)
  : TEveStraightLineSet(name)
{ 
}

//__________________________________________________________
//! default destructor
TAEDwire::~TAEDwire()
{
}

//__________________________________________________________
//! Add wire
//!
//! \param[in] x1 initial position in X-direction
//! \param[in] y1 initial position in Y-direction
//! \param[in] z1 initial position in Z-direction
//! \param[in] x2 final position in X-direction
//! \param[in] y2 final position in Y-direction
//! \param[in] z2 final position in Z-direction
void TAEDwire::AddWire(Float_t x1, Float_t y1, Float_t z1, 
                                    Float_t x2, Float_t y2, Float_t z2)
{
   AddLine(x1, y1, z1, x2, y2, z2);
}

//__________________________________________________________
//! Reset wires
void TAEDwire::ResetWires()
{
   TEveChunkManager& p = GetLinePlex();
   p.Reset(sizeof(TEveStraightLineSet::Line_t), p.N());
}

//__________________________________________________________
//! Refit chunk
void TAEDwire::RefitPlex()
{
   TEveChunkManager& p = GetLinePlex();
   p.Refit();
}
