#define GlobalRecoAna_cxx
#include <TVector3.h>
#include <TRandom.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>


#include <TMultiGraph.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include "BaseReco.hxx"
#include "TAVTtrack.hxx"
#include "TAMCntuPart.hxx"
#include "RecoRaw.hxx"
#include "RecoMC.hxx"

#include "GlobalRecoAna.hxx"

using namespace std;


GlobalRecoAna::GlobalRecoAna(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout, Bool_t isMC, Int_t innTotEv) : RecoRaw(expName, runNumber, fileNameIn, fileNameout)
{
  nchargeok_vt = 0;
  nchargeok_vt_clu = 0;
  nchargeok_it = 0;
  nchargeok_it_clu = 0;
  nchargeok_msd = 0;
  nchargeok_msd_clu = 0;
  nchargeok_tw = 0;
  nchargeall_vt = 0;
  nchargeall_vt_clu = 0;
  nchargeall_it = 0;
  nchargeall_it_clu = 0;
  nchargeall_msd = 0;
  nchargeall_msd_clu = 0;
  nchargeall_tw = 0;

  npure_vt = 0;
  npure_it = 0;
  npure_msd = 0;
  npure = 0;
  nclean = 0;
  ntracks = 0;
  recoEvents = 0;
  fFlagMC = isMC;
  purity_cut=0.51;
  clean_cut=1.;
  nTotEv=innTotEv;

  outfile = fileNameout;

  Th_true = -999.; //from TAMCparticle
  Th_meas = -999.; //from TWpoint
  Th_reco = -999.; //from global tracking
  Th_recoBM = -999.; //from global tracking wrt BM direction


}

GlobalRecoAna::~GlobalRecoAna()
{
}

void GlobalRecoAna::LoopEvent() {

  if(FootDebugLevel(1))
    cout<<"GlboalRecoAna::LoopEvent start"<<endl;
  if (fSkipEventsN>0)
  {
  currEvent=fSkipEventsN;
  nTotEv+=  fSkipEventsN;
  }
  else currEvent=0;

//*********************************************************************************** begin loop on every event **********************************************
  while(gTAGroot->NextEvent()) { //for every event
    //fFlagMC = false;     //N.B.: for MC FAKE REAL
    if (currEvent % 100 == 0 || FootDebugLevel(1))
      cout <<"current Event: " <<currEvent<<endl;
    DiffApp_trkIdx = false;
    SelectionCuts();



    ClustersPositionStudy();

    int evtcutstatus=ApplyEvtCuts();
    ((TH1D*)gDirectory->Get("Evtcutstatus"))->Fill(evtcutstatus);
    if(evtcutstatus){
      ++currEvent;
      continue;
    }


    Int_t nt =  myGlb->GetTracksN(); //number of reconstructed tracks for every event
    ((TH1D*)gDirectory->Get("ntrk"))->Fill(nt);
    if (nt > 0) recoEvents++;

          TAWDntuTrigger *wdTrig = 0x0;
    if (fFlagMC ==false){
      wdTrig = (TAWDntuTrigger*)fpNtuWDtrigInfo->GenerateObject();    //trigger from hardware
      FragTriggerStudies();
    }

    if (fFlagMC ==true && nt >0){
    //initialize Trk Id to 0
      TrkIdMC = -1;
      N_TrkIdMC_TW =-1;
      TrkIdMC_TW = -1;
    }

    //TW ghost hits studies, needed for the following yield measurements
    if (fFlagMC == true )
      TrackVsMCStudy();

    //*********************************************************************************** begin loop on global tracks **********************************************
    for(int it=0;it<nt;it++) { // for every track
      int trkstatus=ApplyTrkCuts();
      ((TH1D*)gDirectory->Get("Trkcutstatus"))->Fill(trkstatus);
      if(trkstatus){
        ntracks++;
        continue;
      }

      isPrimaryInEvent = false;
      fGlbTrack = myGlb->GetTrack(it);

      //fEvtGlbTrkVec.clear(); //!
      //ComputeMCtruth(TrkIdMC, Z_true, P_true, P_cross, Ek_true);

      if(fFlagMC){
        MCGlbTrkLoopSetVariables();
        //GlbTrackPurityStudy();
      }

      //Set the reconstructed quantities
      RecoGlbTrkLoopSetVariables();
      //Evaluate the measured mass
      if(fFlagMC){
      ThetaTrueVSThetaRecoPlots();
      }

      EvaluateMass();

      if(FootDebugLevel(1)){
        cout<<"GlobalRecoAna::Z_meas="<<Z_meas<<"  P_meas="<<P_meas<<"  primary_tof="<<primary_tof<<"  Tof_meas="<<Tof_meas<<"  Beta_meas="<<Beta_meas<<"  M_meas="<<M_meas<<"  Ek_meas="<<Ek_meas<<endl;
        cout<<"MassPb="<<mass_ana->GetMassPb()<<"  MassPe="<<mass_ana->GetMassPe()<<"  MassBe="<<mass_ana->GetMassBe()<<endl;
      }

      //some specific studies
      EkBinningStudies();
      FillUnfoldingPlots();

      //fill the yields plots
      if(fFlagMC)
        FillMCGlbTrkYields();
      else
        FillDataGlbTrkYields();

      //fill the plots related to glb tracks
      FillTrkPlots();

      ntracks++;
      if (Z_meas==primary_cha)
        isPrimaryInEvent = true;  // needed in AlignmentStudy()

      //re-initialize trk ID to 0 ; needed in TWStudy
      if(fFlagMC){
        TrkIdMC = -1;
        N_TrkIdMC_TW =-1;
        TrkIdMC_TW = -1;
      }

    } //********* end loop on global tracks ****************

    //Studies dedicated to MC dataset
    if(fFlagMC)
      MCParticleStudies();

    //studies dedicated to alignment
    if (fFlagMC == false)
      AlignmentStudy();

    // FullCALOanal();

    ++currEvent;
    if (currEvent == nTotEv)
      break;

  }//end of loop event

  return;
}

void GlobalRecoAna:: Booking(){
//fFlagMC = false; //N.B.: for MC FAKE REAL

  if(FootDebugLevel(1))
    cout<<"start GlboalRecoAna::Booking"<<endl;

  char name[200],title[200];
  file_out->cd();


  h = new TH1D("ntrk","",10, 0 ,10.);
  h = new TH1D("Evtcutstatus","Status for the evet cuts;0=ok,1=BM cuts",11, -0.5 ,10.5);
  h = new TH1D("Trkcutstatus","Status for the track cuts;0=ok,1=NoCalEnergy",11, -0.5 ,10.5);
  h = new TH1D("Energy","",100, 0 ,800.);
  h = new TH1D("Charge_trk","",10, 0 ,10.);
  h = new TH1D("Charge_trk_frag","",10, 0 ,10.);
  h = new TH1D("Charge_trk_True","",10, 0 ,10.);
  h2 = new TH2D("Z_track_Mixing_matrix", "Mixing_matrix",8,0.5,8.5,8,0.5,8.5);
  h2 = new TH2D("Z_tw_Mixing_matrix", "Mixing_matrix",8,0.5,8.5,8,0.5,8.5);
  h2 = new TH2D("Z_tw_Mixing_matrix2frag", "Mixing_matrix",8,0.5,8.5,8,0.5,8.5);
  h2 = new TH2D("Z_tw_Mixing_matrix!", "Mixing_matrix",8,0.5,8.5,8,0.5,8.5);
  h2 = new TH2D("devstof2frag","devstof2frag",500,0.,50.,480,0.,120. );
  h2 = new TH2D("devstof","devstof",500,0.,50.,480,0.,120. );
  h2 = new TH2D("devstof!","devstof",500,0.,50.,480,0.,120. );
  h2 = new TH2D("devstofAll","devstof",500,0.,50.,480,0.,120. );

  h = new TH1D("originPosition","originPosition",200,-200,+200);

  //h = new TH1D("Charge_efficiency","Charge_trk_True / Charge_MC ",10, 0 ,10.);

  h = new TH1D("Charge_purity","",10, 0 ,10.);
  h = new TH1D("Mass","Mass [amu]",200, 0 ,20.);
  h = new TH1D("Mass_True","Mass_True [amu]",200, 0 ,20.);
  h = new TH1D("ThReco","",200, 0 ,50.);
  h = new TH1D("ThReco_frag","",200, 0 ,50.);
  h = new TH1D("ThReco_fragMC","",200, 0 ,50.);

  //h = new TH1D("ThReco","",200, 0 ,50.);
  h = new TH1D("theta_VTX_frag","",100, 0 ,50.);
  h = new TH1D("theta_VTX","",100, 0 ,50.);
  h = new TH1D("phi_VTX_frag","",100, -180 ,180.);
  h = new TH1D("phi_VTX","",100, -180 ,180.);
  h = new TH1D("phi_VTX_global_frag","",100, -180 ,180.);
  h = new TH1D("phi_VTX_global","",100, -180 ,180.);

  h = new TH1D("ThTrue","",200, 0 ,50.);
  h = new TH1D("Tof_tw","TOF from TW point; [ns]",200, 0., 10.);
  h = new TH1D("Tof_meas","TOF from TW point-primary tof; [ns]",200, 0., 10.);
  h = new TH1D("Beta_meas","Measured beta;",200, 0., 1.);

  h2  = new TH2D("trackletdirection_frag","",40, -2 ,2., 40, -2 ,2.);
  h2  = new TH2D("trackletdirection","",40, -2 ,2., 40, -2 ,2.);
  h2  = new TH2D("vt_twProjection_frag","",20, -10 ,10., 20, -10 ,10.);
  h2  = new TH2D("vt_twProjection","",20, -10 ,10., 20, -10 ,10.);
  h2  = new TH2D("TWpointsDistribution_frag","",40, -20. ,20., 40, -20 ,20.);
  h2  = new TH2D("TWpointsDistribution","",40, -20. ,20., 40, -20 ,20.);

  if (fFlagMC == true) {
    gDirectory->mkdir("TrkVsMC");
    gDirectory->cd("TrkVsMC");
    h2  = new TH2D("Z_truevsZ_reco_TWFixed","Z_truevsZ_reco_TWFixed;Z_true;Z_reco",10, 0 ,10., 10, 0 ,10.);
    h2  = new TH2D("Z_truevsZ_reco_TrackFixed","Z_truevsZ_reco_TrackFixed;Z_true;Z_reco",10, 0 ,10., 10, 0 ,10.);
    h2  = new TH2D("Z_truevsZ_reco_TWGhostHitsRemoved","Z_truevsZ_reco_TWGhostHitsRemoved;Z_true;Z_reco",10, 0 ,10., 10, 0 ,10.);
    h2  = new TH2D("Z_truevsZ_reco_All","Z_truevsZ_reco_All;Z_true;Z_reco",10, 0 ,10., 10, 0 ,10.);


    gDirectory->cd("..");
  }

  h2  = new TH2D("Z_truevsZ_reco","",10, 0 ,10., 10, 0 ,10.);
  h2  = new TH2D("Z_TWvsZ_fit","",10, 0 ,10., 10, 0 ,10.);

  if(fFlagMC){
    h = new TH1D("Tof_true","TOF from MC truth; [ns]",200, 0., 10.);
    h = new TH1D("Beta_true","Beta from MC truth; ",200, 0., 1.);
  }


  for(int ic=0; ic<=primary_cha; ic++){
    gDirectory->mkdir(Form("Zrec%d",ic));
    gDirectory->cd(Form("Zrec%d",ic));
    int iz=max(ic,1);
      h = new TH1D("Mass","Reconstructed mass spectra;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
      h = new TH1D("Mass_clean","Reconstructed mass spectra for pure tracks;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
      h = new TH1D("Mass_pure","Reconstructed mass spectra for pure tracks;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
      h = new TH1D("Mass_impure","Reconstructed mass spectra not pure tracks;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
      h = new TH1D("Mass_impure_2","Reconstructed mass spectra for tracks with multiplicity 2;Mass[GeV];",iz*200, (Double_t)iz/2. ,(Double_t)iz*2.5);
      h = new TH1D("Track_purity","Number of right clusters over total number of clusters;Mass[GeV];Tracks",120, -0.1 ,1.1);
      for(int kz=0; kz<=primary_cha; kz++){
        gDirectory->mkdir(Form("Zmc%d",kz));
        gDirectory->cd(Form("Zmc%d",kz));
          h = new TH1D("Mass_clean","Reconstructed mass spectra;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
          h = new TH1D("Mass_pure","Reconstructed mass spectra;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
          h = new TH1D("Mass_impure","Reconstructed mass spectra;Mass[Amu];",iz*300, (Double_t)iz/2. ,(Double_t)iz*2.5);
        gDirectory->cd("..");
      }
    gDirectory->cd("..");

  }

  gDirectory->mkdir("Ekin");
  gDirectory->cd("Ekin");

  for(int iz=0; iz<=primary_cha; iz++){
    gDirectory->mkdir(Form("Z%d",iz));
    gDirectory->cd(Form("Z%d",iz));

    h2 = new TH2D("DE_vs_Ekin","", 200, 0.,600., 200,0.,600.);
    h = new TH1D("Ek_meas","",100, 0 ,800.);

    gDirectory->cd("..");
  }
  gDirectory->cd("..");



//binning of theta, ekin, A
th_nbin = 20;
theta_binning = new double *[th_nbin];
for (int i = 0; i<th_nbin; i++) {
  theta_binning[i] = new double [2];
  theta_binning[i][0] = double (i)*0.5;
  theta_binning[i][1] = double (i+1)*0.5;
  //cout << " theta binning "<< i << " "<< theta_binning[i][0] << " "<< theta_binning[i][1];
}
//theta_binning[60] = new double [2];    //!hard coded
//theta_binning[60][0] = double(30);
//theta_binning[60][1] =  double(90);
//cout << " theta binning "<< 10 << " "<< theta_binning[10][0] << " "<< theta_binning[10][1];

ek_nbin = 1;
ek_binning = new double *[ek_nbin];
for (int i = 0; i<ek_nbin; i++) {
  ek_binning[i] = new double [2];
  ek_binning[i][0] = double (i*100);
  ek_binning[i][1] = double ((i+1)*20000);
}
  //ek_binning[4] = new double [2];
  //ek_binning[4][0] = double(400);
  //ek_binning[4][1] =  double(1000);

mass_nbin = 1;
mass_binning = new double *[mass_nbin];
for (int i = 0; i<mass_nbin; i++) {
  mass_binning[i] = new double [2];
  mass_binning[i][0] = double (i*2);
  mass_binning[i][1] = double ((i+1)*100);
}


//UNFOLDING folders
 gDirectory->mkdir("Unfolding");
 gDirectory->cd("Unfolding");
 Int_t tot_bin=th_nbin*ek_nbin*primary_cha;
 h2 = new TH2D("Unfolding_trk_vs_true","Unfolding;trk;true", tot_bin, 0, tot_bin, tot_bin, 0, tot_bin);
 h = new TH1D("RecoDistribution","", tot_bin, 0., tot_bin);
 gDirectory->cd("..");

// Cross section recostruction histos MC
if(fFlagMC){
BookYield ("yield-trkMC", true);

// Cross section recostruction histos MC + GHOST HITS FIXED
BookYield ("yield-trkGHfixMC", true);

// Cross section recostruction histos MC + ALL TW HITS FIXED
BookYield ("yield-trkcutsMC", true);

// Cross section recostruction histos from REAL DATA
BookYield ("yield-trkReco");

// Cross section TRUE histos
BookYield ("yield-true_cut");

// Cross section TRUE histos DETECTABLE (PARTICLES WHICH REACH THE TW)
BookYield ("yield-true_DET");

// Cross section TRUE histos reco real
TString name = GetTitle();
Int_t pos = name.Last('.');
string name_ = "yield-trkREAL";
name_ += name[pos-1];
BookYield (name_.c_str());




} else {

// Cross section recostruction histos from REAL DATA
BookYield ("yield-trkREAL");
}

//cuts selection
h = new TH1D("h_eventSelected", "Event cuts in selector; cut; Events", 5, 0., 5.);
std::string name_h[5] = {"All events","At least 1 TW point","1 BM Track",">0 vtx points","vtx - BM matching"};
for(int i=1; i<=5; i++){
	//int bin2 = h_eventSelected->GetXaxis()->FindBin(i);
	((TH1D*)gDirectory->Get("h_eventSelected"))->GetXaxis()->SetBinLabel(i, name_h[i-1].c_str());
}

  if(fFlagMC == false ){
    gDirectory->mkdir("fragTriggerStudies");
    gDirectory->cd("fragTriggerStudies");
      h = new TH1D("chargeMB","chargeMB",8, 0.5 ,8.5);
      h = new TH1D("chargeMBFrag","chargeMBFrag",8, 0.5 ,8.5);
      h = new TH1D("chargeMBFrag_efficiency","chargeMBFrag",8, 0.5 ,8.5);
      h = new TH1D("chargeMBFrag_RejectPower","",1, 0. ,1.);
    gDirectory->cd("..");
  }

  if(fFlagMC){
    gDirectory->mkdir("MC_check");
    gDirectory->cd("MC_check");
    //"tg" stands for generated from target
    //"tg_tw" stands for generated from target and died after tof wall
    h = new TH1D("TracksN_MC","",100, 0., 100.);
    h = new TH1D("Charge_MC","",20, 0., 20.);
    h = new TH1D("Charge_MC_tg","",10, 0 ,10.);
    h = new TH1D("Charge_MC_tg_tw","",10, 0 ,10.);
    h = new TH1D("Mass_MC","",400, 0., 50.);
    h = new TH1D("Mass_MC_tg","",400, 0., 50.);
    h = new TH1D("Mass_MC_tg_tw","",400, 0., 50.);
    h = new TH1D("Ek_tot_MC","",1000, 0 ,20000.);
    h = new TH1D("Ek_tot_MC_tg","",1000, 0 ,20000.);
    h = new TH1D("Ek_tot_MC_tg_tw","",1000, 0 ,20000.);
    h = new TH1D("InitPosZ_MC","",400, 0., 200.);
    h = new TH1D("InitPosZ_MC_tg","",400, 0., 200.);
    h = new TH1D("InitPosZ_MC_tg_tw","",400, 0., 200.);
    h = new TH1D("FinalPosZ_MC","",400, 0., 200.);
    h = new TH1D("FinalPosZ_MC_tg","",400, 0., 200.);
    h = new TH1D("FinalPosZ_MC_tg_tw","",400, 0., 200.);
    h = new TH1D("TrkLength_MC","",500, 0., 2000.);
    h = new TH1D("TrkLength_MC_tg","",500, 0., 2000.);
    h = new TH1D("TrkLength_MC_tg_tw","",500, 0., 2000.);
    h = new TH1D("Dead_MC","",500, 0., 2000.);
    h = new TH1D("Time_MC","",100, 0., 20.);
    h = new TH1D("Tof_MC","",100, 0., 20.);
    h = new TH1D("Type_MC","",50, 0., 50.);
    h = new TH1D("Type_MC_tg","",50, 0., 50.);
    h = new TH1D("Type_MC_tg_tw","",50, 0., 50.);
    h = new TH1D("Region_MC","",40, 0., 40.);
    h = new TH1D("Baryon_MC","",40, 0., 40.);
    h = new TH1D("Theta_MC","",180, 0., 180.);
    h = new TH1D("Theta_MC_tg","",180, 0., 180.);
    h = new TH1D("Theta_MC_tg_tw","",180, 0., 180.);
    h = new TH1D("FlukaID_MC","",40, 0., 40.);
    h = new TH1D("FlukaID_MC_tg","",40, 0., 40.);
    h = new TH1D("FlukaID_MC_tg_tw","",40, 0., 40.);
    h = new TH1D("MotherID_MC","",40, 0., 40.);
    h = new TH1D("MotherID_MC_tg","",40, 0., 40.);
    h = new TH1D("MotherID_MC_tg_tw","",40, 0., 40.);
    h2 = new TH2D("Mixing_matrix", "Mixing_matrix",8,0.5,8.5,8,0.5,8.5);
    h2= new TH2D("Mixing_matrix_cut", "Mixing_matrix_cut",8,0.5,8.5,8,0.5,8.5);
    gDirectory->cd("..");

    gDirectory->mkdir("MC");
    gDirectory->cd("MC");
    h2 = new TH2D("ChargePoi_vs_ChargeVT","",11, -1. ,10.,11, -1. ,10.);
    h2 = new TH2D("ChargePoi_vs_ChargeIT","",10, 0. ,10.,10, 0. ,10.);
    h2 = new TH2D("ChargePoi_vs_ChargeMSD","",10, 0. ,10.,10, 0. ,10.);
    h2 = new TH2D("ChargePoi_vs_ChargeTW","",10, 0. ,10.,10, 0. ,10.);
    if(TAGrecoManager::GetPar()->IsRegionMc()){
      h2 = new TH2D("MCpartVsGlbtrackNum","Number of MC particles exit from target Vs number of reconstructed tracks;",11, -0.5, 10.5,11, -0.5, 10.5);
      h2 = new TH2D("MCpartVsGlbtrackNum_angle10","Number of MC particles exit from target with angle <10 Vs number of reconstructed tracks;",11, -0.5, 10.5,11, -0.5, 10.5);
    }

    for(int iz=0; iz<=primary_cha; iz++){
      gDirectory->mkdir(Form("Z%d",iz));
      gDirectory->cd(Form("Z%d",iz));
      h = new TH1D("ChargeZ_reso","",11, -5.5, 5.5);
      h = new TH1D("Mass_reso","Mass resolution;Mass measured - MC true [Amu]",600, -3, 3.);
      h = new TH1D("Mom_reso","Momentum resolution with MC initial P; measured - MC true [MeV]",400, -2000., 2000.);
      h = new TH1D("Mom_reso_cross","Momentum resolution with MC crossing P; measured - MC true [MeV]",400, -2000., 2000.);
      h = new TH1D("Ekin_reso","Ekin resolution with MC initial Ekin; measured - MC true [MeV]",200, -100., 100.);
      h = new TH1D("Ekin_reso_cross","Ekin resolution with MC crossing Ekin; measured - MC true [MeV]",200, -100., 100.);
      h = new TH1D("Ekin_calo1cls_reso","Ekin resolution with MC initial Ekin; measured - MC true [MeV]",200, -100., 100.);
      h = new TH1D("Ekin_calo1cls_reso_cross","Ekin resolution with MC crossing Ekin; measured - MC true [MeV]",200, -100., 100.);
      h = new TH1D("Theta_reso","",200, -2., 2.);
      h = new TH1D("Theta_reso_cross","",200, -2., 2.);
      h2 = new TH2D("Theta_reso_cross_vs_th","",50, 0 ,20., 100, -2., 2.);
      h2 = new TH2D("Theta_reso_cross_vs_thmeas","",50, 0 ,20., 100, -2., 2.);
      h2 = new TH2D("Theta_true_cross_vs_meas","Theta measured [deg]; Theta MC cross [deg]",200, 0 ,20., 200, 0., 20.);

      h = new TH1D("Theta_calo1cls_reso","",200, -2., 2.);
      h = new TH1D("Theta_calo1cls_reso_cross","",200, -2., 2.);


      h = new TH1D("Tof_reso","TOF Resolution; MC TOF - Reco TOF [ns]",200, -1., 1.);
      //for hres studies
      gDirectory->mkdir("ThetaRes");
      gDirectory->cd("..");
    }
    gDirectory->cd("..");
  }

  gDirectory->mkdir("MassReco");
  gDirectory->cd("MassReco");
  h = new TH1D("InputStatus","InputStatus; 0=error, 1=no input, 2=Momentum ok, 3=Beta Ok, 5=Energy ok, other values=prod ",31, -0.5, 30.5);
  h = new TH1D("ChiMassStatus","ChiMassStatus; 0=ok, -1=singular matrix for chi2, >0 error ",7, -1.5, 5.5);
  h = new TH1D("AlmMassStatus","AlmMassStatus; 0=ok, -1=singular matrix for chi2, >0 error ",7, -1.5, 5.5);
    for(int ic=0; ic<=primary_cha; ic++){
      gDirectory->mkdir(Form("Zreco%d",ic));
      gDirectory->cd(Form("Zreco%d",ic));
      h = new TH1D("chi2min","chi2 value",250, 0., 2.);
      h = new TH1D("almmin","almmin value",250, 0., 2.);
      h = new TH1D("mom_bestvsmeas","chi2 estimated momentum vs measured;chi2-measured [MeV]",400, -100., 100.);
      h = new TH1D("ekin_bestvsmeas","chi2 estimated ekin vs measured;ekin-measured [MeV]",400, -100., 100.);
      h = new TH1D("tof_bestvsmeas","chi2 estimated tof vs measured;tof-measured [ns]",100, -1., 1.);
      h = new TH1D("Mass_pb","Reconstructed mass spectra with momentum and tof;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_pberr","Mass error for momentum and tof mass estimate;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_be","Reconstructed mass spectra with tof and kinetic energy;Mass number;",1000, 0. ,20.);
      h = new TH1D("Mass_beerr","Mass error for energy and tof mass estimate;Mass number;",250, 0. ,2.);
      h = new TH1D("Mass_pe","Reconstructed mass spectra with momentum and kinetic energy;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_peerr","Mass error for energy and momentum mass estimate;Mass number;",250, 0. ,2.);
      h = new TH1D("Mass_wavg","Reconstructed mass spectra using weighted average;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_wavgerr","Mass error for weighted averate mass estimate;Mass number;",250, 0. ,2.);
      h = new TH1D("Mass_chi","Reconstructed mass spectra using chi square method;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_chierr","Mass error for chi square mass estimate;Mass number;",250, 0. ,2.);
      h = new TH1D("Mom_chi","momentum optimized from chi square method;Momentum [GeV];",1000, 0. ,20);
      h = new TH1D("Mom_chierr","Error for the momentum optimized from chi square method;Momentum [GeV];",200, 0. ,1);
      h = new TH1D("Ekin_chi","Ekin optimized from chi square method;Ekin [GeV];",1000, 0. ,20.);
      h = new TH1D("Ekin_chierr","Error for the Ekin optimized from chi square method;Ekin [GeV];",200, 0. ,0.5);
      h = new TH1D("Tof_chi","TOf optimized from chi square method;Tof [ns];",200, 0. ,15.);
      h = new TH1D("Tof_chierr","Error for the TOf optimized from chi square method;Tof [ns];",200, 0. ,1.);
      h = new TH1D("Mass_alm","Reconstructed mass spectra using alm method;Mass number;",1000, 0. ,20);
      h = new TH1D("Mass_almerr","Mass error for alm mass estimate;Mass number;",250, 0. ,2.);
      h = new TH1D("Mom_alm","momentum optimized from alm method;Momentum [GeV];",1000, 0. ,20);
      h = new TH1D("Mom_almerr","Error for the momentum optimized from alm method;Momentum [GeV];",200, 0. ,1);
      h = new TH1D("Ekin_alm","Ekin optimized from alm method;Ekin [GeV];",1000, 0. ,20.);
      h = new TH1D("Ekin_almerr","Error for the Ekin optimized from alm method;Ekin [GeV];",200, 0. ,0.1);
      h = new TH1D("Tof_alm","TOf optimized from alm method;Tof [ns];",200, 0. ,15.);
      h = new TH1D("Tof_almerr","Error for the TOf optimized from alm method;Tof [ns];",200, 0. ,1.);
      gDirectory->cd("..");
    }
    gDirectory->cd("..");

    gDirectory->mkdir("CALO");
    gDirectory->cd("CALO");

    for(int iz=0; iz<=primary_cha; iz++){
      gDirectory->mkdir(Form("Zreco%d",iz));
      gDirectory->cd(Form("Zreco%d",iz));
      h = new TH1D("ClusSize","Calo cluster size;Number of hits x cluster;Number of clusters",21, -0.5, 20.5);
      h = new TH1D("ClusCharge","Calo cluster total charge;Cluster charge [MeV];Number of clusters",101, -0.5, 100.5);
      gDirectory->cd("..");
    }
    for(int iz=0; iz<=primary_cha; iz++){
      gDirectory->mkdir(Form("Zmc%d",iz));
      gDirectory->cd(Form("Zmc%d",iz));
      h = new TH1D("partNumXCluster","Number of MC particles exit from tw and arriving in calo x cluster",11, -0.5, 10.5);
      h = new TH1D("partChaXCluster","Charge of MC particle exit from tw and arriving in calo x cluster",21, -0.5, 20.5);
      h2 = new TH2D("EkinVsCluscharge","MC particle ekin vs calo cluster total charge;Kinetic energy per nucleon [MeV/n];Cluster total charge [MeV/n]",200, 0., 1000.,200,0.,1000.);
      h2 = new TH2D("ClusSizeVsTheta","Cluster size vs MC crossing in calo theta for each MC particle;Cluster size;Theta [deg]",21, -0.5, 20.5,180,0.,30.);
      gDirectory->cd("..");
    }

    gDirectory->mkdir("GlbTrk");
    gDirectory->cd("GlbTrk");
    h = new TH1D("ClusNum_glbtrk","Number of calo clusters with a glb track per event;Number of cluster;Events",51, -0.5, 50.5);
    h = new TH1D("ClusEfficiency","Number of glb tracks with a Calo cluster over total number of glb tracks;Calo efficiency;Events",100, 0., 1.);
    h = new TH1D("ClusSize_glbtrk","Calo glb tracks cluster size;Number of hits x cluster;Number of clusters",21, -0.5, 20.5);
    gDirectory->cd("..");

    h = new TH1D("MultiPartDistance","Distance btw the particles that enter in the calo in the same event from MC;Distance [cm];Number of particles",600, 0., 200.);
    h = new TH1D("clus2HitDist","Distance btw the calo hits when a cluster have 2 hits;Distance [cm];Number of particles",250, 0., 5.);
    h2 = new TH2D("MCpartNumvscluster","Number of MC particle exit from tw entering in calo vs number of calo clusters per event; Number of MC particles; Number of Calo clusters",11, -0.5, 10.5,11, -0.5, 10.5);
    h2 = new TH2D("MCpartChavsClusSize","MC Charge vs calo cluster size; Charge; Calo cluster size [Number of crystals]",11, -0.5, 10.5,21, -0.5, 20.5);
    h = new TH1D("MCpartNumXHit","Number of MC particle x hit",11, -0.5, 10.5);
    h = new TH1D("ClusNum","Number of calo clusters per event;Number of cluster;Events",51, -0.5, 50.5);
    h = new TH1D("ClusSize","Calo cluster size;Number of hits x cluster;Number of clusters",21, -0.5, 20.5);
    h = new TH1D("ClusCharge","Calo cluster total charge;Cluster charge [MeV];Number of clusters",101, -0.5, 100.5);
    h = new TH1D("distminMag","Distance between track projection and CALO cluster position",100, -1., 10.);
    h = new TH1D("DistX_trkclu","Distance X trk int w calo AND calo clus",200, -20., 20.);
    h = new TH1D("DistY_trkclu","Distance Y trk int w calo AND calo clus",200, -20., 20.);
    h = new TH1D("DistZ_trkclu","Distance Z trk int w calo AND calo clus",200, -20., 20.);
  gDirectory->cd("..");

  gDirectory->mkdir("FIT_vs_Meas");
  gDirectory->cd("FIT_vs_Meas");
  h = new TH1D("Ekin","Fitted Ekin - Measured Ekin;[MeV]",600, -100., 100.);
  h = new TH1D("Tof","Fitted Tof - Measured Tof [ns]",600, -100., 100.);
  h = new TH1D("Mass","Fitted Mass - Measured Mass [MeV]",600, -100., 100.);
  gDirectory->cd("..");

  //ClusPosition studies
  gDirectory->mkdir("clusPosition");
  gDirectory->cd("clusPosition");
    gDirectory->mkdir("BM");
    gDirectory->cd("BM");
    h = new TH1D("bm_n_tracks","Number of BM tracks per event; Number of bm tracks; Events",21,-0.5,20.5);
      gDirectory->mkdir("all-events");
      gDirectory->cd("all-events");
      h2 = new TH2D("bm_target_bmsys","BM tracks on target  projections in BM sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("bm_target_glbsys","BM tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      //h2 = new TH2D("bm_target_bmsys_2","BM tracks on target projections in BM sys (other method);X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      //h2 = new TH2D("bm_target_glbsys_2","BM tracks on target projections in GLB sys (other method);X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h = new TH1D("bm_target_Xpos_glbsys","BM tracks X pos on target projections in GLB sys (other method);X[cm]",600,-3.,3.);
      h = new TH1D("bm_target_Ypos_glbsys","BM tracks Y pos on target projections in GLB sys (other method);Y[cm]",600,-3.,3.);
      h = new TH1D("bm_target_bmsys_theta","theta of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
      h = new TH1D("bm_target_glbsys_theta","theta of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
      h = new TH1D("bm_target_bmsys_phi","phi of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
      h = new TH1D("bm_target_glbsys_phi","phi of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
        gDirectory->mkdir("spill");
        gDirectory->cd("spill");
        h2  = new TH2D("bm_posX_vsEvent","bm track X position in target vs events ; event; X position [cm]",65000, 0 ,64999., 100, -2. ,2.);
        h2  = new TH2D("bm_posY_vsEvent","bm track Y position in target vs events ; event; Y position [cm]",65000, 0 ,64999., 100, -2. ,2.);
        gDirectory->cd("..");
      gDirectory->cd("..");

      gDirectory->mkdir("reco-events");
      gDirectory->cd("reco-events");
      h2 = new TH2D("bm_target_bmsys","BM tracks on target  projections in BM sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("bm_target_glbsys","BM tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      //h2 = new TH2D("bm_target_bmsys_2","BM tracks on target projections in BM sys (other method);X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      //h2 = new TH2D("bm_target_glbsys_2","BM tracks on target projections in GLB sys (other method);X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h = new TH1D("bm_target_Xpos_glbsys","BM tracks X pos on target projections in GLB sys (other method);X[cm]",600,-3.,3.);
      h = new TH1D("bm_target_Ypos_glbsys","BM tracks Y pos on target projections in GLB sys (other method);Y[cm]",600,-3.,3.);
      h = new TH1D("bm_target_bmsys_theta","theta of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
      h = new TH1D("bm_target_glbsys_theta","theta of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
      h = new TH1D("bm_target_bmsys_phi","phi of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
      h = new TH1D("bm_target_glbsys_phi","phi of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
        gDirectory->mkdir("spill");
        gDirectory->cd("spill");
        h2  = new TH2D("bm_posX_vsEvent","bm track X position in target vs events ; event; X position [cm]",65000, 0 ,64999., 100, -2. ,2.);
        h2  = new TH2D("bm_posY_vsEvent","bm track Y position in target vs events ; event; Y position [cm]",65000, 0 ,64999., 100, -2. ,2.);
        gDirectory->cd("..");
      gDirectory->cd("..");

      gDirectory->mkdir("no-reco-events");
      gDirectory->cd("no-reco-events");
      h2 = new TH2D("bm_target_bmsys","BM tracks on target  projections in BM sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("bm_target_glbsys","BM tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      //h2 = new TH2D("bm_target_bmsys_2","BM tracks on target projections in BM sys (other method);X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      //h2 = new TH2D("bm_target_glbsys_2","BM tracks on target projections in GLB sys (other method);X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h = new TH1D("bm_target_Xpos_glbsys","BM tracks X pos on target projections in GLB sys (other method);X[cm]",600,-3.,3.);
      h = new TH1D("bm_target_Ypos_glbsys","BM tracks Y pos on target projections in GLB sys (other method);Y[cm]",600,-3.,3.);
      h = new TH1D("bm_target_bmsys_theta","theta of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
      h = new TH1D("bm_target_glbsys_theta","theta of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
      h = new TH1D("bm_target_bmsys_phi","phi of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
      h = new TH1D("bm_target_glbsys_phi","phi of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
        gDirectory->mkdir("spill");
        gDirectory->cd("spill");
        h2  = new TH2D("bm_posX_vsEvent","bm track X position in target vs events ; event; X position [cm]",65000, 0 ,64999., 100, -2. ,2.);
        h2  = new TH2D("bm_posY_vsEvent","bm track Y position in target vs events ; event; Y position [cm]",65000, 0 ,64999., 100, -2. ,2.);
        gDirectory->cd("..");
      gDirectory->cd("..");
    gDirectory->cd("..");//BM folder in ClustersPositionStudy


  gDirectory->mkdir("VT");
  gDirectory->cd("VT");
    gDirectory->mkdir("vtxpoints");
    gDirectory->cd("vtxpoints");
    h = new TH1D("vtx_num","Number of evtx per event; Number of evtx; Events",21,-0.5,20.5);
    h2 = new TH2D("vtx_vtx_profile_vtxsys","VTX profile on target in VTX system;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    h2 = new TH2D("vtx_vtx_profile_glbsys","VTX profile on target in global system ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    h = new TH1D("vtx_vtx_trknum","Number of vtx tracks associated per vertex;Number of tracks;Events",11,-0.5,10.5);
    h = new TH1D("vtx_vtx_BMmatched","Is matched with the BM flag;0=not matched 1=matched;Events",2,-0.5,1.5);
    h = new TH1D("ev_no_vtxmatched", "n of events with any vertx matched with BM",1,-0.5,1.5);
    gDirectory->cd("..");

    gDirectory->mkdir("vtxmatched");
    gDirectory->cd("vtxmatched");
      gDirectory->mkdir("all_events");
      gDirectory->cd("all_events");
      h = new TH1D("vtx_n_cluster","n of clusters in every track; n of cluster; counts",10, 0 ,10.);
      h = new TH1D("vtx_trk_num","Number of vtx tracks for the BM matched vertex per event;Number of vtx tracks;Events",21,-0.5,20.5);
      h2 = new TH2D("clust0_position_vtxsys","vtx cluster0 position in vtx sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("clust0_position_glbsys","vtx cluster0 position in glb sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("vtx_tw_proj_glbsys","vtx tracks on tw  projections in global sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
      h = new TH1D("vtx_tw_proj_Xpos_proj_glbsys","vtx tracks on X tw  projections in global sys ;X[cm]",100,-30.5,30.5 );
      h = new TH1D("vtx_tw_proj_Ypos_proj_glbsys","vtx tracks on Y tw  projections in global sys ;Y[cm]",100,-30.5,30.5);
      h2 = new TH2D("vtx_tw_proj_twsys","vtx tracks on tw  projections in tw sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
      h = new TH1D("vtx_theta_glbsys","theta of tracks in global system; deg; counts",100, 0 ,50.);
      h = new TH1D("vtx_phi_glbsys","phi of tracks in global system; deg; counts",100, 0 ,50.);
      h = new TH1D("vtx_theta_vtxsys","theta of tracks in vtx system; deg; counts",100, 0 ,50.);
      h = new TH1D("vtx_phi_vtxsys","phi of tracks in vtx system; deg; counts",100, 0 ,50.);
      h2 = new TH2D("vtx_tg_proj_glbsys","vtx tracks on target  projections in global sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
      h = new TH1D("vtx_tg_proj_Xpos_proj_glbsys","vtx tracks on X target  projections in global sys ;X[cm]",100,-30.5,30.5 );
      h = new TH1D("vtx_tg_proj_Ypos_proj_glbsys","vtx tracks on Y target  projections in global sys ;Y[cm]",100,-30.5,30.5);
      h2 = new TH2D("vtx_theta_vs_globtrack_theta","theta of vtx tracklet vs theta of global track; vtx angle [deg]; glb track angle [deg]",100, 0 ,10.,100, 0 ,10.);
      h2 = new TH2D("n_tracklets_vs_n_globaltracks","n of tracklets of vtx vs n of global reco tracks per event; vtx tracklets ; glb tracks [deg]",10, -0.5 ,9.5,10, -0.5 ,9.5);
        gDirectory->mkdir("gltrackcandidate");
        gDirectory->cd("gltrackcandidate");
        h2 = new TH2D("vtx_tw_proj_glbsys_gltrackcandidate","vtx tracks on tw  projections in global sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
        h = new TH1D("vtx_tw_proj_Xpos_glbsys_gltrackcandidate","vtx tracks on X tw  projections in global sys ;X[cm]",100,-30.5,30.5 );
        h = new TH1D("vtx_tw_proj_Ypos_glbsys_gltrackcandidate","vtx tracks on Y tw  projections in global sys ;Y[cm]",100,-30.5,30.5);
        h = new TH1D("vtx_theta_glbsys_gltrackcandidate","theta of tracks in global system; deg; counts",100, 0 ,50.);
        h = new TH1D("vtx_phi_glbsys_gltrackcandidate","phi of tracks in global system; deg; counts",100, 0 ,50.);
        h = new TH1D("vtx_theta_vtxsys_gltrackcandidate","theta of tracks in vtx system; deg; counts",100, 0 ,50.);
        h = new TH1D("vtx_phi_vtxsys_gltrackcandidate","phi of tracks in vtx system; deg; counts",100, 0 ,50.);
        gDirectory->cd("..");

      gDirectory->mkdir("spill");
      gDirectory->cd("spill");
      h2  = new TH2D("vtx_posX_vsEvent","vtx track X position in target vs events ; event; X position [cm]",65000, 0 ,64999., 100, -2. ,2.);
      h2  = new TH2D("vtx_posY_vsEvent","vtx track Y position in target vs events ; event; Y position [cm]",65000, 0 ,64999., 100, -2. ,2.);
      gDirectory->cd("..");
    gDirectory->cd("..");

      gDirectory->mkdir("reco-events");
      gDirectory->cd("reco-events");
      h = new TH1D("vtx_n_cluster","n of clusters in every track; n of cluster; counts",10, 0 ,10.);
      h = new TH1D("vtx_trk_num","Number of vtx tracks for the BM matched vertex per event;Number of vtx tracks;Events",21,-0.5,20.5);
      h2 = new TH2D("clust0_position_vtxsys","vtx cluster0 position in vtx sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("clust0_position_glbsys","vtx cluster0 position in glb sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("vtx_tw_proj_glbsys","vtx tracks on tw  projections in global sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
      h = new TH1D("vtx_tw_proj_Xpos_proj_glbsys","vtx tracks on X tw  projections in global sys ;X[cm]",100,-30.5,30.5 );
      h = new TH1D("vtx_tw_proj_Ypos_proj_glbsys","vtx tracks on Y tw  projections in global sys ;Y[cm]",100,-30.5,30.5);
      h2 = new TH2D("vtx_tw_proj_twsys","vtx tracks on tw  projections in tw sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
      h = new TH1D("vtx_theta_glbsys","theta of tracks in global system; deg; counts",100, 0 ,50.);
      h = new TH1D("vtx_phi_glbsys","phi of tracks in global system; deg; counts",100, 0 ,50.);
      h = new TH1D("vtx_theta_vtxsys","theta of tracks in vtx system; deg; counts",100, 0 ,50.);
      h = new TH1D("vtx_phi_vtxsys","phi of tracks in vtx system; deg; counts",100, 0 ,50.);
      h2 = new TH2D("vtx_tg_proj_glbsys","vtx tracks on target  projections in global sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
      h = new TH1D("vtx_tg_proj_Xpos_proj_glbsys","vtx tracks on X target  projections in global sys ;X[cm]",100,-30.5,30.5 );
      h = new TH1D("vtx_tg_proj_Ypos_proj_glbsys","vtx tracks on Y target  projections in global sys ;Y[cm]",100,-30.5,30.5);
      h2 = new TH2D("vtx_theta_vs_globtrack_theta","theta of vtx tracklet vs theta of global track; vtx angle [deg]; glb track angle [deg]",100, 0 ,10.,100, 0 ,10.);
      h2 = new TH2D("n_tracklets_vs_n_globaltracks","n of tracklets of vtx vs n of global reco tracks per event; vtx tracklets ; glb tracks [deg]",10, -0.5 ,9.5,10, -0.5 ,9.5);
        gDirectory->mkdir("gltrackcandidate");
        gDirectory->cd("gltrackcandidate");
        h2 = new TH2D("vtx_tw_proj_glbsys_gltrackcandidate","vtx tracks on tw  projections in global sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
        h = new TH1D("vtx_tw_proj_Xpos_glbsys_gltrackcandidate","vtx tracks on X tw  projections in global sys ;X[cm]",100,-30.5,30.5 );
        h = new TH1D("vtx_tw_proj_Ypos_glbsys_gltrackcandidate","vtx tracks on Y tw  projections in global sys ;Y[cm]",100,-30.5,30.5);
        h = new TH1D("vtx_theta_glbsys_gltrackcandidate","theta of tracks in global system; deg; counts",100, 0 ,50.);
        h = new TH1D("vtx_phi_glbsys_gltrackcandidate","phi of tracks in global system; deg; counts",100, 0 ,50.);
        h = new TH1D("vtx_theta_vtxsys_gltrackcandidate","theta of tracks in vtx system; deg; counts",100, 0 ,50.);
        h = new TH1D("vtx_phi_vtxsys_gltrackcandidate","phi of tracks in vtx system; deg; counts",100, 0 ,50.);
        gDirectory->cd("..");
        gDirectory->mkdir("spill");
        gDirectory->cd("spill");
        h2  = new TH2D("vtx_posX_vsEvent","vtx track X position in target vs events ; event; X position [cm]",65000, 0 ,64999., 100, -10. ,10.);
        h2  = new TH2D("vtx_posY_vsEvent","vtx track Y position in target vs events ; event; Y position [cm]",65000, 0 ,64999., 100, -10. ,10.);
        h2  = new TH2D("BM_matched_posX_vsEvent","bm matched track X position in target vs events ; event; X position [cm]",65000, 0 ,64999., 100, -10. ,10.);
        gDirectory->cd("..");
      gDirectory->cd("..");
      gDirectory->mkdir("no-reco-events");
      gDirectory->cd("no-reco-events");
      h = new TH1D("vtx_n_cluster","n of clusters in every track; n of cluster; counts",10, 0 ,10.);
      h = new TH1D("vtx_trk_num","Number of vtx tracks for the BM matched vertex per event;Number of vtx tracks;Events",21,-0.5,20.5);
      h2 = new TH2D("clust0_position_vtxsys","vtx cluster0 position in vtx sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("clust0_position_glbsys","vtx cluster0 position in glb sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("vtx_tw_proj_glbsys","vtx tracks on tw  projections in global sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
      h = new TH1D("vtx_tw_proj_Xpos_proj_glbsys","vtx tracks on X tw  projections in global sys ;X[cm]",100,-30.5,30.5 );
      h = new TH1D("vtx_tw_proj_Ypos_proj_glbsys","vtx tracks on Y tw  projections in global sys ;Y[cm]",100,-30.5,30.5);
      h2 = new TH2D("vtx_tw_proj_twsys","vtx tracks on tw  projections in tw sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
      h = new TH1D("vtx_theta_glbsys","theta of tracks in global system; deg; counts",100, 0 ,50.);
      h = new TH1D("vtx_phi_glbsys","phi of tracks in global system; deg; counts",100, 0 ,50.);
      h = new TH1D("vtx_theta_vtxsys","theta of tracks in vtx system; deg; counts",100, 0 ,50.);
      h = new TH1D("vtx_phi_vtxsys","phi of tracks in vtx system; deg; counts",100, 0 ,50.);
      h2 = new TH2D("vtx_tg_proj_glbsys","vtx tracks on target  projections in global sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
      h = new TH1D("vtx_tg_proj_Xpos_proj_glbsys","vtx tracks on X target  projections in global sys ;X[cm]",100,-30.5,30.5 );
      h = new TH1D("vtx_tg_proj_Ypos_proj_glbsys","vtx tracks on Y target  projections in global sys ;Y[cm]",100,-30.5,30.5);
      h2 = new TH2D("vtx_theta_vs_globtrack_theta","theta of vtx tracklet vs theta of global track; vtx angle [deg]; glb track angle [deg]",100, 0 ,10.,100, 0 ,10.);
      h2 = new TH2D("n_tracklets_vs_n_globaltracks","n of tracklets of vtx vs n of global reco tracks per event; vtx tracklets ; glb tracks [deg]",10, -0.5 ,9.5,10, -0.5 ,9.5);
        gDirectory->mkdir("gltrackcandidate");
        gDirectory->cd("gltrackcandidate");
        h2 = new TH2D("vtx_tw_proj_glbsys_gltrackcandidate","vtx tracks on tw  projections in global sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
        h = new TH1D("vtx_tw_proj_Xpos_glbsys_gltrackcandidate","vtx tracks on X tw  projections in global sys ;X[cm]",100,-30.5,30.5 );
        h = new TH1D("vtx_tw_proj_Ypos_glbsys_gltrackcandidate","vtx tracks on Y tw  projections in global sys ;Y[cm]",100,-30.5,30.5);
        h = new TH1D("vtx_theta_glbsys_gltrackcandidate","theta of tracks in global system; deg; counts",100, 0 ,50.);
        h = new TH1D("vtx_phi_glbsys_gltrackcandidate","phi of tracks in global system; deg; counts",100, 0 ,50.);
        h = new TH1D("vtx_theta_vtxsys_gltrackcandidate","theta of tracks in vtx system; deg; counts",100, 0 ,50.);
        h = new TH1D("vtx_phi_vtxsys_gltrackcandidate","phi of tracks in vtx system; deg; counts",100, 0 ,50.);
        gDirectory->cd("..");
        gDirectory->mkdir("spill");
        gDirectory->cd("spill");
        h2  = new TH2D("vtx_posX_vsEvent","vtx track X position in target vs events ; event; X position [cm]",65000, 0 ,64999., 100, -10. ,10.);
        h2  = new TH2D("vtx_posY_vsEvent","vtx track Y position in target vs events ; event; Y position [cm]",65000, 0 ,64999., 100, -10. ,10.);
        h2  = new TH2D("BM_matched_posX_vsEvent","bm matched track X position in target vs events ; event; X position [cm]",65000, 0 ,64999., 100, -10. ,10.);
        gDirectory->cd("..");
      gDirectory->cd("..");
    gDirectory->cd("..");
    h = new TH1D("vtx_target_vtsys_theta","theta of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
    h = new TH1D("vtx_target_glbsys_theta","theta of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
    h = new TH1D("vtx_target_glbsys_theta_gltrackcandidate","theta of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
    h = new TH1D("vtx_target_vtsys_phi","phi of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
    h = new TH1D("vtx_target_glbsys_phi","phi of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
    h2 = new TH2D("vtx_target_vtsys","vtx tracks on target  projections in vt sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    h2 = new TH2D("vtx_tw_glbsys","vtx tracks on tw  projections in global sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
    h = new TH1D("vtx_Xpos_tw_glbsys","vtx tracks on X tw  projections in global sys ;X[cm]",100,-30.5,30.5 );
    h = new TH1D("vtx_Ypos_tw_glbsys","vtx tracks on Y tw  projections in global sys ;Y[cm]",100,-30.5,30.5);
    h2 = new TH2D("vtx_tw_glbsys_gltrackcandidate","vtx tracks on tw  projections in global sys (vt track projection inside tw);X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
    h = new TH1D("vtx_Xpos_tw_glbsys_gltrackcandidate","vtx tracks on X tw  projections in global sys (vt track projection inside tw);X[cm]",100,-30.5,30.5 );
    h = new TH1D("vtx_Ypos_tw_glbsys_gltrackcandidate","vtx tracks on Y tw  projections in global sys (vt track projection inside tw);Y[cm]",100,-30.5,30.5);
    h2 = new TH2D("vtx_tw_twsys","vtx tracks on tw  projections in tw sys ;X[cm];Y[cm]",100,-30.5,30.5, 100,-30.5,30.5);
    h2 = new TH2D("vtx_target_glbsys","vtx tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    h = new TH1D("vtx_Ypos_target_glbsys","vtx tracks Ypos on target projections in GLB sys;Y[cm]",600,-3.,3.);
    h = new TH1D("vtx_Xpos_target_glbsys","vtx tracks Xpos on target projections in GLB sys;X[cm]",600,-3.,3.);
    h = new TH1D("vtx_n_cluster","n of clusters in every track; n of cluster; counts",10, 0 ,10.);
    h = new TH1D("vtx_n_tracks","n of tracks in every event; n of tracks; counts",100, 0 ,100.);
    h2 = new TH2D("vtx_theta_vs_globtrack_theta","theta of vtx tracklet vs theta of global track; vtx angle [deg]; glb track angle [deg]",100, 0 ,10.,100, 0 ,10.);
  gDirectory->cd("..");

    gDirectory->mkdir("TW");
    gDirectory->cd("TW");
    h = new TH1D("tw_points_num","Number of tw points per event;Number of points;Events",21,-0.5,20.5);
    h = new TH1D("tw_GetChargeZ","TW points reco charge;Z reco;Events",11,-0.5,10.5);
    h = new TH1D("tw_Xpos_twsys","TW points X position in tw sys;X [cm];Events",61,-30.5,30.5);
    h = new TH1D("tw_Ypos_twsys","TW points Y position in tw sys;Y [cm];Events",61,-30.5,30.5);
    h2 = new TH2D("tw_XYpos_twsys","TW points XY position in tw sys;X[cm];Y[cm]",61,-30.5,30.5,61,-30.5,30.5);
    h = new TH1D("tw_Xpos_glbsys","TW points X position in glb sys;X [cm];Events",61,-30.5,30.5);
    h = new TH1D("tw_Ypos_glbsys","TW points Y position in glb sys;Y [cm];Events",61,-30.5,30.5);
    h2 = new TH2D("tw_XYpos_glbsysAll","TW points XY position in glb sys;X[cm];Y[cm]",61,-30.5,30.5,61,-30.5,30.5);
    h2 = new TH2D("tw_barmultAll","TW bar map all;FrontBar X;RearBar Y",20,-0.5,19.5,20,0.,19.5);
    //h2 = new TH2D("tw_XYpos_glbsysMag","TW points XY position in glb sys for minimum bias;X[cm];Y[cm]",61,-30.5,30.5,61,-30.5,30.5);
    gDirectory->cd("..");

    gDirectory->mkdir("VTXSYNC");
    gDirectory->cd("VTXSYNC");
    h2 = new TH2D("origin_xx_bmvtx_all","BM originX vs VTX originX for all the evts;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
    gDirectory->cd("..");
  gDirectory->cd("..");


  //theta Mig Matrix

    gDirectory->mkdir("theta_MigMat");
    gDirectory->cd("theta_MigMat");
    for(int iz=1; iz<=primary_cha; iz++){
    string name = "Z_" + to_string(iz-1) +"#"+to_string(iz-0.5)+"_"+to_string(iz+0.5);
    gDirectory->mkdir(name.c_str());
    gDirectory->cd(name.c_str());
    string name_h2 = "migMatrix_theta_Z"+to_string(iz);
    h2 = new TH2D(name_h2.c_str(), "Theta Migration Matrix; theta_true; theta_reco",10,0.,10.,10,0.,10.);
    gDirectory->cd("..");
    }
    gDirectory->cd("..");









  if(FootDebugLevel(1))
    cout<<"GlboalRecoAna::Booking done"<<endl;

  return;
}


int GlobalRecoAna::ApplyEvtCuts(){
  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::ApplyEvtCuts start"<<endl;

  if (myBMNtuTrk->GetTracksN() !=1)
    return 1;

  return 0;
}

int GlobalRecoAna::ApplyTrkCuts(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::ApplyTrkCuts start"<<endl;

  if(isnan(fGlbTrack->GetCalEnergy())){//check if tof + track arc_length + mass hyp are ok      //! GetCalEnergy() is 0 up to now!
    cout<<"TRK energy ISNAN! -> TOF value = "<<fGlbTrack->GetTwTof()<<endl;
    return 1;
  }
  return 0;
}


void GlobalRecoAna::MCGlbTrkLoopSetVariables(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::MCGlbTrkLoopSetVariables start"<<endl;

  Tof_true = -1.;
  Tof_startmc = -1.;
  Beta_true = -1.;
  P_cross.SetXYZ(-999.,-999.,-999.);//also MS contribution in target!

  TrkIdMC = fGlbTrack->GetMcMainTrackId();   //associo l'IdMC alla particella più frequente della traccia  (prima era ottenuto tramite studio purity)
  if(TrkIdMC !=-1) { //TODO: what happens when TrkIdMC is -1?????
    TAMCpart *pNtuMcTrk = GetNtuMcTrk()->GetTrack(TrkIdMC);

    Z_true = pNtuMcTrk->GetCharge();
    P_true = pNtuMcTrk->GetInitP();//also MS contribution in target!
    M_true = pNtuMcTrk->GetMass();
    Ek_true = (sqrt(pow(pNtuMcTrk->GetMass(),2) + pow((pNtuMcTrk->GetInitP()).Mag(),2)) - pNtuMcTrk->GetMass())/M_true;

    //study of crossing regions
    if(TAGrecoManager::GetPar()->IsRegionMc()){
      for(int icr = 0;icr<GetNtuMcReg()->GetRegionsN();icr++){
        TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);
        if((fpNtuMcReg->GetTrackIdx()-1)==TrkIdMC){
          //particle exit from target
          if(fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirPreTW() && fpNtuMcReg->GetOldCrossN()==GetParGeoG()->GetRegTarget()){
            P_cross = fpNtuMcReg->GetMomentum();
            Tof_startmc = fpNtuMcReg->GetTime()*fpFootGeo->SecToNs();
          }
          //particle crossing in the first TW layer
          if(fpNtuMcReg->GetCrossN()<=GetParGeoTw()->GetRegStrip(1,GetParGeoTw()->GetNBars()-1) && fpNtuMcReg->GetCrossN()>=GetParGeoTw()->GetRegStrip(1,0)){
            Tof_true = fpNtuMcReg->GetTime()*fpFootGeo->SecToNs();
            Beta_true=fpNtuMcReg->GetMomentum().Mag()/sqrt(fpNtuMcReg->GetMass()*fpNtuMcReg->GetMass()+fpNtuMcReg->GetMomentum().Mag()*fpNtuMcReg->GetMomentum().Mag());
          }
          //particle crossing in the calo
          if(fpNtuMcReg->GetCrossN()>=GetParGeoCa()->GetRegCrystal(0) && fpNtuMcReg->GetCrossN()<=GetParGeoCa()->GetRegCrystal(GetParGeoCa()->GetCrystalsN()-1)){
            // if(fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirTW() && fpNtuMcReg->GetOldCrossN()<=GetParGeoTw()->GetRegStrip(0,GetParGeoTw()->GetNBars()-1) && fpNtuMcReg->GetOldCrossN()>=GetParGeoTw()->GetRegStrip(0,0)){//
             Double_t M_cross_calo = fpNtuMcReg->GetMass();
             TVector3 P_cross_calo = fpNtuMcReg->GetMomentum();
             Ek_cross_calo = sqrt(P_cross_calo*P_cross_calo + M_cross_calo*M_cross_calo) - M_cross_calo;
             Ek_cross_calo = Ek_cross_calo/M_cross_calo;
            // }
          }
        }
      }
      if(Tof_startmc>=0)
        Tof_true-=Tof_startmc;
      //TODO: we need to define what happens when a particle isn't created in the target!
      if(FootDebugLevel(1)){
        cout<<"Z_true="<<Z_true<<"  M_true="<<M_true<<"  P_true.Mag()="<<P_true.Mag()<<"  Ek_true="<<Ek_true<<endl;
        if(TAGrecoManager::GetPar()->IsRegionMc()){
          cout<<"Target exit crossings:"<<endl<<"  P_cross.Mag()="<<P_cross.Mag()<<"  Tof_startmc="<<Tof_startmc<<endl;
          cout<<"TOF mesurements: particle total time of flight="<<Tof_true<<"  Beta_true="<<Beta_true<<"  primary_tof=Tof_startmc="<<Tof_startmc<<"  particle real mc tof="<<Tof_true-Tof_startmc<<endl;
        }
      }

      Th_true = P_true.Theta()*180./TMath::Pi();
      //cout << "theta true: "<< Th_true <<endl;
      Th_cross = P_cross.Theta()*TMath::RadToDeg();
    }
  }

  return;
}

void GlobalRecoAna::RecoGlbTrkLoopSetVariables(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::RecoGlbTrkLoopSetVariables start"<<endl;

  Th_reco = fGlbTrack-> GetTgtTheta() *TMath::RadToDeg();
  Z_meas = fGlbTrack->GetTwChargeZ();
  P_meas = fGlbTrack->GetMomentum();  // Wrong method of TOE!! but it does not crash
  Tof_meas = fGlbTrack->GetTwTof()-primary_tof;
  Ek_meas = fGlbTrack->GetCalEnergy()*fpFootGeo->MevToGev()/M_meas;//Energy (GeV/u)
  Beta_meas=fGlbTrack->GetLength()/Tof_meas/TAGgeoTrafo::GetLightVelocity();
  M_meas = fGlbTrack->GetMass(); //It's the track mass hp, cannot be used as mass measurement

  //take the vector direction of the fragment in global SdR wrt target
   TVector3 TgtMomentum = fGlbTrack->GetTgtMomentum().Unit();
   // take the direction of the beam in global SdR
   TVector3 BMslope = fpFootGeo->VecFromBMLocalToGlobal(myBMNtuTrk->GetTrack(0)->GetSlope());
   //take the angle between these 2 vectors
   //Th_recoBM = BMslope.Angle( TgtMomentum )*TMath::RadToDeg();
  Th_recoBM =  fGlbTrack->GetTgtThetaBm() *TMath::RadToDeg();
   if(FootDebugLevel(1))
     cout << "momX: " << TgtMomentum.X() << " momY: " << TgtMomentum.Y() << " momZ: " << TgtMomentum.Z() << endl << "BMX: " << BMslope.X() << " BMY: " << BMslope.Y() << " BMZ: " << BMslope.Z() << endl << "TH_mom: " << TgtMomentum.Theta() *TMath::RadToDeg()  <<  " thBM: "<< BMslope.Theta() *TMath::RadToDeg() << " th_recoBM: "<<Th_recoBM <<endl << endl;


  return;
}

void GlobalRecoAna::EvaluateMass(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::EvaluateMass start"<<endl;

  mass_ana->NewMass(P_meas, fGlbTrack->GetLength(), Tof_meas,Ek_meas*M_meas, 0.07, P_meas*0.05, Ek_meas*M_meas*0.015);// sigma values should be defined in a better way.
  M_meas=mass_ana->GetMassWavg()/TAGgeoTrafo::AmuToGev();
  if(mass_ana->GetInputStatus()==30){
    mass_ana->FitChiMass();
    mass_ana->FitAlmMass();
    mass_ana->CombineChi2Alm();
    M_meas=mass_ana->GetMassBest()/TAGgeoTrafo::AmuToGev();
  }
  FillMassPlots();
}

void GlobalRecoAna::EkBinningStudies(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::EkBinningStudies start"<<endl;

  double Dtwpos = 0.01;//m
  double vlight = fpFootGeo->GetLightVelocity()/100.;//m/ns
  double tof_res = 0.6;//ns
  double gamma = 1./sqrt(1. - Beta_meas*Beta_meas);
  double Dbeta=sqrt((Dtwpos*Dtwpos)/(Tof_meas*vlight*Tof_meas*vlight)+fGlbTrack->GetTwPosition().Z()*fpFootGeo->CmToM()*fGlbTrack->GetTwPosition().Z()*fpFootGeo->CmToM()*(pow(tof_res*vlight,2)/(pow(Tof_meas*vlight,4)))); // delta beta
  double Dg=Beta_meas*pow((1-Beta_meas*Beta_meas),-3./2.)*Dbeta; //delta gamma
  double DE=M_meas*Dg; //delta mass
  //----------

  ((TH2D*)gDirectory->Get("Z_TWvsZ_fit"))->Fill(fGlbTrack->GetTwChargeZ(),fGlbTrack->GetFitChargeZ());
  if(Z_meas<=primary_cha && Z_meas>0){
    ((TH2D*)gDirectory->Get(Form("Ekin/Z%d/DE_vs_Ekin",Z_meas)))->Fill(Ek_meas*fpFootGeo->GevToMev(),DE*fpFootGeo->GevToMev());
    ((TH1D*)gDirectory->Get(Form("Ekin/Z%d/Ek_meas",Z_meas)))->Fill(Ek_meas*fpFootGeo->GevToMev());
  }
  //migration matrix plot
  if(fFlagMC)
    ((TH2D*)gDirectory->Get("Z_truevsZ_reco"))->Fill(Z_true,Z_meas);

  return;
}

void GlobalRecoAna::FillUnfoldingPlots(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::FillUnfoldingPlots start"<<endl;

  for (int i = 0; i<th_nbin; i++){
    if(Th_reco>theta_binning[i][0] && Th_reco<theta_binning[i][1])
      theta_bin_meas=i+1;
    if(Th_true>theta_binning[i][0] && Th_true<theta_binning[i][1])
      theta_bin_true=i+1;
  }
  for (int j=0; j<ek_nbin; j++) {
    if((Ek_meas*fpFootGeo->GevToMev())>=ek_binning[j][0] && (Ek_meas*fpFootGeo->GevToMev())<ek_binning[j][1])
      Ek_bin_meas=j+1;
    if((Ek_true*fpFootGeo->GevToMev())>=ek_binning[j][0] && (Ek_true*fpFootGeo->GevToMev())<ek_binning[j][1])
      Ek_bin_true=j+1;
  }

  Int_t tmp_meas=((Z_meas-1)*(th_nbin)*(ek_nbin))+((Ek_bin_meas-1)*theta_bin_meas)+theta_bin_meas;
  Int_t tmp_true=((Z_true-1)*(th_nbin)*(ek_nbin))+((Ek_bin_true-1)*theta_bin_true)+theta_bin_true;
  ((TH2D*)gDirectory->Get(Form("Unfolding/Unfolding_trk_vs_true")))->Fill(tmp_meas, tmp_true);
  ((TH1D*)gDirectory->Get(Form("Unfolding/RecoDistribution")))->Fill(tmp_meas);

return;
}

void GlobalRecoAna::FillMCGlbTrkYields(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::FillMCGlbTrkYields start"<<endl;

  //-------------------------------------------------------------
  //--Yield for CROSS SECTION fragmentation- RECO PARAMETERS FROM MC DATA
  if ( Z_true >0. && Z_true <= primary_cha /*&& TriggerCheckMC() == true*/)
    FillYieldReco("yield-trkMC",Z_true,Z_meas,Th_true );

  //-------------------------------------------------------------
  //--CROSS SECTION fragmentation- RECO PARAMETERS FROM MC DATA + ALLTW FIX : i don't want not fragmented primary

  // check del TAGpoint del TW
  for (int ic = 0; ic < fGlbTrack->GetPointsN(); ic++)
  { // from all the points of the track...
    TAGpoint *tmp_poi = fGlbTrack->GetPoint(ic);
    TString str = tmp_poi->GetDevName();
    if (str.Contains(TATWparGeo::GetBaseName()))
    { //...i just want the TAGPOINT of TW
      if (tmp_poi->GetMcTracksN() == 1)
      { // if there is no ghost hits in the TW
        if (tmp_poi->GetMcTrackIdx(0) == fGlbTrack->GetMcMainTrackId())
        { // if there is match of MCId
          if (Z_true > 0. && Z_true <= primary_cha /*&& TriggerCheckMC() == true*/)
          {
             FillYieldReco("yield-trkcutsMC", Z_true, Z_meas, Th_true);
             ((TH1D *)gDirectory->Get("ThReco_fragMC"))->Fill(Th_recoBM);
          }
        }
      }
    }
  }

  //-------------------------------------------------------------
  //--CROSS SECTION fragmentation- RECO PARAMETERS FROM MC DATA + GHOST HITS FIX : i don't want not fragmented primary
  if (N_TrkIdMC_TW == 1)
    if (Z_true >0. && Z_true <= primary_cha /*&& TriggerCheckMC() == true*/)
      FillYieldReco("yield-trkGHfixMC",Z_true,Z_meas,Th_true );

  //-------------------------------------------------------------
  //--CROSS SECTION fragmentation for trigger efficiency   (comparing triggercheck with TAWDntuTrigger )
  if (Z_meas >0. && Z_meas <= primary_cha /*&& TriggerCheck() == true*/)
    FillYieldReco("yield-trkReco",Z_meas,0,Th_recoBM );

  //-------------------------------------------------------------
  //--CROSS SECTION reco
  if (Z_meas >0. && Z_meas <= primary_cha /*&& TriggerCheck() == true*/){

    TString name = GetTitle();
    Int_t pos = name.Last('.');
    string name_ = "yield-trkREAL";
    name_ += name[pos-1];
    FillYieldReco(name_.c_str(),Z_meas,0,Th_recoBM );












  }



return;
}

void GlobalRecoAna::FillDataGlbTrkYields(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::FillDataGlbTrkYields start"<<endl;

  //-------------------------------------------------------------
  //--CROSS SECTION fragmentation- RECO PARAMETERS FROM REAL DATA : i don't want not fragmented primary
  if ( Z_meas >0. && Z_meas <= primary_cha /*&& wdTrig -> GetTriggersStatus()[1] == 1*/)     //fragmentation hardware trigger ON
  //&& TriggerCheck(fGlbTrack) == true  //NB.: for MC FAKE REAL
   {
    //cout << "inside " <<endl;
    FillYieldReco("yield-trkREAL",Z_meas,0,Th_recoBM );
    //cout << "thBM: "<< Th_recoBM <<endl;
    ((TH1D*)gDirectory->Get("ThReco_frag"))->Fill(Th_recoBM);
    ((TH1D*)gDirectory->Get("Charge_trk_frag"))->Fill(Z_meas);
  }
  return;
}

void GlobalRecoAna::FillTrkPlots(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::FillTrkPlots start"<<endl;

  ((TH1D*)gDirectory->Get("FIT_vs_Meas/Ekin"))->Fill(fGlbTrack->GetFitEnergy()-fGlbTrack->GetCalEnergy());
  ((TH1D*)gDirectory->Get("FIT_vs_Meas/Tof"))->Fill(fGlbTrack->GetFitTof()-(fGlbTrack->GetTwTof()));
  ((TH1D*)gDirectory->Get("FIT_vs_Meas/Mass"))->Fill(fGlbTrack->GetFitMass()-mass_ana->GetMassBest()*TAGgeoTrafo::AmuToGev());

  ((TH1D*)gDirectory->Get("Energy"))->Fill(Ek_meas*fpFootGeo->GevToMev());
  ((TH1D*)gDirectory->Get("Charge_trk"))->Fill(Z_meas);
  ((TH2D*)gDirectory->Get("Z_track_Mixing_matrix"))->Fill(Z_meas,Z_true);
  ((TH1D*)gDirectory->Get("Mass"))->Fill(M_meas);
  ((TH1D*)gDirectory->Get("Mass_True"))->Fill(M_true);
  ((TH1D*)gDirectory->Get("ThReco"))->Fill(Th_recoBM);
  ((TH1D*)gDirectory->Get("Tof_tw"))->Fill(fGlbTrack->GetTwTof());
  ((TH1D*)gDirectory->Get("Tof_meas"))->Fill(Tof_meas);
  ((TH1D*)gDirectory->Get("Beta_meas"))->Fill(Beta_meas);
  if(Z_meas<=primary_cha && Z_meas>0)
    ((TH1D*)gDirectory->Get(Form("Zrec%d/Mass",Z_meas)))->Fill(M_meas);

  if(fFlagMC && Beta_true>=0){
    ((TH1D*)gDirectory->Get("Charge_trk_True"))->Fill(Z_true);
    ((TH1D*)gDirectory->Get("ThTrue"))->Fill(Th_true);
    ((TH1D*)gDirectory->Get("Tof_true"))->Fill(Tof_true);
    ((TH1D*)gDirectory->Get("Beta_true"))->Fill(Beta_true);
  }

  // charge purity
  if (Z_meas == Z_true){
    ((TH1D*)gDirectory->Get("Charge_purity")) -> Fill(Z_meas);
  }
  return;
}

void GlobalRecoAna::MCParticleStudies() {

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::MCParticleStudies start"<<endl;

  Int_t n_particles = myMcNtuPart -> GetTracksN();        // n° of particles of an event
  ((TH1D*)gDirectory->Get("MC_check/TracksN_MC")) -> Fill(myMcNtuPart -> GetTracksN());

  //************************************************************* begin Loop on all MCparticles *************************************************************
  for (Int_t i= 0 ; i < n_particles; i++) {                         // for every particle in an event

  TAMCpart* particle = myMcNtuPart->GetTrack(i);
  auto  Mid = particle->GetMotherID();
  double mass = particle->GetMass();             // Get TRpaid-1
  auto Reg = particle->GetRegion();
  auto finalPos = particle-> GetFinalPos();
  int baryon = particle->GetBaryon();
  TVector3 initMom = particle->GetInitP();
  double InitPmod = sqrt( pow(initMom(0),2) + pow(initMom(1),2) + pow(initMom(2),2));
  Float_t Ek_tr_tot = ( sqrt( pow(InitPmod,2) + pow(mass,2)) - mass );
  Ek_tr_tot = Ek_tr_tot * fpFootGeo->GevToMev();
  Float_t Ek_true = Ek_tr_tot / (double)baryon;
  Float_t theta_tr = particle->GetInitP().Theta()*(180/TMath::Pi());   // in deg
  Float_t charge_tr = particle-> GetCharge();

  //Fill histos for MC variables checks
  ((TH1D*)gDirectory->Get("MC_check/Charge_MC")) -> Fill(particle-> GetCharge());
  ((TH1D*)gDirectory->Get("MC_check/Mass_MC")) -> Fill(particle-> GetMass());
  ((TH1D*)gDirectory->Get("MC_check/Ek_tot_MC")) -> Fill(Ek_tr_tot);
  ((TH1D*)gDirectory->Get("MC_check/InitPosZ_MC")) -> Fill(particle-> GetInitPos().Z() );
  ((TH1D*)gDirectory->Get("MC_check/FinalPosZ_MC")) -> Fill(particle-> GetFinalPos().Z() );
  ((TH1D*)gDirectory->Get("MC_check/TrkLength_MC")) -> Fill(particle-> GetTrkLength());
  ((TH1D*)gDirectory->Get("MC_check/Type_MC")) -> Fill(particle-> GetType());
  ((TH1D*)gDirectory->Get("MC_check/FlukaID_MC")) -> Fill(particle->  GetFlukaID());
  ((TH1D*)gDirectory->Get("MC_check/MotherID_MC")) -> Fill(particle->  GetMotherID());
  ((TH1D*)gDirectory->Get("MC_check/Dead_MC")) -> Fill(particle-> GetDead());   //
  ((TH1D*)gDirectory->Get("MC_check/Time_MC")) -> Fill(particle-> GetTime());   //
  ((TH1D*)gDirectory->Get("MC_check/Tof_MC")) -> Fill(particle-> GetTof());    //
  ((TH1D*)gDirectory->Get("MC_check/Region_MC")) -> Fill(particle->  GetRegion());   //
  ((TH1D*)gDirectory->Get("MC_check/Baryon_MC")) -> Fill(particle->  GetBaryon());   //
  ((TH1D*)gDirectory->Get("MC_check/Theta_MC")) -> Fill(particle->  GetInitP().Theta()*180./TMath::Pi());   //


  //! finalPos.Z() > 189.15 IN GSI2021_MC
  //! finalPos.Z() > 90 IN 16O_400

  //TO BE CHECKED
  // Int_t TG_region = -1;         //! hard coded
  // if(fExpName.IsNull())
  // TG_region = 59; // true in newgeom setup
  // else if(!fExpName.CompareTo("GSI/") || !GlobalRecoAna::fExpName.CompareTo("GSI_MC/"))
  // TG_region = 48;  //  GSI-2019
  // else if(!fExpName.CompareTo("CNAO2020/"))
  // TG_region = 50;  //  CNAO-2020
  // else if(!fExpName.CompareTo("GSI2021_MC/"))
  // TG_region = 50; //   GSI2021_MC

  //-------------  MC TOTAL CROSS SECTION
  // if the particle is generated in the target and it is the fragment of a primary
  //if Z<8 and A<30, so if it is a fragment (not the primitive projectile, nor detector fragments)
  // ifenough energy/n to go beyond the target
  if (  Mid==0 && Reg == GetParGeoG()->GetRegTarget() && particle->GetCharge()>0 && particle->GetCharge()<=primary_cha && Ek_true>100)
  FillYieldMC("yield-true_cut",charge_tr,theta_tr,Ek_tr_tot);

  //-------------  MC FIDUCIAL CROSS SECTION (<8 deg)
  if (  Mid==0 && Reg == GetParGeoG()->GetRegTarget() && particle->GetCharge()>0 && particle->GetCharge()<=primary_cha && Ek_true>100 && theta_tr <= 8.)
    FillYieldMC("yield-true_DET",charge_tr,theta_tr,Ek_tr_tot);

  //-------------  MC FIDUCIAL CROSS SECTION (<2 deg)
  /*
  if (  Mid==0 && Reg == GetParGeoG()->GetRegTarget() && particle->GetCharge()>0 && particle->GetCharge()<=primary_cha && Ek_true>100
  && theta_tr <= 2.  //  myangle // angular aperture < 8 deg
  )
  FillYieldMC("yield-true_DET2",charge_tr,theta_tr,Ek_tr_tot);
  */

  TWAlgoStudy();
  }

  //loop on crossings
  if(TAGrecoManager::GetPar()->IsRegionMc()){
    Int_t exitfragnum=0;    //number of fragmengs exit from the target
    Int_t exitfrag10anglenum=0;    //number of fragmengs exit from the target
    for(int icr = 0;icr<GetNtuMcReg()->GetRegionsN();icr++){
      TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);
        if( fpNtuMcReg->GetCharge()>0 && fpNtuMcReg->GetMass()>0  && fpNtuMcReg->GetOldCrossN()==GetParGeoG()->GetRegTarget() && fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirPreTW()){ //a particle exit from the target
          exitfragnum++;
        if(fpNtuMcReg->GetMomentum().Theta()*TMath::RadToDeg()<10)
          exitfrag10anglenum++;
      }
    }
    ((TH2D*)gDirectory->Get("MC/MCpartVsGlbtrackNum"))->Fill(exitfragnum,myGlb->GetTracksN());
    ((TH2D*)gDirectory->Get("MC/MCpartVsGlbtrackNum_angle10"))->Fill(exitfrag10anglenum,myGlb->GetTracksN());
  }


  return;
}

void GlobalRecoAna::SetupTree(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::SetupTree start"<<endl;

  myReader = new TAGactTreeReader("myReader");
  fpNtuGlbTrack = new TAGdataDsc("glbTrack",new TAGntuGlbTrack());
  gTAGroot->AddRequiredItem("glbTrack");
  myReader->SetupBranch(fpNtuGlbTrack, TAGntuGlbTrack::GetBranchName());

  if(TAGrecoManager::GetPar()->IncludeBM()){
  fpNtuTrackBm = new TAGdataDsc("bmtrack" , new TABMntuTrack());
  gTAGroot->AddRequiredItem("bmtrack");
  myReader->SetupBranch(fpNtuTrackBm);

  }

  if(TAGrecoManager::GetPar()->IncludeVT()){
    fpNtuClusVtx = new TAGdataDsc("vtclus",new TAVTntuCluster());
    fpNtuTrackVtx = new TAGdataDsc("vttrack",new TAVTntuTrack());
    fpNtuVtx = new TAGdataDsc("vtvtx",new TAVTntuVertex());
    gTAGroot->AddRequiredItem("vtclus");
    gTAGroot->AddRequiredItem("vttrack");
    gTAGroot->AddRequiredItem("vtvtx");
    myReader->SetupBranch(fpNtuClusVtx);
    myReader->SetupBranch(fpNtuTrackVtx);
    myReader->SetupBranch(fpNtuVtx);
    if(fFlagMC){
      fpNtuMcVt = new TAGdataDsc(FootDataDscMcName(kVTX),new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcvt");
      myReader->SetupBranch(fpNtuMcVt, FootBranchMcName(kVTX));
    }
  }
  if(TAGrecoManager::GetPar()->IncludeIT()){
    fpNtuClusIt = new TAGdataDsc(new TAITntuCluster());
    gTAGroot->AddRequiredItem("itclus");
    myReader->SetupBranch(fpNtuClusIt);
    if(fFlagMC){
      fpNtuMcIt = new TAGdataDsc(FootDataDscMcName(kITR), new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcit");
      myReader->SetupBranch(fpNtuMcIt, FootBranchMcName(kITR));
    }
  }
  if(TAGrecoManager::GetPar()->IncludeMSD()){
    fpNtuClusMsd = new TAGdataDsc(new TAMSDntuCluster());
    gTAGroot->AddRequiredItem("msdclus");
    myReader->SetupBranch(fpNtuClusMsd);
    fpNtuRecMsd = new TAGdataDsc(new TAMSDntuPoint());
    gTAGroot->AddRequiredItem("msdpoint");
    myReader->SetupBranch(fpNtuRecMsd);
    if(fFlagMC){
      fpNtuMcMsd = new TAGdataDsc(FootDataDscMcName(kMSD),new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcmsd");
      myReader->SetupBranch(fpNtuMcMsd, FootBranchMcName(kMSD));
    }
  }
  if(TAGrecoManager::GetPar()->IncludeTW()){
    fpNtuRecTw = new TAGdataDsc(new TATWntuPoint());
    gTAGroot->AddRequiredItem("twpt");
    myReader->SetupBranch(fpNtuRecTw);

    if (fFlagMC == false){
      fpNtuWDtrigInfo = new TAGdataDsc(new TAWDntuTrigger());
      gTAGroot->AddRequiredItem("WDtrigInfo");
      myReader->SetupBranch(fpNtuWDtrigInfo);
    }
    if(fFlagMC){
      fpNtuMcTw = new TAGdataDsc(FootDataDscMcName(kTW),new TAMCntuHit());
      gTAGroot->AddRequiredItem("mctw");
      myReader->SetupBranch(fpNtuMcTw, FootBranchMcName(kTW));
    }
  }
  if(TAGrecoManager::GetPar()->IncludeCA()){
    fpNtuClusCa = new TAGdataDsc(new TACAntuCluster());
    gTAGroot->AddRequiredItem("caclus");
    myReader->SetupBranch(fpNtuClusCa);
    if(fFlagMC){
      fpNtuMcCa = new TAGdataDsc(FootDataDscMcName(kCAL),new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcca");
      myReader->SetupBranch(fpNtuMcCa, FootBranchMcName(kCAL));
    }
  }

  if(fFlagMC){
    fpNtuMcEvt = new TAGdataDsc(new TAMCntuEvent());
    fpNtuMcTrk = new TAGdataDsc(new TAMCntuPart());
    gTAGroot->AddRequiredItem("mcevt");
    gTAGroot->AddRequiredItem("mctrack");
    myReader->SetupBranch(fpNtuMcEvt);
    myReader->SetupBranch(fpNtuMcTrk);
    if(TAGrecoManager::GetPar()->IsRegionMc()){
      fpNtuMcReg = new TAGdataDsc(new TAMCntuRegion());
      gTAGroot->AddRequiredItem("mcreg");
      myReader->SetupBranch(fpNtuMcReg);
    }
  }

  gTAGroot->AddRequiredItem("myReader");
  gTAGroot->Print();

  return;
}

void GlobalRecoAna::VTanal(TAGpoint *tmp_poi, Int_t &idxCLU){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::VTanal start"<<endl;

  Double_t xp = (tmp_poi->GetPosition()).X();
  Double_t yp = (tmp_poi->GetPosition()).Y();
  Double_t zp = (tmp_poi->GetPosition()).Z();
  Int_t charge = fGlbTrack->GetTwChargeZ();
  TAVTntuCluster *myVTclus = (TAVTntuCluster*)fpNtuClusVtx->GenerateObject();

  if(tmp_poi->GetSensorIdx()>=0 && tmp_poi->GetClusterIdx()>=0){
    TAVTcluster *tmp_vtclu = myVTclus->GetCluster(tmp_poi->GetSensorIdx(), tmp_poi->GetClusterIdx());
    TAVTntuVertex *myVtVtx = (TAVTntuVertex*)fpNtuVtx->GenerateObject();
    for(int ivtx = 0;ivtx<myVtVtx->GetVertexN(); ivtx++){
      TAVTvertex *tmp_vtvtx = myVtVtx->GetVertex(ivtx);
      for(int ivtr = 0;ivtr<tmp_vtvtx->GetTracksN();ivtr++){
        TAVTtrack *tmp_vtvtr = tmp_vtvtx->GetTrack(ivtr);
        for(int ivtcl = 0;ivtcl<tmp_vtvtr->GetClustersN();ivtcl++){
          TAVTbaseCluster *tmp_c = tmp_vtvtr->GetCluster(ivtcl);
          if(tmp_c->GetPosition() == tmp_vtclu->GetPosition())
          Th_meas = tmp_vtvtr->GetTheta()*180./TMath::Pi();
        }
      }
    }
    TVector3 clPos = tmp_vtclu->GetPosition();
    TVector3 GLclPos = fpFootGeo->FromVTLocalToGlobal(clPos);
    if(FootDebugLevel(1))
      cout<<" CHECK POS "<<GLclPos.X()<<" "<<xp<<" "<<GLclPos.Y()<<" "<<yp<<" "<<GLclPos.Z()<<" "<<zp<<endl;


    if(fFlagMC){
      map<int, map<int, map<int,vector<int>>>> mapall;
      mapall.clear();
      for(int ipix=0;ipix<tmp_vtclu->GetPixelsN();ipix++){
        TAVThit* pixel = tmp_vtclu->GetPixel(ipix);
          if(pixel->GetMcTracksN()>0){
            for(int imcpa = 0; imcpa<pixel->GetMcTracksN();imcpa++){

            int ipa = pixel->GetMcTrackIdx(imcpa);

            if(ipa>=0){
             TAMCpart *tmp_mctrack = GetNtuMcTrk()->GetTrack(ipa);     //Error in <TClonesArray::operator[]>: out of bounds at -1 in ...
             Int_t charge_vt_2 = tmp_mctrack->GetCharge();


             if(FootDebugLevel(1))
               cout<<"isen "<<tmp_poi->GetSensorIdx()<<" : vtcl "<<tmp_poi->GetClusterIdx()<<" : pix "<<ipix<<" ::: "<<"Poi MC "<<imcpa<<" = "<<charge<<" - "<<charge_vt_2 <<" "<<pixel->GetMcTrackIdx(imcpa)<<" "<<tmp_mctrack->GetFlukaID()<<" "<<tmp_mctrack->GetMotherID()<<" "<<tmp_mctrack->GetInitPos().X()<<" "<<tmp_mctrack->GetFinalPos().X()<<endl;


             ((TH2D*)gDirectory->Get("MC/ChargePoi_vs_ChargeVT"))->Fill(charge_vt_2,charge);


             mapall[pixel->GetMcTrackIdx(imcpa)][charge_vt_2][tmp_mctrack->GetMotherID()+1].push_back(1);

             //this is ok to check the "clustering" efficiency at step 0
             if(charge_vt_2==charge)
               nchargeok_vt++;
             nchargeall_vt++;

            }
          }
        }//close if MCtracks
      }//close loop on npix


      if(mapall.size()>1){
        if(FootDebugLevel(1))
          cout<<"MORE than 1 idx : ev = "<<myReader->GetCurrentTriger()<<endl; //to be checked
      }else if(mapall.size()==1){
        npure_vt++;
      }
      if(FootDebugLevel(1)){
        for( const auto& x : mapall ){
          cout << "idx: " << x.first << "  ch: [  ";
          for( const auto& d : x.second ){
          cout << d.first<<" ] idpa [ ";
          for( const auto& e : d.second ) cout << e.first<<"] size ["<<e.second.size();
          cout << "]"<<endl;
          }
        }
      }

      Int_t charge_vt_clu = -999;
      Int_t idx_vt_clu = -1;
      Int_t charge_vt_clu_bis = -999;
      Int_t idx_vt_clu_bis = -1;
      SetMCtruth(mapall, idx_vt_clu, charge_vt_clu);
      SetMCtruthBIS(mapall, idx_vt_clu_bis, charge_vt_clu_bis);

      idxCLU = idx_vt_clu;

      if(FootDebugLevel(1))
        cout<<"CLUSTER Zmc "<<charge_vt_clu<<" Idx "<<idx_vt_clu<<endl;

      if(idx_vt_clu!=idx_vt_clu_bis){
        cout<<"DIFFERENT APPROACHES VT:: "<<GetNtuMcEvt()->GetEventNumber()<<endl;
        cout<<idx_vt_clu<<" "<<idx_vt_clu_bis<<endl;
        cout<<charge_vt_clu<<" "<<charge_vt_clu_bis<<endl;
DiffApp_trkIdx = true;
      }

      if(charge_vt_clu==charge)
        nchargeok_vt_clu++;
    }

    nchargeall_vt_clu++;
  }

  if(FootDebugLevel(1))
    cout<<"VTanal done"<<endl;

  return;
}

void GlobalRecoAna::ITanal(TAGpoint *tmp_poi, Int_t &idxCLU){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::ITanal start"<<endl;

  Double_t xp = (tmp_poi->GetPosition()).X();
  Double_t yp = (tmp_poi->GetPosition()).Y();
  Double_t zp = (tmp_poi->GetPosition()).Z();
  Int_t charge = fGlbTrack->GetTwChargeZ();

  TAITntuCluster *myITclus = (TAITntuCluster*)fpNtuClusIt->GenerateObject();
  if(tmp_poi->GetSensorIdx()>=0 && tmp_poi->GetClusterIdx()>=0){
    TAITcluster *tmp_itclu = myITclus->GetCluster(tmp_poi->GetSensorIdx(), tmp_poi->GetClusterIdx());
    if(fFlagMC) {
      //  TAITparGeo* ITparGeo = (TAITparGeo*)fpParGeoIt->Object();
      TVector3 clPos = tmp_itclu->GetPosition();
      TVector3 GLclPos = fpFootGeo->FromITLocalToGlobal(clPos);
      if(FootDebugLevel(1))
      cout<<"CHECK POS "<<GLclPos.X()<<" "<<xp<<" "<<GLclPos.Y()<<" "<<yp<<" "<<GLclPos.Z()<<" "<<zp<<endl;

      map<int, map<int, map<int,vector<int>>>> mapall;
      mapall.clear();


      for(int ipix=0;ipix<tmp_itclu->GetPixelsN();ipix++){

        TAVThit* pixel_it = tmp_itclu->GetPixel(ipix);

        if(pixel_it->GetMcTracksN()>0){

          for(int imcpa = 0; imcpa<pixel_it->GetMcTracksN();imcpa++){
          Int_t ipa = pixel_it->GetMcTrackIdx(imcpa);
          if(ipa>=0){
           TAMCpart *tmp_mctrack = GetNtuMcTrk()->GetTrack(ipa);
           Int_t charge_it = tmp_mctrack->GetCharge();
           ((TH2D*)gDirectory->Get("MC/ChargePoi_vs_ChargeIT"))->Fill(charge_it,charge);
           mapall[pixel_it->GetMcTrackIdx(imcpa)][charge_it][tmp_mctrack->GetMotherID()+1].push_back(1);

           if(charge_it==charge)
             nchargeok_it++;
           nchargeall_it++;

              if(FootDebugLevel(1))
                cout<<"sen "<<tmp_poi->GetSensorIdx()<<" : itcl "<<tmp_poi->GetClusterIdx()<<" : pix "<<ipix<<" ::: "<<"Poi MC "<<imcpa<<" = "<<charge<<" - "<<charge_it <<" "<<pixel_it->GetMcTrackIdx(imcpa)<<" "<<tmp_mctrack->GetFlukaID()<<" "<<tmp_mctrack->GetMotherID()<<" "<<tmp_mctrack->GetInitPos().X()<<" "<<tmp_mctrack->GetFinalPos().X()<<endl;

          }
          }
        }//close if MCtracks

      }//close loop on npix

      if(mapall.size()>1){
        if(FootDebugLevel(1))
          cout<<"MORE than 1 idx : ev = "<<GetNtuMcEvt()->GetEventNumber()<<endl;
      }
      if(mapall.size()==1){
        npure_it++;
      }


      if(FootDebugLevel(1)){
          for( const auto& x : mapall ){
          cout << "idx: " << x.first << "  ch: [  ";
          for( const auto& d : x.second ){
          cout << d.first<<" ] idpa [ ";
          for( const auto& e : d.second )
              cout << e.first<<"] size ["<<e.second.size();
          cout << "]"<<endl;
          }
        }
      }

      Int_t charge_it_clu = -999;
      Int_t idx_it_clu = -1;
      Int_t charge_it_clu_bis = -999;
      Int_t idx_it_clu_bis = -1;
      SetMCtruth(mapall, idx_it_clu, charge_it_clu);
      SetMCtruthBIS(mapall, idx_it_clu_bis, charge_it_clu_bis);
      idxCLU = idx_it_clu;
      if(FootDebugLevel(1))
        cout<<"CLUSTER Zmc "<<charge_it_clu<<" Idx "<<idx_it_clu<<endl;

      if(idx_it_clu!=idx_it_clu_bis){
        cout<<"DIFFERENT APPROACHES IT:: "<<GetNtuMcEvt()->GetEventNumber()<<endl;
        cout<<idx_it_clu<<" "<<idx_it_clu_bis<<endl;
        cout<<charge_it_clu<<" "<<charge_it_clu_bis<<endl;
        DiffApp_trkIdx = true;
      }

      if(charge_it_clu==charge)
        nchargeok_it_clu++;
    }

      nchargeall_it_clu++;
  }

  if(FootDebugLevel(1))
    cout<<"ITanal done"<<endl;

  return;

}

void GlobalRecoAna::MSDanal(TAGpoint *tmp_poi, Int_t &idxCLU) {

    if(FootDebugLevel(1))
      cout<<"GlobalRecoAna::MSDanal start"<<endl;

    Double_t xp = (tmp_poi->GetPosition()).X();
    Double_t yp = (tmp_poi->GetPosition()).Y();
    Double_t zp = (tmp_poi->GetPosition()).Z();

    Int_t charge = fGlbTrack->GetTwChargeZ();

    TAMSDparGeo* MSDparGeo = (TAMSDparGeo*)fpParGeoMsd->Object();
    TAMSDntuCluster *myMSDclus = (TAMSDntuCluster*)fpNtuClusMsd->GenerateObject();
    TAMSDntuPoint *myMSDpoint = (TAMSDntuPoint*)fpNtuRecMsd->GenerateObject();

    if(tmp_poi->GetSensorIdx()>=0 && tmp_poi->GetClusterIdx()>=0){
      TAMSDcluster *tmp_msdclu = myMSDclus->GetCluster(tmp_poi->GetSensorIdx(), tmp_poi->GetClusterIdx());

      TVector3 clPos = tmp_msdclu->GetPosition();
      TVector3 GLclPos = fpFootGeo->FromMSDLocalToGlobal(clPos);

      if(fFlagMC) {
        map<int, map<int, map<int,vector<int>>>> mapall;
        mapall.clear();

        if(FootDebugLevel(1)){
          cout<<"CHECK POS "<<GLclPos.X()<<" "<<xp<<" "<<GLclPos.Y()<<" "<<yp<<" "<<GLclPos.Z()<<" "<<zp<<endl;
          cout<<"tmp_msdclu->GetStripsN()="<<tmp_msdclu->GetStripsN()<<endl;
        }

        if(tmp_msdclu->GetMcTracksN()>0){
          for(int imcpa = 0; imcpa<tmp_msdclu->GetMcTracksN();imcpa++){
            int ipa = tmp_msdclu->GetMcTrackIdx(imcpa);
            if(ipa>=0){
             TAMCpart *tmp_mctrack = GetNtuMcTrk()->GetTrack(ipa);
             Int_t charge_msd = tmp_mctrack->GetCharge();
             if(FootDebugLevel(1))
               cout<<"sen "<<tmp_poi->GetSensorIdx()<<" : msdcl "<<tmp_poi->GetClusterIdx()<<" ::: "<<"Poi MC "<<imcpa<<" = "<<charge<<" - "<<charge_msd<<" "<<tmp_msdclu->GetMcTrackIdx(imcpa)<<" "<<tmp_mctrack->GetFlukaID()<<" "<<tmp_mctrack->GetMotherID()<<" "<<tmp_mctrack->GetInitPos().X()<<" "<<tmp_mctrack->GetFinalPos().X()<<endl;

               ((TH2D*)gDirectory->Get("MC/ChargePoi_vs_ChargeMSD"))->Fill(charge_msd,charge);
               mapall[tmp_msdclu->GetMcTrackIdx(imcpa)][charge_msd][tmp_mctrack->GetMotherID()+1].push_back(1);
               if(charge_msd==charge)
                 nchargeok_msd++;
               nchargeall_msd++;
             }
          }
        }//close if MCtracks

        if(mapall.size()>1){
          if(FootDebugLevel(1))
            cout<<"MORE than 1 idx : ev = "<<GetNtuMcEvt()->GetEventNumber()<<endl;
        }
        if(mapall.size()==1)
          npure_msd++;


        if(FootDebugLevel(1)){
          for( const auto& x : mapall ){
            cout << "idx: " << x.first << "  ch: [  ";
            for( const auto& d : x.second ){
            cout << d.first<<" ] idpa [ ";
            for( const auto& e : d.second )
                cout << e.first<<"] size ["<<e.second.size();
            cout << "]"<<endl;
            }
          }
        }

        Int_t charge_msd_clu = -999;
        Int_t idx_msd_clu = -1;
        Int_t charge_msd_clu_bis = -999;
        Int_t idx_msd_clu_bis = -1;
        SetMCtruth(mapall, idx_msd_clu, charge_msd_clu);
        SetMCtruthBIS(mapall, idx_msd_clu_bis, charge_msd_clu_bis);

        idxCLU = idx_msd_clu;

        if(FootDebugLevel(1))
          cout<<"CLUSTER Zmc "<<charge_msd_clu<<" Idx "<<idx_msd_clu<<endl;

        if(idx_msd_clu!=idx_msd_clu_bis){
          cout<<"DIFFERENT APPROACHES MSD:: "<<GetNtuMcEvt()->GetEventNumber()<<endl;
          cout<<idx_msd_clu<<" "<<idx_msd_clu_bis<<endl;
          cout<<charge_msd_clu<<" "<<charge_msd_clu_bis<<endl;
          DiffApp_trkIdx = true;
        }

        if(charge_msd_clu==charge)
          nchargeok_msd_clu++;

     }

    nchargeall_msd_clu++;
  }

  if(FootDebugLevel(1))
    cout<<"MSDanal done"<<endl;

  return;
}

void GlobalRecoAna::TWanal(TAGpoint *tmp_poi, Int_t &idxCLU){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::TWanal start"<<endl;

  Double_t xp = (tmp_poi->GetPosition()).X();
  Double_t yp = (tmp_poi->GetPosition()).Y();
  Double_t zp = (tmp_poi->GetPosition()).Z();

  Int_t charge = fGlbTrack->GetTwChargeZ();

  if(tmp_poi->GetClusterIdx()>=0){
    TATWpoint *tw_point = GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx());
    TVector3 clPos = tw_point->GetPosition();
    TVector3 GLclPos = fpFootGeo->FromTWLocalToGlobal(clPos);

    Int_t charge_tw = tw_point->GetChargeZ();

    if(fFlagMC){
      if(tw_point->GetPointMatchMCtrkID()>=0){
        TAMCpart *tmp_mctrack = GetNtuMcTrk()->GetTrack(tw_point->GetPointMatchMCtrkID());

        if(FootDebugLevel(1))
        cout<<tmp_poi->GetClusterIdx()<<" : Poi MC "<<tw_point->GetPointMatchMCtrkID()<<" = "<<charge<<" - "<<charge_tw <<" - "<<tmp_mctrack->GetCharge()<<" "<<tmp_mctrack->GetFlukaID()<<" "<<tmp_mctrack->GetMotherID()<<" "<<tmp_mctrack->GetInitPos().X()<<" "<<tmp_mctrack->GetFinalPos().X()<<endl;

        ((TH2D*)gDirectory->Get("MC/ChargePoi_vs_ChargeTW"))->Fill(charge_tw,charge);

        idxCLU = tw_point->GetPointMatchMCtrkID();
      }

      if(FootDebugLevel(1))
        cout<<"CLUSTER Zmc "<<charge_tw<<endl;

      if(charge_tw==charge)
        nchargeok_tw++;
    }

    nchargeall_tw++;
  }

  if(FootDebugLevel(1))
    cout<<"TWanal done"<<endl;

  return;
}

void GlobalRecoAna::CALOanal(Int_t clusidx){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::CALOanal start"<<endl;

    // Bool_t  VtOneTrack = (myVtTr->GetTracksN()==1);
    Int_t Zreco=fGlbTrack->GetTwChargeZ();
    TACAcluster *pCaClu=GetNtuClusterCa()->GetCluster(clusidx);
    ((TH1D*)gDirectory->Get("CALO/GlbTrk/ClusSize_glbtrk"))->Fill(pCaClu->GetHitsN());
    ((TH1D*)gDirectory->Get(Form("CALO/Zreco%d/ClusSize",Zreco)))->Fill(pCaClu->GetHitsN());
    ((TH1D*)gDirectory->Get(Form("CALO/Zreco%d/ClusCharge",Zreco)))->Fill(pCaClu->GetEnergy());

    //
    //
    // ((TH1D*)gDirectory->Get("FIT_vs_Meas/Ekin"))->Fill(fGlbTrack->GetFitEnergy()-pCaClu->GetEnergy());

  if(FootDebugLevel(1))
    cout<<"CALOanal done"<<endl;

  return;
}

void GlobalRecoAna::FullCALOanal(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::FullCALOanal start"<<endl;


  TAVTntuTrack *myVtTr = (TAVTntuTrack*)fpNtuTrackVtx->GenerateObject();

  Double_t caloCharge = 0.;

  ((TH1D*)gDirectory->Get("CALO/ClusNum"))->Fill(GetNtuClusterCa()->GetClustersN());


  map<int,int> MCtwexitpart; //key:TAMCpart index for all the particle exit from tw, value=crossing index for the same particle entering for the first time in calo or -1 if not
  Int_t caloinpartnum=0; //number of particles entering in the calo and coming from the tw
  if(TAGrecoManager::GetPar()->IsRegionMc() && fFlagMC){
    for(int icr = 0;icr<GetNtuMcReg()->GetRegionsN();icr++){
      TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);
      if(fpNtuMcReg->GetCharge()>0 && fpNtuMcReg->GetMass()>0.9 && fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirTW() && fpNtuMcReg->GetOldCrossN()<=GetParGeoTw()->GetRegStrip(0,GetParGeoTw()->GetNBars()-1) && fpNtuMcReg->GetOldCrossN()>=GetParGeoTw()->GetRegStrip(0,0)) //a particle exit from the tw
        MCtwexitpart[fpNtuMcReg->GetID()-1]=-1;
      if(fpNtuMcReg->GetCharge()>0 && fpNtuMcReg->GetMass()>0.9 && fpNtuMcReg->GetCrossN()>=GetParGeoCa()->GetRegCrystal(0) && fpNtuMcReg->GetCrossN()<=GetParGeoCa()->GetRegCrystal(GetParGeoCa()->GetCrystalsN()-1) && MCtwexitpart.count(fpNtuMcReg->GetID()-1)==1) //a particle entering in the calo that also exit from the tw
        if(MCtwexitpart[fpNtuMcReg->GetID()-1]==-1){
          MCtwexitpart[fpNtuMcReg->GetID()-1]=icr;
          caloinpartnum++;
        }
    }
    ((TH2D*)gDirectory->Get("CALO/MCpartNumvscluster"))->Fill(caloinpartnum,GetNtuClusterCa()->GetClustersN());
    for(auto it = MCtwexitpart.begin() ; it != MCtwexitpart.end() ; ++it)
      if(it->second>=0)
        for(auto iu = std::next(it) ; iu !=MCtwexitpart.end() ; ++iu)
          if(iu->second>=0)
            ((TH1D*)gDirectory->Get("CALO/MultiPartDistance"))->Fill((GetNtuMcReg()->GetRegion(it->second)->GetPosition()-GetNtuMcReg()->GetRegion(iu->second)->GetPosition()).Mag());

  }

  if(FootDebugLevel(1))
    cout<<"FullCALOanal:start loop on all the calo clusters"<<endl;

  for(Int_t i=0;i<GetNtuClusterCa()->GetClustersN();i++){

    // TAMCpart *highChaPart;
    map<int,int> MCtrkXClus; //key: number of MC particles arrived from tw and entering in calo, value: number of crystals with a hit from that particle
    // Int_t partfind=0;
    TAMCregion *ntumchighchreg, *ntumcreg;
    Int_t partcharge=-1;

    TACAcluster *pCaClu=GetNtuClusterCa()->GetCluster(i);
    ((TH1D*)gDirectory->Get("CALO/ClusSize"))->Fill(pCaClu->GetHitsN());

    if(FootDebugLevel(1))
      cout<<"FullCALOanal:start loop on hits, cluster number="<<i<<"/"<<GetNtuClusterCa()->GetClustersN()<<endl;

    if(pCaClu->GetHitsN()==2)
      ((TH1D*)gDirectory->Get("CALO/clus2HitDist"))->Fill((pCaClu->GetHit(0)->GetPosition()-pCaClu->GetHit(1)->GetPosition()).Mag());

    for(Int_t k=0;k<pCaClu->GetHitsN();k++){
      TACAhit* pCaHit=pCaClu->GetHit(k);
      if(fFlagMC){
        ((TH1D*)gDirectory->Get("CALO/MCpartNumXHit"))->Fill(pCaHit->GetMcTracksN());
        if(TAGrecoManager::GetPar()->IsRegionMc()){
          for(Int_t m=0;m<pCaHit->GetMcTracksN();m++){
            if(MCtwexitpart.count(pCaHit->GetMcTrackIdx(m)-1)==1){  //the particle arrived from the tw
              if(MCtwexitpart.at(pCaHit->GetMcTrackIdx(m)-1)>=0){  //the particle arrived from tw and crossed the  calo
                MCtrkXClus[pCaHit->GetMcTrackIdx(m)-1]= MCtrkXClus.count(pCaHit->GetMcTrackIdx(m)-1)+1;
                ntumcreg = GetNtuMcReg()->GetRegion(MCtwexitpart.at(pCaHit->GetMcTrackIdx(m)-1));
                if(m==0)
                  ntumchighchreg=GetNtuMcReg()->GetRegion(MCtwexitpart.at(pCaHit->GetMcTrackIdx(m)-1));
                if(std::round(ntumchighchreg->GetCharge())>partcharge){
                  partcharge=std::round(ntumchighchreg->GetCharge());
                  ntumchighchreg=GetNtuMcReg()->GetRegion(MCtwexitpart.at(pCaHit->GetMcTrackIdx(m)-1));
                }
              }
            }
          }
        }
      }
    }//end of loop on hits

    if(FootDebugLevel(1))
      cout<<"FullCALOanal:start loop on hits done, cluster number="<<i<<"/"<<GetNtuClusterCa()->GetClustersN()<<endl;

    ((TH1D*)gDirectory->Get("CALO/MCpartChavsClusSize"))->Fill(partcharge,pCaClu->GetHitsN());
    if(partcharge>=0 && partcharge<=primary_cha){
      ((TH1D*)gDirectory->Get(Form("CALO/Zmc%d/partNumXCluster",partcharge)))->Fill(MCtrkXClus.size());
      for(const auto& x : MCtrkXClus)
        ((TH1D*)gDirectory->Get(Form("CALO/Zmc%d/partChaXCluster",partcharge)))->Fill(GetNtuMcTrk()->GetTrack(x.first)->GetCharge());

      Double_t mcekin = (sqrt(pow(ntumchighchreg->GetMass(),2) + pow((ntumchighchreg->GetMomentum()).Mag(),2)) - ntumchighchreg->GetMass())/ntumchighchreg->GetMass()*fpFootGeo->GevToMev();
      ((TH2D*)gDirectory->Get(Form("CALO/Zmc%d/EkinVsCluscharge",partcharge)))->Fill(mcekin,pCaClu->GetEnergy()/ntumchighchreg->GetMass());
      ((TH2D*)gDirectory->Get(Form("CALO/Zmc%d/ClusSizeVsTheta",partcharge)))->Fill(pCaClu->GetHitsN(),ntumchighchreg->GetMomentum().Theta());
    }
    ((TH1D*)gDirectory->Get("CALO/ClusCharge"))->Fill(pCaClu->GetEnergy());

    caloCharge += pCaClu->GetEnergy();
  }//end of loop on clusters

  if(fEvtGlbTrkVec.size()>0){  //!
    Int_t caloasscls=0;
    for(Int_t i=0;i<fEvtGlbTrkVec.size();i++){
      if(fEvtGlbTrkVec.at(i).at(4).size()==1){
        if(fEvtGlbTrkVec.at(i).at(4).at(0)>=0)
        caloasscls++;
      }
    }
    ((TH1D*)gDirectory->Get("CALO/GlbTrk/ClusNum_glbtrk"))->Fill(caloasscls);
    ((TH1D*)gDirectory->Get("CALO/GlbTrk/ClusEfficiency"))->Fill((double)caloasscls/fEvtGlbTrkVec.size());
  }


  if(FootDebugLevel(1))
    cout<<"FullCALOanal done"<<endl;

  return;
}

void GlobalRecoAna::SetMCtruth(map<int, map<int, map<int,vector<int>>>> mapall, int &idx, int &charge){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::SetMCtruth start"<<endl;

  Int_t my_mult(-1);
  Int_t my_charge(-999);
  Int_t my_idx(-999);
  Int_t my_idpa(-999);

  Int_t tmp_size = 0;

  for(map<int,map<int,map<int,vector<int>>>>::iterator it=mapall.begin(); it!=mapall.end(); it++){
    for(map<int,map<int,vector<int>>>::iterator it2=it->second.begin(); it2!=it->second.end(); it2++){
      for(map<int,vector<int>>::iterator it3=it2->second.begin(); it3!=it2->second.end(); it3++){
      tmp_size = it3->second.size();
      if(tmp_size==my_mult){
       if(FootDebugLevel(1))
         cout<<"same mult"<<endl;
       if(it3->first == (my_idx+1)){
         break;
       }else{
         //take the particle with highest charge, but not the nuclear evaporated frag (idpa=0 & icharge != primary charge!
         if(it2->first > my_charge && (it3->first==0 && it2->first == primary_cha)){
           my_idx = it->first;
           my_charge = it2->first;
         }else if(it2->first == my_charge && (it3->first==0 && it2->first == primary_cha)){
           if(it->first < my_idx){//or take the particle with lowest index -> arbitrary choice!
             my_idx = it->first;
             my_charge = it2->first;
     }
   }
 }

}
if(tmp_size > my_mult) {
       if(it3->first == (my_idx+1))//if particle is daughter of the previous one
         break;
       my_idx = it->first;
       my_charge = it2->first;
       my_idpa = it3->first;
       my_mult=tmp_size;
      }
      }
    }
  }


  idx = my_idx;
  charge = my_charge;

  return;

}

void GlobalRecoAna::SetMCtruthBIS(map<int, map<int, map<int,vector<int>>>> mapall, int &idx, int &charge){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::SetMCtruthBIS start"<<endl;

  Int_t my_mult(-1);
  Int_t my_charge(-999);
  Int_t my_idx(-999);
  Int_t my_idpa(-999);

  Int_t tmp_size = 0;

  for(map<int,map<int,map<int,vector<int>>>>::iterator it=mapall.begin(); it!=mapall.end(); it++){
    for(map<int,map<int,vector<int>>>::iterator it2=it->second.begin(); it2!=it->second.end(); it2++){
      for(map<int,vector<int>>::iterator it3=it2->second.begin(); it3!=it2->second.end(); it3++){
tmp_size = it3->second.size();
      if(tmp_size > my_mult) {
       my_idx = it->first;
       my_charge = it2->first;
       my_idpa = it3->first;
       my_mult=tmp_size;
      }
      }
    }
  }

  idx = my_idx;
  charge = my_charge;

  return;
}

void GlobalRecoAna::ComputeMCtruth(Int_t trkid, int &cha, TVector3 &mom, TVector3 &mom_cross, double &ek){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::ComputeMCtruth start"<<endl;

  cha = -999, mom.SetXYZ(-999.,-999.,-999.), ek = -999.,  mom_cross.SetXYZ(-999.,-999.,-999.);

  if(trkid !=-1){
    TAMCpart *pNtuMcTrk = GetNtuMcTrk()->GetTrack(trkid);

    cha = pNtuMcTrk->GetCharge();
    mom = pNtuMcTrk->GetInitP();//also MS contribution in target!
    //  mom*=fpFootGeo->GevToMev();
    ek = (sqrt(pow(pNtuMcTrk->GetMass(),2) + pow((pNtuMcTrk->GetInitP()).Mag(),2)) - pNtuMcTrk->GetMass())/pNtuMcTrk->GetMass();
    //  ek*=fpFootGeo->GevToMev();
  }

  if(TAGrecoManager::GetPar()->IsRegionMc()){
    for(int icr = 0;icr<GetNtuMcReg()->GetRegionsN();icr++){
      TAMCregion *fpNtuMcReg = GetNtuMcReg()->GetRegion(icr);
      if((fpNtuMcReg->GetTrackIdx()-1)==trkid){
        if(fpNtuMcReg->GetCrossN()==GetParGeoG()->GetRegAirPreTW() && fpNtuMcReg->GetOldCrossN()==GetParGeoG()->GetRegTarget()){
          mom_cross = fpNtuMcReg->GetMomentum();
        }
      }
    }
  }

  if(FootDebugLevel(1))
    cout<<"TRUE MC info:: Z = "<<cha<<"; |P| = "<<mom.Mag()<<"; Ek = "<<ek<<"; |Pcross| = "<<mom_cross.Mag()<<endl;

  return;
}

Double_t GlobalRecoAna::ComputeTrkEkin(TAGtrack *fGlbTrack){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::ComputeTrkEkin start"<<endl;

  //Double_t Ek_meas = fGlbTrack->GetCalEnergy()/atomassu;
  //Double_t Ek_meas = -1.;
  Double_t Ek_meas = 0.;

  Double_t ZCalo = fpFootGeo->GetCACenter().Z()-GetParGeoCa()->GetCrystalThick();
  if(FootDebugLevel(1))
    cout<<"Zcalo = "<<ZCalo<<" "<<fpFootGeo->GetCACenter().Z()<<" "<<GetParGeoCa()->GetCrystalThick()<<endl;

  TAGtrack::PolynomialFit_t trk_param = fGlbTrack->GetParameters();

  Double_t Yint_Calo = trk_param.parameter_y[1] * ZCalo + trk_param.parameter_y[0];
  Double_t Xint_Calo = trk_param.parameter_x[3] * pow(ZCalo,3) + trk_param.parameter_x[2] * pow(ZCalo,2) + trk_param.parameter_x[1] * ZCalo + trk_param.parameter_x[0];

  TVector3 Rint(0.,0.,0.), eRint(0.,0.,0.);
  Double_t distmin = 999.;
  Rint.SetXYZ(Xint_Calo,Yint_Calo,ZCalo);
  eRint.SetXYZ(2.*sqrt(2)*2.,2.*sqrt(2)*2.,2.);//test 0 : diagonale 2 cristalli + 2 cm in z (~ from minZ:maxZ calo)


  for(int ica = 0; ica< pCaNtuClu->GetClustersN();ica++){

TACAcluster *myCAclu = pCaNtuClu->GetCluster(ica);
TVector3 caclu_local = myCAclu->GetPosition();
TVector3 caclu_pos = fpFootGeo->FromCALocalToGlobal(caclu_local);
TVector3 caclu_epos = myCAclu->GetPosError();

// for(int ih = 0; ih < myCAclu->GetHitsN();ih++){

//  TACAhit *myCAhit = myCAclu->GetHit(ih);

//  TVector3 cahit_local = myCAhit->GetPosition();
//  TVector3 cahit_pos = fpFootGeo->FromCALocalToGlobal(cahit_local);
 //  TVector3 cahit_epos = myCAhit->GetPosError();//non esiste. va implementata
//  TVector3 cahit_epos(0.,0.,0.);
//  cahit_epos.SetXYZ(1.,1.,1.);//half of crystal side

 if(FootDebugLevel(1)){
   cout<<"Rint hit pos = "<<Rint.X()<<" "<<Rint.Y()<<" "<<Rint.Z()<<endl;
   //  cout<<"calo hit local = "<<cahit_local.X()<<" "<<cahit_local.Y()<<" "<<cahit_local.Z()<<endl;
   cout<<"calo clus pos = "<<caclu_pos.X()<<" "<<caclu_pos.Y()<<" "<<caclu_pos.Z()<<endl;
   //    cout<<"calo hit pos = "<<cahit_pos.X()<<" "<<cahit_pos.Y()<<" "<<cahit_pos.Z()<<endl;
 //  cout<<"calo hitepos = "<<cahit_epos.X()<<" "<<cahit_epos.Y()<<" "<<cahit_epos.Z()<<endl;
 }
 //  if( (Rint-caclu_pos).Mag() <= (eRint).Mag() ){
 if( (Rint-caclu_pos).Mag() <= distmin && distmin <= caclu_epos.Mag()){
   if(FootDebugLevel(1))
     //      cout<<"IN hit "<<cahit_pos.X()<<" "<<cahit_pos.Y()<<" "<<cahit_pos.Z()<<endl;
     cout<<"IN clu "<<caclu_pos.X()<<" "<<caclu_pos.Y()<<" "<<caclu_pos.Z()<<endl;
   //sum of the energy in each crystal (hit) within a certain distance from the glb track extrapolation to the calo
   //    Ek_meas += (myCAhit->GetCharge()*fpFootGeo->MevToGev());
   //    Ek_meas += (myCAclu->GetEnergy()*fpFootGeo->MevToGev());
   distmin = (Rint-caclu_pos).Mag();
   Ek_meas = (myCAclu->GetEnergy()*fpFootGeo->MevToGev());

 }

 ((TH1D*)gDirectory->Get("CALO/distminMag"))->Fill(distmin<999 ? distmin:-0.5);
 ((TH1D*)gDirectory->Get("CALO/DistX_trkclu"))->Fill(Rint.X()-caclu_pos.X());
 ((TH1D*)gDirectory->Get("CALO/DistY_trkclu"))->Fill(Rint.Y()-caclu_pos.Y());
 ((TH1D*)gDirectory->Get("CALO/DistZ_trkclu"))->Fill(Rint.Z()-caclu_pos.Z());

 //      }close loop on Nhits

      }//close loop on n clusters

      return Ek_meas;//Energy in GeV

}

void GlobalRecoAna::PrintNCharge(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::PrintNCharge start"<<endl;

  cout<<"-----------NchargeOK/NchargeALL-----------"<<endl;
  if(nchargeall_vt)
    cout<<"VT pix:: "<<((double)nchargeok_vt/(double)nchargeall_vt)*100.<<endl;
  if(nchargeall_vt_clu){
    cout<<"VT clu:: "<<((double)nchargeok_vt_clu/(double)nchargeall_vt_clu)*100.<<endl;
    cout<<"VT clu pure :: "<<((double)npure_vt/(double)nchargeall_vt_clu)*100.<<endl;
    cout<<endl;
  }
  if(nchargeall_it)
    cout<<"IT pix :: "<<((double)nchargeok_it/(double)nchargeall_it)*100.<<endl;
  if(nchargeall_it_clu){
    cout<<"IT clu :: "<<((double)nchargeok_it_clu/(double)nchargeall_it_clu)*100.<<endl;
    cout<<"IT clu pure :: "<<((double)npure_it/(double)nchargeall_it_clu)*100.<<endl;
    cout<<endl;
  }
  if(nchargeall_msd)
    cout<<"MSD strip :: "<<((double)nchargeok_msd/(double)nchargeall_msd)*100.<<endl;
  if(nchargeall_msd_clu){
    cout<<"MSD clu :: "<<((double)nchargeok_msd_clu/(double)nchargeall_msd_clu)*100.<<endl;
    cout<<"MSD clu pure :: "<<((double)npure_msd/(double)nchargeall_msd_clu)*100.<<endl;
    cout<<endl;
  }
  if(nchargeall_tw)
    cout<<"TW :: "<<((double)nchargeok_tw/(double)nchargeall_tw)*100.<<endl;
  cout<<endl;
  if(ntracks)
    cout<<"Npure/Ntracks :: "<<((double)npure/(double)ntracks)*100.<<endl<<endl;

  for(Int_t i=1;i<pure_track_xcha.size();i++)
    if(pure_track_xcha.at(i).second)
      cout<<"Npure/Ntracks Z="<<i<<" :: "<<((double)pure_track_xcha.at(i).first/(double)pure_track_xcha.at(i).second)*100.<<endl;

  return;

}

void GlobalRecoAna::FillMassPlots(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::FillMassPlots start"<<endl;

  Int_t Z_meas=fGlbTrack->GetTwChargeZ();
  if(Z_meas<=primary_cha && Z_meas>0){
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/chi2min",Z_meas)))->Fill(mass_ana->GetChiValue());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/almmin",Z_meas)))->Fill(mass_ana->GetAlmValue());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/mom_bestvsmeas",Z_meas)))->Fill((mass_ana->GetMomBest()-mass_ana->GetMomMeas())*TAGgeoTrafo::GevToMev());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/ekin_bestvsmeas",Z_meas)))->Fill((mass_ana->GetEkinBest()-mass_ana->GetEkinMeas())*TAGgeoTrafo::GevToMev());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/tof_bestvsmeas",Z_meas)))->Fill(mass_ana->GetTofBest()-mass_ana->GetTofMeas());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_pb",Z_meas)))->Fill(mass_ana->GetMassPb());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_pberr",Z_meas)))->Fill(mass_ana->GetMassErrPb());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_be",Z_meas)))->Fill(mass_ana->GetMassBe());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_beerr",Z_meas)))->Fill(mass_ana->GetMassErrBe());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_pe",Z_meas)))->Fill(mass_ana->GetMassPe());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_peerr",Z_meas)))->Fill(mass_ana->GetMassErrPe());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_wavg",Z_meas)))->Fill(mass_ana->GetMassWavg());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_wavgerr",Z_meas)))->Fill(mass_ana->GetMassErrWavg());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_chi",Z_meas)))->Fill(mass_ana->GetMassChi());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_chierr",Z_meas)))->Fill(mass_ana->GetMassErrChi());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mom_chi",Z_meas)))->Fill(mass_ana->GetMomChi());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mom_chierr",Z_meas)))->Fill(mass_ana->GetMomErrChi());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Ekin_chi",Z_meas)))->Fill(mass_ana->GetEkinChi());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Ekin_chierr",Z_meas)))->Fill(mass_ana->GetEkinErrChi());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Tof_chi",Z_meas)))->Fill(mass_ana->GetTofChi());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Tof_chierr",Z_meas)))->Fill(mass_ana->GetTofErrChi());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_alm",Z_meas)))->Fill(mass_ana->GetMassAlm());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mass_almerr",Z_meas)))->Fill(mass_ana->GetMassErrAlm());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mom_alm",Z_meas)))->Fill(mass_ana->GetMomAlm());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Mom_almerr",Z_meas)))->Fill(mass_ana->GetMomErrAlm());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Ekin_alm",Z_meas)))->Fill(mass_ana->GetEkinChi());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Ekin_almerr",Z_meas)))->Fill(mass_ana->GetEkinErrAlm());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Tof_alm",Z_meas)))->Fill(mass_ana->GetTofAlm());
    ((TH1D*)gDirectory->Get(Form("MassReco/Zreco%d/Tof_almerr",Z_meas)))->Fill(mass_ana->GetTofErrAlm());
    ((TH1D*)gDirectory->Get("MassReco/InputStatus"))->Fill(mass_ana->GetInputStatus());
    ((TH1D*)gDirectory->Get("MassReco/ChiMassStatus"))->Fill(mass_ana->GetChiFitStatus());
    ((TH1D*)gDirectory->Get("MassReco/AlmMassStatus"))->Fill(mass_ana->GetAlmFitStatus());
  }

  return;
}

void GlobalRecoAna::StudyThetaReso(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::StudyThetaReso start"<<endl;

  TH1D *hres, *mhres;
  TF1 *fgaus = new TF1("fgaus","gaus(0)",-1,1);
  fgaus->SetParameters(1000.,0.,0.5);

  Double_t binwidth = (((TH2D*)gDirectory->Get("MC/Z1/Theta_reso_cross_vs_th"))->GetXaxis())->GetBinWidth(1);
  const Int_t nbinsx = ((TH2D*)gDirectory->Get("MC/Z1/Theta_reso_cross_vs_th"))->GetNbinsX();

  //  cout<<"BIN WIDTH THETA"<<binwidth<<endl;


  Double_t mean[nbinsx], sigma[nbinsx];
  Double_t emean[nbinsx], esigma[nbinsx];

  //vs th true
  Double_t tmp_thcross[nbinsx], tmp_ethcross[nbinsx];
  vector<TGraphErrors*> gr_mea, gr_sig;

  //vs th meas
  Double_t tmp_thm[nbinsx], tmp_ethm[nbinsx];
  vector<TGraphErrors*> grm_mea, grm_sig;

  TCanvas *cc = new TCanvas("cc","",1000.,1000.);
  cc->Divide(10,10);

  for(Int_t iz=1;iz<=primary_cha;iz++){

    for(Int_t ibin = 0; ibin < nbinsx; ibin++){

      mean[ibin] = -999., sigma[ibin] = -999.;
      emean[ibin] = -999., esigma[ibin] = -999.;
      tmp_thcross[ibin] = -999., tmp_ethcross[ibin] = -999.;

      tmp_thm[ibin] = -999., tmp_ethm[ibin] = -999.;

      tmp_thcross[ibin] = (((TH2D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross_vs_th",iz)))->GetXaxis())->GetBinCenter(ibin+1);
      tmp_ethcross[ibin] = binwidth/sqrt(12.);

      tmp_thm[ibin] = (((TH2D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross_vs_thmeas",iz)))->GetXaxis())->GetBinCenter(ibin+1);
      tmp_ethm[ibin] = binwidth/sqrt(12.);

      if( (ibin+2) <= nbinsx ){

      hres = ((TH2D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross_vs_th",iz)))->ProjectionY("",ibin+1, ibin+2,"");
      hres->Fit("fgaus","","",-1,1);

     mean[ibin] = fgaus->GetParameter(1);
      emean[ibin] = fgaus->GetParError(1);
      sigma[ibin] = fgaus->GetParameter(2);
      esigma[ibin] = fgaus->GetParError(2);

     if(hres->GetEntries()<30.){
       mean[ibin] = -999.;
       emean[ibin] = 0.;
       sigma[ibin] = -999.;
       esigma[ibin] = 0.;
      }

      cc->cd(ibin+1);
      hres->Draw();
      cc->Update();

        gDirectory->cd(Form("MC/Z%d/ThetaRes",iz));
      hres->Write(Form("hres_%d",ibin+1));
      gDirectory->cd("../../..");


      }
    }//close loop on nbinsx

    // cc->SaveAs(Form("hres_Z%d.pdf",iz));

    gr_mea.push_back(new TGraphErrors(nbinsx, tmp_thcross, mean, tmp_ethcross, emean));
    gr_sig.push_back(new TGraphErrors(nbinsx, tmp_thcross, sigma, tmp_ethcross, esigma));

    grm_mea.push_back(new TGraphErrors(nbinsx, tmp_thm, mean, tmp_ethm, emean));
    grm_sig.push_back(new TGraphErrors(nbinsx, tmp_thm, sigma, tmp_ethm, esigma));

  }//close loop on Zid

  TMultiGraph *mulmea = new TMultiGraph();
  TLegend* legmea = new TLegend(0.1,0.7,0.48,0.9);

  TMultiGraph *mmulmea = new TMultiGraph();
  TLegend* mlegmea = new TLegend(0.1,0.7,0.48,0.9);

  for(Int_t iz=0;iz<primary_cha;iz++){
    gr_mea[iz]->SetMarkerColor(iz+1);
    mulmea->Add(gr_mea[iz],"pe");
    legmea->AddEntry(gr_mea[iz],Form("Z=%d",iz+1));

    grm_mea[iz]->SetMarkerColor(iz+1);
    mmulmea->Add(grm_mea[iz],"pe");
    legmea->AddEntry(grm_mea[iz],Form("Z=%d",iz+1));
  }

  mulmea->Draw("A");
  legmea->Draw();
  mulmea->Write("restheta_mean");

  mmulmea->Draw("A");
  mlegmea->Draw();
  mmulmea->Write("resthetameas_mean");


  TMultiGraph *mulsig = new TMultiGraph();
  TLegend* legsig = new TLegend(0.1,0.7,0.48,0.9);

  TMultiGraph *mmulsig = new TMultiGraph();
  TLegend* mlegsig = new TLegend(0.1,0.7,0.48,0.9);

  for(Int_t iz=0;iz<primary_cha;iz++){
    gr_sig[iz]->SetMarkerColor(iz+1);
    mulsig->Add(gr_sig[iz],"pe");
    legsig->AddEntry(gr_sig[iz],Form("Z=%d",iz+1));

    grm_sig[iz]->SetMarkerColor(iz+1);
    mmulsig->Add(grm_sig[iz],"pe");
    mlegsig->AddEntry(grm_sig[iz],Form("Z=%d",iz+1));

  }
  mulsig->Draw("A");
  legsig->Draw();
  mulsig->Write("restheta_sigma");

  mmulsig->Draw("A");
  mlegsig->Draw();
  mmulsig->Write("resthetameas_sigma");


  if(FootDebugLevel(1))
    cout<<"StudyThetaReso done"<<endl;

  return;
}

void GlobalRecoAna::BeforeEventLoop(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::BeforeEventLoop start"<<endl;


  ReadParFiles();
  CampaignChecks();
  SetupTree();
  cout <<"fSkipEventsN :" << fSkipEventsN <<endl;
  myReader->Open(GetName(), "READ", "tree");
  if (fSkipEventsN > 0){
       myReader->Reset(fSkipEventsN);
     }

  file_out = new TFile(GetTitle(),"RECREATE");

  cout<<"Going to create "<<GetTitle()<<" outfile "<<endl;
//  SetRunNumber(runNb); //serve veramente?
  //  myReader->GetTree()->Print();

  //initialization of several objects needed for the analysis
  gTAGroot->BeginEventLoop();
  mass_ana=new GlobalRecoMassAna();
  myGlb = (TAGntuGlbTrack*)fpNtuGlbTrack->GenerateObject();
  //myVtNtuClus = (TAVTntuCluster*)fpNtuClusVtx->GenerateObject();
  myVtNtuVtx = (TAVTntuVertex*)fpNtuVtx->GenerateObject();

  myTWNtuPt = (TATWntuPoint*)fpNtuRecTw->GenerateObject();
  myMSDNtuHit = (TAMSDntuHit*)fpNtuRecTw->GenerateObject();
  pCaNtuClu = (TACAntuCluster*)fpNtuClusCa->GenerateObject();
  myBMNtuTrk = (TABMntuTrack*) fpNtuTrackBm->GenerateObject();

  if(fFlagMC){
    myMcNtuEvent = (TAMCntuEvent*)fpNtuMcEvt->GenerateObject();
    myMcNtuPart = (TAMCntuPart*)fpNtuMcTrk->GenerateObject();
  }else{
    wdTrig = (TAWDntuTrigger*)fpNtuWDtrigInfo->GenerateObject();
  }


  //set variables
  primary_cha=GetParGeoG()->GetBeamPar().AtomicNumber;
  Double_t beam_mass_number=GetParGeoG()->GetBeamPar().AtomicMass*TAGgeoTrafo::GetMassFactorMeV(); //primary mass number in mev
  Double_t beam_energy=GetParGeoG()->GetBeamPar().Energy*GetParGeoG()->GetBeamPar().AtomicMass*TAGgeoTrafo::GevToMev(); //Total kinetic energy (MeV)
  Double_t beam_speed = sqrt( beam_energy*beam_energy + 2.*beam_mass_number*beam_energy )/(beam_mass_number + beam_energy)*TAGgeoTrafo::GetLightVelocity(); //cm/ns
  primary_tof=(fpFootGeo->GetTGCenter().Z()-fpFootGeo->GetSTCenter().Z())/beam_speed; //ns
  Booking();
  pure_track_xcha.clear();
  pure_track_xcha.resize(primary_cha+1,std::make_pair(0,0));
  Ntg=GetParGeoG()->GetTargetPar().Density*TMath::Na()*GetParGeoG()->GetTargetPar().Size.Z()/GetParGeoG()->GetTargetPar().AtomicMass;

  if(FootDebugLevel(1)) {
    cout<<"primary_cha="<<primary_cha<<"  beam_mass_number="<<beam_mass_number<<"  beam_energy="<<beam_energy<<"  beam_speed="<<beam_speed<<"  primary_tof="<<primary_tof<<endl;
    cout<<"N_target="<< Ntg << endl;
    cout <<"target density="<< GetParGeoG()->GetTargetPar().Density << endl;
    cout << "target z=" << GetParGeoG()->GetTargetPar().Size.Z() << endl;
    cout << "target A=" << GetParGeoG()->GetTargetPar().AtomicMass << endl;
  }

   //test file for TrackVsMCStudy()
   TString file_name = "/home/FOOT-T3/gubaldifoott3/SHOE/foot0y_anal/results_400/check.txt";
   myfile.open(file_name);
  return;
}

void GlobalRecoAna::AfterEventLoop(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::AfterEventLoop start"<<endl;

  //stamp luminosity
  string luminosity_name="";
    if (fFlagMC == true){
      luminosity_name = "luminosityMC";
    }else {  // real data
      luminosity_name = "luminosityREAL";
    }
    h = new TH1D(luminosity_name.c_str(), "", 1, 0., 1.);
    ((TH1D *)gDirectory->Get(luminosity_name.c_str()))->SetBinContent(1, Ntg * recoEvents);
    cout << "Reconstructed events: " << recoEvents << endl;

    // study efficiency of MB trigger
    if (fFlagMC == false)
    {
      //((TH1D*)gDirectory->Get("fragTriggerStudies/chargeMB"))
      //((TH1D*)gDirectory->Get("fragTriggerStudies/chargeMBFrag"))
      // TH1D *newtrk=((TH1D*)trkplt->Clone("newtrk"));
      ((TH1D *)gDirectory->Get("fragTriggerStudies/chargeMBFrag_efficiency"))->Divide(((TH1D *)gDirectory->Get("fragTriggerStudies/chargeMB")));
      ((TH1D *)gDirectory->Get("fragTriggerStudies/chargeMBFrag_RejectPower"))->SetBinContent(1, (((TH1D *)gDirectory->Get("fragTriggerStudies/chargeMBFrag"))->GetEntries() / ((TH1D *)gDirectory->Get("fragTriggerStudies/chargeMB"))->GetEntries()));
  }


  gTAGroot->EndEventLoop();

  if(fFlagMC)
    //StudyThetaReso(); //to be fixed!!!
  //PrintNCharge();

  cout <<"Writing..." << endl;
  file_out->Write();

  cout <<"Closing..." << endl;
  file_out->Close();

  return;
}

bool GlobalRecoAna::TriggerCheck() {

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::TriggerCheck start"<<endl;

  //cout <<"TRACK CHARGE: " << fGlbTrack->GetTwChargeZ()<< endl;

  for(int ic=0;ic<fGlbTrack->GetPointsN();ic++) {     // from all the points of the track...
          TAGpoint *tmp_poi = fGlbTrack->GetPoint(ic);
          TString str = tmp_poi->GetDevName();
          Int_t cluID = -1;

          if(str.Contains(TATWparGeo::GetBaseName())){  // ... I take the TWPoint
          TATWpoint *tw_point = GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx());

          if ( ( tw_point->GetRowID() == 8 || tw_point->GetRowID() == 9 || tw_point->GetRowID() == 10 )   //VETO Condition in TW Plane X
              && ( tw_point->GetColumnID() == 8 || tw_point->GetColumnID() == 9 || tw_point->GetColumnID() == 10 ) //VETO Condition in TW Plane Y
                && (tw_point->GetEnergyLoss()> 108.) ) { //Threshold energy condition N.B.: THIS IS FOR O_16 AT 400 MeV/n
                  //cout << "Trigger false " <<endl;
                  //cout << "Row ID: " << tw_point->GetRowID()<<endl;
                  //cout << "Column ID: " << tw_point->GetColumnID()<<endl;
                  //cout << "energy loss: "<< tw_point->GetEnergyLoss()<< endl<<endl;

                  return false;

                } else {
                  //cout << "Trigger true " <<endl;
                  //cout << "Row ID: " << tw_point->GetRowID()<<endl;
                  //cout << "Column ID: " << tw_point->GetColumnID()<<endl;
                  //cout << "energy loss: "<< tw_point->GetEnergyLoss()<< endl<<endl;

                  return true;
                }



          }

  }
  return false;

}

bool GlobalRecoAna::TriggerCheckMC() {

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::TriggerCheckMC start"<<endl;

  //cout <<"TRACK CHARGE: " << fGlbTrack->GetTwChargeZ()<< endl;
  int TrkIdMC = fGlbTrack->GetMcMainTrackId();

  int nTWpoints = myTWNtuPt->GetPointsN();
          //cout << nTWpoints <<endl;
          for(int ipoint=0; ipoint<nTWpoints; ipoint++) {       // for all the twpoints of an event
            TATWpoint *tw_point = myTWNtuPt->GetPoint(ipoint);

              if (tw_point->GetPointMatchMCtrkID() == TrkIdMC) { // i want the one whose MCId is matched with MCId of the track

          if ( ( tw_point->GetRowID() == 8 || tw_point->GetRowID() == 9 || tw_point->GetRowID() == 10 )   //VETO Condition in TW Plane X
              && ( tw_point->GetColumnID() == 8 || tw_point->GetColumnID() == 9 || tw_point->GetColumnID() == 10 ) //VETO Condition in TW Plane Y
                && (tw_point->GetEnergyLoss()> 108.) ) { //Threshold energy condition N.B.: THIS IS FOR O_16 AT 400 MeV/n
                  //cout << "Trigger false " <<endl;
                  //cout << "Row ID: " << tw_point->GetRowID()<<endl;
                  //cout << "Column ID: " << tw_point->GetColumnID()<<endl;
                  //cout << "energy loss: "<< tw_point->GetEnergyLoss()<< endl<<endl;

                  return false;

                } else {
                  //cout << "Trigger true " <<endl;
                  //cout << "Row ID: " << tw_point->GetRowID()<<endl;
                  //cout << "Column ID: " << tw_point->GetColumnID()<<endl;
                  //cout << "energy loss: "<< tw_point->GetEnergyLoss()<< endl<<endl;

                  return true;
                }



          }

  }
  return false;

}

void GlobalRecoAna::GlbTrackPurityStudy(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::GlbTrackPurityStudy start"<<endl;

  Int_t tmp_size = 0;
  Int_t TrkTmpIdMC(-1);
  Int_t my_mult(-1);
  bool pure_trk = false;
  bool clean_trk = false;
  //loop on meas points of a glb track

  if(FootDebugLevel(1))
  cout<<"===NEW TRACK with "<<fGlbTrack->GetPointsN()<<" points =="<<endl;

  map<int, int> mapclu;
  mapclu.clear();
  fGlbTrkVec.clear();

  vector<Int_t> vtxpoints, itpoints, msdpoints, twpoints, calpoints; // in principle twpoints and calpoints should be 0 or 1

  for(int ic=0;ic<fGlbTrack->GetPointsN();ic++) {   //for every point of the track
    TAGpoint *tmp_poi = fGlbTrack->GetPoint(ic);
    TString str = tmp_poi->GetDevName();
    Int_t cluID = -1;

    if(str.Contains(TAVTparGeo::GetBaseName())){//vtx
      VTanal(tmp_poi, cluID);
      if(tmp_poi->GetClusterIdx()>=0)
        vtxpoints.push_back(ic);
      if(cluID!=-1)
        mapclu[cluID]++;
    }
    if(str.Contains(TAITparGeo::GetBaseName())){//it
      ITanal(tmp_poi, cluID);
      if(tmp_poi->GetClusterIdx()>=0)
        itpoints.push_back(ic);
      if(cluID!=-1)
        mapclu[cluID]++;
    }

    if(str.Contains(TAMSDparGeo::GetBaseName())){//msd
      MSDanal(tmp_poi, cluID);
      if(tmp_poi->GetClusterIdx()>=0)
        msdpoints.push_back(ic);
      if(cluID!=-1)
        mapclu[cluID]++;
    }

    if(str.Contains(TATWparGeo::GetBaseName())){//tw
      TWanal(tmp_poi, cluID);
      if(tmp_poi->GetClusterIdx()>=0){
        twpoints.push_back(ic);
        calpoints.push_back(max(GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx())->GetMatchCalIdx(),-1));
        if(GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx())->GetMatchCalIdx()>=0){
          CALOanal(GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx())->GetMatchCalIdx());
        }
      }

      if(cluID!=-1)
        mapclu[cluID]++;
    }
      // if(str.Contains(TACAparGeo::GetBaseName()))// actually the calorimeter is not used by the trackers!
      //   CALOanal(tmp_poi,cluID);
  }//end loop on meas points
  fGlbTrkVec.push_back(vtxpoints);
  fGlbTrkVec.push_back(itpoints);
  fGlbTrkVec.push_back(msdpoints);
  fGlbTrkVec.push_back(twpoints);
  fGlbTrkVec.push_back(calpoints);

  if(FootDebugLevel(1))
    cout<<"loop on global track hit done"<<endl;


  for(map<int,int>::iterator it=mapclu.begin(); it!=mapclu.end(); it++){
    if(FootDebugLevel(1))
      cout<<mapclu.size()<<" :: MAPtrk ["<<it->first<<"]["<<it->second<<"]"<<endl;
    tmp_size = it->second;
    if(tmp_size > my_mult){    // track id is the mcId of the most frequent TAGpoint in the track
      TrkTmpIdMC = it->first;
      my_mult = tmp_size;
    }
  }//close loop on mapclu


  if(fFlagMC && TrkTmpIdMC>=0){
    if(((Double_t)mapclu.at(TrkTmpIdMC))/fGlbTrack->GetPointsN()>=purity_cut){
      pure_trk = true;
      npure++;
    }

    if(((Double_t)mapclu.at(TrkTmpIdMC))/fGlbTrack->GetPointsN()==clean_cut){//100% pure track
      clean_trk = true;
      nclean++;
    }
    if(FootDebugLevel(1))
      cout<<"IDX TRK = "<<TrkTmpIdMC<<" "<<endl;
  }

   // if(TrkTmpIdMC != )//to do ::: check if our trk index is different than the true (MC) one assigned by toe/genfit

   // plots concerning track purity
  if(fFlagMC && Z_true>0 && Z_true<=primary_cha && TrkTmpIdMC>=0 && fGlbTrack->GetPointsN()>0)
    ((TH1D*)gDirectory->Get(Form("Zrec%d/Track_purity",Z_true)))->Fill(((Double_t)mapclu.at(TrkTmpIdMC))/fGlbTrack->GetPointsN());
  if(Z_meas>0 && Z_meas<=primary_cha){
    if(fFlagMC && clean_trk){//do it only for tracks made by clusters of same idx
      ((TH1D*)gDirectory->Get(Form("MC/Z%d/ChargeZ_reso",Z_meas)))->Fill(Z_meas - Z_true);
      ((TH1D*)gDirectory->Get(Form("MC/Z%d/Mass_reso",Z_meas)))->Fill(M_meas - M_true/TAGgeoTrafo::AmuToGev());
      ((TH1D*)gDirectory->Get(Form("MC/Z%d/Mom_reso",Z_meas)))->Fill((P_meas - P_true.Mag())*fpFootGeo->GevToMev());
      ((TH1D*)gDirectory->Get(Form("MC/Z%d/Ekin_reso",Z_meas)))->Fill((Ek_meas - Ek_true)*fpFootGeo->GevToMev());
      ((TH1D*)gDirectory->Get(Form("MC/Z%d/Ekin_reso_cross",Z_meas)))->Fill((Ek_meas - Ek_cross_calo)*fpFootGeo->GevToMev());
      ((TH1D*)gDirectory->Get(Form("MC/Z%d/Ekin_reso",Z_meas)))->Fill((Ek_meas - Ek_true)*fpFootGeo->GevToMev());
      if(fGlbTrkVec.at(4).size()==1)
        if(fGlbTrkVec.at(4).at(0)>=0)
          if(GetNtuClusterCa()->GetCluster(fGlbTrkVec.at(4).at(0))->GetHitsN()==1){//only 1 crystal fired
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Ekin_calo1cls_reso",Z_meas)))->Fill((Ek_meas - Ek_true)*fpFootGeo->GevToMev());
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Ekin_calo1cls_reso_cross",Z_meas)))->Fill((Ek_meas - Ek_cross_calo)*fpFootGeo->GevToMev());

            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Theta_calo1cls_reso",Z_meas)))->Fill(Th_meas - Th_true);
            ((TH1D*)gDirectory->Get(Form("MC/Z%d/Theta_calo1cls_reso_cross",Z_meas)))->Fill(Th_meas - Th_cross);
          }
      ((TH1D*)gDirectory->Get(Form("MC/Z%d/Theta_reso",Z_meas)))->Fill(Th_meas - Th_true);
      ((TH1D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross",Z_meas)))->Fill(Th_meas - Th_cross);
      ((TH1D*)gDirectory->Get(Form("MC/Z%d/Theta_true_cross_vs_meas",Z_meas)))->Fill(Th_meas, Th_cross);
      ((TH2D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross_vs_th",Z_meas)))->Fill(Th_cross, Th_meas - Th_cross);
      ((TH2D*)gDirectory->Get(Form("MC/Z%d/Theta_reso_cross_vs_thmeas",Z_meas)))->Fill(Th_meas, Th_meas - Th_cross);
      ((TH1D*)gDirectory->Get(Form("MC/Z%d/Tof_reso",Z_meas)))->Fill(Tof_meas - Tof_true);
      if(TAGrecoManager::GetPar()->IsRegionMc())
        ((TH1D*)gDirectory->Get(Form("MC/Z%d/Mom_reso_cross",Z_meas)))->Fill((P_meas - P_cross.Mag())*fpFootGeo->GevToMev());
      if(Z_true>0 && Z_true<=primary_cha){
        ((TH1D*)gDirectory->Get(Form("Zrec%d/Mass_clean",Z_meas)))->Fill(M_meas);
        ((TH1D*)gDirectory->Get(Form("Zrec%d/Zmc%d/Mass_clean",Z_meas, Z_true)))->Fill(M_meas);
      }
    }
    if(pure_trk){
      pure_track_xcha.at(Z_meas).first++;
      ((TH1D*)gDirectory->Get(Form("Zrec%d/Mass_pure",Z_meas)))->Fill(M_meas);
      if(Z_true>0 && Z_true<=primary_cha)
        ((TH1D*)gDirectory->Get(Form("Zrec%d/Zmc%d/Mass_pure",Z_meas, Z_true)))->Fill(M_meas);
    }else{
      ((TH1D*)gDirectory->Get(Form("Zrec%d/Mass_impure",Z_meas)))->Fill(M_meas);
      if(mapclu.size()==2)
        ((TH1D*)gDirectory->Get(Form("Zrec%d/Mass_impure_2",Z_meas)))->Fill(M_meas);
      if(Z_true>0 && Z_true<=primary_cha)
        ((TH1D*)gDirectory->Get(Form("Zrec%d/Zmc%d/Mass_impure",Z_meas, Z_true)))->Fill(M_meas);
    }
    pure_track_xcha.at(Z_meas).second++;
  }



      /*
      if(fGlbTrkVec.at(4).size()!=fGlbTrkVec.at(3).size() || fGlbTrkVec.at(3).size()>1){ //!
        cout<<"WARNING in global reco:: the number of tw or calo hits associated to a global track is not the expected one:"<<endl;
        cout<<"number of associated twhits="<<fGlbTrkVec.at(3).size()<<"; number of associated calo hits:"<<fGlbTrkVec.at(4).size()<<endl;
        cout<<"event number="<<nTotEv<<"  global track:"<<it<<endl;
      }*/
return;

}

void GlobalRecoAna::AlignmentStudy(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::AlignmentStudy start"<<endl;

  //-------------- STUDY OF VERTEX AND TW ALLIGNMENT / ROTATIONS
    TAVTntuVertex *vertexContainer = (TAVTntuVertex*)fpNtuVtx->GenerateObject();
    int vertexNumber = vertexContainer->GetVertexN();
    TAVTvertex* vtxPD   = 0x0; //NEW
    int TWpointsNumber = myTWNtuPt->GetPointsN();
    //cout << nTWpoints <<endl;

    if (fFlagMC == false) {
    //cout << "event: "<< currEvent << " -- n tracks: "<< nt <<" -- n tracklets of vertex: "<< vertexNumber << endl;

    //STUDY OF VERTEX
    for (Int_t iVtx = 0; iVtx < vertexNumber; ++iVtx) { // for every vertexEvent
        vtxPD = vertexContainer->GetVertex(iVtx);
        /*if (vtxPD == 0x0){
        cout << "Vertex number " << iVtx << " seems to be empty\n";
        continue;
      }
      else if( !m_IsMC && !vtxPD->IsBmMatched() )
      {
        if(m_debug > 0)
        {
          Info("CategorizeVT()", "In event %d: vertex %d found but not matched with BM tracks; Skipping...",gTAGroot->CurrentEventId().EventNumber(), iVtx);
        }
        continue;
      }*/

            //study of vertex tracklets: phi, theta, projection to TW
            for (int iTrack = 0; iTrack < vtxPD->GetTracksN(); iTrack++) {  //for every tracklet


                  TAVTtrack* tracklet = vtxPD->GetTrack( iTrack );
                  TVector3 direction = (tracklet->GetSlopeZ()).Unit();

                  double theta_vtx = direction.Theta()*TMath::RadToDeg();
                  double phi_vtx = direction.Phi()*TMath::RadToDeg();

                  //cout<< "theta vertex: " <<theta_vtx <<endl;

                  TVector3 direction_glb = fpFootGeo->VecFromVTLocalToGlobal(direction);
                  double phi_vtx_glb = direction_glb.Phi()*TMath::RadToDeg();

                  //projection of a tracklet on TW

                  Float_t posZtw = fpFootGeo->FromTWLocalToGlobal(TVector3(0,0,0)).Z();
                  posZtw = fpFootGeo->FromGlobalToVTLocal(TVector3(0, 0, posZtw)).Z();
                  TVector3 A3 = tracklet->Intersection(posZtw);
                  TVector3 A4 = fpFootGeo->FromVTLocalToGlobal(A3);
                  Float_t VTTWX = A4.X();
                  Float_t VTTWY = A4.Y(); //questi sono in coordinate globali



                  if (isPrimaryInEvent == false /*&& nt>0*/ && wdTrig -> GetTriggersStatus()[1] == 1  ) { // if it is a fragment not oxygen
                  ((TH2D*)gDirectory->Get("vt_twProjection_frag")) -> Fill(VTTWX,VTTWY);
                  ((TH1D*)gDirectory->Get("phi_VTX_global_frag")) -> Fill( phi_vtx_glb);
                  ((TH1D*)gDirectory->Get("theta_VTX_frag")) -> Fill( theta_vtx);
                  ((TH1D*)gDirectory->Get("phi_VTX_frag")) -> Fill( phi_vtx);
                  vertex_direction_frag += direction_glb;
                  } else if (wdTrig -> GetTriggerID() == 40) {
                  ((TH2D*)gDirectory->Get("vt_twProjection")) -> Fill(VTTWX,VTTWY);
                  ((TH1D*)gDirectory->Get("phi_VTX_global")) -> Fill( phi_vtx_glb);
                  ((TH1D*)gDirectory->Get("theta_VTX")) -> Fill( theta_vtx);
                  ((TH1D*)gDirectory->Get("phi_VTX")) -> Fill( phi_vtx);
                  vertex_direction += direction_glb;
                  }
            }
    }

    //STUDY OF TW POINTS spatial distribution
    for(int ipoint=0; ipoint<TWpointsNumber; ipoint++) { // for every twpoint
      TATWpoint *twp = myTWNtuPt->GetPoint(ipoint);
      //Float_t tw_x = fpFootGeo->FromTWLocalToGlobal((twp->GetRowHit())->GetPosition()); // global frame
      //Float_t tw_y = fpFootGeo->FromTWLocalToGlobal((twp->GetColumnHit())->GetPosition());

      if (isPrimaryInEvent == false && myGlb->GetTracksN()>0 && wdTrig -> GetTriggersStatus()[1] == 1  ) { // if it is a fragment not oxygen
      ((TH2D*)gDirectory->Get("TWpointsDistribution_frag")) -> Fill(twp->GetPositionGlb().X(),twp->GetPositionGlb().Y()); //global frame

      } else if (wdTrig -> GetTriggerID() == 40) {
        //int Z_tw_reco = twp-> GetChargeZ();
        //TAMCpart *pNtuMcTrk_ = GetNtuMcTrk()->GetTrack(twp->GetPointMatchMCtrkID());
        //int Z_tw_MC = pNtuMcTrk_ ->GetCharge();

        //((TH2D*)gDirectory->Get("Z_tw_Mixing_matrix"))->Fill(Z_tw_reco,Z_tw_MC);

        ((TH2D*)gDirectory->Get("TWpointsDistribution")) -> Fill(twp->GetPositionGlb().X(),twp->GetPositionGlb().Y()); //global frame
      }

    }


    //stamp direction of every vertex object
    ((TH2D*)gDirectory->Get("trackletdirection_frag")) -> Fill(vertex_direction_frag.X(),vertex_direction_frag.Y());
    //cout << "fragment vtx direction: X= " << vertex_direction_frag.X() << " Y= " << vertex_direction_frag.Y() << " theta = " << vertex_direction_frag.Theta()*180/TMath::Pi() << " phi = " << vertex_direction_frag.Phi()*180/TMath::Pi() << endl;

    ((TH2D*)gDirectory->Get("trackletdirection")) -> Fill(vertex_direction.X(),vertex_direction.Y());
    //cout << "beam vtx direction: X= " << vertex_direction.X() << " Y= " << vertex_direction.Y() << " theta = " << vertex_direction.Theta()*180/TMath::Pi() << " phi = " << vertex_direction.Phi()*180/TMath::Pi() << endl;

  }
  //--------------END STUDY OF VERTEX AND TW ALLIGNMENT / ROTATIONS
}

void GlobalRecoAna::TWAlgoStudy(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::TWAlgoStudy start"<<endl;

//------------- STUDY OF TW CHARGE RECONSTRUCTION ALGORITHM
  //----- check for TW Ghost Hits in MC particles   (N.B.: it concerns all TW Points, different from the previous study of points in global tracks)
  int nTWpoints = myTWNtuPt->GetPointsN();
  //cout << nTWpoints <<endl;
  for(int ipoint=0; ipoint<nTWpoints; ipoint++) { // for every TWpoint in an event
    TATWpoint *twp = myTWNtuPt->GetPoint(ipoint);
    int charge = twp->GetChargeZ();
    int ID = twp->GetPointMatchMCtrkID();

    if (ID<0) continue;
    TAMCntuPart* myMcNtuPart = (TAMCntuPart*)fpNtuMcTrk->GenerateObject();
    TAMCpart *mctrk1 = myMcNtuPart->GetTrack(ID);
    double mass = mctrk1->GetMass();
    int moth = mctrk1->GetMotherID();
    TVector3 initPos = mctrk1->GetInitPos();
    TVector3 initMom = mctrk1->GetInitP();
    TVector3 finalPos = mctrk1->GetFinalPos();
    TVector3 finalMom = mctrk1->GetFinalP();
    int reg = mctrk1->GetRegion();
    int Z_MC = mctrk1->GetCharge();
    double time = mctrk1->GetTime();
    double tof = mctrk1->GetTof();
    int type = mctrk1->GetType();
    int baryon = mctrk1->GetBaryon();
    int dead = mctrk1->GetDead();
    double InitPmod = sqrt( pow(initMom(0),2) + pow(initMom(1),2) + pow(initMom(2),2) );
    double Ekin_point = ( sqrt( pow(InitPmod,2) + pow(mass,2)) - mass )/(double)baryon*1000.;
    double CosTheta = initMom(2)/InitPmod;
    double Theta = TMath::ACos(CosTheta)*180./TMath::Pi();
    int indexSize = twp->GetMcTracksN();

    ((TH2D*)gDirectory->Get("Z_tw_Mixing_matrix")) -> Fill(charge,Z_MC); //global frame
    ((TH2D*)gDirectory->Get("devstofAll")) -> Fill(twp->GetMeanTof(),twp->GetEnergyLoss());




    //if(charge > 0 && charge < kCharges+1 && moth == primaryID && reg == TG_region && Ekin_point > 100. && finalPos(2) > 190. && momang <= myangle && initPos.x()<sigma_beamTG && initPos.x()>-sigma_beamTG && initPos.y()<sigma_beamTG && initPos.y()>-sigma_beamTG && Ekin_point <= Ekin_max){
                                                      //NB: 50 IN GSI2021
    if(charge > 0 && charge <9 && moth == 0 && reg == 50 && finalPos(2) > 190.) {   // i want only the particles generated in the target toward the TW

    ((TH2D*)gDirectory->Get("MC_check/Mixing_matrix")) -> Fill(charge, Z_MC);
    ((TH2D*)gDirectory->Get("devstof")) -> Fill(twp->GetMeanTof(),twp->GetEnergyLoss());
    if (indexSize == 1){  // if there is not ghosdt hits
    ((TH2D*)gDirectory->Get("MC_check/Mixing_matrix_cut"))->Fill(charge, Z_MC);
    }
    }

    //----------- study of re-fragments
    else if (moth != 0) {
    ((TH2D*)gDirectory->Get("Z_tw_Mixing_matrix2frag")) -> Fill(charge, Z_MC);
    ((TH2D*)gDirectory->Get("devstof2frag")) -> Fill(twp->GetMeanTof(),twp->GetEnergyLoss());




    }
    else {
      ((TH2D*)gDirectory->Get("Z_tw_Mixing_matrix!")) -> Fill(charge, Z_MC);
      ((TH2D*)gDirectory->Get("devstof!")) -> Fill(twp->GetMeanTof(),twp->GetEnergyLoss());
      //posizione d'origine
      ((TH1D*)gDirectory->Get("originPosition")) -> Fill(reg);



    }
  }
return;
}

void GlobalRecoAna::TrackVsMCStudy(){

if(FootDebugLevel(1))
  cout<<"GlobalRecoAna::TrackVsMCStudy start"<<endl;

N_TrkIdMC_TW =-1;
TrkIdMC_TW = -1;
TrkIdMC = -1;
Int_t Z_meas = -1;
Int_t Z_true = -1;

myfile <<endl<<endl<<"current Event: " <<currEvent ;
myfile<<endl<< "------- track reconstruction "<< endl;
for(int it=0;it<myGlb->GetTracksN();it++){ // for every track
  fGlbTrack = myGlb->GetTrack(it);
  Z_meas = fGlbTrack->GetTwChargeZ();
  myfile<<endl<< "track n° "<< it << endl;


  //----debug: check of TrackID: GetMcMainTrackId() vs GetTrackId() of fGlbTrack
      if ((fGlbTrack->GetMcTrackIdx()).GetSize() > 1){
      myfile<< "track size: "<< (fGlbTrack->GetMcTrackIdx()).GetSize() << endl;
      for (int i =0; i< (fGlbTrack->GetMcTrackIdx()).GetSize(); i++){
      myfile<<"TRACK ID "<<i<<": "<< (fGlbTrack->GetMcTrackIdx())[i] <<endl;
      }
      myfile<<"most probable id: "<< fGlbTrack->GetMcMainTrackId() << endl;
      myfile<<"track id: "<< fGlbTrack->GetTrackId() << endl;
      }

  TrkIdMC = fGlbTrack->GetMcMainTrackId();
  myfile << "  TrkIdMC= "<< TrkIdMC << " --> ";

  if(TrkIdMC !=-1){
  TAMCpart *pNtuMcTrk = GetNtuMcTrk()->GetTrack(TrkIdMC);
  Z_true = pNtuMcTrk -> GetCharge();
  myfile <<" Fluka code: " << pNtuMcTrk->GetFlukaID()<<"("<<pNtuMcTrk->GetCharge()<<")"<< endl<< "  charge TW : "<< fGlbTrack->GetTwChargeZ()<<"  charge Fit : "<<fGlbTrack->GetFitChargeZ() <<endl<<"---------"<<endl;
   //---- testing wrong reconstructed charge in tracks
  if (!(pNtuMcTrk->GetCharge() == fGlbTrack->GetTwChargeZ())) {  //if MCparticle Z is different from track Z
    myfile << "wrong reconstructed charge by TW" <<endl;
  }

  //---- testing TW multiple hits wrt track of TW

        //check del TAGpoint del TW
        for(int ic=0;ic<fGlbTrack->GetPointsN();ic++) { //from all the points of the track...

          TAGpoint *tmp_poi = fGlbTrack->GetPoint(ic);
          TString str = tmp_poi->GetDevName();
          Int_t cluID = -1;

          if(str.Contains(TATWparGeo::GetBaseName())){ //...i just want the TAGPOINT of TW

            N_TrkIdMC_TW = tmp_poi->GetMcTracksN();  // n° of tracks crossing the TW with same MC_ID
            TrkIdMC_TW = tmp_poi->GetMcTrackIdx(0);
            myfile << "TW Point, MC tracks check: TrkIdMC= ";

            for( Int_t i = 0; i < tmp_poi->GetMcTracksN(); ++i) { //I check how many different MC tracks crosses the TW with same MCID
               Int_t trackIdx = tmp_poi->GetMcTrackIdx(i);
               myfile <<trackIdx<<" ";
            }
            myfile << endl;

            if (tmp_poi->GetMcTracksN() == 1){
              if (TrkIdMC == tmp_poi->GetMcTrackIdx(0)) {
                  myfile << " TrkIdMC of track and TW match" << endl;
              } else {
                myfile << " TrkIdMC of track and TW NOT match: " <<"trackId: "<<TrkIdMC << " TWid: "<< tmp_poi->GetMcTrackIdx(0)  <<endl;
                //continue;
              }

            }

            if (tmp_poi->GetMcTracksN() > 1){
                myfile << "ghost hit candidate "<<endl;
                //continue;
            }

            //---- testing TW multiple hits wrt track  of TWPOINT and relative TATWHIT

            myfile << "Inspect of TW Hits: ";
            TATWpoint *tw_point = GetNtuPointTw()->GetPoint(tmp_poi->GetClusterIdx());

            int indexSize = tw_point->GetMcTracksN();
            TATWhit* rowHit = tw_point->GetRowHit();
            TATWhit* colHit = tw_point->GetColumnHit();

            if  (indexSize > 1) {

                myfile <<"----------------------------"<<endl<<"TW GHOST CHECK - McTracks > 1"<<endl;
                for (int iRow = 0; iRow < rowHit->GetMcTracksN(); ++iRow){
                  for (int iCol = 0; iCol < colHit->GetMcTracksN(); ++iCol){
                    if (rowHit->GetMcTrackIdx(iRow) == colHit->GetMcTrackIdx(iCol)){
                    myfile <<"id "<< rowHit->GetMcTrackIdx(iRow) << "matching " << endl;
                    } else {
                      myfile << "row id "<<rowHit->GetMcTrackIdx(iRow)<<" POS: "<<rowHit->GetPosition() << " and column id "<<colHit->GetMcTrackIdx(iCol)<<" POS : "<<colHit->GetPosition() << " not matching"<<endl;
                    }
                  }
                }

            } else   if  (indexSize == 1) {
                myfile <<"----------------------------"<<endl<<"TW GHOST CHECK - McTracks = 1"<<endl;
                myfile << "row id "<<rowHit->GetMcTrackIdx(0)<<" POS: "<<rowHit->GetPosition() << " and column id "<<colHit->GetMcTrackIdx(0)<<" POS : "<<colHit->GetPosition() << " matching"<<endl;
            } else myfile <<endl;

          }

        }


  }
  if (N_TrkIdMC_TW == 1 && TrkIdMC_TW == TrkIdMC) {      //stampa solo se TW point ha id della traccia e non c'è gosh hits
      ((TH2D*)gDirectory->Get("TrkVsMC/Z_truevsZ_reco_TWFixed"))->Fill(Z_true,Z_meas);
  }

  if (TrkIdMC_TW == TrkIdMC) {      //stampa solo se TW point ha id della traccia
      ((TH2D*)gDirectory->Get("TrkVsMC/Z_truevsZ_reco_TrackFixed"))->Fill(Z_true,Z_meas);
  }

  if (N_TrkIdMC_TW == 1) {      //stampa solo se non c'è gosh hits
      ((TH2D*)gDirectory->Get("TrkVsMC/Z_truevsZ_reco_TWGhostHitsRemoved"))->Fill(Z_true,Z_meas);
  }

  ((TH2D*)gDirectory->Get("TrkVsMC/Z_truevsZ_reco_All"))->Fill(Z_true,Z_meas); //stampa senza misreco-cuts

}

//------------------------------  STUDY OF MC PARTICLES
myfile <<endl<< "-----------------  MC study "<< endl;

Int_t n_particles = myMcNtuPart -> GetTracksN();        // n° of particles of an event
for (Int_t i= 0 ; i < n_particles; i++) {                         // for every particle in an event
myfile << endl << "traccia MC: " << i ;

TAMCpart* particle = myMcNtuPart->GetTrack(i);
auto  Mid = particle->GetMotherID();
            double mass = particle->GetMass();             // Get TRpaid-1
            auto Reg = particle->GetRegion();
            auto finalPos = particle-> GetFinalPos();
            int baryon = particle->GetBaryon();
            TVector3 initMom = particle->GetInitP();
            double InitPmod = sqrt( pow(initMom(0),2) + pow(initMom(1),2) + pow(initMom(2),2) );
            Float_t Ek_tr_tot = sqrt( pow(InitPmod,2) + pow(mass,2)) - mass;
            Ek_tr_tot = Ek_tr_tot * fpFootGeo->GevToMev();
            Float_t Ek_true = Ek_tr_tot / (double)baryon;
            Float_t theta_tr = particle->GetInitP().Theta()*(180/TMath::Pi());   // in deg
myfile <<endl <<"  fluka ID: " << particle->GetFlukaID() << "("<<particle->GetCharge()<<")";

//true detectable
//! NB: 50 IN GSI2021_MC
//! NB: 59 IN 16O_400
//! finalPos.Z() > 189.15 IN GSI2021_MC
//! finalPos.Z() > 90 IN 16O_400
if (  Mid==0 && Reg == 50 &&           // if the particle is generated in the target and it is the fragment of a primary
                  particle->GetCharge()>0 && particle->GetCharge()<=8 //&&                       //if Z<8 and A<30, so if it is a fragment (not the primitive projectile, nor detector fragments)
                  && Ek_true>100   //enough energy/n to go beyond the target
                  && theta_tr <= 8.  //  myangle // angular aperture < 8 deg
                  )  {
                      myfile << "  (particle from TG to TW)";
                  }



}
myfile <<endl <<endl;
return;
}

void GlobalRecoAna::FillYieldReco(string folderName, Int_t Z,Int_t Z_meas, Double_t Th, Double_t Ek){
        string path = folderName+"/charge";
        ((TH1D*)gDirectory->Get(path.c_str()))->Fill(Z);
      //((TH1D*)gDirectory->Get(Form("xsec_rec/Z_%d-%d_%d/Theta_meas",Z_meas,Z_meas,(Z_meas+1))))->Fill(Th_meas);
      //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/Ek_meas",Z_meas,Z_meas,(Z_meas+1))))->Fill(Ek_meas*fpFootGeo->GevToMev());
      //((TH2D*)gDirectory->Get(Form("xsec_rec/Z_%d-%d_%d/Theta_vs_Ekin",Z_meas,Z_meas,(Z_meas+1))))->Fill(Ek_meas*fpFootGeo->GevToMev(),Th_meas);



        for (int i = 0; i<th_nbin; i++) {
         if(Th>=theta_binning[i][0] && Th<theta_binning[i][1]){
            //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/theta_%d-%d_%d/Ek_bin",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]))))->Fill(Ek_meas*fpFootGeo->GevToMev());
            //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/theta_%d-%d_%d/Mass_bin",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]))))->Fill(M_meas);

            //if (M_meas <0) cout <<" M MEAS NEGATIVE" << endl;
            //string pathmigz = "xsecrec-trkMC/Z_" + to_string(Z_true) +"-"+to_string(Z_true)+"_"+to_string(Z_true+1)+"/migMatrix";
            //((TH2D*)gDirectory->Get(pathmigz.c_str()))->Fill(Z_true,Z_meas);
            path = folderName+"/Z_" + to_string(Z-1) +"#"+to_string(Z-0.5)+"_"+to_string(Z+0.5)+"/theta_"+to_string(i)+"#"+to_string(theta_binning[i][0])+"_"+to_string(theta_binning[i][1])+"/theta_";
            ((TH1D*)gDirectory->Get(path.c_str()))->Fill(Th);

            string path_matrix = folderName+"/Z_" + to_string(Z-1) +"#"+to_string(Z-0.5)+"_"+to_string(Z+0.5)+"/theta_"+to_string(i)+"#"+to_string(theta_binning[i][0])+"_"+to_string(theta_binning[i][1])+"/migMatrix_Z";
            if (!(Z_meas==0))
            ((TH2D*)gDirectory->Get(path_matrix.c_str()))->Fill(Z,Z_meas);


           /*for (int j=0; j < ek_nbin; j++) {
             if((Ek_meas*fpFootGeo->GevToMev())>=ek_binning[j][0] && (Ek_meas*fpFootGeo->GevToMev())<ek_binning[j][1]) {

                //((TH1D*)gDirectory->Get(Form("xsecrec-/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/Mass_bin_",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]))))->Fill(M_meas);

                  for (int k=0; k < mass_nbin; k++) {
                   if(M_meas>=mass_binning[k][0] && M_meas <mass_binning[k][1]) {

                     ((TH1D*)gDirectory->Get(Form("xsecrec-trkMC/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/A_%d-%d_%d/A_",Z_meas,Z_meas,(Z_meas+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]),k,int(mass_binning[k][0]),int(mass_binning[k][1]))))->Fill(M_meas);



                    }
                  }


              }

            }*/
          }
        //}
        }
}

void GlobalRecoAna::FillYieldMC(string folderName, Int_t charge_tr, Double_t theta_tr, Double_t Ek){

                          /*
                          ((TH1D*)gDirectory->Get("MC_check/Charge_MC_tg")) -> Fill(particle-> GetCharge());
                          ((TH1D*)gDirectory->Get("MC_check/Mass_MC_tg")) -> Fill(particle-> GetMass());
                          ((TH1D*)gDirectory->Get("MC_check/Ek_tot_MC_tg")) -> Fill(Ek_tr_tot);
                          ((TH1D*)gDirectory->Get("MC_check/InitPosZ_MC_tg")) -> Fill(particle-> GetInitPos().Z() );
                          ((TH1D*)gDirectory->Get("MC_check/FinalPosZ_MC_tg")) -> Fill(particle-> GetFinalPos().Z() );
                          ((TH1D*)gDirectory->Get("MC_check/TrkLength_MC_tg")) -> Fill(particle-> GetTrkLength());
                          ((TH1D*)gDirectory->Get("MC_check/Type_MC_tg")) -> Fill(particle-> GetType());
                          ((TH1D*)gDirectory->Get("MC_check/FlukaID_MC_tg")) -> Fill(particle->  GetFlukaID());
                          ((TH1D*)gDirectory->Get("MC_check/MotherID_MC_tg")) -> Fill(particle->  GetMotherID());
                          ((TH1D*)gDirectory->Get("MC_check/Theta_MC_tg")) -> Fill(particle->  GetInitP().Theta()*180./TMath::Pi());

                          */
                          string path = folderName + "/charge";
                          ((TH1D*)gDirectory->Get(path.c_str()))->Fill(charge_tr);

                          for (int i = 0; i<th_nbin; i++) {

                          if(theta_tr>=theta_binning[i][0] && theta_tr<theta_binning[i][1]){

                             path = folderName+"/Z_" + to_string(int(charge_tr)-1) +"#"+to_string(int(charge_tr)-0.5)+"_"+to_string(int(charge_tr)+0.5)+"/theta_"+to_string(i)+"#"+to_string(theta_binning[i][0])+"_"+to_string(theta_binning[i][1])+"/theta_";
                            ((TH1D*)gDirectory->Get(path.c_str()))->Fill(theta_tr);
                            //if ((theta_binning[i][1] <=1) && charge_tr == 6) cout <<"MC : " <<particle->GetFlukaID() << endl;

                            //((TH1D*)gDirectory->Get(Form("xsecrec-true_cut/Z_%d-%d_%d/theta_%d-%d_%d/theta_",int(charge_tr),int(charge_tr),int(charge_tr+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]))))->Fill(theta_tr);
                            /*for (int j=0; j < ek_nbin; j++) {


                              if(Ek_tr_tot >=ek_binning[j][0] && Ek_tr_tot<ek_binning[j][1]) {

                                    for (int k=0; k < mass_nbin; k++) {
                                      Float_t mass_tr = particle -> GetMass();
                                    if(mass_tr>=mass_binning[k][0] && mass_tr <mass_binning[k][1]) {

                                      ((TH1D*)gDirectory->Get(Form("xsecrec-true_cut/Z_%d-%d_%d/theta_%d-%d_%d/Ek_%d-%d_%d/A_%d-%d_%d/A_",int(charge_tr),int(charge_tr),int(charge_tr+1),i,int(theta_binning[i][0]),int(theta_binning[i][1]),j,int(ek_binning[j][0]),int(ek_binning[j][1]),k,int(mass_binning[k][0]),int(mass_binning[k][1]))))->Fill(mass_tr);



                                      }
                                    }


                                }

                              }*/
                            }
                          }


}


void GlobalRecoAna:: BookYield (string path, bool enableMigMatr) {

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::BookYield start"<<endl;

  gDirectory->mkdir(path.c_str());
  gDirectory->cd(path.c_str());
  h = new TH1D("charge","",8, 0.5 ,8.5);

  for(int iz=1; iz<=primary_cha; iz++){
    string name = "Z_" + to_string(iz-1) +"#"+to_string(iz-0.5)+"_"+to_string(iz+0.5);
    gDirectory->mkdir(name.c_str());
    gDirectory->cd(name.c_str());
    name = "";

    for (int i = 0; i<th_nbin; i++) {
      string path = "theta_"+to_string(i)+"#"+to_string(theta_binning[i][0])+"_"+to_string(theta_binning[i][1]);
      gDirectory->mkdir(path.c_str());
      gDirectory->cd(path.c_str());

      h = new TH1D("theta_","",200, 0 ,90.);
      if (enableMigMatr)
      h2 = new TH2D("migMatrix_Z", "migMatrix_Z; Z_{true}; Z_{reco}",8,0.5,8.5,8,0.5,8.5);

      gDirectory->cd("..");
    }
    gDirectory->cd("..");
  }
  gDirectory->cd("..");
}


void GlobalRecoAna::FragTriggerStudies(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::FragTriggerStudies start"<<endl;

  TAWDntuTrigger* wdTrig = 0x0;
  wdTrig = (TAWDntuTrigger*)fpNtuWDtrigInfo->GenerateObject();    //trigger from hardware WD
  //wdTrig -> GetTriggerID() == 40              //MB trigger    ==1 for frag trigger
  //wdTrig -> GetTriggersStatus()[1] == 1      //in MB trigger, even frag

  for(int it=0;it<myGlb->GetTracksN();it++){ // for every track
    fGlbTrack = myGlb->GetTrack(it);
    Int_t Z_meas = fGlbTrack->GetTwChargeZ();
    if (wdTrig -> GetTriggerID() == 40) {  //if MB trigger (Warning! MinimumBias isn't always 40, depending on the campaign!!!)
      ((TH1D*)gDirectory->Get("fragTriggerStudies/chargeMB"))->Fill(Z_meas);

      if (wdTrig -> GetTriggersStatus()[1] == 1) { // if MB trigger with fragmentation
        ((TH1D*)gDirectory->Get("fragTriggerStudies/chargeMBFrag"))->Fill(Z_meas);
        ((TH1D*)gDirectory->Get("fragTriggerStudies/chargeMBFrag_efficiency"))->Fill(Z_meas);

      }

    }


    // Rejection power


  }

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::FragTriggerStudies done"<<endl;

return;
}


void GlobalRecoAna::ClustersPositionStudy(){

//BM track position study
((TH1D*)gDirectory->Get("clusPosition/BM/bm_n_tracks"))->Fill(myBMNtuTrk->GetTracksN());

if (myBMNtuTrk->GetTracksN() == 1){  // select only events with 1 bm track
for( Int_t iTrack = 0; iTrack < myBMNtuTrk->GetTracksN(); ++iTrack ) {
  TABMtrack* track = myBMNtuTrk->GetTrack(iTrack);
  //project to the target in the BM ref., then move to the global ref.
  TVector3 bmlocalproj=ProjectToZ(track->GetSlope(), track->GetOrigin(),fpFootGeo->FromGlobalToBMLocal(fpFootGeo->GetTGCenter()).Z());
  TVector3 bmgloproj=ProjectToZ(fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope()), fpFootGeo->FromBMLocalToGlobal(track->GetOrigin()),fpFootGeo->GetTGCenter().Z());
  ((TH2D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_bmsys"))->Fill(bmlocalproj.X(),bmlocalproj.Y());
  ((TH2D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_glbsys"))->Fill(bmgloproj.X(),bmgloproj.Y());
  ((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_Xpos_glbsys"))->Fill(bmgloproj.X());
  ((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_Ypos_glbsys"))->Fill(bmgloproj.Y());
  ((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_bmsys_theta")) -> Fill( track->GetSlope().Theta()*TMath::RadToDeg());
  ((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_bmsys_phi")) -> Fill( track->GetSlope().Phi()*TMath::RadToDeg());
  ((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_glbsys_theta")) -> Fill( (fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope())).Theta()*TMath::RadToDeg());
  ((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_glbsys_phi")) -> Fill( (fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope())).Phi()*TMath::RadToDeg());

  //study pattern of several quantities wrt events: Spill studies
    ((TH2D*)gDirectory->Get("clusPosition/BM/all-events/spill/bm_posX_vsEvent"))->Fill(currEvent,bmgloproj.X());
    ((TH2D*)gDirectory->Get("clusPosition/BM/all-events/spill/bm_posY_vsEvent"))->Fill(currEvent,bmgloproj.Y());



}


/*
Float_t posZtg = fpFootGeo->FromGlobalToBMLocal(TVector3(0,0,0)).Z();
for( Int_t iTrack = 0; iTrack < myBMNtuTrk->GetTracksN(); ++iTrack ) {

  //bm tracklet projected toward target
  TVector3 A3 = myBMNtuTrk->GetTrack(iTrack)->Intersection(posZtg);    // bm coord in local frame
  TVector3 A4 = fpFootGeo->FromBMLocalToGlobal(A3); //bm coord in global frame
  //BMslope  = fpFootGeo->VecFromBMLocalToGlobal(BMslope);
  ((TH2D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_bmsys_2"))->Fill(A3.X(),A3.Y());
  ((TH2D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_glbsys_2"))->Fill(A4.X(),A4.Y());
}
*/
//**** stop all events

//**** reco events
if (myGlb->GetTracksN() >0){
for( Int_t iTrack = 0; iTrack < myBMNtuTrk->GetTracksN(); ++iTrack ) {
  TABMtrack* track = myBMNtuTrk->GetTrack(iTrack);
  //project to the target in the BM ref., then move to the global ref.
  TVector3 bmlocalproj=ProjectToZ(track->GetSlope(), track->GetOrigin(),fpFootGeo->FromGlobalToBMLocal(fpFootGeo->GetTGCenter()).Z());
  TVector3 bmgloproj=ProjectToZ(fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope()), fpFootGeo->FromBMLocalToGlobal(track->GetOrigin()),fpFootGeo->GetTGCenter().Z());
  ((TH2D*)gDirectory->Get("clusPosition/BM/reco-events/bm_target_bmsys"))->Fill(bmlocalproj.X(),bmlocalproj.Y());
  ((TH2D*)gDirectory->Get("clusPosition/BM/reco-events/bm_target_glbsys"))->Fill(bmgloproj.X(),bmgloproj.Y());
  ((TH1D*)gDirectory->Get("clusPosition/BM/reco-events/bm_target_Xpos_glbsys"))->Fill(bmgloproj.X());
  ((TH1D*)gDirectory->Get("clusPosition/BM/reco-events/bm_target_Ypos_glbsys"))->Fill(bmgloproj.Y());
  ((TH1D*)gDirectory->Get("clusPosition/BM/reco-events/bm_target_bmsys_theta")) -> Fill( track->GetSlope().Theta()*TMath::RadToDeg());
  ((TH1D*)gDirectory->Get("clusPosition/BM/reco-events/bm_target_bmsys_phi")) -> Fill( track->GetSlope().Phi()*TMath::RadToDeg());
  ((TH1D*)gDirectory->Get("clusPosition/BM/reco-events/bm_target_glbsys_theta")) -> Fill( (fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope())).Theta()*TMath::RadToDeg());
  ((TH1D*)gDirectory->Get("clusPosition/BM/reco-events/bm_target_glbsys_phi")) -> Fill( (fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope())).Phi()*TMath::RadToDeg());

  //study pattern of several quantities wrt events: Spill studies
    ((TH2D*)gDirectory->Get("clusPosition/BM/reco-events/spill/bm_posX_vsEvent"))->Fill(currEvent,bmgloproj.X());
    ((TH2D*)gDirectory->Get("clusPosition/BM/reco-events/spill/bm_posY_vsEvent"))->Fill(currEvent,bmgloproj.Y());



}
}
//**** stop reco events



//**** no-reco events
if (myGlb->GetTracksN() ==0){
for( Int_t iTrack = 0; iTrack < myBMNtuTrk->GetTracksN(); ++iTrack ) {
  TABMtrack* track = myBMNtuTrk->GetTrack(iTrack);
  //project to the target in the BM ref., then move to the global ref.
  TVector3 bmlocalproj=ProjectToZ(track->GetSlope(), track->GetOrigin(),fpFootGeo->FromGlobalToBMLocal(fpFootGeo->GetTGCenter()).Z());
  TVector3 bmgloproj=ProjectToZ(fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope()), fpFootGeo->FromBMLocalToGlobal(track->GetOrigin()),fpFootGeo->GetTGCenter().Z());
  ((TH2D*)gDirectory->Get("clusPosition/BM/no-reco-events/bm_target_bmsys"))->Fill(bmlocalproj.X(),bmlocalproj.Y());
  ((TH2D*)gDirectory->Get("clusPosition/BM/no-reco-events/bm_target_glbsys"))->Fill(bmgloproj.X(),bmgloproj.Y());
  ((TH1D*)gDirectory->Get("clusPosition/BM/no-reco-events/bm_target_Xpos_glbsys"))->Fill(bmgloproj.X());
  ((TH1D*)gDirectory->Get("clusPosition/BM/no-reco-events/bm_target_Ypos_glbsys"))->Fill(bmgloproj.Y());
  ((TH1D*)gDirectory->Get("clusPosition/BM/no-reco-events/bm_target_bmsys_theta")) -> Fill( track->GetSlope().Theta()*TMath::RadToDeg());
  ((TH1D*)gDirectory->Get("clusPosition/BM/no-reco-events/bm_target_bmsys_phi")) -> Fill( track->GetSlope().Phi()*TMath::RadToDeg());
  ((TH1D*)gDirectory->Get("clusPosition/BM/no-reco-events/bm_target_glbsys_theta")) -> Fill( (fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope())).Theta()*TMath::RadToDeg());
  ((TH1D*)gDirectory->Get("clusPosition/BM/no-reco-events/bm_target_glbsys_phi")) -> Fill( (fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope())).Phi()*TMath::RadToDeg());

  //study pattern of several quantities wrt events: Spill studies
    ((TH2D*)gDirectory->Get("clusPosition/BM/no-reco-events/spill/bm_posX_vsEvent"))->Fill(currEvent,bmgloproj.X());
    ((TH2D*)gDirectory->Get("clusPosition/BM/no-reco-events/spill/bm_posY_vsEvent"))->Fill(currEvent,bmgloproj.Y());
}
}
//**** stop no-reco events


}

//VT tracklet position study
//! richieste su vtbm match?
TAVTntuVertex *vtxNtuVertex = (TAVTntuVertex*)fpNtuVtx->GenerateObject();
((TH1D*)gDirectory->Get("clusPosition/VT/vtxpoints/vtx_num")) -> Fill(vtxNtuVertex->GetVertexN());

TAVTvertex* vtxvertex   = 0x0;
TVector3 vtxvertposloc,vtxvertposglob;
TAVTvertex* vtxmatchvertex=nullptr;



for (Int_t iVtx = 0; iVtx < vtxNtuVertex->GetVertexN(); ++iVtx) {  // for every vertex in an event
    vtxvertex = vtxNtuVertex->GetVertex(iVtx);
    if (vtxvertex == 0x0)
      continue;
    vtxvertposloc = vtxvertex->GetVertexPosition();
    vtxvertposglob = fpFootGeo->FromVTLocalToGlobal(vtxvertposloc);
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxpoints/vtx_vtx_profile_vtxsys")) -> Fill(vtxvertposloc.X(),vtxvertposloc.Y());
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxpoints/vtx_vtx_profile_glbsys")) -> Fill(vtxvertposglob.X(),vtxvertposglob.Y());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxpoints/vtx_vtx_trknum")) -> Fill(vtxvertex->GetTracksN());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxpoints/vtx_vtx_BMmatched")) -> Fill(vtxvertex->IsBmMatched());

    if (vtxvertex->IsBmMatched() == true) {
    vtxmatchvertex = vtxNtuVertex->GetVertex(iVtx);
    }

    //vtxmatchvertex=(vtxvertex->IsBmMatched()) ? vtxNtuVertex->GetVertex(iVtx) : nullptr;  //matched vertex of an event with BM

  }

// if no one of the vertex points of an event matches with BM...
if (vtxmatchvertex == nullptr) {((TH1D*)gDirectory->Get("clusPosition/VT/vtxpoints/ev_no_vtxmatched")) -> Fill(1);}
else {
  // ---------------------- tracks of the BM-matched vertex

//***** start all-events study despite glb tracking
((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_trk_num"))->Fill(vtxmatchvertex->GetTracksN());
for( Int_t iTrack = 0; iTrack < vtxmatchvertex->GetTracksN(); ++iTrack ) { //loop on vertex tracks
    TAVTtrack* track = vtxmatchvertex->GetTrack(iTrack);

    //cluster study
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_n_cluster")) ->Fill( track->GetClustersN());
    TAVTbaseCluster* clust = track->GetCluster(0);
    TVector3 clPos = clust->GetPosition();
    TVector3 GlbclPos = fpFootGeo->FromVTLocalToGlobal(clPos);
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/clust0_position_vtxsys"))->Fill(clPos.X(),clPos.Y());
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/clust0_position_glbsys"))->Fill(GlbclPos.X(),GlbclPos.Y());


    //projection of vtx tracks on TW
    Float_t posZtw = fpFootGeo->FromTWLocalToGlobal(TVector3(0,0,0)).Z();
    posZtw = fpFootGeo->FromGlobalToVTLocal(TVector3(0, 0, posZtw)).Z();
    TVector3 A3 = track->Intersection(posZtw);         //local frame
    TVector3 A4 = fpFootGeo->FromVTLocalToGlobal(A3);  //global frame
    TVector3 A5 = fpFootGeo->FromGlobalToTWLocal(A4);
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_tw_proj_glbsys"))->Fill(A4.X(),A4.Y());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_tw_proj_Xpos_proj_glbsys"))->Fill(A4.X());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_tw_proj_Ypos_proj_glbsys"))->Fill(A4.Y());
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_tw_proj_twsys"))->Fill(A5.X(),A5.Y());

    // vt track whose projection is inside the TW
    if (A4.X() > -20. && A4.X() < 20. && A4.Y() > -20. && A4.Y() < 20. ) {
      ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/gltrackcandidate/vtx_tw_proj_glbsys_gltrackcandidate"))->Fill(A4.X(),A4.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/gltrackcandidate/vtx_tw_proj_Xpos_glbsys_gltrackcandidate"))->Fill(A4.X());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/gltrackcandidate/vtx_tw_proj_Ypos_glbsys_gltrackcandidate"))->Fill(A4.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/gltrackcandidate/vtx_theta_glbsys_gltrackcandidate")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/gltrackcandidate/vtx_phi_glbsys_gltrackcandidate")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Phi()*TMath::RadToDeg());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/gltrackcandidate/vtx_theta_vtxsys_gltrackcandidate")) -> Fill(( track->GetSlopeZ()).Theta()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/gltrackcandidate/vtx_phi_vtxsys_gltrackcandidate")) -> Fill( (track->GetSlopeZ()).Phi()*TMath::RadToDeg());

      }

    //projection of vtx tracks on TARGET
    Float_t posZtg = fpFootGeo->FromGlobalToVTLocal(fpFootGeo->GetTGCenter()).Z();
    TVector3 A3_tg = track->Intersection(posZtg);         //local frame
    TVector3 A4_tg = fpFootGeo->FromVTLocalToGlobal(A3_tg);  //global frame
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_tg_proj_glbsys"))->Fill(A4_tg.X(),A4_tg.Y());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_tg_proj_Xpos_proj_glbsys"))->Fill(A4_tg.X());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_tg_proj_Ypos_proj_glbsys"))->Fill(A4_tg.Y());

    //track angle
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_theta_glbsys")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_phi_glbsys")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Phi()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_theta_vtxsys")) -> Fill(( track->GetSlopeZ()).Theta()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_phi_vtxsys")) -> Fill( (track->GetSlopeZ()).Phi()*TMath::RadToDeg());

    //compare n° of tracklets vs n° of global tracks
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/n_tracklets_vs_n_globaltracks"))->Fill(vtxmatchvertex->GetTracksN(),myGlb->GetTracksN() );

    if (myGlb->GetTracksN() ==1 && vtxmatchvertex->GetTracksN() == 1){
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/vtx_theta_vs_globtrack_theta"))->Fill((fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg(),myGlb->GetTrack(0)->GetTgtThetaBm() *TMath::RadToDeg());
    }
}
    //study pattern of several quantities wrt events: Spill studies
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/spill/vtx_posX_vsEvent"))->Fill(currEvent,fpFootGeo->FromVTLocalToGlobal(vtxmatchvertex->GetVertexPosition()).X());
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/all_events/spill/vtx_posY_vsEvent"))->Fill(currEvent,fpFootGeo->FromVTLocalToGlobal(vtxmatchvertex->GetVertexPosition()).Y());

//***** stop all-events study despite glb tracking



//***** start reco-events study despite glb tracking
if (myGlb->GetTracksN() >0){
((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_trk_num"))->Fill(vtxmatchvertex->GetTracksN());
for( Int_t iTrack = 0; iTrack < vtxmatchvertex->GetTracksN(); ++iTrack ) { //loop on vertex tracks
    TAVTtrack* track = vtxmatchvertex->GetTrack(iTrack);

    //cluster study
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_n_cluster")) ->Fill( track->GetClustersN());
    TAVTbaseCluster* clust = track->GetCluster(0);
    TVector3 clPos = clust->GetPosition();
    TVector3 GlbclPos = fpFootGeo->FromVTLocalToGlobal(clPos);
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/clust0_position_vtxsys"))->Fill(clPos.X(),clPos.Y());
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/clust0_position_glbsys"))->Fill(GlbclPos.X(),GlbclPos.Y());


    //projection of vtx tracks on TW
    Float_t posZtw = fpFootGeo->FromTWLocalToGlobal(TVector3(0,0,0)).Z();
    posZtw = fpFootGeo->FromGlobalToVTLocal(TVector3(0, 0, posZtw)).Z();
    TVector3 A3 = track->Intersection(posZtw);         //local frame
    TVector3 A4 = fpFootGeo->FromVTLocalToGlobal(A3);  //global frame
    TVector3 A5 = fpFootGeo->FromGlobalToTWLocal(A4);
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_tw_proj_glbsys"))->Fill(A4.X(),A4.Y());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_tw_proj_Xpos_proj_glbsys"))->Fill(A4.X());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_tw_proj_Ypos_proj_glbsys"))->Fill(A4.Y());
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_tw_proj_twsys"))->Fill(A5.X(),A5.Y());

    // vt track whose projection is inside the TW
    if (A4.X() > -20. && A4.X() < 20. && A4.Y() > -20. && A4.Y() < 20. ) {
      ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/gltrackcandidate/vtx_tw_proj_glbsys_gltrackcandidate"))->Fill(A4.X(),A4.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/gltrackcandidate/vtx_tw_proj_Xpos_glbsys_gltrackcandidate"))->Fill(A4.X());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/gltrackcandidate/vtx_tw_proj_Ypos_glbsys_gltrackcandidate"))->Fill(A4.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/gltrackcandidate/vtx_theta_glbsys_gltrackcandidate")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/gltrackcandidate/vtx_phi_glbsys_gltrackcandidate")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Phi()*TMath::RadToDeg());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/gltrackcandidate/vtx_theta_vtxsys_gltrackcandidate")) -> Fill(( track->GetSlopeZ()).Theta()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/gltrackcandidate/vtx_phi_vtxsys_gltrackcandidate")) -> Fill( (track->GetSlopeZ()).Phi()*TMath::RadToDeg());

      }

    //projection of vtx tracks on TARGET
    Float_t posZtg = fpFootGeo->FromGlobalToVTLocal(fpFootGeo->GetTGCenter()).Z();
    TVector3 A3_tg = track->Intersection(posZtg);         //local frame
    TVector3 A4_tg = fpFootGeo->FromVTLocalToGlobal(A3_tg);  //global frame
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_tg_proj_glbsys"))->Fill(A4_tg.X(),A4_tg.Y());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_tg_proj_Xpos_proj_glbsys"))->Fill(A4_tg.X());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_tg_proj_Ypos_proj_glbsys"))->Fill(A4_tg.Y());

    //track angle
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_theta_glbsys")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_phi_glbsys")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Phi()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_theta_vtxsys")) -> Fill(( track->GetSlopeZ()).Theta()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_phi_vtxsys")) -> Fill( (track->GetSlopeZ()).Phi()*TMath::RadToDeg());

    //compare n° of tracklets vs n° of global tracks
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/n_tracklets_vs_n_globaltracks"))->Fill(vtxmatchvertex->GetTracksN(),myGlb->GetTracksN() );

    if (myGlb->GetTracksN() ==1 && vtxmatchvertex->GetTracksN() == 1){
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/vtx_theta_vs_globtrack_theta"))->Fill((fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg(),myGlb->GetTrack(0)->GetTgtThetaBm() *TMath::RadToDeg());
    }




    /*

      TVector3 vtxlocalproj=ProjectToZ(track->GetSlopeZ(), track->GetOrigin(),geoTrafo->FromGlobalToVTLocal(geoTrafo->GetTGCenter()).Z());
      myfill("VT/vtx_target_vtxsys",vtxlocalproj.X(),vtxlocalproj.Y());
      TVector3 glbslope = geoTrafo->VecFromVTLocalToGlobal(track->GetSlopeZ());
      TVector3 glborigin = geoTrafo->FromVTLocalToGlobal(track->GetOrigin());
      TVector3 vtxprojglo=geoTrafo->FromVTLocalToGlobal(vtxlocalproj);
      myfill("VT/vtx_target_glbsys",vtxprojglo.X(),vtxprojglo.Y());
    */

}
    //study pattern of several quantities wrt events: Spill studies
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/spill/vtx_posX_vsEvent"))->Fill(currEvent,fpFootGeo->FromVTLocalToGlobal(vtxmatchvertex->GetVertexPosition()).X());
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/spill/vtx_posY_vsEvent"))->Fill(currEvent,fpFootGeo->FromVTLocalToGlobal(vtxmatchvertex->GetVertexPosition()).Y());
    if (myBMNtuTrk->GetTracksN() == 1){  // i want only events with 1 bm track
      TVector3 bmgloproj=ProjectToZ(fpFootGeo->VecFromBMLocalToGlobal(myBMNtuTrk->GetTrack(0)->GetSlope()), fpFootGeo->FromBMLocalToGlobal(myBMNtuTrk->GetTrack(0)->GetOrigin()),fpFootGeo->GetTGCenter().Z());
      ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/reco-events/spill/BM_matched_posX_vsEvent"))->Fill(currEvent,bmgloproj.X());
    }


}
//***** stop reco-events study


//***** start no-reco-events study
if (myGlb->GetTracksN() ==0){
((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_trk_num"))->Fill(vtxmatchvertex->GetTracksN());
for( Int_t iTrack = 0; iTrack < vtxmatchvertex->GetTracksN(); ++iTrack ) { //loop on vertex tracks
    TAVTtrack* track = vtxmatchvertex->GetTrack(iTrack);

    //cluster study
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_n_cluster")) ->Fill( track->GetClustersN());
    TAVTbaseCluster* clust = track->GetCluster(0);
    TVector3 clPos = clust->GetPosition();
    TVector3 GlbclPos = fpFootGeo->FromVTLocalToGlobal(clPos);
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/clust0_position_vtxsys"))->Fill(clPos.X(),clPos.Y());
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/clust0_position_glbsys"))->Fill(GlbclPos.X(),GlbclPos.Y());


    //projection of vtx tracks on TW
    Float_t posZtw = fpFootGeo->FromTWLocalToGlobal(TVector3(0,0,0)).Z();
    posZtw = fpFootGeo->FromGlobalToVTLocal(TVector3(0, 0, posZtw)).Z();
    TVector3 A3 = track->Intersection(posZtw);         //local frame
    TVector3 A4 = fpFootGeo->FromVTLocalToGlobal(A3);  //global frame
    TVector3 A5 = fpFootGeo->FromGlobalToTWLocal(A4);
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_tw_proj_glbsys"))->Fill(A4.X(),A4.Y());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_tw_proj_Xpos_proj_glbsys"))->Fill(A4.X());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_tw_proj_Ypos_proj_glbsys"))->Fill(A4.Y());
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_tw_proj_twsys"))->Fill(A5.X(),A5.Y());

    // vt track whose projection is inside the TW
    if (A4.X() > -20. && A4.X() < 20. && A4.Y() > -20. && A4.Y() < 20. ) {
      ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/gltrackcandidate/vtx_tw_proj_glbsys_gltrackcandidate"))->Fill(A4.X(),A4.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/gltrackcandidate/vtx_tw_proj_Xpos_glbsys_gltrackcandidate"))->Fill(A4.X());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/gltrackcandidate/vtx_tw_proj_Ypos_glbsys_gltrackcandidate"))->Fill(A4.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/gltrackcandidate/vtx_theta_glbsys_gltrackcandidate")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/gltrackcandidate/vtx_phi_glbsys_gltrackcandidate")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Phi()*TMath::RadToDeg());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/gltrackcandidate/vtx_theta_vtxsys_gltrackcandidate")) -> Fill(( track->GetSlopeZ()).Theta()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/gltrackcandidate/vtx_phi_vtxsys_gltrackcandidate")) -> Fill( (track->GetSlopeZ()).Phi()*TMath::RadToDeg());

      }

    //projection of vtx tracks on TARGET
    Float_t posZtg = fpFootGeo->FromGlobalToVTLocal(fpFootGeo->GetTGCenter()).Z();
    TVector3 A3_tg = track->Intersection(posZtg);         //local frame
    TVector3 A4_tg = fpFootGeo->FromVTLocalToGlobal(A3_tg);  //global frame
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_tg_proj_glbsys"))->Fill(A4_tg.X(),A4_tg.Y());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_tg_proj_Xpos_proj_glbsys"))->Fill(A4_tg.X());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_tg_proj_Ypos_proj_glbsys"))->Fill(A4_tg.Y());

    //track angle
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_theta_glbsys")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_phi_glbsys")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Phi()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_theta_vtxsys")) -> Fill(( track->GetSlopeZ()).Theta()*TMath::RadToDeg());
    ((TH1D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_phi_vtxsys")) -> Fill( (track->GetSlopeZ()).Phi()*TMath::RadToDeg());

    //compare n° of tracklets vs n° of global tracks
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/n_tracklets_vs_n_globaltracks"))->Fill(vtxmatchvertex->GetTracksN(),myGlb->GetTracksN() );

    if (myGlb->GetTracksN() ==1 && vtxmatchvertex->GetTracksN() == 1){
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/vtx_theta_vs_globtrack_theta"))->Fill((fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg(),myGlb->GetTrack(0)->GetTgtThetaBm() *TMath::RadToDeg());
    }




    /*

      TVector3 vtxlocalproj=ProjectToZ(track->GetSlopeZ(), track->GetOrigin(),geoTrafo->FromGlobalToVTLocal(geoTrafo->GetTGCenter()).Z());
      myfill("VT/vtx_target_vtxsys",vtxlocalproj.X(),vtxlocalproj.Y());
      TVector3 glbslope = geoTrafo->VecFromVTLocalToGlobal(track->GetSlopeZ());
      TVector3 glborigin = geoTrafo->FromVTLocalToGlobal(track->GetOrigin());
      TVector3 vtxprojglo=geoTrafo->FromVTLocalToGlobal(vtxlocalproj);
      myfill("VT/vtx_target_glbsys",vtxprojglo.X(),vtxprojglo.Y());
    */

}
    //study pattern of several quantities wrt events: Spill studies
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/spill/vtx_posX_vsEvent"))->Fill(currEvent,fpFootGeo->FromVTLocalToGlobal(vtxmatchvertex->GetVertexPosition()).X());
    ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/spill/vtx_posY_vsEvent"))->Fill(currEvent,fpFootGeo->FromVTLocalToGlobal(vtxmatchvertex->GetVertexPosition()).Y());
    if (myBMNtuTrk->GetTracksN() == 1){  // i want only events with 1 bm track
      TVector3 bmgloproj=ProjectToZ(fpFootGeo->VecFromBMLocalToGlobal(myBMNtuTrk->GetTrack(0)->GetSlope()), fpFootGeo->FromBMLocalToGlobal(myBMNtuTrk->GetTrack(0)->GetOrigin()),fpFootGeo->GetTGCenter().Z());
      ((TH2D*)gDirectory->Get("clusPosition/VT/vtxmatched/no-reco-events/spill/BM_matched_posX_vsEvent"))->Fill(currEvent,bmgloproj.X());
    }
}
//***** stop no-reco-events study


}


TAVTntuTrack *myVtTr = (TAVTntuTrack*)fpNtuTrackVtx->GenerateObject();
Float_t posZtw = fpFootGeo->FromTWLocalToGlobal(TVector3(0,0,0)).Z();
posZtw = fpFootGeo->FromGlobalToVTLocal(TVector3(0, 0, posZtw)).Z();



for( Int_t iTrack = 0; iTrack < myVtTr->GetTracksN(); ++iTrack ) { //loop on vertex tracks
      TAVTtrack* track = myVtTr->GetTrack(iTrack);

      //projection of vt track on target
      TVector3 vtxlocalproj=ProjectToZ(track->GetSlopeZ(), track->GetOrigin(),fpFootGeo->FromGlobalToVTLocal(fpFootGeo->GetTGCenter()).Z());
      TVector3 vtxgloprojglo=ProjectToZ(fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ()), fpFootGeo->VecFromVTLocalToGlobal(track->GetOrigin()),fpFootGeo->GetTGCenter().Z());
      ((TH2D*)gDirectory->Get("clusPosition/VT/vtx_target_vtsys"))->Fill(vtxlocalproj.X(),vtxlocalproj.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_target_vtsys_theta")) -> Fill( track->GetSlopeZ().Theta()*TMath::RadToDeg());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_target_vtsys_phi")) -> Fill( track->GetSlopeZ().Phi()*TMath::RadToDeg());
      ((TH2D*)gDirectory->Get("clusPosition/VT/vtx_target_glbsys"))->Fill(vtxgloprojglo.X(),vtxgloprojglo.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_Xpos_target_glbsys"))->Fill(vtxgloprojglo.X());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_Ypos_target_glbsys"))->Fill(vtxgloprojglo.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_target_glbsys_theta")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_target_glbsys_phi")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Phi()*TMath::RadToDeg());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_n_cluster")) ->Fill( track->GetClustersN());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_n_tracks")) ->Fill( myVtTr->GetTracksN());

      //projection of vt track on tw
      TVector3 A3 = track->Intersection(posZtw);         //local frame
      TVector3 A4 = fpFootGeo->FromVTLocalToGlobal(A3);  //global frame
      TVector3 A5 = fpFootGeo->FromGlobalToTWLocal(A4);
      ((TH2D*)gDirectory->Get("clusPosition/VT/vtx_tw_glbsys"))->Fill(A4.X(),A4.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_Xpos_tw_glbsys"))->Fill(A4.X());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_Ypos_tw_glbsys"))->Fill(A4.Y());

      if (A4.X() > -20. && A4.X() < 20. && A4.Y() > -20. && A4.Y() < 20. ) {    // vt track whose projection is inside the vertex
      ((TH2D*)gDirectory->Get("clusPosition/VT/vtx_tw_glbsys_gltrackcandidate"))->Fill(A4.X(),A4.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_Xpos_tw_glbsys_gltrackcandidate"))->Fill(A4.X());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_Ypos_tw_glbsys_gltrackcandidate"))->Fill(A4.Y());
      ((TH1D*)gDirectory->Get("clusPosition/VT/vtx_target_glbsys_theta_gltrackcandidate")) -> Fill( (fpFootGeo->VecFromVTLocalToGlobal(track->GetSlopeZ())).Theta()*TMath::RadToDeg());
      }


      ((TH2D*)gDirectory->Get("clusPosition/VT/vtx_tw_twsys"))->Fill(A5.X(),A5.Y());






}


//vt tracklet vs global tracking studies
//TAGntuGlbTrack *myGlb = (TAGntuGlbTrack*)fpNtuGlbTrack->Object();
if (myGlb->GetTracksN() ==1 && myVtTr->GetTracksN() == 1){
((TH2D*)gDirectory->Get("clusPosition/VT/vtx_theta_vs_globtrack_theta"))->Fill((fpFootGeo->VecFromVTLocalToGlobal(myVtTr->GetTrack(0)->GetSlopeZ())).Theta()*TMath::RadToDeg(),myGlb->GetTrack(0)->GetTgtThetaBm() *TMath::RadToDeg());
}

//VTX CORRELATION WITH BM
//select events with only one BM track and one vtx track
  if(myBMNtuTrk->GetTracksN() ==1 && vtxNtuVertex->GetVertexN()==1) {


  TAVTvertex *vtxvertex = vtxNtuVertex->GetVertex(0);
  if(vtxvertex->GetTracksN()==1) {


  TAVTtrack *vttrack=vtxvertex->GetTrack(0);
  TABMtrack* bmtrack = myBMNtuTrk->GetTrack(0);
  //cout << "asasd "<< bmtrack->GetOrigin().X() << " "<< vttrack->GetOrigin().X() <<endl;
  ((TH2D*)gDirectory->Get("clusPosition/VTXSYNC/origin_xx_bmvtx_all")) ->Fill(bmtrack->GetOrigin().X(),vttrack->GetOrigin().X());
  }
  }


//TW study
TATWntuPoint* myTWNtuPt = (TATWntuPoint*)fpNtuRecTw->GenerateObject();
((TH1D*)gDirectory->Get("clusPosition/TW/tw_points_num")) -> Fill(myTWNtuPt->GetPointsN());
for (int i = 0; i < myTWNtuPt->GetPointsN(); i++) {
    TATWpoint *point = myTWNtuPt->GetPoint(i);
    ((TH1D*)gDirectory->Get("clusPosition/TW/tw_Xpos_twsys")) -> Fill(point->GetPosition().X());
    ((TH1D*)gDirectory->Get("clusPosition/TW/tw_Ypos_twsys")) -> Fill(point->GetPosition().Y());
    ((TH2D*)gDirectory->Get("clusPosition/TW/tw_XYpos_twsys")) -> Fill(point->GetPosition().X(),point->GetPosition().Y());
    ((TH1D*)gDirectory->Get("clusPosition/TW/tw_Xpos_glbsys")) -> Fill(point->GetPositionGlb().X());
    ((TH1D*)gDirectory->Get("clusPosition/TW/tw_Ypos_glbsys")) -> Fill(point->GetPositionGlb().Y());
    ((TH2D*)gDirectory->Get("clusPosition/TW/tw_XYpos_glbsysAll")) -> Fill(point->GetPositionGlb().X(),point->GetPositionGlb().Y());
    ((TH1D*)gDirectory->Get("clusPosition/TW/tw_barmultAll")) -> Fill(point->GetRowID(),point->GetColumnID());
    ((TH1D*)gDirectory->Get("clusPosition/TW/tw_GetChargeZ")) -> Fill(point->GetChargeZ());
}
/*
 for(int it=0;it<nt;it++){ // for every track
      fGlbTrack = myGlb->GetTrack(it);
      Float_t Th_recoBM2 =  fGlbTrack->GetTgtThetaBm() *TMath::RadToDeg();
*/
//TABMntuHit* bmNtuHit = (TABMntuHit*) fpNtuHitBm->GenerateObject();
//Int_t nbmHits  = bmNtuHit->GetHitsN();
//cout <<nbmHits << endl;
}

bool GlobalRecoAna::SelectionCuts(){
  bool categorize_TWpoint = false;
	bool categorize_BMtrack = false;
	bool categorize_VTvtx = false;
  bool categorize_VTBMmatching = false;
	bool categorize_TWdist = false;
  TAVTvertex* vtxPD   = 0x0; //NEW

//1) The event has at least one TWpoint
if (myTWNtuPt->GetPointsN()==0){
  categorize_TWpoint = false;
}
  else {
  categorize_TWpoint = true;
  }

//2) There is only one BM track
if (myBMNtuTrk->GetTracksN() !=1) {
  categorize_BMtrack = false;
} else {categorize_BMtrack = true;
}

//3) There is at least one vtx point
if (myVtNtuVtx -> GetVertexN()  == 0 ) {
categorize_VTvtx = false;
} else {categorize_VTvtx = true;
}

//4) there is vtx - bm matching with at least one vtx point
if (categorize_BMtrack == true && categorize_VTvtx == true ){
  categorize_VTBMmatching = false;
  for (Int_t iVtx = 0; iVtx < myVtNtuVtx -> GetVertexN(); ++iVtx) {
		vtxPD = myVtNtuVtx->GetVertex(iVtx);
		if (vtxPD == 0x0){
			continue;
		}
		else if(!vtxPD->IsBmMatched() ){
			  continue;
		}
    else {
      categorize_VTBMmatching = true;
    }
  }
} else {
categorize_VTBMmatching = false;
}





((TH1D*)gDirectory->Get("h_eventSelected"))->AddBinContent(1,1);   // all events
if (categorize_TWpoint) ((TH1D*)gDirectory->Get("h_eventSelected"))->AddBinContent(2,1);
if (categorize_TWpoint && categorize_BMtrack)     ((TH1D*)gDirectory->Get("h_eventSelected"))->AddBinContent(3,1);
if (categorize_TWpoint && categorize_BMtrack && categorize_VTvtx) ((TH1D*)gDirectory->Get("h_eventSelected"))->AddBinContent(4,1);
if (categorize_TWpoint && categorize_BMtrack && categorize_VTvtx && categorize_VTBMmatching== 1) ((TH1D*)gDirectory->Get("h_eventSelected"))->AddBinContent(5,1);

return (categorize_TWpoint && categorize_BMtrack && categorize_VTvtx && categorize_VTBMmatching );
}


void GlobalRecoAna::ThetaTrueVSThetaRecoPlots(){
    if(Z_true>0 && Z_true<=primary_cha){

    string name = "theta_MigMat/Z_" + to_string(Z_true-1) +"#"+to_string(Z_true-0.5)+"_"+to_string(Z_true+0.5)+"/migMatrix_theta_Z"+to_string(Z_true);
    //cout << name << endl;
    ((TH2D*)gDirectory->Get(name.c_str()))->Fill(Th_true, Th_recoBM);

    }
}
