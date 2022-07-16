#include "TError.h"

#include "TAMSDcalibrationMap.hxx"
#include "TAGrecoManager.hxx"

ClassImp(TAMSDcalibrationMap)

//_____________________________________________________________________
TAMSDcalibrationMap::TAMSDcalibrationMap(int strip_number_p)
: TAGobject(),
  fStripsN{strip_number_p}
{
    fPedestal.reserve( fStripsN * 16 );
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
      
      double eta, eta_corr;
      
      // loop over all the strips
      while (fin.getline(line, 200, '\n')) {
         
         if(strchr(line,'#')) {
            if(FootDebugLevel(1))
               Info("LoadEnergyCalibrationMap()","Skip comment line:: %s\n",line);
            continue;
         }
         
         sscanf(line, "%lf %lf",&eta, &eta_corr);
         if(FootDebugLevel(1))
            Info("LoadEnergyCalibrationMap()","%lf %lf\n",eta, eta_corr);
         fEloss.eta.push_back(eta);
         fEloss.correction.push_back(eta_corr);          
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
      double sigmaLevelSeed, sigmaLevelHit;
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
        
         sscanf(line, "%lf", &sigmaLevelSeed);
         fSigmaNoiseLevelSeed.push_back(sigmaLevelSeed);
         fin.getline(line, 200, '\n');
         sscanf(line, "%lf", &sigmaLevelHit);
         fSigmaNoiseLevelHit.push_back(sigmaLevelHit);
         if(FootDebugLevel(1)) {
            Info("LoadPedestalMap()","sensorId: %d Sigma Level Seed %.1f - Sigma Level Hit %.1f\n", k, fSigmaNoiseLevelSeed[k], fSigmaNoiseLevelHit[k]);
            sleep(1);
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
         
         fPedestal.push_back( PedParameter_t{Q_corrp0, Q_corrp1, static_cast<bool>(true-Q_corrp2)} );
      }
   } else
      Info("LoadPedestalMap()","File for pedestal %s not open!!",FileName.Data());
   
   fin.close();
}

Double_t TAMSDcalibrationMap::GetElossParam(Float_t eta)
{  
   // find the eta bin
   int eta_bin = -1;
   for (int i = 0; i < fEloss.eta.size(); i++) {
      if (eta < fEloss.eta[i]) {
         eta_bin = i;
         break;
      }
   }

   return fEloss.correction[eta_bin-1];
}