#include "TError.h"

#include "TACAparCal.hxx"

ClassImp(TACAparCal)

TACAparCal::TACAparCal()
: TAGparTools()
{
  fParGeo = (TACAparGeo*)gTAGroot->FindParaDsc(TACAparGeo::GetDefParaName(), "TACAparGeo")->Object();
}

//_____________________________________________________________________

Bool_t TACAparCal::LoadEnergyCalibrationMap(TString name)
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
      fCalibElossMapCrys[crysId].push_back(parameters[p]);
    }
  }
  
  delete [] parameters;
  
  Close();
  
  return true;
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
Double_t TACAparCal::GetElossParameter(Int_t crysId, UInt_t ParameterNumber)
{
    return fCalibElossMapCrys[crysId][ParameterNumber];
}

//_____________________________________________________________________
Double_t TACAparCal::GetTofParameter(Int_t crysId, UInt_t ParameterNumber)
{
    return fCalibTofMapCrys[crysId][ParameterNumber];
}

