#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>
#include <TSystem.h>

#include "TAGrecoManager.hxx"
#include "ConvertNtuple.hxx"

// executable to convert L0 tree into a flat Ntuple
// author: Ch. Finck


int main (int argc, char *argv[])  {

   TString in("");
   TString inMc("");
   TString out("");
   TString exp("");

   Bool_t mc  = false;

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   Int_t nSkipEv = 0;

   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);   }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);   }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]);  }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]);  }   // Number of events to be analized
      if(strcmp(argv[i],"-nsk") == 0)   { nSkipEv = atoi(argv[++i]); }   // Number of events to be skip
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);   }   // Run Number
      if(strcmp(argv[i],"-inmc") == 0)  { inMc = TString(argv[++i]); }   // MC file name

      if(strcmp(argv[i],"-mc") == 0)    { mc = true;    } // reco from MC local reco data

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
         cout<<"      -mc            : reco from MC local reco tree "<<endl;
         cout<<"      -inmc          : MC file name  "<<endl;
         return 1;
      }
   }
   
   TApplication::CreateApplication();
   
   TAGrecoManager::Instance(exp);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();

   if(in.IsNull() || gSystem->AccessPathName(in.Data()))
   {
      Error("main()", "Input file does not exist or is null");
      exit(-1);
   }
   
   ConvertNtuple* conv = new ConvertNtuple(exp, runNb, in, out, mc, inMc);
   
   TStopwatch watch;
   watch.Start();
    
    conv->BeforeEventLoop();
   
   if (nSkipEv > 0)
      conv->GoEvent(nSkipEv);
   
   conv->LoopEvent(nTotEv);
   conv->AfterEventLoop();
   
   watch.Print();

   delete conv;
   
   return 0;
} 















