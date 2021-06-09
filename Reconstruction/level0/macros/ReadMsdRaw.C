
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
#include "TAGrecoManager.hxx"

#include "TAMSDparMap.hxx"
#include "TAMSDparGeo.hxx"

#include "TAGdaqEvent.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAGactDaqReader.hxx"
#include "TAMSDactNtuRaw.hxx"
#include "TAMSDactNtuHit.hxx"

#endif

// main
TAGcampaignManager* campManager  = 0x0;
TAGactTreeWriter*   outFile      = 0x0;
TAGactDaqReader*    daqActReader = 0x0;

TAMSDactNtuRaw*      msdActRaw  = 0x0;
TAMSDactNtuHit*      msdActHit  = 0x0;

void FillMsd(Int_t runNumber)
{
   TAGparaDsc* msdMap    = new TAGparaDsc("msdMap", new TAMSDparMap());
   TAMSDparMap* map   = (TAMSDparMap*) msdMap->Object();
   TString parFileName = campManager->GetCurMapFile(TAMSDparGeo::GetBaseName(), runNumber);
   map->FromFile(parFileName.Data());

   TAGparaDsc* msdGeo    = new TAGparaDsc("msdGeo", new TAMSDparGeo());
   TAMSDparGeo* geomap   = (TAMSDparGeo*) msdGeo->Object();
   parFileName = campManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   
   TAGparaDsc* msdCal = new TAGparaDsc("msdCal", new TAMSDparCal());
   TAMSDparCal* parCalMsd = (TAMSDparCal*)msdCal->Object();
   parFileName = campManager->GetCurCalFile(TAMSDparGeo::GetBaseName(), runNumber);
   parCalMsd->LoadEnergyCalibrationMap(parFileName.Data());
   
   
   TAGdataDsc* msdDaq    = new TAGdataDsc("msdDaq", new TAGdaqEvent());
   TAGdataDsc* msdDat    = new TAGdataDsc("msdDat", new TAMSDntuRaw());
   TAGdataDsc* msdHit    = new TAGdataDsc("msdHit", new TAMSDntuHit());

   daqActReader  = new TAGactDaqReader("daqActReader", msdDaq);
   
   msdActRaw  = new TAMSDactNtuRaw("msdActRaw", msdDat, msdDaq, msdMap, msdGeo);
   msdActRaw->CreateHistogram();

   msdActHit  = new TAMSDactNtuHit("msdActHit", msdDat, msdHit, msdGeo, msdCal);
   msdActHit->CreateHistogram();

}

//void ReadMsdRaw(TString filename = "data/data_test.00003864.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data", Int_t nMaxEmsds = 1,
void ReadMsdRaw(TString filename = "data/data_test.00003890.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data", Int_t nMaxEmsds = 1,
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

   FillMsd(runNumber);
   daqActReader->Open(filename);
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(msdActRaw);
   tagr.AddRequiredItem(msdActHit);
   tagr.AddRequiredItem(outFile);

   tagr.Print();
   
   Int_t pos = filename.First(".");
   
   
   TString outFileName = filename(pos+1, 8);
   outFileName.Prepend("run.");

   outFileName.Append(".root");
   if (outFile->Open(outFileName.Data(), "RECREATE")) return;
   msdActRaw->SetHistogramDir(outFile->File());
   msdActHit->SetHistogramDir(outFile->File());

   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Int_t nEvents = 0;
   while (tagr.NextEvent() ){
      
     // printf("\n");
      if (++nEvents % 100 == 0)
		printf("Event: %d\n", nEvents); 
	  
	  if (nEvents == nMaxEmsds)
		 break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();

}


