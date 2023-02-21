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

#include "LocalRecoMC.hxx"
#include "GlobalRecoMV.hxx"

using namespace std;


GlobalRecoMV::GlobalRecoMV(TString expName, Int_t runNumber, 
			   TString fileNameIn, TString fileNameout, 
			   Bool_t isMC, Int_t innTotEv) : 
  GlobalRecoAna(expName, runNumber, fileNameIn, fileNameout, isMC, innTotEv)
{
}


GlobalRecoMV::~GlobalRecoMV()
{
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

    AnalyzeBM();
    AnalyzeVTX();
    MatchBMVTX();

    ++currEvent;
    if (currEvent == nTotEv)
      break;

  }//end of loop event

  return;
}



void GlobalRecoMV::AfterEventLoop(){

  if(FootDebugLevel(1))
    cout<<"GlobalRecoAna::AfterEventLoop start"<<endl;
  //  GlobalRecoAna::AfterEventLoop();

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
  
  cout<<"Going to create "<<GetTitle()<<" outfile "<<endl;

  //initialization of several objects needed for the analysis
  gTAGroot->BeginEventLoop();
  mass_ana=new GlobalRecoMassAna();

  myVtNtuVtx = (TAVTntuVertex*)fpNtuVtx->GenerateObject();
  myBMNtuTrk = (TABMntuTrack*) fpNtuTrackBm->GenerateObject();


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


void GlobalRecoMV::AnalyzeBM(){

  //BM track position study
  TH1D* h1;
  TH2D* h2;
  h1 = ((TH1D*)gDirectory->Get("clusPosition/BM/bm_n_tracks"));
  h1->Fill(myBMNtuTrk->GetTracksN());

  h1v[ntracks]->Fill(myBMNtuTrk->GetTracksN());

  for( Int_t iTrack = 0; iTrack < myBMNtuTrk->GetTracksN(); ++iTrack ) {
    TABMtrack* track = myBMNtuTrk->GetTrack(iTrack);
    int nhx = track->GetHitsNx();
    int nhy = track->GetHitsNy();
    Double_t chi2x = track->GetChiSquareX()*(nhx-2);
    Double_t chi2y = track->GetChiSquareY()*(nhy-2);
    h1v[chix]->Fill(chi2x );
    h1v[chiy]->Fill(chi2y );
    h1v[nx]->Fill( nhx );
    h1v[ny]->Fill( nhy );
    Double_t probxval = TMath::Prob( chi2x, nhx-2);
    Double_t probyval = TMath::Prob( chi2y, nhy-2);
    h1v[probx]->Fill( probxval );
    h1v[proby]->Fill( probyval );
    if( nhx==3 )       h1v[prob3]->Fill(probxval);
    else if( nhx==4 )  h1v[prob4]->Fill(probxval);
    else if( nhx==5 )  h1v[prob5]->Fill(probxval);
    else if( nhx==6 )  h1v[prob6]->Fill(probxval);
    if( nhy==3 )       h1v[prob3]->Fill(probyval);
    else if( nhy==4 )  h1v[prob4]->Fill(probyval);
    else if( nhy==5 )  h1v[prob5]->Fill(probyval);
    else if( nhy==6 )  h1v[prob6]->Fill(probyval);
  }


  if (myBMNtuTrk->GetTracksN() == 1){  // select only events with 1 bm track
    for( Int_t iTrack = 0; iTrack < myBMNtuTrk->GetTracksN(); ++iTrack ) {
      TABMtrack* track = myBMNtuTrk->GetTrack(iTrack);
      //project to the target in the BM ref., then move to the global ref.
      TVector3 bmlocalproj=ProjectToZ(track->GetSlope(), track->GetOrigin(),
				      fpFootGeo->FromGlobalToBMLocal(fpFootGeo->GetTGCenter()).Z());
      TVector3 bmgloproj=ProjectToZ(fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope()), fpFootGeo->FromBMLocalToGlobal(track->GetOrigin()),fpFootGeo->GetTGCenter().Z());
      h2= ((TH2D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_bmsys"));
      h2->Fill(bmlocalproj.X(),bmlocalproj.Y());
      h2 = ((TH2D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_glbsys"));
      h2->Fill(bmgloproj.X(),bmgloproj.Y());
      h1=((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_Xpos_glbsys"));
      h1->Fill(bmgloproj.X());
      h1=((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_Ypos_glbsys"));
      h1->Fill(bmgloproj.Y());
      h1=((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_bmsys_theta"));
      h1-> Fill( track->GetSlope().Theta()*TMath::RadToDeg() );
      h1=((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_bmsys_phi"));
      h1-> Fill( track->GetSlope().Phi()*TMath::RadToDeg() );
      h1=((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_glbsys_theta"));
      auto globalSlope = (fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope()));
      h1-> Fill( globalSlope.Theta()*TMath::RadToDeg() );
      h1=((TH1D*)gDirectory->Get("clusPosition/BM/all-events/bm_target_glbsys_phi"));
      h1-> Fill( globalSlope.Phi()*TMath::RadToDeg() );
      
      //study pattern of several quantities wrt events: Spill studies
      h2=((TH2D*)gDirectory->Get("clusPosition/BM/all-events/spill/bm_posX_vsEvent"));
      h2->Fill(currEvent,bmgloproj.X());
      h2=((TH2D*)gDirectory->Get("clusPosition/BM/all-events/spill/bm_posY_vsEvent"));
      h2->Fill(currEvent,bmgloproj.Y());
    }

    // only for single track events. 
    // check interesting hits...
    
  }
}


void GlobalRecoMV::AnalyzeVTX(){
}


void GlobalRecoMV::MatchBMVTX(){

  TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)
    gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

  // strong requirements for matching
  if( myBMNtuTrk->GetTracksN()!=1 ) return;

  //skip empty or too crowded events
  TAVTntuVertex *vertexContainer = (TAVTntuVertex*)fpNtuVtx->GenerateObject();
  int vertexNumber = vertexContainer->GetVertexN();
  TAVTvertex* vtxPD   = 0x0; //NEW
  int nvtxtracks=0;
  for (Int_t iVtx = 0; iVtx < vertexNumber; ++iVtx) { // for every vertexEvent
    vtxPD = vertexContainer->GetVertex(iVtx);
    nvtxtracks += vtxPD->GetTracksN();
  }

  h1v[nvtx]->Fill(nvtxtracks);

  if( nvtxtracks<1 || nvtxtracks>3 ) return;

  TABMtrack* bmTrack = myBMNtuTrk->GetTrack(0);
  TVector3 bmPos   = fpFootGeo->FromBMLocalToGlobal( bmTrack->GetOrigin() );
  TVector3 bmSlope = fpFootGeo->VecFromBMLocalToGlobal( bmTrack->GetSlope() );
  bmSlope *= 1./bmSlope.Z();

  Double_t zTgt = fpFootGeo->GetTGCenter().Z();
  
  TVector3 bmTrkg = extrapolate( zTgt, bmPos, bmSlope);


  TAVTtrack* bestMatchTracklet = 0;
  Double_t bestDistancesq = 100000000.0;
  Double_t distancesq = 0.0;
  Double_t xBM = bmTrkg.X();
  Double_t yBM = bmTrkg.Y();
  Double_t zBM = bmTrkg.Z();
  for (Int_t iVtx = 0; iVtx < vertexNumber; ++iVtx) { // for every vertexEvent
    vtxPD = vertexContainer->GetVertex(iVtx);
    for (int iTrack = 0; iTrack < vtxPD->GetTracksN(); iTrack++) {  //for every tracklet
      TAVTtrack* tracklet = vtxPD->GetTrack( iTrack );
      
      TVector3 vtPos   = fpFootGeo->FromVTLocalToGlobal( tracklet->GetOrigin() );
      TVector3 vtSlope = fpFootGeo->VecFromVTLocalToGlobal( tracklet->GetSlopeZ() );

      TVector3 vtTrkg = extrapolate( zTgt, vtPos, vtSlope );
      Double_t xVTX = vtTrkg.X();
      Double_t yVTX = vtTrkg.Y();
      distancesq = pow(xBM-xVTX,2)+pow(yBM-yVTX,2);
      if( distancesq<bestDistancesq ){
	bestDistancesq = distancesq;
	bestMatchTracklet = tracklet;
      }
      h1v[allDistances]->Fill(TMath::Sqrt(distancesq));
    }
  }
  h1v[matchDistance]->Fill(TMath::Sqrt(bestDistancesq));




  // here with the best BM-VTX match
  TVector3 vtPos   = fpFootGeo->FromVTLocalToGlobal( bestMatchTracklet->GetOrigin() );
  TVector3 vtSlope = fpFootGeo->VecFromVTLocalToGlobal( bestMatchTracklet->GetSlopeZ() );
  
  TVector3 vtTrkg = extrapolate( zTgt, vtPos, vtSlope );

  Double_t xVTX = vtTrkg.X();
  Double_t yVTX = vtTrkg.Y();
  Double_t zVTX = vtTrkg.Z();
  Double_t angle= bmSlope.Angle(vtSlope);
  h1v[sensors]->Fill( bestMatchTracklet->GetClustersN() );
  h1v[xdiff]->Fill(xBM-xVTX);
  h1v[ydiff]->Fill(yBM-yVTX);
  h1v[theta]->Fill(angle);
  Double_t ndf = bestMatchTracklet->GetClustersN()-2.;
  h1v[chi2xvt]->Fill( bestMatchTracklet->GetChi2U()*ndf );
  h1v[chi2yvt]->Fill( bestMatchTracklet->GetChi2V()*ndf );
  h2resvstx->Fill( bmTrack->GetSlope().X(), xBM-xVTX);
  h2resvsty->Fill( bmTrack->GetSlope().Y(), yBM-yVTX);
  h2match->Fill(xBM-xVTX, yBM-yVTX);

  goodEvents.push_back(gTAGroot->CurrentEventNumber());

}


TVector3 GlobalRecoMV::extrapolate(Double_t z, 
				   const TVector3 & pos, 
				   const TVector3 & dir) const {

  TVector3 result = pos + dir*(z-pos.Z())*(1./dir.Z());
  return result;
}
