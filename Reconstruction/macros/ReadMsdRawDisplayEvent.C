
// Macro to reconstruct from raw data
// Ch. Finck, sept 11.

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include "TH2.h"
#include "TCanvas.h"
#include "TGraph.h"
#include <stdlib.h> //for using the function sleep

#include "TAGgeoTrafo.hxx"
#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGrecoManager.hxx"
#include "TAGdaqEvent.hxx"

#include "TAMSDparMap.hxx"
#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAGactDaqReader.hxx"
#include "TAMSDactNtuRaw.hxx"
#include "TAMSDactNtuHit.hxx"
#include "TAMSDactNtuCluster.hxx"
#include "TAMSDactNtuPoint.hxx"

#endif

// main
TAGcampaignManager *campManager = 0x0;
TAGactTreeWriter *outFile = 0x0;
TAGactDaqReader *daqActReaderMSD = 0x0;

TAMSDactNtuRaw *msdActRaw = 0x0;

TAGdaqEvent *rawevent = 0x0;

void FillDetectors(Int_t runNumber)
{
   //###################### FILLING MSD #################################
   TAGparaDsc *msdMap = new TAGparaDsc("msdMap", new TAMSDparMap());
   TAMSDparMap *map = (TAMSDparMap *)msdMap->Object();
   TString parFileName = campManager->GetCurMapFile(TAMSDparGeo::GetBaseName(), runNumber);
   map->FromFile(parFileName.Data());

   TAGparaDsc *msdGeo = new TAGparaDsc("msdGeo", new TAMSDparGeo());
   TAMSDparGeo *geomap = (TAMSDparGeo *)msdGeo->Object();
   parFileName = campManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());

   TAGparaDsc *msdConf = new TAGparaDsc("msdConf", new TAMSDparConf());
   TAMSDparConf *confmap = (TAMSDparConf *)msdConf->Object();
   parFileName = campManager->GetCurConfFile(TAMSDparGeo::GetBaseName(), runNumber);
   confmap->FromFile(parFileName.Data());

   TAGparaDsc *msdCal = new TAGparaDsc("msdCal", new TAMSDparCal(geomap->GetStripsN()));
   TAMSDparCal *parCalMsd = (TAMSDparCal *)msdCal->Object();
   parFileName = campManager->GetCurCalFile(TAMSDparGeo::GetBaseName(), runNumber, true);
   parCalMsd->LoadEnergyCalibrationMap(parFileName.Data());

   parFileName = campManager->GetCurCalFile(TAMSDparGeo::GetBaseName(), runNumber, false);
   parCalMsd->LoadPedestalMap(parFileName.Data());

   TAGdataDsc *msdDaq = new TAGdataDsc("msdDaq", new TAGdaqEvent());
   TAGdataDsc *msdDat = new TAGdataDsc("msdDat", new TAMSDntuRaw());

   daqActReaderMSD = new TAGactDaqReader("daqActReader", msdDaq);

   msdActRaw = new TAMSDactNtuRaw("msdActRaw", msdDat, msdDaq, msdMap, msdCal, msdGeo, msdConf);
   msdActRaw->CreateHistogram();
}

void ReadMsdRawDisplayEvent(TString filename = "data/data_test.00003890.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data", Int_t nMaxEmsds = 1000,
                     TString expName = "GSI2021", Int_t runNumber = 1, Int_t fileNumber = 1)
{
   TAGrecoManager::Instance(expName);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();

   TAGroot tagr;

   campManager = new TAGcampaignManager(expName);
   campManager->FromFile();

   TAGgeoTrafo *geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
   geoTrafo->FromFile(parFileName);

   TAGparaDsc *msdGeo = new TAGparaDsc("msdGeo", new TAMSDparGeo());
   TAMSDparGeo *geomap = (TAMSDparGeo *)msdGeo->Object();
   parFileName = campManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), fileNumber);
   geomap->FromFile(parFileName.Data());

   TAGparaDsc *msdCal = new TAGparaDsc("msdCal", new TAMSDparCal(geomap->GetStripsN()));
   TAMSDparCal *parCalMsd = (TAMSDparCal *)msdCal->Object();
   parFileName = campManager->GetCurCalFile(TAMSDparGeo::GetBaseName(), runNumber, true);
   parCalMsd->LoadEnergyCalibrationMap(parFileName.Data());
   parFileName = campManager->GetCurCalFile(TAMSDparGeo::GetBaseName(), runNumber, false);
   parCalMsd->LoadPedestalMap(parFileName.Data());

   TAGparaDsc *msdMap = new TAGparaDsc("msdMap", new TAMSDparMap());
   TAMSDparMap *map = (TAMSDparMap *)msdMap->Object();
   parFileName = campManager->GetCurMapFile(TAMSDparGeo::GetBaseName(), runNumber);
   map->FromFile(parFileName.Data());

   FillDetectors(runNumber);
   daqActReaderMSD->Open(filename);

   tagr.AddRequiredItem(daqActReaderMSD);
   tagr.AddRequiredItem(msdActRaw);

   tagr.Print();

   cout << " Beginning the Event Loop " << endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();

   TCanvas *c1 = new TCanvas("c1", "Raw Event FOOT", 1000, 500);
   c1->SetGrid();

   TGraph *gr = new TGraph(640);
   gr->GetXaxis()->SetTitle("channel");
   TAxis *axis = gr->GetXaxis();
   axis->SetLimits(0, 640);
   axis->SetNdivisions(10, false);

   Int_t nEvents = 0;
   int sensorId;

   while (tagr.NextEvent())
   {

      rawevent = (TAGdaqEvent *)(tagr.FindDataDsc("msdDaq", "TAGdaqEvent")->Object());
      Int_t nFragments = rawevent->GetFragmentsN();

      cout << "Reading " << nFragments << " fragments" << endl;

      for (Int_t i = 0; i < nFragments; ++i)
      {
         TString type = rawevent->GetClassType(i);
         if (type.Contains("DEMSDEvent"))
         {
            const DEMSDEvent *evt = static_cast<const DEMSDEvent *>(rawevent->GetFragment(i));
            gr->Clear();
            gr->SetName(Form("Rawdata_board-%x_detector-%s", evt->boardHeader, "X"));

            sensorId = map->GetSensorId((evt->boardHeader & 0xF) - 1, 0);
            cout << "Board header " << hex << (evt->boardHeader & 0xF) -1 << dec << " sensorId: " << map->GetSensorId((evt->boardHeader & 0xF) - 1, 0) << endl;

            for (int i = 0; i < 640; i++)
            {
               //cout << "\tStrip: " << i << " RAW ADC: " << evt->Xplane[i] << " Pedestal: " << parCalMsd->GetPedestal(sensorId, i).mean << endl;
               gr->SetPoint(i, i, evt->Xplane[i] - parCalMsd->GetPedestal(sensorId, i).mean);
               //gr->SetPoint(i, i, evt->Xplane[i]);
               //gr->SetPoint(i, i, parCalMsd->GetPedestal(sensorId,i).mean);
            }
            gr->GetXaxis()->SetLimits(0, 640);
            gr->Draw("AL*");
            gPad->Update();
            c1->Update();
            sleep(1);

            gr->SetName(Form("Rawdata_board-%x_detector-%s", evt->boardHeader, "Y"));
            sensorId = map->GetSensorId((evt->boardHeader & 0xF) - 1, 1);
            cout << "Board header " << hex << (evt->boardHeader & 0xF) -1 << dec << " sensorId: " << map->GetSensorId((evt->boardHeader & 0xF) - 1, 1) << endl;

            for (int i = 0; i < 640; i++)
            {
               gr->SetPoint(i, i, evt->Yplane[i] - parCalMsd->GetPedestal(sensorId, i).mean);
            }
            gr->GetXaxis()->SetLimits(0, 640);
            gr->Draw("AL*");
            gPad->Update();
            c1->Update();
            sleep(1);
         }
      }

      // printf("\n");
      if (++nEvents % 100 == 0)
         printf("Event: %d\n", nEvents);

      if (nEvents == nMaxEmsds)
         break;
   }
   tagr.EndEventLoop();

   watch.Print();
}
