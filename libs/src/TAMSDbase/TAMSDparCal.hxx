#ifndef _TAMSDparCal_HXX
#define _TAMSDparCal_HXX

/*!
 \file TAMSDparCal.hxx
 \brief   Declaration of TAMSDparCal.
 */
/*------------------------------------------+---------------------------------*/

#include <string>
#include <map>
#include <TSystem.h>
#include <TF1.h>

#include "TAGroot.hxx"
#include "TAGparTools.hxx"
#include "TAGparaDsc.hxx"

#include "TAMSDcalibrationMap.hxx"

class TAMSDparCal : public TAGparTools
{
private:
  TAMSDcalibrationMap* fMapCal;  ///< Calibration map

public:
  TAMSDparCal(int strip_number_p);
  virtual ~TAMSDparCal();

  // Calibration
  Bool_t   LoadEnergyCalibrationMap(TString Filename);
  TAMSDcalibrationMap::ElossParameter_t GetElossParameters();

  // Pedestal
  Bool_t   LoadPedestalMap(TString Filename);
  TAMSDcalibrationMap::PedParameter_t GetPedestal(Int_t sensorId, Int_t stripId);

  Double_t GetPedestalThreshold(Int_t sensorId, TAMSDcalibrationMap::PedParameter_t const& pedestal_p, Bool_t seed);

  Double_t GetElossParam(Float_t eta);

 ClassDef(TAMSDparCal, 0)
};

#endif
