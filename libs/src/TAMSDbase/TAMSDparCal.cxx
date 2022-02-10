#include "TError.h"

#include "TAMSDparCal.hxx"

ClassImp(TAMSDparCal)

//_____________________________________________________________________
TAMSDparCal::TAMSDparCal(int strip_number_p)
: TAGparTools()
{
  // Standard constructor
  fMapCal = new TAMSDcalibrationMap(strip_number_p);
}

//------------------------------------------+-----------------------------------
TAMSDparCal::~TAMSDparCal()
{
  if (fMapCal)
    delete fMapCal;
}

//_________________________________________
Bool_t TAMSDparCal::LoadEnergyCalibrationMap(TString name)
{
   Clear();
   
   fMapCal->LoadEnergyCalibrationMap(name.Data());
   
   Info("LoadEnergyCalibrationMap()", "Open file %s for Energy calibration\n", name.Data());
   
   return true;
}

//_____________________________________________________________________
TAMSDcalibrationMap::ElossParameter_t TAMSDparCal::GetElossParameters()
{
    return fMapCal->GetElossParameters();
}

//_________________________________________
Bool_t TAMSDparCal::LoadPedestalMap(TString name)
{
   Clear();
   
   fMapCal->LoadPedestalMap(name.Data());
   
   Info("LoadPedestalMap()", "Open file %s for Pedestals\n", name.Data());
   
   return true;
}

//_____________________________________________________________________
TAMSDcalibrationMap::PedParameter_t TAMSDparCal::GetPedestal(Int_t sensorId, Int_t stripId) {
    return fMapCal->GetPedestal( sensorId, stripId );
}

//_____________________________________________________________________
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
