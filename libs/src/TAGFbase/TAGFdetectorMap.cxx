#include "TAGFdetectorMap.hxx"


//Constructor -> Detector indices are determined through a static map
TAGFdetectorMap::TAGFdetectorMap()
{ 
	m_detectorIndex.clear();
	vector<string> tmp_detName = { "SC", "BM", "TG", "VT", "IT", "MSD", "TW", "CA" };
	for(int i = 0; i<tmp_detName.size(); ++i)
	{
		m_detectorIndex[tmp_detName.at(i)] = i;
		m_DetToFitPlaneMap[tmp_detName[i]];
	}
}

//Destructor -> clean all the maps
TAGFdetectorMap::~TAGFdetectorMap()
{
	m_detectorIndex.clear();
	m_zOrderingPlanes.clear();
	m_itPossibleZ.clear();

	for(map<string, vector<int>>::iterator it = m_DetToFitPlaneMap.begin(); it != m_DetToFitPlaneMap.end(); ++it)
	{
		it->second.clear();
	}
	m_DetToFitPlaneMap.clear();

	for(map<int, SharedPlanePtr>::iterator it = m_detectorPlanes.begin(); it != m_detectorPlanes.end(); ++it)
	{
		delete &(it->second);
	}
	m_detectorPlanes.clear();
}


/***********************SENSOR/PLANE MAP****************************/

void TAGFdetectorMap::AddFitPlaneIDToDet(int planeId, string detName)
{
	if(m_DetToFitPlaneMap.find(detName) == m_DetToFitPlaneMap.end())
	{
		Error("AddFitPlaneIDToDet()", "Detector '%s' not found in DetToFitPlaneMap!", detName.c_str());
		throw -1;
	}
	m_DetToFitPlaneMap[detName].push_back(planeId);
}


void TAGFdetectorMap::AddPlane_Zorder(float zPos, int indexOfPlane)	{

	if ( m_zOrderingPlanes.find(zPos) == m_zOrderingPlanes.end())
		m_zOrderingPlanes[zPos];
	
	m_zOrderingPlanes[zPos].push_back(indexOfPlane);
	cout << "Assigned IT plane::" << indexOfPlane << " to zPos::" << zPos << endl;
	cout << "zPos::" << zPos << " has now " << m_zOrderingPlanes[zPos].size() << " planes!!" << endl;

	if ( find(m_itPossibleZ.begin(), m_itPossibleZ.end(), zPos) == m_itPossibleZ.end() )
	{
		m_itPossibleZ.push_back(zPos);
		std::sort(m_itPossibleZ.begin(), m_itPossibleZ.end());
		cout << "Added " << zPos << " to possible ITz" << endl;
	}

}





vector<int>* TAGFdetectorMap::GetPlanesAtZ( float zPos ) {

	return &m_zOrderingPlanes[zPos];
}


vector<float>* TAGFdetectorMap::GetPossibleITz() {

	return &m_itPossibleZ;
}



int TAGFdetectorMap::GetFitPlaneID(string detName, int sensorId)
{
	if(m_DetToFitPlaneMap.find(detName) == m_DetToFitPlaneMap.end())
	{
		Error("GetFitPlaneID()", "Detector '%s' not found in TAGFdetectorMap", detName.c_str());
		throw -1;
	}
	if(m_DetToFitPlaneMap.at(detName).size() < sensorId)
	{
		Error("GetFitPlaneID()", "Index of sensor %d is out of range for detector '%s'", sensorId, detName.c_str());
		throw -1;
	}
	else
		return m_DetToFitPlaneMap.at(detName).at(sensorId);
}


int TAGFdetectorMap::GetMinFitPlane(string detName)
{
	if(m_DetToFitPlaneMap.find(detName) == m_DetToFitPlaneMap.end())
	{
		Error("GetMinFitPlane()", "Detector '%s' not found in TAGFdetectorMap", detName.c_str());
		throw -1;
	}
	else
		return m_DetToFitPlaneMap.at(detName).at(0);
}


int TAGFdetectorMap::GetMaxFitPlane(string detName)
{
	if(m_DetToFitPlaneMap.find(detName) == m_DetToFitPlaneMap.end())
	{
		Error("GetMaxFitPlane()", "Detector '%s' not found in TAGFdetectorMap", detName.c_str());
		throw -1;
	}
	else
		return m_DetToFitPlaneMap.at(detName).back();
}


int TAGFdetectorMap::GetFitPlaneTW()
{
	if(m_DetToFitPlaneMap.find("TW") == m_DetToFitPlaneMap.end())
	{
		Error("GetFitPlaneTW()", "TW not found in TAGFdetectorMap ");
		throw -1;
	}

	if(m_DetToFitPlaneMap.at("TW").size() > 0)
	{
		return m_DetToFitPlaneMap.at("TW").at(0);
	}
	else
	{
		Error("GetFitPlneTW()", "No TW plane found in TAGFdetectorMap");
		throw -1;
	}
}


int TAGFdetectorMap::GetFitPlanesN(string detName)
{
	if(detName == "all")
		return m_detectorPlanes.size();
	else
		return m_DetToFitPlaneMap.at(detName).size();
}


bool TAGFdetectorMap::IsFitPlaneInDet(int planeId, string detName)
{
	if(planeId >= GetFitPlanesN())
	{
		Error("IsFitPlaneInDet()", "FitPlaneId %d is out of range!", planeId);
		throw -1;
	}

	bool check = false;
	for(int i = 0; i < m_DetToFitPlaneMap.at(detName).size(); ++i)
	{
		if(m_DetToFitPlaneMap.at(detName).at(i) == planeId)
		{
			check = true;
			break;
		}
	}
	return check;
}


bool TAGFdetectorMap::GetSensorID(int planeId, int* sensorId)
{
	bool check = false;
	if(planeId >= GetFitPlanesN())
	{
		Error("GetSensorID()", "FitPlaneId %d is out of range!", planeId);
		throw -1;
	}

	for(map<string, int>::iterator itDet = m_detectorIndex.begin(); itDet != m_detectorIndex.end(); ++itDet)
	{
		if(IsFitPlaneInDet(planeId, itDet->first))
		{
			for(int i=0; i < m_DetToFitPlaneMap.at(itDet->first).size(); ++i)
			{
				if( m_DetToFitPlaneMap.at(itDet->first).at(i) == planeId )
				{
					*sensorId = m_DetToFitPlaneMap.at(itDet->first).at(i);
					check = true;
					break;
				}
			}
		}
	}

	return check;
}


/**************** GENFIT PLANES HANDLING *************/

void TAGFdetectorMap::AddFitPlane(int planeId, SharedPlanePtr genfitPlane)
{
	m_detectorPlanes[planeId] = genfitPlane;
}


SharedPlanePtr TAGFdetectorMap::GetFitPlane(int planeId)
{
	if(m_detectorPlanes.find(planeId) == m_detectorPlanes.end())
	{
		Error("GetFitPlane()", "No GenFit plane with ID %d found in TAGFdetectorMap", planeId);
		throw -1;
	}
	else
		return m_detectorPlanes.at(planeId);
}



/******************** DETECTOR MAP ********************/

int TAGFdetectorMap::GetDetIDFromName(string detName)
{
	if(m_detectorIndex.find(detName) == m_detectorIndex.end())
	{
		Error("GetDetIDFromName()", "Detector '%s' not found in TAGFdetectorMap", detName.c_str());
		throw -1;
	}
	else
		return m_detectorIndex.at(detName);
}


string TAGFdetectorMap::GetDetNameFromID(int detId)
{
	for(map<string, int>::iterator it = m_detectorIndex.begin(); it != m_detectorIndex.end(); ++it)
	{
		if(it->second == detId)
			return it->first;
	}
	Error("GetDetNameFromID()", "Index %d is not associated with any detector", detId);
	throw -1;
}

bool TAGFdetectorMap::IsDetectorInMap(string detName)
{
	if(m_detectorIndex.find(detName) == m_detectorIndex.end())
		return false;
	else
		return true;
}



/************************ GLOBAL MEASUREMENT ID ******************************/

int TAGFdetectorMap::GetMeasID_eventLevel(string detName, int sensorId, int hitId)
{
	if(m_detectorIndex.find(detName) == m_detectorIndex.end())
	{
		Error("GetMeasID_eventLevel()", "Detector '%s' not found in detectorIndex map", detName.c_str());
		throw -1;
	}

	if(m_DetToFitPlaneMap.at(detName).size() < sensorId)
	{
		Error("GetMeasID_eventLevel()", "Sensor index %d is out of range for detector '%s'", sensorId, detName.c_str());
		throw -1;
	}

	// cout << "TAGFdetectorMap::GetMeasID_eventLevel    = " << m_detectorIndex.at(detName)*1E7 <<" " << sensorId*1E5 << " "<< hitId <<"  =  "  << (m_detectorIndex.at(detName)*1E7 + sensorId*1E5 + hitId) << endl;

	return m_detectorIndex.at(detName)*1E7 + sensorId*1E5 + hitId;
}


int TAGFdetectorMap::GetMeasID_eventLevel(int planeId, int hitId)
{
	string detName = "dummy";
	int sensorId;

	if(planeId >= GetFitPlanesN())
	{
		Error("GetMeasID_eventLevel()", "FitPlaneId %d is out of range!", planeId);
		throw -1;
	}

	bool found = false;

	for(map<string, int>::iterator itDet = m_detectorIndex.begin(); itDet != m_detectorIndex.end(); ++itDet)
	{
		if(found)
			break;

		cout << "GetMeasID_eventLevel()\t" << itDet->first << "\t" << itDet->second << endl;
		cout << "IsFitPlaneInDet\tplane::" << planeId << "\tbool::" << IsFitPlaneInDet(planeId, itDet->first) << endl;
		//WRITE THIS IN A MORE EFFICIENT WAY!!! It just works right now
		if(!found && IsFitPlaneInDet(planeId, itDet->first))
		{
			cout << "GetMeasID_eventLevel()\tDetGood" << endl;
			for(int i=0; i < m_DetToFitPlaneMap.at(itDet->first).size(); ++i)
			{
				cout << "GetMeasID_eventLevel()\ti::" << i << endl;
				if( m_DetToFitPlaneMap.at(itDet->first).at(i) == planeId )
				{
					sensorId = i;
					detName = itDet->first;
					found = true;
					break;
				}
			}
		}
	}

	if( detName == "dummy" )
	{
		Error("GetMeasID_eventLevel()", "FitPlaneId %d not found!", planeId);
		throw -1;
	}


	cout << "det::" << detName << "\tsensorId::" << sensorId << "\thitId::" << hitId << endl;
	return GetMeasID_eventLevel(detName, sensorId, hitId);
}





int TAGFdetectorMap::GetDetIDFromMeasID(int measId)
{
	int Index = measId/1E7;
	if(Index > m_detectorIndex.size())
	{
		Error("GetDetIDFromMeasID()", "Detector index %d is out of range for MeasId %d!" , Index, measId);
		throw -1;
	}
	else
		return Index;
}







string TAGFdetectorMap::GetDetNameFromMeasID(int measId)
{
	int Index = measId/1E7;
	
	if(Index > m_detectorIndex.size())
	{
		Error("GetDetNameFromMeasID()", "Detector index %d is out of range for MeasId %d!" , Index, measId);
		throw -1;
	}

	return GetDetNameFromID(Index);
}


int TAGFdetectorMap::GetSensorIDFromMeasID(int measId)
{
	int sensorId = ( measId % int(1E7) ) / 1E5;
	string detName = GetDetNameFromMeasID(measId);
	
	if(sensorId > m_DetToFitPlaneMap.at(detName).size())
	{
		Error("GetSensorIDFromMeasID()", "SensorID %d is out of range for detector '%s'", sensorId, detName.c_str());
		throw -1;
	}
	return sensorId;
}


int TAGFdetectorMap::GetFitPlaneIDFromMeasID(int measId)
{
	int sensorId = GetSensorIDFromMeasID(measId);
	string detName = GetDetNameFromMeasID(measId);

	return GetFitPlaneID(detName, sensorId);
}


int TAGFdetectorMap::GetHitIDFromMeasID(int measId)
{
	return measId % int(1E5);
}


tuple<string, int, int> TAGFdetectorMap::GetDetSensorHitFromMeasID(int measId)
{
	string detName = GetDetNameFromMeasID(measId);
	int sensorId = GetSensorIDFromMeasID(measId);
	int hitID = GetHitIDFromMeasID(measId);
	return make_tuple(detName, sensorId, hitID);
}







