
#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "GlobalPar.hxx"
#include "LocalRecoMC.hxx"

int main (int argc, char *argv[])  {

   TString in("12C_C_200_1.root");
   TString out("");
   TString exp("");
   
   Bool_t mth = false;
   Bool_t test = false;
   
   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   
   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Root file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
      
      if(strcmp(argv[i],"-mth") == 0)   { mth = true;   } // enable multi threading (for clustering)

      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" Decoder help:"<<endl;
         cout<<" Ex: Decoder [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def= 12C_C_200_1.root] ROOT input file"<<endl;
         cout<<"      -out path/file : [def= 12C_C_200_1_Out.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
         cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
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
   
   GlobalPar::Instance(exp);
   GlobalPar::GetPar()->FromFile();
   GlobalPar::GetPar()->Print();

   Bool_t ntu = GlobalPar::GetPar()->IsSaveTree();
   Bool_t his = GlobalPar::GetPar()->IsSaveHisto();
   Bool_t hit = GlobalPar::GetPar()->IsSaveHits();
   Bool_t trk = GlobalPar::GetPar()->IsTracking();
   Bool_t zmc = GlobalPar::GetPar()->IsTWZmc();
   Bool_t zrec = GlobalPar::GetPar()->IsTWnoPU();
   Bool_t zmatch = GlobalPar::GetPar()->IsTWZmatch();
   
   
   if(!zmc && zrec) {
     Int_t pos_out = out.Last('.');
     out = out(0, pos_out);
     out.Append("_noTWPileUp_Zrec.root");
   }
   if(zmc) {
     Int_t pos_out = out.Last('.');
     out = out(0, pos_out);
     out.Append("_noTWPileUp_Ztrue.root");   
   }

   BaseReco* locRec = new LocalRecoMC(exp, runNb, in, out);

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
   
   if (mth)
      locRec->EnableM28lusMT();

   if(zmc)
     locRec->EnableZfromMCtrue();
   
   if(zrec && !zmc)
     locRec->EnableZrecWithPUoff();
   
   if(zmatch)
     locRec->EnableTWZmatch();
   
   TStopwatch watch;
   watch.Start();
   
   locRec->BeforeEventLoop();
   locRec->LoopEvent(nTotEv);
   locRec->AfterEventLoop();
   
   watch.Print();

   delete locRec;
   
   return 0;
} 















