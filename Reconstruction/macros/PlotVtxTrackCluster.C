// Macro to read back MC info from reconstructed cluster per track of vertex
// Ch. Finck, Jan 21.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TString.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TStyle.h>

#include "TAGrecoManager.hxx"
#include "TAGactTreeReader.hxx"
#include "TAGcampaignManager.hxx"

#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#endif

// main
void PlotVtxTrackCluster(TString nameFile = "12C_C_200_L0Out.root", TString expName = "12C_200", Int_t runNumber = 1)
{
  // global par
  TAGrecoManager::Instance(expName);
  TAGrecoManager::GetPar()->FromFile();
  TAGrecoManager::GetPar()->Print();
  
  // TAGropot
  TAGroot tagr;
  
  // campaign manager
  TAGcampaignManager* campManager = new TAGcampaignManager(expName);
  campManager->FromFile();
  
  // geo file for VTX
  TAGparaDsc* parGeoVtx = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
  TAVTparGeo* pGeoMap = (TAVTparGeo*)parGeoVtx->Object();
  TString parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
  pGeoMap->FromFile(parFileName);
  
  // Branch setting
  TAVTntuTrack *vtTrack = new TAVTntuTrack();
  TAGdataDsc* vtTrck    = new TAGdataDsc("vtTrck", vtTrack);
  TAGactTreeReader* vtActReader  = new TAGactTreeReader("vtActEvtReader");
  vtActReader->SetupBranch(vtTrck, TAVTntuTrack::GetBranchName());
  
  vtActReader->Open(nameFile);
  tagr.AddRequiredItem(vtActReader);
  
  Int_t nentries = vtActReader->NEvents();
  
  // histogram
  TH2F* fpHisTrackClus = new TH2F("vtTrackClus", "Cluster per track vs event number", 10000, 0, nentries*1.1, 2, 2.5, 4.5);
  
  gStyle->SetOptStat(11);
  
  tagr.BeginEventLoop();
  
  Int_t ev = 0;
  
   while (tagr.NextEvent() ){
   
    Int_t nTrack = vtTrack->GetTracksN();
    for(Int_t i = 0; i < nTrack; ++i) {
      TAVTtrack* track = vtTrack->GetTrack(i);
      if (!track) continue;
      Int_t nclus = track->GetClustersN();
      
      fpHisTrackClus->Fill(ev, nclus);
      ev++;
    }
  }
  tagr.EndEventLoop();
  
  fpHisTrackClus->SetTitle("");
  fpHisTrackClus->SetXTitle("Number of events");
  fpHisTrackClus->SetTitleOffset(0.9,"X");
  fpHisTrackClus->SetTitleSize(0.05,"X");
  fpHisTrackClus->SetYTitle("Number of clusters per track");
  fpHisTrackClus->SetTitleOffset(0.9,"Y");
  fpHisTrackClus->SetTitleSize(0.05,"Y");
  fpHisTrackClus->SetLabelSize(0.04,"X");
  fpHisTrackClus->SetLabelSize(0.04,"Y");
  fpHisTrackClus->SetLabelOffset(0.002,"X");
  fpHisTrackClus->SetLabelOffset(0.005,"Y");
  
  fpHisTrackClus->Draw("text");
}



