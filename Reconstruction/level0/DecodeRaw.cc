#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "GlobalPar.hxx"
#include "LocalRecoMC.h"

int main (int argc, char *argv[])  {

	
   TString in("12C_400_vtx.root");
   
   Int_t pos = in.Last('.');
   TString out = in(0, pos);
   out.Append("_Out.root");
   
   Bool_t ntu = false;
   Bool_t his = false;
   Bool_t hit = false;
   Bool_t trk = false;
   
   Int_t nTotEv = 1e7;
   
   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Root file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
  
      
      if(strcmp(argv[i],"-ntu") == 0)   { ntu = true;   } // enable tree filling
      if(strcmp(argv[i],"-his") == 0)   { his = true;   } // enable histograming
      if(strcmp(argv[i],"-hit") == 0)   { hit = true;   } // enable hits saving
      if(strcmp(argv[i],"-trk") == 0)   { trk = true;   } // enable tracking action
      
      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" Decoder help:"<<endl;
         cout<<" Ex: Decoder [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=16O_C2H4_200_1.root] ROOT input file"<<endl;
         cout<<"      -out path/file : [def=6O_C2H4_200_1_Out.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -trk           : enable tracking actions"<<endl;
         cout<<"      -hit           : enable saving hits in tree (activated ntu option)"<<endl;
         cout<<"      -ntu           : enable tree filling"<<endl;
         cout<<"      -his           : enable crtl histograming"<<endl;
         return 1;
      }
   }
   
   TApplication::CreateApplication();
   
   GlobalPar::Instance();
   GlobalPar::GetPar()->Print();
   
   LocalRecoMC* locRec = new LocalRecoMC(in, out);
   
   // global setting
   if (ntu)
      locRec->EnableTree();
   if(his)
      locRec->EnableHisto();
   if(hit) {
      locRec->EnableTree();
      locRec->EnableSaveHits();
   }
   if (trk) {
      locRec->EnableTracking();
   }

   
   TStopwatch watch;
   watch.Start();
   
   locRec->BeforeEventLoop();
   locRec->LoopEvent(nTotEv);
   locRec->AfterEventLoop();
   
   watch.Print();

   
   return 0;
} 















