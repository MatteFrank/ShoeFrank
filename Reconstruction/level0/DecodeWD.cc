#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGrecoManager.hxx"
#include "LocalReco.hxx"

int main (int argc, char *argv[])  {

   TString in("");
   TString out("");
   TString exp("");
   
   Bool_t mth = false;

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   Int_t nSkipEv = 0;
   Int_t nFiles=1;

   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-nsk") == 0)   { nSkipEv = atoi(argv[++i]); }  // Number of events to be skip
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
      if(strcmp(argv[i],"-nf") == 0)   { nFiles = atoi(argv[++i]);  }   // Run Number
      if(strcmp(argv[i],"-mth") == 0)   { mth = true;   } // enable multi threading (for clustering)

      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" Decoder help:"<<endl;
         cout<<" Ex: Decoder [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] raw input file"<<endl;
         cout<<"      -out path/file : [def=*_Out.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -nsk value     : [def=0] Skip number of events"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
         cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
         cout<<"      -mth           : enable multi threading (for clustering)"<<endl;
         return 1;
      }
   }
   
   TApplication::CreateApplication();
   
   TAGrecoManager::Instance(exp);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   if (out.IsNull()) {
      TAGrecoManager::GetPar()->DisableTree();
      TAGrecoManager::GetPar()->DisableHisto();
   }
   
   Bool_t ntu = TAGrecoManager::GetPar()->IsSaveTree();
   Bool_t his = TAGrecoManager::GetPar()->IsSaveHisto();
   Bool_t hit = TAGrecoManager::GetPar()->IsSaveHits();
   Bool_t trk = TAGrecoManager::GetPar()->IsTracking();
   Bool_t obj = TAGrecoManager::GetPar()->IsReadRootObj();
   Bool_t zmatch = TAGrecoManager::GetPar()->IsTWZmatch();
   Bool_t tbc = TAGrecoManager::GetPar()->IsTWCalBar();
   
   if (tbc && !out.IsNull()) {
     Int_t pos = out.Last('.');
     out = out(0, pos);
     out.Append("_TWBarCalib.root");
     cout<<out.Data()<<endl;
   }
   
   LocalReco* locRec = new LocalReco(exp, runNb, in, out);
   locRec->EnableStdAlone();
   locRec->SetStdAloneFiles(nFiles);
   
   // global setting
   if (ntu)
      locRec->EnableTree();
   
   if(his)
      locRec->EnableHisto();
   
   if(hit) {
      locRec->EnableTree();
      locRec->EnableSaveHits();
   }
   
   if (trk)
      locRec->EnableTracking();
   
   if(zmatch)
     locRec->EnableTWZmatch();
   
   if (tbc)
     locRec->EnableTWcalibPerBar();
   
   if (mth)
      locRec->EnableM28lusMT();
   
   if (nSkipEv > 0)
      locRec->GoEvent(nSkipEv);
   
   TStopwatch watch;
   watch.Start();
   
   locRec->BeforeEventLoop();
   locRec->LoopEvent(nTotEv);
   locRec->AfterEventLoop();
   
   watch.Print();

   delete locRec;
   
   return 0;
} 















