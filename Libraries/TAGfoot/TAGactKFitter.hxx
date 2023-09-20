/*!
 \file TAGactKFitter.hxx
 \brief  Header for main class of GenFit Global Reconstruction
 \author M. Franchini, R. Zarrella and R. Ridolfi
*/
#ifndef TAGACTKFITTER_HXX
#define TAGACTKFITTER_HXX

#include <map>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits>

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
#include <TH3F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TRandom3.h>
#include <TVector3.h>
#include <TMath.h>

#include "TAGparGeo.hxx"

#include "TASTparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TABMntuTrack.hxx"
#include "TACAparGeo.hxx"
#include "TADIparGeo.hxx"

#include "TASTntuRaw.hxx"
#include "TAVTntuHit.hxx"
#include "TAIThit.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAMCntuPart.hxx"
#include "TAMCntuRegion.hxx"
#include "TACAntuCluster.hxx"

#include "TAMCntuHit.hxx"
#include "TAITntuTrack.hxx"

#include "TAVTtrack.hxx"
#include "TAITtrack.hxx"

#include "TAGactTreeWriter.hxx"
#include "TAGFuploader.hxx"
#include "TAGFselectorTrue.hxx"
#include "TAGFselectorStandard.hxx"
#include "TAGFselectorLinear.hxx"
#include "TAGFselectorBack.hxx"
#include "TAGFdetectorMap.hxx"
#include "TAGFgeometryConstructor.hxx"
#include "TAGntuGlbTrack.hxx"
#include "TAGF_KalmanStudies.hxx"
#include "UpdatePDG.hxx"

using namespace std;
using namespace genfit;

#define build_string(expr)						\
(static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


class TAGactKFitter : public TAGaction  {

public:

	explicit 	TAGactKFitter(const char* name, TAGdataDsc* p_glbtrackrepo);

	virtual	~TAGactKFitter();

	Bool_t Action();

	virtual	void   CreateHistogram();
	virtual void   SetHistogramDir(TDirectory* dir);

	void Finalize();

	void	SetMcSample();

private:

	AbsKalmanFitter*	InitializeFitter(int nIter, double dPVal);
	TAGFselectorBase*	InitializeSelector();
	int		MakeFit(long evNum, TAGFselectorBase* selector);

	void	RecordTrackInfo( Track* track, string hitSampleName );

	void	PrintEfficiency();
	void	PrintPurity();
	void	PrintSelectionEfficiency();

	void	InitEventDisplay();

	void	MatrixToZero( TMatrixD *matrix );

	int		FindMostFrequent( vector<vector<int>>* mcParticleID_track );
	double	TrackQuality( vector<vector<int>>* mcParticleID_track );

	void	GetMeasInfo( int detID, int hitID, int* iPlane, int* iClus, vector<int>* iPart, TVector3* pos, TVector3* posErr);
	void	GetRecoTrackInfo ( int i, Track* track, TVector3* KalmanPos, TVector3* KalmanMom, TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov );

	void	CalculateTrueMomentumAtTgt();
	void 	MatchCALOclusters();

	void	FillGenCounter( map<string, int> mappa );
	void	EvaluateProjectionEfficiency(Track* fitTrack);
	void	CheckChargeHypothesis(string* PartName, Track* fitTrack, TAGFselectorBase* selector);
	void	AddResidualAndPullHistograms();
	void	ClearData();
	void	ClearHistos();

	TAGdataDsc*  fpGlobTrackRepo;						///< Ntuple of fitted tracks -> OUTPUT

	KalmanFitter* m_fitter_extrapolation;				///< Kalman fitter used for forward extrapolation of tracks
	AbsKalmanFitter* m_fitter;							///< Kalman fitter

	TAMCntuPart*  m_trueParticleRep;					///< Ptr to TAMCntuPart object
	TAGntuGlbTrack* m_outTrackRepo;						///< CHECK WITH MATTEO HOW TO DO THIS

	TAGFselectorBase* m_dummySelector;
	TAGFdetectorMap* m_SensorIDMap;						///< GenFit detector Map for index handling
	TAGF_KalmanStudies* m_trackAnalysis;				///< GenFit custom output class

	map< int, vector<AbsMeasurement*> > m_allHitMeasGF;	///< Map of GenFit measurements; the key is the FitPlane index
	map< int, vector<int> >* m_measParticleMC_collection;	///< Map of the MC particles found in each measurement; the key is the global measurement ID (det*1E7 + sensor*1E5 + hit)

	map<TString,Track*> m_mapTrack;						///< Map of the tracks sent to the MakeFit function; the key is the track name, i.e. particle hypothesis ("H_3", "Li_7", ...) + an index containing the vertex number and tracklet number (1E3*vertex+tracklet). For "TrueParticle" selection, the number at the end of the track name is the MC particle ID
	vector<Track*> m_vectorConvergedTrack;				///< Vector of fitted+converged tracks

	vector<string> m_Particles;							///< Fixed list of possible particles
	map<string, int> m_ParticleIndex;					///< Map w/ indexing of possible particles
	vector<string> m_Isotopes;							///< Fixed list of possible isotopes
	map<string, int> m_IsotopesIndex;					///< Map w/ indexing of possible isotopes
	map<int, TVector3> m_trueMomentumAtTgt;

	EventDisplay* display;								///< GenFit event display
	TAGFgeometryConstructor* m_GFgeometry;				///< GenFit geometry creator object
	TAGgeoTrafo* m_GeoTrafo;							///< GeoTrafo object

	TString m_systemsON;								///< String w/ list of systems on in the campaign

	double m_AMU;										///< Conversion between mass in GeV and atomic mass unit
	double m_BeamEnergy;								///< Kinetic energy per nucleon of the primary beam in GeV

	long m_evNum;										///< Event number
	int m_numGenParticle_noFrag;						///< Number of generated particle in the MC event that didn't fragment between the TG and TW

	// TH2D* MSDresidualOfPrediction;
	// TH2D* ITresidualOfPrediction;
	// TH1F* percentageOfMCTracksVTX;
	TH2I* h_nConvTracksVsStartTracks;					///< Number of track candidates vs fitted tracks -- histo
	TH2I* h_nConvTracksVsStartTracksNmeasureCut;		///< Number of track candidates vs fitted tracks for events where at least one track is lost because of the cut on minimum track points -- histo
	TH2I* h_nConvTracksVsStartTracksConvergeCut;		///< Number of track candidates vs fitted tracks for events where at least one track is lost because of the cut on fit convergence -- histo
	TH2I* h_nConvTracksVsStartTracksYesTW;				///< Number of track candidates vs fitted tracks with a TW point -- histo
	TH2I* h_nConvTracksVsStartTracksNoTW;				///< Number of track candidates vs fitted tracks without a TW point -- histo
	TH1F* h_purity;										///< Purity -- histo
	TH1F* h_trackEfficiency;							///< Tracking efficiency -- histo
	TH1F* h_trackQuality;								///< Track quality -- histo
	TH2F* h_trackQualityVsChi2;							///< Track quality vs chi2 -- histo
	TH2F* h_trackQualityVsNhits;						///< Track quality vs N hits in the track -- histo
	std::vector<TH1F*> h_trackQuality_Z;				///< Track quality for each Z -- histo

	TH1F* h_trackMC_true_id;							///< MC true particle Id -- histo
	TH1F* h_trackMC_reco_id;							///< Fitted particle Id -- histo
	TH1F* h_nTracksPerEv;								///< Numbrer of Fitted track per event -- histo

	TH1F* h_length;										///< Fitted track length (TG-TW) -- histo
	TH1F* h_tof;										///< Fitted track Time-Of-Flight (TG-TW) -- histo
	TH1F* h_nMeas;										///< Number of measurements per track -- histo
	TH1F* h_mass;										///< Fitted mass -- histo
	TH1F* h_chi2;										///< Chi2 -- histo
	std::vector<TH1F*> h_chi2_Z;						///< Chi2 as a function of Z -- histo
	TH1F* h_pVal;										///< P-value -- histo

	TH1F* h_chargeMC;									///< MC charge -- histo
	TH1F* h_chargeMeas;									///< Fitted charge -- histo
	TH1F* h_chargeFlip;									///< Charge flip (MC - Reco) -- histo
	TH1F* h_momentum;									///< Fitted momentum module at the TG -- histo

	TH1F* h_dR;											///< Fitted track dR =  at the target -- histo
	TH1F* h_phi;										///< Fitted track azimuthal angle at the TG -- histo
	TH1F* h_theta;										///< Fitted track polar angle at the TG -- histo
	TH1F* h_theta_BM;									///< Fitted track polar angle at the TG wrt to BM track -- histo
	std::vector <TH1F*> h_theta_BM_Z;					///< Fitted track polar angle at the TG wrt to BM track for all Z separately -- histo
	TH2F* h_thetaGlbVsThetaTW;							///< Fitted track polar angle at the TG wrt to BM track -- histo
	TH1F* h_phi_BM;										///< Fitted track azimuthal angle at the TG wrt to BM track -- histo
	TH2F* h_trackDirBM;									///< Fitted tracks X-Y coordinates of emission direction wrt BM track -- histo
	TH1F* h_theta_BMnoTw;								///< Fitted track polar angle at the TG wrt to BM track when global track does not have a TW point -- histo
	TH1F* h_phi_BMnoTw;									///< Fitted track azimuthal angle at the TG wrt to BM track when global track does not have a TW point -- histo
	TH1F* h_theta_BMyesTw;								///< Fitted track polar angle at the TG wrt to BM track when global track has a TW point -- histo
	TH1F* h_phi_BMyesTw;								///< Fitted track azimuthal angle at the TG wrt to BM track when global track has a TW point -- histo
	TH1F* h_eta;										///< Fitted track eta =  at the TG -- histo
	TH1F* h_dx_dz;										///< Fitted track slope at the TG in the X direction -- histo
	TH1F* h_dy_dz;										///< Fitted track slope at the TG in the Y direction -- histo

	TH1F* h_mcMom;										///< MC momentum module at the TG -- histo
	TH1F* h_mcPosX;										///< MC X position at the TG -- histo
	TH1F* h_mcPosY;										///< MC Y position at the TG -- histo
	TH1F* h_mcPosZ;										///< MC Z position at the TG -- histo
	TH2I* h_PlaneOccupancy[6];							///< Plane occupancy (number of clusters vs FitPlane) for different GF event types; used for debug and background rejection -- histo

	TH1I* h_GFeventType;								///< Event categorization seen by the GF selector

	map<string, map<float, TH1F*> > h_dPOverP_x_bin;	///< Map of the histograms for dP/P; the key of the external map is the name of the particle ("H", "He", "Li", ...), while the key for the internal map is the central P_MC value of the histogram
	map<string, TH2F* > h_dPOverP_vs_P;		///< Map of the histograms for dP/P; the key of the external map is the name of the particle ("H", "He", "Li", ...), while the key for the internal map is the central P_MC value of the histogram
	map<string, TH1F*> h_deltaP;						/// Map of total dP histograms; the key is the particle name ("H", "He", "Li", ...)
	map<string, TH1F*> h_sigmaP;						///< Map of histograms for total sigma of dP distributions; the key is the particle name ("H", "He", "Li", ...)
	map<string, TH1F*> h_resoP_over_Pkf;				///< Map of histograms for dP/P resolution (sigma) for each particle; the key is the particle name ("H", "He", "Li", ...)
	map<string, TH1F*> h_biasP_over_Pkf;				///< Map of histograms for dP/P bias for each particle; the key is the particle name ("H", "He", "Li", ...)
	map< pair<string,pair<int,int>>, TH1F*> h_residual;	///< Map of histograms for global track residuals; the key is the detector name ("VT", "MSD", "IT", ...) paired with sensor index and 1=Y or 0=X view
	map< pair<string,pair<int,int>>, TH2F*> h_residualVsPos;	///< Map of histograms for global track residulas vs measured position; the key is the detector name ("VT", "MSD", "IT", ...) paired with sensor index and 1=Y or 0=X view
	map< pair<string,pair<int,int>>, TH1F*> h_pull;				///< Map of histograms for global track pulls; the key is the detector name ("VT", "MSD", "IT", ...) paired with sensor index and 1=Y or 0=X view
	map< pair<string,pair<int,int>>, TH2F*> h_pullVsClusSize;	///< Map of histograms for global track pulls vs cluster size; the key is the detector name ("VT", "MSD", "IT", ...) paired with sensor index and 1=Y or 0=X view
	map< pair<string,pair<int,int>>, TH1F*> h_extrapDist;		///< Map of histograms for global track extrapolation distance for all sensors; the key is the detector name ("VT", "MSD", "IT", ...) paired with sensor index and 1=Y or 0=X view

	vector<TH1F*> h_momentum_true;						///< Vector of histograms for MC momentum module at the TG
	vector<TH1F*> h_momentum_reco;						///< Vector of histograms for Fitted momentum moduel at the TG
	vector<TH1F*> h_ratio_reco_true;					///< Vector of histograms for Fitted/MC momenutm module at the TG

	vector<TH1F*> h_RecoMass;

	TH2D* h_TWprojVsThetaTot;							///< X-Y projection of fitted global tracks @ TW -- histo
	TH2D* h_TWprojVsThetaTotYesTW;						///< X-Y projection of fitted global tracks with a TW point @ TW -- histo
	TH2D* h_TWprojVsThetaTotNoTW;						///< X-Y projection of fitted global tracks without a TW point @ TW -- histo
	TH1D* h_TWprojZTot;									///< Z of particles with global track projection above and below the "beam center" on the TW -- histo
	vector<TH2D*> h_TWprojVsTheta;						///< X-Y projection of fitted global tracks @ TW as a function of global track theta -- histo
	vector<TH2D*> h_TWprojVsThetaNoTW;					///< X-Y projection of fitted global tracks with a TW point @ TW as a function of global track theta -- histo
	vector<TH2D*> h_TWprojVsThetaYesTW;					///< X-Y projection of fitted global tracks without a TW point @ TW as a function of global track theta -- histo
	vector<TH1D*> h_TWprojZvsTheta;						///< Z of particles with global track projection above and below the "beam center" on the TW as a function of global track theta -- histo

	TH2D* h_TGprojVsThetaTot;							///< X-Y projection of fitted global tracks @ TG -- histo
	TH2D* h_TGprojVsThetaTotYesTW;						///< X-Y projection of fitted global tracks with a TW point @ TG -- histo
	TH2D* h_TGprojVsThetaTotNoTW;						///< X-Y projection of fitted global tracks without a TW point @ TG -- histo
	vector<TH2D*> h_TGprojVsTheta;						///< X-Y projection of fitted global tracks @ TG as a function of global track theta -- histo
	vector<TH2D*> h_TGprojVsThetaNoTW;					///< X-Y projection of fitted global tracks with a TW point @ TG as a function of global track theta -- histo
	vector<TH2D*> h_TGprojVsThetaYesTW;					///< X-Y projection of fitted global tracks without a TW point @ TG as a function of global track theta -- histo

	//Efficiency & Purity variables
	map<string, int> m_nSelectedTrackCandidates;		///< Map of total number of selected track candidates; the key is the particle name ("H", "He", "Li", ...)
	map<string, int> m_nConvergedTracks_all;			///< Map of total number of tracks w/ converged fit; the key is the particle name ("H", "He", "Li", ...)
	map<string, int> m_nConvergedTracks_matched;		///< Map of total number of tracks w/ converged fit and good charge hypothesis; the key is the particle name ("H", "He", "Li", ...)
	map<string, int> m_genCount_vector;					///< Map of total number of particles generated in the geometrical acceptance of the FOOT geometry; the key is the particle name ("H", "He", "Li", ...)

	vector<int> m_NClusTrack;							///< Total number of clusters used in track candidates for each FitPlane; The index of the vector is the GenFit FitPlane index
	vector<int> m_NClusGood;							///< Total number of clusters used in track candidates w/ good charge hypothesis for each FitPlane; The index of the vector is the GenFit FitPlane index

	int m_NTWTracks;									///< Total number of track candidates that reach the TW
	int m_NTWTracksGoodHypo;							///< Total number of track candidates reaching the TW that have the correct charge hypo
	bool m_IsMC;

	uint m_singleVertexCounter;							///< Counter for events w/ only one vertex
	uint m_noVTtrackletEvents;							///< Counter for events w/ no valid VT tracklets
	uint m_SCpileUpEvts;								///< Counter for events flagged as pile-up in the SC
	uint m_noTWpointEvents;								///< Counter for events w/ no valid TW point
	uint m_eventDisplayCounter;							///< Aux counter for event display event counter

	float m_CALOextrapTolerance;						///< Maximum tolerance for CALO cluster matching in global tracks extrapolation

	ClassDef(TAGactKFitter,0);
};



#endif
