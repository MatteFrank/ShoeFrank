#ifndef GlobalRecoAnaGSI_hxx
#define GlobalRecoAnaGSI_hxx

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>
#include <TString.h>
#include <TVector3.h>

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>

#include "TAGactTreeReader.hxx"
#include "TAGroot.hxx"
#include <TStopwatch.h>
#include "TAGdataDsc.hxx"

#include "BaseReco.hxx"
#include "RecoRaw.hxx"
#include "TAGrunInfo.hxx"
#include "TAGgeoTrafo.hxx"
#include "GlobalRecoMassAna.hxx"
#include "TAGnameManager.hxx"
#include "TAWDntuTrigger.hxx"

using namespace std;

class GlobalRecoAnaGSI : public RecoRaw {

  Int_t fRegTG;
  Int_t fRegLastVTplane;
  Int_t fRegAirAfterVT;
  Int_t fRegAirBeforeTW;
  Int_t fRegAirAfterTW;
  Int_t fRegFirstTWbar;
  Int_t fRegFirstTWbarLay0;
  Int_t fRegLastTWbar;
  Int_t fRegFirstCAcrys;
  Int_t fRegLastCAcrys;

  Float_t fVtVtxTolerance = 0.05;

  public:
  GlobalRecoAnaGSI(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC, Int_t nTotEv);
  ~GlobalRecoAnaGSI();

  //BaseReco overwritten methds
  void BeforeEventLoop();
  void LoopEvent();
  void AfterEventLoop();

  //detector analysis
  void VTanal(TAGpoint *tmp_poi,  int &idxcl);
  void ITanal(TAGpoint *tmp_poi,  int &idxcl);
  void MSDanal(TAGpoint *tmp_poi, int &idxcl);
  void TWanal(TAGpoint *tmp_poi,  int &idxcl);
  void CALOanal(int clusidx);
  void FullCALOanal();

  //other analysis
  void Booking();     //Define the plots
  
  Int_t ApplyEvtCuts();  //apply the cuts, return true if the track need to be skipped
 
  void SetupTree();


  //fill plots

  void FillMCGlbTrkYields();    //fill MC quantitites
  void FillDataGlbTrkYields();  //fill reconstructed quantities
  void FillTrkPlots(); //fill plots
  void FillYieldReco(string folderName, Int_t Z,Double_t Th);  // fill histos with reconstructed values
  void FillYieldMC(string folderName, Int_t Z_true,Int_t Z_meas, Double_t Th_true, Double_t Th_meas, bool migMatr); // fill histos with MC values
  void MigMatrixPlots(string folderName, Int_t Z_true, Int_t Z_meas, Double_t Th_true, Double_t Th_meas, Double_t Beta_true, Double_t Beta_meas, bool migMatr); // fill histos with MC values
  void BookYield(string path, bool enableMigMatr= false);
  void BookMigMatrix(string path, bool enableMigMatr= false);
  void BookChargeStudies(string path);
  void BookQualityPlots(string path);
  void MyRecoBooking(string path_name);
  void MyReco(string path_name);
  Bool_t CheckRadiativeDecayChain(TAMCpart* part, Int_t* part_ID);

  vector<bool> CheckTwPointInMoreTracks();     // check if more than one glb track has the same tw point
  vector<bool> CheckTwParticleOrigin();
  void RecoGlbTrkLoopSetVariables();           // Set Reco variables, to be done for each glbal track
  void MCGlbTrkLoopSetVariables();             // Set MC variables, to be done for each glbal track
  void MCParticleStudies();                    // Loop on MC particles
  void FillMCPartYields();
  bool isGoodReco(Int_t Id_part);
  void ChargeStudies(string path, Int_t charge, TAGtrack *fGlbTrack);
  void QualityPlots(string path, TAGtrack *fGlbTrack);

  // useful formulas
  TVector3 ProjectToZ(TVector3 Slope, TVector3 Pos0, Double_t FinalZ)
  {
    return TVector3(Slope.X() / Slope.Z() * (FinalZ - Pos0.Z()) + Pos0.X(), Slope.Y() / Slope.Z() * (FinalZ - Pos0.Z()) + Pos0.Y(), FinalZ);
    }

    // useful analysis variables
    Double_t atomassu = 0.931494; // atomic mass unit in GeV/c^2
    Int_t fPrimaryCharge;            // charge of primary particle
    Double_t primary_tof;         // TOF of the primary particle from ST to target (ns), evaluated from Beam initial energy value
    Double_t Ntg;                 // Number of particles per unit surface within the target = density*Na*thickness/A

    // counters (maybe a struct could be useful)
    Int_t nchargeok_vt;
    Int_t nchargeok_vt_clu;
    Int_t nchargeok_it;
    Int_t nchargeok_it_clu;
    Int_t nchargeok_msd;
    Int_t nchargeok_msd_clu;
    Int_t nchargeok_tw;

    Int_t nchargeall_vt;
    Int_t nchargeall_vt_clu;
    Int_t nchargeall_it;
    Int_t nchargeall_it_clu;
    Int_t nchargeall_msd;
    Int_t nchargeall_msd_clu;
    Int_t nchargeall_tw;

    Int_t npure_vt;
    Int_t npure_it;
    Int_t npure_msd;
    Int_t npure;
    Int_t nclean;
    Int_t ntracks;
    Int_t nTotEv; // total number of events (-nev flag)
    Int_t currEvent;
    Int_t recoEvents; // events in whick global tracks are created
    Int_t trueEvents; // events in whick events are considered from a MC level
    bool istrueEvent; // flag to sign if an event is good from a MC lever
    float residual; // worst residual in a track
    vector<pair<Int_t, Int_t>> pure_track_xcha; // vector index = particle charge; first=npure; second=ntracks

    // reconstructed quantities:
    Double_t Th_meas;
    Double_t Th_reco;
    Double_t Th_recoBM;
    Double_t Phi_reco;
    Int_t Z_meas;         // charge reco from TW
    Double_t P_meas;      // GeV
    Double_t Tof_meas;    // Tof from TW minus primary_tof
    Double_t M_meas;      // measured mass (u)
    Double_t Ek_meas_tot; // Ek total (GeV)
    Double_t Ek_meas;     // Energy per mass (GeV/u)
    Double_t Beta_meas;

    // MC quantities: true=production, cross=target, cross_calo=at calo enterance
    Int_t Z_true;
    Int_t Z_true_TW;
    Int_t MC_id;
    Double_t Ek_true;
    Double_t Ek_tot;
    Double_t M_true;
    Double_t Tof_true;
    Double_t Tof_startmc;
    Double_t Beta_true;
    TVector3 P_true;
    Double_t Th_true;
    Double_t Th_BM;
    Double_t Th_cross;      // target exit angle
    TVector3 P_cross;       // target exit momentum
    TVector3 P_beforeTG;    // target entering momentum
    Double_t Ek_cross_calo; // crossing out from TW

    Double_t initTWPosition;
    Int_t TrkIdMC;                   // MC id of the glb track
    Int_t TrkIdMC_TW;                // MC id of the twpoint
    vector<Int_t> vecTwTrkId;        // vector of all MC id of the twpoint
    vector<vector<Int_t>> vecVtZMC;  // vector of all MC charge of the 4 vt hits
    vector<vector<Int_t>> vecMsdZMC; // vector of all MC charge of the 6 msd clus
    vector<bool> hasSameTwPoint;     //vector of bool of tracks with the same tw point in the same event
    vector<bool> isParticleBorninTG;

    std::map<Int_t, Int_t> m_twId;

    // setting variables maybe we should use a config file?
    Double_t purity_cut; // minumum purity value for a track to be defined as pure
    Double_t clean_cut;  // is a 100% pure track

    bool DiffApp_trkIdx; // Total number of cases in which SetMCtruth!=SetMCtruthBIS

    // useful variables
    TFile *file_out;
    TAGactTreeReader *myReader;
    GlobalRecoMassAna *mass_ana;
    TAGtrack *fGlbTrack;
    TAGntuGlbTrack *myGlb;
    TAVTntuVertex *myVtNtuVtx;
    TATWntuPoint *myTWNtuPt;
    TAMSDntuHit *myMSDNtuHit;
    TACAntuCluster *pCaNtuClu;
    TABMntuTrack *myBMNtuTrk;
    TAMCntuEvent *myMcNtuEvent;
    TAMCntuPart *myMcNtuPart;
    TAWDntuTrigger *wdTrig;

    vector<vector<Int_t>> fGlbTrkVec;            // store the global track detector point index, 0=VTX, 1=IT, 2=MSD, 3=TW, 4=CALO, for each global track
    vector<vector<vector<Int_t>>> fEvtGlbTrkVec; // store collection of fGlbTrkVec for each event
    map<int, map<int, int>> m_nClone;            ///< Map of total number of clone id for the same charge in a single event; the key is [particle charge][specific ID]
    map<int, int> n_clones;                      ///< Map of total number of clone id for the same charge ; the key is [particle charge]

    // nuisance variables
    TH1D *h;
    TH2D *h2;
    TH3D *h3;

    // binning
    Int_t th_nbin;
    Double_t **theta_binning;
    Int_t ek_nbin;
    Double_t **ek_binning;
    Int_t mass_nbin;
    Double_t **mass_binning;

    // Unfolding
    Int_t theta_bin_meas, Ek_bin_meas, theta_bin_true, Ek_bin_true;

    // for beam direction studies in vertex
    TVector3 vertex_direction_frag;
    TVector3 vertex_direction;

    // debug variable to check relation between tracking reconstruction and MC events
    bool debug_trackid;
    ofstream myfile;

    // debug variable for plots of triggered events
    bool isPrimaryInEvent;

    // outfile string
    TString outfile;

    int novtmatch;
  };

#endif
