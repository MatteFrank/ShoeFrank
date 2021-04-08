// Macro to read back MC info from reconstructed points of TW
// Ch. Finck, Jan. Position difference


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

#include "TATWntuPoint.hxx"
#endif

// main
void PlotTwPos(TString nameFile = "12C_C_200_L0Out.root", TString expName = "12C_200", Int_t runNumber = 1)
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
  
  // geo file for TW
  TAGparaDsc* parGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
  TATWparGeo* pGeoMap = (TATWparGeo*)parGeoTw->Object();
  TString parFileName = campManager->GetCurGeoFile(TATWparGeo::GetBaseName(), runNumber);
  pGeoMap->FromFile(parFileName);
  
  // Branch setting
  TATWntuPoint *twPoint = new TATWntuPoint();
  TAGdataDsc* twPo    = new TAGdataDsc("twPoint", twPoint);
  TAGactTreeReader* vtActReader  = new TAGactTreeReader("vtActEvtReader");
  vtActReader->SetupBranch(twPo, TATWntuPoint::GetBranchName());
  
  vtActReader->Open(nameFile);
  tagr.AddRequiredItem(vtActReader);
  
  // histogram
  TH2F* fpHisTrackClus = new TH2F("twPoint", "Position Y vs X", 40, -20, 20, 40, -20, 20);
  
  gStyle->SetOptStat(11);

  tagr.BeginEventLoop();

  Int_t ev = 0;

   while (tagr.NextEvent() ){

    Int_t npoints = twPoint->GetPointN();
    for(Int_t i = 0; i < npoints; ++i) {
      TATWpoint* point = twPoint->GetPoint(i);
      if (!point) continue;
      TVector3 pos = point->GetPosition();

      fpHisTrackClus->Fill(pos[0], pos[1]);
      ev++;
    }
  }
  
  
  tagr.EndEventLoop();
  
  fpHisTrackClus->SetTitle("");
  fpHisTrackClus->SetXTitle("Position X");
  fpHisTrackClus->SetTitleOffset(0.9,"X");
  fpHisTrackClus->SetTitleSize(0.05,"X");
  fpHisTrackClus->SetYTitle("Position Y");
  fpHisTrackClus->SetTitleOffset(0.9,"Y");
  fpHisTrackClus->SetTitleSize(0.05,"Y");
  fpHisTrackClus->SetLabelSize(0.04,"X");
  fpHisTrackClus->SetLabelSize(0.04,"Y");
  fpHisTrackClus->SetLabelOffset(0.002,"X");
  fpHisTrackClus->SetLabelOffset(0.005,"Y");
  
  fpHisTrackClus->Draw("colz");
}



