#ifndef TACACALIBRATIONMAP_H
#define TACACALIBRATIONMAP_H
#include <map>
#include <TSystem.h>
#include <TString.h>
#include "TAGobject.hxx"

using namespace std;
typedef map<pair<Int_t, Int_t>, vector<Double_t> > TCalibrationMapType;

class TAMSDcalibrationMap : public TAGobject
{
private:
  TCalibrationMapType fCalibElossMapStrip;     // map for energy calibration per strip

public:
  TAMSDcalibrationMap();
    
  void     LoadEnergyCalibrationMap(TString Filename);
  Double_t GetElossParam(Int_t sensorId, Int_t stripId, UInt_t parId)  { return fCalibElossMapStrip[make_pair(sensorId, stripId)][parId]; }
 
  ClassDef(TAMSDcalibrationMap, 0)
};

#endif