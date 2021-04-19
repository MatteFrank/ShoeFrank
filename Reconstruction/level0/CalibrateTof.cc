
#include <TString.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGrecoManager.hxx"
#include "LocalReco.hxx"

int main (int argc, char *argv[])  {

   TString in("12C_C_200_1.root");
   TString out("");
   TString exp("");

   Bool_t mth = false;

   Int_t runNb = -1;
   Int_t nTotEv = 1e7;

   for (int i = 0; i < argc; i++){
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

   TAGrecoManager::Instance(exp);
   TAGrecoManager::GetPar()->Print();

   Bool_t ntu = TAGrecoManager::GetPar()->IsSaveTree();
   Bool_t his = TAGrecoManager::GetPar()->IsSaveHisto();
   Bool_t hit = TAGrecoManager::GetPar()->IsSaveHits();

   //Whatever it is we're ONLY going to calibrate TW!
   TAGrecoManager::GetPar()->IncludeST(true);
   TAGrecoManager::GetPar()->IncludeTW(true);
   TAGrecoManager::GetPar()->CalibTW(true);
   TAGrecoManager::GetPar()->IncludeDI(false);
   TAGrecoManager::GetPar()->IncludeBM(false);
   TAGrecoManager::GetPar()->IncludeMSD(false);
   TAGrecoManager::GetPar()->IncludeCA(false);
   TAGrecoManager::GetPar()->IncludeTG(false);
   TAGrecoManager::GetPar()->IncludeVT(false);
   TAGrecoManager::GetPar()->IncludeIT(false);

   out = Form("./dE-TOF_Calib_%s_run%d.root",exp.Data(),runNb);

   BaseReco* locRec = new LocalReco(exp, runNb, in, out);

   // global setting
   if (ntu)
      locRec->EnableTree();
   if(his)
      locRec->EnableHisto();
   if(hit) {
      locRec->EnableTree();
      locRec->EnableSaveHits();
   }

   TStopwatch watch;
   watch.Start();

   locRec->BeforeEventLoop();
   locRec->LoopEvent(nTotEv);
   locRec->AfterEventLoop();

   watch.Print();

   delete locRec;

   return 0;
}
