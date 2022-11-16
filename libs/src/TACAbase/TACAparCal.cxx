/*!
 \file TACAparCal.cxx
 \brief  Implementation of TACAparCal
 */

#include "TError.h"
#include "TACAparCal.hxx"

/*!
 \class TACAparCal
 \brief Calibration class
 */

TString TACAparCal::fgkCrysStatus = "./config/TACACrysMapStatus.map";

//! Class Imp
ClassImp(TACAparCal)

//_____________________________________________________________________
//! Constructor
TACAparCal::TACAparCal()
: TAGparTools()
{
  // Standard constructor
  fMapCal = new TACAcalibrationMap();
  fParGeo = (TACAparGeo*)gTAGroot->FindParaDsc(TACAparGeo::GetDefParaName(), "TACAparGeo")->Object();
}


//------------------------------------------+-----------------------------------
//! Destructor
TACAparCal::~TACAparCal()
{
  if (fMapCal!=nullptr)
  {
    free (fMapCal);
  }
}

//_____________________________________________________________________
//! Load crystal temperature calibration map file
//!
//! \param[in] name input file
Bool_t TACAparCal::LoadCryTemperatureCalibrationMap(const TString& name)
{
  Clear();

  TString name_calib_temp_cry = name;
  
  gSystem->ExpandPathName(name_calib_temp_cry);
  // fMapCal->LoadCalibrationMap(name_exp.Data());
  

  fMapCal->LoadCryTemperatureCalibrationMap(name_calib_temp_cry.Data());
  
  Info("LoadCryTemperatureCalibrationMap()", "Open file %s for Temperature calibration\n", name_calib_temp_cry.Data());

  return kFALSE;    
}  

//_____________________________________________________________________
//! Load crystal energy calibration map file
//!
//! \param[in] name input file
Bool_t TACAparCal::LoadEnergyCalibrationMap(TString name)
{
   Clear();
   
   TString name_calib_en_cry = name;
   
   gSystem->ExpandPathName(name_calib_en_cry);
   
   fMapCal->LoadEnergyCalibrationMap(name_calib_en_cry.Data());
   
   Info("FromCalibFile()", "Open file %s for Energy calibration\n", name_calib_en_cry.Data());
   
   return kFALSE;
}


//_______________________________________________
//! Get temparture parameter
//!
//! \param[in] crysId crystal id
Double_t TACAparCal::GetTemperatureCry(Int_t crysId)
{
   return fMapCal->GetTemperatureCry(crysId);
}

//_______________________________________________
//! Get temparture equalized parameter
//!
//! \param[in] crysId crystal id
Double_t TACAparCal::GetEqualiseCry(Int_t crysId)
{
   return fMapCal->GetEqualiseCry(crysId);
}

//_______________________________________________
//! Get energy loss parameter
//!
//! \param[in] crysId crystal id
//! \param[in] ParameterNumber paramenter id
Double_t TACAparCal::GetElossParam(Int_t crysId, UInt_t ParameterNumber)
{
   return fMapCal->GetElossParam(crysId, ParameterNumber);
}

//_____________________________________________________________________
//! Get crystal map
//!
//! \param[in] crysId crystal id
//! \param[in] crysBoard crystal board id
//! \param[in] crysCh crystal channel id
void TACAparCal::GetCrysMap(Int_t crysId, Int_t& crysBoard, Int_t& crysCh)
{
   pair<int, int> id = fStatusCrysHwId[crysId];
   crysBoard = id.first;
   crysCh = id.second;
}

//_____________________________________________________________________
//! Load crystal status file
//!
//! \param[in] name input file
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

