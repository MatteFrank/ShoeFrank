/*!
 \file TAGFdetectorMap.cxx
 \brief  Utility class used to handle the detectors, sensors, clusters/points and GenFit planes indices
 \author R. Zarrella and M. Franchini
*/

#include "TAGFdetectorMap.hxx"

/*!
 \class TAGFdetectorMap
 \brief Utility class used to handle the detectors, sensors, clusters/points and GenFit planes indices

 Class used to handle all the indices used in the global reconstruciton routine with GenFit:
 - Detector -> Either name ("VT", "MSD", etc....) or index
 - Sensor -> Local index of sensor in the detector (e.g. 0-3 for "VT", 0-32 for "IT", etc....)
 - FitPlane -> Global GenFit plane index (does not restart for each detector)
 - Hit -> Local index of the hit in the current sensor/plane
 - Measurement -> Global index of the measurement defined combining detector, sensor and hit indices
*/

//! \brief Default constructor
//!
//! Detector indices are determined through a static map: { "SC", "BM", "TG", "VT", "IT", "MSD", "TW", "CA" }
TAGFdetectorMap::TAGFdetectorMap()
{ 
	m_detectorIndex.clear();
	vector<string> tmp_detName = { "SC", "BM", "TG", "VT", "IT", "MSD", "TW", "CA" };
	for(int i = 0; i<tmp_detName.size(); ++i)
	{
		m_detectorIndex[tmp_detName.at(i)] = i;
		m_DetToFitPlaneMap[tmp_detName[i]];
	}

	m_debug = TAGrecoManager::GetPar()->Debug();
}

//! \brief Default destructor
//!
//! Clean all the maps
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

//! \brief Add an index to FitPlane for a detector
//!
//! \param[in] planeId Index of the FitPlane to add
//! \param[in] detName Name of the detector where to add the plane
void TAGFdetectorMap::AddFitPlaneIDToDet(int planeId, string detName)
{
	if(m_DetToFitPlaneMap.find(detName) == m_DetToFitPlaneMap.end())
	{
		Error("AddFitPlaneIDToDet()", "Detector '%s' not found in DetToFitPlaneMap!", detName.c_str());
		throw -1;
	}
	m_DetToFitPlaneMap[detName].push_back(planeId);
}


//! \brief Add an IT plane to the map assigned to its Z coordinate
//!
//! \param[in] zPos Z coordinate of the FitPlane to add
//! \param[in] indexOfPlane Index of the plane to add
void TAGFdetectorMap::AddPlane_Zorder(float zPos, int indexOfPlane)	{

	if ( m_zOrderingPlanes.find(zPos) == m_zOrderingPlanes.end())
		m_zOrderingPlanes[zPos];
	
	m_zOrderingPlanes[zPos].push_back(indexOfPlane);
	if(m_debug > 0)
	{
		cout << "Assigned IT plane::" << indexOfPlane << " to zPos::" << zPos << "\nzPos::" << zPos << " has now " << m_zOrderingPlanes[zPos].size() << " planes!!\n";
	}

	if ( find(m_itPossibleZ.begin(), m_itPossibleZ.end(), zPos) == m_itPossibleZ.end() )
	{
		m_itPossibleZ.push_back(zPos);
		std::sort(m_itPossibleZ.begin(), m_itPossibleZ.end());
		
		if(m_debug > 0)	cout << "Added " << zPos << " to possible ITz\n";
	}

}


//! \brief Get the vector of IT planes' indices at a given Z coordinate
//!
//! \param[in] zPos Z coordinate of the IT planes
//! \return Pointer to the vector of plane indices
vector<int>* TAGFdetectorMap::GetPlanesAtZ( float zPos ) {

	return &m_zOrderingPlanes[zPos];
}


//! \brief Get the possible values for the Z coordinate of IT planes
//!
//! \return Pointer to vector of possible values for Z coordinate of IT planes
vector<float>* TAGFdetectorMap::GetPossibleITz() {

	return &m_itPossibleZ;
}


//! \brief Get the index of a FitPlane
//!
//! \param[in] detName name of the detector
//! \param[in] sensorId Local index of the sensor
//! \return Index of a FitPlane
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


//! \brief Get the minimum FitPlane index for a detector
//!
//! \param[in] detName Name of the detector
//! \return Minimum FitPlane index for the detector
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


//! \brief Get the maximum FitPlane index for a detector
//!
//! \param[in] detName Name of the detector
//! \return Maximum FitPlane index for the detector
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


//! \brief Get the index of the TW FitPlane
//!
//! \return Index of the TW FitPlane
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


//! \brief Get the number of FitPlanes for the whole setup or for a detector
//!
//! \param[in] detName Name of the detector (default=="all")
//! \return Number of FitPlanes in the detector or in the full setup if detName=="all"
int TAGFdetectorMap::GetFitPlanesN(string detName)
{
	if(detName == "all")
		return m_detectorPlanes.size();
	else
		return m_DetToFitPlaneMap.at(detName).size();
}


//! \brief Check if a FitPlane is contained in a detector
//!
//! \param[in] planeId Index of the FitPlane
//! \param[in] detName Name of the detector
//! \return True if the FitPlane is associated to the detector
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


//! \brief Get the local sensor index of a FitPlane
//!
//! \param[in] planeId Index of the FitPlane
//! \param[out] sensorId Pointer used to store the local index of the sensor
//! \return True if the sensor was found
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
		if( IsFitPlaneInDet(planeId, itDet->first) )
		{
			for(int i=0; i < m_DetToFitPlaneMap.at(itDet->first).size(); ++i)
			{
				if( m_DetToFitPlaneMap.at(itDet->first).at(i) == planeId )
				{
					*sensorId = m_DetToFitPlaneMap.at(itDet->first).at(i) - GetMinFitPlane(itDet->first);
					check = true;
					break;
				}
			}
		}
	}

	return check;
}


/**************** GENFIT PLANES HANDLING *************/

//! \brief Add a FitPlane to the detector map
//!
//! \param[in] planeId Index of the Plane
//! \param[in] genfitPlane Pointer to the allocated plane
void TAGFdetectorMap::AddFitPlane(int planeId, SharedPlanePtr genfitPlane)
{
	m_detectorPlanes[planeId] = genfitPlane;
}

//! \brief Get a GenFit FitPlane
//!
//! \param[in] planeId Index of the FitPlane
//! \return Pointer to the FitPlane
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

//! \brief Get the index of a detector
//!
//! \param[in] deteName name of the detector
//! \return index of the detector
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

//! \brief Get the name of a detector
//!
//! \param[in] detId Index of the detector
//! \return Name of the detector
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

//! \brief Check if a detector has been loaded in the map
//!
//! \param[in] detName Name of the detector
//! \return True if the detector has been loaded correctly
bool TAGFdetectorMap::IsDetectorInMap(string detName)
{
	if(m_detectorIndex.find(detName) == m_detectorIndex.end())
		return false;
	else
		return true;
}



/************************ GLOBAL MEASUREMENT ID ******************************/

//! \brief Get the global index of the measurement 
//!
//! \param[in] detName Name of the detector
//! \param[in] sensorId Local index of the sensor
//! \param[in] hitId Index of the cluster in the sensor
//! \return Global index of the measurement constructed as 1E7*(detector index) + 1E5*(sensor index) + cluster index
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

	return m_detectorIndex.at(detName)*1E7 + sensorId*1E5 + hitId;
}


//! \brief Get the global index of the measurement 
//!
//! \param[in] planeId Index of the GenFit FitPlane
//! \param[in] hitId Index of the cluster in the sensor
//! \return Global index of the measurement constructed as 1E7*(detector index) + 1E5*(sensor index) + cluster index
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

		if(m_debug > 1)
		{
			Info("GetMeasId_eventLevel()", "%s\t%d", itDet->first.c_str(), itDet->second);
			Info("GetMeasId_eventLevel()", "plane::%d\tIsFitPlaneInDet::%d", planeId, IsFitPlaneInDet(planeId, itDet->first));
		}
		//WRITE THIS IN A MORE EFFICIENT WAY!!! It just works right now
		if(!found && IsFitPlaneInDet(planeId, itDet->first))
		{
			if(m_debug > 1)	cout << "GetMeasID_eventLevel()\tDetGood\n";
			
			for(int i=0; i < m_DetToFitPlaneMap.at(itDet->first).size(); ++i)
			{
				if(m_debug > 1)	cout << "GetMeasID_eventLevel()\ti::" << i << "\n";
				
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


	if(m_debug > 1)
		cout << "det::" << detName << "\tsensorId::" << sensorId << "\thitId::" << hitId << "\n";
	return GetMeasID_eventLevel(detName, sensorId, hitId);
}



//! \brief Get the detector index from the global measurement id
//!
//! \param[in] measId Global measurement index
//! \return Detector index
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



//! \brief Get the detector name from the global measurement index
//!
//! \param[in] measId Global measurement index
//! \return Detector name
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


//! \brief Get local sensor Id from measurement index
//!
//! \param[in] measId Global measurement index
//! \return Local sensor index
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


//! \brief Get FitPlane index from global measurement id
//!
//! \param[in] measId Global measurement index
//! \return FitPlane index
int TAGFdetectorMap::GetFitPlaneIDFromMeasID(int measId)
{
	int sensorId = GetSensorIDFromMeasID(measId);
	string detName = GetDetNameFromMeasID(measId);

	return GetFitPlaneID(detName, sensorId);
}


//! \brief Get local cluster/point index from global measurement id
//!
//! \param[in] measId Global measurement index
//! \return Local cluster/point index
int TAGFdetectorMap::GetHitIDFromMeasID(int measId)
{
	return measId % int(1E5);
}


//! \brief Get all detector-sensor-cluster information from global measurement index
//!
//! \param[in] measId Global measurement index
//! \return Tuple containing detector name, local sensor index and cluster/point index
tuple<string, int, int> TAGFdetectorMap::GetDetSensorHitFromMeasID(int measId)
{
	string detName = GetDetNameFromMeasID(measId);
	int sensorId = GetSensorIDFromMeasID(measId);
	int hitID = GetHitIDFromMeasID(measId);
	return make_tuple(detName, sensorId, hitID);
}
