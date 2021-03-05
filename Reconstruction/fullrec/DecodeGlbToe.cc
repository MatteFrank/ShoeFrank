#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "GlobalPar.hxx"
#include "LocalReco.hxx"
#include "LocalRecoNtuMC.hxx"
#include "GlobalToeReco.hxx"

// executabel to read back from local reconstruction tree or from MC/raw data
// author: Ch. Finck


int main (int argc, char *argv[])  {

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
   
   if (out.IsNull()) {
      Int_t pos = in.Last('.');
      out = in(0, pos);
      out.Append("_Out.root");
   }
   
   TApplication::CreateApplication();
   
   GlobalPar::Instance(exp);
   GlobalPar::GetPar()->Print();
   
   Bool_t lrc = GlobalPar::GetPar()->IsLocalReco();
   Bool_t ntu = GlobalPar::GetPar()->IsSaveTree();
   Bool_t his = GlobalPar::GetPar()->IsSaveHisto();
   Bool_t hit = GlobalPar::GetPar()->IsSaveHits();
   Bool_t trk = GlobalPar::GetPar()->IsTracking();
   Bool_t zmc = GlobalPar::GetPar()->IsTWZmc();
   Bool_t zrec = GlobalPar::GetPar()->IsTWnoPU();
   Bool_t zmatch = GlobalPar::GetPar()->IsTWZmatch();
   Bool_t tbc = GlobalPar::GetPar()->IsTWCalBar();

   GlobalPar::GetPar()->IncludeTOE(true);
   GlobalPar::GetPar()->IncludeKalman(false);

   BaseReco* glbRec = 0x0;
   
   if (lrc)
      glbRec = new GlobalToeReco(exp, runNb, in, out, mc);
   else if (mc) {
     glbRec = new LocalRecoNtuMC(exp, runNb, in, out);
     
      if(zmc)
         glbRec->EnableZfromMCtrue();
      if(zrec && !zmc)
        glbRec->EnableZrecWithPUoff();
      if(zmatch)
        glbRec->EnableTWZmatch();

   } else {
     glbRec = new LocalReco(exp, runNb, in, out);
     if (tbc)
       glbRec->EnableTWcalibPerBar();
     if(zmatch)
       glbRec->EnableTWZmatch();
   }
   

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
   
   if (mth)
      glbRec->EnableM28lusMT();
   
   TStopwatch watch;
   watch.Start();
   
   glbRec->BeforeEventLoop();
   glbRec->LoopEvent(nTotEv);
   glbRec->AfterEventLoop();
   
   watch.Print();

   delete glbRec;
   
   return 0;
} 















