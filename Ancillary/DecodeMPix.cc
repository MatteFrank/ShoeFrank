
// Exdcutable to read back Monopix2
// DecodeMPix -in dataRaw/run000675.txt -out toto.root -nev 2000 -exp MPIX2023 -run 1
// Ch. Finck, July 23.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TApplication.h>

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
TAMPactAscReader*   daqActReader = 0x0;

TAVTactNtuCluster*  mpActClus    = 0x0;
TAVTactNtuTrackF*   mpActTrck    = 0x0;

void FillMonopix(Int_t runNumber, Bool_t treeFlag = true, Bool_t trackFlag = false)
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
   
   if (treeFlag) {
      outFile->SetupElementBranch(mpNtu, "mprh.");
      outFile->SetupElementBranch(mpClus, "mpclus.");
      if (trackFlag)
         outFile->SetupElementBranch(mpTrck, "mptrack.");
   }
}

int main (int argc, char *argv[])  {
   
   TString in("");
   TString out("");
   TString exp("");
   
   Int_t runNb = -1;
   Int_t nTotEv = 1e7;
   
   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-out") == 0)   { out =TString(argv[++i]);  }   // Raw file name for output
      if(strcmp(argv[i],"-in") == 0)    { in = TString(argv[++i]);  }   // Root file in input
      if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
      if(strcmp(argv[i],"-nev") == 0)   { nTotEv = atoi(argv[++i]); }   // Number of events to be analized
      if(strcmp(argv[i],"-run") == 0)   { runNb = atoi(argv[++i]);  }   // Run Number

      if(strcmp(argv[i],"-help") == 0)  {
         cout<<" DecodeMPix help:"<<endl;
         cout<<" Ex: DecodeMPix -in dataRaw/run000675.txt -out toto.root -nev 2000 -exp MPIX2023 -run 1  "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] raw input file"<<endl;
         cout<<"      -out path/file : [def=*_Out.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
         cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
         return 1;
      }
   }
   
   TApplication::CreateApplication();
   TAGrecoManager::Instance(exp);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();
   
   // saving in tree
   Bool_t treeFlag = TAGrecoManager::GetPar()->IsSaveTree();

   // saving in tree
   Bool_t trackFlag = TAGrecoManager::GetPar()->IsTracking();
   
   TAGroot tagr;
   
   campManager = new TAGcampaignManager(exp);
   campManager->FromFile();

   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNb);
   geoTrafo->FromFile(parFileName);
   
   outFile = new TAGactTreeWriter("outFile");

   FillMonopix(runNb, treeFlag, trackFlag);
   daqActReader->Open(in);
   
   tagr.AddRequiredItem(daqActReader);
   tagr.AddRequiredItem(mpActClus);
   if (trackFlag)
      tagr.AddRequiredItem(mpActTrck);
   tagr.AddRequiredItem(outFile);

   tagr.Print();
   
   TString outFileName = out;

   if (outFile->Open(outFileName.Data(), "RECREATE")) return 0;
   daqActReader->SetHistogramDir(outFile->File());
   mpActClus->SetHistogramDir(outFile->File());
   if (trackFlag)
      mpActTrck->SetHistogramDir(outFile->File());

   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Int_t nEvents = 0;
   while (tagr.NextEvent() ){

      if (nEvents == nTotEv)
         break;
      if (++nEvents % 100 == 0)
		printf("Event: %d\n", nEvents); 
   
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();

   return 0;

}


