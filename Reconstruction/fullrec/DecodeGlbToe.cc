#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "GlobalPar.hxx"
#include "GlobalToeReco.hxx"

// executabel to read back from local reconstruction tree
// author: Ch. Finck


int main (int argc, char *argv[])  {

   TString in("");
   TString exp("");

   Int_t pos = in.Last('.');
   TString out = in(0, pos);
   out.Append("_Out.root");
   
   Bool_t ntu = true;
   Bool_t his = false;
   Bool_t hit = false;
   Bool_t trk = true;
   Bool_t mc  = false;

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   
   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
  
      if(strcmp(argv[i],"-ntu") == 0)   { ntu = true;   } // enable tree filling
      if(strcmp(argv[i],"-his") == 0)   { his = true;   } // enable histograming
      if(strcmp(argv[i],"-hit") == 0)   { hit = true;   } // enable hits saving
      if(strcmp(argv[i],"-trk") == 0)   { trk = true;   } // enable tracking action
      if(strcmp(argv[i],"-mc") == 0)    { mc = true;    } // reco from MC local reco data

      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" Decoder help:"<<endl;
         cout<<" Ex: Decoder [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] raw input file"<<endl;
         cout<<"      -out path/file : [def=*_Out.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
         cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
         cout<<"      -trk           : enable tracking actions"<<endl;
         cout<<"      -hit           : enable saving hits in tree (activated ntu option)"<<endl;
         cout<<"      -ntu           : enable tree filling"<<endl;
         cout<<"      -his           : enable crtl histograming"<<endl;
         cout<<"      -mc            : reco from MC local reco tree"<<endl;
         return 1;
      }
   }
   
   TApplication::CreateApplication();
   
   GlobalPar::Instance();
   GlobalPar::GetPar()->Print();
   
   BaseReco* glbRec = 0x0;
   glbRec = new GlobalToeReco(exp, in, out, mc);

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
   

   if (runNb != -1)
      glbRec->BaseReco::SetRunNumber(runNb);

   
   TStopwatch watch;
   watch.Start();
   
   glbRec->BeforeEventLoop();
   glbRec->LoopEvent(nTotEv);
   glbRec->AfterEventLoop();
   
   watch.Print();

   delete glbRec;
   
   return 0;
} 















