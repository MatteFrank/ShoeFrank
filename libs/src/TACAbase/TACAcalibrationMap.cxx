#include "TError.h"

#include "TACAcalibrationMap.hxx"
#include "TAGxmlParser.hxx"
#include "TAGrecoManager.hxx"

ClassImp(TACAcalibrationMap)

TACAcalibrationMap::TACAcalibrationMap(TACAparMap* p_parmap)
: TAGobject(),
fpCalMap(p_parmap)
{
  // fCalMap = new TACAparMap();
}

//_____________________________________________________________________
void TACAcalibrationMap::LoadCryTemperatureCalibrationMap(std::string FileName)
{

  if (gSystem->AccessPathName(FileName.c_str()))
    {
    Error("LoadCryTemperatureCalibrationMap()","File %s doesn't exist",FileName.c_str());
    } 

  ///////// read the file with Charge calibration

  ifstream fin;
  fin.open(FileName,std::ifstream::in);
  
  Int_t nCrystals = fpCalMap->GetCrystalsN();
  
  Int_t cryId[nCrystals];  // Id of crystal
  Double_t temp[nCrystals];   // temperature
  Double_t equalis[nCrystals];   // equalis factor

  if(fin.is_open()){

    int  cnt(0);
    char line[200];
    
    // loop over all the slat crosses ( nSlatCross*nLayers ) for two TW layers
    while (fin.getline(line, 200, '\n')) {

      if(strchr(line,'#')) {
        if(FootDebugLevel(1))
          Info("LoadCryTemperatureCalibrationMap()","Skip comment line:: %s\n",line);
        continue;
      }

      sscanf(line, "%d %lf %lf",&cryId[cnt],&temp[cnt],&equalis[cnt]);
    
      if(FootDebugLevel(1))
        Info("LoadCryTemperatureCalibrationMap()","%d %f %f\n",cryId[cnt],temp[cnt],equalis[cnt]);
      
      cnt++;
    }

  }
  else
    Info("LoadCryTemperatureCalibrationMap()","File Calibration Energy %s not open!!",FileName.data());

  fin.close();

  
  // store in a Map the temperature parameter for the temperature correction with key their cry Id
  for(int i=0; i<nCrystals; i++) {


      fCalibTemperatureCry[cryId[i]].push_back(temp[i]);
      fEqualisFactorCry[cryId[i]].push_back(equalis[i]);

      if(FootDebugLevel(1))
        cout<<"Crystal ID: "<<cryId[i]<<" Temperature: "<<temp[i]<<" Equalisation factor: "<<equalis[i]<<endl;

    // cout<<"Crystal ID: "<<cryId[i]<<" Temperature: "<<temp[i]<<endl;
    
  }

}

//_____________________________________________________________________
void TACAcalibrationMap::ExportToFile(std::string FileName)
{
  TAGxmlParser x;
  XMLNodePointer_t main=x.CreateMainNode("CALIBRATION");
  x.AddElementWithContent(TString::Format("DATE").Data(),main," ");
  x.AddElementWithContent(TString::Format("DESCRIPTION").Data(),main," ");
  for (auto it=fCalibrationMap.begin();it!=fCalibrationMap.end();++it)
  {
    Int_t cryId=it->first;
    XMLNodePointer_t b=x.AddElement(TString::Format("CRY").Data(),main);
    x.AddElementWithContent("CRY_ID",b, TString::Format("%d",cryId).Data());
    x.AddElementWithContent("T",b,TString::Format("%f",fCalibrationMap[cryId][0]).Data());
    x.AddElementWithContent("EQ",b,TString::Format("%f",fCalibrationMap[cryId][0]).Data());
  }
  x.ExportToFile(FileName,main);
}

//_______________________________________________
bool TACAcalibrationMap::Exists(Int_t cryId)
{
  if (fCalibrationMap.count(cryId)==0)
  {
    return false;
  }
  return true;
}
