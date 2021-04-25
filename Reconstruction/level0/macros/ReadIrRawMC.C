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

#include "TAGrecoManager.hxx"

#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"

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
#include "TAGactTreeReader.hxx"
#include "TAVTactNtuHitMC.hxx"
#include "TAVTactNtuCluster.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuVertexPD.hxx"
#include "TAVTactNtuVertex.hxx"

//ITR
#include "TAITparGeo.hxx"
#include "TAITparMap.hxx"
#include "TAITparConf.hxx"
#include "TAITntuHit.hxx"
#include "TAITntuCluster.hxx"

#include "TAITactNtuHitMC.hxx"
#include "TAITactNtuClusterF.hxx"

// IR
#include "TAIRntuTrack.hxx"
#include "TAIRactNtuTrack.hxx"


#endif

// main
TAGcampaignManager* campManager = 0x0;
TAGactTreeWriter*   outFile     = 0x0;
TAGactTreeReader*   vtActReader = 0x0;
TAVTactNtuHitMC*    vtActRaw    = 0x0;
TAVTactNtuCluster* vtActClus   = 0x0;
TAVTactNtuTrackF*   vtActTrck   = 0x0;
TAVTactNtuVertex*   vtActVtx  = 0x0;
TAGdataDsc*         vtVtx     = 0x0;
TAGparaDsc*         vtGeo     = 0x0;

TAGdataDsc*         vtEve     = 0x0;

TAITactNtuHitMC*       itActRaw  = 0x0;
TAITactNtuClusterF* itActClus = 0x0;

TAIRactNtuTrack*    irActTrck = 0x0;

void FillMCVertex(Int_t runNumber) {
   
   /*Ntupling the MC Vertex information*/
   TAGparaDsc* tgGeo = new TAGparaDsc("tgGeo", new TAGparGeo());
   TAGparGeo* geomapg = (TAGparGeo*)tgGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(TAGparGeo::GetBaseName(), runNumber);
   geomapg->FromFile(parFileName.Data());
   
   TAGparaDsc* vtGeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());
   TAVTparGeo* geomap   = (TAVTparGeo*) vtGeo->Object();
   parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   
   vtEve  = new TAGdataDsc("vtEve", new TAMCntuPart());
   TAGdataDsc* vtMc   = new TAGdataDsc("vtMc", new TAMCntuHit());
   
   TAGdataDsc* vtNtu    = new TAGdataDsc("vtHit", new TAVTntuHit());
   TAGdataDsc* vtClus   = new TAGdataDsc("vtClus", new TAVTntuCluster());
   TAGdataDsc* vtTrck   = new TAGdataDsc("vtTrck", new TAVTntuTrack());
   vtVtx    = new TAGdataDsc("vtVtx", new TAVTntuVertex());
   
   TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
   TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
   parconf->FromFile();
   
   TAVTparConf::SetHistoMap();
   vtActReader  = new TAGactTreeReader("vtActEvtReader");
   vtActReader->SetupBranch(vtMc, TAMCntuHit::GetVtxBranchName());
   vtActReader->SetupBranch(vtEve,TAMCntuPart::GetBranchName());
   
   vtActRaw= new TAVTactNtuHitMC("vtActNtu", vtMc, vtEve, vtNtu, vtGeo);
   vtActRaw->CreateHistogram();
   
   vtActClus =  new TAVTactNtuCluster("vtActClus", vtNtu, vtClus, vtConf, vtGeo);
   vtActClus->CreateHistogram();
   
   vtActTrck = new TAVTactNtuTrackF("vtActTrck", vtClus, vtTrck, vtConf, vtGeo);
   vtActTrck->CreateHistogram();
   
   vtActVtx = new TAVTactNtuVertex("vtActVtx", vtTrck, vtVtx, vtConf, vtGeo, tgGeo);
   vtActVtx->CreateHistogram();
}

void FillMCInnerTracker(Int_t runNumber) {
   
   /*Ntupling the MC Inner tracker information*/
   TAGparaDsc* itGeo    = new TAGparaDsc("itGeo", new TAITparGeo());
   TAITparGeo* geomap   = (TAITparGeo*) itGeo->Object();
   TString parFileName = campManager->GetCurGeoFile(TAITparGeo::GetBaseName(), runNumber);
   geomap->FromFile(parFileName.Data());
   
   TAGdataDsc* itMc   = new TAGdataDsc("itMc", new TAMCntuHit());
   TAGdataDsc* itNtu    = new TAGdataDsc("itHit", new TAITntuHit());
   TAGdataDsc* itClus   = new TAGdataDsc("itClus", new TAITntuCluster());
   
   TAGparaDsc*  itConf  = new TAGparaDsc("itConf", new TAITparConf());
   TAITparConf* parconf = (TAITparConf*) itConf->Object();
   parconf->FromFile();
   
   vtActReader->SetupBranch(itMc, TAMCntuHit::GetItrBranchName());
   
   itActRaw= new TAITactNtuHitMC("itActNtu", itMc, vtEve, itNtu, itGeo);
   
   itActClus = new TAITactNtuClusterF("itActCluster", itNtu, itClus, itConf, itGeo);
   
   TAGdataDsc* irTrck   = new TAGdataDsc("irTrack", new TAIRntuTrack());
   irActTrck = new TAIRactNtuTrack("irActTrack", itClus, vtVtx, irTrck, itConf, itGeo, vtGeo);
   irActTrck->CreateHistogram();
   
   outFile->SetupElementBranch(irTrck, TAIRntuTrack::GetBranchName());
}

void ReadGlbSRawMC(TString filename = "12C_C_200shoereg.root", Int_t nMaxEvts = 1000,
                   TString expName = "12C_200", Int_t runNumber = 1)
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
   
   TFile* f = new TFile(filename.Data());
   f->ls();
   
   outFile = new TAGactTreeWriter("outFile");
   
   FillMCVertex(runNumber);
   FillMCInnerTracker(runNumber);
   vtActReader->Open(filename, "READ", "EventTree");
   
   tagr.AddRequiredItem("vtActRaw");
   tagr.AddRequiredItem("vtActCluster");
   tagr.AddRequiredItem("vtActTrack");
   tagr.AddRequiredItem("vtActVtx");
   
   tagr.AddRequiredItem("itActRaw");
   tagr.AddRequiredItem("itActCluster");
   
   tagr.AddRequiredItem("irActTrack");
   
   tagr.AddRequiredItem("outFile");
   tagr.Print();
   
   Int_t pos = filename.Last('.');
   TString nameOut = filename(0, pos);
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


