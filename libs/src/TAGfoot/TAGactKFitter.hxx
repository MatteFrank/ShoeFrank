/*!
 \file TAGactKFitter.hxx
 \brief  Header for main class of GenFit Global Reconstruction
 \author M. Franchini, R. Zarrella and R. Ridolfi
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

	explicit 	TAGactKFitter(const char* name, TAGdataDsc* p_glbtrackrepo);

	virtual	~TAGactKFitter();

	Bool_t Action();

	virtual	void   CreateHistogram();


	int MakeFit(long evNum);
	void MakePrefit();

	void RecordTrackInfo( Track* track, string hitSampleName );

	void IncludeDetectors();
	void CreateGeometry();

	void Finalize();

	void PrintEfficiency();
	void PrintPurity();
	void PrintSelectionEfficiency();

	void InitEventDisplay();

	void MatrixToZero( TMatrixD *matrix );

	int FindMostFrequent( vector<vector<int>>* mcParticleID_track );
	double TrackQuality( vector<vector<int>>* mcParticleID_track );

	void GetMeasInfo( int detID, int hitID, int* iPlane, int* iClus, vector<int>* iPart );
	void GetRecoTrackInfo ( int i, Track* track, TVector3* KalmanPos, TVector3* KalmanMom,
											TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov );
	// void GetRecoTrackInfo ( StateOnPlane* state,
	// 										TVector3* KalmanPos, TVector3* KalmanMom,
	// 										TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov );
	void GetMeasTrackInfo( int hitID, TVector3* pos, TVector3* posErr );

	void FillGenCounter( map<string, int> mappa );

private:

	void	EvaluateProjectionEfficiency(string* PartName, Track* fitTrack);

	TAGdataDsc*  fpGlobTrackRepo;						///< Ntuple of fitted tracks -> OUTPUT

	KalmanFitter* m_fitter_extrapolation;				///< Kalman fitter used for forward extrapolation of tracks

	KalmanFitter* m_fitter;								///< Main Kalman fitter
	AbsKalmanFitter*  m_refFitter;						///< Kalman fitter w/ reference track()
	AbsKalmanFitter*  m_dafRefFitter;					///< DAF (Deterministic annealing filter) with kalman ref
	AbsKalmanFitter*  m_dafSimpleFitter;				///< DAF (Deterministic annealing filter) with simple kalman

	TAMCntuPart*  m_trueParticleRep;					///< Ptr to TAMCntuPart object

	TAGFuploader* m_uploader;							///< GenFit Uploader
	TAGFselector* m_selector;							///< GenFit Selector
	TAGntuGlbTrack* m_outTrackRepo;						///< CHECK WITH MATTEO HOW TO DO THIS

	TAGFdetectorMap* m_sensorIDmap;						///< GenFit detector Map for index handling
	TAGF_KalmanStudies* m_trackAnalysis;				///< GenFit custom output class

	map< int, vector<AbsMeasurement*> > m_allHitMeasGF;	///< Map of GenFit measurements; the key is the FitPlane index
	map< int, vector<int> >* m_measParticleMC_collection;	///< Map of the MC particles found in each measurement; the key is the global measurement ID (det*1E7 + sensor*1E5 + hit)

	map<TString,Track*> m_mapTrack;						///< Map of the tracks sent to the MakeFit function; the key is the track name, i.e. particle hypothesis ("H_3", "Li_7", ...) + an index containing the vertex number and tracklet number (1E3*vertex+tracklet). For "TrueParticle" selection, the number at the end of the track name is the MC particle ID
	vector<Track*> m_vectorConvergedTrack;				///< Vector of fitted+converged tracks

	vector<string> m_Particles;							///< Fixed list of possible particles
	map<string, int> m_ParticleIndex;					///< Map w/ indexing of possible particles
	vector<string> m_Isotopes;							///< Fixed list of possible isotopes
	map<string, int> m_IsotopesIndex;					///< Map w/ indexing of possible isotopes

	EventDisplay* display;								///< GenFit event display

	//  delete non va fatto il delete perche APPARENTEMENTE gia fatto
	// vector<TAVTntuHit*> m_VT_hitCollection;
	// vector<TAIThit*> m_IT_hitCollection;
	// vector<TAMSDntuHit*> m_MSD_hitCollection;
	// vector<TATWpoint*> m_TW_hitCollection;

	// vector<TAVTcluster*> m_VT_clusCollection;
	// vector<TAITcluster*> m_IT_clusCollection;
	// vector<TAMSDcluster*> m_MSD_clusCollection;

	// map <string, vector<AbsMeasurement*> > m_hitCollectionToFit;
	// map <int, vector<AbsMeasurement*> > m_hitCollectionToFit_dataLike;
	// vector<AbsMeasurement*> m_allHitsInMeasurementFormat;

	shared_ptr<TASTparGeo> m_ST_geo;					///< Pointer to ST parGeo
	shared_ptr<TABMparGeo> m_BM_geo;					///< Pointer to BM parGeo
	shared_ptr<TAGparGeo> m_TG_geo;						///< Pointer to TG parGeo
	shared_ptr<TADIparGeo> m_DI_geo;					///< Pointer to DI parGeo
	shared_ptr<TAVTparGeo> m_VT_geo;					///< Pointer to VT parGeo
	shared_ptr<TAITparGeo> m_IT_geo;					///< Pointer to IT parGeo
	shared_ptr<TAMSDparGeo> m_MSD_geo;					///< Pointer to MSD parGeo
	shared_ptr<TATWparGeo> m_TW_geo;					///< Pointer to TW parGeo
	shared_ptr<TACAparGeo> m_CA_geo;					///< Pointer to CA parGeo
	TGeoVolume* m_TopVolume;							///< Top volume of geometry

	TAGgeoTrafo* m_GeoTrafo;							///< GeoTrafo object

	vector<Color_t> m_vecHistoColor;

	string m_systemsON;									///< String w/ list of systems on in the campaign
	// string m_kalmanOutputDir;

	double m_AMU;										///< Conversion between mass in GeV and atomic mass unit
	double m_BeamEnergy;								///< Kinetic energy per nucleon of the primary beam in GeV

	int m_debug;										///< Global debug level

	long m_evNum;										///< Event number

	int m_numGenParticle_noFrag;						///< Number of generated particle in the MC event that didn't fragment between the TG and TW

	// TH2D* MSDresidualOfPrediction;
	// TH2D* ITresidualOfPrediction;
	// TH1F* percentageOfMCTracksVTX;
	TH1F* h_purity;										///< Purity -- histo
	TH1F* h_trackEfficiency;							///< Tracking efficiency -- histo
	TH1F* h_trackQuality;								///< Track quality -- histo

	TH1F* h_trackMC_true_id;							///< MC true particle Id -- histo
	TH1F* h_trackMC_reco_id;							///< Fitted particle Id -- histo
	
	TH1F* h_length;										///< Fitted track length (TG-TW) -- histo
	TH1F* h_tof;										///< Fitted track Time-Of-Flight (TG-TW) -- histo
	TH1F* h_nMeas;										///< Number of measurements per track -- histo
	TH1F* h_mass;										///< Fitted mass -- histo
	TH1F* h_chi2;										///< Chi2 -- histo
	TH1F* h_pVal;										///< P-value -- histo

	TH1F* h_chargeMC;									///< MC charge -- histo
	TH1F* h_chargeMeas;									///< Fitted charge -- histo
	TH1F* h_chargeFlip;									///< Charge flip (MC - Reco) -- histo
	TH1F* h_momentum;									///< Fitted momentum module at the TG -- histo
	
	TH1F* h_dR;											///< Fitted track dR =  at the target -- histo
	TH1F* h_phi;										///< Fitted track azimuthal angle at the TG -- histo
	TH1F* h_theta;										///< Fitted track polar angle at the TG -- histo
	TH1F* h_eta;										///< Fitted track eta =  at the TG -- histo
	TH1F* h_dx_dz;										///< Fitted track slope at the TG in the X direction -- histo
	TH1F* h_dy_dz;										///< Fitted track slope at the TG in the Y direction -- histo

	TH1F* h_mcMom;										///< MC momentum module at the TG -- histo
	TH1F* h_mcPosX;										///< MC X position at the TG -- histo
	TH1F* h_mcPosY;										///< MC Y position at the TG -- histo
	TH1F* h_mcPosZ;										///< MC Z position at the TG -- histo
	
	map<string, map<float, TH1F*> > h_dPOverP_x_bin;	///< Map of the histograms for dP/P; the key of the external map is the name of the particle ("H", "He", "Li", ...), while the key for the internal map is the central P_MC value of the histogram
	map<string, TH1F*> h_deltaP;						/// Map of total dP histograms; the key is the particle name ("H", "He", "Li", ...)
	map<string, TH1F*> h_sigmaP;						///< Map of histograms for total sigma of dP distributions; the key is the particle name ("H", "He", "Li", ...)
	map<string, TH1F*> h_resoP_over_Pkf;				///< Map of histograms for dP/P resolution (sigma) for each particle; the key is the particle name ("H", "He", "Li", ...)
	map<string, TH1F*> h_biasP_over_Pkf;				///< Map of histograms for dP/P bias for each particle; the key is the particle name ("H", "He", "Li", ...)

	vector<TH1F*> h_momentum_true;						///< Vector of histograms for MC momentum module at the TG
	vector<TH1F*> h_momentum_reco;						///< Vector of histograms for Fitted momentum moduel at the TG
	vector<TH1F*> h_ratio_reco_true;					///< Vector of histograms for Fitted/MC momenutm module at the TG

	//Efficiency & Purity variables
	map<string, int> m_nSelectedTrackCandidates;		///< Map of total number of selected track candidates; the key is the particle name ("H", "He", "Li", ...)
	map<string, int> m_nConvergedTracks_all;			///< Map of total number of tracks w/ converged fit; the key is the particle name ("H", "He", "Li", ...)
	map<string, int> m_nConvergedTracks_matched;		///< Map of total number of tracks w/ converged fit and good charge hypothesis; the key is the particle name ("H", "He", "Li", ...)
	map<string, int> m_genCount_vector;					///< Map of total number of particles generated in the geometrical acceptance of the FOOT geometry; the key is the particle name ("H", "He", "Li", ...)

	vector<int> m_NClusTrack;							///< Total number of clusters used in track candidates for each FitPlane; The index of the vector is the GenFit FitPlane index
	vector<int> m_NClusGood;							///< Total number of clusters used in track candidates w/ good charge hypothesis for each FitPlane; The index of the vector is the GenFit FitPlane index

	int m_NTWTracks;									///< Total number of track candidates that reach the TW
	int m_NTWTracksGoodHypo;							///< Total number of track candidates reaching the TW that have the correct charge hypo

	ClassDef(TAGactKFitter,0);
};



#endif
