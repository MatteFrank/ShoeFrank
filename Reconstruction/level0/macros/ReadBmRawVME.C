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

#include "TAGparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TABMparMap.hxx"
#include "TABMparCon.hxx"
#include "TASTdatRaw.hxx"
#include "TABMdatRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TABMactNtuRaw.hxx"
#include "TABMactVmeReader.hxx"
#include "TABMactNtuTrack.hxx"

#include "GlobalPar.hxx"

#endif

// main
TAGactTreeWriter* outFile   = 0x0;
TABMactVmeReader* bmActVmeReader  = 0x0;
TABMactNtuRaw* bmActNtuRaw  = 0x0;
TABMactNtuTrack* bmActTrack = 0x0;

void FillBmVME(TString name) {
   
   cout<<"start FillBmVME"<<endl;
   
   TAGparaDsc* bmGeo    = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
   TABMparGeo* geomap   = (TABMparGeo*) bmGeo->Object();
   TString parFileName = "./geomaps/TABMdetector.map";
   geomap->FromFile(parFileName.Data());
            
   TAGparaDsc* tgGeo = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
   TAGparGeo* parGeo = (TAGparGeo*)tgGeo->Object();
   parGeo->FromFile();

   TAGparaDsc*  bmConf  = new TAGparaDsc("bmConf", new TABMparCon());
   TABMparCon* parConf = (TABMparCon*)bmConf->Object();
   parFileName = "./config/TABMdetector.cfg";
   parConf->FromFile(parFileName.Data());
   parFileName = "./config/trento_marzo.cfg";
   parConf->loadT0s(parFileName);
   parConf->CoutT0();

   TAGparaDsc*  bmMap  = new TAGparaDsc("bmMap", new TABMparMap());
   TABMparMap*  parMap = (TABMparMap*)bmMap->Object();
   parFileName = "./config/TABMdetector.map";
   parMap->FromFile(parFileName.Data(), geomap);

  
   TAGdataDsc* stDatRaw    = new TAGdataDsc("stDatRaw", new TASTdatRaw());
   TAGdataDsc* bmDatRaw    = new TAGdataDsc("bmDatRaw", new TABMdatRaw());
   bmActVmeReader  = new TABMactVmeReader("bmActVmeReader", bmDatRaw, bmMap, bmConf, bmGeo,stDatRaw);
   bmActVmeReader->Open(name);


   TAGdataDsc* bmNtuRaw    = new TAGdataDsc("bmNtuRaw", new TABMntuRaw());
   bmActNtuRaw  = new TABMactNtuRaw("bmActNtuRaw", bmNtuRaw, bmDatRaw, bmGeo, bmConf);
   bmActNtuRaw->CreateHistogram();   
   
   TAGdataDsc* bmTrack = new TAGdataDsc("bmTrack", new TABMntuTrack());
   bmActTrack  = new TABMactNtuTrack("bmActTrack", bmTrack, bmNtuRaw, bmGeo, bmConf, tgGeo);
   bmActTrack->CreateHistogram();

   cout<<"end of FillBmVME"<<endl;

   outFile->SetupElementBranch(bmNtuRaw, TABMntuRaw::GetBranchName());
   outFile->SetupElementBranch(bmTrack, TABMntuTrack::GetBranchName());
}

void ReadBmRawVME(TString name = "./data/msd_marzo/bmdata/6_March_2019/80MeV_HV2175_100kEv.dat")
{  
  Int_t maxevt=1000;
  GlobalPar::Instance();
  GlobalPar::GetPar()->Print();

  TAGroot tagr;
  TAGgeoTrafo geoTrafo;
  geoTrafo.FromFile();

  tagr.SetCampaignNumber(-1);
  tagr.SetRunNumber(1);

  outFile = new TAGactTreeWriter("outFile");
  FillBmVME(name);
  
  tagr.AddRequiredItem("bmActVmeReader");
  tagr.AddRequiredItem("bmActNtuRaw");
  tagr.AddRequiredItem("bmActTrack");
  tagr.AddRequiredItem("outFile");
  tagr.Print();
  
  TString nameOut = name(name.Last('/')+1, name.Last('.'));
  nameOut.Append("_readbmrawvme_Out.root");
  
  if (outFile->Open(nameOut.Data(), "RECREATE")) return;
  bmActTrack->SetHistogramDir(outFile->File());
  bmActNtuRaw->SetHistogramDir(outFile->File());
  
  cout<<" Beginning the Event Loop "<<endl;
  tagr.BeginEventLoop();
  TStopwatch watch;
  watch.Start();
  
  for (Long64_t ientry = 0; ientry < maxevt; ientry++) {
    if(ientry % 100 == 0)
      cout<<" Loaded Event:: " << ientry << endl;
    if(!tagr.NextEvent())
      break;
  }
  
  tagr.EndEventLoop();
  
  outFile->Print();
  outFile->Close();
  bmActVmeReader->Close();
  
  watch.Print();
  cout<<"job done, the output file is: "<<nameOut.Data()<<endl;
}


