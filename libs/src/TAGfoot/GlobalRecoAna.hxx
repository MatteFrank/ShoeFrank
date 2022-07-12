#ifndef GlobalRecoAna_hxx
#define GlobalRecoAna_hxx

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
#include "TAGrunInfo.hxx"
#include "TAGgeoTrafo.hxx"
#include "GlobalRecoMassAna.hxx"

using namespace std;

class GlobalRecoAna : public BaseReco {

  public:
  GlobalRecoAna(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC, Int_t nTotEv);
  ~GlobalRecoAna();

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
  void Booking();
  void SetupTree();
  void SetMCtruth(map<int, map<int, map<int,vector<int>>>> mymap, int &idx, int &cha);
  void SetMCtruthBIS(map<int, map<int, map<int,vector<int>>>> mymap, int &idx, int &cha);
  void ComputeMCtruth( Int_t trkid, int &cha, TVector3 &mom, TVector3 &mom_cross, double &ek);
  Double_t ComputeTrkEkin(TAGtrack *track);//from calo infos
  void resetStatus(); //to reset the subdetectors status flags

  //fill plots
  void FillGlbTrackPlots();
  void FillMassPlots();

  //useful formulas
  Double_t GetGamma(Double_t beta){return 1./sqrt(1.-beta*beta);};
  Double_t GetMassPB(Double_t mom, Double_t beta) {return (1./atomassu)*mom*sqrt(1.-beta*beta)/beta;};
  Double_t GetMassBE(Double_t beta, Double_t ekin){return (1./atomassu)*ekin/(GetGamma(beta)-1);};
  Double_t GetMassPE(Double_t mom, Double_t ekin) {return (1./atomassu)*(mom*mom-ekin*ekin)/2./ekin;};

  //after event loop
  void StudyThetaReso();
  //Print infors
  void PrintNCharge();

  //useful analysis variables
  Double_t atomassu = 0.931494;//atomic mass unit in GeV/c^2
  Int_t primary_cha;       //charge of primary particle
  Double_t primary_tof;    //TOF of the primary particle from ST to target (ns), evaluated from Beam initial energy value
  Double_t Ntg;            //Number of particles per unit surface within the target = density*Na*thickness/A

  //counters (maybe a struct could be useful)
  Int_t nchargeok_vt ;
  Int_t nchargeok_vt_clu ;
  Int_t nchargeok_it ;
  Int_t nchargeok_it_clu ;
  Int_t nchargeok_msd ;
  Int_t nchargeok_msd_clu ;
  Int_t nchargeok_tw ;
  Int_t nchargeall_vt ;
  Int_t nchargeall_vt_clu ;
  Int_t nchargeall_it ;
  Int_t nchargeall_it_clu ;
  Int_t nchargeall_msd ;
  Int_t nchargeall_msd_clu ;
  Int_t nchargeall_tw ;

  Int_t npure_vt ;
  Int_t npure_it ;
  Int_t npure_msd ;
  Int_t npure ;
  Int_t nclean ;
  Int_t ntracks;
  Int_t nTotEv;  //total number of events (-nev flag)

  vector<pair<Int_t,Int_t>> pure_track_xcha;    //vector index = particle charge; first=npure; second=ntracks

  //status variables
  Int_t twstatus;       //0=ok 1 tw hit on both bars, -1=not set

  //setting variables maybe we should use a config file?
  Double_t Th_meas;
  Double_t purity_cut;      //minumum purity value for a track to be defined as pure
  Double_t clean_cut;       //is a 100% pure track

  bool DiffApp_trkIdx;

  //useful variables
  TFile *file_out;
  TAGactTreeReader* myReader;
  GlobalRecoMassAna *mass_ana;
  TAGtrack *fGlbTrack;
  vector<vector<Int_t>> fGlbTrkVec; //store the global track detector point index, 0=VTX, 1=IT, 2=MSD, 3=TW, 4=CALO, for each global track
  vector<vector<vector<Int_t>>> fEvtGlbTrkVec; //store collection of fGlbTrkVec for each event

  //nuisance variables
  TH1D *h;  
  TH2D *h2;
  TH3D *h3;


  //binning
  Int_t th_nbin;
  Double_t **theta_binning;
  Int_t ek_nbin;
  Double_t **ek_binning;
  Int_t mass_nbin;
  Double_t **mass_binning;

  TFile *f;

};

#endif
