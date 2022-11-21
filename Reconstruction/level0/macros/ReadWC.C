// Macro to read catania raw data
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

#include "TAGactWCreader.hxx"
#include "TAPLactNtuRaw.hxx"
#include "TACEactNtuRaw.hxx"

#endif

// main
TAGgeoTrafo*      pGeoTrafo = 0x0;
TAGactTreeWriter* outFile   = 0x0;
TAGactWCreader*   wcFile    = 0x0;
TAPLactNtuRaw*    stActNtu  = 0x0;
TACEactNtuRaw*    twActNtu  = 0x0;

// tree flag
Bool_t treeFlag = true;
// saving oscillogram in tree
Bool_t oscFlag = false;

void FillClinm()
{
   TAGdataDsc* stRaw = 0x0;
   TAGdataDsc* stNtu = 0x0;
   
   TAGdataDsc* twRaw = 0x0;
   TAGdataDsc* twNtu = 0x0;
      
   TAGparaDsc* wcMap = new TAGparaDsc("wcMap", new TAGbaseWCparMap());
   TAGbaseWCparMap* map = (TAGbaseWCparMap*) wcMap->Object();
   map->FromFile("./config/WCdetector_CLINM.map");
   
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
      stRaw    = new TAGdataDsc("stRaw", new TAPLntuRaw());
      stNtu    = new TAGdataDsc("stNtu", new TAPLntuHit());
      
      stActNtu = new TAPLactNtuRaw("stActNtu", stNtu, stRaw);
      stActNtu->CreateHistogram();
   }
   
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      twRaw    = new TAGdataDsc("twRaw", new TACEntuRaw());
      twNtu    = new TAGdataDsc("twNtu", new TACEntuHit());
      
      twActNtu = new TACEactNtuRaw("twActNtu", twNtu, twRaw);
      twActNtu->CreateHistogram();
   }
   
   wcFile = new TAGactWCreader("wcFile", wcMap, stRaw, twRaw);
   
   if (treeFlag) {
      if (oscFlag) {
         if (pGeoTrafo->GetDeviceStatus(TAPLparGeo::GetBaseName()))
            outFile->SetupElementBranch(stRaw, TAPLntuRaw::GetBranchName());
         
         if (pGeoTrafo->GetDeviceStatus(TACEparGeo::GetBaseName()))
            outFile->SetupElementBranch(twRaw, TACEntuRaw::GetBranchName());
      }
      if (pGeoTrafo->GetDeviceStatus(TAPLparGeo::GetBaseName()))
         outFile->SetupElementBranch(stNtu, TAPLntuHit::GetBranchName());
      
      if (pGeoTrafo->GetDeviceStatus(TACEparGeo::GetBaseName()))
         outFile->SetupElementBranch(twNtu, TACEntuHit::GetBranchName());
   }
}

void ReadWC(TString name = "/work/desis/STIVI/dataWC/calbutcyrce/jour2/cyrce_1221/Run_1200plas_1500plasnew_backTipex_2plas_coinc_24.6MeV_Data_12_10_2021_Binary.bin",
            TString expName = "CLINM")
{

   TAGroot tagr;
   tagr.SetCampaignNumber(100);
   tagr.SetRunNumber(1);
   
   // Par instance
   TAGrecoManager::Instance(expName);
   TAGrecoManager::GetPar()->FromFile();
   
   outFile = new TAGactTreeWriter("outFile");
   
   FillClinm();
   char path[400];
   strcpy(path, name.Data());
   wcFile->Open(path);
   
   tagr.AddRequiredItem(wcFile);
   if (pGeoTrafo->GetDeviceStatus(TAPLparGeo::GetBaseName()))
      tagr.AddRequiredItem(stActNtu);
   if (pGeoTrafo->GetDeviceStatus(TACEparGeo::GetBaseName()))
      tagr.AddRequiredItem(twActNtu);
   
   tagr.AddRequiredItem(outFile);
   tagr.Print();
   
   Int_t pos1 = name.Last('/');
   Int_t pos2 = name.Last('.');
   TString outputRootFile(name(pos1+1,pos2-pos1-1));
   
   outputRootFile += ".root";

   if (outFile->Open(outputRootFile.Data(), "RECREATE")) return;
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
      stActNtu->SetHistogramDir(outFile->File());
      if (TAGrecoManager::GetPar()->IncludeTW()) {
      twActNtu->SetHistogramDir(outFile->File());

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


