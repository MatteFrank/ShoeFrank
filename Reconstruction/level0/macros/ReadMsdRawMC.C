// Macro to make reconstruction from MC data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>

#include "Evento.hxx"

#include "TAGcampaignManager.hxx"
#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDparMap.hxx"
#include "TAVTparConf.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDdatRaw.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDntuCluster.hxx"

#include "TAMSDactNtuMC.hxx"
#include "TAMSDactNtuCluster.hxx"

#include "GlobalPar.hxx"

#endif

// main
TAGcampaignManager* campManager = 0x0;
TAGactTreeWriter* outFile = 0x0;
TAMSDactNtuMC* msdActRaw = 0x0;
TAMSDactNtuCluster* msdActClus = 0x0;

void FillMCMsd(EVENT_STRUCT *myStr, Int_t runNumber) {
   
   /*Ntupling the MC Vertex information*/
   TAGparaDsc* msdGeo    = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
   TAMSDparGeo* geomap   = (TAMSDparGeo*) msdGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   
   TAGdataDsc* msdRaw    = new TAGdataDsc("msdRaw", new TAMSDntuHit());
   TAGdataDsc* msdClus   = new TAGdataDsc("msdClus", new TAMSDntuCluster());
   
   TAGparaDsc*  msdConf  = new TAGparaDsc("msdConf", new TAMSDparConf());
   TAMSDparConf* parconf = (TAMSDparConf*) msdConf->Object();
//   parconf->FromFile("./config/TAMSDdetector.cfg");
   
   TAVTparConf::SetHistoMap();
   msdActRaw  = new TAMSDactNtuMC("msdActRaw", msdRaw, msdGeo, myStr);
   msdActRaw->CreateHistogram();
   
   TAMSDparConf::SetHistoMap();
   msdActClus = new TAMSDactNtuCluster("msdActCluster", msdRaw, msdClus, msdConf, msdGeo);
   msdActClus->CreateHistogram();
   
   // outFile->SetupElementBranch(itRaw, TAMSDntuHit::GetBranchName());
  //outFile->SetupElementBranch(itClus, TAMSDntuCluster::GetBranchName());
   // outFile->SetupElementBranch(itTrck, TAMSDntuTrack::GetBranchName());
   
}

void ReadMsdRawMC(TString name = "16O_C2H4_200_1.root", TString expName = "16O_200", Int_t runNumber = 1)
{
   GlobalPar::Instance(expName);
   GlobalPar::GetPar()->Print();
   
   TAGroot tagr;
   
   campManager = new TAGcampaignManager(expName);
   campManager->FromFile();
   
   TAGgeoTrafo* geoTrafo = new TAGgeoTrafo();
   TString parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);
   geoTrafo->FromFile(parFileName);
   
   TFile* f = new TFile(name.Data());
   f->ls();
   
   TTree* tree = (TTree*)gDirectory->Get("EventTree");
   
   Evento *ev  = new Evento();
   
   EVENT_STRUCT evStr;
   
   ev->FindBranches(tree,&evStr);
   
   outFile = new TAGactTreeWriter("outFile");
   
   FillMCMsd(&evStr, runNumber);
   
   tagr.AddRequiredItem("msdActRaw");
   tagr.AddRequiredItem("msdActCluster");
   tagr.AddRequiredItem("outFile");
   tagr.Print();
   
   Int_t pos = name.Last('.');
   TString nameOut = name(0, pos);
   nameOut.Append("_Out.root");
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   msdActRaw->SetHistogramDir(outFile->File());
   msdActClus->SetHistogramDir(outFile->File());
   
   cout<<" Beginning the Event Loop "<<endl;
   tagr.BeginEventLoop();
   TStopwatch watch;
   watch.Start();
   
   Long64_t nentries = tree->GetEntries();
   
   cout<<"Running against ntuple with:: "<<nentries<<" entries!"<<endl;
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t ientry = 0; ientry < nentries; ientry++) {
      
      nb = tree->GetEntry(ientry);
      nbytes += nb;
      
      if(ientry % 100 == 0)
         cout<<" Loaded Event:: " << ientry << endl;
      
//      if (ientry == 500)
      if (ientry == 20000)
         break;
      
      if (!tagr.NextEvent()) break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();
}


