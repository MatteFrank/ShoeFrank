#ifndef _TAMSDparCal_HXX
#define _TAMSDparCal_HXX
#include <string>
#include <map>
#include <TSystem.h>

#include "TAGroot.hxx"
#include "TAGparTools.hxx"
#include "TAGparaDsc.hxx"

#include "TAMSDcalibrationMap.hxx"

class TAMSDparCal : public TAGparTools
{
private:
  TAMSDcalibrationMap *fMapCal;
  
public:
  TAMSDparCal(int strip_number_p);
  virtual ~TAMSDparCal();

  // Calibration
  Bool_t   LoadEnergyCalibrationMap(TString Filename);
  Double_t GetElossParam(Int_t sensorId, Int_t stripId, UInt_t ParameterNumber);
  TAMSDcalibrationMap::ElossParameter_t GetElossParameters(Int_t sensorId, Int_t stripId);

  // Pedestal
  Bool_t   LoadPedestalMap(TString Filename);
  Double_t GetPedestalMean(Int_t sensorId, Int_t stripId);
  Double_t GetPedestalSigma(Int_t sensorId, Int_t stripId);
  Int_t    GetPedestalStatus(Int_t sensorId, Int_t stripId);
  TAMSDcalibrationMap::PedParameter_t GetPedestal(Int_t sensorId, Int_t stripId);

  Double_t GetPedestalValue(Int_t sensorId, Int_t stripId);
  Double_t GetPedestalValue(Int_t sensorId, TAMSDcalibrationMap::PedParameter_t const& pedestal_p);
  Double_t GetPedestalNoiseLevel(Int_t sensorId);


 ClassDef(TAMSDparCal, 0)
};

#endif
