#include "TError.h"

#include "TACAcalibrationMap.hxx"
#include "TAGxmlParser.hxx"
#include "TAGrecoManager.hxx"

ClassImp(TACAcalibrationMap)

TACAcalibrationMap::TACAcalibrationMap()
: TAGobject()
{
}

//_____________________________________________________________________
void TACAcalibrationMap::LoadCryTemperatureCalibrationMap(std::string FileName)
{

  if (gSystem->AccessPathName(FileName.c_str())) {
     Error("LoadCryTemperatureCalibrationMap()","File %s doesn't exist",FileName.c_str());
  }

  ///////// read the file with Charge calibration

  ifstream fin;
  fin.open(FileName,std::ifstream::in);
  
  Int_t nCrystals = 0;
  
  Int_t cryId[nCrystals];  // Id of crystal
  Double_t temp[nCrystals];   // temperature
  Double_t equalis[nCrystals];   // equalis factor

  if(fin.is_open()){

    int  cnt(0);
    char line[200];
    
     fin.getline(line, 200, '\n');
     if(strchr(line,'#')) // skip first line if comment
        fin.getline(line, 200, '\n');
     sscanf(line, "%d", &nCrystals);
     
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
void TACAcalibrationMap::LoadEnergyCalibrationMap(std::string FileName)
{
   
   if (gSystem->AccessPathName(FileName.c_str()))
   {
      Error("LoadEnergyCalibrationMap()","File %s doesn't exist",FileName.c_str());
   }
   
   ///////// read the file with Charge calibration

   ifstream fin_Q;
   fin_Q.open(FileName,std::ifstream::in);
   
   // parameters for energy calibration p0 and p1
   Int_t nCrystals = 0;
   
   if(fin_Q.is_open()){
      
      int  cnt(0);
      char line[200];
      
      fin_Q.getline(line, 200, '\n');
      if(strchr(line,'#')) // skip first line if comment
         fin_Q.getline(line, 200, '\n');
      sscanf(line, "%d", &nCrystals);
      
      int crysId[nCrystals];  // Id of the crystal
      double Q_corrp0[nCrystals], Q_corrp1[nCrystals];
      
      // loop over all the slat crosses ( nSlatCross*nLayers ) for two TW layers
      while (fin_Q.getline(line, 200, '\n')) {
         
         if(strchr(line,'#')) {
            if(FootDebugLevel(1))
               Info("LoadEnergyCalibrationMap()","Skip comment line:: %s\n",line);
            continue;
         }
         
         sscanf(line, "%d %lf %lf",&crysId[cnt], &Q_corrp0[cnt], &Q_corrp1[cnt]);
         if(FootDebugLevel(1))
            Info("LoadEnergyCalibrationMap()","%d %.5f %.7f\n",crysId[cnt], Q_corrp0[cnt], Q_corrp1[cnt]);
         
         fCalibElossMapCry[crysId[cnt]].push_back(Q_corrp0[cnt]);
         fCalibElossMapCry[crysId[cnt]].push_back(Q_corrp1[cnt]);
         cnt++;
      }
   } else
      Info("LoadEnergyCalibrationMap()","File Calibration Energy %s not open!!",FileName.data());
   
   fin_Q.close();
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
