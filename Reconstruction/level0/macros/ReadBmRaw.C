// Macro to make reconstruction from MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>

#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"

#include "TAGparGeo.hxx"

#include "TASTparMap.hxx"
#include "TASTdatRaw.hxx"
#include "TASTactDatRaw.hxx"

#include "TABMparGeo.hxx"
#include "TABMparMap.hxx"
#include "TABMparCon.hxx"
#include "TABMdatRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TABMactDatRaw.hxx"
#include "TABMactNtuRaw.hxx"
#include "TABMactNtuTrack.hxx"

#include "GlobalPar.hxx"
#include "Materials.hxx"

#endif

// main
TAGactTreeWriter* outFile   = 0x0;

TAGactDaqReader*  daqActReader = 0x0;
TASTactDatRaw* stActDatRaw  = 0x0;
TABMactDatRaw* bmActDatRaw  = 0x0;
TABMactNtuRaw* bmActNtuRaw  = 0x0;
TABMactNtuTrack* bmActTrack = 0x0;

void FillBm(TString fExpName) {
   
   cout<<"start FillBm"<<endl;
   
   //parameters
   TAGparaDsc* tgGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
   TAGparGeo* parGeo = (TAGparGeo*)tgGeo->Object();
   parGeo->FromFile();
     
   TAGparaDsc* bmGeo    = new TAGparaDsc("bmGeo", new TABMparGeo());
   TABMparGeo* bmgeomap   = (TABMparGeo*) bmGeo->Object();
   TString parFileName = "./geomaps/TABMdetector.map";
   bmgeomap->FromFile(parFileName.Data());

   TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparCon());
   TABMparCon* parConf = (TABMparCon*)bmConf->Object();
   parFileName = "./config/TABMdetector.cfg";
   parConf->FromFile(parFileName.Data());
   parFileName = "./config/bmreso_vs_r.root";
   parConf->LoadReso(parFileName);
   parFileName = Form("./config/%sT0_beammonitor.cfg", fExpName.Data());
   parConf->loadT0s(parFileName);
   
   TAGparaDsc*  bmMap  = new TAGparaDsc("bmMap", new TABMparMap());
   TABMparMap*  bmparMap = (TABMparMap*)bmMap->Object();
   parFileName = Form("./config/%sTABMdetector.map", fExpName.Data());
   bmparMap->FromFile(parFileName.Data(), bmgeomap);
   
   TAGparaDsc* fpParTimeSt = new TAGparaDsc("stTime", new TASTparTime()); // need the file
   TASTparTime* parTimeSt = (TASTparTime*) fpParTimeSt->Object();
   parTimeSt->FromFile("GSI", 2190);
  
   TAGparaDsc*  stMap  = new TAGparaDsc("stMap", new TASTparMap());
   TASTparMap*  stparMap = (TASTparMap*)stMap->Object();
   parFileName="./config/TASTdetector.cfg";
   stparMap->FromFile(parFileName.Data());
  
  //TAGdataDsc
   TAGdataDsc* bmDaq    = new TAGdataDsc("bmDaq", new TAGdaqEvent());
   daqActReader  = new TAGactDaqReader("daqActReader", bmDaq); 
  
   TAGdataDsc* stDatRaw    = new TAGdataDsc("stDat", new TASTdatRaw());
   stActDatRaw  = new TASTactDatRaw("stActDatRaw", stDatRaw,bmDaq,stMap, fpParTimeSt);
   
   TAGdataDsc* bmDatRaw    = new TAGdataDsc("bmDat", new TABMdatRaw());
   bmActDatRaw  = new TABMactDatRaw("bmActDatRaw", bmDatRaw, bmDaq, bmMap, bmConf, bmGeo,stDatRaw);

   TAGdataDsc* bmNtuRaw    = new TAGdataDsc("bmNtuRaw", new TABMntuRaw());
   bmActNtuRaw  = new TABMactNtuRaw("bmActNtuRaw", bmNtuRaw, bmDatRaw, stDatRaw, bmGeo, bmConf);
   bmActNtuRaw->CreateHistogram();   
   
   TAGdataDsc* bmTrack = new TAGdataDsc("bmTrack", new TABMntuTrack());
   bmActTrack  = new TABMactNtuTrack("bmActTrack", bmTrack, bmNtuRaw, bmGeo, bmConf, tgGeo);
   bmActTrack->CreateHistogram();

   cout<<"end of FillBm"<<endl;

   outFile->SetupElementBranch(bmDatRaw, TABMdatRaw::GetBranchName());
   outFile->SetupElementBranch(bmNtuRaw, TABMntuRaw::GetBranchName());
   outFile->SetupElementBranch(bmTrack, TABMntuTrack::GetBranchName());
}

//~ void ReadBmRaw(TString name = "data_test.00001462.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data")
//~ void ReadBmRaw(TString name = "data/GSI_electronic/DataGSI_match/data_built.2212.physics_foot.daq.VTX.1.dat")
void ReadBmRaw(TString name = "data/GSI_electronic/DataGSI_match/data_built.2242.physics_foot.daq.VTX.1.dat")
{  
   Int_t maxevents=200;
   TString fExpName="";
   GlobalPar::Instance();
   GlobalPar::GetPar()->Print();

   TAGroot tagr;
   TAGgeoTrafo geoTrafo;
   geoTrafo.FromFile();

   tagr.SetCampaignNumber(1);
   tagr.SetRunNumber(1);

   outFile = new TAGactTreeWriter("outFile");
   FillBm(fExpName);
   daqActReader->Open(name);
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(stActDatRaw);
   tagr.AddRequiredItem(bmActDatRaw);
   tagr.AddRequiredItem(bmActNtuRaw);
   tagr.AddRequiredItem(bmActTrack);
   tagr.AddRequiredItem(outFile);
   tagr.Print();
   
   TString nameOut = name(name.Last('/')+1, name.Last('.'));
   nameOut.Append("_readbmraw_Out.root");
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   bmActDatRaw->SetHistogramDir(outFile->File());
   bmActNtuRaw->SetHistogramDir(outFile->File());
   bmActTrack->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   TABMdatRaw* pbmdatraw;
   
   for (Long64_t ientry = 0; ientry < maxevents; ientry++) {
     cout<<" Loaded Event:: " <<std::dec<< ientry << endl;
     if(!tagr.NextEvent()) 
       break; 
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();
}


