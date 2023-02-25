#include <stdlib.h>
#include <stdarg.h>
#include <TApplication.h>
#include "GlobalRecoMV.hxx"

using namespace std;

int main(int argc, char *argv[]) {

   TString in("");
   TString out("");
   TString exp("");

   Bool_t mc  = false;

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   Int_t nSkipEv = 0;
   bool  found = false;
   for (int i = 1; i < argc; i++){
     found = false;
     if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  found=true;}   // Raw file name for output
     if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  found=true;}   // Root file in input
     if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); found=true;}   // extention for config/geomap files
     if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); found=true;}   // Number of events to be analized
     if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  found=true;}   // Run Number
     if(strcmp(argv[i],"-skipEv") == 0)   { nSkipEv = atoi(argv[++i]); found=true;}  // Number of events to be skip
     if(strcmp(argv[i],"-mc") == 0)    { mc = true; found=true;   } // reco from MC local reco data
     
     if(strcmp(argv[i],"-help") == 0 || found==false)  {
       cout<<" "<<argv[0]<<" help:     \n"
	   <<" Ex: DecodeGlbMV [opts]  \n"
	   <<" possible opts are:      \n"
	   <<"      -in path/file  : [def=""] raw/root input file  \n"
	   <<"      -out path/file : [def=*_Out.root] Root output file  \n"
	   <<"      -nev value     : [def=10^7] Numbers of events to process  \n"
	   <<"      -run value     : [def=-1] Run number  \n"
	   <<"      -exp name      : [def=""] experient name for config/geomap extention  \n"
	   <<"      -skipEv nevts  : [def=0] number of events to skip  \n"
	   <<"      -mc            : reco from MC local reco tree  \n";       
       if( found==false ){
	 cout<<" option  "<<argv[i]<<"  not known!\n";
       }
       return 1;
     }
   }

   if(in.IsNull() || gSystem->AccessPathName(in.Data()))
   {
      Error("main()", "Input file does not exist or is null");
      exit(-1);
   }

  TApplication::CreateApplication();
  TAGrecoManager::Instance(exp);
  TAGrecoManager::GetPar()->FromFile();
  TAGrecoManager::GetPar()->Print();

  TStopwatch watch;
  watch.Start();

  GlobalRecoMV glbAna(exp, runNb, in, out, mc, nTotEv);
  if(nSkipEv > 0 )  glbAna.GoEvent(nSkipEv);

  glbAna.BeforeEventLoop(); 
  glbAna.LoopEvent();
  glbAna.AfterEventLoop();

  watch.Print();

  cout<<"DecodeGlbMV finished, output file="<<out.Data()<<endl;

  return 0;
}
