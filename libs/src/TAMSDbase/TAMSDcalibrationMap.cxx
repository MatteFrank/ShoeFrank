#include "TError.h"

#include "TAMSDcalibrationMap.hxx"
#include "TAGrecoManager.hxx"

ClassImp(TAMSDcalibrationMap)

//_____________________________________________________________________
TAMSDcalibrationMap::TAMSDcalibrationMap(int strip_number_p)
: TAGobject(),
fStripNumber{strip_number_p}
{
    fEloss.reserve( fStripNumber * 6 );
    fPedestal.reserve( fStripNumber * 6 );
}

//_____________________________________________________________________
void TAMSDcalibrationMap::LoadEnergyCalibrationMap(TString FileName)
{
   
   if (gSystem->AccessPathName(FileName.Data()))
      Error("LoadEnergyCalibrationMap()","File %s doesn't exist",FileName.Data());
   
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
         fEloss.push_back( ElossParameter_t{Q_corrp0, Q_corrp1} );
          
      }
   } else
      Info("LoadEnergyCalibrationMap()","File Calibration Energy %s not open!!",FileName.Data());
   
   fin.close();
}

//_____________________________________________________________________
void TAMSDcalibrationMap::LoadPedestalMap(TString FileName)
{
   
   if (gSystem->AccessPathName(FileName.Data()))
      Error("LoadPedestalMap()","File %s doesn't exist",FileName.Data());
   
   ///////// read the file with pedestal
   ifstream fin;
   fin.open(FileName,std::ifstream::in);
   
   // parameters for mean and sigma of pedestal
   if(fin.is_open()){
      char line[200];
      
      int sensorId, stripId;
      int asicId, asicCh;
      double Q_corrp0, Q_corrp1, Q_corrp2;
      double sigmaLevel;
      int k = 0;
      
      // loop over sigma level per sensor
      while (fin.getline(line, 200, '\n')) {
         
         // separate noise level and pedestal by ##
         if (line[0] == '#' && line[1] == '#')
            break;
         
         if(strchr(line,'#')) {
            if(FootDebugLevel(1))
               Info("LoadEnergyCalibrationMap()","Skip comment line:: %s\n",line);
            continue;
         }
        
         sscanf(line, "%lf", &sigmaLevel);
         fSigmaNoiseLevel.push_back(sigmaLevel);
         if(FootDebugLevel(1)) {
            Info("LoadPedestalMap()","sensorId: %d Sigma Level %.1f\n", k, fSigmaNoiseLevel[k]);
            k++;
         }
      }
      
      // loop over all the strips
      while (fin.getline(line, 200, '\n')) {
         
         if(strchr(line,'#')) {
            if(FootDebugLevel(1))
               Info("LoadEnergyCalibrationMap()","Skip comment line:: %s\n",line);
            continue;
         }
         
         sscanf(line, "%d %d %d %d %lf %lf %lf",&sensorId, &stripId, &asicId, &asicCh, &Q_corrp0, &Q_corrp1, &Q_corrp2);
         if(FootDebugLevel(1))
            Info("LoadPedestalMap()","sensorId: %d stripId %d Mean: %5.1f Sigma: %3.1f status: %d\n",sensorId, stripId, Q_corrp0, Q_corrp1, int(Q_corrp2));
         
         pair<int, int> p(sensorId, stripId);
         fCalibPedMapStrip[p].push_back(Q_corrp0);
         fCalibPedMapStrip[p].push_back(Q_corrp1);
         fCalibPedMapStrip[p].push_back(Q_corrp2);
         fPedestal.push_back( PedParameter_t{Q_corrp0, Q_corrp1, static_cast<bool>(true-Q_corrp2)} );
      }
   } else
      Info("LoadPedestalMap()","File for pedestal %s not open!!",FileName.Data());
   
   fin.close();
}
