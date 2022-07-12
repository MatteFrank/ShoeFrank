#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <TApplication.h>

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TSpline.h>
#include <TVector3.h>
#include <TVectorD.h>
#include <TKey.h>
#include <TMath.h>

#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include <TStopwatch.h>
#include <TApplication.h>
#include "TAGaction.hxx"
#include "TAGactTreeReader.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeReader.hxx"
#include "TAGntuGlbTrack.hxx"

#include "GlobalRecoAna.hxx"

using namespace std;

int main(int argc, char *argv[]) {

   TString in("");
   TString out("");
   TString exp("");

   Bool_t mc  = false;
   Bool_t mth = false;

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;

   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number

      if(strcmp(argv[i],"-mc") == 0)    { mc = true;    } // reco from MC local reco data
      if(strcmp(argv[i],"-mth") == 0)   { mth = true;   } // enable multi threading (for clustering)

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


  TApplication::CreateApplication();
  TAGrecoManager::Instance(exp);
  TAGrecoManager::GetPar()->FromFile();
  TAGrecoManager::GetPar()->Print();
  TStopwatch watch;
  watch.Start();

  GlobalRecoAna* glbAna = new GlobalRecoAna(exp, runNb, in, out, mc, nTotEv);
  glbAna->BeforeEventLoop();
  glbAna->LoopEvent();
  glbAna->AfterEventLoop();
  watch.Print();

  cout<<"DecodeGlbAna finished, output file="<<out.Data()<<endl;

  return 0;

}
