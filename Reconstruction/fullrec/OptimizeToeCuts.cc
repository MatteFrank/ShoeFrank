#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGrecoManager.hxx"
#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "GlobalToeReco.hxx"

// executabel to read back from local reconstruction tree or from MC/raw data
// author: Ch. Finck
#include "TATOEcutter.hxx"
#include "TATOEutilities.hxx"


int main (int argc, char *argv[])  {

   TString in("");
   TString out("");
   TString exp("");

   Bool_t mc  = true;
   Bool_t mth = false;

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   
   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number

      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" Decoder help:"<<endl;
         cout<<" Ex: Decoder [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] raw input file"<<endl;
         cout<<"      -out path/file : [def=*_Out.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
         cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
         cout<<"      -mc            : reco from MC local reco tree"<<endl;
         cout<<"      -mth           : enable multi threading (for clustering)"<<endl;
         return 1;
      }
   }
   
   if (out.IsNull()) {
      Int_t pos = in.Last('.');
      out = in(0, pos);
      out.Append("_Out.root");
   }
   
   TApplication::CreateApplication();
   
   TAGrecoManager::Instance(exp);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   Bool_t lrc = TAGrecoManager::GetPar()->IsLocalReco();
   Bool_t ntu = TAGrecoManager::GetPar()->IsSaveTree();
   Bool_t his = TAGrecoManager::GetPar()->IsSaveHisto();
   Bool_t hit = TAGrecoManager::GetPar()->IsSaveHits();
   Bool_t trk = TAGrecoManager::GetPar()->IsTracking();
   Bool_t zmc = TAGrecoManager::GetPar()->IsTWZmc();
   Bool_t zrec = TAGrecoManager::GetPar()->IsTWnoPU();
   Bool_t zmatch = TAGrecoManager::GetPar()->IsTWZmatch();
   Bool_t tbc = TAGrecoManager::GetPar()->IsTWCalBar();

   TAGrecoManager::GetPar()->IncludeTOE(true);
   TAGrecoManager::GetPar()->IncludeKalman(false);

   BaseReco* glbRec = 0x0;
   
   glbRec = new GlobalToeReco(exp, runNb, in, out, mc);
   glbRec->EnableRecCutter();
    
    // global setting
    if (ntu)
        glbRec->EnableTree();
    if(his)
        glbRec->EnableHisto();
    if(hit) {
        glbRec->EnableTree();
        glbRec->EnableSaveHits();
    }
    if (trk)
        glbRec->EnableTracking();
    
    if (mth)
        glbRec->EnableM28lusMT();
    
    
   TStopwatch watch;
   watch.Start();
   
   glbRec->BeforeEventLoop();
    using config_t = details::configuration< details::range<3>, details::vertex_tag, details::it_tag, details::msd_tag, details::tof_tag >;
    glbRec->SetCutter(new TATOEcutter< config_t >{"toeActCutter"} );
    gTAGroot->AddRequiredItem("toeActCutter");
    for(auto i{0}; i < config_t::iteration_count ; ++i){
        glbRec->LoopEvent(nTotEv);
    }
   glbRec->AfterEventLoop();
   
   watch.Print();

   delete glbRec;
   
   return 0;
} 















