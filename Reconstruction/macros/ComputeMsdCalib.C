#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include "TH2.h"
#include "TGraph.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TError.h"
#include "TAxis.h"

#include "TAGgeoTrafo.hxx"
#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGcampaignManager.hxx"
#include "TAGrecoManager.hxx"
#include "TAGdaqEvent.hxx"
#include "TAGactDaqReader.hxx"

#include "TAMSDparMap.hxx"
#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDactNtuRaw.hxx"
#include "TAMSDactNtuHit.hxx"

#endif

// main
TAGcampaignManager *campManager = 0x0;
TAGactTreeWriter *outFile = 0x0;
TAGactDaqReader *daqActReaderMSD = 0x0;
TAGdaqEvent *rawevent = 0x0;

TAMSDactNtuRaw *msdActRaw = 0x0;
TAMSDactNtuHit *msdActHit = 0x0;

void ComputeMsdCalib(TString filename = "dataRaw/data_test.00003890.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data", Int_t nMaxEmsds = 1000,
                     TString expName = "GSI2021", Int_t runNumber = 1)
{
   gROOT->SetBatch(kTRUE);
   gErrorIgnoreLevel = kWarning;

   //Check if the file exists
   if (gSystem->AccessPathName(filename))
   {
      cout << "File " << filename << " does not exist" << endl;
      return;
   }

   TAGrecoManager::Instance(expName);
   TAGrecoManager::GetPar()->FromFile();
   TAGrecoManager::GetPar()->Print();

   TAGroot tagr;

   campManager = new TAGcampaignManager(expName);
   campManager->FromFile();

   TAGgeoTrafo *geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(FootBaseName("TAGgeoTrafo"), runNumber);
   geoTrafo->FromFile(parFileName);

   // ###################### FILLING MSD #################################
   TAGparaDsc *msdMap = new TAGparaDsc("msdMap", new TAMSDparMap());
   TAMSDparMap *map = (TAMSDparMap *)msdMap->Object();
   parFileName = campManager->GetCurMapFile(FootBaseName("TAMSDparGeo"), runNumber);
   map->FromFile(parFileName.Data());

   TAGparaDsc *msdGeo = new TAGparaDsc("msdGeo", new TAMSDparGeo());
   TAMSDparGeo *geomap = (TAMSDparGeo *)msdGeo->Object();
   parFileName = campManager->GetCurGeoFile(FootBaseName("TAMSDparGeo"), runNumber);
   geomap->FromFile(parFileName.Data());

   TAGparaDsc *msdConf = new TAGparaDsc("msdConf", new TAMSDparConf());
   TAMSDparConf *confmap = (TAMSDparConf *)msdConf->Object();
   parFileName = campManager->GetCurConfFile(FootBaseName("TAMSDparGeo"), runNumber);
   confmap->FromFile(parFileName.Data());

   TAGparaDsc *msdCal = new TAGparaDsc("msdCal", new TAMSDparCal(geomap->GetStripsN()));
   TAMSDparCal *parCalMsd = (TAMSDparCal *)msdCal->Object();
   parFileName = campManager->GetCurCalFile(FootBaseName("TAMSDparGeo"), runNumber, true);
   parCalMsd->LoadEnergyCalibrationMap(parFileName.Data());

   parFileName = campManager->GetCurCalFile(FootBaseName("TAMSDparGeo"), runNumber);
   parCalMsd->LoadPedestalMap(parFileName.Data());

   TAGdataDsc *msdDaq = new TAGdataDsc("msdDaq", new TAGdaqEvent());
   TAGdataDsc *msdDat = new TAGdataDsc("msdDat", new TAMSDntuRaw());
   TAGdataDsc *msdHit = new TAGdataDsc("msdHit", new TAMSDntuHit());

   daqActReaderMSD = new TAGactDaqReader("daqActReader", msdDaq);

   msdActRaw = new TAMSDactNtuRaw("msdActRaw", msdDat, msdDaq, msdMap, msdCal, msdGeo, msdConf);
   msdActHit = new TAMSDactNtuHit("msdActHit", msdDat, msdHit, msdGeo, msdConf, msdCal);
   msdActHit->CreateHistogram();
   daqActReaderMSD->Open(filename);

   tagr.AddRequiredItem(daqActReaderMSD);
   tagr.AddRequiredItem(msdActRaw);
   tagr.AddRequiredItem(msdActHit);

   tagr.Print();

   const int sensors = map->GetSensorsN();
   const int NChannels = geomap->GetStripsN();

   int boardId;
   int sensorId;
   float cn;

   TH1D *hADC[sensors][NChannels];
   TH1D *hSignal[sensors][NChannels];

   TGraph *ped_graph[sensors];
   TGraph *sig_graph[sensors];
   TAxis *ped_axis[sensors];
   TAxis *sig_axis[sensors];

   for (int sen = 0; sen < sensors; sen++)
   {
      ped_graph[sen] = new TGraph(NChannels);
      ped_graph[sen]->SetTitle(Form("Pedestals for detector %i RUN %i", sen, runNumber));
      ped_graph[sen]->GetXaxis()->SetTitle("channel");
      ped_graph[sen]->GetYaxis()->SetTitle("Pedestal");

      sig_graph[sen] = new TGraph(NChannels);
      sig_graph[sen]->SetTitle(Form("Sigmas for detector %i RUN %i", sen, runNumber));
      sig_graph[sen]->GetXaxis()->SetTitle("channel");
      sig_graph[sen]->GetYaxis()->SetTitle("Sigma");

      for (int ch = 0; ch < NChannels; ch++)
      {
         hADC[sen][ch] = new TH1D(Form("pedestal_channel_%d_sensor_%d", ch, sen), Form("Pedestal %d", ch), 50, 4096, -1);
         hADC[sen][ch]->GetXaxis()->SetTitle("ADC");
         hSignal[sen][ch] = new TH1D(Form("signal_channel_%d_sensor_%d", ch, sen), Form("Signal %d", ch), 50, 4096, -1);
         hSignal[sen][ch]->GetXaxis()->SetTitle("ADC");
      }
   }

   vector<vector<float>> pedestals(sensors, vector<float>(NChannels, 0));
   vector<vector<float>> rsigma(sensors, vector<float>(NChannels, 0));
   vector<vector<float>> signals(sensors, vector<float>(NChannels, 0));
   vector<vector<float>> sigma(sensors, vector<float>(NChannels, 0));

   FILE *calfile;
   char overwrite[3];
   TString calfile_name = "./calib/" + expName + "/TAMSD_Pedestal.cal";

   cout << "\nComputing MSD calibration file for campaign " << expName << " with run " << dec << runNumber << endl;
   cout << "(Make sure you are using a calibration run with no beam)" << endl;

   if (FILE *file = fopen("./calib/" + expName + "/TAMSD_Pedestal.cal", "r"))
   {
      fclose(file);
      cout << "Default calibration file exists, do you want to overwrite it? (Y/N)" << endl;
      cin >> overwrite;

      if (!strncmp(overwrite, "N", 1) || !strncmp(overwrite, "n", 1))
      {
         calfile_name = "./calib/" + expName + "/TAMSD_Pedestal_" + runNumber + ".cal";
      }
   }

   double high_threshold, low_threshold;
   cout << "\tEnter value for high threshold: \n";
   cout << "\t";
   cin >> high_threshold;
   cout << "\tEnter value for low threshold: \n";
   cout << "\t";
   cin >> low_threshold;

   calfile = fopen(calfile_name, "w");
   fprintf(calfile, "# SigmaSeedLevel\n");
   fprintf(calfile, "# SigmaHitLevel\n");

   cout << "\nBeginning the Event Loop " << endl;
   tagr.BeginEventLoop();

   TStopwatch watch;
   watch.Start();

   Int_t nEvents = 0;
   while (tagr.NextEvent() && nEvents < nMaxEmsds / 2)
   {

      rawevent = (TAGdaqEvent *)(tagr.FindDataDsc("msdDaq", "TAGdaqEvent")->Object());
      Int_t nFragments = rawevent->GetFragmentSize("DEMSDEvent");

      if (nFragments == 0)
      {
         std::cout << "No fragments found for MSD, exiting ..." << std::endl;
         return;
      }

      for (Int_t i = 0; i < nFragments; ++i)
      {
         const DEMSDEvent *evt = static_cast<const DEMSDEvent *>(rawevent->GetFragment("DEMSDEvent", i));
         if (evt->boardHeader == 0x0000dead)
            continue;

         boardId = (evt->boardHeader & 0xF) - 1;

         sensorId = map->GetSensorId(boardId, 0);

         if (sensorId % 2 == 0)
         {
            for (int ch = 0; ch < NChannels; ch++)
            {
               hADC[sensorId][ch]->Fill(evt->FirstPlane[ch]);
            }
         }
         else
         {
            for (int ch = 0; ch < NChannels; ch++)
            {
               hADC[sensorId][ch]->Fill(evt->SecondPlane[ch]);
            }
         }

         sensorId = map->GetSensorId(boardId, 1);

         if (sensorId % 2 == 0)
         {
            for (int ch = 0; ch < NChannels; ch++)
            {
               hADC[sensorId][ch]->Fill(evt->FirstPlane[ch]);
            }
         }
         else
         {
            for (int ch = 0; ch < NChannels; ch++)
            {
               hADC[sensorId][ch]->Fill(evt->SecondPlane[ch]);
            }
         }
      }
      // printf("\n");
      if (++nEvents % 100 == 0)
         printf("Event for pedestal calculation: %d\n", nEvents);
   }

   TF1 *fittedgaus;

   for (int sen = 0; sen < sensors; sen++)
   {
      fprintf(calfile, "#sensorId: %i\n", sen);
      fprintf(calfile, "%f\n", high_threshold);
      fprintf(calfile, "%f\n", low_threshold);

      for (int ch = 0; ch < NChannels; ch++)
      {
         // Fitting histos with gaus to compute ped and raw_sigma
         if (hADC[sen][ch]->GetEntries())
         {
            hADC[sen][ch]->Fit("gaus", "QS");
            fittedgaus = (TF1 *)hADC[sen][ch]->GetListOfFunctions()->FindObject("gaus");
            pedestals[sen][ch] = fittedgaus->GetParameter(1);
            rsigma[sen][ch] = fittedgaus->GetParameter(2);
            hADC[sen][ch]->Delete();
         }
         else
         {
            pedestals[sen][ch] = 0;
            rsigma[sen][ch] = 0;
         }
      }
   }
   fprintf(calfile, "############\n");
   fprintf(calfile, "#sensorId stripId AsicId AsicCh Mean Sigma status\n");

   nEvents = 0;
   while (tagr.NextEvent() && nEvents < nMaxEmsds / 2)
   {

      rawevent = (TAGdaqEvent *)(tagr.FindDataDsc("msdDaq", "TAGdaqEvent")->Object());
      Int_t nFragments = rawevent->GetFragmentSize("DEMSDEvent");
      cn = 0;

      for (Int_t i = 0; i < nFragments; ++i)
      {

         const DEMSDEvent *evt = static_cast<const DEMSDEvent *>(rawevent->GetFragment("DEMSDEvent", i));
         boardId = (evt->boardHeader & 0xF) - 1;

         sensorId = map->GetSensorId(boardId, 0);
         for (int ch = 0; ch < NChannels; ch++)
         {
            signals[sensorId][ch] = (evt->FirstPlane[ch] - pedestals[sensorId][ch]);
         }
         for (int ch = 0; ch < NChannels; ch++)
         {
            if (!(ch % 64))
            {
               cn = TMath::Mean(signals[sensorId].begin() + ch, signals[sensorId].begin() + ch + 64);
            }
            hSignal[sensorId][ch]->Fill(signals[sensorId][ch] - cn);
         }

         sensorId = map->GetSensorId(boardId, 1);
         for (int ch = 0; ch < NChannels; ch++)
         {
            signals[sensorId][ch] = (evt->SecondPlane[ch] - pedestals[sensorId][ch]);
         }
         for (int ch = 0; ch < NChannels; ch++)
         {
            if (!(ch % 64))
            {
               cn = TMath::Mean(signals[sensorId].begin() + ch, signals[sensorId].begin() + ch + 64);
            }
            hSignal[sensorId][ch]->Fill(signals[sensorId][ch] - cn);
         }
      }

      // printf("\n");
      if (++nEvents % 100 == 0)
         printf("Event for common noise subtraction: %d\n", nEvents);
   }

   for (int sen = 0; sen < sensors; sen++)
   {
      for (int ch = 0; ch < NChannels; ch++)
      {
         // Fitting histos with gaus to compute ped and raw_sigma
         if (hSignal[sen][ch]->GetEntries())
         {
            hSignal[sen][ch]->Fit("gaus", "QS");
            fittedgaus = (TF1 *)hSignal[sen][ch]->GetListOfFunctions()->FindObject("gaus");
            sigma[sen][ch] = fittedgaus->GetParameter(2);
            hSignal[sen][ch]->Delete();
         }
         else
         {
            sigma[sen][ch] = 0;
         }
      }
   }

   for (int sen = 0; sen < sensors; sen++)
   {
      fprintf(calfile, "#sensorId: %i\n", sen);
      for (int ch = 0; ch < NChannels; ch++)
      {
         ped_graph[sen]->SetPoint(ch, ch, pedestals[sen][ch]);
         sig_graph[sen]->SetPoint(ch, ch, sigma[sen][ch]);

         fprintf(calfile, "%2d %3d %2d %2d %5.1f %3.1f %d\n", sen, ch, ch / 64, ch % 64, pedestals[sen][ch], sigma[sen][ch], (sigma[sen][ch] < 1.8 || sigma[sen][ch] > 5));
      }
   }

   tagr.EndEventLoop();
   fclose(calfile);
   watch.Print();

   // plot correlation in telescope planes
   TCanvas *calibrations = new TCanvas("calib", "calibrations", 1920, 1080);
   ped_graph[0]->GetXaxis()->SetRangeUser(0, NChannels);
   ped_graph[0]->GetXaxis()->SetRangeUser(0, NChannels);
   ped_graph[0]->GetXaxis()->SetNdivisions(NChannels / 64, false);
   ped_graph[0]->Draw("AL*");
   calibrations->Print(calfile_name + ".pdf(", "pdf");
   sig_graph[0]->GetXaxis()->SetRangeUser(0, NChannels);
   sig_graph[0]->GetXaxis()->SetNdivisions(NChannels / 64, false);
   sig_graph[0]->Draw("AL*");
   calibrations->Print(calfile_name + ".pdf", "pdf");

   for (int i = 2; i < 2 * sensors; i += 2)
   {
      ped_graph[i / 2]->GetXaxis()->SetRangeUser(0, NChannels);
      ped_graph[i / 2]->GetXaxis()->SetNdivisions(NChannels / 64, false);
      ped_graph[i / 2]->Draw("AL*");
      calibrations->Print(calfile_name + ".pdf", "pdf");
      sig_graph[i / 2]->GetXaxis()->SetRangeUser(0, NChannels);
      sig_graph[i / 2]->GetXaxis()->SetNdivisions(NChannels / 64, false);
      sig_graph[i / 2]->Draw("AL*");
      calibrations->Print(calfile_name + ".pdf", "pdf");
   }
   calibrations->Print(calfile_name + ".pdf)", "pdf");

   return;

   // Delete all histograms
   for (int i = 0; i < sensors; i++)
   {
      for (int j = 0; j < NChannels; j++)
      {
         hADC[i][j]->Delete();
         hSignal[i][j]->Delete();
      }
   }
}
