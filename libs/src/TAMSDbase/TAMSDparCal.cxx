/*!
 \file TAMSDparCal.cxx
 \brief   Implementation of TAMSDparCal.
 */

#include "TError.h"

#include "TAMSDparCal.hxx"

/*!
 \class TAMSDparCal
 \brief Calibration class for MSD
 */

//! Class Imp
ClassImp(TAMSDparCal)

//_____________________________________________________________________
//! Constructor
//!
//! \param[in] strip_number_p strip number
TAMSDparCal::TAMSDparCal(int strip_number_p)
: TAGparTools()
{
  // Standard constructor
  fMapCal = new TAMSDcalibrationMap(strip_number_p);
}

//------------------------------------------+-----------------------------------
//! Destructor
TAMSDparCal::~TAMSDparCal()
{
  if (fMapCal)
    delete fMapCal;
}

//_____________________________________________________________________
//! Load energy calibration map file
//!
//! \param[in] name input file
Bool_t TAMSDparCal::LoadEnergyCalibrationMap(TString name)
{
   Clear();
   
   fMapCal->LoadEnergyCalibrationMap(name.Data());
   
   Info("LoadEnergyCalibrationMap()", "Open file %s for Energy calibration\n", name.Data());
   
   return true;
}

//_____________________________________________________________________
//! return energy loss parameters
TAMSDcalibrationMap::ElossParameter_t TAMSDparCal::GetElossParameters()
{
    return fMapCal->GetElossParameters();
}

//_____________________________________________________________________
//! Load pesdtal file
//!
//! \param[in] name input file
Bool_t TAMSDparCal::LoadPedestalMap(TString name)
{
   Clear();
   
   fMapCal->LoadPedestalMap(name.Data());
   
   Info("LoadPedestalMap()", "Open file %s for Pedestals\n", name.Data());
   
   return true;
}

//_____________________________________________________________________
//! Get pedestal parameter
//!
//! \param[in] sensorId sensor id
//! \param[in] stripId strip id
TAMSDcalibrationMap::PedParameter_t TAMSDparCal::GetPedestal(Int_t sensorId, Int_t stripId) {
    return fMapCal->GetPedestal( sensorId, stripId );
}

//_____________________________________________________________________
//! Get pedestal threshold
//!
//! \param[in] sensorId sensor id
//! \param[in] pedestal_p pedestal parameter
//! \param[in] seed seed flag
Double_t TAMSDparCal::GetPedestalValue(Int_t sensorId, TAMSDcalibrationMap::PedParameter_t const& pedestal_p, Bool_t seed)
{
   Double_t sigLevel = 0;
   
   if(seed){
      sigLevel = fMapCal->GetPedestalSeedLevel(sensorId);
   }else {
      sigLevel = fMapCal->GetPedestalHitLevel(sensorId);
   }

   Double_t mean     = pedestal_p.mean;
   Double_t sigma    = pedestal_p.sigma;

   Double_t value = mean + sigLevel*sigma;
   
   if (pedestal_p.status)
      return value;
   else
      return -1;
}

//_____________________________________________________________________
//! Get energy loss parameter
//!
//! \param[in] eta eta value
Double_t TAMSDparCal::GetElossParam(Float_t eta)
{
   return fMapCal->GetElossParam(eta);
}


