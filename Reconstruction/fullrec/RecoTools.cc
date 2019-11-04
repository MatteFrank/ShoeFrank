#define RecoTools_cxx

#include "RecoTools.h"



using namespace std;




//----------------------------------------------------------------------------------------------------
RecoTools::RecoTools(int d, TString istr, bool list, TString ostr, TString wd, int nev, TFile *hf) {

  cout << "\tstart Constructor RecoTools\n";
  
  my_files.clear();
  m_debug = GlobalPar::GetPar()->Debug();
  
  m_oustr = ostr;
  m_wd = wd;
  
  m_tempo_kal=0;
  
  // take input file list
  ifstream inS;
  char bufConf[200]; char fname[400];
  if(!list) {
    m_instr = istr;
    my_files.push_back(m_instr.Data());
  } else {
    m_instr = istr;
    inS.open(m_instr.Data());
    while (inS.getline(bufConf, 200, '\n')) {
      sscanf(bufConf,"%s",fname);
      my_files.push_back(fname);
    }
  }
  
  m_nev = nev;
  gErrorIgnoreLevel = kError;
  
  m_hf = hf;
  cout << "\tend Constructor RecoTools\n";
}






//----------------------------------------------------------------------------------------------------
void RecoTools::RecoLoop(int fr) {

  // input ntuple tree
  TChain *tree = new TChain("EventTree");
  for(unsigned int ifi=0; ifi<my_files.size(); ifi++) {
    tree->Add(my_files.at(ifi).data());
    cout<<"Adding :: "<<my_files.at(ifi).data() << " file"<<endl;
  }
  
  // create the event object
  if(m_debug) cout<<" Creating Eve "<<endl;
  Evento *ev =  new Evento();
  EVENT_STRUCT evStr;
  TAGpadGroup* pg = new TAGpadGroup();
  
  ev->FindBranches(tree,&evStr);
  
  if(m_debug) cout<<" Found branches "<<endl;
  
  //Configure the output flagging
  gTAGroot->SetCampaignNumber(100);
  gTAGroot->SetRunNumber(1);
  
  //Define the output file content.
  // my_out = new TAGactTreeWriter("my_out");
  // gTAGroot->AddRequiredItem("my_out");
  // gTAGroot->Print();
  // if (my_out->Open(m_oustr, "RECREATE")) return;
  
  Booter* booter = new Booter();
  booter->Initialize( &evStr );
  //
  //    
  //    MultiTrackCheck* multiTrackCheck = new MultiTrackCheck();
  //    multiTrackCheck->Initialize( &evStr );
  
  // gTAGroot->AddRequiredItem("itRaw");
  // gTAGroot->AddRequiredItem("myn_mceve");
  // gTAGroot->AddRequiredItem("an_mceve");
  // gTAGroot->Print();
  
  
  /***********  The event Loop   ****************************************   */
  gTAGroot->BeginEventLoop();
  Long64_t nentries = tree->GetEntries();
  Long64_t nbytes = 0, nb = 0;
  char flag[200]; bool tobedrawn = kFALSE;
  if(m_nev != 0)      nentries = m_nev;
  
  if(m_debug)         cout<<" Starting Event Loop "<<endl;
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    if(m_debug) cout<<" New Eve "<<endl;
    nb = tree->GetEntry(jentry);   nbytes += nb;
    // if (Cut(ientry) < 0) continue;
    // if (jentry>1)  break;
    // if (jentry<33061)  continue;
    
    gTAGroot->NextEvent();
    if(!(jentry%fr))        cout<<"Processed:: "<<jentry<<" evts!"<<endl;
    
    ///////////////  Call here your Process() functions    /////////////////////////////////////////////
    
    
    booter->Process( jentry );
    //	multiTrackCheck->Process( jentry );
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    if (!pg->IsEmpty() && tobedrawn && !(jentry%fr)) {
      pg->Modified();
      pg->Update();
      
      sprintf(flag,"plots/%s_%d","Test_MC",(int)jentry);
      pg->Print(flag);
    }
    
    
    if(m_debug) cout<<" Loaded Event:: "<<jentry<<endl;
    
    if ( GlobalPar::GetPar()->Debug() > 0 )         cout << "End event n: " << jentry << endl;
    
  }
  cout << "End of the event loop " << endl;
  
  booter->Finalize();
  //    multiTrackCheck->Finalize();
  
  gTAGroot->EndEventLoop();
  
  // my_out->Print();
  // my_out->Close();
  
  // materialEffects->drawdEdx( 11 );  // to look at it in genfit
  
  return;
}









