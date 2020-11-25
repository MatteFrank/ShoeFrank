#ifndef TATWCALIBRATIONMAP_H
#define TATWCALIBRATIONMAP_H
#include <string>
#include <map>
#include <TSystem.h>

#include "TAGroot.hxx"
#include "TAGparTools.hxx"
#include "TACAparGeo.hxx"
#include "TAGparaDsc.hxx"

typedef std::map<Int_t,std::vector<Double_t> > TCalibrationMapType;

class TACAparCal : public TAGparTools
{
private:
  TACAparGeo* fParGeo;
  TCalibrationMapType fCalibElossMapCrys; // map for energy calibration per crystal
  TCalibrationMapType fCalibTofMapCrys;   // map for tof calibration per crystal
  
public:
  TACAparCal();
  void LoadEnergyCalibrationMap(TString Filename);
  void LoadTofCalibrationMap(TString Filename);
  
  Double_t GetElossParameter(Int_t crystalId, UInt_t ParameterNumber);
  Double_t GetTofParameter(Int_t crystalId, UInt_t ParameterNumber);

   ClassDef(TACAparCal, 0)
};

#endif
