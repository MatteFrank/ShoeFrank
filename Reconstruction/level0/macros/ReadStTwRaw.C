
// Macro to read catania raw data
// Ch. Finck, sept 11.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>

#include "Evento.hxx"
#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"

#include "TASTparMap.hxx"
#include "TATWparMap.hxx"

#include "TASTparGeo.hxx"
#include "TATWparGeo.hxx"

#include "TATWparCal.hxx"
#include "TATWparMap.hxx"
#include "TATWparTime.hxx"

#include "TASTdatRaw.hxx"
#include "TATWdatRaw.hxx"

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"
#include "TAGactWDreader.hxx"
#include "TATWactNtuHit.hxx"

#endif

// main
TAGactTreeWriter*   outFile   = 0x0;
TAGactDaqReader*    daqActReader = 0x0;
TAGactWDreader*      wdActRaw  = 0x0;
TATWactNtuHit*      twActNtu  = 0x0;

void FillStTw()
{
   TString expName = "GSI/";
   TAGdataDsc* twDaq    = new TAGdataDsc("twDaq", new TAGdaqEvent());
   daqActReader  = new TAGactDaqReader("daqActReader", twDaq);
   
   TAGparaDsc* parGeoSt = new TAGparaDsc(TASTparGeo::GetDefParaName(), new TASTparGeo());
   TASTparGeo* parGeo = (TASTparGeo*)parGeoSt->Object();
   TString parFileName = "./geomaps/TASTdetector.geo";
   parGeo->FromFile(parFileName.Data());
   
   TAGparaDsc* parMapSt = new TAGparaDsc("stMap", new TASTparMap()); // need the file
   TASTparMap* parMap = (TASTparMap*) parMapSt->Object();
   parFileName = Form("./config/%sTASTdetector.cfg", expName.Data());
   parMap->FromFile(parFileName);
   
   TAGparaDsc* parTimeSt = new TAGparaDsc("stTime", new TASTparTime()); // need the file
   TASTparTime* parTime = (TASTparTime*) parTimeSt->Object();
   parTime->FromFile(expName.Data(), 2190);
   
   TAGparaDsc* parMapTw = new TAGparaDsc("twMap", new TATWparMap());
   TATWparMap* parMapt = (TATWparMap*)parMapTw->Object();
   parFileName = Form("./config/%sTATWChannelMap.xml", expName.Data());
   parMapt->FromFile(parFileName.Data());
   
   TAGparaDsc*  parTimeTw = new TAGparaDsc("twTim", new TATWparTime());
   TATWparTime* parTimet = (TATWparTime*) parTimeTw->Object();
   parTimet->FromFile(expName.Data(), 2190);
   
   TAGparaDsc* parGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
   TATWparGeo* parGeoT = (TATWparGeo*)parGeoTw->Object();
   TString parFileNameT = "./geomaps/TATWdetector.geo";
   parGeoT->FromFile(parFileNameT);
   
   TAGparaDsc* parCalTw = new TAGparaDsc("twCal", new TATWparCal());
   TATWparCal* parCalT = (TATWparCal*)parCalTw->Object();
   parFileNameT = Form("./config/%sTATWCalibrationMap.xml", expName.Data());
   parCalT->FromFile(parFileNameT.Data());
   
   TAGdataDsc* stDat   = new TAGdataDsc("stDat", new TASTdatRaw());
   TAGdataDsc* twDat   = new TAGdataDsc("twdDat", new TATWdatRaw());
   wdActRaw  = new TAGactWDreader("wdActRaw", twDaq, stDat, twDat, parMapSt, parMapTw, parTimeSt, parTimeTw);
   wdActRaw->CreateHistogram();
   
   TAGdataDsc* twNtu  = new TAGdataDsc("twNtu", new TATWntuHit());
   twActNtu  = new TATWactNtuHit("twNtuRaw", twDat, twNtu, parGeoTw, parMapTw, parCalTw);
   twActNtu->CreateHistogram();
   
   //   outFile->SetupElementBranch(stDat, TASTdatRaw::GetBranchName());
   //   outFile->SetupElementBranch(twDat, TASTdatRaw::GetBranchName());
   outFile->SetupElementBranch(twNtu, TATWntuHit::GetBranchName());
}

void ReadStTwRaw(TString filename = "data/data_built.2242.physics_foot.daq.VTX.1.dat", Int_t nMaxEvts = 100)
{
   
   TAGroot tagr;
   tagr.SetCampaignNumber(100);
   tagr.SetRunNumber(1);
   
   outFile = new TAGactTreeWriter("outFile");
   
   FillStTw();
   
   daqActReader->Open(filename);
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(wdActRaw);
   
   tagr.AddRequiredItem(twActNtu);
   tagr.AddRequiredItem(outFile);
   
   tagr.Print();
   
   Int_t pos = filename.First(".");
   
   TString outFileName = filename(pos+1, 8);
   outFileName.Prepend("run.");
   
   outFileName.Append(".root");
   if (outFile->Open(outFileName.Data(), "RECREATE")) return;
   
   wdActRaw->SetHistogramDir(outFile->File());
   twActNtu->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Int_t nEvents = 0;
   
   while (tagr.NextEvent() ){
      
      // printf("\n");
      if (++nEvents % 1 == 0)
         printf("Event: %d\n", nEvents);
      
      if (nEvents == nMaxEvts)
         break;
   }
   tagr.EndEventLoop();
   outFile->Print();
   outFile->Close();
   watch.Print();
   
}


