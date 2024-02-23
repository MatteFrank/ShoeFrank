/*!
 \file TANAactNtuMass.cxx
 \brief   Implementation of TANAactNtuMass.
 */
#include "TClonesArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"

//TAGroot
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"
#include "TAGnameManager.hxx"

// GLB
#include "TAGntuPoint.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TANAactNtuMass.hxx"

/*!
 \class TANAactNtuMass
 \brief Base class for global analysis
 */

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[out] pNtuTrack global track container descriptor
//! \param[in] pgGeoMap target geometry parameter descriptor
TANAactNtuMass::TANAactNtuMass(const char* name,TAGdataDsc* pNtuTrack, TAGparaDsc* pgGeoMap)
:  TANAactBaseNtu(name, pNtuTrack, pgGeoMap)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TANAactNtuMass::~TANAactNtuMass()
{
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TANAactNtuMass::CreateHistogram()
{
}

//_____________________________________________________________________________
//! Action
Bool_t TANAactNtuMass::Action()
{
   return true;
}
