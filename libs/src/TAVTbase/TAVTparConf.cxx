/*!
  \file
  \version $Id: TAVTparConf.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TAVTparConf.
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
  \class TAVTparConf TAVTparConf.hxx "TAVTparConf.hxx"
  \brief Map and Geometry parameters for Inner Tracker **
*/

ClassImp(TAVTparConf);

TString TAVTbaseParConf::fgkDefaultConfName = "./config/TAITdetector.cfg";

//______________________________________________________________________________
TAVTparConf::TAVTparConf()
: TAVTbaseParConf()
{
   // Standard constructor
}

//______________________________________________________________________________
TAVTparConf::~TAVTparConf()
{
   // Destructor
}
