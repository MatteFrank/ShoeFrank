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

  // build up the analysis chain
  if (TAGrecoManager::GetPar()->IncludeBM()) {
      methods.Register(new TANLBMAnalysis());
      if (TAGrecoManager::GetPar()->IncludeVT()) {
          methods.Register(new TANLBMVTmatchAnalysis());
      } 
  }

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

  file.close();

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
