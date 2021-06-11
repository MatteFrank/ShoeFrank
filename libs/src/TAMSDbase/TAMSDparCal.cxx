#include "TError.h"

#include "TAMSDparCal.hxx"

ClassImp(TAMSDparCal)

//_____________________________________________________________________
TAMSDparCal::TAMSDparCal()
: TAGparTools()
{
  // Standard constructor
  fMapCal = new TAMSDcalibrationMap();
  fParGeo = (TAMSDparGeo*)gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object();
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
   
   Info("FromCalibFile()", "Open file %s for calibration\n", name.Data());
   
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
   
   Info("LoadPedestalMap()", "Open file %s for calibration\n", name.Data());
   
   return true;
}

//_____________________________________________________________________
Double_t TAMSDparCal::GetPedestalMean(Int_t sensorId, Int_t stripId)
{
   return fMapCal->GetElossParam(sensorId, stripId, 0);
}

//_____________________________________________________________________
Double_t TAMSDparCal::GetPedestalSigma(Int_t sensorId, Int_t stripId)
{
   return fMapCal->GetElossParam(sensorId, stripId, 1);
}
