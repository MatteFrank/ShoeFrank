#if !defined(__CINT__) || defined(__MAKECINT__)

#include <vector>
#include <map>

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>
#include <TH1.h>

#include "TAGcampaignManager.hxx"
#include "TAGactTreeReader.hxx"
#include "TAGrecoManager.hxx"
#include "TAGnameManager.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGionisMaterials.hxx"
#include "TAGrunInfo.hxx"
#include "TAGparGeo.hxx"

#include "TASTparGeo.hxx"
#include "TASTntuHit.hxx"

#include "TABMparGeo.hxx"
#include "TABMntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TABMntuTrack.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTntuHit.hxx"
#include "TAVThit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTcluster.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAITparGeo.hxx"
#include "TAITntuCluster.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDcluster.hxx"
#include "TAMSDntuTrack.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDhit.hxx"
#include "TAMSDtrack.hxx"

#include "TATWparGeo.hxx"
#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"
#include "TATWparameters.hxx"

#include "TAMCntuPart.hxx"
#include "TAMCntuRegion.hxx"
#include "TAMCntuHit.hxx"

#include "TAGntuEvent.hxx"

#include "TAGntuGlbTrack.hxx"


#endif

static TAGrunInfo*   runinfo;  //runinfo

static TAGgeoTrafo*  geoTrafo; //geometry
static TAGparGeo* parGeo; //beam info
static TASTparGeo* stparGeo;
static TABMparGeo* bmparGeo;
static TAVTparGeo* vtparGeo;
static TAITparGeo* itparGeo;
static TAMSDparGeo* msdparGeo;
static TATWparGeo* twparGeo;

static TASTntuHit *stNtuHit;
static TAMCntuHit *stMcNtuHit;
static TABMntuHit*  bmNtuHit;
static TABMntuTrack*  bmNtuTrack;
static TAMCntuHit *bmMcNtuHit;
static TAVTntuVertex* vtxNtuVertex;
static TAVTntuCluster *vtxNtuCluster;
static TAVTntuTrack *vtxNtuTrack;
static TAMCntuHit *vtMcNtuHit;
static TAMSDntuCluster *msdNtuClus;
static TAMSDntuTrack *msdNtuTrack;
static TAMSDntuPoint *msdNtuPoint;
static TAMSDntuHit *msdNtuHit;
static TAITntuCluster *itNtuClus;
static TATWntuHit *twNtuHit;
static TATWntuPoint *twNtuPoint;
static TAMCntuHit *twMcNtuHit;

static TAMCntuPart *mcNtuPart;
static TAMCntuRegion *mcNtuRegion;
static TAGntuGlbTrack *glbntutrk;
static TAGntuEvent* tgNtuEvent;

static int  IncludeReg; // from runinfo
static int  IncludeTOE; // from runinfo

//other global parameters taken from campaign manager
static int  IncludeMC;
static int  IncludeDI;
static int  IncludeSC;
static int  IncludeBM;
static int  IncludeTG;
static int  IncludeVT;
static int  IncludeIT;
static int  IncludeMSD;
static int  IncludeTW;
static int  IncludeCA;
static int  IncludeDAQ;

Bool_t ClusteringPositionResolution =  false;
Bool_t StudyEffAndXS                =  true;
Bool_t StudyTWreco                  =  true;
Bool_t StudyFragmentation           =  false;
Bool_t neutronPosOnTW               =  false;
Bool_t StudyCrossings               =  true;

Bool_t dbg        = false;
Bool_t debug      = false;
Bool_t debugEff   = false;
Bool_t debugCMM   = false;
Bool_t dbg_vtx    = false;

enum{kCharges=8,kLayers=2};
enum{kVTreg=2,kTWreg=4};
enum FlukaVar {kPrimaryID=0,kNeutronFlukaId=8};

const TString ElementName[kCharges] = {"H","He","Li","Be","B","C","N","O"};

typedef std::map<Int_t,std::vector<Int_t> > TMapMatch;
typedef std::vector<std::pair<Int_t,Int_t> > TVecPair;

const double Na = 6.022*1e23;
const double A_TG = 12.0107;  // carbon target
const double dx_TG = 0.5;  // TG thickness in cm
const double rho_TG = 1.83;  // TG density in g/cm3

const double maxTGx = 10;
const double maxTGy = 10;
// const double maxTGx = 1.0;
// const double maxTGy = 1.0;

double Nprim = 1e7;  // Number of primaries in simulation GSI with fragmentation trigger
double Nprim_impingingOnTG = 0;  // Number of primaries in simulation GSI with fragmentation trigger
double Nprim_notInteracting = 0;  // Number of primaries in simulation GSI with fragmentation trigger
int Nprim_interacting=0;


TH1I *h_multHit;
TH1I *h_multHitcnt;
TH1I *h_multHitBar;
TH1D *h_dist;
TH2D *h_posXY;
TH2D *dE_vs_tof[kLayers];
TH2D *dE_vs_tof_Z[kLayers][kCharges];
TH2D *dE_vs_tofRec[kLayers];
TH2D *h_reg_vs_z;
TH2D* hCMM;                        // Charge Mixing Matrix
TH2D* hCMM_rec;                    // Charge Mixing Matrix for reconstructed TW points
TH2D* hCMM_crossing;               // Charge Mixing Matrix for reconstructed TW points
TH2D* hCMM_crossing_inTG;          // Charge Mixing Matrix for reconstructed TW points
TH2D* hCMM_TWhits[kLayers];        // Charge Mixing Matrix for reconstructed TW hits
TH2D* hekin_vs_angle[kCharges];    // ekin vs angle at production for only fragmetns exiting the TG
TH1D* hekin[kCharges];             // ekin at production for only fragmetns exiting the TG
TH1D* hangle[kCharges];            // angle at production for only fragmetns exiting the TG

TH1D* hyields_fromTG;              // integral yields coming from TG + mothId=0
TH1D* hyields_allCuts;             // integral yields coming from TG + mothId=0 + all cuts
TH1D* hyields_intersections;       // integral yields of trks produced in TG + mothId=0 & projected on the TW
TH1D* hyields_fromCrossings;       // integral yields of trks crossing the TW from the Air pre-TW
TH1D* hyields_fromCrossings_inTG;  // integral yields of trks crossing the TW from the Air pre-TW produced in TG
TH1D* hyields_multiHitReco;        // integral yields of trks crossing the TW from the Air pre-TW
TH1D* hyields_multiHitReco_inTG;   // integral yields of trks crossing the TW from the Air pre-TW produced in TG
TH1D* hWrongZ;                     // wrong Z reco
TH1D* hWrongZ_inTG;                // wrong Z reco of tracks produced in TG
TH1D* hWrongZ_crossing;            // wrong Z reco from crossing TW
TH2D* hWrongZmult;            // wrong Z reco from crossing TW
TH1D* hyields_rec;                 // integral yields rec
TH1D* hyields_crossTW;             // integral yields rec
TH1D* hyields_TWhits[kLayers];     // integral yields rec TW hits
TH1D* hyields_bkg;                 // integral yields bkg
TH1D* hyields_trk_bkg;                 // integral yields bkg mothId=0 MC true
TH1D* hyields_prim_bkg;                 // integral yields bkg mothId=0
TH1D* hyieldsPU;                   // integral yields due to Pile-Up
TH1D* hyields_ang[kCharges];       // differential angular yields
TH1D* hyields_ekin[kCharges];      // differential ekin yields

TH1D* hyields_trk;                 // integral yields - MC true tracks - produced in TG + mothId=0
TH1D* hyields_trk_inTG;            // integral yields - MC true tracks - produced in TG
TH1D* hyields_XStrue;              // integral yields for XS true - MC true tracks
TH1D* hyields_ang_trk[kCharges];   // differential angular yields - MC true tracks
TH1D* hyields_ekin_trk[kCharges];  // differential ekin yields - MC true tracks

TH1D *hXS_dOmega[kCharges];
TH1D *hXS_dEkin[kCharges];

TH1D* heff;                     // eff to correct TG contribution for XS
TH1D* heff_bkg;                     // eff to correct no TG contribution
TH1D* heff_ang[kCharges];       // differential angular yields
TH1D* heff_ekin[kCharges];      // differential ekin yields

// TH1D* htof_all;
// TH1D* htof[kCharges];
TH1D* heloss_all;
TH1D* heloss[kCharges];
TH1D* heloss_all_frg_noTG;
// TH1D* heloss_frg_noTG[kCharges];
// TH1D* heloss_all_frg_cut;
// TH1D* heloss_frg_cut[kCharges];	

TH1D* heloss_all_prim_noTG;
TH1D* heloss_prim_noTG[kCharges];
TH1D* heloss_all_primVT;
TH1D* heloss_primVT[kCharges];

TH1D* heloss_all_cut;
TH1D* heloss_cut[kCharges];
TH1D* heloss_all_cutXS;
TH1D* heloss_cutXS[kCharges];

TH1D *distAlongX[kCharges];
TH1D *distAlongY[kCharges];
TH1D *distAlongX_cutZ[kCharges];
TH1D *distAlongY_cutZ[kCharges];
TH1D *Z_prim_noTG;
TH1D *Z_sec_fragmentation;
TH1D *Reg_prim_noTG;
TH1D *Tof_prim_noTG;

//Bins for theta in degree
const Int_t nAbins = 32;
Double_t abins[nAbins] = {0.0,0.4,0.8,1.2,1.6,2.0,2.4,2.8,3.2,3.6,4.0,4.4,4.8,5.2,5.6,6.0,6.4,6.8,7.2,7.6,8.0,8.4,8.8,9.2,9.6,10.0,10.4,10.8,11.2,11.6,12};

// const Int_t nAbins = 16;
// Double_t abins[nAbins] = {0.,0.4,0.8,1.2,1.6,2.,2.4,2.8,3.2,3.6,4.,4.4,4.8,5.2,5.6,6.};

// Bins for KeA -> (gamma-1) * 1000
// const Int_t nEbins = 15;
// Double_t ebins[nEbins] = {0.,200.,230.,260.,290.,320.,350.,380.,420.,460.,500.,550.,600.,700.,800.};
const Int_t nEbins = 15;
Double_t ebins[nEbins] = {0.,40.,80.,120.,160.,200.,240.,280.,340.,400.,460.,520.,600.,680.,800.};
// Double_t ebins[nEbins] = {0.,20.,40.,60.,80.,100.,120.,140.,170.,200.,230.,260.,300.,340.,400.};

// Bins for Beta  
const Int_t nBB = 20;
Double_t bbins[nBB] = {0,0.2,0.3,0.4,0.5,0.54,0.58,0.60,0.62,0.64,0.66,0.68,0.70,0.72,0.74,0.78,0.82,0.86,0.9,1.0};

// Bins for Momentum/A
const Int_t nmomZ = 19;  // momZ = momentum/Chg_generated

Double_t mbins[nmomZ] = {0.0,0.4,0.8,1.0,1.2,1.4,1.6,1.8,2.0,
  2.2,2.4,2.6,2.8,3.0,3.2,3.4,3.6,3.8,4.0};

TH1D *h_eloss;

void  BookHistograms();
void  GetFOOTgeo(TAGcampaignManager* camp_manager, Int_t run_number);
void  GetRunAndGeoInfo( TAGcampaignManager* campManager, Int_t runNumber);
void  SetTreeBranchAddress(TAGactTreeReader *treeReader);
void  ComputeBayesianEfficiency(Int_t ibin, Double_t num, Double_t den, Double_t& eff, Double_t& e_eff);
void  ComputeBinomialEfficiency(Int_t ibin, Double_t num, Double_t den, Double_t& eff, Double_t& e_eff);
void  ProjectTracksOnTw(int Z, TVector3 init_pos, TVector3 init_p);
void  LoopOverMCtracks(Int_t Emin, Int_t Emax, Bool_t isnotrig);

Bool_t IsVTregion(int reg);

inline Int_t GetZbeam() {return parGeo->GetBeamPar().AtomicNumber;}   
// TW center in global ref frame
inline TVector3 GetTwCenter() {return geoTrafo->GetTWCenter();}   
// TW theta angle geometrical acceptance
inline Double_t GetMaxAngle() {return TMath::ATan(((nBarsPerLayer*twparGeo->GetBarWidth())/2-TMath::Abs(GetTwCenter().y())-maxTGy)/GetTwCenter().z()); } // rad   


