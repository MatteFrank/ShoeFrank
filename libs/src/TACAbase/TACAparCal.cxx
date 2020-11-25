#include "TError.h"

#include "TACAparCal.hxx"

ClassImp(TACAparCal)

TACAparCal::TACAparCal()
: TAGparTools()
{
  fParGeo = (TACAparGeo*)gTAGroot->FindParaDsc(TACAparGeo::GetDefParaName(), "TACAparGeo")->Object();
}

//_____________________________________________________________________

void TACAparCal::LoadEnergyCalibrationMap(TString FileName)
{
  if (Open(FileName)) return;
  
  // read for parameter
  Double_t* parameters = new Double_t[4];
  Int_t crysId = -1;
  
  for (Int_t i = 0; i < fParGeo->GetCrystalsN(); ++i) { // Loop over crystal

    // Read crystal Id
    ReadItem(crysId);
    
    // read parameters
    ReadItem(parameters, 4, ' ');
    
    // fill map
    for (Int_t p = 0; p < 4; p++) { // Loop over parameters
      fCalibElossMapCrys[crysId].push_back(parameters[p]);
    }
  }
  
  delete [] parameters;
  
  Close();
}

//_____________________________________________________________________

void TACAparCal::LoadTofCalibrationMap(TString FileName)
{
  if (Open(FileName)) return;
  
  // read for parameter
  Double_t* parameters = new Double_t[4];
  Int_t crysId = -1;
  
  for (Int_t i = 0; i < fParGeo->GetCrystalsN(); ++i) { // Loop over crystal
    
    // Read crystal Id
    ReadItem(crysId);
    
    // read parameters
    ReadItem(parameters, 4, ' ');
    
    // fill map
    for (Int_t p = 0; p < 4; p++) { // Loop over parameters
      fCalibTofMapCrys[crysId].push_back(parameters[p]);
    }
  }
  
  delete [] parameters;
  
  Close();
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

