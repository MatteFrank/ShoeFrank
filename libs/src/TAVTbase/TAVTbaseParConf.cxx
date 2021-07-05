/*!
  \file
  \version $Id: TAVTbaseParConf.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TAVTbaseParConf.
*/

#include <Riostream.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TList.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"

#include "TAGrecoManager.hxx"
#include "TAVTbaseParConf.hxx"

//##############################################################################

/*!
  \class TAVTbaseParConf TAVTbaseParConf.hxx "TAVTbaseParConf.hxx"
  \brief Map and Geometry parameters for Tof wall. **
*/

ClassImp(TAVTbaseParConf);

//______________________________________________________________________________
TAVTbaseParConf::TAVTbaseParConf()
: TAGparTools(),
  fSensorsN(0)
{
   // Standard constructor
}

//______________________________________________________________________________
TAVTbaseParConf::~TAVTbaseParConf()
{
   // Destructor
}

//______________________________________________________________________________
Bool_t TAVTbaseParConf::FromFile(const TString& name) 
{   
   // simple file reading, waiting for real config file
   TString nameExp;
   
   if (name.IsNull())
     nameExp = fkDefaultConfName;
   else 
     nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   // read position algorithme
   ReadItem(fAnalysisParameter.TracksMaximum);  
   if(FootDebugLevel(1))
      cout << "Maximum tracks per event: "<< fAnalysisParameter.TracksMaximum << endl;
   
   ReadItem(fAnalysisParameter.PlanesForTrackMinimum);  
   if(FootDebugLevel(1))
     cout << "Minimum number of plane for tracking: "<< fAnalysisParameter.PlanesForTrackMinimum << endl;
   
   ReadItem(fAnalysisParameter.SearchHitDistance);  
   if(FootDebugLevel(1))
     cout << "Maximum distance between cluster and corresponding track: "<< fAnalysisParameter.SearchHitDistance << endl;
   
   ReadItem(fAnalysisParameter.TrackChi2Limit);  
   if(FootDebugLevel(1))
     cout << "Limit of Chi2 for tracks: "<< fAnalysisParameter.TrackChi2Limit << endl;
      
   ReadItem(fAnalysisParameter.BmTrackChi2Limit);  
   if(FootDebugLevel(1))
	  cout << "Limit of Chi2 for BM tracks: "<< fAnalysisParameter.BmTrackChi2Limit << endl;
   
   ReadItem(fSensorsN);  
   if(FootDebugLevel(1))
     cout << endl << "Reading Parameters for "<< fSensorsN << " Sensors" << endl;
   
   for (Int_t p = 0; p < fSensorsN; p++) { // Loop on each plane
     
     // read sensor index
	  ReadItem(fSensorParameter[p].SensorIdx);
      if(FootDebugLevel(1))
       cout << endl << " - Reading Parameters of Sensor " <<  fSensorParameter[p].SensorIdx << endl;
     
     // read sensor status
	  ReadItem(fSensorParameter[p].Status);
      if(FootDebugLevel(1))
       cout << "Status: "<< fSensorParameter[p].Status << endl;
     
     // read min of pixel per cluster
	  ReadItem(fSensorParameter[p].MinNofPixelsInCluster);  
      if(FootDebugLevel(1))
       cout << "Minimum number of pixels per cluster: "<< fSensorParameter[p].MinNofPixelsInCluster << endl;
     
     // read max of pixel per cluster
	  ReadItem(fSensorParameter[p].MaxNofPixelsInCluster);  
      if(FootDebugLevel(1))
       cout << "Maximum number of pixels per cluster: "<< fSensorParameter[p].MaxNofPixelsInCluster << endl;
      
      // read dead pixel map
      ReadItem(fSensorParameter[p].DeadPixelMap);
      if(FootDebugLevel(1)) {
         cout << "Dead Pixels: ";
         map<pair<int, int>, int>::iterator it;
         for (it = fSensorParameter[p].DeadPixelMap.begin(); it != fSensorParameter[p].DeadPixelMap.end(); it++) {
            pair<int, int> pair = it->first;
            cout << "(" << pair.first << ":" << pair.second << ") ";
         }
         cout << endl;
      }
   }	  
   
   return kFALSE;
}

//------------------------------------------+-----------------------------------
//! Get major number status
Int_t TAVTbaseParConf::GetStatus(Int_t idx) const
{
   Int_t major = fSensorParameter[idx].Status/10;
   if (major == 0)
      return fSensorParameter[idx].Status;
   else
      return major;
   
}

//------------------------------------------+-----------------------------------
//! Get minor number status
Int_t TAVTbaseParConf::GetStatusMinor(Int_t idx) const
{
   Int_t major = fSensorParameter[idx].Status/10;
   if (major == 0) {
      Warning("GetStatusMinor", "No status minor number!");
      return -99;
   }  else
      return (fSensorParameter[idx].Status % 10);
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.
void TAVTbaseParConf::Clear(Option_t*)
{
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAVTbaseParConf::ToStream(ostream& os, Option_t*) const
{
//  os << "TAVTbaseParConf " << GetName() << endl;
//  os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
//     << "   ver_x   ver_y   ver_z  width" << endl;

  return;
}

