#ifndef TAMSDCALIBRATIONMAP_H
#define TAMSDCALIBRATIONMAP_H
#include <map>
#include <TSystem.h>
#include <TString.h>
#include "TAGobject.hxx"

using namespace std;
typedef map<pair<Int_t, Int_t>, vector<Double_t> > TCalibMapType;

class TAMSDcalibrationMap : public TAGobject
{
private:
   TCalibMapType fCalibElossMapStrip;     // map for energy calibration per strip
   TCalibMapType fCalibPedMapStrip;       // map for pedestal per strip
   vector<Float_t> fSigmaNoiseLevel;

public:
  TAMSDcalibrationMap();
    
  void     LoadEnergyCalibrationMap(TString Filename);
  Double_t GetElossParam(Int_t sensorId, Int_t stripId, UInt_t parId)    { return fCalibElossMapStrip[make_pair(sensorId, stripId)][parId]; }

  void     LoadPedestalMap(TString Filename);
  Double_t GetPedestalParam(Int_t sensorId, Int_t stripId, UInt_t parId) { return fCalibPedMapStrip[make_pair(sensorId, stripId)][parId];   }

  ClassDef(TAMSDcalibrationMap, 0)
};

#endif
