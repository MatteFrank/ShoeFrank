/*!
 \file TAGFdetectorMap.hxx
 \brief  Header for TAGFdetectorMap class
 \author R. Zarrella and M. Franchini
*/

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

class TAGFdetectorMap : public TAGobject {
  
public:
	TAGFdetectorMap();
	virtual ~TAGFdetectorMap();

	void	AddFitPlaneIDToDet(int planeId, string detName);
	int		GetFitPlaneID(string detName, int sensorId);
	int		GetMinFitPlane(string detName);
	int		GetMaxFitPlane(string detName);
	int		GetFitPlaneTW();
	int		GetFitPlaneTGcenter();
	int		GetFitPlaneTGairInterface();

	int		GetFitPlanesN(string detName="all");
	bool	IsFitPlaneInDet(int planeId, string detName);
	bool	GetSensorID(int planeId, int* sensorId);
	int		GetDetIDFromName(string detName);
	string	GetDetNameFromID(int detId);
	string	GetDetNameFromFitPlaneId(int planeId);
	bool	IsDetectorInMap(string detName);

	void	AddFitPlane(int planeId, SharedPlanePtr genfitPlane);
	void 	AddPlane_Zorder(float zPos, int indexOfPlane);
	void 	AddPlane_ZorderLocal(float zPos, int indexOfPlane);
	SharedPlanePtr	GetFitPlane(int planeId);

	int		GetMeasID_eventLevel(string detName, int sensorId, int hitId);
	int		GetMeasID_eventLevel(int planeId, int hitId);
	int		GetDetIDFromMeasID(int measId);
	string	GetDetNameFromMeasID(int measId);
	int		GetSensorIDFromMeasID(int measId);
	int		GetFitPlaneIDFromMeasID(int measId);
	int		GetHitIDFromMeasID(int measId);
	tuple<string, int, int> GetDetSensorHitFromMeasID(int measId);

	//IT functions
	vector<int>* GetPlanesAtZ( float zPos );
	vector<float>* GetPossibleITz();
	vector<int>* GetPlanesAtZLocal( float zPos );
	vector<float>* GetPossibleITzLocal();

	//MSD functions
	void	SetMSDsensorView(int iSensor, const int &view);
	int 	GetMSDsensorView(int iSensor) const;

private:
	map<string, vector<int>> m_DetToFitPlaneMap;	///< Map linking detector names to corresponding GenFit FitPlanes
	map<int, SharedPlanePtr> m_detectorPlanes;		///< Map linking FitPlane indices to the actual FitPlane objects
	map<string, int> m_detectorIndex;				///< Map linking detector names to their index

	map<float, vector<int> > m_zOrderingPlanes;		///< Map linking possible Z coordinates of IT planes w/ vector of their indices
	vector<float> m_itPossibleZ;					///< Vector with possible values for Z coordinate of IT planes
	map<float, vector<int>> m_zOrderingPlanesLocal;	///< Map linking possible Z local coordinates of IT planes w/ vector of their indices
	vector<float> m_itPossibleZLocal;				///< Vector with possible values for Z local coordinate of IT planes
	map<int, int> m_MSDviewMap;						///< Map linking MSD sensors to their view: X=0 (vertical strips), Y=1 (horizontal strips)

	ClassDef(TAGFdetectorMap,0);
};

#endif