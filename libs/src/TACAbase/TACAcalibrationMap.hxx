#ifndef TACACALIBRATIONMAP_H
#define TACACALIBRATIONMAP_H
#include <string>
#include <map>
#include <TSystem.h>
#include "TAGobject.hxx"
//#include "Parameters.h"
#include "TAGparTools.hxx"
#include "TACAparGeo.hxx"
#include "TACAparMap.hxx"
#include "TAGaction.hxx"

typedef std::map<Int_t,std::vector<Double_t> > TCalibrationMapType;

class TACAcalibrationMap : public TAGobject
{
private:
	TCalibrationMapType fCalibrationMap;

  TCalibrationMapType fCalibTemperatureCry;   // map for temperature calibration per cry ID

  TACAparMap *fpCalMap;
  // TAGparaDsc *fpParMap;
  

public:
  TACAcalibrationMap(TACAparMap* p_parmap);
    
  void LoadCryTemperatureCalibrationMap(std::string Filename);
  
  bool Exists(Int_t cryId);
  void ExportToFile(std::string FileName);

  TCalibrationMapType::iterator begin() { return fCalibrationMap.begin(); }
  TCalibrationMapType::iterator end()   { return fCalibrationMap.end();   }

  Int_t GetNumberOfCrystals() const         { return fCalibrationMap.size();  }
  Double_t GetTemperatureCry(Int_t cryId) { return fCalibTemperatureCry[cryId][0]; }
 
  ClassDef(TACAcalibrationMap, 0)
};

#endif
