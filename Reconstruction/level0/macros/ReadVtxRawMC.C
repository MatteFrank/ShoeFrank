
// Macro to reconstruct from G4 MC hits
// Ch. Finck, Juky 19.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TROOT.h>
#include <TStopwatch.h>

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGaction.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"

#include "TAVTparMap.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"

#include "TAGcampaignManager.hxx"
#include "TAGactTreeReader.hxx"
#include "TAVTactNtuHitMC.hxx"

#include "TAVTactNtuClusterF.hxx"
#include "TAVTactNtuTrackF.hxx"

#endif

// main
TAGcampaignManager* campManager = 0x0;
TAGactTreeWriter*   outFile     = 0x0;
TAGactTreeReader*   vtActReader = 0x0;
TAVTactNtuHitMC*    vtActRaw    = 0x0;
TAVTactNtuClusterF* vtActClus   = 0x0;
TAVTactNtuTrackF*   vtActTrck   = 0x0;

void FillVertex(Int_t runNumber)
{
  TAGparaDsc* vtGeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());
  TAVTparGeo* geomap   = (TAVTparGeo*) vtGeo->Object();
  TString parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
  geomap->FromFile(parFileName.Data());
  
  TAGparaDsc*  vtConf  = new TAGparaDsc("vtConf", new TAVTparConf());
  TAVTparConf* parconf = (TAVTparConf*) vtConf->Object();
  parFileName = campManager->GetCurConfFile(TAVTparGeo::GetBaseName(), runNumber);
  parconf->FromFile(parFileName.Data());
  
  TAVTparConf::SetHistoMap();
  TAGdataDsc* vtEve  = new TAGdataDsc("vtEve", new TAMCntuEve());
  TAGdataDsc* vtMc   = new TAGdataDsc("vtMc", new TAMCntuHit());
  TAGdataDsc* vtNtu  = new TAGdataDsc("vtNtu", new TAVTntuRaw());
  TAGdataDsc* vtClus = new TAGdataDsc("vtClus", new TAVTntuCluster());
  TAGdataDsc* vtTrck = new TAGdataDsc("vtTrck", new TAVTntuTrack());
  
  vtActReader  = new TAGactTreeReader("vtActEvtReader");
  vtActReader->SetupBranch(vtMc, TAMCntuHit::GetVtxBranchName());
  vtActReader->SetupBranch(vtEve,TAMCntuEve::GetBranchName());
  
  vtActRaw= new TAVTactNtuHitMC("vtActNtu", vtMc, vtEve, vtNtu, vtGeo);
  vtActRaw->CreateHistogram();
  
  vtActClus =  new TAVTactNtuClusterF("vtActClus", vtNtu, vtClus, vtConf, vtGeo);
  vtActClus->CreateHistogram();
  
  vtActTrck = new TAVTactNtuTrackF("vtActTrck", vtClus, vtTrck, vtConf, vtGeo);
  vtActTrck->CreateHistogram();
  
  //   outFile->SetupElementBranch(vtNtu, "vtrh.");
  //   outFile->SetupElementBranch(vtClus, "vtclus.");
  //   outFile->SetupElementBranch(vtTrck, "vttrack.");
  
}

void ReadVtxRawMC(TString filename = "12C_C_200shoe.root", Int_t nMaxEvts = 0,
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
  
  outFile = new TAGactTreeWriter("outFile");
  
  FillVertex(runNumber);
  vtActReader->Open(filename, "READ", "EventTree");
  
  tagr.AddRequiredItem(vtActReader);
  tagr.AddRequiredItem(vtActRaw);
  tagr.AddRequiredItem(vtActClus);
  tagr.AddRequiredItem(vtActTrck);
  tagr.AddRequiredItem(outFile);
  
  tagr.Print();
  
  Int_t pos = filename.Last('.');
  
  TString outFileName = filename(0,pos);
  outFileName.Append("_Out.root");
  
  if (outFile->Open(outFileName.Data(), "RECREATE")) return;
  vtActRaw->SetHistogramDir(outFile->File());
  vtActClus->SetHistogramDir(outFile->File());
  vtActTrck->SetHistogramDir(outFile->File());
  
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


