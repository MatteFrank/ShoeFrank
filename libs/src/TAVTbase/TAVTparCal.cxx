/*!
  \file
  \version $Id: TAVTparCal.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TAVTparCal.
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
  \brief Charge (raw) calibration for vertex. **
*/

ClassImp(TAVTparCal);


//------------------------------------------+-----------------------------------
TAVTparCal::TAVTparCal()
: TAVTbaseParCal()
{
  // Standard constructor
   fkDefaultCalName = "./calib/TAVTdetector.cal";

}

//------------------------------------------+-----------------------------------
TAVTparCal::~TAVTparCal()
{
}


