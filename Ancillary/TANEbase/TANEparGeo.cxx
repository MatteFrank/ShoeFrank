/*!
  \file
  \version $Id: TANEparGeo.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TANEparGeo.
*/

#include <string.h>

#include <fstream>
#include <sstream>
#include "TSystem.h"
#include "TString.h"

#include "TANEparGeo.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TANEparGeo TANEparGeo.hxx "TANEparGeo.hxx"
  \brief Map and Geometry parameters for the start counter. **
*/

ClassImp(TANEparGeo);

const TString TANEparGeo::fgkBaseName    = "NE";
const TString TANEparGeo::fgkDefParaName = "neGeo";

//------------------------------------------+-----------------------------------
//! Default constructor.
TANEparGeo::TANEparGeo()
 : TAGparTools(),
   fSize(),
   fDebugLevel(0)
{
  fkDefaultGeoName = "./geomaps/TANEdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.

TANEparGeo::~TANEparGeo()
{
}

//______________________________________________________________________________
Bool_t TANEparGeo::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;

   Info("FromFile()", "Open file %s for geometry\n", name.Data());

   //The center is taken from the global setup of the experiment.
   ReadVector3(fSize);
   if(fDebugLevel)
      cout  << "  Size: "
      << fSize[0] << " " << fSize[1]  << " " <<  fSize[2] << endl;
   
   ReadStrings(fMaterial);
   if(fDebugLevel)
      cout  << "   TW material: " <<  fMaterial << endl;
   
   ReadItem(fDensity);
   if(fDebugLevel)
      cout  << "   TW density: " <<  fDensity << endl;

   // window entrance
   ReadVector3(fSizeWindow);
   if(fDebugLevel)
      cout  << "  Size of entrance window: "
      << fSizeWindow[0] << " " << fSizeWindow[1]  << " " <<  fSizeWindow[2] << endl;
   
   ReadStrings(fMatWindow);
   if(fDebugLevel)
      cout  << "   TW entrance window material: " <<  fMatWindow << endl;
   
   ReadItem(fDensityWindow);
   if(fDebugLevel)
      cout  << "   TW entrance window density: " <<  fDensityWindow << endl;
      
   return true;
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.

void TANEparGeo::Clear(Option_t*)
{
   return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TANEparGeo::ToStream(ostream& os, Option_t*) const
{
  os << "TANEparGeo " << GetName() << endl;

  return;
}
