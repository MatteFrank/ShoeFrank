#ifndef TAGFutilities_H
#define TAGFutilities_H

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

#include "TVector3.h"
#include "TAGobject.hxx"
#include "RectangularFinitePlane.h"
#include "SharedPlanePtr.h"



using namespace std;
using namespace genfit;

class DetectorIDMap {
  
public:
	DetectorIDMap();
	virtual ~DetectorIDMap() {};

	int		GetMinDetPlane(string detName);
	int		GetMaxDetPlane(string detName);
	int		GetPlaneTW();
	int		GetSensorsN(string detName);
	bool	IsSensorInDet(int SensorId, string detName);

	SharedPlanePtr	GetGenFitPlane(int planeId);

	map<string, vector<int>> m_DetToSensorMap;


private:

	int Nsensors;

};

#endif















