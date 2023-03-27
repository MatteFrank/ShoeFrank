/*!
  \file TAVTparConf.cxx
  \brief   Configuration parameters for VTX
*/

#include <Riostream.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TList.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"

#include "TAVTparConf.hxx"

//##############################################################################

/*!
  \class TAVTparConf 
  \brief Configuration parameters for VTX
*/

//! Class Imp
ClassImp(TAVTparConf);

//______________________________________________________________________________
//! Standard constructor
TAVTparConf::TAVTparConf()
: TAVTbaseParConf()
{
   fkDefaultConfName = "./config/TAVTdetector.cfg";
}

//______________________________________________________________________________
//! Destructor
TAVTparConf::~TAVTparConf()
{
}
