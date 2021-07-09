
// Macro to reconstruct from raw data
// Ch. Finck, sept 11.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>

#include "TAGgeoTrafo.hxx"
#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGcampaignManager.hxx"

#include "TAVTparMap.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"
#include "TAVTactNtuHit.hxx"

#include "TAVTactNtuCluster.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAGrecoManager.hxx"

#endif

// main
TAGcampaignManager* campManager  = 0x0;
TAGactTreeWriter*   outFile      = 0x0;
TAGactDaqReader*    daqActReader = 0x0;

TAVTactNtuHit*      vtActRaw  = 0x0;
TAVTactNtuCluster* vtActClus = 0x0;
TAVTactNtuTrackF*   vtActTrck = 0x0;

void FillVertex(Int_t runNumber)
{
   TAGparaDsc* vtMap    = new TAGparaDsc("vtMap", new TAVTparMap());
   TAVTparMap* map   = (TAVTparMap*) vtMap->Object();
   TString parFileName = campManager->GetCurMapFile(TAVTparGeo::GetBaseName(), runNumber);
   map->FromFile(parFileName.Data());

   TAGparaDsc* vtGeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());
   TAVTparGeo* geomap   = (TAVTparGeo*) vtGeo->Object();
   parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   
   TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
   TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
   parFileName = campManager->GetCurConfFile(TAVTparGeo::GetBaseName(), runNumber);
   parconf->FromFile(parFileName.Data());

   TAVTparConf::SetHistoMap();
   TAGdataDsc* vtDaq    = new TAGdataDsc("vtDaq", new TAGdaqEvent());
   TAGdataDsc* vtNtu    = new TAGdataDsc("vtNtu", new TAVTntuHit());
   TAGdataDsc* vtClus   = new TAGdataDsc("vtClus", new TAVTntuCluster());
   TAGdataDsc* vtTrck   = new TAGdataDsc("vtTrck", new TAVTntuTrack());

   daqActReader  = new TAGactDaqReader("daqActReader", vtDaq);
   
   vtActRaw  = new TAVTactNtuHit("vtActRaw", vtNtu, vtDaq, vtGeo, vtConf, vtMap);
   vtActRaw->CreateHistogram();

   vtActClus =  new TAVTactNtuCluster("vtActClus", vtNtu, vtClus, vtConf, vtGeo);
   vtActClus->CreateHistogram();

   vtActTrck = new TAVTactNtuTrackF("vtActTrck", vtClus, vtTrck, vtConf, vtGeo);
   vtActTrck->CreateHistogram();
   
//   outFile->SetupElementBranch(vtNtu, "vtrh.");
//   outFile->SetupElementBranch(vtClus, "vtclus.");
//   outFile->SetupElementBranch(vtTrck, "vttrack.");

}

void ReadVtxRaw(TString filename = "data/data_test.00004037.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data", Int_t nMaxEvts = 100000,
                TString expName = "GSI2021", Int_t runNumber = 1)
{
   TAGrecoManager::Instance(expName);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   TAGroot tagr;
   
   campManager = new TAGcampaignManager(expName);
   campManager->FromFile();

   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
   geoTrafo->FromFile(parFileName);
   
   outFile = new TAGactTreeWriter("outFile");

   FillVertex(runNumber);
   daqActReader->Open(filename);
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(vtActRaw);
   tagr.AddRequiredItem(vtActClus);
   //   tagr.AddRequiredItem(vtActTrck);
   tagr.AddRequiredItem(outFile);

   tagr.Print();
   
   Int_t pos = filename.First(".");
   
   
   TString outFileName = filename(pos+1, 8);
   outFileName.Prepend("run.");

   outFileName.Append(".root");
   if (outFile->Open(outFileName.Data(), "RECREATE")) return;
   vtActRaw->SetHistogramDir(outFile->File());
   vtActClus->SetHistogramDir(outFile->File());
   vtActTrck->SetHistogramDir(outFile->File());

   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Int_t nEvents = 0;
   while (tagr.NextEvent() ){
      
     // printf("\n");
      if (++nEvents % 100 == 0)
		printf("Event: %d\n", nEvents); 
	  
	  if (nEvents == nMaxEvts)
		 break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();

}


