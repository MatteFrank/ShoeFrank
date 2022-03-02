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
  
   Int_t cryId;  // Id of crystal
   Double_t temp;   // temperature
   Double_t equalis;   // equalis factor

  if(fin.is_open()){

    char line[200];
    
     fin.getline(line, 200, '\n');
     if(strchr(line,'/') || strchr(line,'#')) // skip first line if comment
        fin.getline(line, 200, '\n');
     sscanf(line, "%d", &nCrystals);
  
     fCalibTemperatureCry.reserve(nCrystals);
     fEqualisFactorCry.reserve(nCrystals);

     
    // loop over all the slat crosses ( nSlatCross*nLayers ) for two TW layers
    while (fin.getline(line, 200, '\n')) {

       if(strchr(line,'/') || strchr(line,'#')){ // skip first line if comment
        if(FootDebugLevel(1))
          Info("LoadCryTemperatureCalibrationMap()","Skip comment line:: %s\n",line);
        continue;
      }

      sscanf(line, "%d %lf %lf",&cryId, &temp, &equalis);
    
       fCalibTemperatureCry[cryId] = temp;
       fEqualisFactorCry[cryId] = equalis;

       
       if(FootDebugLevel(1))
        Info("LoadCryTemperatureCalibrationMap()","%d %f %f\n",cryId, temp, equalis);

      //Info("********************LoadCryTemperatureCalibrationMap()","%d %f %f\n",cryId, temp, equalis);
    }
  }
  else
    Info("LoadCryTemperatureCalibrationMap()","File Calibration Energy %s not open!!",FileName.data());

  fin.close();
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
      if(strchr(line,'/') || strchr(line,'#'))  // skip first line if comment
         fin_Q.getline(line, 200, '\n');
      sscanf(line, "%d", &nCrystals);
      fCalibElossMapCry.reserve(nCrystals);
      
      int crysId;  // Id of the crystal
      double Q_corrp0, Q_corrp1;
      
      // loop over all the slat crosses ( nSlatCross*nLayers ) for two TW layers
      while (fin_Q.getline(line, 200, '\n')) {
         
         if(strchr(line,'/') || strchr(line,'#'))  {
            if(FootDebugLevel(1))
               Info("LoadEnergyCalibrationMap()","Skip comment line:: %s\n",line);
            continue;
         }
         
         sscanf(line, "%d %lf %lf",&crysId, &Q_corrp0, &Q_corrp1);
         if(FootDebugLevel(1))
            Info("LoadEnergyCalibrationMap()","%d %.5f %.7f\n",crysId, Q_corrp0, Q_corrp1);
         
         fCalibElossMapCry[crysId] = ElossParameter_t{Q_corrp0, Q_corrp1};
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

//_______________________________________________
Double_t TACAcalibrationMap::GetElossParam(Int_t cryId, UInt_t parId)
{
   if (parId == 0)
      return fCalibElossMapCry[cryId].offset;
   else if (parId == 1)
      return fCalibElossMapCry[cryId].slope;
   else {
      Error("GetElossParam()", "No parameter %d found", parId);
      return -99999;
   }
}
