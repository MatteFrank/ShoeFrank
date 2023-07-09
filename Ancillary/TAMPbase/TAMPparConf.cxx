/*!
  \file TAMPparConf.cxx
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

#include "TAMPparConf.hxx"

//##############################################################################

/*!
  \class TAMPparConf
  \brief Base class of configuration parameters Monopix2
*/

//! Class Imp
ClassImp(TAMPparConf);

//______________________________________________________________________________
//! Standard constructor
TAMPparConf::TAMPparConf()
: TAVTbaseParConf()
{
   fkDefaultConfName = "./config/TAMPdetector.cfg";
}

//______________________________________________________________________________
//! Destructor
TAMPparConf::~TAMPparConf()
{
}
