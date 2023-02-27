/*!
  \file TAMSDparConf.cxx
  \brief   Implementation of TAMSDparConf.
*/

#include "TAGrecoManager.hxx"

#include "TAMSDparConf.hxx"

//##############################################################################

/*!
  \class TAMSDparConf
  \brief Map and Geometry parameters for MSD **
*/

//! Class imp
ClassImp(TAMSDparConf);


//______________________________________________________________________________
//! Standard constructor
TAMSDparConf::TAMSDparConf()
: TAVTbaseParConf()
{
   fkDefaultConfName = "./config/TAMSDdetector.cfg";
}

//______________________________________________________________________________
//! Destructor
TAMSDparConf::~TAMSDparConf()
{
}

//_____________________________________________________________________
//! From file
//!
//! \param[in] name input file
Bool_t TAMSDparConf::FromFile(const TString& name)
{
   // simple file reading, waiting for real config file
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultConfName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
  
   Int_t tmp;
   ReadItem(tmp);
   fAnalysisParameter.PedestalFlag = tmp;
   if(FootDebugLevel(1))
      cout << "Pedestal run flag: "<< fAnalysisParameter.PedestalFlag << endl;
   
   ReadItem(tmp);
   fAnalysisParameter.TrackingFlag = tmp;
   if(FootDebugLevel(1))
      cout << " Tracking flag: "<< fAnalysisParameter.TrackingFlag << endl;
   
   ReadItem(fAnalysisParameter.TrackingAlgo);
   if(FootDebugLevel(1))
      cout << "Tracking algorithm: "<< fAnalysisParameter.TrackingAlgo << endl;
   
   ReadItem(fAnalysisParameter.PlanesForTrackMinimum);
   if(FootDebugLevel(1))
      cout << "Minimum number of plane for tracking: "<< fAnalysisParameter.PlanesForTrackMinimum << endl;
   
   ReadItem(fAnalysisParameter.SearchHitDistance);
   if(FootDebugLevel(1))
      cout << "Maximum distance between cluster and corresponding track: "<< fAnalysisParameter.SearchHitDistance << endl;
   
   ReadItem(fSensorsN);
   if(FootDebugLevel(1))
      cout << endl << "Reading Parameters for "<< fSensorsN << " Sensors" << endl;
   
   for (Int_t p = 0; p < fSensorsN; p++) { // Loop on each plane
      
      // read sensor index
      ReadItem(fSensorParameter[p].SensorIdx);
      if(FootDebugLevel(1))
         cout << endl << " - Reading Parameters of Sensor " <<  fSensorParameter[p].SensorIdx << endl;
      
      // read sensor type
      ReadItem(fSensorParameter[p].Type);
      if(FootDebugLevel(1))
         cout << endl << "Type:   " <<  fSensorParameter[p].Type << endl;
      
      // read sensor status
      ReadItem(fSensorParameter[p].Status);
      if(FootDebugLevel(1))
         cout << "Status: "<< fSensorParameter[p].Status << endl;
      
      // read min of strip per cluster
      ReadItem(fSensorParameter[p].MinNofStripsInCluster);
      if(FootDebugLevel(1))
         cout << "Minimum number of strips per cluster: "<< fSensorParameter[p].MinNofStripsInCluster << endl;
      
      // read max of strip per cluster
      ReadItem(fSensorParameter[p].MaxNofStripsInCluster);
      if(FootDebugLevel(1))
         cout << "Maximum number of strips per cluster: "<< fSensorParameter[p].MaxNofStripsInCluster << endl;
      
      // read dead strips map
      fSensorParameter[p].DeadStripMap.Set(640);
      ReadItem(fSensorParameter[p].DeadStripMap);
      if(FootDebugLevel(1)) {
         cout << "Dead Strips: ";
         for (Int_t it = 0; it < fSensorParameter[p].DeadStripMap.GetSize(); ++it) {
            if (fSensorParameter[p].DeadStripMap[it])
               printf(" %d ", it);
         }
         cout << endl;
      }
   }
   
   return kFALSE;
}

