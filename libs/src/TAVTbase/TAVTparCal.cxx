/*!
  \file TAVTparCal.cxx
   \brief    Charge (raw) calibration for vertex
*/

#include <Riostream.h>

#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"

#include "TAVTparMap.hxx"
#include "TAVTparCal.hxx"

//##############################################################################

/*!
  \class TAVTparCal TAVTparCal.hxx "TAVTparCal.hxx"
  \brief Charge (raw) calibration for vertex
*/

ClassImp(TAVTparCal);


//------------------------------------------+-----------------------------------
//! Standard constructor
TAVTparCal::TAVTparCal()
: TAVTbaseParCal()
{
   fkDefaultCalName = "./calib/TAVTdetector.cal";

}

//------------------------------------------+-----------------------------------
//! Destructor
TAVTparCal::~TAVTparCal()
{
}


