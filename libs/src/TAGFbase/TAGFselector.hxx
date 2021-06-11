#ifndef TAGFselector_H
#define TAGFselector_H

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

#include "TVector3.h"
#include "TAGobject.hxx"
#include "Track.h"
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

#include "TAMCntuPart.hxx"

#include <KalmanFitterRefTrack.h>
#include <KalmanFitter.h>
#include <KalmanFitterInfo.h>
#include <KalmanFitStatus.h>
#include <DAF.h>
#include <RKTrackRep.h>
#include <Track.h>

#include <Exception.h>
#include <FieldManager.h>
#include <StateOnPlane.h>

#include <TrackPoint.h>
#include "RKTrackRep.h"
#include "KalmanFitter.h"
#include "KalmanFittedStateOnPlane.h"
#include "TAVTntuVertex.hxx"
#include "TAVTtrack.hxx"
#include "UpdatePDG.hxx"
#include "TAGFdetectorMap.hxx"


using namespace std;
using namespace genfit;

class TAGFselector {

public:

	TAGFselector(map< int, vector<AbsMeasurement*> >* allHitMeas, vector<int>* chVect, 
						TAGFdetectorMap* SensorIDmap, map<TString, Track*>* trackCategoryMap, 
						map< int, vector<int> >* measParticleMC_collection);

	virtual ~TAGFselector() { }

	int			Categorize( );
	TString		GetRecoTrackName(Track* tr);
	int			GetChargeFromTW(Track* trackToCheck);

private:

	int			FillTrackRepVector();
	bool		PrefitRequirements( map< string, vector<AbsMeasurement*> >::iterator element ); //CHECK!!!!!!

	int			Categorize_TruthMC( );
	void		GetTrueParticleType(int trackid, int* flukaID, int* charge, double* mass, TVector3* posV, TVector3* momV );

	int			Categorize_dataLike( );
	void		CategorizeVT();
	void		CategorizeIT();
	void		CategorizeMSD();
	void		CategorizeTW();
	void		FillTrackCategoryMap();
	TVector3	ExtrapolateToOuterTracker( Track* trackToFit, int whichPlane, int repId =-1);


	vector<int>* m_chargeVect; //Vector with charge values seen by TW -> used for track representation
	map<int, vector<AbsMeasurement*> >* m_allHitMeas; //Vector with all the Measurements in GenFit format
	TAGFdetectorMap* m_SensorIDMap;
	vector<AbsTrackRep*> m_trackRepVec;
	
	map<TString, Track*>* m_trackCategoryMap;
	map<int, Track*> m_trackTempMap;
	map<int, TVector3> m_trackSlopeMap;
	map< int, vector<int> >* m_measParticleMC_collection;
	
	TAMCntuPart* m_McNtuEve;

	Bool_t flagMC; 
	int m_debug;
	string m_systemsON;
	
	double m_AMU = 0.9310986964; // in GeV // conversion betweem mass in GeV and atomic mass unit
};

#endif















