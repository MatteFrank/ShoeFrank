#include "TROOT.h"
#include "BM_MSD_strel_func.cpp"

using namespace std;
using namespace TMath;

void BM_MSD_strel_main(TString in_filename = "", Int_t nentries = 0){

  gROOT->SetBatch(kTRUE);//turn off graphical output on screen
  TFile *infile = new TFile(in_filename.Data(),"READ");
  if(!infile->IsOpen()){
    cout<<"I cannot open "<<in_filename.Data()<<endl;
    return;
  }
  TTree *tree = (TTree*)infile->Get("tree");
  if(tree==nullptr){
    cout<<"FATAL ERROR: your input file do not contains a tree, check it!  input file="<<in_filename.Data()<<endl;
    return;
  }
  TString out_filename=in_filename(in_filename.Last('/')+1, in_filename.Last('.'));
  out_filename.Prepend("strelcal_");
  out_filename.Append("_myreadbmmsdraw_Out.root");
  TFile *f_out = new TFile(out_filename.Data(),"RECREATE");

  //define and charge varaibles and geometrical parameters
  TAGroot gTAGroot;
  TAGrunInfo* runinfo=(TAGrunInfo*)(infile->Get("runinfo"));
  const TAGrunInfo construninfo(*runinfo);
  gTAGroot.SetRunInfo(construninfo);
  TString expName=runinfo->CampaignName();
  if(expName.EndsWith("/")) //fix a bug present in shoe
    expName.Remove(expName.Length()-1);
  Int_t runNumber=runinfo->RunNumber();
  TAGrecoManager::Instance(expName);
  TAGcampaignManager* campManager = new TAGcampaignManager(expName);
  campManager->FromFile();
  geoTrafo = new TAGgeoTrafo();
  TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
  geoTrafo->FromFile(parFileName);

  TAGparaDsc* bmGeo    = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
  TABMparGeo* bmpargeo   = (TABMparGeo*) bmGeo->Object();
  parFileName = campManager->GetCurGeoFile(TABMparGeo::GetBaseName(), runNumber);
  bmpargeo->FromFile(parFileName.Data());

  // TAGparaDsc*  bmCal  = new TAGparaDsc("bmCal", new TABMparCal());
  // TABMparCal* bmparcal = (TABMparCal*)bmCal->Object();
  // parFileName = campManager->GetCurCalFile(TABMparGeo::GetBaseName(), runNumber);
  // bmparCal->FromFile(parFileName.Data());

  // TAGparaDsc*  bmMap  = new TAGparaDsc("bmMap", new TABMparMap());
  // TABMparMap*  bmparmap = (TABMparMap*)bmMap->Object();
  // parFileName = campManager->GetCurMapFile(TABMparGeo::GetBaseName(), runNumber);
  // bmparMap->FromFile(parFileName.Data(), bmpargeo);

  TAGparaDsc* msdGeo    = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
  TAMSDparGeo* msdpargeo   = (TAMSDparGeo*) msdGeo->Object();
  parFileName = campManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), runNumber);
  msdpargeo->FromFile(parFileName.Data());

  bmNtuHit = new TABMntuHit();
  tree->SetBranchAddress(TABMntuHit::GetBranchName(), &bmNtuHit);
  TBranch *bmBraNtuHit=tree->GetBranch(TABMntuHit::GetBranchName());
  bmNtuTrack = new TABMntuTrack();
  tree->SetBranchAddress(TABMntuTrack::GetBranchName(), &bmNtuTrack);
  TBranch *bmBraNtuTrack=tree->GetBranch(TABMntuTrack::GetBranchName());
  msdNtuCluster = new TAMSDntuCluster();
  tree->SetBranchAddress(TAMSDntuCluster::GetBranchName(), &msdNtuCluster);
  TBranch *msdBraNtuTrackN=tree->GetBranch(TAMSDntuCluster::GetBranchName());

  msdNtuPoint = new TAMSDntuPoint();
  tree->SetBranchAddress(TAMSDntuPoint::GetBranchName(), &msdNtuPoint);
  TBranch *msdBraNtuPointN=tree->GetBranch(TAMSDntuPoint::GetBranchName());

  BookingBMVTX(f_out);
  //used for alignment:
  vector<TVector3> vtxslopevec;
  vector<TVector3> vtxoriginvec;
  vector<TVector3> bmslopevec;
  vector<TVector3> bmoriginvec;
  char hname[200];
  //****************************************** Event Loop ****************************************

  //read BM and Vertex loop
  if (nentries == 0 || nentries>tree->GetEntries())
    nentries = tree->GetEntries();
  for (evnum = 0; evnum < nentries; ++evnum) {
    if(debug)
      cout<<"processing evnum="<<evnum<<endl;

    bmBraNtuHit->GetEntry(evnum);
    bmBraNtuTrack->GetEntry(evnum);
    msdBraNtuTrackN->GetEntry(evnum);
    msdBraNtuPointN->GetEntry(evnum);

    FillSeparated();

    for(int iL=0; iL<3; iL++) {
      if(msdNtuPoint->GetPointN(iL)) {

	for(int iPoi=0; iPoi<msdNtuPoint->GetPointN(iL); iPoi++) {
	  TAMSDpoint* aPoi = msdNtuPoint->GetPoint(iL,iPoi);
	  sprintf(hname,"Sensor_%d",iL);
	  ((TH2D*)gDirectory->Get(hname))->Fill(aPoi->GetPosition().X(),aPoi->GetPosition().Y());
	}
	
      }
    }
    
    
    if(msdNtuCluster->GetClustersN(msdplane) == 1 && bmNtuTrack->GetTracksN()==1){
      TABMtrack* bmtrack = bmNtuTrack->GetTrack(0);
      TAMSDcluster* msdclus=msdNtuCluster->GetCluster(msdplane,0);
      if(bmtrack->GetChiSquare()<10 ){
        ((TH1D*)gDirectory->Get("combinedStatus"))->Fill(0);
        FillCombined(bmtrack, msdclus);
      }else{
        if(bmtrack->GetChiSquare()>=10)
          ((TH1D*)gDirectory->Get("combinedStatus"))->Fill(6);
      }
    }else{
      if(msdNtuCluster->GetClustersN(msdplane)!=1)
        ((TH1D*)gDirectory->Get("combinedStatus"))->Fill((msdNtuCluster->GetClustersN(msdplane)>1) ? 1:2);
      if(bmNtuTrack->GetTracksN()!=1)
        ((TH1D*)gDirectory->Get("combinedStatus"))->Fill((bmNtuTrack->GetTracksN()>1) ? 3:4);

    }

  }

  if(debug)
    cout<<"End of event loop, total number of events read="<<evnum<<endl;

  //****************************************************** end of program, print! *********************************
  PostLoopAnalysis();

  f_out->Write();
  f_out->Close();

  cout<<"End of program, output file="<<out_filename.Data()<<endl;

  return ;
}
