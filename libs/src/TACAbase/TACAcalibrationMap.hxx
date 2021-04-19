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

  TCalibrationMapType fCalibTemperatureCry;  // map for temperature calibration per cry ID
  TCalibrationMapType fEqualisFactorCry;     // map for equalisation factor per cry ID
  TCalibrationMapType fCalibElossMapCry;     // map for energy calibration per crystal

  TACAparMap *fParpMap;
   
public:
  TACAcalibrationMap();
    
  void LoadCryTemperatureCalibrationMap(std::string Filename);
  void LoadEnergyCalibrationMap(std::string Filename);

  bool Exists(Int_t cryId);
  void ExportToFile(std::string FileName);

  TCalibrationMapType::iterator begin() { return fCalibrationMap.begin(); }
  TCalibrationMapType::iterator end()   { return fCalibrationMap.end();   }

  Int_t GetNumberOfCrystals()                  const { return fCalibrationMap.size();          }
  Double_t GetTemperatureCry(Int_t cryId)            { return fCalibTemperatureCry[cryId][0];  }
  Double_t GetEqualiseCry(Int_t cryId)               { return fEqualisFactorCry[cryId][0];     }
  Double_t GetElossParam(Int_t cryId, UInt_t parId)  { return fCalibElossMapCry[cryId][parId]; }
 
  ClassDef(TACAcalibrationMap, 0)
};

#endif
