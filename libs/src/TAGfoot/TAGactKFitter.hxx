/*!
 \file
 \version $Id: TAGactKFitter.hxx
 \brief  Header for main class of GenFit Global Reconstruction
*/
#ifndef TAGACTKFITTER_HXX
#define TAGACTKFITTER_HXX

#include <map>

#include <TGeoManager.h>
#include <TGeoMedium.h>
#include <TGeoMaterial.h>

#include <TGeoMaterialInterface.h>
#include <ConstField.h>

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
#include <SpacepointMeasurement.h>
#include <MaterialEffects.h>
#include <TMatrixDSym.h>
#include <TMatrixD.h>

#include <EventDisplay.h>

#include "RectangularFinitePlane.h"

#include <TStyle.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TGraphErrors.h>

#include <TRandom3.h>

#include <TVector3.h>
#include <vector>

#include <TMath.h>

#include "TASTparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TACAparGeo.hxx"

#include "TADIparGeo.hxx"

#include "TAVTntuHit.hxx"
#include "TAIThit.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAMCntuPart.hxx"

#include "MagicSkills.hxx"

#include <sys/types.h>
#include <sys/stat.h>
#include <limits>

#include "TAMCntuHit.hxx"
#include "TAITntuTrack.hxx"

#include "TAVTtrack.hxx"
#include "TAITtrack.hxx"


#include "TAGFuploader.hxx"
#include "TAGFselector.hxx"
#include "TAGFdetectorMap.hxx"
#include "TAGntuGlbTrack.hxx"
#include "TAGF_KalmanStudies.hxx"
#include "UpdatePDG.hxx"
#include "TAGactNtuGlbTrack.hxx"


//#include "GlobalTrackFoot.hxx"

using namespace std;
using namespace genfit;

#define build_string(expr)						\
(static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


typedef vector<genfit::AbsMeasurement*> MeasurementVector;

class TAGactKFitter : public TAGaction  {

public:


	// explicit 	TAGactKFitter(const char* name);
	// explicit 	TAGactKFitter(const char* name, TAGdataDsc* p_glbtrackrepoGF, TAGdataDsc* p_glbtrackrepo);
	explicit 	TAGactKFitter(const char* name, TAGdataDsc* p_glbtrackrepo);

	virtual	~TAGactKFitter();

	//! Action
	Bool_t Action();

	//! create histogram
	virtual	void   CreateHistogram();

	ClassDef(TAGactKFitter,0);

	int MakeFit(long evNum);
	void MakePrefit();

	void RecordTrackInfo( Track* track, string hitSampleName );

	void IncludeDetectors();
	void CreateGeometry();

	void Finalize();	// save control plot and calculate resolutions

	void PrintEfficiency();
	void PrintPurity();
	void PrintSelectionEfficiency();

	void InitEventDisplay();

	void MatrixToZero( TMatrixD *matrix );


	int FindMostFrequent( vector<vector<int>>* mcParticleID_track );
	double TrackQuality( vector<vector<int>>* mcParticleID_track );

	void GetMeasInfo( int detID, int hitID, int* iPlane, int* iClus, vector<int>* iPart );
	void GetRecoTrackInfo ( int i, Track* track,
											TVector3* KalmanPos, TVector3* KalmanMom,
											TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov );
	// void GetRecoTrackInfo ( StateOnPlane* state,
	// 										TVector3* KalmanPos, TVector3* KalmanMom,
	// 										TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov );
	void GetMeasTrackInfo( int hitID, TVector3* pos, TVector3* posErr );

	void FillGenCounter( map<string, int> mappa );

// TVector3 ExtrapolateToTarget( Track* trackToFit );

private:

	// TAGdataDsc*  fpGlobTrackRepoGenFit;
	TAGdataDsc*  fpGlobTrackRepo;

	KalmanFitter* m_fitter_extrapolation;

	KalmanFitter* m_fitter;
	AbsKalmanFitter*  m_refFitter;    		 /// KalmanFitterRefTrack()
	AbsKalmanFitter*  m_dafRefFitter;    	         //DAF with kalman ref
	AbsKalmanFitter*  m_dafSimpleFitter;    	 //DAF with simple kalman

	TAMCntuPart*  m_trueParticleRep;

	TAGFuploader* m_uploader;
	TAGntuGlbTrack* m_outTrackRepo;
	// TAGntuTrackRepository* m_outTrackRepo;
	TAGFselector* m_selector;

	TAGFdetectorMap* m_sensorIDmap;
	TAGF_KalmanStudies* m_trackAnalysis;

	// key is the sensor ID
	map< int, vector<AbsMeasurement*> > m_allHitMeasGF;
	// key is the hit ID (commbo of det+plane+hit)
	map< int, vector<int> >* m_measParticleMC_collection;


	map<int, int> m_detectorPlaneID;

	map<TString,Track*> m_mapTrack;
	vector<Track*> m_vectorConvergedTrack;

	vector<string> m_Particles;
	map<string, int> m_ParticleIndex;
	vector<string> m_Isotopes;
	map<string, int> m_IsotopesIndex;

	// init event display
	EventDisplay* display;
	bool m_IsEDOn;

	// ControlPlotsRepository* m_controlPlotter;

	bool m_isMC;


	//  delete non va fatto il delete perche APPARENTEMENTE gia fatto
	vector<TAVTntuHit*> m_VT_hitCollection;
	vector<TAIThit*> m_IT_hitCollection;
	vector<TAMSDntuHit*> m_MSD_hitCollection;
	vector<TATWpoint*> m_TW_hitCollection;

	vector<TAVTcluster*> m_VT_clusCollection;
	vector<TAITcluster*> m_IT_clusCollection;
	vector<TAMSDcluster*> m_MSD_clusCollection;


	// vector<TVector3> m_MSD_posVectorSmearedHit;
	// vector<TVector3> m_MSD_momVectorSmearedHit;
	// vector<double> m_MSD_mass;


	map <string, vector<AbsMeasurement*> > m_hitCollectionToFit;
	map <int, vector<AbsMeasurement*> > m_hitCollectionToFit_dataLike;
	vector<AbsMeasurement*> m_allHitsInMeasurementFormat;

	shared_ptr<TASTparGeo> m_ST_geo;
	shared_ptr<TABMparGeo> m_BM_geo;
	shared_ptr<TAGparGeo> m_TG_geo;
	shared_ptr<TADIparGeo> m_DI_geo;
	shared_ptr<TAVTparGeo> m_VT_geo;
	shared_ptr<TAITparGeo> m_IT_geo;
	shared_ptr<TAMSDparGeo> m_MSD_geo;
	shared_ptr<TATWparGeo> m_TW_geo;
	shared_ptr<TACAparGeo> m_CA_geo;

	TGeoVolume* m_TopVolume;          // top volume of geometry

	// TrackVector* m_fitTrackCollection;

	vector<Color_t> m_vecHistoColor;

	// vector<string> m_categoryFitted;

	string m_systemsON;
	string m_kalmanOutputDir;

	double m_AMU; // conversion betweem mass in GeV and atomic mass unit
	double m_BeamEnergy; 

	int m_debug;

	long m_evNum;

	int m_numGenParticle_noFrag;

	bool m_reverse;
	bool m_recolike1;

	TAGgeoTrafo* m_GeoTrafo;


	// TH2D* MSDresidualOfPrediction;
	// TH2D* ITresidualOfPrediction;
	// TH1F* percentageOfMCTracksVTX;
	TH1F* h_purity;
	TH1F* h_trackEfficiency;

	TH1F* h_trackMC_true_id;
	TH1F* h_trackMC_reco_id;
	TH1F* h_trackQuality;
	
	TH1F* h_length;
	TH1F* h_tof;

	TH1F* h_nMeas;
	TH1F* h_mass;
	TH1F* h_chi2;
	TH1F* h_pVal;

	TH1F* h_chargeMC;
	TH1F* h_chargeMeas;
	TH1F* h_chargeFlip;
	TH1F* h_momentum;
	
	TH1F* h_dR; 
	TH1F* h_phi; 
	TH1F* h_theta; 
	TH1F* h_eta; 
	TH1F* h_dx_dz; 
	TH1F* h_dy_dz; 

	TH1F* h_mcMom;
	TH1F* h_mcPosX;
	TH1F* h_mcPosY;
	TH1F* h_mcPosZ;
	
	map<string, map<float, TH1F*> > h_dPOverP_x_bin;
	map<string, TH1F*> h_deltaP;
	map<string, TH1F*> h_sigmaP;
	map<string, TH1F*> h_resoP_over_Pkf;
	map<string, TH1F*> h_biasP_over_Pkf;

	vector<TH1F*> h_momentum_true;
	vector<TH1F*> h_momentum_reco;
	vector<TH1F*> h_ratio_reco_true;

	int MSDforwardcounter;


	//Efficiency & Purity variables
	map<string, int> m_nTruthMCparticles;
	map<string, int> m_nSelectedTrackCandidates;
	map<string, int> m_nConvergedTracks_all;
	map<string, int> m_nConvergedTracks_good;
	map<string, int> m_nConvergedTracks_matched;
	map<string, int> m_genCount_vector;


	std::ofstream ofs;

	vector<int> m_NClusTrack;
	vector<int> m_NClusGood;

	int m_NTWTracksGoodHypo;
	int m_NTWTracks;

	void	EvaluateProjectionEfficiency(string* PartName, Track* fitTrack);
};



#endif
