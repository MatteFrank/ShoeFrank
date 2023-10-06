#ifndef TACACALIBRATIONMAP_H
#define TACACALIBRATIONMAP_H

/*!
 \file TACAcalibrationMap.hxx
 \brief   Declaration of TACAcalibrationMap.
 */
/*------------------------------------------+---------------------------------*/

#include <string>
#include <map>
#include <TSystem.h>

#include "TAGobject.hxx"
#include "TAGparTools.hxx"
#include "TACAparGeo.hxx"
#include "TACAparMap.hxx"
#include "TAGaction.hxx"

typedef std::map<Int_t,std::vector<Double_t> > TCalibrationMapType; ///< Calibration type
using namespace std;

class TACAcalibrationMap : public TAGobject
{
private:
   //! \struct ElossParameter_t
   struct ADC2EnergyParam_t {
      double p0; 
      double p1;
      double p2; 
      double p3;
      double p4;
      double p5;
      double p6;
      double p7;
      double p8;
      double p9;
      double p10;
      double p11;
   };
  
   struct ADC2TempParam_t {
      double p0; 
      double p1;
      double p2; 
   };
   

  TCalibrationMapType       fCalibrationMap;     ///< Calibration map
  vector<ADC2TempParam_t>   fCalibTempMapCry;    ///< map for temperature calibration per crystal
  ADC2EnergyParam_t         fCalibEnergyMap;     ///< map for energy calibration 
  vector<double>            fCalibChargeMap;     ///< map for charge calibration per crystal

  TACAparMap*               fParpMap;            ///< mapping file
   
public:
  TACAcalibrationMap();
    
  void LoadCryTemperatureCalibrationMap(std::string Filename);
  void LoadEnergyCalibrationMap(std::string Filename);

  bool Exists(Int_t cryId);
  void ExportToFile(std::string FileName);

  //! Calibration map iterator begin
  TCalibrationMapType::iterator begin()              { return fCalibrationMap.begin();         }
  //! Calibration map iterator end
  TCalibrationMapType::iterator end()                { return fCalibrationMap.end();           }

  //! Get crystal number
  Int_t    GetCrystalsN()                      const { return fCalibrationMap.size();          }
    
  Double_t GetADC2EnergyParam(UInt_t parId);
   
  Double_t GetChargeEqParam(UInt_t cryId);

  Double_t GetADC2TempParam(Int_t cryId, UInt_t parId);
 
  ClassDef(TACAcalibrationMap, 0)
};

#endif
