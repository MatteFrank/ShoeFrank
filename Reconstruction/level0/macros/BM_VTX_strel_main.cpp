#include "BM_VTX_strel_main.h"
#include "BM_VTX_strel_func.cpp"

using namespace std;
using namespace TMath;

void BM_VTX_strel_main(Int_t nentries=0){
  gROOT->SetBatch(kTRUE);//turn off graphical output on screen
  TString out_filename("Outstrel_mcvertex.root");
  TString in_filename("footC200_C2H4_bmvtxtree.root");
    
  TFile *f_out = new TFile(out_filename.Data(),"RECREATE");  
  TFile *infile = new TFile(in_filename.Data(),"READ");  
  if(!infile->IsOpen()){
    cout<<"I cannot open "<<in_filename.Data()<<endl; 
    return;
  }
  TAGroot gTAGroot;
  
  cout<<"Input file="<<in_filename.Data()<<endl;
  cout<<"output file="<<out_filename.Data()<<endl;

  TTree *tree = (TTree*)infile->Get("tree");  

  TAGparaDsc* tgGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
  TAGparGeo* parGeo = (TAGparGeo*)tgGeo->Object();
  parGeo->FromFile();
  
  TAGparaDsc* bmGeoDsc    = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
  TABMparGeo* bmgeo   = (TABMparGeo*) bmGeoDsc->Object();
  bmgeo->FromFile();

  TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparConf());
  TABMparConf* bmcon = (TABMparConf*)bmConf->Object();
  TString parFileName = "./config/beammonitor.cfg";
  bmcon->FromFile(parFileName.Data());
  parFileName = "./config/bmreso_vs_r.root";

  TAGparaDsc*  bmMap  = new TAGparaDsc("bmMap", new TABMparMap());
  TABMparMap*  parMap = (TABMparMap*)bmMap->Object();
  parFileName = "./geomaps/";
  parFileName += bmcon->GetParmapfile();
  parMap->FromFile(parFileName.Data(), bmgeo);
    
  TABMntuRaw*  bmRawHit = new TABMntuRaw(); 
  tree->SetBranchAddress(TABMntuRaw::GetBranchName(), &bmRawHit);    
  TABMntuTrack*  bmNtuTrack = new TABMntuTrack();
  tree->SetBranchAddress(TABMntuTrack::GetBranchName(), &bmNtuTrack);

  TAVTntuTrack* vttrack = new TAVTntuTrack();
  tree->SetBranchAddress(TAVTntuTrack::GetBranchName(), &vttrack);
  //~ TAGdataDsc* track = new TAGdataDsc("vttrack", vttrack);  

  BookingBMVTX(f_out, false, false);

  //******************************************BM stuff****************************************
  
  //read event bm eventtree stuff

  BM_evstruct bmevent;
  vector<BM_evstruct> allbmeventin;
  clean_bmevstruct(bmevent, true);
  
  vtx_evstruct vtxevent;
  vector<vtx_evstruct> allvtxeventin;
  clean_vtxevstruct(vtxevent);

  //read BM and Vertex loop
  if (nentries == 0)
   nentries = tree->GetEntries();
  for (Int_t ev = 0; ev < nentries; ++ev) {
    bmNtuTrack->Clear();
    vttrack->Clear();
    cout<<"provo a fare getentry"<<endl;
    tree->GetEntry(ev);    
    cout<<" fatto getentry"<<endl;
    if(vttrack->GetTracksN() == 1){
      TAVTtrack* track = vttrack->GetTrack(0);
      bmreadevent(bmgeo, bmRawHit, bmNtuTrack, bmevent, allbmeventin);
      clean_vtxevstruct(vtxevent);
      
    }
  } 
  
  cout<<"number of events read="<<allbmeventin.size()<<endl;

  if(allbmeventin.size()!=allvtxeventin.size()){
    cout<<"ERROR::all*eventsize is wrong: allbmeventin.size()="<<allbmeventin.size()<<"  allvtxeventin.size()="<<allvtxeventin.size()<<endl;
    return ;
  }
  //****************************************************** evaluate strel *************************************
  vector<vector<double>> space_residual(STBIN+1);
  vector<vector<double>> time_residual(STBIN+1);
  vector<vector<int>> selected_index;//matrix with the two index in allbmeventin and allvtxeventin that are correlated
  vector<int> event_index(3,-1);//0=allbmeventin index, 1=allvtxeventin index, 2=combine code(0=bm && msd, 1=only msd1, 2=only msd2)
  int last_vtxindex=0;
  for(int i=0;i<allbmeventin.size();i++){
    //~ cout<<"i="<<i<<"  allbmeventin.at(i).evnum="<<allbmeventin.at(i).evnum<<"  k="<<k<<endl;
    if(allbmeventin.at(i).bm_track_chi2<BMNCUT && (allbmeventin.at(i).status==0 || allbmeventin.at(i).status==5)){
      //~ cout<<"trovato traccia con stesso evnum: l="<<l<<"  allvtxeventin.at(l).evnum="<<allvtxeventin.at(l).evnum<<endl;
      if(allvtxeventin.at(i).vtx_track_chi2tot<1. && allvtxeventin.at(i).vtx_track_chi2uview<1. && allvtxeventin.at(i).vtx_track_chi2vview<1.){//there's a good track fitted both by msd and bm
        event_index.at(0)=i;
        event_index.at(1)=i;
        event_index.at(2)=0;
        selected_index.push_back(event_index);
        EvaluateSpaceResidual(space_residual, time_residual, allbmeventin.at(i), allvtxeventin.at(i), bmgeo);        
        //~ ((TH1D*)gDirectory->Get("time_diff"))->Fill(allbmeventin.at(i).timeacq-allvtxeventin.at(k).timeacq);
      }else if(allvtxeventin.at(i).vtx_track_chi2uview>1.){
        event_index.at(0)=i;
        event_index.at(1)=i;
        event_index.at(2)=1;
        selected_index.push_back(event_index);
      }else if (allvtxeventin.at(i).vtx_track_chi2vview>1.){
        event_index.at(0)=i;
        event_index.at(1)=i;
        event_index.at(2)=2;
        selected_index.push_back(event_index);
      }
    }
  }
  
  cout<<"number of combined events="<<selected_index.size()<<endl;
  
  //****************************************************** end of program, print! *********************************
  Printoutput(f_out, allbmeventin, allvtxeventin, space_residual, time_residual, selected_index, false);
  //~ fitPositionResidual();
  Allign_estimate();
  
  f_out->Write();
  //~ f_out->Close();
  gROOT->SetBatch(kFALSE);//enable graphical output  
  new TBrowser();
  cout<<"End of program"<<endl;
  
  return ;
}



