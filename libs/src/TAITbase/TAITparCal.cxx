/*!
  \file TAITparCal.cxx
  \brief    Charge (raw) calibration for ITR
*/

#include <Riostream.h>

#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"

#include "TAVTparMap.hxx"
#include "TAITparCal.hxx"

//##############################################################################

/*!
  \class TAITparCal 
  \brief Charge (raw) calibration for ITR
*/

ClassImp(TAITparCal);


//------------------------------------------+-----------------------------------
//! Standard constructor
TAITparCal::TAITparCal()
: TAVTbaseParCal()
{
   fkDefaultCalName = "./calib/TAITdetector.cal";
}

//------------------------------------------+-----------------------------------
//! Destructor
TAITparCal::~TAITparCal()
{
}


