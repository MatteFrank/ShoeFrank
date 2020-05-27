#ifndef KFITTER_H
#define KFITTER_H


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
#include <MaterialEffects.h>
#include <TMatrixDSym.h>
#include <TMatrixD.h>


#include <EventDisplay.h>

#include "WireMeasurement.h"
#include "PlanarMeasurement.h"
#include "SpacepointMeasurement.h"
#include "SharedPlanePtr.h"
#include "RectangularFinitePlane.h"

#include <TROOT.h>
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

#include "TAGparGeo.hxx"

#include "TADIparGeo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactNtuClusterF.hxx"

#include "TAITparGeo.hxx"
#include "TAITntuRaw.hxx"
#include "TAITntuCluster.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDntuPoint.hxx"

#include "TATWparGeo.hxx"
#include "TATWntuPoint.hxx"

#include "TAMCntuEve.hxx"

#include "TAGroot.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

#include "GlobalPar.hxx"
#include "ControlPlotsRepository.hxx"
#include "GlobalTrackRepostory.hxx"
#include "MagicSkills.hxx"
#include "UpdatePDG.hxx"

#include <sys/types.h>
#include <sys/stat.h>
#include <limits>

#include "TAGgeoTrafo.hxx"
#include "TAMCntuHit.hxx"
#include "TAVTactBaseNtuTrack.hxx"
#include "TAVTactNtuVertex.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuVertexPD.hxx"

#include "TAVTtrack.hxx"



//#include "GlobalTrackFoot.hxx"

using namespace std;
using namespace genfit;

#define build_string(expr)						\
  (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


typedef vector<genfit::AbsMeasurement*> MeasurementVector;

class KFitter {

public:


  KFitter();
  ~KFitter() {
    delete m_fitter;
  };


  struct MCTruthInfo{

    int MCTrackId;
    int MCFlukaId;
    int MCMass;
    int MCCharge;
    TVector3 MCGenPosition;
    TVector3 MCGenMomentum;
    TVector3 MCPosition;
    TVector3 MCMomentum;
  };


  // int PrepareData4Fit( string option );
  int PrepareData4Fit();
  int PrepareData4Fit_dataLike();

  void Prepare4Vertex( TAVTcluster* clus, int track_ID, int iHit );
  void Prepare4Vertex();

  void Prepare4InnerTracker();
  void Prepare4InnerTracker( TAITcluster* clus, int track_ID, int iHit );

  void Prepare4Strip();
  void Prepare4Strip(TVector3 pos, int track_ID, int iHit );

  void Prepare4TofWall();

  bool PrefitRequirements( map< string, vector<AbsMeasurement*> >::iterator element );

  int MakeFit(long evNum);

  void GetTrueMCInfo( string hitSampleName, int x,
		      TVector3* tmpPos, TVector3* tmpMom, double* tmp_mass,
		      TVector3* tmp_genPos,  TVector3* tmp_genMom, TVector3* hitPos );
  void GetKalmanTrackInfo ( string hitSampleName, int i, Track* track,
			    TVector3* KalmanPos, TVector3* KalmanMom, TVector3* KalmanPos_err, TVector3* KalmanMom_err,
			    TMatrixD* KalmanPos_cov, TMatrixD* KalmanMom_cov,
			    double* KalmanMass );
  void GetTrueParticleType( AbsMeasurement* , int* flukaID, int* partID, int* charge, double* mass );

  void SetTrueSeed( TVector3* pos, TVector3* mom );
  void MakePrefit();

  void CategoriseHitsToFit_withTrueInfo();
  void CategoriseHitsToFit_fromVTX() {};


  void RecordTrackInfo( Track* track, string hitSampleName );

  void IncludeDetectors();

  int UploadHitsVT();
  int UploadHitsIT();
  int UploadHitsMSD();
  int UploadHitsTW();

  int UploadClusVT();
  int UploadClusIT();
  int UploadClusMSD();

  void TestExtrapolation(vector<AbsMeasurement*> extrapTest, string particleHypo);


  void CreateDetectorPlanes();


  void Finalize();	// save control plot and calculate resolutions

  void PrintEfficiency();
  // void PrintPositionResidual( TVector3 pos, TVector3 expectedPos, string hitSampleName );
  // void PrintMomentumResidual( TVector3 pos, TVector3 expectedPos, TVector3 cov, string hitSampleName );
  // void PrintMomentumResidual( TVector3 pos, TVector3 expectedPos, TMatrixD cov, string hitSampleName );
  // void PrintMomentumResidual( TVector3 pos, TVector3 expectedPos, double cov, string hitSampleName );

  // void InitAllHistos( string hitSampleName );
  // void InitSingleHisto( map< string, TH1F* >* histoMap, string collectionName, string histoName, int nBin, float minBin, float maxBin );
  // void InitMultiBinHistoMap( map< string, vector<TH1F*> >* histoMap, string collectionName, string histoName, int nBin, float minBin, float maxBin );

  void InitEventDisplay();

  // void Save();
  // void SaveHisto( TCanvas* mirror, map< string, TH1F* > histoMap, string title, string saveName );
  // void SaveHisto( TCanvas* mirror, TH1F* histoMap, string title, string saveName );


  double EvalError( TVector3 mom, TVector3 err );
  double EvalError( TVector3 mom, TMatrixD cov );
  void MatrixToZero( TMatrixD *matrix );

  int GetChargeFromTW(Track* trackToCheck);
  TVector3 ExtrapolateToOuterTracker( Track* trackToFit, int whichPlane );
  bool CheckTrackFinding(Track* trackToCheck, int MCEveCharge, double MCEveMomentum, double MCEveMass, int chargeFromTofWall);
  TVector3 ExtrapolateToTofWall( Track* trackToFit );

  int GetTWTrackFixed (TATWpoint* pointToCheck);

  void RecordTrackInfoDataLike( Track* trackToRecord, int tCharge, string particlename );
  void GetKalmanTrackInfoDataLike ( int indexOfState, Track* track,
    const TVectorD* KalmanState, const TMatrixDSym* KalmanCov);

  genfit::KalmanFittedStateOnPlane GetKalmanTrackInfoDataLike_ ( int indexOfState, Track* track );




private:

  KalmanFitter* m_fitter_extrapolation;

  KalmanFitter* m_fitter;
  AbsKalmanFitter*  m_refFitter;    		 //KalmanFitterRefTrack()
  AbsKalmanFitter*  m_dafRefFitter;    	         //DAF with kalman ref
  AbsKalmanFitter*  m_dafSimpleFitter;    	 //DAF with simple kalman


  // init event display
  EventDisplay* display;
  bool m_IsEDOn;

  ControlPlotsRepository* m_controlPlotter;
  GlobalTrackRepostory* m_fitTrackCollection;

  // TRandom3* m_diceRoll;

  bool m_workWithMC;


  //  delete non va fatto il delete perche APPARENTEMENTE gia fatto
  vector<TAVTntuHit*> m_VT_hitCollection;
  vector<TAITntuHit*> m_IT_hitCollection;
  vector<TAMSDntuHit*> m_MSD_hitCollection;
  vector<TATWpoint*> m_TW_hitCollection;

  vector<TAVTcluster*> m_VT_clusCollection;
  vector<TAITcluster*> m_IT_clusCollection;
  vector<TAMSDcluster*> m_MSD_clusCollection;

  vector<TAMSDpoint*> m_MSD_pointCollection;

  // vector<TVector3> m_MSD_posVectorSmearedHit;
  // vector<TVector3> m_MSD_momVectorSmearedHit;
  // vector<double> m_MSD_mass;


  map <string, vector<AbsMeasurement*> > m_hitCollectionToFit;
  map <int, vector<AbsMeasurement*> > m_hitCollectionToFit_dataLike;
  vector<AbsMeasurement*> m_allHitsInMeasurementFormat;

  map <int, map<int, MCTruthInfo> > m_MCInfo;

  shared_ptr<TAGparGeo> m_TG_geo;
  shared_ptr<TADIparGeo> m_DI_geo;
  shared_ptr<TAVTparGeo> m_VT_geo;
  shared_ptr<TAITparGeo> m_IT_geo;
  shared_ptr<TAMSDparGeo> m_MSD_geo;
  shared_ptr<TATWparGeo> m_TW_geo;

  TGeoVolume* m_TopVolume;          // top volume of geometry

  // TrackVector* m_fitTrackCollection;
  vector<int> m_evNum_vect;

  vector<Color_t> m_vecHistoColor;

  map<string, int> m_nTotTracks;
  map<string, int> m_nConvergedTracks;
  vector<string> m_categoryFitted;
  map<string, int> m_detectorID_map;

  string m_systemsON;
  string m_kalmanOutputDir;

  double m_AMU; // conversion betweem mass in GeV and atomic mass unit

  int m_debug;

  long m_evNum;

  bool m_reverse;
  bool m_recolike1;

  TAGgeoTrafo* m_GeoTrafo;

  map<int, genfit::SharedPlanePtr> m_detectorPlanes;

  //temporary placeholder for trackfinding histos and graphs
  TGraphErrors* graphErrorX;
  TGraphErrors* graphErrorY;

  TH2D* MSDresidualOfPrediction;
  TH2D* ITresidualOfPrediction;
  TH1D* percentageOfMCTracksVTX;
  TH1D* tempPurity;
  TH1D* qoverp;
  TH1D* qoverpsel;

  vector<TH1D*> momentum_true;
  vector<TH1D*> momentum_reco;
  vector<TH1D*> ratio_reco_true;

  TFile* outfile;
  int MSDforwardcounter;

  std::vector<Track*> m_vectorTrack;

  std::ofstream ofs;


};





#endif
