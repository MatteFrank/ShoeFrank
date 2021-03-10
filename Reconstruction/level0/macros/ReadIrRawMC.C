// Macro to make IR track reconstruction from MC data
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
#include "GlobalPar.hxx"

#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"

// VTX
#include "TAGparGeo.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparMap.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAGcampaignManager.hxx"
#include "TAVTactNtuMC.hxx"
#include "TAVTactNtuClusterF.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuVertexPD.hxx"
#include "TAVTactNtuVertex.hxx"

//ITR
#include "TAITparGeo.hxx"
#include "TAITparMap.hxx"
#include "TAITparConf.hxx"
#include "TAITntuRaw.hxx"
#include "TAITntuCluster.hxx"

#include "TAITactNtuMC.hxx"
#include "TAITactNtuClusterF.hxx"

// IR
#include "TAIRntuTrack.hxx"
#include "TAIRactNtuTrack.hxx"


#endif

// main
TAGcampaignManager* campManager = 0x0;
TAGactTreeWriter*   outFile   = 0x0;
TAVTactNtuMC*       vtActRaw  = 0x0;
TAVTactNtuClusterF* vtActClus = 0x0;
TAVTactNtuTrackF*   vtActTrck = 0x0;
TAVTactNtuVertex*   vtActVtx  = 0x0;
TAGdataDsc*         vtVtx     = 0x0;
TAGparaDsc*         vtGeo     = 0x0;

TAITactNtuMC*       itActRaw  = 0x0;
TAITactNtuClusterF* itActClus = 0x0;

TAIRactNtuTrack*    irActTrck = 0x0;

void FillMCVertex(EVENT_STRUCT *myStr, Int_t runNumber) {
   
   /*Ntupling the MC Vertex information*/
   TAGparaDsc* tgGeo = new TAGparaDsc("tgGeo", new TAGparGeo());
   TAGparGeo* geomapg = (TAGparGeo*)tgGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(TAGparGeo::GetBaseName(), runNumber);
   geomapg->FromFile(parFileName.Data());
   
   TAGparaDsc* itGeo    = new TAGparaDsc("itGeo", new TAVTparGeo());
   TAVTparGeo* geomap   = (TAVTparGeo*) itGeo->Object();
   parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   
   TAGdataDsc* vtRaw    = new TAGdataDsc("vtRaw", new TAVTntuHit());
   TAGdataDsc* vtClus   = new TAGdataDsc("vtClus", new TAVTntuCluster());
   TAGdataDsc* vtTrck   = new TAGdataDsc("vtTrck", new TAVTntuTrack());
               vtVtx    = new TAGdataDsc("vtVtx", new TAVTntuVertex());
   
   TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
   TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
   parconf->FromFile();
   
   TAVTparConf::SetHistoMap();
   vtActRaw  = new TAVTactNtuMC("vtActRaw", vtRaw, vtGeo, myStr);
   
   vtActClus = new TAVTactNtuClusterF("vtActCluster", vtRaw, vtClus, vtConf, vtGeo);
   
   vtActTrck = new TAVTactNtuTrackF("vtActTrack", vtClus, vtTrck, vtConf, vtGeo);
   
   vtActVtx = new TAVTactNtuVertex("vtActVtx", vtTrck, vtVtx, vtConf, vtGeo, tgGeo);
   vtActVtx->CreateHistogram();
}

void FillMCInnerTracker(EVENT_STRUCT *myStr, Int_t runNumber) {
   
   /*Ntupling the MC Inner tracker information*/
   TAGparaDsc* itGeo    = new TAGparaDsc("itGeo", new TAITparGeo());
   TAITparGeo* geomap   = (TAITparGeo*) itGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(TAITparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   
   TAGdataDsc* itRaw    = new TAGdataDsc("itRaw", new TAITntuRaw());
   TAGdataDsc* itClus   = new TAGdataDsc("itClus", new TAITntuCluster());
   
   TAGparaDsc*  itConf  = new TAGparaDsc("itConf", new TAITparConf());
   TAITparConf* parconf = (TAITparConf*) itConf->Object();
   parconf->FromFile();
   
   itActRaw  = new TAITactNtuMC("itActRaw", itRaw, itGeo, myStr);
   
   itActClus = new TAITactNtuClusterF("itActCluster", itRaw, itClus, itConf, itGeo);
   
   TAGdataDsc* irTrck   = new TAGdataDsc("irTrack", new TAIRntuTrack());
   irActTrck = new TAIRactNtuTrack("irActTrack", itClus, vtVtx, irTrck, itConf, itGeo, vtGeo);
   irActTrck->CreateHistogram();

   outFile->SetupElementBranch(irTrck, TAIRntuTrack::GetBranchName());
}

void ReadIrRawMC(TString name = "ionEventoC_All.root", Int_t nMaxEvts = 20000,
                 TString expName = "12C_200", Int_t runNumber = 1)
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
   
   FillMCVertex(&evStr, runNumber);
   FillMCInnerTracker(&evStr, runNumber);

   tagr.AddRequiredItem("vtActRaw");
   tagr.AddRequiredItem("vtActCluster");
   tagr.AddRequiredItem("vtActTrack");
   tagr.AddRequiredItem("vtActVtx");
   
   tagr.AddRequiredItem("itActRaw");
   tagr.AddRequiredItem("itActCluster");

   tagr.AddRequiredItem("irActTrack");

   tagr.AddRequiredItem("outFile");
   tagr.Print();
   
   Int_t pos = name.Last('.');
   TString nameOut = name(0, pos);
   nameOut.Append("_Out.root");
   
   if (outFile->Open(nameOut.Data(), "RECREATE")) return;
   vtActRaw->SetHistogramDir(outFile->File());
   vtActClus->SetHistogramDir(outFile->File());
   vtActTrck->SetHistogramDir(outFile->File());
   vtActVtx->SetHistogramDir(outFile->File());
   
   itActRaw->SetHistogramDir(outFile->File());
   itActClus->SetHistogramDir(outFile->File());

   irActTrck->SetHistogramDir(outFile->File());

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
      
     
      if (ientry == nMaxEvts)
         break;
      
      if (!tagr.NextEvent()) break;
   }
   
   tagr.EndEventLoop();
   
   outFile->Print();
   outFile->Close();
   
   watch.Print();
}


