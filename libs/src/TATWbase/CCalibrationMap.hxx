#ifndef CCALIBRATIONMAP_H
#define CCALIBRATIONMAP_H
#include <string>
#include <map>
#include <TSystem.h>
#include "Parameters.h"

typedef std::map<Int_t,std::vector<Double_t> > TCalibrationMapType;

class CCalibrationMap
{
private:
	TCalibrationMapType fCalibrationMap;
	bool fCalibrationMapIsOk;
   
public:
	CCalibrationMap();
	void LoadCalibrationMap(std::string Filename, Int_t verbose);
	bool Exists(Int_t BarId);
	TCalibrationMapType::iterator begin();
	TCalibrationMapType::iterator end();
	Int_t GetNumberOfBars();
	Double_t GetBarParameter(Int_t BarId, UInt_t ParameterNumber);
	void SetBarParameter(Int_t BarId, UInt_t ParameterNumber, Double_t p);
	void AddBar(Int_t BarId);
	void ExportToFile(std::string FileName);

};

#endif
