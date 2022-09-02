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
   struct ElossParameter_t {
      double offset;  ///< offset
      double slope;   ///< slope
   };
   
  TCalibrationMapType       fCalibrationMap;       ///< Calibration map
  vector<Double_t>          fCalibTemperatureCry;  ///< map for temperature calibration per cry ID
  vector<Double_t>          fEqualisFactorCry;     ///< map for equalisation factor per cry ID
  vector<ElossParameter_t>  fCalibElossMapCry;     ///< map for energy calibration per crystal
  TACAparMap*               fParpMap;              ///< mapping file
   
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

  //! Get cystal number
  Int_t    GetCrystalsN()                      const { return fCalibrationMap.size();          }
  //! Get temperature parameter per crystal
  Double_t GetTemperatureCry(Int_t cryId)            { return fCalibTemperatureCry[cryId];     }
  //! Get equilized temperature per crystal
  Double_t GetEqualiseCry(Int_t cryId)               { return fEqualisFactorCry[cryId];        }
   
  Double_t GetElossParam(Int_t cryId, UInt_t parId);
 
  ClassDef(TACAcalibrationMap, 0)
};

#endif
