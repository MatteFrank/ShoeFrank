#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>
#include <stdio.h>
#include <cstdio>
#include <iostream>

#include "TAMCntuPart.hxx"
#include "TAMCntuHit.hxx"


#include "TASTparGeo.hxx"
#include "TASTntuHit.hxx"


#include "TABMparGeo.hxx"
#include "TABMntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TABMntuTrack.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
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
#include "TAMSDtrack.hxx"

#include "TATWparGeo.hxx"
#include "TATWntuPoint.hxx"

#include "TAGntuGlbTrack.hxx"

#include "TAGcampaignManager.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrunInfo.hxx"
#include "TAGparGeo.hxx"

#include "TAMCntuHit.hxx"
#include "TAGntuEvent.hxx"


#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>

#endif

#define DEG2RAD  0.01745329251994329577
#define RAD2DEG  57.2957795130823208768

//setting global parameters
#define   debug        0

using namespace std;

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
static int  evnum;             //current event number
static int  maxentries;        //max number of events
static TAGrunInfo*   runinfo;  //runinfo
static TAGgeoTrafo*  geoTrafo; //geometry
static TAGparGeo* parGeo; //beam info
static TAMCntuHit *bmNtuEve;
static TASTntuHit *stNtuHit;
static TABMntuHit*  bmNtuHit;
static TABMntuTrack*  bmNtuTrack;
static TAMCntuHit *vtMc;
static TAVTntuCluster *vtClus;
static TAVTntuVertex* vtxNtuVertex;
static TAVTntuCluster *vtxNtuCluster;
static TAVTntuTrack *vtxNtuTrack;
static TAMSDntuCluster *msdntuclus;
static TAMSDntuTrack *msdntutrack;
static TAITntuCluster *itClus;
static TATWntuPoint *twNtuPoint;
static TAMCntuPart *mcNtuPart;
static TAGntuGlbTrack *glbntutrk;
static TASTparGeo* stparGeo;
static TABMparGeo* bmparGeo;
static TAVTparGeo* vtparGeo;
static TAITparGeo* itparGeo;
static TAMSDparGeo* msdparGeo;
static TATWparGeo* twparGeo;
static TAGntuEvent* tgevent;

int Zbeam;
TString tobeprint;
Int_t vtxsynch=0; //=0 is still synch, !=0 is the number of events where the synch is lost
Int_t checkrate=5000; //rate to check the correlation between BM and VTX synch

//use the detector GLOBAL frame!
struct beamtrk {
    TVector3 slopeloc; //slope in local frame
    TVector3 originloc;//origin in local frame
    TVector3 slopeglo; //slope in global frame
    TVector3 tgposglo; //tgposition in global frame
    Int_t evts;
};

template <class t>
void myfill(const char *graphname, t x){
  if(gDirectory->Get(graphname)!=nullptr){
    ((TH1D*)(gDirectory->Get(graphname)))->Fill(x);
  }else{
    cout<<"ERROR!!!: "<<graphname<<" cannot be filled because it does not exist in gROOT, check Booking!"<<endl;
  }
return;
}

template <class t, class u>
void myfill(const char *graphname, t x, u y){
  if(gDirectory->Get(graphname)!=nullptr){
    ((TH2D*)(gDirectory->Get(graphname)))->Fill(x,y);
  }else{
    cout<<"ERROR!!!: "<<graphname<<" cannot be filled because it does not exist in gROOT, check Booking!"<<endl;
  }
return;
}

template <class t>
void myweightfill(const char *graphname, t x, double w){
  if(gDirectory->Get(graphname)!=nullptr){
    ((TH1D*)(gDirectory->Get(graphname)))->Fill(x,w);
  }else{
    cout<<"ERROR!!!: "<<graphname<<" cannot be filled because it does not exist in gROOT, check Booking!"<<endl;
  }
return;
}

template <class t>
void myweightfill(const char *graphname, t x, t y, double w){
  if(gDirectory->Get(graphname)!=nullptr){
    ((TH2D*)(gDirectory->Get(graphname)))->Fill(x,y,w);
  }else{
    cout<<"ERROR!!!: "<<graphname<<" cannot be filled because it does not exist in gROOT, check Booking!"<<endl;
  }
return;
}

TVector3 ProjectToZ(TVector3 Slope, TVector3 Pos0, Double_t FinalZ) {
  return TVector3(Slope.X()/Slope.Z()*(FinalZ-Pos0.Z())+Pos0.X() ,Slope.Y()/Slope.Z()*(FinalZ-Pos0.Z())+Pos0.Y(), FinalZ);
}


void Booking(TFile* file_out) {

  if(debug>0)
    cout<<"Booking start"<<endl;

  file_out->cd();
  TH1D *h;
  TH2D *h2;

  if(IncludeBM){
    file_out->mkdir("BM");
    file_out->cd("BM");
      h = new TH1D("BM_Hit_num","Number of BM hits x event all evts;Number of BM hits;Events",31,-0.5,30.5);
      h2 = new TH2D("bm_target_bmsys","BM tracks on target  projections in BM sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("bm_target_glbsys","BM tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h = new TH1D("AlignWrtTarget_slopeX_glbsys","BM mx in glb sys ;mx;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_slopeY_glbsys","BM my in glb sys ;my;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_tgposX_glbsys","BM projection on target Xpos in glb sys;X[cm];Events",600,-5.,5.);
      h = new TH1D("AlignWrtTarget_tgposY_glbsys","BM projection on target Ypos in glb sys;Y[cm];Events",600,-5.,5.);
      h = new TH1D("AlignWrtTarget_slopeX_glbsys_afterrot","BM mx in glb sys ;mx;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_slopeY_glbsys_afterrot","BM my in glb sys ;my;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_tgposX_glbsys_afterrot","BM projection on target Xpos in glb sys;X[cm];Events",600,-5.,5.);
      h = new TH1D("AlignWrtTarget_tgposY_glbsys_afterrot","BM projection on target Ypos in glb sys;Y[cm];Events",600,-5.,5.);
      h = new TH1D("AlignWrtTarget_slopeX_glbsys_final","BM mx in glb sys ;mx;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_slopeY_glbsys_final","BM my in glb sys ;my;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_tgposX_glbsys_final","BM projection on target Xpos in glb sys;X[cm];Events",600,-5.,5.);
      h = new TH1D("AlignWrtTarget_tgposY_glbsys_final","BM projection on target Ypos in glb sys;Y[cm];Events",600,-5.,5.);
    gDirectory->cd("..");
    file_out->cd("..");
  }


  if(IncludeVT){
    file_out->mkdir("VT");
    file_out->cd("VT");

      h = new TH1D("VT_vtx_num","Number of evtx per event;Number of evtx;Events",21,-0.5,20.5);
      h = new TH1D("VT_vtx_trknum","Number of vtx tracks associated per vertex;Number of tracks;Events",11,-0.5,10.5);
      h2 = new TH2D("VT_vtx_profile","VTX profile on target;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h = new TH1D("VT_vtx_BMmatched","Is matched with the BM flag;0=not matched 1=matched;Events",2,-0.5,1.5);
      h = new TH1D("VT_vtx_vtxDistanceMin","distance value between the vertex point and the tracks;Distance [cm];Events",300,-0.5,2.5);
      h = new TH1D("VT_vtx_vtxPositionErrorX","Vertex position error on X;Err [cm];Events",300,-0.5,2.5);
      h = new TH1D("VT_vtx_vtxPositionErrorZ","Vertex position error on Z;Err [cm];Events",300,-0.5,2.5);
      h = new TH1D("VT_vtx_vtxPositionErrorModule","Vertex position error Module;Err [cm];Events",300,-0.5,2.5);

      h = new TH1D("VT_trk_num","Number of vtx tracks for the BM matched vertex per event;Number of vtx tracks;Events",21,-0.5,20.5);
      h2 = new TH2D("vtx_target_vtxsys","vtx tracks on target  projections in vtx sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("vtx_target_glbsys","vtx tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h = new TH1D("AlignWrtTarget_slopeX_glbsys","VT mx in glb sys ;mx;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_slopeY_glbsys","VT my in glb sys ;my;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_tgposX_glbsys","VT projection on target Xpos in glb sys;X[cm];Events",600,-5.,5.);
      h = new TH1D("AlignWrtTarget_tgposY_glbsys","VT projection on target Ypos in glb sys;Y[cm];Events",600,-5.,5.);
      h = new TH1D("AlignWrtTarget_slopeX_glbsys_afterrot","VT mx in glb sys ;mx;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_slopeY_glbsys_afterrot","VT my in glb sys ;my;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_tgposX_glbsys_afterrot","VT projection on target Xpos in glb sys;X[cm];Events",600,-5.,5.);
      h = new TH1D("AlignWrtTarget_tgposY_glbsys_afterrot","VT projection on target Ypos in glb sys;Y[cm];Events",600,-5.,5.);
      h = new TH1D("AlignWrtTarget_slopeX_glbsys_final","VT mx in glb sys ;mx;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_slopeY_glbsys_final","VT my in glb sys ;my;Events",1000,-0.1,0.1);
      h = new TH1D("AlignWrtTarget_tgposX_glbsys_final","VT projection on target Xpos in glb sys;X[cm];Events",600,-5.,5.);
      h = new TH1D("AlignWrtTarget_tgposY_glbsys_final","VT projection on target Ypos in glb sys;Y[cm];Events",600,-5.,5.);
    gDirectory->cd("..");
    file_out->cd("..");
  }

  if(IncludeMSD){
    file_out->mkdir("MSD");
    file_out->cd("MSD");
    h = new TH1D("msd_cls_num_total","Total number of msd clusters x event;Total number clusters;Events",21,-0.5,20.5);
    for(int i=0;i<msdparGeo->GetSensorsN();i++){
      h = new TH1D(Form("msd_cls_num_%d",i),"Number of msd clusters;Number clusters;Events",21,-0.5,20.5);
      if(i%2==0){
        h = new TH1D(Form("msd_clspos_msdsys_%d",i),"msd cluster position in the msd system;pos [cm];Events",1000,-5.,5.);
        h = new TH1D(Form("msd_clspos_glbsys_%d",i),"msd cluster position in the glb system;pos [cm];Events",1000,-5.,5.);
        //check the msd internal alignment
        h = new TH1D(Form("msd_AllXmeas_%d",i),"MSD point for all the MSD tracks on X view;(poipos).X[cm];Events",250,-5.,5.);
        h = new TH1D(Form("msd_AllYmeas_%d",i),"MSD point for all the MSD tracks on Y view;(poipos).Y[cm];Events",250,-5.,5.);
        h = new TH1D(Form("msd_AllXfit_%d",i),"MSD trk for all the MSD tracks on X view;(poipos).X[cm];Events",250,-5.,5.);
        h = new TH1D(Form("msd_AllYfit_%d",i),"MSD trk for all the MSD tracks on Y view;(poipos).Y[cm];Events",250,-5.,5.);

        h = new TH1D(Form("msd_residualAllX_%d",i),"Residual of MSD tracks with MSD clusters for all the MSD tracks on X view;(Trackpos-cluspos).X[cm];Events",400,-0.02,0.02);
        h = new TH1D(Form("msd_residualAllY_%d",i),"Residual of MSD tracks with MSD clusters for all the MSD tracks on Y view;(Trackpos-cluspos).Y[cm];Events",400,-0.02,0.02);

	h = new TH1D(Form("msd_1TrkXmeas_%d",i),"MSD point for events with 1 MSD track on X view;(trkpos).X[cm];Events",250,-5.,5.);
        h = new TH1D(Form("msd_1TrkYmeas_%d",i),"MSD point for events with 1 MSD track on Y view;(trkpos).Y[cm];Events",250,-5.,5.);
        h = new TH1D(Form("msd_1TrkXfit_%d",i),"MSD point for events with 1 MSD track on X view;(trkpos).X[cm];Events",250,-5.,5.);
        h = new TH1D(Form("msd_1TrkYfit_%d",i),"MSD point for events with 1 MSD track on Y view;(trkpos).Y[cm];Events",250,-5.,5.);

	h = new TH1D(Form("msd_residual1TrkX_%d",i),"Residual of MSD tracks with MSD clusters for the events with 1 MSD track on X view;(Trackpos-cluspos).X[cm];Events",400,-0.02,0.02);
        h = new TH1D(Form("msd_residual1TrkY_%d",i),"Residual of MSD tracks with MSD clusters for the events with 1 MSD track on Y view;(Trackpos-cluspos).Y[cm];Events",400,-0.02,0.02);
      }
    }

    h = new TH1D("msd_trk_num","Number of msd tracks per event;Number of msd tracks;Events",21,-0.5,20.5);
    h = new TH1D("msd_slopeX_msdsys","msd mx in msd sys ;mx;Events",1000,-0.3,0.3);
    h = new TH1D("msd_slopeY_msdsys","msd my in msd sys ;my;Events",1000,-0.3,0.3);
    h = new TH1D("msd_originX_msdsys","msd origin X in msd sys;X[cm];Events",600,-3.,3.);
    h = new TH1D("msd_originY_msdsys","msd origin Y in msd sys;Y[cm];Events",600,-3.,3.);
    h2 = new TH2D("msd_target_msdsys","msd tracks on target  projections in msd sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    h = new TH1D("msd_slopeX_glbsys","msd mx in glb sys ;mx;Events",1000,-0.1,0.1);
    h = new TH1D("msd_slopeY_glbsys","msd my in glb sys ;my;Events",1000,-0.1,0.1);
    h = new TH1D("msd_originX_glbsys","msd origin X in glb sys;X[cm];Events",600,-5.,5.);
    h = new TH1D("msd_originY_glbsys","msd origin Y in glb sys;Y[cm];Events",600,-5.,5.);
    h2 = new TH2D("msd_target_glbsys","msd tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);

    h = new TH1D("AlignWrtTarget_slopeX_glbsys","MSD mx in glb sys ;mx;Events",1000,-0.1,0.1);
    h = new TH1D("AlignWrtTarget_slopeY_glbsys","MSD my in glb sys ;my;Events",1000,-0.1,0.1);
    h = new TH1D("AlignWrtTarget_tgposX_glbsys","MSD projection on target Xpos in glb sys;X[cm];Events",600,-5.,5.);
    h = new TH1D("AlignWrtTarget_tgposY_glbsys","MSD projection on target Ypos in glb sys;Y[cm];Events",600,-5.,5.);
    h = new TH1D("AlignWrtTarget_slopeX_glbsys_afterrot","MSD mx in glb sys ;mx;Events",1000,-0.1,0.1);
    h = new TH1D("AlignWrtTarget_slopeY_glbsys_afterrot","MSD my in glb sys ;my;Events",1000,-0.1,0.1);
    h = new TH1D("AlignWrtTarget_tgposX_glbsys_afterrot","MSD projection on target Xpos in glb sys;X[cm];Events",600,-5.,5.);
    h = new TH1D("AlignWrtTarget_tgposY_glbsys_afterrot","MSD projection on target Ypos in glb sys;Y[cm];Events",600,-5.,5.);
    h = new TH1D("AlignWrtTarget_slopeX_glbsys_final","MSD mx in glb sys ;mx;Events",1000,-0.1,0.1);
    h = new TH1D("AlignWrtTarget_slopeY_glbsys_final","MSD my in glb sys ;my;Events",1000,-0.1,0.1);
    h = new TH1D("AlignWrtTarget_tgposX_glbsys_final","MSD projection on target Xpos in glb sys;X[cm];Events",600,-5.,5.);
    h = new TH1D("AlignWrtTarget_tgposY_glbsys_final","MSD projection on target Ypos in glb sys;Y[cm];Events",600,-5.,5.);
    gDirectory->cd("..");
    file_out->cd("..");
  }

  if(IncludeTW){
    file_out->mkdir("TW");
    file_out->cd("TW");
    h = new TH1D("tw_points_num","Number of tw points per event;Number of points;Events",21,-0.5,20.5);
    h = new TH1D("tw_GetChargeZ","TW points reco charge;Z reco;Events",11,-0.5,10.5);
    h = new TH1D("tw_Xpos_twsys","TW points X position in tw sys;X [cm];Events",61,-30.5,30.5);
    h = new TH1D("tw_Ypos_twsys","TW points Y position in tw sys;Y [cm];Events",61,-30.5,30.5);
    h2 = new TH2D("tw_XYpos_twsys","TW points XY position in tw sys;X[cm];Y[cm]",61,-30.5,30.5,61,-30.5,30.5);
    h = new TH1D("tw_Xpos_glbsys","TW points X position in glb sys;X [cm];Events",61,-30.5,30.5);
    h = new TH1D("tw_Ypos_glbsys","TW points Y position in glb sys;Y [cm];Events",61,-30.5,30.5);
    h2 = new TH2D("tw_XYpos_glbsysAll","TW points XY position in glb sys;X[cm];Y[cm]",61,-30.5,30.5,61,-30.5,30.5);
    h2 = new TH2D("tw_XYpos_glbsysMag","TW points XY position in glb sys for minimum bias;X[cm];Y[cm]",61,-30.5,30.5,61,-30.5,30.5);
    h2 = new TH2D("tw_barmultAll","TW bar map all;FrontBar X;RearBar Y",20,-0.5,19.5,20,0.,19.5);
    gDirectory->cd("..");
    file_out->cd("..");
  }

  if(IncludeDAQ){
    file_out->mkdir("DAQ");
    file_out->cd("DAQ");
    h = new TH1D("daq_acqrate","Mean acquisition rate with 1/timeSinceLastTrigger;Daq acquisition rate [khz];events",400,0.,10.);
    gDirectory->cd("..");
    file_out->cd("..");
  }

  //check the vtx syncronization
  file_out->mkdir("VTXSYNC");
  file_out->cd("VTXSYNC");
  h2 = new TH2D("origin_xx_bmvtx_all","BM originX vs VTX originX for all the evts;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_xx_bmvtx_synch","BM originX vs VTX originX when they are synch;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_xx_bmvtx_unsynch","BM originX vs VTX originX current bunch of events;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_xx_bmvtx_lost","BM originX vs VTX originX after the unsync;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
  gDirectory->cd("..");
  file_out->cd("..");

  file_out->mkdir("BMVT");
  file_out->cd("BMVT");
  h = new TH1D("slopeX_diff_start","angle difference in glb frame;Angle[rad];Events",2000,-0.3,0.3);
  h = new TH1D("slopeY_diff_start","angle difference in glb frame;Angle[rad];Events",2000,-0.3,0.3);
  h = new TH1D("tgprojX_diff_start","target projection residuals Xpos;resX [cm];Events",1000,-5.,5.);
  h = new TH1D("tgprojY_diff_start","target projection residuals Ypos;resY [cm];Events",1000,-5.,5.);
  h = new TH1D("slopeX_diff_afterrot","angle difference in glb frame after rotation;AngleX[rad];Events",2000,-0.3,0.3);
  h = new TH1D("slopeY_diff_afterrot","angle difference in glb frame after rotation;AngleY[rad];Events",2000,-0.3,0.3);
  h = new TH1D("tgprojX_diff_afterrot","target projection residualX after rotation;resX[cm];Events",1000,-5.,5.);
  h = new TH1D("tgprojY_diff_afterrot","target projection residualY after rotation;resY[cm];Events",1000,-5.,5.);
  h = new TH1D("slopeX_diff_final","angle difference in glb frame with new geo pars;AngleX[rad];Events",2000,-0.3,0.3);
  h = new TH1D("slopeY_diff_final","angle difference in glb frame with new geo pars;AngleY[rad];Events",2000,-0.3,0.3);
  h = new TH1D("tgprojX_diff_final","target projection residualX with new geo pars;resX[cm];Events",1000,-5.,5.);
  h = new TH1D("tgprojY_diff_final","target projection residualY with new geo pars;resY[cm];Events",1000,-5.,5.);
  gDirectory->cd("..");
  file_out->cd("..");

  file_out->mkdir("MSDVT");
  file_out->cd("MSDVT");
  h = new TH1D("slopeX_diff_start","angle difference in glb frame;Angle[rad];Events",2000,-0.3,0.3);
  h = new TH1D("slopeY_diff_start","angle difference in glb frame;Angle[rad];Events",2000,-0.3,0.3);
  h = new TH1D("tgprojX_diff_start","target projection residuals Xpos;resX [cm];Events",1000,-5.,5.);
  h = new TH1D("tgprojY_diff_start","target projection residuals Ypos;resY [cm];Events",1000,-5.,5.);
  h = new TH1D("slopeX_diff_afterrot","angle difference in glb frame after rotation;AngleX[rad];Events",2000,-0.3,0.3);
  h = new TH1D("slopeY_diff_afterrot","angle difference in glb frame after rotation;AngleY[rad];Events",2000,-0.3,0.3);
  h = new TH1D("tgprojX_diff_afterrot","target projection residualX after rotation;resX[cm];Events",1000,-5.,5.);
  h = new TH1D("tgprojY_diff_afterrot","target projection residualY after rotation;resY[cm];Events",1000,-5.,5.);
  h = new TH1D("slopeX_diff_final","angle difference in glb frame with new geo pars;AngleX[rad];Events",2000,-0.3,0.3);
  h = new TH1D("slopeY_diff_final","angle difference in glb frame with new geo pars;AngleY[rad];Events",2000,-0.3,0.3);
  h = new TH1D("tgprojX_diff_final","target projection residualX with new geo pars;resX[cm];Events",1000,-5.,5.);
  h = new TH1D("tgprojY_diff_final","target projection residualY with new geo pars;resY[cm];Events",1000,-5.,5.);
  gDirectory->cd("..");
  file_out->cd("..");


  if(debug>0)
    cout<<"Booking done"<<endl;

  return;
}

//BM analysis
void BeamMonitor(){

  if(debug>0)
    cout<<"BeamMonitor start"<<endl;

  //BM reconstructed analysis
    Int_t nbmHits  = bmNtuHit->GetHitsN();
    myfill("BM/BM_Hit_num",nbmHits);

    ///BM tracks
    int  Nbmtrack = bmNtuTrack->GetTracksN();
    for( Int_t iTrack = 0; iTrack < bmNtuTrack->GetTracksN(); ++iTrack ) {
      TABMtrack* track = bmNtuTrack->GetTrack(iTrack);

      //project to the target in the BM ref., then move to the global ref.
      TVector3 bmlocalproj=ProjectToZ(track->GetSlope(), track->GetOrigin(),geoTrafo->FromGlobalToBMLocal(geoTrafo->GetTGCenter()).Z());
      TVector3 bmgloproj=ProjectToZ(geoTrafo->VecFromBMLocalToGlobal(track->GetSlope()), geoTrafo->FromBMLocalToGlobal(track->GetOrigin()),geoTrafo->GetTGCenter().Z());
      myfill("BM/bm_target_bmsys", bmlocalproj.X(),bmlocalproj.Y());
      myfill("BM/bm_target_glbsys", bmgloproj.X(),bmgloproj.Y());
    }

  if(debug>0)
    cout<<"BeamMonitor done"<<endl;

  return;
}

//Vertex analysis
void Vertex(){

  if(debug>0)
    cout<<"Vertex start"<<endl;

  TAVTvertex* vtxvertex   = 0x0;
  TVector3 vtxvertpos;
  TAVTvertex* vtxmatchvertex=nullptr;

  myfill("VT/VT_vtx_num",vtxNtuVertex->GetVertexN());
  for (Int_t iVtx = 0; iVtx < vtxNtuVertex->GetVertexN(); ++iVtx) {
    vtxvertex = vtxNtuVertex->GetVertex(iVtx);
    if (vtxvertex == 0x0)
      continue;
    vtxvertpos = vtxvertex->GetVertexPosition();
    vtxvertpos = geoTrafo->FromVTLocalToGlobal(vtxvertpos);
    myfill("VT/VT_vtx_profile",vtxvertpos.X(),vtxvertpos.Y());
    myfill("VT/VT_vtx_trknum",vtxvertex->GetTracksN());
    myfill("VT/VT_vtx_BMmatched",vtxvertex->IsBmMatched());
    vtxmatchvertex=(vtxvertex->IsBmMatched()) ? vtxNtuVertex->GetVertex(iVtx) : nullptr;
  }

  if(debug>0)
    cout<<"VTX vertex done, start loop on VTX tracks for the BM matched vertex"<<endl;

  //Vertex tracks
  if(vtxmatchvertex!=nullptr){
    myfill("VT/VT_trk_num",vtxmatchvertex->GetTracksN());
    for( Int_t iTrack = 0; iTrack < vtxmatchvertex->GetTracksN(); ++iTrack ) { //loop on vertex tracks

      TAVTtrack* track = vtxmatchvertex->GetTrack(iTrack);
      TVector3 vtxlocalproj=ProjectToZ(track->GetSlopeZ(), track->GetOrigin(),geoTrafo->FromGlobalToVTLocal(geoTrafo->GetTGCenter()).Z());
      myfill("VT/vtx_target_vtxsys",vtxlocalproj.X(),vtxlocalproj.Y());
      TVector3 glbslope = geoTrafo->VecFromVTLocalToGlobal(track->GetSlopeZ());
      TVector3 glborigin = geoTrafo->FromVTLocalToGlobal(track->GetOrigin());
      TVector3 vtxprojglo=geoTrafo->FromVTLocalToGlobal(vtxlocalproj);
      myfill("VT/vtx_target_glbsys",vtxprojglo.X(),vtxprojglo.Y());
    }
  }

  if(debug>0)
    cout<<"Vertex done"<<endl;

  return;
}

//MSD analysis
void MSD(){

  if(debug>0)
    cout<<"MSD start"<<endl;

  Int_t totalmsdnclus=0;
  for (int i = 0; i < msdparGeo->GetSensorsN(); i++) {
    totalmsdnclus+=msdntuclus->GetClustersN(i);
    myfill(Form("MSD/msd_cls_num_%d",i),msdntuclus->GetClustersN(i));
    if(i%2==0){
      for(int k=0;k<msdntuclus->GetClustersN(i);k++){
        TAMSDcluster *msdclus=msdntuclus->GetCluster(i,k);
	//see forward comment
        myfill(Form("MSD/msd_clspos_msdsys_%d",i), msdclus->GetPosition().Y());
        myfill(Form("MSD/msd_clspos_glbsys_%d",i), msdclus->GetPositionG().Y());
      }
    }
  }
  myfill("MSD/msd_cls_num_total",totalmsdnclus);

  //loop on msd tracks
  myfill("MSD/msd_trk_num",msdntutrack->GetTracksN());
  for( Int_t iTrack = 0; iTrack < msdntutrack->GetTracksN(); ++iTrack ) { //loop on msd tracks
    TAMSDtrack* track = msdntutrack->GetTrack(iTrack);
    TVector3 msdlocalproj=ProjectToZ(track->GetSlopeZ(), track->GetOrigin(),geoTrafo->FromGlobalToMSDLocal(geoTrafo->GetTGCenter()).Z());
    myfill("MSD/msd_slopeX_msdsys",track->GetSlopeZ().X());
    myfill("MSD/msd_slopeY_msdsys",track->GetSlopeZ().Y());
    myfill("MSD/msd_originX_msdsys",track->GetOrigin().X());
    myfill("MSD/msd_originY_msdsys",track->GetOrigin().Y());
    myfill("MSD/msd_target_msdsys",msdlocalproj.X(),msdlocalproj.Y());
    TVector3 glbslope = geoTrafo->VecFromMSDLocalToGlobal(track->GetSlopeZ());
    TVector3 glborigin = geoTrafo->FromMSDLocalToGlobal(track->GetOrigin());
    TVector3 msdprojglo=geoTrafo->FromMSDLocalToGlobal(msdlocalproj);
    myfill("MSD/msd_slopeX_glbsys",glbslope.X());
    myfill("MSD/msd_slopeY_glbsys",glbslope.Y());
    myfill("MSD/msd_originX_glbsys",glborigin.X());
    myfill("MSD/msd_originY_glbsys",glborigin.Y());
    myfill("MSD/msd_target_glbsys",msdprojglo.X(),msdprojglo.Y());

    //check the msd internal alignment
    for(int k=0;k<track->GetClustersN();k++){
      TAMSDpoint* msdpoi = (TAMSDpoint*)track->GetCluster(k);
      //WARNING!! in theory GetPosition() should return the position in the local frame and GetPositionG should return the position in the global frame, but at the moment GetPosition() returns a (0,0,0) vector and GetPositionG return the position in the local frame, once it will be fixed, please modify here!
      TVector3 poipos=msdpoi->GetPositionG();
      TVector3 trkpos=ProjectToZ(track->GetSlopeZ(), track->GetOrigin(),poipos.Z());
      if(msdpoi->GetSensorIdx()%2==0){
        myfill(Form("MSD/msd_AllXmeas_%d",msdpoi->GetSensorIdx()),poipos.X());
        myfill(Form("MSD/msd_AllYmeas_%d",msdpoi->GetSensorIdx()),poipos.Y());
        myfill(Form("MSD/msd_AllXfit_%d",msdpoi->GetSensorIdx()),trkpos.X());
        myfill(Form("MSD/msd_AllYfit_%d",msdpoi->GetSensorIdx()),trkpos.Y());
        myfill(Form("MSD/msd_residualAllX_%d",msdpoi->GetSensorIdx()),trkpos.X()-poipos.X());
        myfill(Form("MSD/msd_residualAllY_%d",msdpoi->GetSensorIdx()),trkpos.Y()-poipos.Y());
        if(msdntutrack->GetTracksN()==1){
        myfill(Form("MSD/msd_1TrkXmeas_%d",msdpoi->GetSensorIdx()),poipos.X());
        myfill(Form("MSD/msd_1TrkYmeas_%d",msdpoi->GetSensorIdx()),poipos.Y());
        myfill(Form("MSD/msd_1TrkXfit_%d",msdpoi->GetSensorIdx()),trkpos.X());
        myfill(Form("MSD/msd_1TrkYfit_%d",msdpoi->GetSensorIdx()),trkpos.Y());
          myfill(Form("MSD/msd_residual1TrkX_%d",msdpoi->GetSensorIdx()),trkpos.X()-poipos.X());
          myfill(Form("MSD/msd_residual1TrkY_%d",msdpoi->GetSensorIdx()),trkpos.Y()-poipos.Y());
        }
      }else{
        cout<<"WARNING!!! in MSD()::msdpoi->GetSensorIdx()="<<msdpoi->GetSensorIdx()<<endl;
        cout<<"When this code has been written, all the TAMSDpoint are related to an even number of sensor index. Something changed in TAMSDpoint, please check and update the macro!!!"<<endl;
      }
    }
  }

  if(debug>0)
    cout<<"MSD done"<<endl;

  return;
}

//TofWall analysis
void TofWall(){

  if(debug>0)
    cout<<"TofWall start"<<endl;

  myfill("TW/tw_points_num",twNtuPoint->GetPointsN());
  for (int i = 0; i < twNtuPoint->GetPointsN(); i++) {
    TATWpoint *point = twNtuPoint->GetPoint(i);
    myfill("TW/tw_Xpos_twsys",point->GetPosition().X());
    myfill("TW/tw_Ypos_twsys",point->GetPosition().Y());
    myfill("TW/tw_XYpos_twsys",point->GetPosition().X(),point->GetPosition().Y());
    myfill("TW/tw_Xpos_glbsys",point->GetPositionGlb().X());
    myfill("TW/tw_Ypos_glbsys",point->GetPositionGlb().Y());
    myfill("TW/tw_XYpos_glbsysAll",point->GetPositionGlb().X(),point->GetPositionGlb().Y());
    myfill("TW/tw_barmultAll",point->GetRowID(),point->GetColumnID());
    myfill("TW/tw_GetChargeZ",point->GetChargeZ());
  }

  if(debug>0)
    cout<<"TofWall done"<<endl;

  return;
}

void DataAcquisition(){

  if(debug)
    cout<<"DataAcquisition analysis start"<<endl;

  myfill("DAQ/daq_acqrate",100./tgevent->GetTimeSinceLastTrigger()); //tgevent->GetTimeSinceLastTrigger() is in decisec.

  if(debug)
    cout<<"DataAcquisition done"<<endl;
  return;
}



//check the VTX sync with the BM
void VTXSYNC(){

  if(debug)
    cout<<"VTXSYNC analysis start"<<endl;

  if(vtxsynch==0 && evnum%checkrate==0 && evnum>1){// check the synchronization every checkrate events
    Double_t corr=((TH2D*)gDirectory->Get("VTXSYNC/origin_xx_bmvtx_unsynch"))->GetCorrelationFactor();
    if(corr<0.5){
      cout<<"BM-VT correlation lost: correlation factor:"<<corr<<endl;
      cout<<"The synchronization has been lost somewhere between events number"<<evnum-checkrate<<" and "<<evnum<<endl;
      vtxsynch=evnum-checkrate-1; //the -1 is to take into account the case in which the synch is lost from the beginning
      ((TH2D*)gDirectory->Get("VTXSYNC/origin_xx_bmvtx_unsynch"))->SetTitle(Form("BM originX vs VTX originX when start to unsync from events %d to events %d",evnum-checkrate,evnum));
      ((TH2D*)gDirectory->Get("VTXSYNC/origin_xx_bmvtx_synch"))->SetTitle(Form("BM originX vs VTX originX when they are synch till events %d",evnum-checkrate-1));
      ((TH2D*)gDirectory->Get("VTXSYNC/origin_xx_bmvtx_lost"))->SetTitle(Form("BM originX vs VTX originX after the unsync from events %d",evnum));
    }else{
      ((TH2D*)gDirectory->Get("VTXSYNC/origin_xx_bmvtx_synch"))->Add(((TH2D*)gDirectory->Get("VTXSYNC/origin_xx_bmvtx_unsynch")));
      ((TH2D*)gDirectory->Get("VTXSYNC/origin_xx_bmvtx_unsynch"))->Reset("ICESM");
    }
  }

  //select events with only one BM track and one vtx track
  if(bmNtuTrack->GetTracksN()!=1)
    return;
  if(vtxNtuVertex->GetVertexN()!=1)
    return;
  TAVTvertex *vtxvertex = vtxNtuVertex->GetVertex(0);
  if(vtxvertex->GetTracksN()!=1)
    return;

  TAVTtrack *vttrack=vtxvertex->GetTrack(0);
  TABMtrack* bmtrack = bmNtuTrack->GetTrack(0);
  myfill("VTXSYNC/origin_xx_bmvtx_all",bmtrack->GetOrigin().X(),vttrack->GetOrigin().X());
  myfill((vtxsynch==0) ? "VTXSYNC/origin_xx_bmvtx_unsynch":"VTXSYNC/origin_xx_bmvtx_lost",bmtrack->GetOrigin().X(),vttrack->GetOrigin().X());

  if(debug)
    cout<<"VTXSYNC analysis done"<<endl;

  return;
}


void FillTrackVect(vector<beamtrk> &bmtrk, vector<beamtrk> &vttrk, vector<beamtrk> &msdtrk){

  //define some selection criteria with TW infos
  if(twNtuPoint->GetPointsN()!=1)
    return;
  TATWpoint *twpoint = twNtuPoint->GetPoint(0);
  if(twpoint->GetChargeZ()!=Zbeam)
    return;

  //fill BM tracks
  if(IncludeBM){
    if(bmNtuTrack->GetTracksN()==1){
      TABMtrack* bmtrack = bmNtuTrack->GetTrack(0);
      TVector3 bmslopeglo=geoTrafo->VecFromBMLocalToGlobal(bmtrack->GetSlope());
      TVector3 bmoriginglo=geoTrafo->FromBMLocalToGlobal(bmtrack->GetOrigin());
      TVector3 bmtgpos=ProjectToZ(bmslopeglo, bmoriginglo,geoTrafo->GetTGCenter().Z());
      beamtrk bmstrct={bmtrack->GetSlope(),bmtrack->GetOrigin(), geoTrafo->VecFromBMLocalToGlobal(bmtrack->GetSlope()), bmtgpos, evnum};
      bmtrk.push_back(bmstrct);
    }
  }

  //fill VT tracks
  if(IncludeVT){
    if(vtxNtuVertex->GetVertexN()==1){
      TAVTvertex *vtxvertex = vtxNtuVertex->GetVertex(0);
      if(vtxvertex->GetTracksN()==1){
        TAVTtrack *vttrack=vtxvertex->GetTrack(0);
        TVector3 vtslopeglo=geoTrafo->VecFromVTLocalToGlobal(vttrack->GetSlopeZ());
        TVector3 vtoriginglo=geoTrafo->FromVTLocalToGlobal(vttrack->GetOrigin());
        TVector3 vttgpos=ProjectToZ(vtslopeglo, vtoriginglo,geoTrafo->GetTGCenter().Z());
        beamtrk vtstrct={vttrack->GetSlopeZ(), vttrack->GetOrigin(), vtslopeglo, vttgpos, evnum};
        vttrk.push_back(vtstrct);
      }
    }
  }

  //fill MSD Tracks
  if(IncludeMSD){
    if(msdntutrack->GetTracksN()==1){
      TAMSDtrack* msdtrack = msdntutrack->GetTrack(0);
      TVector3 msdslopeglo=geoTrafo->VecFromMSDLocalToGlobal(msdtrack->GetSlopeZ());
      TVector3 msdoriginglo=geoTrafo->FromMSDLocalToGlobal(msdtrack->GetOrigin());
      TVector3 msdtgpos=ProjectToZ(msdslopeglo, msdoriginglo,geoTrafo->GetTGCenter().Z());
      beamtrk msdstrct={msdtrack->GetSlopeZ(),msdtrack->GetOrigin(), msdslopeglo, msdtgpos, evnum};
      msdtrk.push_back(msdstrct);
    }
  }

  return;
}


void AlignWrtTarget(vector<beamtrk> &dettrk, TString detname) {

  if(debug)
    cout<<"AlignWrtTarget analysis start with the "<<detname.Data()<<endl;

  TString slopexname=detname+"/AlignWrtTarget_slopeX_glbsys";
  TString slopeyname=detname+"/AlignWrtTarget_slopeY_glbsys";
  TString tgposxname=detname+"/AlignWrtTarget_tgposX_glbsys";
  TString tgposyname=detname+"/AlignWrtTarget_tgposY_glbsys";

  for(Int_t k=0;k<dettrk.size();k++){
    myfill(slopexname.Data(),dettrk.at(k).slopeglo.X()/dettrk.at(k).slopeglo.Z());
    myfill(slopeyname.Data(),dettrk.at(k).slopeglo.Y()/dettrk.at(k).slopeglo.Z());
    myfill(tgposxname.Data(),dettrk.at(k).tgposglo.X());
    myfill(tgposyname.Data(),dettrk.at(k).tgposglo.Y());
  }


  TF1 *gaus=new TF1("gaus","gaus", -3., 3.);

  cout<<detname.Data()<<" starting position and angles:"<<endl;
  cout<<"PosX: "<<geoTrafo->GetDeviceCenter(detname.Data()).X()<<" PosY: "<<geoTrafo->GetDeviceCenter(detname.Data()).Y()<<" PosZ: "<<geoTrafo->GetDeviceCenter(detname.Data()).Z()<<endl;
  cout<<"AngX: "<<geoTrafo->GetDeviceAngle(detname.Data()).X()<<" AngY: "<<geoTrafo->GetDeviceAngle(detname.Data()).Y()<<" AngZ: "<<geoTrafo->GetDeviceAngle(detname.Data()).Z()<<endl;

  gaus->SetParameters(((TH1D*)gDirectory->Get(slopexname))->GetEntries(),((TH1D*)gDirectory->Get(slopexname))->GetMean(), ((TH1D*)gDirectory->Get(slopexname))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopexname))->Fit("gaus","","QB+",-0.3,0.3);
  Double_t resyrot=gaus->GetParameter(1);
  Double_t yrotpar=-atan(gaus->GetParameter(1))*RAD2DEG;

  gaus->SetParameters(((TH1D*)gDirectory->Get(slopeyname))->GetEntries(),((TH1D*)gDirectory->Get(slopeyname))->GetMean(), ((TH1D*)gDirectory->Get(slopeyname))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopeyname))->Fit("gaus","","QB+",-0.3,0.3);
  Double_t resxrot=gaus->GetParameter(1);
  Double_t xrotpar=atan(gaus->GetParameter(1))*RAD2DEG;

  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposxname))->GetEntries(),((TH1D*)gDirectory->Get(tgposxname))->GetMean(), ((TH1D*)gDirectory->Get(tgposxname))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposxname))->Fit("gaus","","QB+",-2,2);
  Double_t resxtra=gaus->GetParameter(1);

  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposyname))->GetEntries(),((TH1D*)gDirectory->Get(tgposyname))->GetMean(), ((TH1D*)gDirectory->Get(tgposyname))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposyname))->Fit("gaus","","QB+",-2,2);
  Double_t resytra=gaus->GetParameter(1);

  cout<<"AlignWrtTarget:: first residual parameters:"<<endl;
  cout<<"resxtra="<<resxtra<<"  resytra="<<resytra<<endl;
  cout<<"resxrot="<<resxrot<<"  resyrot="<<resyrot<<endl;
  TVector3 detcenter = geoTrafo->GetDeviceCenter(detname.Data());
  TVector3 detnewangle(xrotpar+geoTrafo->GetDeviceAngle(detname.Data()).X(),yrotpar+geoTrafo->GetDeviceAngle(detname.Data()).Y(),geoTrafo->GetDeviceAngle(detname.Data()).Z());
  cout<<"new "<<detname.Data()<<" rotation parameters:"<<endl;
  cout<<detname.Data()<<"AngX: "<<detnewangle.X()<<"  "<<detname.Data()<<"AngY: "<<detnewangle.Y()<<endl;

  Double_t loc[] = {0.,0.,0};
  Double_t glo[] = {0.,0.,0};

  //define the trasformation matrix
  TGeoRotation detrot;
  detrot.RotateX(detnewangle.X());
  detrot.RotateY(detnewangle.Y());
  loc[0] = detcenter.X();
  loc[1] = detcenter.Y();
  loc[2] = detcenter.Z();
  glo[0] = 0.;
  glo[1] = 0.;
  glo[2] = 0.;
  detrot.LocalToMaster(loc, glo);
  TGeoTranslation dettrans(glo[0], glo[1], glo[2]);
  TGeoHMatrix  dettransfo;
  dettransfo=dettrans;
  dettransfo*=detrot;

  slopexname=detname+"/AlignWrtTarget_slopeX_glbsys_afterrot";
  slopeyname=detname+"/AlignWrtTarget_slopeY_glbsys_afterrot";
  tgposxname=detname+"/AlignWrtTarget_tgposX_glbsys_afterrot";
  tgposyname=detname+"/AlignWrtTarget_tgposY_glbsys_afterrot";

  for(Int_t k=0;k<dettrk.size();k++){
    Double_t slope_loc[3]={dettrk.at(k).slopeloc.X(),dettrk.at(k).slopeloc.Y(),dettrk.at(k).slopeloc.Z()};
    Double_t origin_loc[3]={dettrk.at(k).originloc.X(),dettrk.at(k).originloc.Y(),dettrk.at(k).originloc.Z()};
    Double_t slope_glo[3] = {0., 0., 0.};
    Double_t origin_glo[3] = {0., 0., 0.};
    dettransfo.LocalToMasterVect(slope_loc, slope_glo);
    dettransfo.LocalToMaster(origin_loc,origin_glo);
    TVector3 slope_vecglo(slope_glo[0], slope_glo[1],slope_glo[2]);
    TVector3 origin_vecglo(origin_glo[0], origin_glo[1], origin_glo[2]);
    TVector3 projtg_glo=ProjectToZ(slope_vecglo, origin_vecglo,geoTrafo->GetTGCenter().Z());
    myfill(slopexname.Data(),slope_vecglo.X()/slope_vecglo.Z());
    myfill(slopeyname.Data(),slope_vecglo.Y()/slope_vecglo.Z());
    myfill(tgposxname.Data(),projtg_glo.X());
    myfill(tgposyname.Data(),projtg_glo.Y());
  }

  gaus->SetParameters(((TH1D*)gDirectory->Get(slopexname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(slopexname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(slopexname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopexname.Data()))->Fit("gaus","","QB+",-0.3,0.3);
  resxrot=gaus->GetParameter(1);
  gaus->SetParameters(((TH1D*)gDirectory->Get(slopeyname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(slopeyname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(slopeyname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopeyname.Data()))->Fit("gaus","","QB+",-0.3,0.3);
  resyrot=gaus->GetParameter(1);
  cout<<"AlignWrtTarget::after the rotation matrix the new residuals are:"<<endl;
  cout<<"resxrot="<<resxrot<<"  resyrot="<<resyrot<<endl;

  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposxname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(tgposxname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(tgposxname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposxname.Data()))->Fit("gaus","","QB+",-2.,2.);
  Double_t newresxtra=gaus->GetParameter(1);
  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposyname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(tgposyname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(tgposyname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposyname.Data()))->Fit("gaus","","QB+",-2.,2.);
  Double_t newresytra=gaus->GetParameter(1);
  cout<<"newresxtra="<<newresxtra<<"  newresytra="<<newresytra<<endl;

  loc[0] = 0.;
  loc[1] = 0.;
  loc[2] = 0.;
  glo[0] = -newresxtra;
  glo[1] = -newresytra;
  glo[2] = 0.;
  detrot.MasterToLocal(glo,loc);

  TVector3 detfinalpos(loc[0]+detcenter.X(),loc[1]+detcenter.Y(),loc[2]+detcenter.Z());
  cout<<detname.Data()<<"new position estimate PosX: "<<detfinalpos.X()<<"  "<<detname.Data()<<"PosY= "<<detfinalpos.Y()<<"  "<<detname.Data()<<"PosZ= "<<detfinalpos.Z()<<endl;

  //define the trasformation matrix
  TGeoRotation detfinalrot;
  detfinalrot.RotateX(detnewangle.X());
  detfinalrot.RotateY(detnewangle.Y());
  loc[0] = detfinalpos.X();
  loc[1] = detfinalpos.Y();
  loc[2] = detfinalpos.Z();
  glo[0] = 0.;
  glo[1] = 0.;
  glo[2] = 0.;
  detfinalrot.LocalToMaster(loc, glo);
  TGeoTranslation detfinaltrans(glo[0], glo[1], glo[2]);
  TGeoHMatrix  detfinaltransfo;
  detfinaltransfo=detfinaltrans;
  detfinaltransfo*=detfinalrot;


  slopexname=detname+"/AlignWrtTarget_slopeX_glbsys_final";
  slopeyname=detname+"/AlignWrtTarget_slopeY_glbsys_final";
  tgposxname=detname+"/AlignWrtTarget_tgposX_glbsys_final";
  tgposyname=detname+"/AlignWrtTarget_tgposY_glbsys_final";

  for(Int_t k=0;k<dettrk.size();k++){
    Double_t slope_loc[3]={dettrk.at(k).slopeloc.X(),dettrk.at(k).slopeloc.Y(),dettrk.at(k).slopeloc.Z()};
    Double_t origin_loc[3]={dettrk.at(k).originloc.X(),dettrk.at(k).originloc.Y(),dettrk.at(k).originloc.Z()};
    Double_t slope_glo[3] = {0., 0., 0.};
    Double_t origin_glo[3] = {0., 0., 0.};
    detfinaltransfo.LocalToMasterVect(slope_loc, slope_glo);
    detfinaltransfo.LocalToMaster(origin_loc,origin_glo);
    TVector3 slope_vecglo(slope_glo[0], slope_glo[1],slope_glo[2]);
    TVector3 origin_vecglo(origin_glo[0], origin_glo[1], origin_glo[2]);
    TVector3 projtg_glo=ProjectToZ(slope_vecglo, origin_vecglo,geoTrafo->GetTGCenter().Z());
    myfill(slopexname.Data(),slope_vecglo.X()/slope_vecglo.Z());
    myfill(slopeyname.Data(),slope_vecglo.Y()/slope_vecglo.Z());
    myfill(tgposxname.Data(),projtg_glo.X());
    myfill(tgposyname.Data(),projtg_glo.Y());
  }

  gaus->SetParameters(((TH1D*)gDirectory->Get(slopexname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(slopexname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(slopexname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopexname.Data()))->Fit("gaus","","QB+",-0.3,0.3);
  Double_t finalresxrot=gaus->GetParameter(1);
  gaus->SetParameters(((TH1D*)gDirectory->Get(slopeyname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(slopeyname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(slopeyname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopeyname.Data()))->Fit("gaus","","QB+",-0.3,0.3);
  Double_t finalresyrot=gaus->GetParameter(1);

  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposxname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(tgposxname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(tgposxname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposxname.Data()))->Fit("gaus","","QB+",-2.,2.);
  Double_t finalresxtra=gaus->GetParameter(1);
  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposyname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(tgposyname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(tgposyname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposyname.Data()))->Fit("gaus","","QB+",-2.,2.);
  Double_t finalresytra=gaus->GetParameter(1);

  cout<<endl<<endl<<endl;
  cout<<"AlignWrtTarget::final results for "<<detname.Data()<<endl;
  cout<<detname.Data()<<"new position estimate:"<<endl;
  cout<<detname.Data()<<"PosX: "<<detfinalpos.X()<<"  "<<detname.Data()<<"PosY: "<<detfinalpos.Y()<<"  "<<detname.Data()<<"PosZ: "<<detfinalpos.Z()<<endl;
  cout<<detname.Data()<<"new rotation estimate:"<<endl;
  cout<<detname.Data()<<"AngX: "<<detnewangle.X()<<"  "<<detname.Data()<<"AngY: "<<detnewangle.Y()<<"  "<<detname.Data()<<"AngZ: "<<detnewangle.Z()<<endl;
  cout<<"residual on traslations with the new geometrical parameters: finalresxtra="<<finalresxtra<<" finalresytra="<<finalresytra<<endl;
  cout<<"residual on rotations with the new geometrical parameters: finalresxrot="<<finalresxrot<<" finalresyrot="<<finalresyrot<<endl<<endl;


  if(debug)
    cout<<"AlignWrtTarget analysis done with the "<<detname.Data()<<endl;
}



//find the align parameters of detector A with respect to detector B (that is fixed)
//detname: name of the folder where to store the plots (the plots must be already declared in Booking())
//detacenter: center of the detector A in the global frame
void AlignDetaVsDetb(vector<beamtrk> &detatrk, vector<beamtrk> &detbtrk, TString detname, TVector3 detacenter, TVector3 detaangle){

  if(debug)
    cout<<"AlignDetaVsDetb analysis start with detname="<<detname.Data()<<endl;


  TString slopexname=detname+"/slopeX_diff_start";
  TString slopeyname=detname+"/slopeY_diff_start";
  TString tgposxname=detname+"/tgprojX_diff_start";
  TString tgposyname=detname+"/tgprojY_diff_start";

  //loop to fill the initial residual plots
  int indexb=0;
  for(int i=0;i<detatrk.size();i++){
    for(int k=indexb;k<detbtrk.size();k++){
      if(detatrk.at(i).evts==detbtrk.at(k).evts){
        myfill(slopexname.Data(),detatrk.at(i).slopeglo.X()/detatrk.at(i).slopeglo.Z()-detbtrk.at(k).slopeglo.X()/detbtrk.at(k).slopeglo.Z());
        myfill(slopeyname.Data(),detatrk.at(i).slopeglo.Y()/detatrk.at(i).slopeglo.Z()-detbtrk.at(k).slopeglo.Y()/detbtrk.at(k).slopeglo.Z());
        myfill(tgposxname.Data(),detatrk.at(i).tgposglo.X()-detbtrk.at(k).tgposglo.X());
        myfill(tgposyname.Data(),detatrk.at(i).tgposglo.Y()-detbtrk.at(k).tgposglo.Y());
        indexb=k+1;
        break;
      }
      if(detatrk.at(i).evts<detbtrk.at(k).evts){
        indexb=k;
        break;
      }
    }
  }


  TF1 *gaus=new TF1("gaus","gaus", -3., 3.);
  TVector3 detanewpos, detanewangle;

  //initial residual btw deta and detb:
  gaus->SetParameters(((TH1D*)gDirectory->Get(slopexname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(slopexname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(slopexname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopexname.Data()))->Fit("gaus","","QB+",-0.3,0.3);
  Double_t resyrot=gaus->GetParameter(1);
  Double_t yrotpar=-atan(gaus->GetParameter(1))*RAD2DEG;
  gaus->SetParameters(((TH1D*)gDirectory->Get(slopeyname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(slopeyname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(slopeyname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopeyname.Data()))->Fit("gaus","","QB+",-0.3,0.3);
  Double_t resxrot=gaus->GetParameter(1);
  Double_t xrotpar=atan(gaus->GetParameter(1))*RAD2DEG;

  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposxname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(tgposxname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(tgposxname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposxname.Data()))->Fit("gaus","","QB+",-2.,2.);
  Double_t resxtra=gaus->GetParameter(1);
  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposyname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(tgposyname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(tgposyname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposyname.Data()))->Fit("gaus","","QB+",-2.,2.);
  Double_t resytra=gaus->GetParameter(1);

  detanewangle.SetXYZ(xrotpar+detaangle.X(),yrotpar+detaangle.Y(),detaangle.Z());

  cout<<"Initial residuals between "<<detname.Data()<<" with the current geo parameters:"<<endl;
  cout<<"Initial residual on translations: resxtra="<<resxtra<<"  resytra="<<resytra<<endl;
  cout<<"Initial residual on rotations in mx and my: resxrot="<<resxrot<<"  resyrot="<<resyrot<<endl;
  cout<<"Initial residual on rotation in degree: xrotpar="<<xrotpar<<"  yrotpar="<<yrotpar<<endl;
  cout<<"Detector A initial rotation parameters: detaangle.X()="<<detaangle.X()<<"  detaangle.Y()="<<detaangle.Y()<<"  detaangle.Z()="<<detaangle.Z()<<endl;
  cout<<"Detector A new rotation parameters: detanewangle.X()="<<detanewangle.X()<<"  detanewangle.Y()="<<detanewangle.Y()<<"  detanewangle.Z()="<<detanewangle.Z()<<endl;


  Double_t loc[] = {0.,0.,0};
  Double_t glo[] = {0.,0.,0};

  //define the trasformation matrix
  TGeoRotation detarot;
  detarot.RotateX(detanewangle.X());
  detarot.RotateY(detanewangle.Y());
  loc[0] = detacenter.X();
  loc[1] = detacenter.Y();
  loc[2] = detacenter.Z();
  glo[0] = 0.;
  glo[1] = 0.;
  glo[2] = 0.;
  detarot.LocalToMaster(loc, glo);
  //      cout<<"NewAng :: "<<vtnewangle.X()<<" "<<vtnewangle.Y()<<" "<<vtnewangle.Z()<<endl;
  //      cout<<"GLO :: "<<glo[0]<<" "<<glo[1]<<" "<<glo[2]<<endl;
  TGeoTranslation detatrans(glo[0], glo[1], glo[2]);
  TGeoHMatrix  detatransfo;
  detatransfo=detatrans;
  detatransfo*=detarot;

  slopexname=detname+"/slopeX_diff_afterrot";
  slopeyname=detname+"/slopeY_diff_afterrot";
  tgposxname=detname+"/tgprojX_diff_afterrot";
  tgposyname=detname+"/tgprojY_diff_afterrot";

  //loop to fill the initial residual plots
  indexb=0;
  for(int i=0;i<detatrk.size();i++){
    for(int k=indexb;k<detbtrk.size();k++){
      if(detatrk.at(i).evts==detbtrk.at(k).evts){

        Double_t detaoriginloc[3] = {detatrk.at(i).originloc.X(), detatrk.at(i).originloc.Y(), detatrk.at(i).originloc.Z()};
        Double_t detaslopeloc[3] = {detatrk.at(i).slopeloc.X(), detatrk.at(i).slopeloc.Y(), detatrk.at(i).slopeloc.Z()};
        Double_t detaoriginglo[3] = {0., 0., 0.};
        Double_t detaslopeglo[3] = {0., 0., 0.};
        detatransfo.LocalToMaster(detaoriginloc,detaoriginglo);
        detatransfo.LocalToMasterVect(detaslopeloc,detaslopeglo);
        TVector3 detaslope_glo (detaslopeglo[0], detaslopeglo[1], detaslopeglo[2]);
        TVector3 detaorigin_glo (detaoriginglo[0], detaoriginglo[1], detaoriginglo[2]);
        TVector3 detaproj_glo=ProjectToZ(detaslope_glo, detaorigin_glo, geoTrafo->GetTGCenter().Z());
        myfill(slopexname.Data(),detaslope_glo.X()/detaslope_glo.Z()-detbtrk.at(k).slopeglo.X()/detbtrk.at(k).slopeglo.Z());
        myfill(slopeyname.Data(),detaslope_glo.Y()/detaslope_glo.Z()-detbtrk.at(k).slopeglo.Y()/detbtrk.at(k).slopeglo.Z());
        myfill(tgposxname.Data(),detaproj_glo.X()-detbtrk.at(k).tgposglo.X());
        myfill(tgposyname.Data(),detaproj_glo.Y()-detbtrk.at(k).tgposglo.Y());
        indexb=k+1;
        break;
      }
      if(detatrk.at(i).evts<detbtrk.at(k).evts){
        indexb=k;
        break;
      }
    }
  }

  //estimate the new bm geometry parameters
  gaus->SetParameters(((TH1D*)gDirectory->Get(slopexname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(slopexname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(slopexname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopexname.Data()))->Fit("gaus","","QB+",-0.3,0.3);
  Double_t newresxrot=gaus->GetParameter(1);
  gaus->SetParameters(((TH1D*)gDirectory->Get(slopeyname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(slopeyname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(slopeyname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopeyname.Data()))->Fit("gaus","","QB+",-0.3,0.3);
  Double_t newresyrot=gaus->GetParameter(1);

  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposxname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(tgposxname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(tgposxname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposxname.Data()))->Fit("gaus","","QB+",-2.,2.);
  Double_t newresxtra=gaus->GetParameter(1);
  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposyname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(tgposyname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(tgposyname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposyname.Data()))->Fit("gaus","","QB+",-2.,2.);
  Double_t newresytra=gaus->GetParameter(1);

  loc[0] = 0.;
  loc[1] = 0.;
  loc[2] = 0.;
  glo[0] = -newresxtra;
  glo[1] = -newresytra;
  glo[2] = 0.;
  detarot.MasterToLocal(glo,loc);
  detanewpos.SetXYZ(loc[0]+detacenter.X(),loc[1]+detacenter.Y(),loc[2]+detacenter.Z());

  cout<<"first rotation step done with "<<detname.Data()<<endl;
  cout<<"residual on traslations with the new rotation parameters: newresxtra="<<newresxtra<<" newresytra="<<newresytra<<endl;
  cout<<"old position parameters:"<<endl;
  cout<<"OldPosX: "<<detacenter.X()<<"  OldPosY: "<<detacenter.Y()<<"  OldPosZ: "<<detacenter.Z()<<endl;
  cout<<"new position parameters:"<<endl;
  cout<<detname.Data()<<"PosX: "<<detanewpos.X()<<"  "<<detname.Data()<<"PosY: "<<detanewpos.Y()<<"  "<<detname.Data()<<"PosZ: "<<detanewpos.Z()<<endl;
  // cout<<"new rotation parameters: xrotpar="<<xrotpar<<"  yrotpar="<<yrotpar<<endl;
  // cout<<"residuals on rotations with the new geo parameters:"<<endl;
  // cout<<"newresxrot="<<newresxrot<<"  newresyrot="<<newresyrot<<endl;

  //define the trasformation matrix
  TGeoRotation finaldetarot;
  finaldetarot.RotateX(detanewangle.X());
  finaldetarot.RotateY(detanewangle.Y());
  loc[0] = detanewpos.X();
  loc[1] = detanewpos.Y();
  loc[2] = detanewpos.Z();
  glo[0] = 0.;
  glo[1] = 0.;
  glo[2] = 0.;
  finaldetarot.LocalToMaster(loc, glo);
  //      cout<<"NewAng :: "<<vtnewangle.X()<<" "<<vtnewangle.Y()<<" "<<vtnewangle.Z()<<endl;
  //      cout<<"GLO :: "<<glo[0]<<" "<<glo[1]<<" "<<glo[2]<<endl;
  TGeoTranslation finaldetatrans(glo[0], glo[1], glo[2]);
  TGeoHMatrix  finaldetatransfo;
  finaldetatransfo=finaldetatrans;
  finaldetatransfo*=finaldetarot;

  slopexname=detname+"/slopeX_diff_final";
  slopeyname=detname+"/slopeY_diff_final";
  tgposxname=detname+"/tgprojX_diff_final";
  tgposyname=detname+"/tgprojY_diff_final";

  //loop to fill the initial residual plots
  indexb=0;
  for(int i=0;i<detatrk.size();i++){
    for(int k=indexb;k<detbtrk.size();k++){
      if(detatrk.at(i).evts==detbtrk.at(k).evts){

        Double_t detaoriginloc[3] = {detatrk.at(i).originloc.X(), detatrk.at(i).originloc.Y(), detatrk.at(i).originloc.Z()};
        Double_t detaslopeloc[3] = {detatrk.at(i).slopeloc.X(), detatrk.at(i).slopeloc.Y(), detatrk.at(i).slopeloc.Z()};
        Double_t detaoriginglo[3] = {0., 0., 0.};
        Double_t detaslopeglo[3] = {0., 0., 0.};
        finaldetatransfo.LocalToMaster(detaoriginloc,detaoriginglo);
        finaldetatransfo.LocalToMasterVect(detaslopeloc,detaslopeglo);
        TVector3 detaslope_glo (detaslopeglo[0], detaslopeglo[1], detaslopeglo[2]);
        TVector3 detaorigin_glo (detaoriginglo[0], detaoriginglo[1], detaoriginglo[2]);
        TVector3 detaproj_glo=ProjectToZ(detaslope_glo,detaorigin_glo, geoTrafo->GetTGCenter().Z());
        myfill(slopexname.Data(),detaslope_glo.X()/detaslope_glo.Z()-detbtrk.at(k).slopeglo.X()/detbtrk.at(k).slopeglo.Z());
        myfill(slopeyname.Data(),detaslope_glo.Y()/detaslope_glo.Z()-detbtrk.at(k).slopeglo.Y()/detbtrk.at(k).slopeglo.Z());
        myfill(tgposxname.Data(),detaproj_glo.X()-detbtrk.at(k).tgposglo.X());
        myfill(tgposyname.Data(),detaproj_glo.Y()-detbtrk.at(k).tgposglo.Y());
        indexb=k+1;
        break;
      }
      if(detatrk.at(i).evts<detbtrk.at(k).evts){
        indexb=k;
        break;
      }
    }
  }

  //estimate the new bm geometry parameters
  gaus->SetParameters(((TH1D*)gDirectory->Get(slopexname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(slopexname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(slopexname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopexname.Data()))->Fit("gaus","","QB+",-0.3,0.3);
  Double_t finalresxrot=gaus->GetParameter(1);
  gaus->SetParameters(((TH1D*)gDirectory->Get(slopeyname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(slopeyname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(slopeyname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(slopeyname.Data()))->Fit("gaus","","QB+",-0.3,0.3);
  Double_t finalresyrot=gaus->GetParameter(1);

  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposxname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(tgposxname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(tgposxname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposxname.Data()))->Fit("gaus","","QB+",-2.,2.);
  Double_t finalresxtra=gaus->GetParameter(1);
  gaus->SetParameters(((TH1D*)gDirectory->Get(tgposyname.Data()))->GetEntries(),((TH1D*)gDirectory->Get(tgposyname.Data()))->GetMean(), ((TH1D*)gDirectory->Get(tgposyname.Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(tgposyname.Data()))->Fit("gaus","","QB+",-2.,2.);
  Double_t finalresytra=gaus->GetParameter(1);

  cout<<endl<<endl<<endl;
  cout<<detname.Data()<<" AlignDetaVsDetb analysis done"<<endl;
  cout<<"Detector A new position parameters:"<<endl;
  cout<<detname.Data()<<"PosX: "<<detanewpos.X()<<"  "<<detname.Data()<<"PosY: "<<detanewpos.Y()<<"  "<<detname.Data()<<"PosZ: "<<detanewpos.Z()<<endl;
  cout<<"Detector A new rotation parameters:"<<endl;
  cout<<detname.Data()<<"AngX: "<<detanewangle.X()<<"  "<<detname.Data()<<"AngY: "<<detanewangle.Y()<<"  "<<detname.Data()<<"AngZ: "<<detanewangle.Z()<<endl;
  cout<<"residual on traslations with the new geometrical parameters: finalresxtra="<<finalresxtra<<" finalresytra="<<finalresytra<<endl;
  cout<<"residual on rotations with the new geometrical parameters: finalresxrot="<<finalresxrot<<" finalresyrot="<<finalresyrot<<endl<<endl;

  return;
}


//Here the detector plots will be checked and the overall run status will be printed
void CheckUp(TFile *inputFile, TFile *file_out){

  cout<<"CheckUp file status:"<<endl;

  TH1D* h;
  TH2D* h2;

  //BM control plots
  if(IncludeBM){
    //check on input file
    inputFile->cd();
    cout<<"BM status:"<<endl;
    h=((TH1D*)gDirectory->Get("BM/bmRawHitXEvent"));
      if(h!=nullptr){
        cout<<"The fraction of evts in which the number of hits is less than 6 is:"<<(Int_t) h->Integral(1,7)<<"/"<<(Int_t)h->GetEntries()<<"="<< (Double_t)h->Integral(1,7)/h->GetEntries()<<endl;
      }
    h=((TH1D*)gDirectory->Get("BM/bmTrackTotNumber"));
      if(h!=nullptr){
        cout<<"The number of events with one BM reco track is:"<<(Int_t) h->GetBinContent(2)<<"/"<<(Int_t) h->GetEntries()<<"="<<(Double_t) h->GetBinContent(2)/h->GetEntries()<<endl;
      }
    //check on current output file
    file_out->cd();
    h=((TH1D*)gDirectory->Get("BM/bmRawHitXEvent"));
      if(h!=nullptr){
        cout<<"The fraction of evts in which the number of hits is less than 6 is:"<<(Int_t) h->Integral(1,7)<<"/"<<(Int_t) h->GetEntries()<<"="<< (Double_t)h->Integral(1,7)/h->GetEntries()<<endl;
      }
  }

  if(IncludeBM && IncludeVT){
    switch(vtxsynch){
      case 0 :
        cout<<"The correlation between BM and VTX has been mantained till the end of the processed events maxentries="<<maxentries<<endl;
        break;
      case -1 :
        cout<<"The correlation between BM and VTX has been lost from the beginning"<<endl;
        break;
      default :
        cout<<"The correlation between BM and VTX has been maintained till the event "<<vtxsynch<<" then the synch has been lost somewhere between event "<<vtxsynch+1<<" and event "<<vtxsynch+checkrate<<endl;
    }
  }

return;
}
