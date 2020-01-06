
// Macro to read back local reco tree and make global reconstruction
// Ch. Finck, July 19.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>

#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"

#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TADIparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"

#include "TAVTntuVertex.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TATWntuPoint.hxx"

#include "TAGntuGlbTrack.hxx"
#include "TAGactNtuGlbTrack.hxx"

#endif

// main
TAGactTreeWriter*   outFile   = 0x0;
TAGactNtuGlbTrack*  actGlbTrack = 0x0;

void FillGlb(TString expName)
{
   TAGactNtuGlbTrack::EnableMacStdAlone();
   
   TAGparaDsc* pParGeoDi = new TAGparaDsc(TADIparGeo::GetDefParaName(), new TADIparGeo());
   TADIparGeo* parGeoDi = (TADIparGeo*)pParGeoDi->Object();
   TString parFileName = "./geomaps/TADIdetector.map";
   parGeoDi->FromFile(parFileName.Data());
   
   TAGparaDsc* pParGeoVtx = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
   TAVTparGeo* parGeoVtx = (TAVTparGeo*)pParGeoVtx->Object();
   TString parVtxFileName = Form("./geomaps/%sTAVTdetector.map", expName.Data());
   parGeoVtx->FromFile(parVtxFileName.Data());

   TAGparaDsc* pParGeoIt = new TAGparaDsc(TAITparGeo::GetItDefParaName(), new TAITparGeo());
   TAITparGeo* parGeoIt = (TAITparGeo*)pParGeoIt->Object();
   TString parItFileName = Form("./geomaps/%sTAITdetector.map", expName.Data());
   parGeoIt->FromFile(parItFileName.Data());
   
   TAGparaDsc* pParGeoMsd = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
   TAMSDparGeo* parGeoMsd = (TAMSDparGeo*)pParGeoMsd->Object();
   TString parMsdFileName = Form("./geomaps/%sTAMSDdetector.map", expName.Data());
   parGeoMsd->FromFile(parMsdFileName.Data());

   TAGparaDsc* pParGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
   TATWparGeo* parGeoTw = (TATWparGeo*)pParGeoTw->Object();
   TString parTwFileName = "./geomaps/TATWdetector.map";
   parGeoTw->FromFile(parTwFileName);
   
   TAGdataDsc* vtVtx   = new TAGdataDsc("vtVertex", new TAVTntuVertex());
   TAGdataDsc* itClus  = new TAGdataDsc("itClus",   new TAITntuCluster());
   TAGdataDsc* msClus  = new TAGdataDsc("msClus",   new TAMSDntuCluster());
   TAGdataDsc* twPoint = new TAGdataDsc("twPoint",  new TATWntuPoint());
   
   TAGdataDsc* glbTrack = new TAGdataDsc("glbTrack", new TAGntuGlbTrack());
   actGlbTrack  = new TAGactNtuGlbTrack("glbActTrack", vtVtx, itClus, msClus, twPoint, glbTrack, pParGeoDi, pParGeoVtx, pParGeoIt, pParGeoMsd, pParGeoTw);
   actGlbTrack->CreateHistogram();
}

void ReadGlbToeStd(TString filename = "run_2211_GSI.root", Int_t nMaxEvts = 500)
{
   TAGroot tagr;
   tagr.SetCampaignNumber(100);
   tagr.SetRunNumber(1);
   
   TString expName = "GSI/";
   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = Form("./geomaps/%sFOOT_geo.map", expName.Data());
   geoTrafo->FromFile(parFileName);
   
   outFile = new TAGactTreeWriter("outFile");

   FillGlb("");
   actGlbTrack->Open(filename);
   
   tagr.AddRequiredItem(actGlbTrack);
   tagr.AddRequiredItem(outFile);

   tagr.Print();
   
   Int_t pos = filename.First(".");
   TString filenameOut(filename(0, pos));
   filenameOut.Append("_Out.root");
   
   if (outFile->Open(filenameOut.Data(), "RECREATE")) return;
   actGlbTrack->SetHistogramDir(outFile->File());

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


