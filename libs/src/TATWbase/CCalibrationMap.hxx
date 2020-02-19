#ifndef CCALIBRATIONMAP_H
#define CCALIBRATIONMAP_H
#include <string>
#include <map>
#include <TSystem.h>

#define NUMBEROFCALIBRATIONPARAMETERS 2

typedef Int_t TChannelId;
typedef Int_t TBarId;
typedef Int_t TBoardId;

typedef Double_t CalibrationParType;
typedef std::map<TBarId,std::vector<CalibrationParType> > TCalibrationMapType;

class CCalibrationMap
{
	TCalibrationMapType _CalibrationMap;
	bool _CalibrationMapIsOk;
public:
	CCalibrationMap();
	void LoadCalibrationMap(std::string Filename, int verbose);
	bool Exists(TBarId BarId);
	TCalibrationMapType::iterator begin();
	TCalibrationMapType::iterator end();
	Int_t GetNumberOfBars();
	CalibrationParType GetBarParameter(TBarId,unsigned int ParameterNumber);
	void SetBarParameter(TBarId,unsigned int ParameterNumber,CalibrationParType p);
	void AddBar(TBarId BarId);
	void ExportToFile(std::string FileName);

};

#endif
