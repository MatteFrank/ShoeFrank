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
#include "TAMCntuEvent.hxx"
#include "LocalReco.hxx"
#include "TAGntuEvent.hxx"
#include "LocalRecoMC.hxx"
#include "GlobalRecoMV.hxx"

using namespace std;


GlobalRecoMV::GlobalRecoMV(TString expName, Int_t runNumber, 
			   TString fileNameIn, TString fileNameout, 
			   Bool_t isMC, Int_t innTotEv) : 
  GlobalRecoAna(expName, runNumber, fileNameIn, fileNameout, isMC, innTotEv)
{

  methods.Register(new TANLBMAnalysis());
  methods.Register(new TANLBMVTmatchAnalysis());
}


GlobalRecoMV::~GlobalRecoMV()
{
}


void GlobalRecoMV::BeforeEventLoop(){

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
  file_out->cd();

  cout<<"Going to create "<<GetTitle()<<" outfile "<<endl;

  //initialization of several objects needed for the analysis
  gTAGroot->BeginEventLoop();
  mass_ana=new GlobalRecoMassAna();

  myVtNtuVtx = (TAVTntuVertex*)fpNtuVtx->GenerateObject();
  myBMNtuTrk = (TABMntuTrack*) fpNtuTrackBm->GenerateObject();

  methods.BeforeEventLoop();

  return;
}


void GlobalRecoMV::LoopEvent() {

  if(FootDebugLevel(1))
    cout<<"GlboalRecoAna::LoopEvent start"<<endl;
  if (fSkipEventsN>0) {
      currEvent=fSkipEventsN;
      nTotEv+=  fSkipEventsN;
    }
  else currEvent=0;

  //********************** begin loop on every event ********************
  while(gTAGroot->NextEvent()) { //for every event

    if (currEvent<20 || (currEvent<1000 && (currEvent % 10) == 0) ||
	(currEvent%100==0)|| FootDebugLevel(1))
      cout <<"current Event: " <<currEvent<<endl;

    methods.ProcessEvent();

    ++currEvent;
    if (currEvent == nTotEv)
      break;

  }//end of loop event

  return;
}



void GlobalRecoMV::AfterEventLoop(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::AfterEventLoop start"<<endl;

  methods.AfterEventLoop();

  gTAGroot->EndEventLoop();

  cout <<"Writing..." << endl;
  file_out->Write();

  cout <<"Closing..." << endl;
  file_out->Close();

  std::ofstream file("SelectedEvents.txt");
  for(std::vector<int>::const_iterator iter=goodEvents.begin(); iter!=goodEvents.end(); ++iter){
    file<<*iter<<"\n";
  }

  file.close();

  return;
}


void GlobalRecoMV:: Booking(){
  //fFlagMC = false; //N.B.: for MC FAKE REAL

  if(FootDebugLevel(1))
    cout<<"start GlboalRecoAna::Booking"<<endl;

  file_out->cd();

  h = new TH1D("ntrk","",10, 0 ,10.);

  //ClusPosition studies
  gDirectory->mkdir("clusPosition");
  gDirectory->cd("clusPosition");
  gDirectory->mkdir("BM");
  gDirectory->cd("BM");
  h = new TH1D("bm_n_tracks","Number of BM tracks per event; Number of bm tracks; Events",21,-0.5,20.5);
  h1v[ntracks] = new TH1D("BM_NTracks",
			  "Number of BM track in each event; # of BM tracks; Events",
			  21,-0.5,20.5);
  h1v[chix] = new TH1D("BM_chix",
		       "Chisquare of the X projection; chisquare (X); entries",
		       101,-0.5,100.5);
  h1v[chiy] = new TH1D("BM_chiy",
		       "Chisquare of the Y projection; chisquare (Y); entries",
		       101,-0.5,100.5);
  h1v[nx] = new TH1D("BM_nx",
		     "Number of BM measurements in the X projection; Number of X hits in BM; entries",
		       13,-0.5,12.5);
  h1v[ny] = new TH1D("BM_ny",
		     "Number of BM measurements in the Y projection; Number of Y hits in BM; entries",
		       13,-0.5,12.5);
  h1v[probx] = new TH1D("BM_probx",
		     "BM: Chisq probability of the X projection; X chisq probability in BM; entries",
		       51,-0.01,1.01);
  h1v[proby] = new TH1D("BM_proby",
		     "BM: Chisq probability of the Y projection; Y chisq probability in BM; entries",
		       51,-0.01,1.01);
  h1v[prob3] = new TH1D("BM_prob3",
		     "BM: Chisq probability for 3 hits; chisq probability in BM for 3 hits (in x or y); entries",
		       51,-0.01,1.01);
  h1v[prob4] = new TH1D("BM_prob4",
		     "BM: Chisq probability for 4 hits; chisq probability in BM for 4 hits (in x or y); entries",
		       51,-0.01,1.01);
  h1v[prob5] = new TH1D("BM_prob5",
		     "BM: Chisq probability for 5 hits; chisq probability in BM for 5 hits (in x or y); entries",
		       51,-0.01,1.01);
  h1v[prob6] = new TH1D("BM_prob6",
		     "BM: Chisq probability for 6 hits; chisq probability in BM for 6 hits (in x or y); entries",
		       51,-0.01,1.01);




  gDirectory->mkdir("all-events");
  gDirectory->cd("all-events");
  h2 = new TH2D("bm_target_bmsys","BM tracks on target  projections in BM sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
  h2 = new TH2D("bm_target_glbsys","BM tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
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
  h = new TH1D("bm_target_Xpos_glbsys","BM tracks X pos on target projections in GLB sys (other method);X[cm]",600,-3.,3.);
  h = new TH1D("bm_target_Ypos_glbsys","BM tracks Y pos on target projections in GLB sys (other method);Y[cm]",600,-3.,3.);
  h = new TH1D("bm_target_bmsys_theta","theta of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
  h = new TH1D("bm_target_glbsys_theta","theta of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
  h = new TH1D("bm_target_bmsys_phi","phi of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
  h = new TH1D("bm_target_glbsys_phi","phi of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
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
  h1v[nvtx] = new TH1D("VT_NTracks",
			  "Number of VT tracks in each event where BMtracks=1; # of VT tracks; Events",
		       21,-0.5,20.5);
  h1v[allDistances] = new TH1D("VT_ALLMatchDist","All BM-VT tracks distance; distance [cm]; Entries",100, 0.,2.);
  h1v[matchDistance] = new TH1D("VT_MatchDist","Match BM-VT tracks distance; distance [cm]; Entries",100, 0.,1.);
  h1v[sensors] = new TH1D("VT_NSENS","Match BM-VT: selected VT track; # planes ; Entries",5, -.5, 4.5);
  h1v[xdiff] = new TH1D("VT_BM_X","Match BM-VT: Xbm-Xvt; BM-VT x difference [cm]; Entries",200, -1.,1.);
  h1v[ydiff] = new TH1D("VT_BM_Y","Match BM-VT: Ybm-Yvt; BM-VT y difference [cm]; Entries",200, -1.,1.);
  h1v[theta] = new TH1D("VT_BM_theta","Match BM-VT: theta angle; tracklet opening angle [rad]; Entries",500, 0,0.05);
  h2match = new TH2D("VT_BM_XY","Match BM-VT; BM-VT x difference [cm]; BM-VT y difference",100, -1.,1., 100, -1.,1.);
  h1v[chi2xvt] = new TH1D("VT_Chi2_X","VT selected track; Chisq X; Entries",2000, 0.,100.);
  h1v[chi2yvt] = new TH1D("VT_Chi2_Y","VT selected track; Chisq Y; Entries",2000, 0.,100.);
  h2resvstx = new TH2D("VT_BM_resvsTx",
		       "Match BM-VT Residuals vs tx; tx BM; BM-VT x difference",
		       100, -0.01,0.01, 100, -0.5,0.5);
  h2resvsty = new TH2D("VT_BM_resvsTy",
		       "Match BM-VT Residuals vs ty; ty BM; BM-VT y difference",
		       100, -0.01,0.01, 100, -0.5,0.5);

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


  gDirectory->mkdir("VTXSYNC");
  gDirectory->cd("VTXSYNC");
  h2 = new TH2D("origin_xx_bmvtx_all","BM originX vs VTX originX for all the evts;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
  gDirectory->cd("..");
  gDirectory->cd("..");


  if(FootDebugLevel(1))
    cout<<"GlboalRecoAna::Booking done"<<endl;

  return;
}




void GlobalRecoMV::SetupTree(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::SetupTree start"<<endl;

  myReader = new TAGactTreeReader("myReader");

  if(TAGrecoManager::GetPar()->IncludeBM()){
    fpNtuTrackBm = new TAGdataDsc("bmtrack" , new TABMntuTrack());
    gTAGroot->AddRequiredItem("bmtrack");
    myReader->SetupBranch(fpNtuTrackBm, TABMntuTrack::GetBranchName());
    
    fpNtuEvt = new TAGdataDsc("evtNtu", new TAGntuEvent());
    gTAGroot->AddRequiredItem("evt");
    myReader->SetupBranch(fpNtuEvt, TAGntuEvent::GetBranchName());
  }

  if(TAGrecoManager::GetPar()->IncludeVT()){
    fpNtuClusVtx = new TAGdataDsc("vtclus",new TAVTntuCluster());
    fpNtuTrackVtx = new TAGdataDsc("vttrack",new TAVTntuTrack());
    fpNtuVtx = new TAGdataDsc("vtvtx",new TAVTntuVertex());
    gTAGroot->AddRequiredItem("vtclus");
    gTAGroot->AddRequiredItem("vttrack");
    gTAGroot->AddRequiredItem("vtvtx");
    myReader->SetupBranch(fpNtuClusVtx, TAVTntuCluster::GetBranchName());
    myReader->SetupBranch(fpNtuTrackVtx, TAVTntuTrack::GetBranchName());
    myReader->SetupBranch(fpNtuVtx, TAVTntuVertex::GetBranchName());
    if(fFlagMC){
      fpNtuMcVt = new TAGdataDsc("mcvt",new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcvt");
      myReader->SetupBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
    }
  }
  if(TAGrecoManager::GetPar()->IncludeIT()){
    fpNtuClusIt = new TAGdataDsc("itclus",new TAITntuCluster());
    gTAGroot->AddRequiredItem("itclus");
    myReader->SetupBranch(fpNtuClusIt, TAITntuCluster::GetBranchName());
    if(fFlagMC){
      fpNtuMcIt = new TAGdataDsc("mcit",new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcit");
      myReader->SetupBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
    }
  }
  if(TAGrecoManager::GetPar()->IncludeMSD()){
    fpNtuClusMsd = new TAGdataDsc("msdclus",new TAMSDntuCluster());
    gTAGroot->AddRequiredItem("msdclus");
    myReader->SetupBranch(fpNtuClusMsd, TAMSDntuCluster::GetBranchName());
    fpNtuRecMsd = new TAGdataDsc("msdpoint",new TAMSDntuPoint());
    gTAGroot->AddRequiredItem("msdpoint");
    myReader->SetupBranch(fpNtuRecMsd, TAMSDntuPoint::GetBranchName());
    if(fFlagMC){
      fpNtuMcMsd = new TAGdataDsc("mcmsd",new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcmsd");
      myReader->SetupBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
    }
  }
  if(TAGrecoManager::GetPar()->IncludeTW()){
    fpNtuRecTw = new TAGdataDsc("twpt",new TATWntuPoint());
    gTAGroot->AddRequiredItem("twpt");
    myReader->SetupBranch(fpNtuRecTw, TATWntuPoint::GetBranchName());

    if (fFlagMC == false){
      fpNtuWDtrigInfo = new TAGdataDsc("WDtrigInfo",new TAGWDtrigInfo());
      gTAGroot->AddRequiredItem("WDtrigInfo");
      myReader->SetupBranch(fpNtuWDtrigInfo, TAGWDtrigInfo::GetBranchName());
    }
    if(fFlagMC){
      fpNtuMcTw = new TAGdataDsc("mctw",new TAMCntuHit());
      gTAGroot->AddRequiredItem("mctw");
      myReader->SetupBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
    }
  }
  if(TAGrecoManager::GetPar()->IncludeCA()){
    fpNtuClusCa = new TAGdataDsc("caclus",new TACAntuCluster());
    gTAGroot->AddRequiredItem("caclus");
    myReader->SetupBranch(fpNtuClusCa, TACAntuCluster::GetBranchName());
    if(fFlagMC){
      fpNtuMcCa = new TAGdataDsc("mcca",new TAMCntuHit());
      gTAGroot->AddRequiredItem("mcca");
      myReader->SetupBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
    }
  }

  if(fFlagMC){
    fpNtuMcEvt = new TAGdataDsc("mcevt",new TAMCntuEvent());
    fpNtuMcTrk = new TAGdataDsc("mctrack",new TAMCntuPart());
    gTAGroot->AddRequiredItem("mcevt");
    gTAGroot->AddRequiredItem("mctrack");
    myReader->SetupBranch(fpNtuMcEvt, TAMCntuEvent::GetBranchName());
    myReader->SetupBranch(fpNtuMcTrk, TAMCntuPart::GetBranchName());
    if(TAGrecoManager::GetPar()->IsRegionMc()){
      fpNtuMcReg = new TAGdataDsc("mcreg",new TAMCntuRegion());
      gTAGroot->AddRequiredItem("mcreg");
      myReader->SetupBranch(fpNtuMcReg, TAMCntuRegion::GetBranchName());
    }
  }

  gTAGroot->AddRequiredItem("myReader");
  gTAGroot->Print();

  return;
}

