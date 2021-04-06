#include "TAGFutilities.hxx"


DetectorIDMap::DetectorIDMap()
{ }

int DetectorIDMap::GetMinDetPlane(string detName)
{
	return m_DetToSensorMap.at(detName).at(0);
}

int DetectorIDMap::GetMaxDetPlane(string detName)
{
	return m_DetToSensorMap.at(detName).at( m_DetToSensorMap.at(detName).size() - 1);
}

int DetectorIDMap::GetPlaneTW()
{
	if(m_DetToSensorMap.at("TW").size() > 0)
	{
		return m_DetToSensorMap.at("TW").at(0);
	}
	else
	{
		cout << "No TW plane found in Sensor Map " << endl, exit(0);
		return -1;
	}
}

int DetectorIDMap::GetSensorsN(string detName)
{
	return m_DetToSensorMap.at(detName).size();
}

bool DetectorIDMap::IsSensorInDet(int SensorId, string detName)
{
	bool check = false;
	for(int i = 0; i < m_DetToSensorMap.at(detName).size(); ++i)
	{
		if(m_DetToSensorMap.at(detName).at(i) == SensorId)
		{
			check = true;
			break;
		}
	}
	return check;
}

//RZ: Write this function from scratch!!
SharedPlanePtr DetectorIDMap::GetGenFitPlane(int planeId)
{
	SharedPlanePtr PlanePtr;
	return PlanePtr;
}
