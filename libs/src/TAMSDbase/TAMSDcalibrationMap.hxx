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
public:
    struct pedestal_values {
        double mean;
        double sigma;
        bool status;
    };
    
    struct eloss_parameters {
        double offset;
        double slope;
    };
    
private:
   TCalibMapType fCalibElossMapStrip;     // map for energy calibration per strip
   TCalibMapType fCalibPedMapStrip;       // map for pedestal per strip
   vector<Double_t> fSigmaNoiseLevel;
    vector<pedestal_values> pedestal_c;
    vector<eloss_parameters> eloss_c;
    int strip_number_m;

public:
  TAMSDcalibrationMap(int strip_number_p);
    
  void     LoadEnergyCalibrationMap(TString Filename);
  Double_t GetElossParam(Int_t sensorId, Int_t stripId, UInt_t parId)  { return fCalibElossMapStrip[make_pair(sensorId, stripId)][parId]; }
  eloss_parameters GetElossParameters(Int_t sensorId, Int_t stripId) { return eloss_c[ sensorId*strip_number_m + stripId];}

  void     LoadPedestalMap(TString Filename);
    pedestal_values GetPedestal(Int_t sensorId, Int_t stripId) { return pedestal_c[ sensorId*strip_number_m + stripId ]; }
  Double_t GetPedestalParam(Int_t sensorId, Int_t stripId, UInt_t parId) { return fCalibPedMapStrip[make_pair(sensorId, stripId)][parId];   }
  Double_t GetPedestalNoiseLevel(Int_t sensorId)                         { return fSigmaNoiseLevel[sensorId];   }

  ClassDef(TAMSDcalibrationMap, 0)
};

#endif
