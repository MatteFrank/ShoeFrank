#ifndef TATWCALIBRATIONMAP_H
#define TATWCALIBRATIONMAP_H
#include <string>
#include <map>
#include <TSystem.h>
#include "TAGobject.hxx"
#include "Parameters.h"

typedef std::map<Int_t,std::vector<Double_t> > TCalibrationMapType;

class TATWcalibrationMap : public TAGobject
{
private:
	TCalibrationMapType fCalibrationMap;
	bool fCalibrationMapIsOk;
   
public:
	TATWcalibrationMap();
	void LoadCalibrationMap(std::string Filename);
	bool Exists(Int_t BarId);
   void ExportToFile(std::string FileName);

   TCalibrationMapType::iterator begin() { return fCalibrationMap.begin(); }
   TCalibrationMapType::iterator end()   { return fCalibrationMap.end();   }
   Int_t GetNumberOfBars() const         { return fCalibrationMap.size();  }
   Double_t GetBarParameter(Int_t BarId, UInt_t ParameterNumber) { return fCalibrationMap[BarId][ParameterNumber];}
   
   void SetBarParameter(Int_t BarId, UInt_t ParameterNumber, Double_t p) { fCalibrationMap[BarId][ParameterNumber]=p;}
   void AddBar(Int_t BarId) { fCalibrationMap[BarId].resize(NUMBEROFCALIBRATIONPARAMETERS);}

   ClassDef(TATWcalibrationMap, 0)
};

#endif
