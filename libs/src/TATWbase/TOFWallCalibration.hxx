#ifndef TOFWALLCALIBRATION_H
#define TOFWALLCALIBRATION_H
#include "TTree.h"
#include <TH1D.h>
#include <utility>
#include <vector>
#include <map>

#include "Parameters.h"


class TOFWallCalibration
{
  //Maps used to store MC reference values
  std::map<std::pair<Int_t, Double_t>, std::pair<Double_t, Double_t>> _dERefValues, _TOFRefValues;

  //Raw Q and TOF histograms
  std::map<std::pair<Int_t, Double_t>, std::vector<TH1D*>> _hQFrontLayer;
  std::map<std::pair<Int_t, Double_t>, std::vector<TH1D*>> _hQRearLayer;
  std::map<std::pair<Int_t, Double_t>, std::vector<TH1D*>> _hTOFFrontLayer;
  std::map<std::pair<Int_t, Double_t>, std::vector<TH1D*>> _hTOFRearLayer;

  //Vector storing the beams found in the MC table
  //Used to control the MC table, initialize histograms and deallocate memory at the end
  std::vector<std::pair<Int_t, Double_t>> _PartEnPairsMC;

  //Vector storing the beams found in data
  //Used to run the calibration on the beams found
  std::vector<std::pair<Int_t, Double_t>> _PartEnPairsData;

  //Function used for the light output model -> Energy calibration
  TF1* _funcdECal;

  //Variable used to set debug mode
  Bool_t _Debug;

  //Functions
  void   InitHists();
  void   LoadSingleFileData(TString FileName);
  void   SaveRawHistograms(TString OutputDir);
  void   EnergyCalibration(TString OutputDir);
  void   TOFCalibration(TString OutputDir);
  Int_t  CalculatePositionID(Int_t Frontbar, Int_t Rearbar);

public:
  TOFWallCalibration();
  void   RunCalibration(TString OutputDir);
  void   LoadRecData(TString InputDataDir);
  void   LoadMCRefValues(TString MCRefValuesFile);
  void   ClearData();
  void   SetDebugMode();
};

#endif
