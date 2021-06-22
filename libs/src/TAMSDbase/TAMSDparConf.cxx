/*!
  \file
  \version $Id: TAMSDparConf.cxx
  \brief   Implementation of TAMSDparConf.
*/

#include "TAGrecoManager.hxx"

#include "TAMSDparConf.hxx"

//##############################################################################

/*!
  \class TAMSDparConf TAMSDparConf.hxx "TAMSDparConf.hxx"
  \brief Map and Geometry parameters for MSD **
*/

ClassImp(TAMSDparConf);


//______________________________________________________________________________
TAMSDparConf::TAMSDparConf()
: TAVTbaseParConf()
{
   // Standard constructor
   fkDefaultConfName = "./config/TAMSDdetector.cfg";
}

//______________________________________________________________________________
TAMSDparConf::~TAMSDparConf()
{
   // Destructor
}

//______________________________________________________________________________
Bool_t TAMSDparConf::FromFile(const TString& name)
{
   // simple file reading, waiting for real config file
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultConfName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
      
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
      
      // read min of pixel per cluster
      ReadItem(fSensorParameter[p].MinNofStripsInCluster);
      if(FootDebugLevel(1))
         cout << "Minimum number of pixels per cluster: "<< fSensorParameter[p].MinNofStripsInCluster << endl;
      
      // read max of pixel per cluster
      ReadItem(fSensorParameter[p].MaxNofStripsInCluster);
      if(FootDebugLevel(1))
         cout << "Maximum number of pixels per cluster: "<< fSensorParameter[p].MaxNofStripsInCluster << endl;
      
      // read dead pixel map
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

//------------------------------------------+-----------------------------------
//! Get major number status
Int_t TAMSDparConf::GetStatus(Int_t idx) const
{
   Int_t major = fSensorParameter[idx].Status/10;
   if (major == 0)
      return fSensorParameter[idx].Status;
   else
      return major;
   
}

//------------------------------------------+-----------------------------------
//! Get minor number status
Int_t TAMSDparConf::GetStatusMinor(Int_t idx) const
{
   Int_t major = fSensorParameter[idx].Status/10;
   if (major == 0) {
      Warning("GetStatusMinor", "No status minor number!");
      return -99;
   }  else
      return (fSensorParameter[idx].Status % 10);
}
