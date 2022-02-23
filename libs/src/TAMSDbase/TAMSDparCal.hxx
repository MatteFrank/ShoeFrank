#ifndef _TAMSDparCal_HXX
#define _TAMSDparCal_HXX
#include <string>
#include <map>
#include <TSystem.h>
#include <TF1.h>

#include "TAGroot.hxx"
#include "TAGparTools.hxx"
#include "TAGparaDsc.hxx"

#include "TAMSDcalibrationMap.hxx"

class TAMSDparCal : public TAGparTools
{
private:
  TAMSDcalibrationMap *fMapCal;
   TF1* fFunc1;
   TF1* fFunc2;
   TF1* fFunc3;
   TF1* fFuncEta;
   
private:
   void SetFunctions();

public:
  TAMSDparCal(int strip_number_p);
  virtual ~TAMSDparCal();

  // Calibration
  Bool_t   LoadEnergyCalibrationMap(TString Filename);
  TAMSDcalibrationMap::ElossParameter_t GetElossParameters();

  // Pedestal
  Bool_t   LoadPedestalMap(TString Filename);
  TAMSDcalibrationMap::PedParameter_t GetPedestal(Int_t sensorId, Int_t stripId);

  Double_t GetPedestalValue(Int_t sensorId, TAMSDcalibrationMap::PedParameter_t const& pedestal_p, Bool_t seed);

  Float_t ComputeEtaChargeCorrection(Float_t eta);
  Float_t ComputeEtaPosCorrection(Float_t eta);

 ClassDef(TAMSDparCal, 0)
};

#endif
