
// Macro to reconstruct from G4 MC hits
// Ch. Finck, Juky 19.


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

#include "TAGparGeo.hxx"

#include "TAITparMap.hxx"
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITntuRaw.hxx"
#include "TAITntuCluster.hxx"
#include "TAITntuTrack.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"

#include "TAGactTreeReader.hxx"
#include "TAITactNtuHitMC.hxx"

#include "TAITactNtuClusterF.hxx"
#include "TAITactNtuTrackF.hxx"

#endif

// main
TAGactTreeWriter*   outFile     = 0x0;
TAGactTreeReader*   itActReader = 0x0;
TAITactNtuHitMC*    itActRaw    = 0x0;
TAITactNtuClusterF* itActClus   = 0x0;
TAITactNtuTrackF*   itActTrck   = 0x0;

void FillInnerTracker()
{
   TAGparaDsc* tgGeo = new TAGparaDsc("tgGeo", new TAGparGeo());
   TAGparGeo* geomapg = (TAGparGeo*)tgGeo->Object();
   geomapg->FromFile("./geomaps/TAGdetector.geo");

   TAGparaDsc* itGeo    = new TAGparaDsc("itGeo", new TAITparGeo());
   TAITparGeo* geomap   = (TAITparGeo*) itGeo->Object();
   geomap->FromFile("./geomaps/TAITdetector.geo");
   
   TAGparaDsc*  itConf  = new TAGparaDsc("itConf", new TAITparConf());
   TAITparConf* parconf = (TAITparConf*) itConf->Object();
   parconf->FromFile("./config/TAITdetector.cfg");

   TAITparConf::SetHistoMap();
   TAGdataDsc* itEve  = new TAGdataDsc("itEve", new TAMCntuEve());
   TAGdataDsc* itMc   = new TAGdataDsc("itMc", new TAMCntuHit());
   TAGdataDsc* itNtu  = new TAGdataDsc("itNtu", new TAITntuRaw());
   TAGdataDsc* itClus = new TAGdataDsc("itClus", new TAITntuCluster());
   TAGdataDsc* itTrck = new TAGdataDsc("itTrck", new TAITntuTrack());

   itActReader  = new TAGactTreeReader("itActEvtReader");
   itActReader->SetupBranch(itMc, TAMCntuHit::GetItrBranchName());
   itActReader->SetupBranch(itEve,TAMCntuEve::GetBranchName());

   itActRaw= new TAITactNtuHitMC("itActNtu", itMc, itEve, itNtu, itGeo);
   itActRaw->CreateHistogram();

   itActClus =  new TAITactNtuClusterF("itActClus", itNtu, itClus, itConf, itGeo);
   itActClus->CreateHistogram();

   itActTrck = new TAITactNtuTrackF("itActTrck", itClus, itTrck, itConf, itGeo, 0x0, tgGeo);
   itActTrck->CreateHistogram();
   
//   outFile->SetupElementBranch(itNtu, "itrh.");
//   outFile->SetupElementBranch(itClus, "itclus.");
//   outFile->SetupElementBranch(itTrck, "ittrack.");

}

void ReadItrRawG4(TString filename = "12C_C_200_ntu.root", Int_t nMaxEvts = 0)
{
   TAGroot tagr;
   tagr.SetCampaignNumber(100);
   tagr.SetRunNumber(1);
   
   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   geoTrafo->FromFile();
   
   outFile = new TAGactTreeWriter("outFile");

   FillInnerTracker();
   itActReader->Open(filename, "READ", "EventTree");
   
   tagr.AddRequiredItem(itActReader);
   tagr.AddRequiredItem(itActRaw);
   tagr.AddRequiredItem(itActClus);
   tagr.AddRequiredItem(itActTrck);
   tagr.AddRequiredItem(outFile);

   tagr.Print();
   
   Int_t pos = filename.Last('.');
   
   TString outFileName = filename(0,pos);
   outFileName.Append("_Out.root");

   if (outFile->Open(outFileName.Data(), "RECREATE")) return;
   itActRaw->SetHistogramDir(outFile->File());
   itActClus->SetHistogramDir(outFile->File());
   itActTrck->SetHistogramDir(outFile->File());

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


