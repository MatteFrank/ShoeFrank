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
        vector<double> eta;
        vector<double> correction;
    };

private:
   vector<Double_t>           fSigmaNoiseLevelSeed;
   vector<Double_t>           fSigmaNoiseLevelHit;
   vector<PedParameter_t>     fPedestal;
   ElossParameter_t           fEloss;
   Int_t                      fStripsN;

public:
  TAMSDcalibrationMap(int strip_number_p);
    
  void                      LoadEnergyCalibrationMap(TString Filename);
  ElossParameter_t          GetElossParameters()                              { return fEloss;                                   }

  void                      LoadPedestalMap(TString Filename);
  PedParameter_t            GetPedestal(Int_t sensorId, Int_t stripId)       { return fPedestal[ sensorId*fStripsN + stripId];  }
  Double_t                  GetPedestalSeedLevel(Int_t sensorId)             { return fSigmaNoiseLevelSeed[sensorId];               }
  Double_t                  GetPedestalHitLevel(Int_t sensorId)              { return fSigmaNoiseLevelHit[sensorId];               }
  Double_t                  GetElossParam(Float_t eta);

  ClassDef(TAMSDcalibrationMap, 0)
};

#endif
