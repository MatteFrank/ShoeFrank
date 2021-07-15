#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TProfile.h>
#include <TBrowser.h>


#include "TAGparGeo.hxx"
#include "TAGrecoManager.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGactTreeReader.hxx"

#include "TABMparGeo.hxx"
#include "TABMparConf.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"
#include "TABMntuTrack.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDcluster.hxx"


#include "TAGgeoTrafo.hxx"

#endif

//fixed par
#define DEG2RAD  0.01745329251994329577
#define RAD2DEG  57.2957795130823208768

//general par
#define debug            0
#define msdplane         0

static int  evnum;         //current event number
static TAGgeoTrafo* geoTrafo;
static TABMntuHit*  bmNtuHit;
static TABMntuTrack*  bmNtuTrack;
static TAMSDntuCluster* msdNtuCluster;


//*********************************************  combined functions  *****************************************

TVector3 Intersection(double zloc, TVector3 fSlope, TVector3 fOrigin) const
{
   TVector3 projected(fSlope.X()/fSlope.Z()*zloc+fOrigin.X() ,fSlope.Y()/fSlope.Z()*zloc+fOrigin.Y(), zloc);
   return projected;
}


void BookingBMVTX(TFile* f_out){

  if(debug>0)
    cout<<"I'm in Booking BMVTX"<<endl;

  f_out->cd();
  TH1D *h;
  TH2D *h2;
  char tmp_char[200];

  //vtx
  h = new TH1D("vtx_originX_vtxsys","VTX origin X in vtx sys;X[cm];Events",600,-3.,3.);
  h = new TH1D("vtx_originY_vtxsys","VTX origin Y in vtx sys;Y[cm];Events",600,-3.,3.);

  h = new TH1D("vtx_originX_glbsys","VTX origin X in glb sys;X[cm];Events",600,-3.,3.);
  h = new TH1D("vtx_originY_glbsys","VTX origin Y in glb sys;Y[cm];Events",600,-3.,3.);
  h2 = new TH2D("vtx_target_glbsys","VTX tracks projection on target plane in glb sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
  h2 = new TH2D("msd_tw_glbsys","MSD tracks projection on tw plane in glb sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);

  //bm
  h = new TH1D("bm_slopeX_bmsys","BM mx in bm sys ;mx;Events",1000,-0.3,0.3);
  h = new TH1D("bm_slopeY_bmsys","BM my in bm sys ;my;Events",1000,-0.3,0.3);
  h = new TH1D("bm_originX_bmsys","BM origin X in bm sys;X[cm];Events",600,-3.,3.);
  h = new TH1D("bm_originY_bmsys","BM origin Y in bm sys;Y[cm];Events",600,-3.,3.);
  h2 = new TH2D("bm_target_bmsys","BM tracks on target  projections in BM sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);

  h = new TH1D("bm_slopeX_glbsys","BM mx in glb sys ;mx;Events",1000,-0.3,0.3);
  h = new TH1D("bm_slopeY_glbsys","BM my in glb sys ;my;Events",1000,-0.3,0.3);
  h = new TH1D("bm_originX_glbsys","BM origin X in glb sys;X[cm];Events",600,-3.,3.);
  h = new TH1D("bm_originY_glbsys","BM origin Y in glb sys;Y[cm];Events",600,-3.,3.);
  h2 = new TH2D("bm_target_glbsys","BM tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);

  h = new TH1D("bm_polar_angle","BM polar angle distribution ;AngZ(deg);Events",400,0.,10.);
  h = new TH1D("bm_azimuth_angle","BM azimuth angle distribution ;Phi(deg);Events",180,0.,180.);

  //bm-vtx combined
  h2 = new TH2D("origin_xx_bmvtx","BM originX vs VTX originX;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_yy_bmvtx","BM originY vs VTX originY;BM originY;vtx originY",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_xy_bmvtx","BM originX vs VTX originY;BM originX;vtx originY",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_yx_bmvtx","BM originY vs VTX originX;BM originY;vtx originX",600,-3.,3.,600,-3.,3.);
  // h = new TH1D("originX_glb_diff","Difference of bm tracks and vtx tracks X projection on tg in glb sys;x diff[cm];Events",400,-0.2,0.2);
  // h = new TH1D("originY_glb_diff","Difference of bm tracks and vtx tracks Y projection on tg in glb sys;y diff[cm];Events",400,-0.2,0.2);
  h = new TH1D("combinedStatus","Status of combined events;0=ok, 1=vttrknum>1, 2=vttrknum<1, 3=bmtrknum>1, 4=bmtrknum<1, 5=vttrkChi2, 6=bmtrkChi2;Events",7,-0.5,6.5);

  if(debug>0)
    cout<<"Booking finished"<<endl;

  return;
}


void FillSeparated(){

  if(debug)
    cout<<"I'm in PrintSeparated"<<endl;

  TVector3 tmp_tvector3, glbslope, glborigin;
  char tmp_char[200];
  TVector3 postgglo = geoTrafo->FromTGLocalToGlobal(TVector3(0,0,0));  //Target position in global  framework
  TVector3 postgvt = geoTrafo->FromGlobalToMSDLocal(postgglo);     //Target position in VT framework
  TVector3 postgbm = geoTrafo->FromGlobalToBMLocal(postgglo);      //Target position in BM framework

  //vtx tracks loop
  for(Int_t i=0;i<msdNtuCluster->GetClustersN(msdplane);i++){
    TAMSDcluster* msdclus = msdNtuCluster->GetCluster(msdplane,i);
    ((TH1D*)gDirectory->Get("vtx_originX_vtxsys"))->Fill(msdclus->GetPosition().X());
    ((TH1D*)gDirectory->Get("vtx_originY_vtxsys"))->Fill(msdclus->GetPosition().Y());

    glborigin = geoTrafo->FromMSDLocalToGlobal(msdclus->GetPosition());
    ((TH1D*)gDirectory->Get("vtx_originX_glbsys"))->Fill(glborigin.X());
    ((TH1D*)gDirectory->Get("vtx_originY_glbsys"))->Fill(glborigin.Y());
    tmp_tvector3 = geoTrafo->FromMSDLocalToGlobal(tmp_tvector3);
    ((TH2D*)gDirectory->Get("vtx_target_glbsys"))->Fill(tmp_tvector3.X(),tmp_tvector3.Y());
  }

  //tentative to fit a dummy msd track from 1 hit of the first and 1 hit of the last plane
  if(msdNtuCluster->GetClustersN(0)==1 && msdNtuCluster->GetClustersN(2)==1){
    TAMSDcluster* msd0clus = msdNtuCluster->GetCluster(0,0);
    TAMSDcluster* msd2clus = msdNtuCluster->GetCluster(2,0);
    TVector3 msdglbpos = geoTrafo->FromMSDLocalToGlobal(msd0clus->GetPosition());
    TVector3 msdglbdir = geoTrafo->VecFromMSDLocalToGlobal(msd2clus->GetPosition()-msd0clus->GetPosition());
    TVector3 twposglo = geoTrafo->FromTWLocalToGlobal(TVector3(0,0,0));  //Target position in global  framework
    TVector3 postgmsd = geoTrafo->FromGlobalToMSDLocal(twposglo);     //TW position in MSD framework
    TVector3 msdglbtw=geoTrafo->FromMSDLocalToGlobal(Intersection(postgmsd.Z(),msdglbdir, msdglbpos));
    ((TH2D*)gDirectory->Get("msd_tw_glbsys"))->Fill(msdglbtw.X(), msdglbtw.Y());
  }

  //bm tracks loop
  for(int i=0; i<bmNtuTrack->GetTracksN();i++){
    TABMtrack* bmtrack=bmNtuTrack->GetTrack(i);
    ((TH1D*)gDirectory->Get("bm_slopeX_bmsys"))->Fill(bmtrack->GetSlope().X()/bmtrack->GetSlope().Z());
    ((TH1D*)gDirectory->Get("bm_slopeY_bmsys"))->Fill(bmtrack->GetSlope().Y()/bmtrack->GetSlope().Z());
    ((TH1D*)gDirectory->Get("bm_originX_bmsys"))->Fill(bmtrack->GetOrigin().X());
    ((TH1D*)gDirectory->Get("bm_originY_bmsys"))->Fill(bmtrack->GetOrigin().Y());
    ((TH1D*)gDirectory->Get("bm_polar_angle"))->Fill(bmtrack->GetSlope().Theta()*RAD2DEG);
    ((TH1D*)gDirectory->Get("bm_azimuth_angle"))->Fill(bmtrack->GetSlope().Phi()*RAD2DEG);

    glbslope = geoTrafo->VecFromBMLocalToGlobal(bmtrack->GetSlope());
    glborigin = geoTrafo->FromBMLocalToGlobal(bmtrack->GetOrigin());
    ((TH1D*)gDirectory->Get("bm_slopeX_glbsys"))->Fill(glbslope.X()/glbslope.Z());
    ((TH1D*)gDirectory->Get("bm_slopeY_glbsys"))->Fill(glbslope.Y()/glbslope.Z());
    ((TH1D*)gDirectory->Get("bm_originX_glbsys"))->Fill(glborigin.X());
    ((TH1D*)gDirectory->Get("bm_originY_glbsys"))->Fill(glborigin.Y());

    tmp_tvector3=bmtrack->Intersection(postgbm.Z());
    ((TH2D*)gDirectory->Get("bm_target_bmsys"))->Fill(tmp_tvector3.X(), tmp_tvector3.Y());
    tmp_tvector3 = geoTrafo->FromBMLocalToGlobal(tmp_tvector3);
    ((TH2D*)gDirectory->Get("bm_target_glbsys"))->Fill(tmp_tvector3.X(), tmp_tvector3.Y());
  }

  if(debug)
    cout<<"PrintSeparated done"<<endl;

  return;
}

void FillCombined(TABMtrack* bmtrack, TAMSDcluster* msdclus){

  if(debug)
    cout<<"I'm in PrintCombined"<<endl;

  ((TH2D*)gDirectory->Get("origin_xx_bmvtx"))->Fill(bmtrack->GetOrigin().X(), msdclus->GetPosition().X());
  ((TH2D*)gDirectory->Get("origin_yy_bmvtx"))->Fill(bmtrack->GetOrigin().Y(), msdclus->GetPosition().Y());
  ((TH2D*)gDirectory->Get("origin_xy_bmvtx"))->Fill(bmtrack->GetOrigin().X(), msdclus->GetPosition().Y());
  ((TH2D*)gDirectory->Get("origin_yx_bmvtx"))->Fill(bmtrack->GetOrigin().Y(), msdclus->GetPosition().X());

  TVector3 postgglo = geoTrafo->FromTGLocalToGlobal(TVector3(0,0,0));  //Target position in global  framework
  TVector3 postgvt = geoTrafo->FromGlobalToMSDLocal(postgglo);     //Target position in VT framework
  TVector3 postgbm = geoTrafo->FromGlobalToBMLocal(postgglo);      //Target position in BM framework

  TVector3 bmglbvec=geoTrafo->FromBMLocalToGlobal(bmtrack->Intersection(postgbm.Z()));
  // TVector3 vtglbvec=geoTrafo->FromMSDLocalToGlobal(msdclus->Intersection(postgvt.Z()));
  // ((TH1D*)gDirectory->Get("originX_glb_diff"))->Fill(bmglbvec.X()-vtglbvec.X());
  // ((TH1D*)gDirectory->Get("originY_glb_diff"))->Fill(bmglbvec.Y()-vtglbvec.Y());
  // vtxoriginvec.push_back(vtglbvec);
  // bmoriginvec.push_back(bmglbvec);

  if(debug)
    cout<<"PrintCombined done"<<endl;

  return;
}

void PostLoopAnalysis(){

  if(debug)
    cout<<"I'm in PostLoopAnalysis"<<endl;

  TF1* first_strel_tf1_1=new TF1("first1_strel_green","1./0.78*(0.032891770+0.0075746330*x-(5.1692440e-05)*x*x+(1.8928600e-07)*x*x*x-(2.4652420e-10)*x*x*x*x)", 0., 320.);
  TF1* first_strel_tf1_08=new TF1("first08_strel_blue","0.032891770+0.0075746330*x-(5.1692440e-05)*x*x+(1.8928600e-07)*x*x*x-(2.4652420e-10)*x*x*x*x", 0., 320.);
  TF1* MCinput_strel=new TF1("McStrel_cyan","0.0096625*x -6.461555e-05*x*x + 2.366075e-07*x*x*x -3.0815525e-10*x*x*x*x", 0., 350.);
  TF1* garfield_strel_tf1=new TF1("garfield_strel_yellow","0.00915267+0.00634507*x+2.02527e-05*x*x-7.60133e-07*x*x*x+5.55868e-09*x*x*x*x-1.68944e-11*x*x*x*x*x+1.87124e-14*x*x*x*x*x*x", 0., 320.);
  first_strel_tf1_1->SetLineColor(3);
  first_strel_tf1_1->Write();
  first_strel_tf1_08->SetLineColor(4);
  first_strel_tf1_08->Write();
  MCinput_strel->SetLineColor(7);
  MCinput_strel->Write();
  garfield_strel_tf1->SetLineColor(41);
  garfield_strel_tf1->Write();

  cout<<"BM VTX X origin correlation factor= "<<((TH2D*)gDirectory->Get("origin_xx_bmvtx"))->GetCorrelationFactor()<<endl;
  cout<<"BM VTX Y origin correlation factor= "<<((TH2D*)gDirectory->Get("origin_yy_bmvtx"))->GetCorrelationFactor()<<endl;

  if(debug)
    cout<<"PostLoopAnalysis done"<<endl;

    return;
}


void Allign_estimate(){

  if(debug>0)
    cout<<"I'm in Allign_estimate"<<endl;

  TF1 *gaus=new TF1("gaus","gaus", -3., 3.);

  //BM initial parameters
  gaus->SetParameters(((TH1D*)gDirectory->Get("bm_originX_glbsys"))->GetEntries(),((TH1D*)gDirectory->Get("bm_originX_glbsys"))->GetMean(), ((TH1D*)gDirectory->Get("bm_originX_glbsys"))->GetStdDev());
  ((TH1D*)gDirectory->Get("bm_originX_glbsys"))->Fit("gaus","QB+");
  Double_t BMxtra=gaus->GetParameter(1);

  gaus->SetParameters(((TH1D*)gDirectory->Get("bm_originY_glbsys"))->GetEntries(),((TH1D*)gDirectory->Get("bm_originY_glbsys"))->GetMean(), ((TH1D*)gDirectory->Get("bm_originY_glbsys"))->GetStdDev());
  ((TH1D*)gDirectory->Get("bm_originY_glbsys"))->Fit("gaus","QB+");
  Double_t BMytra=gaus->GetParameter(1);

  //N.B.: here slope X represents the track on the XZ plane, meaning a rotation around Y axis (in FOOT.geo AngY is the rotation around Y axis)
  gaus->SetParameters(((TH1D*)gDirectory->Get("bm_slopeX_glbsys"))->GetEntries(),((TH1D*)gDirectory->Get("bm_slopeX_glbsys"))->GetMean(), ((TH1D*)gDirectory->Get("bm_slopeX_glbsys"))->GetStdDev());
  ((TH1D*)gDirectory->Get("bm_slopeX_glbsys"))->Fit("gaus","QB+");
  Double_t BMyrot=atan(gaus->GetParameter(1))*RAD2DEG;

  //N.B.: here is the same thing before, but for the rotations around X axis a minus sign is needed
  gaus->SetParameters(((TH1D*)gDirectory->Get("bm_slopeY_glbsys"))->GetEntries(),((TH1D*)gDirectory->Get("bm_slopeY_glbsys"))->GetMean(), ((TH1D*)gDirectory->Get("bm_slopeY_glbsys"))->GetStdDev());
  ((TH1D*)gDirectory->Get("bm_slopeY_glbsys"))->Fit("gaus","QB+");
  Double_t BMxrot=-atan(gaus->GetParameter(1))*RAD2DEG;

  cout<<"Beam Monitor allignment parameters:"<<endl;
  cout<<"BM traslation posX="<<BMxtra<<"  posy="<<BMytra<<endl;
  cout<<"BM rotation around X axys BMxrot="<<BMxrot<<"  rotation around Y axis BMyrot="<<BMyrot<<endl;

  //VTX initial parameters:
  gaus->SetParameters(((TH1D*)gDirectory->Get("vtx_originX_glbsys"))->GetEntries(),((TH1D*)gDirectory->Get("vtx_originX_glbsys"))->GetMean(), ((TH1D*)gDirectory->Get("vtx_originX_glbsys"))->GetStdDev());
  ((TH1D*)gDirectory->Get("vtx_originX_glbsys"))->Fit("gaus","QB+");
  Double_t VTXxtra=gaus->GetParameter(1);

  gaus->SetParameters(((TH1D*)gDirectory->Get("vtx_originY_glbsys"))->GetEntries(),((TH1D*)gDirectory->Get("vtx_originY_glbsys"))->GetMean(), ((TH1D*)gDirectory->Get("vtx_originY_glbsys"))->GetStdDev());
  ((TH1D*)gDirectory->Get("vtx_originY_glbsys"))->Fit("gaus","QB+");
  Double_t VTXytra=gaus->GetParameter(1);

  cout<<"Vertex allignment parameters:"<<endl;
  cout<<"VTX traslation posX="<<VTXxtra<<"  posy="<<VTXytra<<endl;

  if(debug>0)
    cout<<"Allign_estimate finished"<<endl;

  return;
}
