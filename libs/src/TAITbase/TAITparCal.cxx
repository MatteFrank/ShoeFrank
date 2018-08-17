/*!
  \file
  \version $Id: TAITparCal.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TAITparCal.
*/

#include <Riostream.h>

#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"

#include "TAITparMap.hxx"
#include "TAITparCal.hxx"

//##############################################################################

/*!
  \class TAITparCal TAITparCal.hxx "TAITparCal.hxx"
  \brief Charge (raw) calibration for Inner tracker. **
*/

ClassImp(TAITparCal);


//------------------------------------------+-----------------------------------
TAITparCal::TAITparCal()
: TAVTparCal()
{
  // Standard constructor
}

//------------------------------------------+-----------------------------------
TAITparCal::~TAITparCal()
{

}

