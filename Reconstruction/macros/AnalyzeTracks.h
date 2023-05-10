
#if !defined(__CINT__) || defined(__MAKECINT__)

#include <vector>
#include <map>

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>
#include <TH1.h>
#include <TH3.h>

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


//Bins for theta in degree
const Int_t nAbins = 32;
Double_t abins[nAbins] = {0.0,0.4,0.8,1.2,1.6,2.0,2.4,2.8,3.2,3.6,4.0,4.4,4.8,5.2,5.6,6.0,6.4,6.8,7.2,7.6,8.0,8.4,8.8,9.2,9.6,10.0,10.4,10.8,11.2,11.6,12};

const Int_t nEbins = 15;
Double_t ebins[nEbins] = {0.,40.,80.,120.,160.,200.,240.,280.,340.,400.,460.,520.,600.,680.,800.};

// Bins for Beta  
const Int_t nBB = 20;
Double_t bbins[nBB] = {0,0.2,0.3,0.4,0.5,0.54,0.58,0.60,0.62,0.64,0.66,0.68,0.70,0.72,0.74,0.78,0.82,0.86,0.9,1.0};

// Bins for Momentum/A
const Int_t nmomZ = 19;  // momZ = momentum/Chg_generated

Double_t mbins[nmomZ] = {0.0,0.4,0.8,1.0,1.2,1.4,1.6,1.8,2.0,
  2.2,2.4,2.6,2.8,3.0,3.2,3.4,3.6,3.8,4.0};


void  BookHistograms();
void  GetFOOTgeo(TAGcampaignManager* camp_manager, Int_t run_number);
void  GetRunAndGeoInfo( TAGcampaignManager* campManager, Int_t runNumber);
void  SetTreeBranchAddress(TAGactTreeReader *treeReader);
void  ComputeBayesianEfficiency(Int_t ibin, Double_t num, Double_t den, Double_t& eff, Double_t& e_eff);
void  ComputeBinomialEfficiency(Int_t ibin, Double_t num, Double_t den, Double_t& eff, Double_t& e_eff);
void  ProjectTracksOnTw(int Z, TVector3 init_pos, TVector3 init_p);
void  LoopOverMCtracks(Int_t Emin, Int_t Emax, Bool_t isnotrig);
TVector3 GetPOCA(TVector3 p1,TVector3 dir1, TVector3 p2,TVector3 dir2);

Bool_t IsVTregion(int reg);

inline Int_t GetZbeam() {return parGeo->GetBeamPar().AtomicNumber;}   
// TW center in global ref frame
inline TVector3 GetTwCenter() {return geoTrafo->GetTWCenter();}   
// TW theta angle geometrical acceptance
//inline Double_t GetMaxAngle() {return TMath::ATan(((nBarsPerLayer*twparGeo->GetBarWidth())/2-TMath::Abs(GetTwCenter().y())-maxTGy)/GetTwCenter().z()); } // rad   



TH1D *hResxVT_plane[4];
TH1D *hResyVT_plane[4];
TH1D *hReszVT_plane[4];
TH1D *hResxMSD_plane[6];
TH1D *hResyMSD_plane[6];
TH1D *hReszMSD_plane[6];
TH1D *hResxTW;
TH1D *hResyTW;
TH1D *hReszTW;
TH1D *hResxVT_plane_Z[4][kCharges];
TH1D *hResyVT_plane_Z[4][kCharges];
TH1D *hReszVT_plane_Z[4][kCharges];
TH1D *hResxMSD_plane_Z[6][kCharges];
TH1D *hResyMSD_plane_Z[6][kCharges];
TH1D *hReszMSD_plane_Z[6][kCharges];
TH1D *hResxTW_Z[kCharges];
TH1D *hResyTW_Z[kCharges];
TH1D *hReszTW_Z[kCharges];



TH2D *hResTGT;
TH2D *hResTW;
TH1D *hResThetaTGT;
TH1D *hResThetaTGTbm;
TH2D *hResThetaVsThetaTGT;
TH2D *hResThetaVsThetaTGTbm;
TH2D *hThetaRecoVsTrueTGT;
TH2D *hThetaRecoVsTrueTGTbm;
TH3D *hPOCA;
TH3D *hPosTGT;


TH2D *hResTGT_Z[kCharges];
TH2D *hResTW_Z[kCharges];
TH1D *hResThetaTGT_Z[kCharges];
TH1D *hResThetaTGTbm_Z[kCharges];
TH2D *hResThetaVsThetaTGT_Z[kCharges];
TH2D *hResThetaVsThetaTGTbm_Z[kCharges];
TH2D *hThetaRecoVsTrueTGT_Z[kCharges];
TH2D *hThetaRecoVsTrueTGTbm_Z[kCharges];
TH3D *hPOCA_Z[kCharges];
TH3D *hPosTGT_Z[kCharges];





TH2D *hZRecoVsTrue;
TH2D *hZRecoVsTrueCut;

