/*!
 \file TAGFuploader.hxx
 \brief  Header for TAGFuploader class
 \author M. Franchini and R. Zarrella
*/

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

#include "PlanarMeasurement.h"
#include "SharedPlanePtr.h"

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
	virtual ~TAGFuploader();
	
	int TakeMeasHits4Fit(  map< int, vector<AbsMeasurement*> > &allHitMeas  );
	void GetPossibleCharges( vector<int>* chVect, bool IsMC );
	int GetNumGenParticle_noFrag();

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

	TAGgeoTrafo* m_GeoTrafo;								///< GeoTrafo object for reference frame changes

	TAGFdetectorMap* m_sensorIDmap;							///< TAGFdetectorMap ptr for index handling

	map< int, vector<AbsMeasurement*> >* m_allHitMeas;		///< Container for GenFit AbsMeasurements, FitPlane index to vector of respective measurements
	map< int, vector<int> >* m_measParticleMC_collection;	///< Maps global measurement index to MC particles in the cluster/point

	string m_systemsON;										///< String containing all systems on in the campaign
	int m_debug;											///< Global debug value
	bool switchOff_HHe;										///< Boolean flag to switch off light fragments

};

#endif















