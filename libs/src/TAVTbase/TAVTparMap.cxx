/*!
 \file TAVTparMap.cxx
  \brief   Map parameters for VTX
 */

#include "TAVTparMap.hxx"

//##############################################################################

/*!
 \class TAVTparMap
 \brief Map parameters for VTX
 */

//! Class Imp
ClassImp(TAVTparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTparMap::TAVTparMap() 
 : TAVTbaseParMap()
{
   fkDefaultMapName = "./config/TAVTdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTparMap::~TAVTparMap()
{
}
