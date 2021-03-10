
// Macro to read catania raw data
// Ch. Finck, sept 11.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <iostream>
#include <map>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include "Evento.hxx"
#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TASTparMap.hxx"
#include "TASTdatRaw.hxx"
#include "TASTntuHit.hxx"
#include "TASTactNtuHit.hxx"
#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"
#include "TASTactDatRaw.hxx"

#endif

// main
TAGactTreeWriter*   outFile   = 0x0;
TAGactDaqReader*    daqActReader = 0x0;
TASTactDatRaw*      stActRaw  = 0x0;
TASTactNtuHit*      stActNtuRaw  = 0x0;
TASTparMap *parMap;
TAGdataDsc* stNtu;

void FillST(){

  
  TAGparaDsc* stMap;
  TAGdataDsc* stDaq;
  TAGdataDsc* stDat;
  TAGparaDsc* stTime;
  

   stMap    = new TAGparaDsc("stMap", new TASTparMap());
   stDaq    = new TAGdataDsc("stDaq", new TAGdaqEvent());
   stDat    = new TAGdataDsc("stDat", new TASTdatRaw());
   stTime   = new TAGparaDsc("stTime", new TASTparTime());
   stNtu    = new TAGdataDsc("stNtu", new TASTntuHit());
   
   daqActReader  = new TAGactDaqReader("daqActReader", stDaq);

   stActRaw  = new TASTactDatRaw("stActRaw", stDat, stDaq, stMap, stTime);
   stActNtuRaw  = new TASTactNtuHit("stActNtuRaw", stDat, stNtu);
   //stNtuRaw->CreateHistogram();
   //   stActNtuRaw->SetHistogramDir(outFile->File());
   
   parMap = (TASTparMap*)stMap->Object();
   parMap->FromFile("config/TASTdetector.cfg");

}

void ReadStRaw(TString filename = "data_test.00001313.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data",
                Int_t nMaxEvts = 3)
{

   TAGroot tagr;
   tagr.SetCampaignNumber(100);
   tagr.SetRunNumber(1);
   
   outFile = new TAGactTreeWriter("outFile");
   

   FillST();

   outFile->SetupElementBranch(stNtu,"stNtu");
   
   daqActReader->Open(filename);


   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(stActRaw);
   tagr.AddRequiredItem(stActNtuRaw);
   tagr.AddRequiredItem(outFile);

   tagr.Print();


   stActRaw->CreateHistogram();
 

   Int_t pos = filename.First(".");
   TString outFileName = filename(pos+1, 8);
   outFileName.Prepend("run.");
   outFileName.Append(".root");
   if (outFile->Open(outFileName.Data(), "RECREATE")) return;
   stActRaw->SetHistogramDir(outFile->File());
   
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


