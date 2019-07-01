
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

#include "TASTdatRaw.hxx"
#include "TASTparMap.hxx"
#include "TASTparTime.hxx"

#include "TABMparGeo.hxx"
#include "TABMparCon.hxx"
#include "TABMdatRaw.hxx"

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"
#include "TASTactDatRaw.hxx"
#include "TABMactDatRaw.hxx"

#endif

// main
TAGactTreeWriter*   outFile   = 0x0;
TAGactDaqReader*    daqActReader = 0x0;
TAGdataDsc*         stDat     = 0x0;
TAGdataDsc*         evDaq     = 0x0;
TASTactDatRaw*      stActRaw  = 0x0;
TABMactDatRaw*      bmActRaw  = 0x0;

void FillST()
{
   TAGparaDsc* stMap = new TAGparaDsc("stMap", new TASTparMap());
   evDaq             = new TAGdataDsc("evDaq", new TAGdaqEvent());
   stDat             = new TAGdataDsc("stDat", new TASTdatRaw());
   TAGparaDsc* stTime = new TAGparaDsc("stTime", new TASTparTime());
   
   stActRaw  = new TASTactDatRaw("stActRaw", stDat, evDaq, stMap, stTime);
   stActRaw->CreateHistogram();
}

void FillBM()
{
   TAGparaDsc* bmGeo    = new TAGparaDsc("bmGeo", new TABMparGeo());
   TABMparGeo* geomap   = (TABMparGeo*) bmGeo->Object();
   geomap->FromFile("./geomaps/TABMdetector.map");
   
   TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparCon());
   TABMparCon* parconf = (TABMparCon*) bmConf->Object();
   parconf->FromFile("./config/TABMdetector.cfg");

   TAGparaDsc* bmMap = new TAGparaDsc("bmMap", new TABMparMap());
   TAGdataDsc* bmDat = new TAGdataDsc("bmDat", new TABMdatRaw());
   daqActReader      = new TAGactDaqReader("daqActReader", evDaq);

   bmActRaw  = new TABMactDatRaw("bmActRaw", bmDat, evDaq, bmMap, bmConf, bmGeo, stDat);
   bmActRaw->CreateHistogram();
}

//void ReadStBmRaw(TString filename = "data_test.00001431.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data",  Int_t nMaxEvts = 3)
void ReadStBmRaw(TString filename = "data_test.00001313.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data",  Int_t nMaxEvts = 3)
{
   TAGroot tagr;
   tagr.SetCampaignNumber(100);
   tagr.SetRunNumber(1);
   
   outFile = new TAGactTreeWriter("outFile");

   FillST();
   FillBM();
   daqActReader->Open(filename);
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(stActRaw);
   tagr.AddRequiredItem(bmActRaw);
   tagr.AddRequiredItem(outFile);

   tagr.Print();
   
   Int_t pos = filename.First(".");
   
   TString outFileName = filename(pos+1, 8);
   outFileName.Prepend("run.");

   outFileName.Append(".root");
   if (outFile->Open(outFileName.Data(), "RECREATE")) return;
   bmActRaw->SetHistogramDir(outFile->File());

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


