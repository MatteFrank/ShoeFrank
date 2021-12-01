#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGrecoManager.hxx"
#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "GlobalToeReco.hxx"

// executabel to read back from local reconstruction tree or from MC/raw data
// author: Ch. Finck


int main (int argc, char *argv[])  {

   TString in("");
   TString inMc("");
   TString out("");
   TString exp("");

   Bool_t mc  = false;
   Bool_t mth = false;

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   Int_t nSkipEv = 0;

   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);   }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);   }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]);  }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]);  }   // Number of events to be analized
      if(strcmp(argv[i],"-nsk") == 0)   { nSkipEv = atoi(argv[++i]); }  // Number of events to be skip
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);   }   // Run Number
      if(strcmp(argv[i],"-inmc") == 0)  { inMc = TString(argv[++i]);   } // MC file name

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
         cout<<"      -mc            : reco from MC local reco tree "<<endl;
         cout<<"      -inmc          : MC file name  "<<endl;
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
   
   Bool_t lrc = TAGrecoManager::GetPar()->IsLocalReco();
   Bool_t ntu = TAGrecoManager::GetPar()->IsSaveTree();
   Bool_t his = TAGrecoManager::GetPar()->IsSaveHisto();
   Bool_t hit = TAGrecoManager::GetPar()->IsSaveHits();
   Bool_t trk = TAGrecoManager::GetPar()->IsTracking();
   Bool_t zmc = TAGrecoManager::GetPar()->IsTWZmc();
   Bool_t zrec = TAGrecoManager::GetPar()->IsTWnoPU();
   Bool_t zmatch = TAGrecoManager::GetPar()->IsTWZmatch();
   Bool_t tbc = TAGrecoManager::GetPar()->IsTWCalBar();

   TAGrecoManager::GetPar()->IncludeTOE(true);
   TAGrecoManager::GetPar()->IncludeKalman(false);

   BaseReco* glbRec = 0x0;
   
   if (lrc)
      glbRec = new GlobalToeReco(exp, runNb, in, out, mc, inMc);
   else if (mc) {
     glbRec = new LocalRecoMC(exp, runNb, in, out);
     
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
   
   if (nSkipEv > 0 && (mc || lrc))
      glbRec->GoEvent(nSkipEv);
   
   TStopwatch watch;
   watch.Start();
    
    glbRec->BeforeEventLoop();
//    static_cast<TAGactTreeReader*>(gTAGroot->FindAction("evtReader"))->Reset();
//    gTAGroot->SetEventNumber(100000);
//    gTAGroot->SetEventNumber(1000000);
    glbRec->LoopEvent(nTotEv);
   glbRec->AfterEventLoop();
   
   watch.Print();

   delete glbRec;
   
   return 0;
} 















