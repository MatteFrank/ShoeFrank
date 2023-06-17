// Macro to read WC format
// Ch. Finck, sept 11.

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>

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
TAPWactNtuHit*      twActNtu    = 0x0;

// tree flag
Bool_t treeFlag = true;
// saving oscillogram in tree
Bool_t oscFlag = false;

void FillClinm(Int_t runNumber)
{
   TAGdataDsc* stRaw = 0x0;
   TAGdataDsc* stNtu = 0x0;
   
   TAGdataDsc* twRaw = 0x0;
   TAGdataDsc* twNtu = 0x0;
   
   TAGdataDsc* pwRaw = 0x0;
   TAGdataDsc* pwNtu = 0x0;
      
   TAGparaDsc* wcMap = new TAGparaDsc("wcMap", new TAGbaseWCparMap());
   TAGbaseWCparMap* map = (TAGbaseWCparMap*) wcMap->Object();
   TString parFileName = campManager->GetCurMapFile(TACEparGeo::GetBaseName(), runNumber);
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

void ReadWC(TString name = "Run_1200plas_1500plasnew_backTipex_2plas_coinc_24.6MeV_Data_12_10_2021_Binary.bin",
            TString expName = "TIIM2023", Int_t runNumber = 1)
{

   TAGroot tagr;
   
   // Par instance
   TAGrecoManager::Instance(expName);
   TAGrecoManager::GetPar()->FromFile();
   
   campManager = new TAGcampaignManager(expName);
   campManager->FromFile();

   outFile = new TAGactTreeWriter("outFile");
   
   FillClinm(runNumber);
   char path[400];
   strcpy(path, name.Data());
   wcFile->Open(path);
   
   tagr.AddRequiredItem(wcFile);
   if (TAGrecoManager::GetPar()->IncludeST())
      tagr.AddRequiredItem(stActNtu);
   if (TAGrecoManager::GetPar()->IncludeTW())
      tagr.AddRequiredItem(twActNtu);
   if (TAGrecoManager::GetPar()->IncludeCA())
      tagr.AddRequiredItem(pwActNtu);

   tagr.AddRequiredItem(outFile);
   tagr.Print();
   
   Int_t pos1 = name.Last('/');
   Int_t pos2 = name.Last('.');
   TString outputRootFile(name(pos1+1,pos2-pos1-1));
   
   outputRootFile += ".root";

   if (outFile->Open(outputRootFile.Data(), "RECREATE")) return;
   
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
	  
	  if (++nEvents == 200000) {
		 break;
	  }

   }
   
   tagr.EndEventLoop();

   outFile->Print();
   outFile->Close();
   
   watch.Print();
}


