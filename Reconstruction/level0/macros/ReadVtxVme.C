
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

#include "TAVTparMap.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"

#include "TAVTactVmeReader.hxx"
#include "TAVTactNtuClusterF.hxx"
#include "TAVTactNtuTrack.hxx"

#endif

// main
TAGactTreeWriter*   outFile   = 0x0;
TAVTactVmeReader*   vmeFile   = 0x0;
TAVTactNtuClusterF* vtActClus = 0x0;
TAVTactNtuTrack*   vtActTrck = 0x0;

void FillVertex()
{
   TAGparaDsc* vtMap    = new TAGparaDsc("vtMap", new TAVTparMap());
   TAVTparMap* map   = (TAVTparMap*) vtMap->Object();
   map->FromFile("./config/TAVTdetector.map");
   
   
   TAGparaDsc* vtGeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());
   TAVTparGeo* geomap   = (TAVTparGeo*) vtGeo->Object();
   geomap->FromFile("./geomaps/GSI/TAVTdetector.geo");
   
   TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
   TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
   parconf->FromFile("./config/GSI/TAVTdetector.cfg");
   
   TAVTparConf::SetHistoMap();
   TAGdataDsc* vtNtu    = new TAGdataDsc("vtNtu", new TAVTntuHit());
   TAGdataDsc* vtClus   = new TAGdataDsc("vtClus", new TAVTntuCluster());
   TAGdataDsc* vtTrck   = new TAGdataDsc("vtTrck", new TAVTntuTrack());
   
   
   vtActClus =  new TAVTactNtuClusterF("vtActClus", vtNtu, vtClus, vtConf, vtGeo);
   vtActClus->CreateHistogram();
   
   vtActTrck = new TAVTactNtuTrack("vtActTrck", vtClus, vtTrck, vtConf, vtGeo);
   vtActTrck->CreateHistogram();
   
   //   outFile->SetupElementBranch(vtNtu, "vtrh.");
   //   outFile->SetupElementBranch(vtClus, "vtclus.");
   //   outFile->SetupElementBranch(vtTrck, "vttrack.");
   
   vmeFile = new TAVTactVmeReader("vmeFile", vtNtu, vtGeo, vtConf, vtMap);
   vmeFile->CreateHistogram();
   
}

void ReadVtxVme(TString path = "./run_1026", Int_t nMaxEvts = 10000)
{
   TAGroot tagr;
   tagr.SetCampaignNumber(100);
   tagr.SetRunNumber(1);
   
   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   geoTrafo->FromFile();
   
   outFile = new TAGactTreeWriter("outFile");
   
   FillVertex();
   vmeFile->Open(path);
   
   tagr.AddRequiredItem(vmeFile);
   tagr.AddRequiredItem(vtActClus);
   // tagr.AddRequiredItem(vtActTrck);
   tagr.AddRequiredItem(outFile);
   
   tagr.Print();
   
   TString outFileName = path;
   outFileName.Append(".root");
   if (outFile->Open(outFileName.Data(), "RECREATE")) return;
   vmeFile->SetHistogramDir(outFile->File());
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


