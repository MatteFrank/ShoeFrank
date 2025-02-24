
// Exdcutable to read back M28 PXi
// DecodeMPix -in ./128022 -out toto.root -nev 2000 -exp PXI -run 1
// Ch. Finck, July 23.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TApplication.h>

#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGrecoManager.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparMap.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"

#include "TAPXIactReader.hxx"
#include "TAVTactNtuCluster.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuTrackF.hxx"

#endif

// main
TAGcampaignManager* campManager  = 0x0;
TAPXIactReader*     daqActReader = 0x0;
TAGactTreeWriter*   outFile      = 0x0;

TAVTactNtuCluster*  vtActClus    = 0x0;
TAVTactNtuTrackF*   vtActTrck    = 0x0;

void FillMonopix(Int_t runNumber, Bool_t treeFlag = true, Bool_t trackFlag = false)
{
   TAGparaDsc* vtMap    = new TAGparaDsc("vtMap", new TAVTparMap());
   TAVTparMap* map   = (TAVTparMap*) vtMap->Object();
   TString parFileName = campManager->GetCurMapFile(TAVTparGeo::GetBaseName(), runNumber);
   map->FromFile(parFileName.Data());

   TAGparaDsc* vtGeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());
   TAVTparGeo* geomap   = (TAVTparGeo*) vtGeo->Object();
   parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   Int_t sensorsN = geomap->GetSensorsN();

   TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
   TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
   parFileName = campManager->GetCurConfFile(TAVTparGeo::GetBaseName(), runNumber);
   parconf->FromFile(parFileName.Data());

   TAGdataDsc* vtNtu    = new TAGdataDsc(new TAVTntuHit(sensorsN));
   TAGdataDsc* vtClus   = new TAGdataDsc(new TAVTntuCluster(sensorsN));
   TAGdataDsc* vtTrck   = new TAGdataDsc(new TAVTntuTrack());

   daqActReader  = new TAPXIactReader("pxiFile", vtNtu, vtGeo, vtConf);
   daqActReader->CreateHistogram();

   vtActClus =  new TAVTactNtuCluster("vtActClus", vtNtu, vtClus, vtConf, vtGeo);
   vtActClus->CreateHistogram();

   vtActTrck = new TAVTactNtuTrackF("vtActTrck", vtClus, vtTrck, vtConf, vtGeo);
   vtActTrck->CreateHistogram();
   
   if (treeFlag) {
      outFile->SetupElementBranch(vtNtu, "vtrh.");
      outFile->SetupElementBranch(vtClus, "vtclus.");
      if (trackFlag)
         outFile->SetupElementBranch(vtTrck, "vttrack.");
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
         cout<<" DecodePix help:"<<endl;
         cout<<" Ex: DecodePix -in ./128022 -out run128022.root -nev 20000 -exp PXI -run 1  "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"      -in path/file  : [def=""] raw input file"<<endl;
         cout<<"      -out path/file : [def=*_Out.root] Root output file"<<endl;
         cout<<"      -nev value     : [def=10^7] Numbers of events to process"<<endl;
         cout<<"      -run value     : [def=-1] Run number"<<endl;
         cout<<"      -exp name      : [def=""] experient name for config/geomap extention"<<endl;
         return 1;
      }
   }
   
   Int_t frequency = 1;
   
   if (nTotEv >= 100000)      frequency = 10000;
   else if (nTotEv >= 10000)  frequency = 1000;
   else if (nTotEv >= 1000)   frequency = 100;
   else if (nTotEv >= 100)    frequency = 10;
   else if (nTotEv >= 10)     frequency = 1;
   
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
   tagr.AddRequiredItem(vtActClus);
   if (trackFlag)
      tagr.AddRequiredItem(vtActTrck);
   tagr.AddRequiredItem(outFile);

   tagr.Print();
   
   TString outFileName = out;

   if (outFile->Open(outFileName.Data(), "RECREATE")) return 0;
   daqActReader->SetHistogramDir(outFile->File());
   vtActClus->SetHistogramDir(outFile->File());
   if (trackFlag)
      vtActTrck->SetHistogramDir(outFile->File());

   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Int_t nEvents = 0;
   while (tagr.NextEvent() ){

      if (++nEvents == nTotEv)
         break;
      
      if (nEvents % frequency == 0)
         printf("Event: %d\n", nEvents);
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();

   return 0;

}


