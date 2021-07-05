#include "TError.h"

#include "TAMSDparCal.hxx"

ClassImp(TAMSDparCal)

//_____________________________________________________________________
TAMSDparCal::TAMSDparCal()
: TAGparTools()
{
  // Standard constructor
  fMapCal = new TAMSDcalibrationMap();
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
   
   Info("FromCalibFile()", "Open file %s for Energy calibration\n", name.Data());
   
   return true;
}

//_____________________________________________________________________
Double_t TAMSDparCal::GetElossParam(Int_t sensorId, Int_t stripId, UInt_t ParameterNumber)
{
    return fMapCal->GetElossParam(sensorId, stripId, ParameterNumber);
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
Double_t TAMSDparCal::GetPedestalMean(Int_t sensorId, Int_t stripId)
{
   return fMapCal->GetPedestalParam(sensorId, stripId, 0);
}

//_____________________________________________________________________
Double_t TAMSDparCal::GetPedestalSigma(Int_t sensorId, Int_t stripId)
{
   return fMapCal->GetPedestalParam(sensorId, stripId, 1);
}

//_____________________________________________________________________
Int_t TAMSDparCal::GetPedestalStatus(Int_t sensorId, Int_t stripId)
{
   return (int)fMapCal->GetPedestalParam(sensorId, stripId, 2);
}

//_____________________________________________________________________
Double_t TAMSDparCal::GetPedestalNoiseLevel(Int_t sensorId)
{
   return fMapCal->GetPedestalNoiseLevel(sensorId);
}

//_____________________________________________________________________
Double_t TAMSDparCal::GetPedestalValue(Int_t sensorId, Int_t stripId)
{
   
   Double_t sigLevel = fMapCal->GetPedestalNoiseLevel(sensorId);
   Double_t mean     = fMapCal->GetPedestalParam(sensorId, stripId, 0);
   Double_t sigma    = fMapCal->GetPedestalParam(sensorId, stripId, 1);
   Int_t status      = (int)fMapCal->GetPedestalParam(sensorId, stripId, 2);

   Double_t value = mean + sigLevel*sigma;
   
   if (status == 0)
      return value;
   else
      return -1;
}
