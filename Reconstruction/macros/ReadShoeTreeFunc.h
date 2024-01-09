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
#include "TABMntuRaw.hxx"

#include "TADIparGeo.hxx"

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

#include "TACAparGeo.hxx"
#include "TACAntuHit.hxx"
#include "TACAntuCluster.hxx"
#include "TACAntuRaw.hxx"

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

#endif

using namespace std;

//from runinfo (given by the input file), campaign manager (that read the cammpas/yourfilecampaign.cam file) and tagrecomanager (that read the config/campaign/FootGlobal.par file)
//all the static variables can be used and even modified everywhere in the code. 
//In principle, all the static variables are inizialized/charged 
static int  IncludeREG; 
static int  IncludeGLB;
static int  runNumber;
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
static int debug;             //debug flag
static int  evnum;             //current event number
static int  maxentries;        //max number of events in ttree or set by the user
static TAGrunInfo*   runinfo;  //runinfo from the input file
static TAGgeoTrafo*  geoTrafo; //useful geometry class that handle all the detector/global transformations and units conversions 
static TAGparGeo* parGeo;      //beam and target info
static TAGcampaignManager* campManager; //campaign manager
static  TTree *tree;                    //input tree
static  TFile *inputFile;
static  TFile *outputFile;
TAGroot *gTAGroot;

//detector and MC variables
static TAMCntuHit *scNtuMc;
static TAMCntuHit *bmNtuMc;
static TAMCntuHit *vtNtuMc;
static TAMCntuHit *itNtuMc;
static TAMCntuHit *msdNtuMc;
static TAMCntuHit *twNtuMc;
static TAMCntuHit *caNtuMc;
static TASTntuHit *stNtuHit;
static TASTntuHit *stNtuRaw;
static TABMntuHit*  bmNtuHit;
static TABMntuRaw*  bmNtuRaw;
static TABMntuTrack*  bmNtuTrack;
static TAVTntuVertex* vtxNtuVertex;
static TAVTntuCluster *vtxNtuClus;
static TAVTntuTrack *vtxNtuTrack;
static TAVTntuHit *vtxNtuHit;
static TAMSDntuCluster *msdNtuClus;
static TAMSDntuTrack *msdNtuTrack;
static TAMSDntuPoint *msdNtuPoint;
static TAMSDntuHit *msdNtuHit;
static TAITntuCluster *itNtuClus;
static TAITntuTrack *itNtuTrack;
static TATWntuPoint *twNtuPoint;
static TATWntuRaw *twNtuRaw;
static TACAntuHit *caNtuHit;
static TACAntuCluster *caNtuClus;
static TACAntuRaw *caNtuRaw;
static TAMCntuPart *mcNtuPart;
static TAMCntuRegion *mcNtuReg;
static TAGntuGlbTrack *glbNtuTrack;
static TAGntuEvent* tgevent;
static TAWDntuTrigger *wdTrigInfo;
static TASTparGeo* stparGeo;
static TABMparGeo* bmparGeo;
static TAVTparGeo* vtparGeo;
static TAITparGeo* itparGeo;
static TAMSDparGeo* msdparGeo;
static TATWparGeo* twparGeo;
static TACAparGeo* caparGeo;

//methods to fill plots
//N.B.!!!! these methods are "safe", but SLOWER than the usual ((TH1D*)(gDirectory->Get(graphname)))->Fill etc.
//use myfill("plotname",variabletobefilled) or myfill("plotname",firstvariabletobefilled, secondvariabletobefilled) or to fill h1 and h2 plots
//use myweightfill("plotname",variabletobefilled, weight) to fill h1 and h2 plots with weight
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


//declare here your hitstos
void BookHisto() {

  if(debug)
    cout<<"BookHisto start"<<endl;

  outputFile->cd();
  TH1D *h;
  TH2D *h2;

  if(IncludeVT){
    outputFile->mkdir("VT");
    outputFile->cd("VT");
      h = new TH1D("VT_vtx_num","Number of evtx per event;Number of evtx;Events",21,-0.5,20.5);
      h = new TH1D("VT_vtx_trknum","Number of vtx tracks associated per vertex;Number of tracks;Events",11,-0.5,10.5);
      h2 = new TH2D("vtx_target_vtxsys","VTX tracks on target projections in vtx sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      h2 = new TH2D("vtx_target_glbsys","VTX tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
      if(IncludeBM)
        h2 = new TH2D("origin_xx_bmvtx_all","BM track X position vs VTX vertex X position in local ref.;BM originX;vtx originX",600,-3.,3.,600,-3.,3.);
      if(IncludeMC){
        h = new TH1D("mc_deltaE","MC energy loss in VTX layers;Energy loss [MeV];Events",100,0.,10.);
        h2 = new TH2D("mc_vtxsize_vs_charge","vertex cluster size vs charge from MC;cluster size;MC charge",51,-0.5,50.5,12,-1.5,10.5);        
      }
    gDirectory->cd("..");
    outputFile->cd("..");
  }
  

  if(IncludeGLB){
    outputFile->mkdir("GLB");
    outputFile->cd("GLB");
      h = new TH1D("trknum","Number of glb tracks per event;Number of tracks;Events",11,-0.5,10.5);
      h = new TH1D("trkpoints","Number of points per track;Number of points;Events",13,-0.5,12.5);
      h = new TH1D("vtpoints","Number of vt points per track;Number of vt points;Events",5,-0.5,4.5);
    gDirectory->cd("..");
    outputFile->cd("..");
  }

  if(IncludeDAQ){
    outputFile->mkdir("DAQ");
    outputFile->cd("DAQ");
      h = new TH1D("daq_acqrate","Mean acquisition rate with 1/timeSinceLastTrigger;Daq acquisition rate [khz];events",400,0.,10.);
    gDirectory->cd("..");
    outputFile->cd("..");
  }

  if(IncludeMC){
    outputFile->mkdir("MC");
    outputFile->cd("MC");
    h = new TH1D("part_num","Total number of particle per event;Number of particels;Events",21,-0.5,20.5);
    h = new TH1D("part_charge","Charge distribution;Charge;Number of particles",22,-1.5,20.5);
    h2 = new TH2D("target_exit_profile","profile of the particle exit from the target;X[cm];Y[cm]",500,-5.,5.,500,-5.,5.);
    if(IncludeREG){
      h2 = new TH2D("target_chavsmass","charge and mass of the particles exit from the target;Z;Mass [GeV]",12,-1.5,10.5,40,0.,20.);      
    }
    gDirectory->cd("..");
    outputFile->cd("..");
  }

  if(debug)
    cout<<"BookHisto done"<<endl;

  return;
}


//Vertex analysis
void Vertex(){

  if(debug)
    cout<<"Vertex start"<<endl;

  myfill("VT/VT_vtx_num",vtxNtuVertex->GetVertexN());
  for (Int_t iVtx = 0; iVtx < vtxNtuVertex->GetVertexN(); ++iVtx) {
    TAVTvertex *vtxvertex = vtxNtuVertex->GetVertex(iVtx);
    if (vtxvertex == 0x0)
      continue;
    TVector3 vtxvertpos = vtxvertex->GetVertexPosition();
    vtxvertpos = geoTrafo->FromVTLocalToGlobal(vtxvertpos);
    myfill("VT/VT_vtx_trknum",vtxvertex->GetTracksN());
    if(vtxvertex->IsBmMatched()){
      for( Int_t iTrack = 0; iTrack < vtxvertex->GetTracksN(); ++iTrack ) { //loop on vertex tracks
        TAVTtrack* track = vtxvertex->GetTrack(iTrack);
        TVector3 vtxlocalproj=ProjectToZ(track->GetSlopeZ(), track->GetOrigin(),geoTrafo->FromGlobalToVTLocal(geoTrafo->GetTGCenter()).Z());
        myfill("VT/vtx_target_vtxsys",vtxlocalproj.X(),vtxlocalproj.Y());
        TVector3 vtxprojglo=geoTrafo->FromVTLocalToGlobal(vtxlocalproj);
        myfill("VT/vtx_target_glbsys",vtxprojglo.X(),vtxprojglo.Y());
        if(IncludeBM)
          if(bmNtuTrack->GetTracksN()==1)
             myfill("VT/origin_xx_bmvtx_all", bmNtuTrack->GetTrack(0)->GetOrigin().X(),vtxvertex->GetVertexPosition().X());   
      }
    }
  }

  if(IncludeMC){
    for(Int_t i=0;i<vtNtuMc->GetHitsN();i++){
      TAMChit *vthit=vtNtuMc->GetHit(i);
      myfill("VT/mc_deltaE",vthit->GetDeltaE()*geoTrafo->GevToMev());
    }
    for(Int_t i=0;i<vtxNtuClus->GetClustersN(0);i++){
      TAVTcluster*  vtcluster=vtxNtuClus->GetCluster(0,i);
      Int_t maxmccha=-99;
      for(Int_t k=0;k<vtcluster->GetMcTracksN();k++){
        TAMCpart* mcpart=mcNtuPart->GetTrack(vtcluster->GetMcTrackIdx(k));
        if(mcpart->GetCharge()>maxmccha)
          maxmccha=mcpart->GetCharge();
      }
      myfill("VT/mc_vtxsize_vs_charge", vtcluster->GetPixelsN(),maxmccha);
    }
  }

  if(debug)
    cout<<"Vertex analysis done"<<endl;

  return;
}

//data acquisition analysis
void DataAcquisition(){

  if(debug)
    cout<<"DataAcquisition analysis start"<<endl;

  myfill("DAQ/daq_acqrate",100./tgevent->GetTimeSinceLastTrigger()); //tgevent->GetTimeSinceLastTrigger() is in decisec.

  if(debug)
    cout<<"DataAcquisition done"<<endl;
  return;
}

//global track analysis
void GLBTRKstudies(){
  
  if(debug)
    cout<<"GLBTRKstudies start"<<endl;

  myfill("GLB/trknum",glbNtuTrack->GetTracksN());
  for(Int_t i=0;i<glbNtuTrack->GetTracksN();i++){
    TAGtrack* track=glbNtuTrack->GetTrack(i);
    myfill("GLB/trkpoints",track->GetPointsN());
    Int_t vtpt=0;
    for(Int_t k=0;k<track->GetPointsN();k++){
      TAGpoint* point=track->GetPoint(k);
      TString detname = point->GetDevName();
      if(detname.Contains("VT"))
        vtpt++;
    }
    myfill("GLB/vtpoints",vtpt);
  } 

  if(debug)
    cout<<"GLBTRKstudies end"<<endl;

return;
}

//MC analysis
void MonteCarlo(){

  if(debug)
    cout<<"MC start"<<endl;
    
  myfill("MC/part_num", mcNtuPart->GetTracksN());
  for( Int_t iTrack = 0; iTrack < mcNtuPart->GetTracksN(); ++iTrack ) {
    TAMCpart* mcpart = mcNtuPart->GetTrack(iTrack);
    myfill("MC/part_charge", mcpart->GetCharge());
  }
  //crossing studies
  if(IncludeREG){
    for (int i=0; i<mcNtuReg->GetRegionsN(); i++) {
      TAMCregion* cross=mcNtuReg->GetRegion(i);
      if(cross->GetCrossN()==parGeo->GetRegTarget()){
        myfill("MC/target_exit_profile", cross->GetPosition().X(),cross->GetPosition().Y());
        TAMCpart* mcpart = mcNtuPart->GetTrack(cross->GetTrackIdx()-1);
        myfill("MC/target_chavsmass", mcpart->GetCharge(), mcpart->GetMass());        
      }
    }
  }

  if(debug)
    cout<<"MC done"<<endl;
 
  return;
}

//open output files
void SetOutputFiles(TString nameFile){
  Int_t pos = nameFile.Last('.');
  TString nameOut;
  nameOut = nameFile(nameFile.Last('/')+1, pos);
  nameOut.Prepend("readshoetreeout_");
  outputFile = new TFile(nameOut,"RECREATE");  
  return;
}

//**************************************************************************** be aware of what you change from here on out ****************************** 
//the following methods are used to open the input file, charge the parameters etc., they should be changed only if really needed

//open the input files
Int_t OpenInputFile(TString nameFile){
  inputFile = new TFile(nameFile.Data());  
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


  //define and charge varaibles and geometrical parameters from the campmanager
Int_t ChargeCampaignParameters(){

  if(debug)
    cout<<"ChargeCampaignParameters start"<<endl;

  gTAGroot=new TAGroot();
  runinfo=(TAGrunInfo*)(inputFile->Get("runinfo"));
  if(debug)
    runinfo->Print();
  const TAGrunInfo construninfo(*runinfo);
  gTAGroot->SetRunInfo(construninfo);
  TString expName=runinfo->CampaignName();
  if(expName.EndsWith("/")) //fix a bug present in shoe
    expName.Remove(expName.Length()-1);
  runNumber=runinfo->RunNumber();
  TAGrecoManager::Instance(expName);
  TAGrecoManager::GetPar()->FromFile();
  if(debug)
    TAGrecoManager::GetPar()->Print();
  campManager = new TAGcampaignManager(expName);
  campManager->FromFile();
  if(debug)
    campManager->Print();
  IncludeREG=(runinfo->GetGlobalPar().EnableRegionMc && TAGrecoManager::GetPar()->IsRegionMc());
  IncludeGLB=(runinfo->GetGlobalPar().EnableTracking && (runinfo->GetGlobalPar().IncludeKalman || runinfo->GetGlobalPar().IncludeTOE) &&  (TAGrecoManager::GetPar()->IsTracking() && (TAGrecoManager::GetPar()->IncludeTOE() || TAGrecoManager::GetPar()->IncludeKalman() || TAGrecoManager::GetPar()->IncludeStraight())));
  IncludeMC=campManager->GetCampaignPar(campManager->GetCurrentCamNumber()).McFlag;
  IncludeDAQ=!IncludeMC;  
  IncludeDI=campManager->IsDetectorOn(TADIparGeo::GetBaseName());
  IncludeSC=campManager->IsDetectorOn(TASTparGeo::GetBaseName()) && runinfo->GetGlobalPar().IncludeST && TAGrecoManager::GetPar()->IncludeST();
  IncludeBM=campManager->IsDetectorOn(TABMparGeo::GetBaseName()) && runinfo->GetGlobalPar().IncludeBM && TAGrecoManager::GetPar()->IncludeBM();
  IncludeTG=campManager->IsDetectorOn(TAGparGeo::GetBaseName());
  IncludeVT=campManager->IsDetectorOn(TAVTparGeo::GetBaseName()) && runinfo->GetGlobalPar().IncludeVT && TAGrecoManager::GetPar()->IncludeVT();
  IncludeIT=campManager->IsDetectorOn(TAITparGeo::GetBaseName()) && runinfo->GetGlobalPar().IncludeIT && TAGrecoManager::GetPar()->IncludeIT();
  IncludeMSD=campManager->IsDetectorOn(TAMSDparGeo::GetBaseName()) && runinfo->GetGlobalPar().IncludeMSD && TAGrecoManager::GetPar()->IncludeMSD();
  IncludeTW=campManager->IsDetectorOn(TATWparGeo::GetBaseName()) && runinfo->GetGlobalPar().IncludeTW && TAGrecoManager::GetPar()->IncludeTW();
  IncludeCA=campManager->IsDetectorOn(TACAparGeo::GetBaseName()) && runinfo->GetGlobalPar().IncludeCA && TAGrecoManager::GetPar()->IncludeCA();

  if(IncludeMC && IncludeDAQ){
    cout<<"IncludeMC and IncludeDAQ are both true... check your input file and the configuration files, this program will be ended"<<endl;
    return -1;
  }  

  if(debug)
    cout<<"ChargeCampaignParameters done"<<endl;

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

  TAGparaDsc* parGeoCa = new TAGparaDsc(new TACAparGeo());
  caparGeo = (TACAparGeo*)parGeoCa->Object();
  parFileName = campManager->GetCurGeoFile(TACAparGeo::GetBaseName(), runNumber);
  caparGeo->FromFile(parFileName);

  //define and load the detector interesting quantities that will be passed in the analysis methods
  if(IncludeSC){
    if(runinfo->GetGlobalPar().EnableSaveHits){
      stNtuHit= new TASTntuHit();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(stNtuHit->ClassName()), &stNtuHit);
    }
    if(IncludeMC){
      scNtuMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kST), &scNtuMc);      
    }
  }

  if(IncludeBM){
    if(runinfo->GetGlobalPar().EnableSaveHits){
      bmNtuHit = new TABMntuHit();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(bmNtuHit->ClassName()), &bmNtuHit);
    }
    if(runinfo->GetGlobalPar().EnableTracking){
      bmNtuTrack = new TABMntuTrack();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(bmNtuTrack->ClassName()), &bmNtuTrack);
    }
    if(IncludeMC){
      bmNtuMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kBM), &bmNtuMc);
    }
    if(IncludeDAQ){
      bmNtuRaw = new TABMntuRaw();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(bmNtuRaw->ClassName()), &bmNtuRaw);
    }
  }
  
  if(IncludeVT){
    if(runinfo->GetGlobalPar().EnableSaveHits){
      vtxNtuVertex = new TAVTntuVertex();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(vtxNtuVertex->ClassName()), &vtxNtuVertex);
      vtxNtuHit = new TAVTntuHit();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(vtxNtuHit->ClassName()), &vtxNtuHit);
    }
    if(runinfo->GetGlobalPar().EnableTracking){    
      vtxNtuTrack = new TAVTntuTrack();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(vtxNtuTrack->ClassName()), &vtxNtuTrack);
    }
    Int_t sensorsN = vtparGeo->GetSensorsN();
    vtxNtuClus = new TAVTntuCluster(sensorsN);
    tree->SetBranchAddress(TAGnameManager::GetBranchName(vtxNtuClus->ClassName()), &vtxNtuClus);
    if(IncludeMC){
      vtNtuMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kVTX), &vtNtuMc);
    }
  }

  if(IncludeIT){
    if(runinfo->GetGlobalPar().EnableSaveHits){
      itNtuClus = new TAITntuCluster(sensorsNit);
      tree->SetBranchAddress(TAGnameManager::GetBranchName(itNtuClus->ClassName()), &itNtuClus);
    }
    if(runinfo->GetGlobalPar().EnableSaveHits){
      itNtuTrack= new TAITntuTrack();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(itNtuTrack->ClassName()), &itNtuTrack);
    }
    if(IncludeMC){
      itNtuMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kITR), &itNtuMc);
    }
  }

  if(IncludeMSD){
    msdNtuClus= new TAMSDntuCluster(sensorsNms);
    tree->SetBranchAddress(TAGnameManager::GetBranchName(msdNtuClus->ClassName()), &msdNtuClus);
    if(runinfo->GetGlobalPar().EnableSaveHits){
      msdNtuHit= new TAMSDntuHit(sensorsNms);
      tree->SetBranchAddress(TAGnameManager::GetBranchName(msdNtuHit->ClassName()), &msdNtuHit);
    }
    if(runinfo->GetGlobalPar().EnableTracking){    
      msdNtuTrack= new TAMSDntuTrack();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(msdNtuTrack->ClassName()), &msdNtuTrack);
    }
    msdNtuPoint= new TAMSDntuPoint(stationsNms);
    tree->SetBranchAddress(TAGnameManager::GetBranchName(msdNtuPoint->ClassName()), &msdNtuPoint);
    if(IncludeMC){
      msdNtuMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kMSD), &msdNtuMc);
    }    
  }

  if(IncludeTW){
    twNtuPoint = new TATWntuPoint();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(twNtuPoint->ClassName()), &twNtuPoint);
    if(IncludeMC){
      twNtuMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kTW), &twNtuMc);
    }
    if(IncludeDAQ){
      twNtuRaw=new TATWntuRaw();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(twNtuRaw->ClassName()), &twNtuRaw);
    }
  }

  if(IncludeCA){
    if(runinfo->GetGlobalPar().EnableSaveHits){    
      caNtuHit = new TACAntuHit();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(caNtuHit->ClassName()), &caNtuHit);
    }    
    caNtuClus = new TACAntuCluster();
    tree->SetBranchAddress(TAGnameManager::GetBranchName(caNtuClus->ClassName()), &caNtuClus);    
    if(IncludeMC){
      caNtuMc = new TAMCntuHit();
      tree->SetBranchAddress(FootBranchMcName(kCAL), &caNtuMc);      
    }
    if(IncludeDAQ){
      caNtuRaw = new TACAntuRaw();
      tree->SetBranchAddress(TAGnameManager::GetBranchName(caNtuRaw->ClassName()), &caNtuRaw);    
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
    tree->SetBranchAddress(TAGnameManager::GetBranchName(tgevent->ClassName()), &tgevent);
  }

  if(IncludeGLB){
    glbNtuTrack = new TAGntuGlbTrack();
    tree->SetBranchAddress(TAGntuGlbTrack::GetBranchName(), &glbNtuTrack);
  }

  if (nentries == 0 || nentries>tree->GetEntries())
    maxentries = tree->GetEntries();
  else
    maxentries=nentries;

  return;
}
