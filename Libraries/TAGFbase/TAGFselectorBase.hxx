/*!
 \file TAGFselectorBase.hxx
 \brief  Header of GenFit track finding/selection base class
 \author M. Franchini and R. Zarrella
*/

#ifndef TAGFselectorBase_H
#define TAGFselectorBase_H

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
#include "TH2.h"

#include "TAGroot.hxx"
#include "TAGobject.hxx"
#include "TAGrecoManager.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TAGnameManager.hxx"

#include "TAGparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"

#include "TAVTntuVertex.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TATWntuPoint.hxx"
#include "TAMCntuPart.hxx"

#include <TrackPoint.h>
#include "RKTrackRep.h"
#include "KalmanFittedStateOnPlane.h"
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
#include "PlanarMeasurement.h"
#include "SharedPlanePtr.h"

#include "UpdatePDG.hxx"
#include "TAGFdetectorMap.hxx"
#include "TAGFtrackUtilities.hxx"

using namespace std;
using namespace genfit;

class TAGFselectorBase : public TAGobject
{

public:
	TAGFselectorBase();
	virtual ~TAGFselectorBase();

	void SetVariables(TAGFtrackUtilities* trackUtils, map<int, vector<AbsMeasurement *>> *allHitMeas, TString GFsystemsOn, vector<int> *chVect, map<TString, Track *> *trackCategoryMap, map<int, vector<int>> *measParticleMC_collection, bool isMC, uint *singleVertexCounter, uint *noVTtrackletEvents, uint* noTWpointEvents);

	void SetExtrapolationHistogram(map< pair<string,pair<int,int>>, TH1F*>& extrapDist) {h_extrapDist = extrapDist;}

	int FindTrackCandidates();
	virtual void Categorize() { return; }

	int					GetEventType();
	TString				GetRecoTrackName(Track* tr);
	map<string, int>	CountParticleGeneratedAndVisible();
	void				FillPlaneOccupancy(TH2I** h_PlaneOccupancy);
	string				GetParticleNameFromCharge(int ch);

protected:
	void		CheckPlaneOccupancy();
	int			FillTrackRepVector();
	void		CreateDummyTrack();

	void		GetTrueParticleType(int trackid, int* flukaID, int* charge, double* mass, TVector3* posV, TVector3* momV);

	void		FillTrackCategoryMap();
	void		PrintCurrentTracksMC();
	string		ListMCparticlesOfHit(int HitId);

	int m_eventType;
	vector<int>* m_chargeVect;								///< Vector with charge values seen by TW -> used for track representation declaration
	map<int, vector<AbsMeasurement*> >* m_allHitMeas;		///< Map with all the Measurements in GenFit format
	TAGFdetectorMap* m_SensorIDMap;							///< TAGFdetectorMap for index handling
	vector<AbsTrackRep*> m_trackRepVec;						///< Track representation vector

	map<TString, Track*>* m_trackCategoryMap;				///< Final map of selected tracks to process in TAGactKFitter (name = track representation + vertex-tracklet index)
	map<int, Track*> m_trackTempMap;						///< Temporary map where to store tracks during selection
	map<int, TVector3> m_trackSlopeMap;						///< Map of track slopes @ VT
	map< int, vector<int> >* m_measParticleMC_collection;	///< Map of MC particles associated w/ global measurement index
	map< pair<string,pair<int,int>>, TH1F*> h_extrapDist;	///< Map of histograms for global track extrapolation distance for all sensors; the key is the detector name ("VT", "MSD", "IT", ...) paired with sensor index and 1=Y or 0=X view

	TAGFtrackUtilities* fTrackUtilities;

	map<string, vector<int>> m_PlaneOccupancy;
	vector<string> m_detectors;

	TAGgeoTrafo* m_GeoTrafo;								///< GeoTrafo object for reference frame transformations
	TAVTparGeo* m_VT_geo;									///< Geometry transformations for VTX detector
	TAITparGeo* m_IT_geo;									///< Geometry transformations for IT detector
	TAMSDparGeo* m_MSD_geo;									///< Geometry transformations for MSD detector
	TATWparGeo* m_TW_geo;									///< Geometry transformations for TW detector

	TAMCntuPart* m_McNtuEve;								///< MC eve for efficiency/quality checks

	Bool_t m_IsMC; 											///< flag for MC variables
	TString m_systemsON;									///< String w/ systems on in the campaign

	float m_VTtolerance;									///< VT selector extrapolation tolerance 
	float m_ITtolerance;									///< IT selector extrapolation tolerance 
	float m_MSDtolerance;									///< MSD selector extrapolation tolerance 
	float m_TWtolerance;									///< TW selector extrapolation tolerance 

	double m_BeamEnergy;									///< Beam energy in GeV/u
	double m_AMU = 0.9310986964;							///< Conversion betweem mass in GeV and atomic mass unit
	uint *m_singleVertexCounter;							///< Counter for single vertex events
	uint *m_noVTtrackletEvents;								///< Counter for events w/ no valid tracklet
	uint *m_noTWpointEvents;								///< Counter for events w/ no valid TW point

	ClassDef(TAGFselectorBase,0);
};

#endif
