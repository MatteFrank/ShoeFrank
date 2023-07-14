
// Macro to reconstruct from raw data
// Ch. Finck, sept 11.


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
#include "TAGcampaignManager.hxx"

#include "TAMPparMap.hxx"
#include "TAMPparGeo.hxx"
#include "TAMPparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"

#include "TAMPactAscReader.hxx"

#include "TAVTactNtuCluster.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAGrecoManager.hxx"

#endif

// main
TAGcampaignManager* campManager  = 0x0;
TAGactTreeWriter*   outFile      = 0x0;
TAMPactAscReader*    daqActReader = 0x0;

TAVTactNtuCluster* mpActClus = 0x0;
TAVTactNtuTrackF*   mpActTrck = 0x0;

void FillMonopix(Int_t runNumber)
{
   TAGparaDsc* mpMap    = new TAGparaDsc("mpMap", new TAMPparMap());
   TAMPparMap* map   = (TAMPparMap*) mpMap->Object();
   TString parFileName = campManager->GetCurMapFile(TAMPparGeo::GetBaseName(), runNumber);
   map->FromFile(parFileName.Data());

   TAGparaDsc* mpGeo    = new TAGparaDsc("mpGeo", new TAMPparGeo());
   TAMPparGeo* geomap   = (TAMPparGeo*) mpGeo->Object();
   parFileName = campManager->GetCurGeoFile(TAMPparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   Int_t sensorsN = geomap->GetSensorsN();

   TAGparaDsc*  mpConf  = new TAGparaDsc("mpConf", new TAMPparConf());
   TAMPparConf* parconf = (TAMPparConf*) mpConf->Object();
   parFileName = campManager->GetCurConfFile(TAMPparGeo::GetBaseName(), runNumber);
   parconf->FromFile(parFileName.Data());

   TAGdataDsc* mpNtu    = new TAGdataDsc(new TAVTntuHit(sensorsN));
   TAGdataDsc* mpClus   = new TAGdataDsc(new TAVTntuCluster(sensorsN));
   TAGdataDsc* mpTrck   = new TAGdataDsc(new TAVTntuTrack());

   daqActReader  = new TAMPactAscReader("daqActReader", mpNtu, mpGeo, mpConf, mpMap);
   daqActReader->CreateHistogram();


   mpActClus =  new TAVTactNtuCluster("mpActClus", mpNtu, mpClus, mpConf, mpGeo);
   mpActClus->CreateHistogram();

   mpActTrck = new TAVTactNtuTrackF("mpActTrck", mpClus, mpTrck, mpConf, mpGeo);
   mpActTrck->CreateHistogram();
   
//   outFile->SetupElementBranch(mpNtu, "mprh.");
//   outFile->SetupElementBranch(mpClus, "mpclus.");
//   outFile->SetupElementBranch(mpTrck, "mptrack.");

}

void ReadMPix(TString filename = "dataRaw/run000675.txt", Int_t nMaxEvts = 2000, TString expName = "MPIX2023", Int_t runNumber = 1)
{
   TAGrecoManager::Instance(expName);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   TAGroot tagr;
   
   campManager = new TAGcampaignManager(expName);
   campManager->FromFile();

   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
   geoTrafo->FromFile(parFileName);
   
   outFile = new TAGactTreeWriter("outFile");

   FillMonopix(runNumber);
   daqActReader->Open(filename);
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(mpActClus);
   //tagr.AddRequiredItem(mpActTrck);
   tagr.AddRequiredItem(outFile);

   tagr.Print();
   
   Int_t posi = filename.First('/');
   Int_t posf = filename.Last('.');
   TString outFileName = filename(posi+1, posf-posi-1);
   outFileName.Append(".root");
   
   if (outFile->Open(outFileName.Data(), "RECREATE")) return;
   daqActReader->SetHistogramDir(outFile->File());
   mpActClus->SetHistogramDir(outFile->File());
   mpActTrck->SetHistogramDir(outFile->File());

   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Int_t nEvents = 0;
   while (tagr.NextEvent() ){

      if (nEvents == nMaxEvts)
         break;
      if (++nEvents % 100 == 0)
		printf("Event: %d\n", nEvents); 
   
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();

}


