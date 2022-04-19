
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
TAMSDactNtuHit *msdActHit = 0x0;
TAMSDactNtuCluster *msdActClus = 0x0;
TAMSDactNtuPoint *msdActPoint = 0x0;

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

   parFileName = campManager->GetCurCalFile(TAMSDparGeo::GetBaseName(), runNumber);
   parCalMsd->LoadPedestalMap(parFileName.Data());

   TAGdataDsc *msdDaq = new TAGdataDsc("msdDaq", new TAGdaqEvent());
   TAGdataDsc *msdDat = new TAGdataDsc("msdDat", new TAMSDntuRaw());
   TAGdataDsc *msdHit = new TAGdataDsc("msdHit", new TAMSDntuHit());
   TAGdataDsc *msdclus = new TAGdataDsc("msdClus", new TAMSDntuCluster());
   TAGdataDsc *msdPoint = new TAGdataDsc("msdPoint", new TAMSDntuPoint());

   daqActReaderMSD = new TAGactDaqReader("daqActReader", msdDaq);

   msdActRaw = new TAMSDactNtuRaw("msdActRaw", msdDat, msdDaq, msdMap, msdCal, msdGeo, msdConf);
   msdActRaw->CreateHistogram();

   msdActHit = new TAMSDactNtuHit("msdActHit", msdDat, msdHit, msdGeo, msdConf, msdCal);
   msdActHit->CreateHistogram();

   msdActClus = new TAMSDactNtuCluster("msdActClus", msdHit, msdclus, msdConf, msdGeo, msdCal);
   msdActClus->CreateHistogram();

   msdActPoint = new TAMSDactNtuPoint("msdActPoint", msdclus, msdPoint, msdGeo);
   msdActPoint->CreateHistogram();
}

void Booking(TFile *f_out)
{
   f_out->cd();
   const int sensors = 6;
   TH1D *hClusOneStrip[sensors];
   TH1D *hClusTwoStrip[sensors];
   TH1D *hClusManyStrip[sensors];
   TH1D *hClusOneStripCorr[sensors];
   TH1D *hClusTwoStripCorr[sensors];
   TH1D *hClusManyStripCorr[sensors];
   TH2D *hMsdVsVtxClusNum[sensors];
   TH2D *hMsdVsVtxEnergy[sensors];
   TH2D *hClusVsEta[sensors];

   for (int i = 0; i < sensors; i++)
   {
      hClusOneStrip[i] = new TH1D(Form("singleStripClus_sensor_%i", i), Form("Cluster with a single strip for sensor %i", i), 1000, 0., 5000.);
      hClusTwoStrip[i] = new TH1D(Form("doubleStripClus_sensor_%i", i), Form("Cluster with two strips for sensor %i", i), 1000, 0., 5000.);
      hClusManyStrip[i] = new TH1D(Form("manyStripClus_sensor_%i", i), Form("Cluster with more than two strips for sensor %i", i), 1000, 0., 5000.);
      hClusOneStripCorr[i] = new TH1D(Form("singleStripClus_corr_sensor_%i", i), Form("Cluster with a single strip for sensor %i (corrected)", i), 1000, 0., 5000.);
      hClusTwoStripCorr[i] = new TH1D(Form("doubleStripClus_corr_sensor_%i", i), Form("Cluster with two strips for sensor %i (corrected)", i), 1000, 0., 5000.);
      hClusManyStripCorr[i] = new TH1D(Form("manyStripClus_corr_sensor_%i", i), Form("Cluster with more than two strips for sensor %i (corrected)", i), 1000, 0., 5000.);

      hClusVsEta[i] = new TH2D(Form("msd_%i_clus_vs_eta", i), Form("Cluster ADC vs 'Eta' for sensor %i", i), 2000, 0, 5000, 100, 0, 1);
   }
   return;
}

void ReadMsdRaw(TString filename = "dataRaw/data_test.00003890.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data", Int_t nMaxEmsds = 1000,
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

   outFile = new TAGactTreeWriter("outFile");

   FillDetectors(runNumber);
   daqActReaderMSD->Open(filename);

   tagr.AddRequiredItem(daqActReaderMSD);
   tagr.AddRequiredItem(msdActRaw);
   tagr.AddRequiredItem(msdActHit);
   tagr.AddRequiredItem(msdActClus);
   tagr.AddRequiredItem(outFile);

   tagr.Print();

   Int_t pos = filename.First(".");

   TString outFileName = filename(pos + 1, 8);

   //outFileName.Prepend("/nfs/NASPG/BTData/Jul2021_FOOT_GSI_data/analysis/run_");
   outFileName.Prepend("./run_");
   outFileName.Append("_");
   outFileName.Append(Form("%i", fileNumber));
   outFileName.Append(".root");
   if (outFile->Open(outFileName.Data(), "RECREATE"))
      return;

   TString myfileoutname = filename(pos + 1, 8);
   //myfileoutname.Prepend("/nfs/NASPG/BTData/Jul2021_FOOT_GSI_data/analysis/pg_run.");
   myfileoutname.Prepend("./pg_run.");
   myfileoutname.Append("_");
   myfileoutname.Append(Form("%i", fileNumber));
   myfileoutname.Append(".root");
   TFile *f_out = new TFile(myfileoutname.Data(), "RECREATE");

   msdActRaw->SetHistogramDir(outFile->File());
   msdActHit->SetHistogramDir(outFile->File());
   msdActClus->SetHistogramDir(outFile->File());
   msdActPoint->SetHistogramDir(outFile->File());

   cout << " Beginning the Event Loop " << endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();

   Booking(f_out);
   TString hist_name;

   TAMSDntuCluster *msdClus;

   const int sensors = 6;

   Int_t nEvents = 0;
   while (tagr.NextEvent())
   {
      msdClus = (TAMSDntuCluster *)(tagr.FindDataDsc("msdClus", "TAMSDntuCluster")->Object());

      for (int i = 0; i < sensors; i++)
      {
         if (msdClus->GetClustersN(i))
         {
            for (int j = 0; j < msdClus->GetClustersN(i); j++)
            {
               if (msdClus->GetCluster(i, j)->GetStripsN() > 1)
               {
                  hist_name = Form("msd_%i_clus_vs_eta", i);
                  ((TH1D *)(f_out->Get(hist_name)))->Fill(msdClus->GetCluster(i, j)->GetEnergyLoss(), msdClus->GetCluster(i, j)->GetEta());
               }

               if (msdClus->GetCluster(i, j)->GetStripsN() == 1)
               {
                  hist_name = Form("singleStripClus_sensor_%i", i);
                  ((TH1D *)(f_out->Get(hist_name)))->Fill(msdClus->GetCluster(i, j)->GetEnergyLoss());
                  hist_name = Form("singleStripClus_corr_sensor_%i", i);
                  ((TH1D *)(f_out->Get(hist_name)))->Fill(msdClus->GetCluster(i, j)->GetEnergyLossCorr());
               }
               else if (msdClus->GetCluster(i, j)->GetStripsN() == 2)
               {
                  hist_name = Form("doubleStripClus_sensor_%i", i);
                  ((TH1D *)(f_out->Get(hist_name)))->Fill(msdClus->GetCluster(i, j)->GetEnergyLoss());
                  hist_name = Form("doubleStripClus_corr_sensor_%i", i);
                  ((TH1D *)(f_out->Get(hist_name)))->Fill(msdClus->GetCluster(i, j)->GetEnergyLossCorr());
               }
               else
               {
                  hist_name = Form("manyStripClus_sensor_%i", i);
                  ((TH1D *)(f_out->Get(hist_name)))->Fill(msdClus->GetCluster(i, j)->GetEnergyLoss());
                  hist_name = Form("manyStripClus_corr_sensor_%i", i);
                  ((TH1D *)(f_out->Get(hist_name)))->Fill(msdClus->GetCluster(i, j)->GetEnergyLossCorr());
               }
            }
         }
      }
      // printf("\n");
      if (++nEvents % 100 == 0)
         printf("Event: %d\n", nEvents);

      if (nEvents == nMaxEmsds)
         break;
   }
   tagr.EndEventLoop();

   outFile->Print();
   outFile->Close();

   f_out->Write();
   f_out->Close();

   watch.Print();
}
