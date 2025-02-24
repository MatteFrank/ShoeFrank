#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>
#include <TSystem.h>

#include "TAGrecoManager.hxx"
#include "RecoRaw.hxx"

int main (int argc, char *argv[])  {

   TString in("");
   TString out("");
   TString exp("");
   
   Bool_t mth = false;

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   Int_t nSkipEv = 0;
   Int_t nFiles=1;
   Bool_t IsSubFile = false;   

   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-nsk") == 0)   { nSkipEv = atoi(argv[++i]); }  // Number of events to be skip
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
      if(strcmp(argv[i],"-nf") == 0)   { nFiles = atoi(argv[++i]);  }   // Run Number
      if(strcmp(argv[i],"-mth") == 0)   { mth = true;   } // enable multi threading (for clustering)
      if(strcmp(argv[i],"-subfile") == 0)   { IsSubFile = true;   } // Disable the processing of the chain of all the sub file related to a given run: only the subfile related to the input file is processed
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

   Bool_t toe = TAGrecoManager::GetPar()->IncludeTOE();
   Bool_t gf  = TAGrecoManager::GetPar()->IncludeKalman();


  
   if (out.IsNull()) {
      TAGrecoManager::GetPar()->DisableTree();
      TAGrecoManager::GetPar()->DisableHisto();
   }

   
   if(in.IsNull() || gSystem->AccessPathName(in.Data()))
   {
      Error("main()", "Input file does not exist or is null");
      exit(-1);
   }


   
      
   RecoRaw* glbRec = 0x0;
	
	// check input file exists 
   if(in.IsNull() || gSystem->AccessPathName(in.Data()))
   {
      Error("main()", "Input file does not exist or is null");
      exit(-1);
   }

  
   glbRec = new RecoRaw(exp, runNb, in, out, IsSubFile);
   glbRec->EnableStdAlone();   
   glbRec->SetStdAloneFiles(nFiles);
   glbRec->EnableM28lusMT();
   
   TStopwatch watch;
   watch.Start();
   
   
   if(nSkipEv > 0) glbRec->GoEvent(nSkipEv);       
   glbRec->BeforeEventLoop();
   glbRec->LoopEvent(nTotEv);
   glbRec->AfterEventLoop();
   
   watch.Print();

	
   // Bool_t ntu = TAGrecoManager::GetPar()->IsSaveTree();
   // Bool_t his = TAGrecoManager::GetPar()->IsSaveHisto();
   // Bool_t hit = TAGrecoManager::GetPar()->IsSaveHits();
   // Bool_t trk = TAGrecoManager::GetPar()->IsTracking();
   // Bool_t obj = TAGrecoManager::GetPar()->IsReadRootObj();
   
//   if (tbc && !out.IsNull()) {
//     Int_t pos = out.Last('.');
//     out = out(0, pos);
//     out.Append("_TWBarCalib.root");
//     cout<<out.Data()<<endl;
//   }
   
   // RecoRaw* locRec = new RecoRaw(exp, runNb, in, out);
   // locRec->EnableStdAlone();
   // locRec->SetStdAloneFiles(nFiles);
   
   // // global setting
   // if (ntu)
   //    locRec->EnableTree();
   
   // if(his)
   //    locRec->EnableHisto();
   
   // if(hit) {
   //    locRec->EnableTree();
   //    locRec->EnableSaveHits();
   // }
   
   // if (trk)
   //    locRec->EnableTracking();
   
   // if (mth)
   //    locRec->EnableM28lusMT();
   
   // if (nSkipEv > 0)
   //    locRec->GoEvent(nSkipEv);
   
   // TStopwatch watch;
   // watch.Start();
   
   // locRec->BeforeEventLoop();
   // locRec->LoopEvent(nTotEv);
   // locRec->AfterEventLoop();
   
   // watch.Print();

   // delete locRec;
   
   return 0;
} 















