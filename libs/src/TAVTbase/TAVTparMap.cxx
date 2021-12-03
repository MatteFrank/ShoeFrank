/*!
 \file
 \version $Id: TAVTparMap.cxx
 \brief   Map parameters for VTX
 */

#include "TAVTparMap.hxx"

//##############################################################################

/*!
 \class TAVTparMap
 \brief Map parameters for VTX
 */

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
