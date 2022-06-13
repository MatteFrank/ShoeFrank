#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>

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

#include "TAMCntuHit.hxx"
#include "TAGntuEvent.hxx"


#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>

#endif

//setting global parameters
#define   debug        0

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
static int  doalign;           //number of iterations for alignment
static TAGrunInfo*   runinfo;  //runinfo
static TAGgeoTrafo*  geoTrafo; //geometry
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
static TAVTvertex* vtxmatchvertex; //pointer to the BM matched VTX vertex

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


int Booking(TFile* file_out) {

  if(debug>0)
    cout<<"Booking start"<<endl;

  file_out->cd();
  TH1D *h;
  TH2D *h2;

  if(IncludeSC){
    file_out->mkdir("SC");
    file_out->cd("SC");
      h = new TH1D("SC_Nhit_evt","Number of st hit per event;Number of hits;Events",11,-0.5,10.5);
      h = new TH1D("SC_Hit_edep","Energy deposition per hit;edep;Events",100,0,10);
      h = new TH1D("SC_Hit_charge","charge per hit;charge;Events",600,0.,30.);

    gDirectory->cd("..");
    file_out->cd("..");
  }

  if(IncludeBM){
    file_out->mkdir("BM");
    file_out->cd("BM");
      h = new TH1D("BM_Hit_num","Number of BM hits x event all evts;Number of BM hits;Events",31,-0.5,30.5);
      h = new TH1D("BM_Trk_num","Number of BM reconstructed tracks;Number of tracks;Events",6,-0.5,5.5);
      h = new TH1D("bm_slopeX_bmsys","BM mx in bm sys ;mx;Events",1000,-0.3,0.3);
      h = new TH1D("bm_slopeY_bmsys","BM my in bm sys ;my;Events",1000,-0.3,0.3);
      h = new TH1D("bm_originX_bmsys","BM origin X in bm sys;X[cm];Events",600,-3.,3.);
      h = new TH1D("bm_originY_bmsys","BM origin Y in bm sys;Y[cm];Events",600,-3.,3.);
      h2 = new TH2D("bm_target_bmsys","BM tracks on target  projections in BM sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h = new TH1D("bm_slopeX_glbsys","BM mx in glb sys ;mx;Events",1000,-0.1,0.1);
      h = new TH1D("bm_slopeY_glbsys","BM my in glb sys ;my;Events",1000,-0.1,0.1);
      h = new TH1D("bm_originX_glbsys","BM origin X in glb sys;X[cm];Events",600,-5.,5.);
      h = new TH1D("bm_originY_glbsys","BM origin Y in glb sys;Y[cm];Events",600,-5.,5.);
      h2 = new TH2D("bm_target_glbsys","BM tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
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

      h = new TH1D("VT_trk_num","Number of vtx tracks per event;Number of vtx tracks;Events",21,-0.5,20.5);
      h = new TH1D("vtx_slopeX_vtxsys","vtx mx in vtx sys ;mx;Events",1000,-0.3,0.3);
      h = new TH1D("vtx_slopeY_vtxsys","vtx my in vtx sys ;my;Events",1000,-0.3,0.3);
      h = new TH1D("vtx_originX_vtxsys","vtx origin X in vtx sys;X[cm];Events",600,-3.,3.);
      h = new TH1D("vtx_originY_vtxsys","vtx origin Y in vtx sys;Y[cm];Events",600,-3.,3.);
      h2 = new TH2D("vtx_target_vtxsys","vtx tracks on target  projections in vtx sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h = new TH1D("vtx_slopeX_glbsys","vtx mx in glb sys ;mx;Events",1000,-0.1,0.1);
      h = new TH1D("vtx_slopeY_glbsys","vtx my in glb sys ;my;Events",1000,-0.1,0.1);
      h = new TH1D("vtx_originX_glbsys","vtx origin X in glb sys;X[cm];Events",600,-5.,5.);
      h = new TH1D("vtx_originY_glbsys","vtx origin Y in glb sys;Y[cm];Events",600,-5.,5.);
      h2 = new TH2D("vtx_target_glbsys","vtx tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);

    gDirectory->cd("..");
    file_out->cd("..");
  }

  if(IncludeMSD){
    file_out->mkdir("MSD");
    file_out->cd("MSD");
    h = new TH1D("msd_cls_num_total","Total number of msd clusters x event;Total number clusters;Events",21,-0.5,20.5);
    for(int i=0;i<msdparGeo->GetSensorsN();i++){
      h = new TH1D(Form("msd_cls_num_%d",i),"Number of msd clusters;Number clusters;Events",21,-0.5,20.5);
      h = new TH1D(Form("msd_clspos_msdsys_%d",i),"msd cluster position in the msd system;pos [cm];Events",1000,-5.,5.);
      h = new TH1D(Form("msd_clspos_glbsys_%d",i),"msd cluster position in the glb system;pos [cm];Events",1000,-5.,5.);

      //check the msd internal alignment
      h = new TH1D(Form("msd_residualAllX_%d",i),"Residual of MSD tracks with MSD clusters for all the MSD tracks on X view;(Trackpos-cluspos).X[cm];Events",500,-0.5,0.5);
      h = new TH1D(Form("msd_residualAllY_%d",i),"Residual of MSD tracks with MSD clusters for all the MSD tracks on Y view;(Trackpos-cluspos).Y[cm];Events",500,-0.5,0.5);
      h = new TH1D(Form("msd_residual1TrkX_%d",i),"Residual of MSD tracks with MSD clusters for the events with 1 MSD track on X view;(Trackpos-cluspos).X[cm];Events",500,-0.5,0.5);
      h = new TH1D(Form("msd_residual1TrkY_%d",i),"Residual of MSD tracks with MSD clusters for the events with 1 MSD track on Y view;(Trackpos-cluspos).Y[cm];Events",500,-0.5,0.5);
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
  h2 = new TH2D("origin_xx_bmvtx","BM originX vs VTX originX;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_yy_bmvtx","BM originY vs VTX originY;BM originY;vtx originY",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_xy_bmvtx","BM originX vs VTX originY;BM originX;vtx originY",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_yx_bmvtx","BM originY vs VTX originX;BM originY;vtx originX",600,-3.,3.,600,-3.,3.);
  for(Int_t i=0;i<maxentries/1000+1;i++){
    h2 = new TH2D(Form("origin_xx_bmvtx_%d",i),"BM originX vs VTX originX;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
    h2 = new TH2D(Form("origin_yy_bmvtx_%d",i),"BM originY vs VTX originY;BM originY;vtx originY",600,-3.,3.,600,-3.,3.);
  }
  gDirectory->cd("..");
  file_out->cd("..");

  file_out->mkdir("BMVTX");
  file_out->cd("BMVTX");
  for(Int_t i=0;i<doalign;i++){
    h = new TH1D(Form("slopeX_glb_diff_%d",i),"angle difference bm_track - vtx_track in glb sys;[rad];Events",2000,-0.3,0.3);
    h = new TH1D(Form("slopeY_glb_diff_%d",i),"angle difference bm_track - vtx_track in glb sys;[rad];Events",2000,-0.3,0.3);
    h = new TH1D(Form("originX_glb_diff_%d",i),"Difference of bm tracks and vtx tracks X projection on tg in glb sys;x diff[cm];Events",1000,-5.,5.);
    h = new TH1D(Form("originY_glb_diff_%d",i),"Difference of bm tracks and vtx tracks Y projection on tg in glb sys;y diff[cm];Events",1000,-5.,5.);
  }

  gDirectory->cd("..");
  file_out->cd("..");


  if(debug>0)
    cout<<"Booking done"<<endl;

  return 0;
}

//SC analysis
int StartCounter(){

  if(debug>0)
    cout<<"StartCounter start"<<endl;

  myfill("SC/SC_Nhit_evt",stNtuHit->GetHitsN());
  for(Int_t i =0;i<stNtuHit->GetHitsN();i++){
    TASThit* sthit=stNtuHit->GetHit(i);
    myfill("SC/SC_Hit_edep",sthit->GetDe());
    myfill("SC/SC_Hit_charge",sthit->GetCharge());
  }

  if(debug>0)
    cout<<"StartCounter done"<<endl;

  return 0;
}

//BM analysis
int BeamMonitor(){

  if(debug>0)
    cout<<"BeamMonitor start"<<endl;

  //BM reconstructed analysis
    Int_t nbmHits  = bmNtuHit->GetHitsN();
    myfill("BM/BM_Hit_num",nbmHits);

    ///BM tracks
    int  Nbmtrack = bmNtuTrack->GetTracksN();
    myfill("BM/BM_Trk_num", Nbmtrack);
    for( Int_t iTrack = 0; iTrack < bmNtuTrack->GetTracksN(); ++iTrack ) {
      TABMtrack* track = bmNtuTrack->GetTrack(iTrack);

      //project to the target in the BM ref., then move to the global ref.
      TVector3 bmlocalproj=ProjectToZ(track->GetSlope(), track->GetOrigin(),geoTrafo->FromGlobalToBMLocal(geoTrafo->GetTGCenter()).Z());
      TVector3 glbslope = geoTrafo->VecFromBMLocalToGlobal(track->GetSlope());
      TVector3 glborigin = geoTrafo->FromBMLocalToGlobal(track->GetOrigin());
      myfill("BM/bm_slopeX_bmsys", track->GetSlope().X());
      myfill("BM/bm_slopeY_bmsys", track->GetSlope().Y());
      myfill("BM/bm_originX_bmsys", track->GetOrigin().X());
      myfill("BM/bm_originY_bmsys", track->GetOrigin().Y());
      myfill("BM/bm_target_bmsys", bmlocalproj.X(),bmlocalproj.Y());
      myfill("BM/bm_slopeX_glbsys", glbslope.X());
      myfill("BM/bm_slopeY_glbsys", glbslope.Y());
      myfill("BM/bm_originX_glbsys", glborigin.X());
      myfill("BM/bm_originY_glbsys", glborigin.Y());
      TVector3 bmprojglo=geoTrafo->FromBMLocalToGlobal(bmlocalproj);
      myfill("BM/bm_target_glbsys", bmprojglo.X(),bmprojglo.Y());
    }

  if(debug>0)
    cout<<"BeamMonitor done"<<endl;

  return 0;
}

//Vertex analysis
int Vertex(){

  if(debug>0)
    cout<<"Vertex start"<<endl;

  TAVTvertex* vtxvertex   = 0x0;
  TVector3 vtxvertpos;

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
      myfill("VT/vtx_slopeX_vtxsys",track->GetSlopeZ().X());
      myfill("VT/vtx_slopeY_vtxsys",track->GetSlopeZ().Y());
      myfill("VT/vtx_originX_vtxsys",track->GetOrigin().X());
      myfill("VT/vtx_originY_vtxsys",track->GetOrigin().Y());
      myfill("VT/vtx_target_vtxsys",vtxlocalproj.X(),vtxlocalproj.Y());
      TVector3 glbslope = geoTrafo->VecFromVTLocalToGlobal(track->GetSlopeZ());
      TVector3 glborigin = geoTrafo->FromVTLocalToGlobal(track->GetOrigin());
      TVector3 vtxprojglo=geoTrafo->FromVTLocalToGlobal(vtxlocalproj);
      myfill("VT/vtx_slopeX_glbsys",glbslope.X());
      myfill("VT/vtx_slopeY_glbsys",glbslope.Y());
      myfill("VT/vtx_originX_glbsys",glborigin.X());
      myfill("VT/vtx_originY_glbsys",glborigin.Y());
      myfill("VT/vtx_target_glbsys",vtxprojglo.X(),vtxprojglo.Y());
    }
  }

  if(debug>0)
    cout<<"Vertex done"<<endl;

  return 0;
}

//TofWall analysis
int MSD(){

  if(debug>0)
    cout<<"MSD start"<<endl;

  Int_t totalmsdnclus=0;
  for (int i = 0; i < msdparGeo->GetSensorsN(); i++) {
    totalmsdnclus+=msdntuclus->GetClustersN(i);
    myfill(Form("MSD/msd_cls_num_%d",i),msdntuclus->GetClustersN(i));
    for(int k=0;k<msdntuclus->GetClustersN(i);k++){
      TAMSDcluster *msdclus=msdntuclus->GetCluster(i,k);
      myfill(Form("MSD/msd_clspos_msdsys_%d",i), msdclus->GetPosition().Y());
      myfill(Form("MSD/msd_clspos_glbsys_%d",i), msdclus->GetPositionG().Y());
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
      TAGcluster *msdclus=track->GetCluster(k);
      TVector3 trkpos=ProjectToZ(track->GetSlopeZ(), track->GetOrigin(),msdclus->GetPosition().Z());
      myfill(Form("MSD/msd_residualAllX_%d",msdclus->GetSensorIdx()),trkpos.X()-msdclus->GetPosition().X());
      myfill(Form("MSD/msd_residualAllY_%d",msdclus->GetSensorIdx()),trkpos.Y()-msdclus->GetPosition().Y());
      if(msdntutrack->GetTracksN()==1){
        myfill(Form("MSD/msd_residual1TrkX_%d",msdclus->GetSensorIdx()),trkpos.X()-msdclus->GetPosition().X());
        myfill(Form("MSD/msd_residual1TrkY_%d",msdclus->GetSensorIdx()),trkpos.Y()-msdclus->GetPosition().Y());
      }
    }
  }

  if(debug>0)
    cout<<"MSD done"<<endl;

  return 0;
}

//TofWall analysis
int TofWall(){

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

  return 0;
}

int DataAcquisition(){

  if(debug)
    cout<<"DataAcquisition analysis start"<<endl;

  myfill("DAQ/daq_acqrate",100./tgevent->GetTimeSinceLastTrigger()); //tgevent->GetTimeSinceLastTrigger() is in decisec.

  if(debug)
    cout<<"DataAcquisition done"<<endl;
  return 0;
}

//check the VTX sync with the BM
int VTXSYNC(){

  if(debug)
    cout<<"VTXSYNC analysis start"<<endl;

  TABMtrack* bmtrack = bmNtuTrack->GetTrack(0);
  myfill("VTXSYNC/origin_xx_bmvtx",bmtrack->GetOrigin().X(),vtxmatchvertex->GetVertexPosition().X());
  myfill("VTXSYNC/origin_yy_bmvtx",bmtrack->GetOrigin().Y(),vtxmatchvertex->GetVertexPosition().Y());
  myfill("VTXSYNC/origin_xy_bmvtx",bmtrack->GetOrigin().X(),vtxmatchvertex->GetVertexPosition().Y());
  myfill("VTXSYNC/origin_yx_bmvtx",bmtrack->GetOrigin().Y(),vtxmatchvertex->GetVertexPosition().X());
  myfill(Form("VTXSYNC/origin_xx_bmvtx_%d",evnum/1000),bmtrack->GetOrigin().X(),vtxmatchvertex->GetVertexPosition().X());
  myfill(Form("VTXSYNC/origin_yy_bmvtx_%d",evnum/1000),bmtrack->GetOrigin().Y(),vtxmatchvertex->GetVertexPosition().Y());

  if(debug)
    cout<<"VTXSYNC analysis done"<<endl;

  return 0;
}

int BMVTX(){

  if(debug)
    cout<<"BMVTX analysis start"<<endl;


  if(debug)
    cout<<"BMVTX analysis done"<<endl;

  return 0;
}
