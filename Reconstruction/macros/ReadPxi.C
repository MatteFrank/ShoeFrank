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

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"

#include "TAPXIactReader.hxx"
#include "TAVTactNtuCluster.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuTrackF.hxx"

#endif

// main
TAGactTreeWriter* outFile   = 0x0;
TAPXIactReader* pxiFile   = 0x0;
TAVTactNtuCluster* vtActClus = 0x0;
TAVTactNtuTrackF*    vtActTrck = 0x0;

void FillVertex() {
   
   TAGdataDsc* vtNtu    = new TAGdataDsc("vtNtu", new TAVTntuHit());
   TAGdataDsc* vtClus   = new TAGdataDsc("vtClus", new TAVTntuCluster());
   TAGdataDsc* vtTrck   = new TAGdataDsc("vtTrck", new TAVTntuTrack());

   TAGparaDsc* vtGeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());
   TAVTparGeo* geomap   = (TAVTparGeo*) vtGeo->Object();
   geomap->FromFile("./geomaps/PXI/TAVTdetector.geo");
   
   TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
   TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
   parconf->FromFile("./config/PXI/TAVTdetector.cfg");
   
   vtActClus =  new TAVTactNtuCluster("vtActClus", vtNtu, vtClus, vtConf, vtGeo);
   vtActClus->CreateHistogram();
   
   vtActTrck = new TAVTactNtuTrackF("vtActTrck", vtClus, vtTrck, vtConf, vtGeo);
   vtActTrck->CreateHistogram();
   
   pxiFile = new TAPXIactReader("pxiFile", vtNtu, vtGeo, vtConf);
   
   outFile->SetupElementBranch(vtClus, "vtclus.");
   outFile->SetupElementBranch(vtTrck, "vtTrack.");
}

void ReadPxi(TString name = "./128022") 
{

   TAGroot tagr;
   tagr.SetRunNumber(1);

   outFile = new TAGactTreeWriter("outFile");

   FillVertex();
   char path[100];
   strcpy(path, name.Data());
   pxiFile->Open(path);
   
   
   tagr.AddRequiredItem(vtActClus);
   tagr.AddRequiredItem(vtActTrck);
   tagr.AddRequiredItem(outFile);
   
   tagr.Print();
   
   if (outFile->Open("Pxi_EvtVT_Out.root", "RECREATE")) return;
   vtActClus->SetHistogramDir(outFile->File());
   vtActTrck->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Int_t nEvents = 0;
   while (tagr.NextEvent() ){

     if (++nEvents % 100 == 0) {
		printf("Event: %d\n", nEvents); 
     }
	  
	  if (nEvents == 5000) {
		 break;
	  }
   }
   
   tagr.EndEventLoop();
 
   outFile->Print();
   outFile->Close();
   
   watch.Print();

}


