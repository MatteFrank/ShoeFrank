#ifndef TOFWALLCALIBRATION_H
#define TOFWALLCALIBRATION_H
#include "TTree.h"
#include <TH1D.h>
#include <utility>
#include <vector>
#include <map>

#include "../../../libs/src/TATWbase/Parameters.h"

typedef struct {
  TString       ExpName;
  Int_t         runNb;
  ParticleType  PartID;
  Double_t      BeamEn;
  std::map<TLayer, Double_t> dE_MC;
  std::map<TLayer, Double_t> TOF_MC;
} MCRefType;

typedef std::pair<ParticleType, Double_t> PartEnPairType;

class TOFWallCalibration
{
  //Map used to store the X and Y bars corresponding to a TW position
  std::map<Int_t, std::pair<Int_t, Int_t>> _PosIDtoXYBarsMap;

  //Vector storing the beams found in the MC table
  //Used to control that the MC table is properly filled and to identify the files to load
  //NB: if a run is not included in the MC table, the corresponding data file will NOT be loaded
  std::vector<MCRefType> _MCRefValues;

  //Vector storing the pointers to all the files used for calibration
  std::vector<TFile*> _ListOfFiles;

  //Function used for the light output model -> Energy calibration
  TF1* _funcdECal;

  //Variable used to set debug mode
  Bool_t _Debug;

  //Functions
  void   LoadRecData(TString InputDir);
  void   LoadMCRefValues(TString MCTableFile);
  void   TOFCalibration(TString OutputDir);
  void   CalibrateLayerTOF(TFile* f, MCRefType MCRef, std::ofstream* ofsSHOE, TLayer layer, TString OutputDir, TFile* fOutDebug);
  void   EnergyCalibration(TString OutputDir);
  void   CalibrateLayerDeltaE(std::ofstream* ofsSHOE, TLayer layer, TString OutputDir, TFile* fOutDebug);
  Int_t  CalculatePositionID(Int_t XBar, Int_t YBar);
  void   CalculatePosIDtoBarsMap();

public:
  TOFWallCalibration();
  void   RunCalibration(TString MCTableFile, TString InputDir, TString OutputDir);
  void   SetDebugMode();
};

#endif
