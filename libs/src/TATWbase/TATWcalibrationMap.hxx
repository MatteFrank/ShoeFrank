#ifndef TATWCALIBRATIONMAP_H
#define TATWCALIBRATIONMAP_H
#include <string>
#include <map>
#include <TSystem.h>
#include "TAGobject.hxx"
#include "Parameters.h"
#include "TAGparTools.hxx"
#include "TATWparGeo.hxx"

typedef std::map<Int_t,std::vector<Double_t> > TCalibrationMapType;

class TATWcalibrationMap : public TAGobject
{
private:
	TCalibrationMapType fCalibrationMap;
	bool fCalibrationMapIsOk;

  TCalibrationMapType fCalibTofMapLayX;   // map for tof calibration per pos xy
  TCalibrationMapType fCalibTofMapLayY;   // map for tof calibration per pos xy
  
  TCalibrationMapType fCalibElossMapLayX; // map for energy calibration per pos xy
  TCalibrationMapType fCalibElossMapLayY; // map for energy calibration per pos xy
    
  TCalibrationMapType fCalibElossBarMapLayX; // map for energy calibration per Bar new
  TCalibrationMapType fCalibElossBarMapLayY; // map for energy calibration per Bar new
  
  TCalibrationMapType fCalibTofBarMapLayX;   // map for tof calibration per Bar new
  TCalibrationMapType fCalibTofBarMapLayY;   // map for tof calibration per Bar new

  TCalibrationMapType fElossTuningBarMapLayX; // map for energy tuning per Bar new
  TCalibrationMapType fElossTuningBarMapLayY; // map for energy tuning per Bar new
  

public:
  TATWcalibrationMap();
  void LoadCalibrationMap(std::string Filename);
  void LoadEnergyCalibrationMap(std::string Filename);
  void LoadTofCalibrationMap(std::string Filename);
    
  void LoadBarEnergyCalibrationMap(std::string Filename);
  void LoadBarTofCalibrationMap(std::string Filename);
  void LoadBarElossTuning(std::string Filename);
  
  bool Exists(Int_t BarId);
   void ExportToFile(std::string FileName);

   TCalibrationMapType::iterator begin() { return fCalibrationMap.begin(); }
   TCalibrationMapType::iterator end()   { return fCalibrationMap.end();   }
   Int_t GetNumberOfBars() const         { return fCalibrationMap.size();  }
  Int_t GetNumberOfElossPos() const ;
  Int_t GetNumberOfTofPos() const;

  Double_t GetBarParameter(Int_t BarId, UInt_t ParameterNumber) { return fCalibrationMap[BarId][ParameterNumber];}
  Double_t GetPosElossParameter(Int_t PosId, Int_t layer, UInt_t ParameterNumber);
  Double_t GetPosTofParameter(Int_t PosId, Int_t layer, UInt_t ParameterNumber);

  Double_t GetBarElossParameter(Int_t BarId, Int_t layer, UInt_t ParameterNumber);
  Double_t GetBarTofParameter(Int_t BarId, Int_t layer, UInt_t ParameterNumber);
  Double_t GetBarElossTuningParameter(Int_t BarId, Int_t layer, UInt_t ParameterNumber);
   
   void SetBarParameter(Int_t BarId, UInt_t ParameterNumber, Double_t p) { fCalibrationMap[BarId][ParameterNumber]=p;}
   void AddBar(Int_t BarId) { fCalibrationMap[BarId].resize(NUMBEROFCALIBRATIONPARAMETERS);}

   ClassDef(TATWcalibrationMap, 0)
};

#endif
