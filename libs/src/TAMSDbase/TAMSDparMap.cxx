/*!
 \file
 \version $Id: TAMSDparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
 \brief   Implementation of TAMSDparMap.
 */

#include "TAMSDparMap.hxx"

//##############################################################################

/*!
 \class TAVTparMap TAVTparMap.hxx "TAVTparMap.hxx"
 \brief Mapping for MSD. **
 */

ClassImp(TAMSDparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMSDparMap::TAMSDparMap()
: TAVTbaseParMap()
{
   fkDefaultMapName = "./config/TAMSDdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMSDparMap::~TAMSDparMap()
{
}
