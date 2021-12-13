/*!
 \file TAGFselector.hxx
 \brief  Header of GenFit track finding/selection class
 \author M. Franchini and R. Zarrella
*/

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

	int					Categorize();

	TString				GetRecoTrackName(Track* tr);
	int					GetChargeFromTW(Track* trackToCheck);
	map<string, int>	CountParticleGenaratedAndVisible();

private:

	int			FillTrackRepVector();
	bool		PrefitRequirements( map< string, vector<AbsMeasurement*> >::iterator element ); 

	int			Categorize_TruthMC( );
	void		GetTrueParticleType(int trackid, int* flukaID, int* charge, double* mass, TVector3* posV, TVector3* momV );

	int			Categorize_dataLike( );
	int			Categorize_Linear( );
	void		CategorizeVT();
	void		CategorizeIT();
	void		CategorizeMSD();
	void		CategorizeTW();
	void		CategorizeMSD_Linear();
	void		CategorizeTW_Linear();
	void		FillTrackCategoryMap();
	TVector3	ExtrapolateToOuterTracker( Track* trackToFit, int whichPlane, int repId =-1);


	vector<int>* m_chargeVect;								///< Vector with charge values seen by TW -> used for track representation declaration
	map<int, vector<AbsMeasurement*> >* m_allHitMeas;		///< Map with all the Measurements in GenFit format
	TAGFdetectorMap* m_SensorIDMap;							///< TAGFdetectorMap for index handling
	vector<AbsTrackRep*> m_trackRepVec;						///< Track representation vector
	
	map<TString, Track*>* m_trackCategoryMap;				///< Final map of selected tracks to process in TAGactKFitter (name = track representation + vertex-tracklet index)
	map<int, Track*> m_trackTempMap;						///< Temporary map where to store tracks during selection
	map<int, TVector3> m_trackSlopeMap;						///< Map of track slopes @ VT
	map< int, vector<int> >* m_measParticleMC_collection;	///< Map of MC particles associated w/ global measurement index
	
	TAGgeoTrafo* m_GeoTrafo;								///< GeoTrafo object for reference frame transformations

	TAMCntuPart* m_McNtuEve;								///< MC eve for efficiency/quality checks

	Bool_t flagMC; 											///< flag for MC variables
	int m_debug;											///< Global debug value
	string m_systemsON;										///< String w/ systems on in the campaign
	
	double m_BeamEnergy;									///< Beam energy in GeV/u
	double m_AMU = 0.9310986964;							///< Conversion betweem mass in GeV and atomic mass unit
};

#endif















