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

#include "TAGbaseWCparMap.hxx"
#include "TASTparGeo.hxx"
#include "TASTdatRaw.hxx"
#include "TASTntuHit.hxx"

#include "TATWparGeo.hxx"
#include "TATWdatRaw.hxx"
#include "TATWntuHit.hxx"

#include "TAPWparGeo.hxx"
#include "TAPWdatRaw.hxx"
#include "TAPWntuHit.hxx"

#include "TAGactWCreader.hxx"
#include "TASTactNtuRaw.hxx"
#include "TATWactNtuRaw.hxx"
#include "TAPWactNtuRaw.hxx"

#endif

// main
TAGgeoTrafo*      pGeoTrafo = 0x0;
TAGactTreeWriter* outFile   = 0x0;
TAGactWCreader*   wcFile    = 0x0;
TASTactNtuRaw*    stActNtu  = 0x0;
TATWactNtuRaw*    twActNtu  = 0x0;
TAPWactNtuRaw*    pwActNtu  = 0x0;

// tree flag
Bool_t treeFlag = true;
// saving oscillogram in tree
Bool_t oscFlag = false;

void FillClinm() {
   
   TAGdataDsc* stRaw = 0x0;
   TAGdataDsc* stNtu = 0x0;
   
   TAGdataDsc* twRaw = 0x0;
   TAGdataDsc* twNtu = 0x0;
   
   TAGdataDsc* pwRaw = 0x0;
   TAGdataDsc* pwNtu = 0x0;
   
   TAGparaDsc* wcMap = new TAGparaDsc("wcMap", new TAGbaseWCparMap());
   TAGbaseWCparMap* map = (TAGbaseWCparMap*) wcMap->Object();
   map->FromFile("./config/WCdetector_CLINM.map");
   
   
   if (pGeoTrafo->GetDeviceStatus(TASTparGeo::GetBaseName())) {
      stRaw    = new TAGdataDsc("stRaw", new TASTdatRaw());
      stNtu    = new TAGdataDsc("stNtu", new TASTntuHit());
      
      stActNtu = new TASTactNtuRaw("stActNtu", stNtu, stRaw);
      stActNtu->CreateHistogram();
   }
   
   if (pGeoTrafo->GetDeviceStatus(TATWparGeo::GetBaseName())) {
      twRaw    = new TAGdataDsc("twRaw", new TATWdatRaw());
      twNtu    = new TAGdataDsc("twNtu", new TATWntuHit());
      
      twActNtu = new TATWactNtuRaw("twActNtu", twNtu, twRaw);
      twActNtu->CreateHistogram();
   }
   
   if (pGeoTrafo->GetDeviceStatus(TAPWparGeo::GetBaseName())) {
      pwRaw    = new TAGdataDsc("pwRaw", new TAPWdatRaw());
      pwNtu    = new TAGdataDsc("pwNtu", new TAPWntuHit());
      
      pwActNtu = new TAPWactNtuRaw("pwActNtu", pwNtu, pwRaw);
      pwActNtu->CreateHistogram();
   }
   
   wcFile = new TAGactWCreader("wcFile", wcMap, stRaw, twRaw, pwRaw);
   
   if (treeFlag) {
      if (oscFlag) {
         if (pGeoTrafo->GetDeviceStatus(TASTparGeo::GetBaseName()))
            outFile->SetupElementBranch(stRaw, TASTdatRaw::GetBranchName());
         
         if (pGeoTrafo->GetDeviceStatus(TATWparGeo::GetBaseName()))
            outFile->SetupElementBranch(twRaw, TATWdatRaw::GetBranchName());
         
         if (pGeoTrafo->GetDeviceStatus(TAPWparGeo::GetBaseName()))
            outFile->SetupElementBranch(pwRaw, TAPWdatRaw::GetBranchName());
      }
      if (pGeoTrafo->GetDeviceStatus(TASTparGeo::GetBaseName()))
         outFile->SetupElementBranch(stNtu, TASTntuHit::GetBranchName());
      
      if (pGeoTrafo->GetDeviceStatus(TATWparGeo::GetBaseName()))
         outFile->SetupElementBranch(twNtu, TATWntuHit::GetBranchName());
      
      if (pGeoTrafo->GetDeviceStatus(TAPWparGeo::GetBaseName()))
         outFile->SetupElementBranch(pwNtu, TAPWntuHit::GetBranchName());
   }
}

void ReadWC(TString name = "/work/desis/STIVI/dataWC/calbutcyrce/jour2/cyrce_1221/Run_1200plas_1500plasnew_backTipex_2plas_coinc_24.6MeV_Data_12_10_2021_Binary.bin",
            TString expName = "CLINM")
{

   TAGroot tagr;
   tagr.SetCampaignNumber(100);
   tagr.SetRunNumber(1);
   
   TString geoFileName(Form("./geomaps/%s_geo.map", expName.Data()));
   
   pGeoTrafo = new TAGgeoTrafo();
   pGeoTrafo->InitGeo(geoFileName.Data());
   
   outFile = new TAGactTreeWriter("outFile");
   
   FillClinm();
   char path[400];
   strcpy(path, name.Data());
   wcFile->Open(path);
   
   tagr.AddRequiredItem(wcFile);
   if (pGeoTrafo->GetDeviceStatus(TASTparGeo::GetBaseName()))
      tagr.AddRequiredItem(stActNtu);
   if (pGeoTrafo->GetDeviceStatus(TATWparGeo::GetBaseName()))
      tagr.AddRequiredItem(twActNtu);
   if (pGeoTrafo->GetDeviceStatus(TAPWparGeo::GetBaseName()))
      tagr.AddRequiredItem(pwActNtu);
   
   tagr.AddRequiredItem(outFile);
   tagr.Print();
   
   Int_t pos1 = name.Last('/');
   Int_t pos2 = name.Last('.');
   TString outputRootFile(name(pos1+1,pos2-pos1-1));
   
   outputRootFile += ".root";

   if (outFile->Open(outputRootFile.Data(), "RECREATE")) return;
   if (pGeoTrafo->GetDeviceStatus(TASTparGeo::GetBaseName()))
      stActNtu->SetHistogramDir(outFile->File());
   if (pGeoTrafo->GetDeviceStatus(TATWparGeo::GetBaseName()))
      twActNtu->SetHistogramDir(outFile->File());
   if (pGeoTrafo->GetDeviceStatus(TAPWparGeo::GetBaseName()))
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


