
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
#include "TAGactTreeWriter.hxx"

#include "TAGgeoTrafo.hxx"

#include "TAGparGeo.hxx"

#include "TASTdatRaw.hxx"
#include "TASTparMap.hxx"
#include "TASTparTime.hxx"

#include "TABMparGeo.hxx"
#include "TABMparConf.hxx"
#include "TABMdatRaw.hxx"
#include "TABMNtuRaw.hxx"

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"
#include "TASTactDatRaw.hxx"
#include "TABMactDatRaw.hxx"
#include "TABMactNtuRaw.hxx"
#include "TABMactNtuTrack.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAVTactNtuRaw.hxx"
#include "TAVTactNtuClusterF.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAVTactNtuVertexPD.hxx"

#endif

// main
TAGactTreeWriter*   outFile   = 0x0;
TAGactDaqReader*    daqActReader = 0x0;
TAGparaDsc*         tgGeo     = 0x0;
TAGdataDsc*         stDat     = 0x0;
TAGdataDsc*         evDaq     = 0x0;
TAGdataDsc*         bmTrack   = 0x0;
TASTactDatRaw*      stActDat  = 0x0;
TABMactDatRaw*      bmActDat  = 0x0;
TABMactNtuRaw*      bmActNtu  = 0x0;
TABMactNtuTrack*    bmActTrack  = 0x0;
TAVTactNtuRaw*      vtActRaw  = 0x0;
TAVTactNtuClusterF* vtActClus = 0x0;
TAVTactNtuTrackF*   vtActTrack = 0x0;
TAVTactNtuVertexPD* vtActVtx  = 0x0;

void FillTG()
{
   tgGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
   TAGparGeo* parGeo = (TAGparGeo*)tgGeo->Object();
   TString parFileName = "./geomaps/TAGdetector%s.geo";
   parGeo->FromFile(parFileName.Data());
}

void FillST()
{
   TAGparaDsc* stMap  = new TAGparaDsc("stMap", new TASTparMap());
   evDaq              = new TAGdataDsc("evDaq", new TAGdaqEvent());
   stDat              = new TAGdataDsc("stDat", new TASTdatRaw());
   TAGparaDsc* stTime = new TAGparaDsc("stTime", new TASTparTime());

   stActDat  = new TASTactDatRaw("stActDat", stDat, evDaq, stMap, stTime);
   stActDat->CreateHistogram();
}

void FillBM()
{
   TAGparaDsc* bmGeo    = new TAGparaDsc("bmGeo", new TABMparGeo());
   TABMparGeo* geomap   = (TABMparGeo*) bmGeo->Object();
   geomap->FromFile("./geomaps/TABMdetector.geo");
   
   TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparConf());
   TABMparConf* parconf = (TABMparConf*) bmConf->Object();
   parconf->FromFile("./config/TABMdetector.cfg");

   TAGparaDsc* bmMap   = new TAGparaDsc("bmMap", new TABMparMap());
   TAGdataDsc* bmDat   = new TAGdataDsc("bmDat", new TABMdatRaw());
   TAGdataDsc* bmNtu   = new TAGdataDsc("bmNtu", new TABMntuRaw());
               bmTrack = new TAGdataDsc("bmTrack", new TABMntuTrack());


   bmActDat  = new TABMactDatRaw("bmActDat", bmDat, evDaq, bmMap, bmConf, bmGeo, stDat);
   bmActDat->CreateHistogram();
   
   bmActNtu = new TABMactNtuRaw("bmActNtu", bmNtu, bmDat, bmGeo, bmConf);
   bmActNtu->CreateHistogram();

   bmActTrack  = new TABMactNtuTrack("bmActTrack", bmTrack, bmNtu, bmGeo, bmConf, tgGeo);
   bmActTrack->CreateHistogram();

}

void FillVertex()
{
   TAGparaDsc* vtMap    = new TAGparaDsc("vtMap", new TAVTparMap());
   TAVTparMap* map   = (TAVTparMap*) vtMap->Object();
   map->FromFile("./config/TAVTdetector.map");

   TAGparaDsc* vtGeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());
   TAVTparGeo* geomap   = (TAVTparGeo*) vtGeo->Object();
   geomap->FromFile("./geomaps/TAVTdetector.geo");
   
   TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
   TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
   parconf->FromFile("./config/TAVTdetector.cfg");
   
   TAVTparConf::SetHistoMap();
   TAGdataDsc* vtNtu    = new TAGdataDsc("vtNtu", new TAVTntuRaw());
   TAGdataDsc* vtClus   = new TAGdataDsc("vtClus", new TAVTntuCluster());
   TAGdataDsc* vtTrck   = new TAGdataDsc("vtTrck", new TAVTntuTrack());
   TAGdataDsc* vtVtx    =  new TAGdataDsc("vtVtx",   new TAVTntuVertex());

   daqActReader  = new TAGactDaqReader("daqActReader", evDaq);
   
   vtActRaw  = new TAVTactNtuRaw("vtActRaw", vtNtu, evDaq, vtGeo, vtConf, vtMap);
   vtActRaw->CreateHistogram();
   
   vtActClus =  new TAVTactNtuClusterF("vtActClus", vtNtu, vtClus, vtConf, vtGeo);
   vtActClus->CreateHistogram();
   
   vtActTrack = new TAVTactNtuTrackF("vtActTrack", vtClus, vtTrck, vtConf, vtGeo);
   vtActTrack->CreateHistogram();
   
   vtActVtx    = new TAVTactNtuVertexPD("vtActVtx", vtTrck, vtVtx, vtConf, vtGeo, tgGeo, bmTrack);
   vtActVtx->CreateHistogram();

   //   outFile->SetupElementBranch(vtNtu, "vtrh.");
   //   outFile->SetupElementBranch(vtClus, "vtclus.");
   //   outFile->SetupElementBranch(vtTrck, "vtTrack.");
   
}

//void ReadStBmVt(TString filename = "data_test.00001431.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data",  Int_t nMaxEvts = 3)
void ReadStBmVt(TString filename = "data_test.00001313.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data",  Int_t nMaxEvts = 3)
{
   TAGroot tagr;
   tagr.SetCampaignNumber(100);
   tagr.SetRunNumber(1);
   
   TAGgeoTrafo* pFootGeo = new TAGgeoTrafo();
   pFootGeo->FromFile();

   outFile = new TAGactTreeWriter("outFile");

   FillTG();
   FillST();
   FillBM();
   FillVertex();

   daqActReader->Open(filename);
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(stActDat);
   tagr.AddRequiredItem(bmActDat);
   tagr.AddRequiredItem(bmActNtu);
   tagr.AddRequiredItem(bmActTrack);
   tagr.AddRequiredItem(vtActRaw);
   tagr.AddRequiredItem(vtActClus);
   tagr.AddRequiredItem(vtActTrack);
   tagr.AddRequiredItem(vtActVtx);
   tagr.AddRequiredItem(outFile);

   tagr.Print();
   
   Int_t pos = filename.First(".");
   
   TString outFileName = filename(pos+1, 8);
   outFileName.Prepend("run.");

   outFileName.Append(".root");
   if (outFile->Open(outFileName.Data(), "RECREATE")) return;
   bmActDat->SetHistogramDir(outFile->File());

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


