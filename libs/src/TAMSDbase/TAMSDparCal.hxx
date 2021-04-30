#ifndef _TAMSDparCal_HXX
#define _TAMSDparCal_HXX
#include <string>
#include <map>
#include <TSystem.h>

#include "TAGroot.hxx"
#include "TAGparTools.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGparaDsc.hxx"

#include "TAMSDcalibrationMap.hxx"

class TAMSDparCal : public TAGparTools
{
private:
  TAMSDparGeo* fParGeo;
  TAMSDcalibrationMap *fMapCal;
  
public:
  TAMSDparCal();
  virtual ~TAMSDparCal();

  // Calibration
  Bool_t   LoadEnergyCalibrationMap(TString Filename);
  Double_t GetElossParam(Int_t sensorId, Int_t stripId, UInt_t ParameterNumber);

 ClassDef(TAMSDparCal, 0)
};

#endif
