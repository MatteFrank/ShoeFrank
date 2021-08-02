#ifndef TAMSDCALIBRATIONMAP_H
#define TAMSDCALIBRATIONMAP_H
#include <map>
#include <TSystem.h>
#include <TString.h>
#include "TAGobject.hxx"

using namespace std;

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
   vector<Double_t>         fSigmaNoiseLevel;
   vector<PedParameter_t>   fPedestal;
   vector<ElossParameter_t> fEloss;
   Int_t                    fStripsN;

public:
  TAMSDcalibrationMap(int strip_number_p);
    
  void             LoadEnergyCalibrationMap(TString Filename);
  ElossParameter_t GetElossParameters(Int_t sensorId, Int_t stripId) { return fEloss[sensorId*fStripsN + stripId];      }

  void             LoadPedestalMap(TString Filename);
  PedParameter_t   GetPedestal(Int_t sensorId, Int_t stripId)        { return fPedestal[ sensorId*fStripsN + stripId];  }
  Double_t         GetPedestalNoiseLevel(Int_t sensorId)             { return fSigmaNoiseLevel[sensorId];               }

  ClassDef(TAMSDcalibrationMap, 0)
};

#endif
