#include "TError.h"

#include "TACAparCal.hxx"

ClassImp(TACAparCal)



TString TACAparCal::fgkCrysStatus = "./config/TACACrysMapStatus.map";

//_____________________________________________________________________
TACAparCal::TACAparCal()
: TAGparTools()
{
  // Standard constructor
  fMapCal = new TACAcalibrationMap();
  fParGeo = (TACAparGeo*)gTAGroot->FindParaDsc(TACAparGeo::GetDefParaName(), "TACAparGeo")->Object();
}


//------------------------------------------+-----------------------------------
TACAparCal::~TACAparCal()
{
  if (fMapCal!=nullptr)
  {
    free (fMapCal);
  }
}

//------------------------------------------+-----------------------------------
Bool_t TACAparCal::FromCalibTempFile(const TString& name)
{

  Clear();

  TString name_calib_temp_cry = name;
  
  gSystem->ExpandPathName(name_calib_temp_cry);
  // fMapCal->LoadCalibrationMap(name_exp.Data());
  

  fMapCal->LoadCryTemperatureCalibrationMap(name_calib_temp_cry.Data());
  
  Info("FromCalibFile()", "Open file %s for Temperature calibration\n", name_calib_temp_cry.Data());

  return kFALSE;    
}  


//_________________________________________
Bool_t TACAparCal::LoadEnergyCalibrationMap(TString name)
{
   Clear();
   
   TString name_calib_en_cry = name;
   
   gSystem->ExpandPathName(name_calib_en_cry);
   
   fMapCal->LoadEnergyCalibrationMap(name_calib_en_cry.Data());
   
   Info("FromCalibFile()", "Open file %s for Energy calibration\n", name_calib_en_cry.Data());
   
   return kFALSE;
}

//_____________________________________________________________________

Bool_t TACAparCal::LoadTofCalibrationMap(TString name)
{
  if (!Open(name)) {
    Error("FromFile()", "Cannot open file %s", name.Data());
    return false;
  }
  
  // read for parameter
  Double_t* parameters = new Double_t[4];
  Int_t crysId = -1;
  
  for (Int_t i = 0; i < fParGeo->GetCrystalsN(); ++i) { // Loop over crystal
    
    // Read crystal Id
    ReadItem(crysId);
    
    // read parameters
    ReadItem(parameters, 4, ' ', false);
    
    // fill map
    for (Int_t p = 0; p < 4; p++) { // Loop over parameters
      fCalibTofMapCrys[crysId].push_back(parameters[p]);
    }
  }
  
  delete [] parameters;
  
  Close();
  
  return true;
}

//_____________________________________________________________________
Double_t TACAparCal::GetTemperatureCry(Int_t crysId)
{
   return fMapCal->GetTemperatureCry(crysId);
}

//_____________________________________________________________________
Double_t TACAparCal::GetElossParam(Int_t crysId, UInt_t ParameterNumber)
{
    return fMapCal->GetElossParam(crysId, ParameterNumber);
}

//_____________________________________________________________________
Double_t TACAparCal::GetTofParam(Int_t crysId, UInt_t ParameterNumber)
{
    return fCalibTofMapCrys[crysId][ParameterNumber];
}

//_____________________________________________________________________
void TACAparCal::GetCrysMap(Int_t crysId, Int_t& crysBoard, Int_t& crysCh)
{
  pair<int, int> id = fStatusCrysHwId[crysId];
  crysBoard = id.first;
  crysCh = id.second;
}

//_____________________________________________________________________
Bool_t TACAparCal::FromCrysStatusFile(const TString& name)
{
  TString nameExp;
  
  if (name.IsNull())
    nameExp = fgkCrysStatus;
  else
    nameExp = name;
  
  if (!Open(nameExp)) return false;
  
  if(FootDebugLevel(4))
    Info("FromCrysStatusFile()", "Open file %s", name.Data());
  
  Double_t* tmp = new Double_t[4];
  
  Int_t nCrys = fParGeo->GetCrystalsN();
  
  for (Int_t iCrys = 0; iCrys < nCrys; iCrys++) { // Loop over the bars
    
    // read parameters
    ReadItem(tmp, 4, ' ');
    
    Int_t crysboard = int(tmp[0]);
    Int_t crysCh    = int(tmp[1]);
    Float_t emin    = tmp[2];
    Int_t status    = bool(tmp[3]);

    if(FootDebugLevel(1))
      printf("crysid %d :crysboard %d crysCh %d Eloss_min %.2f active_crys %d\n", iCrys, crysboard, crysCh, emin, status);
    
    
    fStatusCrys.push_back(status);
    fStatusEmin.push_back(emin);
    pair<int, int> id(crysboard, crysCh);
    fStatusCrysHwId[iCrys] = id;
  }
  
  delete[] tmp;
  
  Close();
  
  return true;
}

