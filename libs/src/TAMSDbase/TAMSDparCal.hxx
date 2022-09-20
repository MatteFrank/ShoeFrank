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
   TF1*                fFunc1;   ///< 1st function
   TF1*                fFunc2;   ///< 2nd function
   TF1*                fFunc3;   ///< 3rd function
   TF1*                fFuncEta; ///< Eta function
   
private:
   void SetFunctions();

public:
  TAMSDparCal(int strip_number_p);
  virtual ~TAMSDparCal();

  // Calibration
  Bool_t   LoadEnergyCalibrationMap(TString Filename);
  TAMSDcalibrationMap::ElossParameter_t GetElossParameters();

  // Pedestal
  Bool_t   LoadPedestalMap(TString Filename);
  TAMSDcalibrationMap::PedParameter_t GetPedestal(Int_t sensorId, Int_t stripId);

  Double_t GetPedestalValue(Int_t sensorId, TAMSDcalibrationMap::PedParameter_t const& pedestal_p, Bool_t seed);

  Float_t ComputeEtaChargeCorrection(Float_t eta);
  Float_t ComputeEtaPosCorrection(Float_t eta);
  Double_t GetElossParam(Float_t eta);

 ClassDef(TAMSDparCal, 0)
};

#endif
