#ifndef _TACAparCal_HXX
#define _TACAparCal_HXX

/*!
 \file TACAparCal.hxx
 \brief   Declaration of TACAparCal.
 */
/*------------------------------------------+---------------------------------*/


#include <string>
#include <map>
#include <TSystem.h>

#include "TAGroot.hxx"
#include "TAGparTools.hxx"
#include "TACAparGeo.hxx"
#include "TAGparaDsc.hxx"

#include "TACAcalibrationMap.hxx"

using namespace std;
typedef std::map<Int_t,std::vector<Double_t> > TCalibrationMapType; ///< Calibration type

class TACAparCal : public TAGparTools
{
private:
  TACAparGeo*               fParGeo;            ///< geometry parameter
  TCalibrationMapType       fCalibTofMapCrys;   ///< map for tof calibration per crystal
  vector<bool>              fStatusCrys;        ///< status vector
  vector<float>             fStatusEmin;        ///< energy minimum vector
  map<int, pair<int, int> > fStatusCrysHwId;    ///< crystal HW id
  TACAcalibrationMap*       fMapCal;            ///< calibratiom type

public:
  TACAparCal();
  virtual ~TACAparCal();

  //! Calibration
  TACAcalibrationMap* getCalibrationMap() {return fMapCal;}

  Bool_t   LoadEnergyCalibrationMap(TString Filename);
  Bool_t   LoadCryTemperatureCalibrationMap(const TString& Filename);

  Double_t GetChargeEqParam(Int_t crystalId);
  Double_t GetADC2EnergyParam(UInt_t ParameterNumber);
  Double_t GetADC2TempParam(Int_t crystalId, UInt_t ParameterNumber);
  Double_t GetADC2EnergyParamCry(Int_t crystalId, UInt_t ParameterNumber);

  Bool_t  FromCrysStatusFile(const TString& name);
  void    GetCrysMap(Int_t crysId, Int_t& crysBoard, Int_t& crysCh);

  //! Get crystal status
  //! \param[in] id crystal id
  Bool_t  GetCrystalStatus(Int_t id) const { return fStatusCrys[id]; }
  //! Get crystal threshold
  //! \param[in] id crystal id
  Float_t GetCrystalThres(Int_t id)  const { return fStatusEmin[id]; }

private:
  static TString fgkCrysStatus;    ///< default bar status file

   ClassDef(TACAparCal, 0)
};

#endif
