#ifndef TAGFdetectorMap_H
#define TAGFdetectorMap_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <math.h>
#include <array>
#include <sys/stat.h>

#include "TAGrecoManager.hxx"
#include "TVector3.h"
#include "TAGobject.hxx"
#include "RectangularFinitePlane.h"
#include "SharedPlanePtr.h"



using namespace std;
using namespace genfit;

/**** Class used to handle all the indices used in the global reconstruciton routine with GenFit
 * Detector -> Either name ("VT", "MSD", etc....) or index
 * Sensor -> Local index of sensor in the detector (e.g. 0-3 for "VT", 0-32 for "IT", etc....)
 * FitPlane -> Global GenFit plane index (does not restart for each detector)
 * Hit -> Local index of the hit in the current sensor/plane
 * Measurement -> Global index of the measurement defined combining detector, sensor and hit indices
 */


class TAGFdetectorMap {
  
public:
	TAGFdetectorMap();
	virtual ~TAGFdetectorMap();

	void	AddFitPlaneIDToDet(int planeId, string detName);
	int		GetFitPlaneID(string detName, int sensorId);
	int		GetMinFitPlane(string detName);
	int		GetMaxFitPlane(string detName);
	int		GetFitPlaneTW();

	int		GetFitPlanesN(string detName="all");
	bool	IsFitPlaneInDet(int planeId, string detName);
	bool	GetSensorID(int planeId, int* sensorId);
	int		GetDetIDFromName(string detName);
	string	GetDetNameFromID(int detId);
	bool	IsDetectorInMap(string detName);

	void	AddFitPlane(int planeId, SharedPlanePtr genfitPlane);
	void 	AddPlane_Zorder(float zPos, int indexOfPlane);
	SharedPlanePtr	GetFitPlane(int planeId);

	int		GetMeasID_eventLevel(string detName, int sensorId, int hitId);
	int		GetMeasID_eventLevel(int planeId, int hitId);
	int		GetDetIDFromMeasID(int measId);
	string	GetDetNameFromMeasID(int measId);
	int		GetSensorIDFromMeasID(int measId);
	int		GetFitPlaneIDFromMeasID(int measId);
	int		GetHitIDFromMeasID(int measId);
	tuple<string, int, int> GetDetSensorHitFromMeasID(int measId);

	vector<int>* GetPlanesAtZ( float zPos );
	vector<float>* GetPossibleITz();

private:
	map<string, vector<int>> m_DetToFitPlaneMap;
	map<int, SharedPlanePtr> m_detectorPlanes;
	map<string, int> m_detectorIndex;

	map<float, vector<int> > m_zOrderingPlanes;
	vector<float> m_itPossibleZ;

	int m_debug;
};

#endif