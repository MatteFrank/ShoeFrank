/*!
  \file
  \version $Id: TAITparCal.cxx
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
  \class TAITparCal TAITparCal.hxx "TAITparCal.hxx"
  \brief Charge (raw) calibration for ITR
*/

ClassImp(TAITparCal);


//------------------------------------------+-----------------------------------
TAITparCal::TAITparCal()
: TAVTbaseParCal()
{
  // Standard constructor
   fkDefaultCalName = "./calib/TAITdetector.cal";
}

//------------------------------------------+-----------------------------------
TAITparCal::~TAITparCal()
{
}


