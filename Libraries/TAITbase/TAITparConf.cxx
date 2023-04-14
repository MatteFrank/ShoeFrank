/*!
  \file TAITparConf.cxx
  \brief   Configuration parameters for ITR
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

#include "TAITparConf.hxx"

//##############################################################################

/*!
  \class TAITparConf
  \brief Configuration parameters for ITR
*/

//! Class Imp
ClassImp(TAITparConf);

//______________________________________________________________________________
//! Standard constructor
TAITparConf::TAITparConf()
: TAVTbaseParConf()
{
   fkDefaultConfName = "./config/TAITdetector.cfg";
}

//______________________________________________________________________________
//! Destructor
TAITparConf::~TAITparConf()
{
}
