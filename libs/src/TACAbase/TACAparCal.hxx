#ifndef _TACAparCal_HXX
#define _TACAparCal_HXX
#include <string>
#include <map>
#include <TSystem.h>

#include "TAGroot.hxx"
#include "TAGparTools.hxx"
#include "TACAparGeo.hxx"
#include "TAGparaDsc.hxx"

#include "TACAcalibrationMap.hxx"

using namespace std;
typedef std::map<Int_t,std::vector<Double_t> > TCalibrationMapType;

class TACAparCal : public TAGparTools
{
private:
  TACAparGeo* fParGeo;
  TCalibrationMapType fCalibElossMapCrys; // map for energy calibration per crystal
  TCalibrationMapType fCalibTofMapCrys;   // map for tof calibration per crystal
  
  vector<bool> fStatusCrys;
  vector<float> fStatusEmin;
  map<int, pair<int, int> > fStatusCrysHwId;

  TACAcalibrationMap *fMapCal;
  TACAparMap *fParMap;
  
public:
  TACAparCal(TACAparMap *parMap);
  virtual ~TACAparCal();

  // Calibration
  inline TACAcalibrationMap* getCalibrationMap() {return fMapCal;}

  Bool_t LoadEnergyCalibrationMap(TString Filename);
  Bool_t LoadTofCalibrationMap(TString Filename);
  Bool_t FromCalibTempFile(const TString& Filename);
  
  Double_t GetElossParameter(Int_t crystalId, UInt_t ParameterNumber);
  Double_t GetTofParameter(Int_t crystalId, UInt_t ParameterNumber);

  Bool_t FromCrysStatusFile(const TString& name);
  
  Bool_t  GetCrystalStatus(Int_t id) const { return fStatusCrys[id]; }
  Float_t GetCrystalThres(Int_t id)  const { return fStatusEmin[id]; }
  void    GetCrysMap(Int_t crysId, Int_t& crysBoard, Int_t& crysCh);
  
private:
  static TString fgkCrysStatus;    // bar status file

  
   ClassDef(TACAparCal, 0)
};

#endif
