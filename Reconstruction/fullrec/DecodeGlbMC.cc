#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGrecoManager.hxx"
#include "GlobalRecoMC.hxx"

int main (int argc, char *argv[])  {

   TString in("");
   TString out("");
   TString exp("");

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   Int_t skipEv = 0;
   
   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
      if(strcmp(argv[i],"-skipEv") == 0)   { skipEv = atoi(argv[++i]); }   // Number of events to be skipped
      
      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" Decoder help:"<<endl;
         cout<<" Ex: Decoder [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] raw input file"<<endl;
         cout<<"      -out path/file : [def=*_Out.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
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
   // TAGrecoManager::GetPar()->Print();
   
   Bool_t lrc = TAGrecoManager::GetPar()->IsLocalReco();
   Bool_t ntu = TAGrecoManager::GetPar()->IsSaveTree();
   Bool_t his = TAGrecoManager::GetPar()->IsSaveHisto();
   Bool_t hit = TAGrecoManager::GetPar()->IsSaveHits();
   Bool_t trk = TAGrecoManager::GetPar()->IsTracking();
   Bool_t obj = TAGrecoManager::GetPar()->IsReadRootObj();

   Bool_t zmc = TAGrecoManager::GetPar()->IsTWZmc();
   Bool_t zrec = TAGrecoManager::GetPar()->IsTWnoPU();
   Bool_t zmatch = TAGrecoManager::GetPar()->IsTWZmatch();
   Bool_t tbc = TAGrecoManager::GetPar()->IsTWCalBar();
   
   // TAGrecoManager::GetPar()->IncludeTOE(false);
   // TAGrecoManager::GetPar()->IncludeKalman(true);
   
   GlobalRecoMC* glbRec = new GlobalRecoMC(exp, runNb, in, out);

   // global setting
   if (ntu)
      glbRec->EnableTree();
   if(his)
      glbRec->EnableHisto();
   if(hit) {
      glbRec->EnableTree();
      glbRec->EnableSaveHits();
   }
   if (trk) {
      glbRec->EnableTracking();
   }

   
   TStopwatch watch;
   watch.Start();
   
   glbRec->BeforeEventLoop();
   glbRec->LoopEvent(nTotEv, skipEv);
   glbRec->AfterEventLoop();
   
   watch.Print();

   
   return 0;
} 















