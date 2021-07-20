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
    struct PedParameter_t {
        double mean;
        double sigma;
        bool status;
    };
    
    struct ElossParameter_t {
        double offset;
        double slope;
    };
    
private:
   TCalibMapType            fCalibElossMapStrip;     // map for energy calibration per strip
   TCalibMapType            fCalibPedMapStrip;       // map for pedestal per strip
   vector<Double_t>         fSigmaNoiseLevel;
   vector<PedParameter_t>   fPedestal;
   vector<ElossParameter_t> fEloss;
   Int_t                    fStripNumber;

public:
  TAMSDcalibrationMap(int strip_number_p);
    
  void     LoadEnergyCalibrationMap(TString Filename);
  Double_t GetElossParam(Int_t sensorId, Int_t stripId, UInt_t parId)  { return fCalibElossMapStrip[make_pair(sensorId, stripId)][parId]; }
  ElossParameter_t GetElossParameters(Int_t sensorId, Int_t stripId) { return fEloss[ sensorId*fStripNumber + stripId];}

  void     LoadPedestalMap(TString Filename);
    PedParameter_t GetPedestal(Int_t sensorId, Int_t stripId) { return fPedestal[ sensorId*fStripNumber + stripId ]; }
  Double_t GetPedestalParam(Int_t sensorId, Int_t stripId, UInt_t parId) { return fCalibPedMapStrip[make_pair(sensorId, stripId)][parId];   }
  Double_t GetPedestalNoiseLevel(Int_t sensorId)                         { return fSigmaNoiseLevel[sensorId];   }

  ClassDef(TAMSDcalibrationMap, 0)
};

#endif
