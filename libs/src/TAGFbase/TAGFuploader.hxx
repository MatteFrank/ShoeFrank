#ifndef TAGFuploader_H
#define TAGFuploader_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <array>
#include <sys/stat.h>

#include <TROOT.h>

#include "TVector3.h"
#include "TAGobject.hxx"

// #include "WireMeasurement.h"
#include "PlanarMeasurement.h"
// #include "SpacepointMeasurement.h"
#include "SharedPlanePtr.h"
// #include "RectangularFinitePlane.h"

#include "TAGrecoManager.hxx"

#include "TAGroot.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

#include "TAGparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"

#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TATWntuPoint.hxx"
#include "TAGFdetectorMap.hxx"

#include "TAMCntuPart.hxx"


using namespace std;
using namespace genfit;

class TAGFuploader {
  
public:
  
  TAGFuploader ( TAGFdetectorMap* aSensorIDmap );
  
  virtual ~TAGFuploader() {}
  
  int TakeMeasHits4Fit(  map< int, vector<AbsMeasurement*> > &allHitMeas  );
  void GetPossibleCharges( vector<int>* chVect );

  map< int, vector<int> >* TakeMeasParticleMC_Collection();
  
private:

	int UploadClusVT();
	int UploadClusIT();
	int UploadClusMSD();
	int UploadHitsTW();

	int GetTWTrackFixed ( TATWpoint* point );


	void Prepare4Vertex( TAVTcluster* clus, int iClus );
	void Prepare4InnerTracker( TAITcluster* clus, int iClus );
	void Prepare4Strip( TAMSDcluster* clus, int iClus );
	void Prepare4TofWall( TATWpoint* point, int iPoint);

	TAGgeoTrafo* m_GeoTrafo;

	TAGFdetectorMap* m_sensorIDmap;

	map< int, vector<AbsMeasurement*> >* m_allHitMeas;
	map< int, vector<int> >* m_measParticleMC_collection;

	map<int, int> m_detectorPlaneID;

	string m_systemsON;
	int m_debug;
	bool switchOff_HHe;

};

#endif















