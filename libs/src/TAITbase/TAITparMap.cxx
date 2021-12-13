/*!
 \file TAITparMap.cxx
 \brief    Map parameters for ITR
 */

#include "TAITparMap.hxx"

//##############################################################################

/*!
 \class TAITparMap
 \brief Map parameters for ITR
 */

ClassImp(TAITparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAITparMap::TAITparMap() 
: TAVTparMap()
{
   fkDefaultMapName = "./config/TAITdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITparMap::~TAITparMap()
{}
