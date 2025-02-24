#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>
#include <TSystem.h>

#include "TAGrecoManager.hxx"
#include "RecoRaw.hxx"
#include "RecoMC.hxx"
#include "GlobalReco.hxx"
#include "GlobalToeReco.hxx"

int main (int argc, char *argv[])  {

   TString in("");
   TString inMc("");
   TString out("");
   TString exp("");
   
   Bool_t mc  = false;
   Bool_t mth = false;
   Bool_t IsSubFile = false;   
   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   Int_t nSkipEv = 0;
   
   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-nsk") == 0)   { nSkipEv = atoi(argv[++i]); }  // Number of events to be skip
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
      if(strcmp(argv[i],"-inmc") == 0)  { inMc = TString(argv[++i]);   } // MC file name
      if(strcmp(argv[i],"-subfile") == 0)   { IsSubFile = true;   } // Disable the processing of the chain of all the sub file related to a given run: only the subfile related to the input file is processed
      if(strcmp(argv[i],"-mc") == 0)    { mc = true;    } // reco from MC local reco data
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
         cout<<"      -mc            : reco from MC local reco tree"<<endl;
         cout<<"      -inmc          : MC file name  "<<endl;
         cout<<"      -subfile       : [def=false] when true disable the processing of the chain of all the sub file related to a given run: only the subfile related to the input file is processed"<<endl;         
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

	Bool_t lrc = TAGrecoManager::GetPar()->IsFromLocalReco();
	
	BaseReco* glbRec = 0x0;

	// check input file exists 
   if(in.IsNull() || gSystem->AccessPathName(in.Data()))
   {
      Error("main()", "Input file does not exist or is null");
      exit(-1);
   }
   
   if (!lrc) {
     if (mc) 
       glbRec = new RecoMC(exp, runNb, in, out);
     else
       glbRec = new RecoRaw(exp, runNb, in, out, IsSubFile);
   } else {
     if (toe)
       glbRec = new GlobalToeReco(exp, runNb, in, out, mc, inMc);
     else if (gf)
       glbRec = new GlobalReco(exp, runNb, in, out, mc);
     else
       Error("main()", "Running on decoded files but no Global reconstruction algorithm (TOE or Genfit) chosen! Enable them from the FootGlobal.par file"), exit(0);
   }
   
   // global setting
   if (mth)
      glbRec->EnableM28lusMT();
   
   TStopwatch watch;
   watch.Start();
   
   if (mc){
      glbRec->BeforeEventLoop();
      if(nSkipEv > 0) glbRec->GoEvent(nSkipEv);
   } else {
      if(nSkipEv > 0) glbRec->GoEvent(nSkipEv);       
      glbRec->BeforeEventLoop();
   }
   
   
   
   glbRec->LoopEvent(nTotEv);
   glbRec->AfterEventLoop();
   
   watch.Print();

   
   return 0;
} 















