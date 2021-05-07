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

#include "TAVTparGeo.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"


#include "TAGgeoTrafo.hxx"

#endif

//fixed par
#define DEG2RAD  0.01745329251994329577
#define RAD2DEG  57.2957795130823208768

//general par
#define debug            0

static int  evnum;         //current event number
static TAGgeoTrafo* geoTrafo;
static TABMntuHit*  bmNtuHit;
static TABMntuTrack*  bmNtuTrack;
static TAVTntuTrack* vtNtuTrack;


//*********************************************  combined functions  *****************************************

void BookingBMVTX(TFile* f_out){
  
  if(debug>0)
    cout<<"I'm in Booking BMVTX"<<endl;  

  f_out->cd();
  TH1D *h; 
  TH2D *h2;
  char tmp_char[200];
  
  //vtx
  h = new TH1D("vtx_slopeX","VTX mx in vtx sys;mx;Events",1000,-0.5,0.5);
  h = new TH1D("vtx_slopeY","VTX my in vtx sys;my;Events",1000,-0.5,0.5);
  h = new TH1D("vtx_originX","VTX origin X in vtx sys;X[cm];Events",600,-3.,3.);
  h = new TH1D("vtx_originY","VTX origin Y in vtx sys;Y[cm];Events",600,-3.,3.);
  h = new TH1D("vtx_polar_angle","vtx polar angle distribution ;AngZ(deg);Events",400,0.,10.);
  h = new TH1D("vtx_azimuth_angle","vtx azimuth angle distribution ;Phi(deg);Events",180,0.,180.);
  h2 = new TH2D("vtx_target_vtxsys","VTX tracks projection on target plane in VTX sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
  h2 = new TH2D("vtx_target_glbsys","VTX tracks projection on target plane in Global sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    
  //bm 
  h = new TH1D("bm_slopeX","BM mx in bm sys ;mx;Events",1000,-0.5,0.5);
  h = new TH1D("bm_slopeY","BM my in bm sys ;my;Events",1000,-0.5,0.5);
  h = new TH1D("bm_originX","BM origin X in bm sys;X[cm];Events",600,-3.,3.);
  h = new TH1D("bm_originY","BM origin Y in bm sys;Y[cm];Events",600,-3.,3.);    
  h = new TH1D("bm_polar_angle","BM polar angle distribution ;AngZ(deg);Events",400,0.,10.);
  h = new TH1D("bm_azimuth_angle","BM azimuth angle distribution ;Phi(deg);Events",180,0.,180.);
  h2 = new TH2D("bm_target_bmsys","BM tracks on target  projections in BM sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
  h2 = new TH2D("bm_target_glbsys","BM tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);

  //bm-vtx combined 
  h2 = new TH2D("slope_mxx_bmvtx","BM slopeX vs VTX slopeX;BM mx;vtx mx",500,-0.05,0.05,500,-0.05,0.05);
  h2 = new TH2D("slope_myy_bmvtx","BM slopeY vs VTX slopeY;BM my;vtx my",500,-0.05,0.05,500,-0.05,0.05);
  h2 = new TH2D("slope_mxy_bmvtx","BM slopeX vs VTX slopeY;BM mx;vtx my",500,-0.05,0.05,500,-0.05,0.05);
  h2 = new TH2D("slope_myx_bmvtx","BM slopeY vs VTX slopeX;BM my;vtx mx",500,-0.05,0.05,500,-0.05,0.05);
  h2 = new TH2D("origin_xx_bmvtx","BM originX vs VTX originX;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_yy_bmvtx","BM originY vs VTX originY;BM originY;vtx originY",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_xy_bmvtx","BM originX vs VTX originY;BM originX;vtx originY",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("origin_yx_bmvtx","BM originY vs VTX originX;BM originY;vtx originX",600,-3.,3.,600,-3.,3.);
  h = new TH1D("slopeX_glb_diff","angle difference bm_track - vtx_track in glb sys;[rad];Events",2000,-0.1,0.1);
  h = new TH1D("slopeY_glb_diff","angle difference bm_track - vtx_track in glb sys;[rad];Events",2000,-0.1,0.1);
  h = new TH1D("originX_glb_diff","Difference of bm tracks and vtx tracks X projection on tg in glb sys;x diff[cm];Events",400,-0.2,0.2);
  h = new TH1D("originY_glb_diff","Difference of bm tracks and vtx tracks Y projection on tg in glb sys;y diff[cm];Events",400,-0.2,0.2);
  h = new TH1D("combinedStatus","Status of combined events;0=ok, 1=vttrknum>1, 2=vttrknum<1, 3=bmtrknum>1, 4=bmtrknum<1, 5=vttrkChi2, 6=bmtrkChi2;Events",7,-0.5,6.5);
  
  //strel plots
  h2 = new TH2D("NewStrel","BM tdrift vs VTX rdrift;BM tdrift [ns];VTX rdrift [cm]",200,0.,400.,200,0.,1.);
  h2 = new TH2D("bmvt_rdrift_residual","BM rdrift - VT rdrift;Residual [cm];BM rdrift [cm]",400,-0.1,0.1,100,0.,1.);
  h = new TH1D("bm_notsel_rdrift","VT track rdrift for not selected bm hits;rdrift [cm];Number of hits",100,0.,1.);
  
  if(debug>0)
    cout<<"Booking finished"<<endl;  
  
  return;
}


void FillSeparated(){

  if(debug)
    cout<<"I'm in PrintSeparated"<<endl;

  TVector3 tmp_tvector3;
  char tmp_char[200];  
  TVector3 postgglo = geoTrafo->FromTGLocalToGlobal(TVector3(0,0,0));  //Target position in global  framework
  TVector3 postgvt = geoTrafo->FromGlobalToVTLocal(postgglo);     //Target position in VT framework
  TVector3 postgbm = geoTrafo->FromGlobalToBMLocal(postgglo);      //Target position in BM framework
     
  //vtx tracks loop   
  for(Int_t i=0;i<vtNtuTrack->GetTracksN();i++){
    TAVTtrack* vttrack = vtNtuTrack->GetTrack(i);
    ((TH1D*)gDirectory->Get("vtx_slopeX"))->Fill(vttrack->GetSlopeZ().X()/vttrack->GetSlopeZ().Z());
    ((TH1D*)gDirectory->Get("vtx_slopeY"))->Fill(vttrack->GetSlopeZ().Y()/vttrack->GetSlopeZ().Z());
    ((TH1D*)gDirectory->Get("vtx_originX"))->Fill(vttrack->GetOrigin().X());
    ((TH1D*)gDirectory->Get("vtx_originY"))->Fill(vttrack->GetOrigin().Y());
    ((TH1D*)gDirectory->Get("vtx_polar_angle"))->Fill(vttrack->GetSlopeZ().Theta()*RAD2DEG);
    ((TH1D*)gDirectory->Get("vtx_azimuth_angle"))->Fill(vttrack->GetSlopeZ().Phi()*RAD2DEG);
    tmp_tvector3=vttrack->Intersection(postgvt.Z());
    ((TH2D*)gDirectory->Get("vtx_target_vtxsys"))->Fill(tmp_tvector3.X(),tmp_tvector3.Y());
    tmp_tvector3 = geoTrafo->FromVTLocalToGlobal(tmp_tvector3);
    ((TH2D*)gDirectory->Get("vtx_target_glbsys"))->Fill(tmp_tvector3.X(),tmp_tvector3.Y());
  }

  //bm tracks loop
  for(int i=0; i<bmNtuTrack->GetTracksN();i++){
    TABMtrack* bmtrack=bmNtuTrack->GetTrack(i);
    ((TH1D*)gDirectory->Get("bm_slopeX"))->Fill(bmtrack->GetSlope().X()/bmtrack->GetSlope().Z());
    ((TH1D*)gDirectory->Get("bm_slopeY"))->Fill(bmtrack->GetSlope().Y()/bmtrack->GetSlope().Z());
    ((TH1D*)gDirectory->Get("bm_originX"))->Fill(bmtrack->GetOrigin().X());
    ((TH1D*)gDirectory->Get("bm_originY"))->Fill(bmtrack->GetOrigin().Y());
    ((TH1D*)gDirectory->Get("bm_polar_angle"))->Fill(bmtrack->GetSlope().Theta()*RAD2DEG);
    ((TH1D*)gDirectory->Get("bm_azimuth_angle"))->Fill(bmtrack->GetSlope().Phi()*RAD2DEG);
    tmp_tvector3=bmtrack->Intersection(postgbm.Z());
    ((TH2D*)gDirectory->Get("bm_target_bmsys"))->Fill(tmp_tvector3.X(), tmp_tvector3.Y());
    tmp_tvector3 = geoTrafo->FromBMLocalToGlobal(tmp_tvector3);
    ((TH2D*)gDirectory->Get("bm_target_glbsys"))->Fill(tmp_tvector3.X(), tmp_tvector3.Y());
  }

  if(debug)
    cout<<"PrintSeparated done"<<endl;

  return;
}

void FillCombined(TABMtrack* bmtrack, TAVTtrack* vttrack){

  if(debug)
    cout<<"I'm in PrintCombined"<<endl;  
  
  ((TH2D*)gDirectory->Get("slope_mxx_bmvtx"))->Fill(bmtrack->GetSlope().X()/bmtrack->GetSlope().Z(), vttrack->GetSlopeZ().X()/vttrack->GetSlopeZ().Z());
  ((TH2D*)gDirectory->Get("slope_myy_bmvtx"))->Fill(bmtrack->GetSlope().Y()/bmtrack->GetSlope().Z(), vttrack->GetSlopeZ().Y()/vttrack->GetSlopeZ().Z());
  ((TH2D*)gDirectory->Get("slope_mxy_bmvtx"))->Fill(bmtrack->GetSlope().X()/bmtrack->GetSlope().Z(), vttrack->GetSlopeZ().Y()/vttrack->GetSlopeZ().Z());
  ((TH2D*)gDirectory->Get("slope_myx_bmvtx"))->Fill(bmtrack->GetSlope().Y()/bmtrack->GetSlope().Z(), vttrack->GetSlopeZ().X()/vttrack->GetSlopeZ().Z());
  ((TH2D*)gDirectory->Get("origin_xx_bmvtx"))->Fill(bmtrack->GetOrigin().X(), vttrack->GetOrigin().X());
  ((TH2D*)gDirectory->Get("origin_yy_bmvtx"))->Fill(bmtrack->GetOrigin().Y(), vttrack->GetOrigin().Y());
  ((TH2D*)gDirectory->Get("origin_xy_bmvtx"))->Fill(bmtrack->GetOrigin().X(), vttrack->GetOrigin().Y());
  ((TH2D*)gDirectory->Get("origin_yx_bmvtx"))->Fill(bmtrack->GetOrigin().Y(), vttrack->GetOrigin().X());

  TVector3 postgglo = geoTrafo->FromTGLocalToGlobal(TVector3(0,0,0));  //Target position in global  framework
  TVector3 postgvt = geoTrafo->FromGlobalToVTLocal(postgglo);     //Target position in VT framework
  TVector3 postgbm = geoTrafo->FromGlobalToBMLocal(postgglo);      //Target position in BM framework

  TVector3 bmglbvec=geoTrafo->VecFromBMLocalToGlobal(bmtrack->GetSlope());
  TVector3 vtglbvec=geoTrafo->VecFromVTLocalToGlobal(vttrack->GetSlopeZ());
  ((TH1D*)gDirectory->Get("slopeX_glb_diff"))->Fill(bmglbvec.X()/bmglbvec.Z()-vtglbvec.X()/vtglbvec.Z());
  ((TH1D*)gDirectory->Get("slopeY_glb_diff"))->Fill(bmglbvec.Y()/bmglbvec.Z()-vtglbvec.Y()/vtglbvec.Z());  

  bmglbvec=geoTrafo->FromBMLocalToGlobal(bmtrack->Intersection(postgbm.Z()));
  vtglbvec=geoTrafo->FromVTLocalToGlobal(vttrack->Intersection(postgvt.Z()));
  ((TH1D*)gDirectory->Get("originX_glb_diff"))->Fill(bmglbvec.X()-vtglbvec.X());
  ((TH1D*)gDirectory->Get("originY_glb_diff"))->Fill(bmglbvec.Y()-vtglbvec.Y());

  if(debug)
    cout<<"PrintCombined done"<<endl;

  return;
}


void FillStrel(TAVTtrack* vttrack, TABMparGeo* bmpargeo){

  TVector3 bmposvtx = geoTrafo->FromGlobalToVTLocal(geoTrafo->FromBMLocalToGlobal(TVector3(0,0,0)));    //BM detector position in vtx ref.
  TVector3 vtpos_bmproj_glb = geoTrafo->FromVTLocalToGlobal(vttrack->Intersection(bmposvtx.Z()));           //vertex track positin on bm in global ref
  TVector3 vtposbm = geoTrafo->FromGlobalToBMLocal(vtpos_bmproj_glb);                                   //vtx track origin in bm ref
  TVector3 vtslp_bmproj_glb = geoTrafo->VecFromVTLocalToGlobal(vttrack->GetSlopeZ());                   //vertex track slope in global ref
  TVector3 vtslpbm = geoTrafo->VecFromGlobalToBMLocal(vtslp_bmproj_glb);                                //vtx track slope in bm ref

  for(int i=0;i<bmNtuHit->GetHitsN();i++){
    TABMhit* bmhit=bmNtuHit->GetHit(i);
    int wireid = bmpargeo->GetSenseId(bmhit->GetCell());
    double vtdrift=bmpargeo->FindRdrift(vtposbm, vtslpbm, bmpargeo->GetWirePos(bmhit->GetView(), bmhit->GetPlane(), wireid), bmpargeo->GetWireDir(bmhit->GetView()), true);
    ((TH1D*)gDirectory->Get("NewStrel"))->Fill(bmhit->GetTdrift(), vtdrift);
    if(bmhit->GetIsSelected())
      ((TH2D*)gDirectory->Get("bmvt_rdrift_residual"))->Fill(bmhit->GetRdrift() - vtdrift, bmhit->GetRdrift());
    else
      ((TH1D*)gDirectory->Get("bm_notsel_rdrift"))->Fill(vtdrift);
  }

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
  cout<<"BM VTX Y slope correlation factor= "<<((TH2D*)gDirectory->Get("slope_myy_bmvtx"))->GetCorrelationFactor()<<endl;

  TProfile *prof_newstrel=((TH2D*)gDirectory->Get("NewStrel"))->ProfileX(); 
  prof_newstrel->SetLineColor(3);
  prof_newstrel->Draw();
  TF1 poly ("poly","pol5", 0, 400);
  prof_newstrel->Fit("poly","QB+");
  poly.SetName("fitted_Newstrel");
  poly.Write();
  cout<<"New_STrel_function:  "<<poly.GetFormula()->GetExpFormula().Data()<<endl;
  cout<<"STrel_number_of_parameters:  "<<poly.GetNpar()<<endl;
  for(int i=0;i<poly.GetNpar();i++)
    cout<<poly.GetParameter(i)<<"  ";
  cout<<endl; 

  if(debug)
    cout<<"PostLoopAnalysis done"<<endl;
         
    return;  
}


//estimate allign with projections on mylars 
//da sistemare!!!!
void Allign_estimate(){

  if(debug>0)
    cout<<"I'm in Allign_estimate"<<endl;  
  
  // //BM par
  // Double_t BMxrot=atan((((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(2) - ((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(2))/(BMN_MYLAR2Z-BMN_MYLAR1Z))*RAD2DEG;    
  // Double_t BMyrot=-atan((((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(1)-((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(1))/(BMN_MYLAR2Z-BMN_MYLAR1Z))*RAD2DEG;
  // Double_t BMxtra=-(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(1)+((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(1))/2.;  
  // Double_t BMxtr_err=sqrt(pow(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(1)/sqrt(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetEntries()),2.)  + pow(((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(1)/sqrt(((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetEntries()),2.));  
  // Double_t BMytra=-(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(2)+((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(2))/2.;
  // Double_t BMytr_err=sqrt(pow(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(2)/sqrt(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetEntries()),2.)  +  pow(((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(2)/sqrt(((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetEntries()),2.));  
    
  // cout<<"Beam Monitor allignment parameters:"<<endl;
  // cout<<"estimated rotation around X axis= "<<BMxrot<<endl;
  // cout<<"estimated rotation around Y axis= "<<BMyrot<<endl;
  // cout<<"estimated translation in X="<<BMxtra<<"   +-   "<<BMxtr_err<<endl;
  // cout<<"estimated translation in Y="<<BMytra<<"   +-   "<<BMytr_err<<endl<<endl;
      
  // TString tmp_str("BM allign par: xrot="); tmp_str+= BMxrot; tmp_str+="  yrot="; tmp_str+=BMyrot;
  // tmp_str+="  x_tra="; tmp_str+=BMxtra; tmp_str+=" +- ";  tmp_str+=BMxtr_err;
  // tmp_str+="  y_tra="; tmp_str+=BMytra; tmp_str+=" +- ";  tmp_str+=BMytr_err;
  // TNamed n("BM_MSD_allign_par",tmp_str.Data());
  // n.Write();
  
  if(debug>0)
    cout<<"Allign_estimate finished"<<endl;
  
  return;
}


