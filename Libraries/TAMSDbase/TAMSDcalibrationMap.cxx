/*!
 \file TAMSDcalibrationMap.cxx
 \brief  Implementation of TAMSDcalibrationMap
 */

#include "TError.h"

#include "TAMSDcalibrationMap.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TAMSDcalibrationMap
 \brief Calibration map class
 */

//! Class Imp
ClassImp(TAMSDcalibrationMap)

//_____________________________________________________________________
//! Destructor
//!
//! \param[in] strip_number_p number of strips
TAMSDcalibrationMap::TAMSDcalibrationMap(int strip_number_p)
: TAGobject(),
  fStripsN{strip_number_p}
{
    fPedestal.reserve( fStripsN * 16 );
}

//_____________________________________________________________________
//! Load energy calibration map file
//!
//! \param[in] FileName input file
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
      double eta, eta_corr, signal;
      
      // loop over all the strips
      while (fin.getline(line, 200, '\n')) {
         
         if(strchr(line,'#')) {
            if(FootDebugLevel(1))
               Info("LoadEnergyCalibrationMap()","Skip comment line:: %s\n",line);
            continue;
         }
         
         sscanf(line, "%lf %lf %lf",&eta,&signal, &eta_corr);
         if(FootDebugLevel(1))
            Info("LoadEnergyCalibrationMap()","%lf %lf %lf\n",eta, signal, eta_corr);
         fEloss.eta.push_back(eta);
         fEloss.correction.push_back(eta_corr);  
         fEloss.signals.push_back(signal);  
      }
   } else
      Info("LoadEnergyCalibrationMap()","File Calibration Energy %s not open!!",FileName.Data());
   
   fin.close();
}

//_____________________________________________________________________
//! Load pedestal map file
//!
//! \param[in] FileName input file
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
      double ped_mean, ped_sigma;
      int noise_status;
      double sigmaLevelSeed, sigmaLevelHit;
      int k = 0;
      
      // loop over sigma level per sensor
      while (fin.getline(line, 200, '\n')) {
         
         // separate noise level and pedestal by ##
         if (line[0] == '#' && line[1] == '#')
            break;
         
         if(strchr(line,'#')) {
            if(FootDebugLevel(1))
               Info("LoadPedestalMap()","Skip comment line:: %s\n",line);
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
         sscanf(line, "%d %d %d %d %lf %lf %d",&sensorId, &stripId, &asicId, &asicCh, &ped_mean, &ped_sigma, &noise_status);
         if(FootDebugLevel(1))
            Info("LoadPedestalMap()","sensorId: %d stripId %d Mean: %5.1f Sigma: %3.1f status: %d\n",sensorId, stripId, ped_mean, ped_sigma, noise_status);
         fPedestal.push_back( PedParameter_t{ped_mean, ped_sigma, static_cast<bool>(noise_status)} );
      }
   } else
      Info("LoadPedestalMap()","File for pedestal %s not open!!",FileName.Data());
   
   fin.close();
}

//_____________________________________________________________________
//! Get energy loss parameter
//!
//! \param[in] eta eta value
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

//! Get energy loss parameter using corretion map
//! 
//! \param[in] eta eta value
//! \param[in] signal cluster signal value
Double_t TAMSDcalibrationMap::GetElossParam2D(Float_t eta, Float_t signal)
{  

   int eta_bin = -1;
   for (int i = 0; i < fEloss.eta.size(); i++) {
      if (eta < fEloss.eta[i]) {
         for(int i_sig = 0; i_sig < 800; i_sig++){
            if(sqrt(signal)<fEloss.signals[i+i_sig]){
               eta_bin = i+i_sig;
               break;
            }
         }
         break;
      }
   }
   if(fEloss.correction[eta_bin-1]!=0) return fEloss.correction[eta_bin-1];
   else return 1;
}