
// Macro to reconstruct from raw data
// Ch. Finck, sept 11.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGactTreeWriter.hxx"

#include "TAGrecoManager.hxx"
#include "TAGbaseWCparMap.hxx"
#include "TAPLparGeo.hxx"
#include "TAPLntuRaw.hxx"
#include "TAPLntuHit.hxx"

#include "TACEparGeo.hxx"
#include "TACEntuRaw.hxx"
#include "TACEntuHit.hxx"

#include "TAPWparGeo.hxx"
#include "TAPWntuRaw.hxx"
#include "TAPWntuHit.hxx"

#include "TAGactWCreader.hxx"
#include "TAPLactNtuHit.hxx"
#include "TACEactNtuHit.hxx"
#include "TAPWactNtuHit.hxx"

#include "TAGcampaignManager.hxx"

#endif

// main
TAGgeoTrafo*        pGeoTrafo   = 0x0;
TAGcampaignManager* campManager = 0x0;
TAGactTreeWriter*   outFile     = 0x0;
TAGactWCreader*     wcFile      = 0x0;
TAPLactNtuHit*      stActNtu    = 0x0;
TACEactNtuHit*      twActNtu    = 0x0;
TAPWactNtuHit*      pwActNtu    = 0x0;

void FillClinm(Int_t runNumber, Bool_t treeFlag = true, Bool_t oscFlag = false)
{
   TAGdataDsc* stRaw = 0x0;
   TAGdataDsc* stNtu = 0x0;
   
   TAGdataDsc* twRaw = 0x0;
   TAGdataDsc* twNtu = 0x0;
   
   TAGdataDsc* pwRaw = 0x0;
   TAGdataDsc* pwNtu = 0x0;
   
   TAGparaDsc* wcMap = new TAGparaDsc("wcMap", new TAGbaseWCparMap());
   TAGbaseWCparMap* map = (TAGbaseWCparMap*) wcMap->Object();
   TString parFileName = campManager->GetCurMapFile(TAPLparGeo::GetBaseName(), runNumber);
   map->FromFile(parFileName.Data());
   
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
      stRaw    = new TAGdataDsc("stRaw", new TAPLntuRaw());
      stNtu    = new TAGdataDsc("stNtu", new TAPLntuHit());
      
      stActNtu = new TAPLactNtuHit("stActNtu", stNtu, stRaw);
      stActNtu->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      twRaw    = new TAGdataDsc("twRaw", new TACEntuRaw());
      twNtu    = new TAGdataDsc("twNtu", new TACEntuHit());
      
      twActNtu = new TACEactNtuHit("twActNtu", twNtu, twRaw);
      twActNtu->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeCA()) {
      pwRaw    = new TAGdataDsc("pwRaw", new TAPWntuRaw());
      pwNtu    = new TAGdataDsc("pwNtu", new TAPWntuHit());
      
      pwActNtu = new TAPWactNtuHit("pwActNtu", pwNtu, pwRaw);
      pwActNtu->CreateHistogram();
   }
   
   wcFile = new TAGactWCreader("wcFile", wcMap, stRaw, twRaw, pwRaw);
   
   if (treeFlag) {
      if (oscFlag) {
         if (TAGrecoManager::GetPar()->IncludeST())
            outFile->SetupElementBranch(stRaw, TAPLntuRaw::GetBranchName());
         
         if (TAGrecoManager::GetPar()->IncludeTW())
            outFile->SetupElementBranch(twRaw, TACEntuRaw::GetBranchName());
         
         if (TAGrecoManager::GetPar()->IncludeCA())
            outFile->SetupElementBranch(pwRaw, TAPWntuRaw::GetBranchName());
      }
      if (TAGrecoManager::GetPar()->IncludeST())
         outFile->SetupElementBranch(stNtu, TAPLntuHit::GetBranchName());
      
      if (TAGrecoManager::GetPar()->IncludeTW())
         outFile->SetupElementBranch(twNtu, TACEntuHit::GetBranchName());
      
      if (TAGrecoManager::GetPar()->IncludeCA())
         outFile->SetupElementBranch(pwNtu, TAPWntuHit::GetBranchName());
   }
}

int main (int argc, char *argv[])  {
   
   TString in("");
   TString out("");
   TString exp("");
   
   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   Bool_t oscFlag = false;

   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number
      if(strcmp(argv[i],"-osc") == 0)   { oscFlag = true;           } // reco from MC local reco data


      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" Decoder help:"<<endl;
         cout<<" Ex: Decoder [opts] "<<endl;
         cout << "usage: DecodeWC -in dataRaw/Run_6_config1_200MeVu_12C_Data_5_26_2023_Binary.bin -out toto.root -exp TIIM2023 -run 1 -nev 10000" <<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] raw input file"<<endl;
         cout<<"      -out path/file : [def=*_Out.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
         cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
         cout<<"      -osc           : save oscillograms in tree"<<endl;

         return 1;
      }
   }
   
   TApplication::CreateApplication();
   TAGrecoManager::Instance(exp);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   // saving in tree
   Bool_t treeFlag = TAGrecoManager::GetPar()->IsSaveTree();

   TAGroot tagr;
   
   campManager = new TAGcampaignManager(exp);
   campManager->FromFile();

   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNb);
   geoTrafo->FromFile(parFileName);
   
   outFile = new TAGactTreeWriter("outFile");

   FillClinm(runNb, treeFlag, oscFlag);

   wcFile->Open(in);
   
   tagr.AddRequiredItem(wcFile);
   if (TAGrecoManager::GetPar()->IncludeST())
      tagr.AddRequiredItem(stActNtu);
   if (TAGrecoManager::GetPar()->IncludeTW())
      tagr.AddRequiredItem(twActNtu);
   if (TAGrecoManager::GetPar()->IncludeCA())
      tagr.AddRequiredItem(pwActNtu);
   
   tagr.AddRequiredItem(outFile);
   tagr.Print();

   TString outputRootFile  = out;
   
   if (outFile->Open(outputRootFile.Data(), "RECREATE")) return 0;
   
   if (TAGrecoManager::GetPar()->IncludeST())
      stActNtu->SetHistogramDir(outFile->File());
   if (TAGrecoManager::GetPar()->IncludeTW())
      twActNtu->SetHistogramDir(outFile->File());
   if (TAGrecoManager::GetPar()->IncludeCA())
      pwActNtu->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Int_t nEvents = 0;
   while (tagr.NextEvent() ){
      
      if (++nEvents == nTotEv)
         break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();
   
   return 0;

}


