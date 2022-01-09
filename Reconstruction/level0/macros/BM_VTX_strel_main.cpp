#include "BM_VTX_strel_func.cpp"

using namespace std;
using namespace TMath;

void BM_VTX_strel_main(TString in_filename = "", Int_t nentries = 0, Int_t doalign=1, TString addoutname=""){

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
  out_filename.Append("_myreadbmvtxraw_Out");
  out_filename.Append(addoutname);
  out_filename.Append(".root");
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

  TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparConf());
  bmparconf = (TABMparConf*)bmConf->Object();
  parFileName = campManager->GetCurConfFile(TABMparGeo::GetBaseName(), runNumber);
  bmparconf->FromFile(parFileName.Data());

  // TAGparaDsc*  bmCal  = new TAGparaDsc("bmCal", new TABMparCal());
  // TABMparCal* bmparcal = (TABMparCal*)bmCal->Object();
  // parFileName = campManager->GetCurCalFile(TABMparGeo::GetBaseName(), runNumber);
  // bmparCal->FromFile(parFileName.Data());

  // TAGparaDsc*  bmMap  = new TAGparaDsc("bmMap", new TABMparMap());
  // TABMparMap*  bmparmap = (TABMparMap*)bmMap->Object();
  // parFileName = campManager->GetCurMapFile(TABMparGeo::GetBaseName(), runNumber);
  // bmparMap->FromFile(parFileName.Data(), bmpargeo);

  bmNtuHit = new TABMntuHit();
  tree->SetBranchAddress(TABMntuHit::GetBranchName(), &bmNtuHit);
  TBranch *bmBraNtuHit=tree->GetBranch(TABMntuHit::GetBranchName());
  bmNtuTrack = new TABMntuTrack();
  tree->SetBranchAddress(TABMntuTrack::GetBranchName(), &bmNtuTrack);
  TBranch *bmBraNtuTrack=tree->GetBranch(TABMntuTrack::GetBranchName());
  vtNtuTrack = new TAVTntuTrack();
  tree->SetBranchAddress(TAVTntuTrack::GetBranchName(), &vtNtuTrack);
  TBranch *vtBraNtuTrack=tree->GetBranch(TAVTntuTrack::GetBranchName());

  //used for alignment:
  vector<TVector3> vtxslopevec;
  vector<TVector3> vtxoriginvec;
  vector<TVector3> bmslopevec;
  vector<TVector3> bmoriginvec;

  //check the bm sense wire position
  Int_t ilay, iview, icell;
  // for(Int_t i=0;i<36;i++){
  //   bmpargeo->GetBMNlvc(i,ilay, iview, icell);
  //   cout<<"cellid="<<i<<" ilay="<<ilay<<"  iview="<<iview<<"  icell="<<icell<<"  GetWireX="<<bmpargeo->GetWireX(bmpargeo->GetSenseId(icell),ilay, iview)<<"  GetwireY="<<bmpargeo->GetWireY(bmpargeo->GetSenseId(icell),ilay, iview)<<"  getwireZ="<<bmpargeo->GetWireZ(bmpargeo->GetSenseId(icell),ilay, iview)<<"  getwirecx="<<bmpargeo->GetWireCX(bmpargeo->GetSenseId(icell),ilay, iview)<<"  getwirecy="<<bmpargeo->GetWireCY(bmpargeo->GetSenseId(icell),ilay, iview)<<"  getwirecz="<<bmpargeo->GetWireCZ(bmpargeo->GetSenseId(icell),ilay, iview)<<endl;
  // }

  //****************************************** Event Loop ****************************************

  //read BM and Vertex loop
  if (nentries == 0 || nentries>tree->GetEntries())
    maxentries = tree->GetEntries();
  else
    maxentries=nentries;

  BookingBMVTX(f_out, doalign);

  for (evnum = 0; evnum < maxentries; ++evnum) {
    if(debug)
      cout<<"processing evnum="<<evnum<<endl;

    bmBraNtuHit->GetEntry(evnum);
    bmBraNtuTrack->GetEntry(evnum);
    vtBraNtuTrack->GetEntry(evnum);

    FillSeparated(); //here I inverted the vtx SlopeZ.Y and origin.Y

    // if(vtNtuTrack->GetTracksN() == 1 && bmNtuTrack->GetTracksN()==1){
    if(vtNtuTrack->GetTracksN() >0 && bmNtuTrack->GetTracksN()==1){
      FillCombined(vtxslopevec, vtxoriginvec, bmslopevec, bmoriginvec, bmpargeo);
    }else{
      if(vtNtuTrack->GetTracksN()!=1)
        ((TH1D*)gDirectory->Get("combinedStatus"))->Fill((vtNtuTrack->GetTracksN()>1) ? 1:2);
      if(bmNtuTrack->GetTracksN()!=1)
        ((TH1D*)gDirectory->Get("combinedStatus"))->Fill((bmNtuTrack->GetTracksN()>1) ? 3:4);
    }
  }

  cout<<"End of event loop, total number of events read="<<evnum<<endl;

  //****************************************************** end of program, print! *********************************
  PostLoopAnalysis();
  if(doalign)
    Allign_estimate(vtxslopevec, vtxoriginvec, bmslopevec, bmoriginvec, doalign);

  f_out->Write();
  f_out->Close();

  cout<<"End of program, output file="<<out_filename.Data()<<endl;

  return ;
}
