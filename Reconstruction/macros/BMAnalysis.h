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
#include "TAGnameManager.hxx"


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
#include "TAITntuTrack.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDcluster.hxx"
#include "TAMSDntuTrack.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDhit.hxx"
#include "TAMSDtrack.hxx"

#include "TATWparGeo.hxx"
#include "TATWntuPoint.hxx"

#include "TAGntuGlbTrack.hxx"

#include "TAGcampaignManager.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAWDntuTrigger.hxx"
#include "TAGrunInfo.hxx"
#include "TAGparGeo.hxx"

#include "TAMCntuHit.hxx"
#include "TAMCntuRegion.hxx"
#include "TAGntuEvent.hxx"


#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include "Math/DistFunc.h"
#include "Math/GoFTest.h"
#include "Math/Functor.h"

#endif

#define DEG2RAD  0.01745329251994329577
#define RAD2DEG  57.2957795130823208768

//setting global parameters
#define   debug        0

//setting bm analysis parameters
#define   effstep        100
vector<pair<double,double>> selfeff;
vector<pair<double,double>> vtxeff;

using namespace std;


//from runinfo
static int  IncludeREG; 
static int  IncludeGLB;
static int  runNumber;

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

//global variables
TAGroot *gTAGroot;
static int  evnum;             //current event number
static int  maxentries;        //max number of events
static TAGrunInfo*   runinfo;  //runinfo
static TAGgeoTrafo*  geoTrafo; //geometry
static TAGparGeo* parGeo; //beam info
static TAGcampaignManager* campManager;
static TTree *tree;
static  TFile *outputFile;

//detector variables
static TAMCntuHit *bmMCntuHit;
static TAMCntuHit *vtMc;
static TAMCntuHit *itMc;
static TAMCntuHit *msdMc;
static TAMCntuHit *twMc;
static TAMCntuHit *caMc;
static TASTntuHit *stNtuHit;
static TABMntuHit*  bmNtuHit;
static TABMntuTrack*  bmNtuTrack;
static TAVTntuVertex* vtxNtuVertex;
static TAVTntuCluster *vtxNtuCluster;
static TAVTntuTrack *vtxNtuTrack;
static TAMSDntuCluster *msdntuclus;
static TAMSDntuTrack *msdntutrack;
static TAMSDntuPoint *msdNtuPoint;
static TAMSDntuHit *msdNtuHit;
static TAITntuCluster *itClus;
static TAITntuTrack *itntutrack;
static TATWntuPoint *twNtuPoint;
static TAMCntuPart *mcNtuPart;
static TAMCntuRegion *mcNtuReg;
static TAGntuGlbTrack *glbntutrk;
static TAWDntuTrigger *wdTrigInfo;
static TASTparGeo* stparGeo;
static TABMparGeo* bmparGeo;
static TAVTparGeo* vtparGeo;
static TAITparGeo* itparGeo;
static TAMSDparGeo* msdparGeo;
static TATWparGeo* twparGeo;
static TAGntuEvent* tgevent;

//methods to fill plots
//N.B.: these methods are "safe", but slower than the usual ((TH1D*)(gDirectory->Get(graphname)))->Fill etc.
template <class t>
void myfill(const char *graphname, t x){
  if(gDirectory->Get(graphname)!=nullptr){
    ((TH1D*)(gDirectory->Get(graphname)))->Fill(x);
  }else{
    cout<<"ERROR!!!: "<<graphname<<" cannot be filled because it does not exist in gROOT, check BookHisto!"<<endl;
  }
return;
}

template <class t, class u>
void myfill(const char *graphname, t x, u y){
  if(gDirectory->Get(graphname)!=nullptr){
    ((TH2D*)(gDirectory->Get(graphname)))->Fill(x,y);
  }else{
    cout<<"ERROR!!!: "<<graphname<<" cannot be filled because it does not exist in gROOT, check BookHisto!"<<endl;
  }
return;
}

template <class t>
void myweightfill(const char *graphname, t x, double w){
  if(gDirectory->Get(graphname)!=nullptr){
    ((TH1D*)(gDirectory->Get(graphname)))->Fill(x,w);
  }else{
    cout<<"ERROR!!!: "<<graphname<<" cannot be filled because it does not exist in gROOT, check BookHisto!"<<endl;
  }
return;
}

template <class t>
void myweightfill(const char *graphname, t x, t y, double w){
  if(gDirectory->Get(graphname)!=nullptr){
    ((TH2D*)(gDirectory->Get(graphname)))->Fill(x,y,w);
  }else{
    cout<<"ERROR!!!: "<<graphname<<" cannot be filled because it does not exist in gROOT, check BookHisto!"<<endl;
  }
return;
}


TVector3 ProjectToZ(TVector3 Slope, TVector3 Pos0, Double_t FinalZ) {
  return TVector3(Slope.X()/Slope.Z()*(FinalZ-Pos0.Z())+Pos0.X() ,Slope.Y()/Slope.Z()*(FinalZ-Pos0.Z())+Pos0.Y(), FinalZ);
}

//declaration of functions defined below
void EvaluateSelfEfficiency();
void EvaluateVTXeff();

void BookHisto() {

  if(debug>0)
    cout<<"BookHisto start"<<endl;

  outputFile->cd();
  TH1D *h;
  TH2D *h2;

  h2 = new TH2D("bm_target_bmsys","BM tracks on target  projections in BM sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
  h2 = new TH2D("bm_target_glbsys","BM tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
  h = new TH1D("nhitsTot","BM number of hit detected;Number of hits;Events",36,-0.5,35.5);
  h = new TH1D("nhitsXtrack","BM number of hit per track;Number of hits;Events",36,-0.5,35.5);
  h = new TH1D("selfefficiency","BM raw self efficiency;Drift distance;Self efficiency",effstep,0.,0.8);
  h = new TH1D("vtxefficiency","BM vtx projected efficiency;Drift distance;Vtxefficiency",effstep,0.,0.8);
  
  //MC
  h2 = new TH2D("MC_target_enter_profile","profile of the particle exit from the target;X[cm];Y[cm]",500,-5.,5.,500,-5.,5.);
  h = new TH1D("MC_BMvsMC_enter_profileX","BM vs MC primary target profile;BM - MC X[cm];primaries",1000,-0.2,0.2);
  h = new TH1D("MC_BMvsMC_enter_profileY","BM vs MC primary target profile;BM - MC Y[cm];primaries",1000,-0.2,0.2);
  h = new TH1D("MC_BMvsMC_enter_profileMag","BM vs MC primary target profile;BM - MC Mag[cm];primaries",1000,-0.2,0.2);
  h = new TH1D("MC_BMvsMC_AngleX","BM vs MC primary Angular spread;BM - MC SlopeX[rad];primaries",1000,-0.2,0.2);
  h = new TH1D("MC_BMvsMC_AngleY","BM vs MC primary Angular spread;BM - MC SlopeY[rad];primaries",1000,-0.2,0.2);
  h = new TH1D("MC_BMvsMC_AngleMag","BM vs MC primary Angular spread;BM - MC Angle[rad];primaries",1000,-0.2,0.2);

  if(IncludeVT){
    outputFile->mkdir("VT");
    outputFile->cd("VT");
      h = new TH1D("VT_vtx_BMmatched","Is BM matched;BMMatched flag;Events",2,-0.5,1.5);
      h = new TH1D("VT_vtx_num","Number of evtx per event;Number of evtx;Events",21,-0.5,20.5);
      h = new TH1D("VT_vtx_trknum","Number of vtx tracks associated per vertex;Number of tracks;Events",11,-0.5,10.5);
      h2 = new TH2D("vtx_target_vtxsys","VTX vertex on target  projections in VTX sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("vtx_target_glbsys","VTX vertex on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      if(IncludeBM){
        h2 = new TH2D("origin_xx_bmvtx_all","BM track X position vs VTX vertex X position in local ref.;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
        h = new TH1D("BMresidualX","Residual between BM and VTX track on the target plane;BM-VTX X[cm];Tracks",600,-1,1);
        h = new TH1D("BMresidualY","Residual between BM and VTX track on the target plane;BM-VTX Y[cm];Tracks",600,-1,1);
        h = new TH1D("BMresidualMag","Residual between BM and VTX track on the target plane;BM-VTX Mag[cm];Tracks",600,0.,2.);
      }
    gDirectory->cd("..");
    outputFile->cd("..");
  }

  if(debug>0)
    cout<<"BookHisto done"<<endl;

  return;
}

void BMC(){

if(bmNtuTrack->GetTracksN()==1){
  TABMtrack* track = bmNtuTrack->GetTrack(0);
  TVector3 bmgloproj;
  TVector3 bmgloslope=geoTrafo->VecFromBMLocalToGlobal(track->GetSlope());
  bmgloproj=ProjectToZ(bmgloslope geoTrafo->FromBMLocalToGlobal(track->GetOrigin()),geoTrafo->GetTGCenter().Z()-parGeo->GetTargetPar().Size.Z()/2.);  
  TAMCregion* cross;
  Bool_t foundcross=false;
  for (int i=0; i<mcNtuReg->GetRegionsN(); i++) {
    cross=mcNtuReg->GetRegion(i);
    if(cross->GetCrossN()==parGeo->GetRegTarget()){
      foundcross=true;
      break;
    }
  }
  if(foundcross){
    myfill("MC_target_enter_profile", cross->GetPosition().X(),cross->GetPosition().Y());
    myfill("MC_BMvsMC_enter_profileX", bmgloproj.X()-cross->GetPosition().X());
    myfill("MC_BMvsMC_enter_profileY", bmgloproj.Y()-cross->GetPosition().Y());
    myfill("MC_BMvsMC_enter_profileMag", (bmgloproj-cross->GetPosition()).Mag());
    TVector3 crossdir(cross->GetMomentum().X()/cross->GetMomentum().Z(), cross->GetMomentum().Y()/cross->GetMomentum().Z(),1.);
    TVector3 bmdir(bmgloslope.X()/bmgloslope.Z(), bmgloslope.Y()/bmgloslope.Z(), 1.);
    myfill("MC_BMvsMC_AngleX", bmdir.X()-crossdir.X());
    myfill("MC_BMvsMC_AngleY", bmdir.Y()-crossdir.Y());
    myfill("MC_BMvsMC_AngleMag", (bmdir-cross).Mag());

  }

}

  return;
}


//BM analysis
void BeamMonitor(){

  if(debug>0)
    cout<<"BeamMonitor start"<<endl;

  TVector3 bmgloproj, bmlocalproj;

  //BM hits:
  myfill("nhitsTot",bmNtuHit->GetHitsN());

  ///BM tracks
  for( Int_t iTrack = 0; iTrack < bmNtuTrack->GetTracksN(); ++iTrack ) {
    TABMtrack* track = bmNtuTrack->GetTrack(iTrack);
    myfill("nhitsXtrack",track->GetHitsNtot());
    //project to the target in the BM ref., then move to the global ref.
    bmlocalproj=ProjectToZ(track->GetSlope(), track->GetOrigin(),geoTrafo->FromGlobalToBMLocal(geoTrafo->GetTGCenter()).Z());
    bmgloproj=ProjectToZ(geoTrafo->VecFromBMLocalToGlobal(track->GetSlope()), geoTrafo->FromBMLocalToGlobal(track->GetOrigin()),geoTrafo->GetTGCenter().Z());
    myfill("bm_target_bmsys", bmlocalproj.X(),bmlocalproj.Y());
    myfill("bm_target_glbsys", bmgloproj.X(),bmgloproj.Y());
    if(bmNtuTrack->GetTracksN()==1)
      EvaluateSelfEfficiency();
  }

  if(debug>0)
    cout<<"Vertex start"<<endl;

  if(IncludeVT){
    for (Int_t iVtx = 0; iVtx < vtxNtuVertex->GetVertexN(); ++iVtx) {
      TAVTvertex *vtxvertex = vtxNtuVertex->GetVertex(iVtx);
      if (vtxvertex == 0x0)
        continue;
      TVector3 vtxvertpos = vtxvertex->GetVertexPosition();
      vtxvertpos = geoTrafo->FromVTLocalToGlobal(vtxvertpos);
      myfill("VT/VT_vtx_trknum",vtxvertex->GetTracksN());
      myfill("VT/VT_vtx_BMmatched",vtxvertex->IsBmMatched());
      if(vtxNtuVertex->GetVertexN()==1){
        if(vtxvertex->GetTracksN()==1){
          TAVTtrack* vttrack = vtxvertex->GetTrack(0);
          TVector3 vtxlocalproj=ProjectToZ(vttrack->GetSlopeZ(), vttrack->GetOrigin(),geoTrafo->FromGlobalToVTLocal(geoTrafo->GetTGCenter()).Z());
          myfill("VT/vtx_target_vtxsys",vtxlocalproj.X(),vtxlocalproj.Y());
          TVector3 vtxprojglo=geoTrafo->FromVTLocalToGlobal(vtxlocalproj);
          myfill("VT/vtx_target_glbsys",vtxprojglo.X(),vtxprojglo.Y());
          if(bmNtuTrack->GetTracksN()==1){
            TVector3 vtgloproj=ProjectToZ(geoTrafo->VecFromVTLocalToGlobal(vttrack->GetSlopeZ()), geoTrafo->FromVTLocalToGlobal(vttrack->GetOrigin()),geoTrafo->GetTGCenter().Z());
            myfill("VT/origin_xx_bmvtx_all", bmgloproj.X(),vtgloproj.X()); 
            TVector3 residual=bmgloproj-vtgloproj;
            myfill("VT/BMresidualX", residual.X());   
            myfill("VT/BMresidualY", residual.Y());   
            myfill("VT/BMresidualMag", residual.Mag());   
            EvaluateVTXeff();
          }
        }
      }
    }
  }

  if(debug>0)
    cout<<"BeamMonitor done"<<endl;

  return;
}

void EvaluateSelfEfficiency(){

  if(bmNtuTrack->GetTracksN()!=1)
    return;

  if(debug>0)
    cout<<"EvaluateSelfEfficiency start"<<endl;

  int xviewhits[6]={-1,-1,-1,-1,-1,-1}, yviewhits[6]={-1,-1,-1,-1,-1,-1}; //xview->wire along x view and provide yz coordinate measurement
  map<int, double> rejdrift; //cellid, rdrift
  TABMtrack* bmtrack = bmNtuTrack->GetTrack(0);  
  //order all the bm hits
  for(Int_t i=0;i<bmNtuHit->GetHitsN();i++){
    TABMhit *bmhit=bmNtuHit->GetHit(i);
    if(bmhit->GetIsSelected()==1){
      if(bmhit->GetView()==0)
        xviewhits[bmhit->GetPlane()]=i;
      else
        yviewhits[bmhit->GetPlane()]=i;
      int effbin=(int) (bmhit->GetRdrift()/0.8*effstep);
      if(effbin<effstep){
        selfeff.at(effbin).first++;
        selfeff.at(effbin).second++;
      }
    }else{
      rejdrift[bmhit->GetIdCell()]=max(bmhit->GetRdrift(), rejdrift[bmhit->GetIdCell()]);
    }
  }

  if(debug>0)
    cout<<"EvaluateSelfEfficiency: BM ordering done"<<endl;

  for(Int_t plane=0;plane<6;plane++){
    if(xviewhits[plane]==-1){
      for(Int_t cell=0; cell<3;cell++){
        int cellid=bmparGeo->GetBMNcell(plane, 0, cell);
        if(bmNtuHit->GetCellOccupy(cellid)==0){
          double driftdist=bmparGeo->PointLineDist(bmtrack->GetSlope().Y()/bmtrack->GetSlope().Z(), bmtrack->GetOrigin().Y(), bmparGeo->GetWireY(bmparGeo->GetSenseId(cell), plane, 0), bmparGeo->GetWireZ(bmparGeo->GetSenseId(cell), plane, 0));
          if(driftdist<0.6){ //inefficiency detected 
            int effbin=(int) (driftdist/0.8*effstep);
            if(effbin<effstep)
              selfeff.at(effbin).second++;
            break; //end the loop
          }else if(driftdist<0.8){//check if the contiguous cell have a hit with large rdrift since this apparent inefficiency could be due to track resolution
            if(cell<2){
              if(rejdrift.count(cellid+1))
                if(rejdrift.at(cellid+1)>0.6)
                  break;
            }
            if(cell>0){
              if(rejdrift.count(cellid+-1))
                if(rejdrift.at(cellid-1)>0.6)
                  break;              
            }
            int effbin=(int) (driftdist/0.8*effstep);
            if(effbin<effstep)
              selfeff.at(effbin).second++;   
            break;         
          }
        }
      }
    }
    if(yviewhits[plane]==-1){
      for(Int_t cell=0; cell<3;cell++){
        int cellid=bmparGeo->GetBMNcell(plane, 1, cell);
        if(bmNtuHit->GetCellOccupy(cellid)==0){
          double driftdist=bmparGeo->PointLineDist(bmtrack->GetSlope().X()/bmtrack->GetSlope().Z(), bmtrack->GetOrigin().X(), bmparGeo->GetWireX(bmparGeo->GetSenseId(cell), plane, 1), bmparGeo->GetWireZ(bmparGeo->GetSenseId(cell), plane, 1));
          if(driftdist<0.6){
            int effbin=(int) (driftdist/0.8*effstep);
            if(effbin<effstep)
              selfeff.at(effbin).second++;
            break; //end the loop
          }else if(driftdist<0.8){//check if the contiguous cell have a hit with large rdrift since this apparent inefficiency could be due to track resolution
            if(cell<2){
              if(rejdrift.count(cellid+1))
                if(rejdrift.at(cellid+1)>0.6)
                  break;
            }
            if(cell>0){
              if(rejdrift.count(cellid+-1))
                if(rejdrift.at(cellid-1)>0.6)
                  break;              
            }
            int effbin=(int) (driftdist/0.8*effstep);
            if(effbin<effstep)
              selfeff.at(effbin).second++;   
            break;         
          }
        }
      }
    }    
  }

  if(debug>0)
    cout<<"EvaluateSelfEfficiency done"<<endl;

  return;
}

void EvaluateVTXeff(){
  
  TAVTtrack* vttrack = vtxNtuVertex->GetVertex(0)->GetTrack(0);
  for(Int_t plane=0;plane<6;plane++){
    for(Int_t view=0;view<2;view++){
      for(Int_t cell=0; cell<3;cell++){
        int cellid=bmparGeo->GetBMNcell(plane, view, cell);
        double driftdist=bmparGeo->PointLineDist(vttrack->GetSlopeZ().X()/vttrack->GetSlopeZ().Z(), vttrack->GetOrigin().X(), (view==0) ? bmparGeo->GetWireY(bmparGeo->GetSenseId(cell), plane, 0) : bmparGeo->GetWireX(bmparGeo->GetSenseId(cell), plane, 1), bmparGeo->GetWireZ(bmparGeo->GetSenseId(cell), plane, view));
        if(driftdist<0.8){
          int effbin=(int) (driftdist/0.8*effstep);
          if(effbin<effstep){
            vtxeff.at(effbin).second++;
            if(bmNtuHit->GetCellOccupy(cellid)>0){
              vtxeff.at(effbin).first++;
            }else if(driftdist>0.6){
              if(cell<2){
                if(bmNtuHit->GetCellOccupy(cellid+1)>0){
                  for(Int_t i=0;i<bmNtuHit->GetHitsN();i++){
                    TABMhit *bmhit=bmNtuHit->GetHit(i);
                    if(bmhit->GetIdCell()==cellid+1){
                      if(bmhit->GetRdrift()>0.6)
                        vtxeff.at(effbin).first++;
                      if(cell==0)
                        cell=3;
                      break;
                    }
                  }
                }
              }
              if(cell>1){
                if(bmNtuHit->GetCellOccupy(cellid-1)>0){
                  for(Int_t i=0;i<bmNtuHit->GetHitsN();i++){
                    TABMhit *bmhit=bmNtuHit->GetHit(i);
                    if(bmhit->GetIdCell()==cellid-1){
                      if(bmhit->GetRdrift()>0.6)
                        vtxeff.at(effbin).first++;
                      cell=3;
                      break;
                    }
                  }
                }                
              }
            }
          }
          break;
        }        
      } 
    }  
  }  

  return;
}

void PostLoopAnalysis(){

  for(int i=0;i<effstep;i++){
    if(selfeff.at(i).second!=0){
      ((TH1D*)(gDirectory->Get("selfefficiency")))->SetBinContent(i+1,(selfeff.at(i).first+1.)/(selfeff.at(i).second+2.));
      ((TH1D*)(gDirectory->Get("selfefficiency")))->SetBinError(i+1,sqrt((selfeff.at(i).first+1.)*(selfeff.at(i).first+2.)/(selfeff.at(i).second+2.)/(selfeff.at(i).second+3.)-(selfeff.at(i).first+1.)*(selfeff.at(i).first+1.)/(selfeff.at(i).second+2.)/(selfeff.at(i).second+2.)));
    }
    if(vtxeff.at(i).second!=0){
      ((TH1D*)(gDirectory->Get("vtxefficiency")))->SetBinContent(i+1,(vtxeff.at(i).first+1.)/(vtxeff.at(i).second+2.));
      ((TH1D*)(gDirectory->Get("vtxefficiency")))->SetBinError(i+1,sqrt((vtxeff.at(i).first+1.)*(vtxeff.at(i).first+2.)/(vtxeff.at(i).second+2.)/(vtxeff.at(i).second+3.)-(vtxeff.at(i).first+1.)*(vtxeff.at(i).first+1.)/(vtxeff.at(i).second+2.)/(vtxeff.at(i).second+2.)));
    }
  }

  //fit nhits
  TF1 nhitsfit ("nhitsfit", "[3]*(TMath::Exp(([1]*([0]-x))-TMath::Exp([2]*([0]-x))))",5,21);  
  nhitsfit.SetParameters(12,3,2,1000);
  ((TH1D*)(gDirectory->Get("nhitsTot")))->Fit("nhitsfit", "qB+",0,36);
  cout<<"nhitsTot fitting distribution:"<<endl<<nhitsfit.GetFormula()->GetExpFormula()<<endl;
  cout<<"Fitted_parameters:"<<endl;
  for(int i=0;i<nhitsfit.GetNpar();i++)
    cout<<nhitsfit.GetParameter(i)<<"  ";
  cout<<endl;

  //fit hit selfefficiency
  TF1 selfefffit ("selfefffit", "1./([0]*exp(x/[1])+1.)+[2]*x",0,0.8);  
  selfefffit.SetParameters(6.41785e-07  0.0494121  -0.182363);
  ((TH1D*)(gDirectory->Get("selfefficiency")))->Fit("selfefffit", "qB+",0,36);
  cout<<"selfefficiency fitting distribution:"<<endl<<selfefffit.GetFormula()->GetExpFormula()<<endl;
  cout<<"Fitted_parameters:"<<endl;
  for(int i=0;i<selfefffit.GetNpar();i++)
    cout<<selfefffit.GetParameter(i)<<"  ";
  cout<<endl;

  //fit hit vtxefficiency
  TF1 vtxefffit ("vtxefffit", "1./([0]+[1]*exp(x*[2]))",0,1);  
  vtxefffit.SetParameters(12,3,2,1000);
  ((TH1D*)(gDirectory->Get("vtxefficiency")))->Fit("vtxefffit", "qB+",0,36);
  cout<<"vtxefficiency fitting distribution:"<<endl<<vtxefffit.GetFormula()->GetExpFormula()<<endl;
  cout<<"Fitted_parameters:"<<endl;
  for(int i=0;i<vtxefffit.GetNpar();i++)
    cout<<vtxefffit.GetParameter(i)<<"  ";
  cout<<endl;


  return;
}

//inizialize few things:
void InitializeVariables(){
  selfeff.assign(effstep,make_pair(0.,0.));
  vtxeff.assign(effstep,make_pair(0.,0.));
  return;
}

void DeleteVariables(){
  return;
}

Int_t OpenInputFile(TFile *inputFile){
  if(inputFile->IsOpen()==false){
    cout<<"FATAL ERROR: I cannot open the input file"<<endl;
    return -1;
  }
  tree = (TTree*)inputFile->Get("tree");
  if(tree==nullptr){
    cout<<"FATAL ERROR: your input file do not contains a tree"<<endl;
    return -1;
  }
return 0;
}

void SetOutputFiles(TString nameFile){
  Int_t pos = nameFile.Last('.');
  TString nameOut;
  nameOut = nameFile(nameFile.Last('/')+1, pos);
  nameOut.Prepend("readshoetreeout_");
  nameOut.Append("_out.root");
  outputFile = new TFile(nameOut,"RECREATE");  
  return;
}

  //define and charge varaibles and geometrical parameters from the campmanager
Int_t ChargeCampaignParameters(TFile *inputFile){
  gTAGroot=new TAGroot();
  runinfo=(TAGrunInfo*)(inputFile->Get("runinfo"));
  const TAGrunInfo construninfo(*runinfo);
  gTAGroot->SetRunInfo(construninfo);
  TString expName=runinfo->CampaignName();
  if(expName.EndsWith("/")) //fix a bug present in shoe
    expName.Remove(expName.Length()-1);
  runNumber=runinfo->RunNumber();
  TAGrecoManager::Instance(expName);
  campManager = new TAGcampaignManager(expName);
  campManager->FromFile();
  IncludeREG=runinfo->GetGlobalPar().EnableRegionMc;
  if(tree)
    tree->ls();
  else
    cout<<"tree è un dannato nullptr dimmerda"<<endl;
  IncludeGLB=(tree->FindBranch("glbtrack.")!=nullptr);
  IncludeDAQ=(tree->FindBranch("evt.")!=nullptr);
  IncludeMC=campManager->GetCampaignPar(campManager->GetCurrentCamNumber()).McFlag;
  IncludeDI=campManager->IsDetectorOn("DI");
  IncludeSC=campManager->IsDetectorOn("ST");
  IncludeBM=campManager->IsDetectorOn("BM");
  IncludeTG=campManager->IsDetectorOn("TG");
  IncludeVT=campManager->IsDetectorOn("VT");
  IncludeIT=campManager->IsDetectorOn("IT");
  IncludeMSD=campManager->IsDetectorOn("MSD");
  IncludeTW=campManager->IsDetectorOn("TW");
  IncludeCA=campManager->IsDetectorOn("CA");

  if(IncludeMC && IncludeDAQ){
    cout<<"IncludeMC and IncludeDAQ are both true... check your input file and the configuration files, this program will be ended"<<endl;
    return -1;
  }  

  return 0;
}


//method used to charge the par files
void ChargeParFiles(Int_t nentries){
    //define and charge the detectors par files
  geoTrafo = new TAGgeoTrafo();
  TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
  geoTrafo->FromFile(parFileName);

  TAGparaDsc* fpParGeoG = new TAGparaDsc(new TAGparGeo());
  parGeo = (TAGparGeo*)fpParGeoG->Object();
  parFileName = campManager->GetCurGeoFile(TAGparGeo::GetBaseName(), runNumber);
  parGeo->FromFile(parFileName.Data());

  TAGparaDsc* parGeoSt = new TAGparaDsc(new TASTparGeo());
  stparGeo = (TASTparGeo*)parGeoSt->Object();
  parFileName = campManager->GetCurGeoFile(TASTparGeo::GetBaseName(), runNumber);
  stparGeo->FromFile(parFileName);

  TAGparaDsc* parGeoBm = new TAGparaDsc(new TABMparGeo());
  bmparGeo = (TABMparGeo*)parGeoBm->Object();
  parFileName = campManager->GetCurGeoFile(TABMparGeo::GetBaseName(), runNumber);
  bmparGeo->FromFile(parFileName);

  TAGparaDsc* parGeoVtx = new TAGparaDsc(new TAVTparGeo());
  vtparGeo = (TAVTparGeo*)parGeoVtx->Object();
  parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
  vtparGeo->FromFile(parFileName);
   Int_t sensorsNvt = vtparGeo->GetSensorsN();

  TAGparaDsc* parGeoIt = new TAGparaDsc(new TAITparGeo());
  itparGeo = (TAITparGeo*)parGeoIt->Object();
  parFileName = campManager->GetCurGeoFile(TAITparGeo::GetBaseName(), runNumber);
  itparGeo->FromFile(parFileName);
  Int_t sensorsNit = itparGeo->GetSensorsN();

  TAGparaDsc* parGeoMsd = new TAGparaDsc(new TAMSDparGeo());
  msdparGeo = (TAMSDparGeo*)parGeoMsd->Object();
  parFileName = campManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), runNumber);
  msdparGeo->FromFile(parFileName);
  Int_t sensorsNms = msdparGeo->GetSensorsN();
  Int_t stationsNms = msdparGeo->GetStationsN();

  TAGparaDsc* parGeoTw = new TAGparaDsc(new TATWparGeo());
  twparGeo = (TATWparGeo*)parGeoTw->Object();
  parFileName = campManager->GetCurGeoFile(TATWparGeo::GetBaseName(), runNumber);
  twparGeo->FromFile(parFileName);

  //define and load the detector interesting quantities that will be passed in the analysis methods
  if(IncludeSC>0){
    stNtuHit= new TASTntuHit();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(stNtuHit->ClassName()), &stNtuHit);
  }

  if(IncludeBM){
    bmNtuHit = new TABMntuHit();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(bmNtuHit->ClassName()), &bmNtuHit);
    bmNtuTrack = new TABMntuTrack();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(bmNtuTrack->ClassName()), &bmNtuTrack);
    if(IncludeMC){
      bmMCntuHit = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kBM), &bmMCntuHit);
    }
  }
  
  if(IncludeVT){
    vtxNtuVertex = new TAVTntuVertex();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(vtxNtuVertex->ClassName()), &vtxNtuVertex);
    vtxNtuTrack = new TAVTntuTrack();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(vtxNtuTrack->ClassName()), &vtxNtuTrack);
    Int_t sensorsN = vtparGeo->GetSensorsN();
    vtxNtuCluster = new TAVTntuCluster(sensorsN);
     
    tree->SetBranchAddress(TAGnameManager::GetBranchName(vtxNtuCluster->ClassName()), &vtxNtuCluster);
    if(IncludeMC){
      vtMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kVTX), &vtMc);
    }
  }

  if(IncludeIT){
    itClus = new TAITntuCluster(sensorsNit);
    tree->SetBranchAddress(TAGnameManager::GetBranchName(itClus->ClassName()), &itClus);
    itntutrack= new TAITntuTrack();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(itntutrack->ClassName()), &itntutrack);
    if(IncludeMC){
      itMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kITR), &itMc);
    }
  }

  if(IncludeMSD){
    msdntuclus= new TAMSDntuCluster(sensorsNms);
    tree->SetBranchAddress(TAGnameManager::GetBranchName(msdntuclus->ClassName()), &msdntuclus);
    msdntutrack= new TAMSDntuTrack();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(msdntutrack->ClassName()), &msdntutrack);
    msdNtuPoint= new TAMSDntuPoint(stationsNms);
    tree->SetBranchAddress(TAGnameManager::GetBranchName(msdNtuPoint->ClassName()), &msdNtuPoint);
    msdNtuHit= new TAMSDntuHit(sensorsNms);
    tree->SetBranchAddress(TAGnameManager::GetBranchName(msdNtuHit->ClassName()), &msdNtuHit);
    if(IncludeMC){
      msdMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kMSD), &msdMc);
    }    
  }

  if(IncludeTW){
    twNtuPoint = new TATWntuPoint();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(twNtuPoint->ClassName()), &twNtuPoint);
    if(IncludeMC){
      twMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kTW), &twMc);
    }
  }

  if(IncludeMC>0){
    mcNtuPart = new TAMCntuPart();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(mcNtuPart->ClassName()), &mcNtuPart);
    if(IncludeREG){
      mcNtuReg=new TAMCntuRegion();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(mcNtuReg->ClassName()), &mcNtuReg);
    }
  }else if(IncludeDAQ){
    wdTrigInfo=new TAWDntuTrigger();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(wdTrigInfo->ClassName()), &wdTrigInfo);
    tgevent= new TAGntuEvent();
    tree->SetBranchAddress("evt.", &tgevent);
  }

  if(IncludeGLB){
    glbntutrk = new TAGntuGlbTrack();
    tree->SetBranchAddress(TAGntuGlbTrack::GetBranchName(), &glbntutrk);
  }

  if (nentries == 0 || nentries>tree->GetEntries())
    maxentries = tree->GetEntries();
  else
    maxentries=nentries;

  return;
}
