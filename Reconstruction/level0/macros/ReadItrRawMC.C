
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
#include "TAITntuHit.hxx"
#include "TAITntuCluster.hxx"
#include "TAITntuTrack.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"

#include "TAGcampaignManager.hxx"
#include "TAGactTreeReader.hxx"
#include "TAITactNtuHitMC.hxx"

#include "TAITactNtuClusterF.hxx"
#include "TAITactNtuTrackF.hxx"

#endif

// main
TAGcampaignManager* campManager = 0x0;
TAGactTreeWriter*   outFile     = 0x0;
TAGactTreeReader*   itActReader = 0x0;
TAITactNtuHitMC*    itActRaw    = 0x0;
TAITactNtuClusterF* itActClus   = 0x0;
TAITactNtuTrackF*   itActTrck   = 0x0;

void FillInnerTracker(Int_t runNumber)
{
   TAGparaDsc* tgGeo = new TAGparaDsc("tgGeo", new TAGparGeo());
   TAGparGeo* geomapg = (TAGparGeo*)tgGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(TAGparGeo::GetBaseName(), runNumber);
   geomapg->FromFile(parFileName.Data());

   TAGparaDsc* itGeo    = new TAGparaDsc("itGeo", new TAITparGeo());
   TAITparGeo* geomap   = (TAITparGeo*) itGeo->Object();
   parFileName = campManager->GetCurGeoFile(TAITparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   
   TAGparaDsc*  itConf  = new TAGparaDsc("itConf", new TAITparConf());
   TAITparConf* parconf = (TAITparConf*) itConf->Object();
   parFileName = campManager->GetCurConfFile(TAITparGeo::GetBaseName(), runNumber);
   parconf->FromFile(parFileName.Data());

   TAITparConf::SetHistoMap();
   TAGdataDsc* itEve  = new TAGdataDsc("itEve", new TAMCntuPart());
   TAGdataDsc* itMc   = new TAGdataDsc("itMc", new TAMCntuHit());
   TAGdataDsc* itNtu  = new TAGdataDsc("itNtu", new TAITntuHit());
   TAGdataDsc* itClus = new TAGdataDsc("itClus", new TAITntuCluster());
   TAGdataDsc* itTrck = new TAGdataDsc("itTrck", new TAITntuTrack());

   itActReader  = new TAGactTreeReader("itActEvtReader");
   itActReader->SetupBranch(itMc, TAMCntuHit::GetItrBranchName());
   itActReader->SetupBranch(itEve,TAMCntuPart::GetBranchName());

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

void ReadItrRawMC(TString filename = "12C_C_200_ntu.root", Int_t nMaxEvts = 0,
                  TString expName = "12C_200", Int_t runNumber = 1)
{
   GlobalPar::Instance(expName);
   GlobalPar::GetPar()->FromFile();
   GlobalPar::GetPar()->Print();
   
   TAGroot tagr;
   
   campManager = new TAGcampaignManager(expName);
   campManager->FromFile();
   
   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
   geoTrafo->FromFile(parFileName);
   
   outFile = new TAGactTreeWriter("outFile");

   FillInnerTracker(runNumber);
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


