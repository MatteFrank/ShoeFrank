#include "TError.h"

#include "TAMSDcalibrationMap.hxx"
#include "TAGxmlParser.hxx"
#include "TAGrecoManager.hxx"

ClassImp(TAMSDcalibrationMap)

TAMSDcalibrationMap::TAMSDcalibrationMap()
: TAGobject()
{
}

//_____________________________________________________________________
void TAMSDcalibrationMap::LoadEnergyCalibrationMap(std::string FileName)
{
   
   if (gSystem->AccessPathName(FileName.c_str()))
      Error("LoadEnergyCalibrationMap()","File %s doesn't exist",FileName.c_str());
   
   ///////// read the file with Charge calibration
   ifstream fin;
   fin.open(FileName,std::ifstream::in);
   
   // parameters for energy calibration p0 and p1
   if(fin.is_open()){
      char line[200];
      
      int sensorId, stripId;
      double Q_corrp0, Q_corrp1;
      
      // loop over all the strips
      while (fin.getline(line, 200, '\n')) {
         
         if(strchr(line,'#')) {
            if(FootDebugLevel(1))
               Info("LoadEnergyCalibrationMap()","Skip comment line:: %s\n",line);
            continue;
         }
         
         sscanf(line, "%d %d %lf %lf",&sensorId, &stripId, &Q_corrp0, &Q_corrp1);
         if(FootDebugLevel(1))
            Info("LoadEnergyCalibrationMap()","%d %d %.5f %.7f\n",sensorId, stripId, Q_corrp0, Q_corrp1);
         pair<int, int> p(sensorId, stripId);
         fCalibElossMapStrip[p].push_back(Q_corrp0);
         fCalibElossMapStrip[p].push_back(Q_corrp1);
      }
   } else
      Info("LoadEnergyCalibrationMap()","File Calibration Energy %s not open!!",FileName.data());
   
   fin.close();
}
